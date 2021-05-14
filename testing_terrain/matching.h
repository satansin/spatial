#ifndef __MATCHING_H_
#define __MATCHING_H_

#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "KDtree.h"
#include "lineqn.h"

#include "mathutil.h"
#include "RTree.h"

#include <Eigen/Dense>

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <cassert>
#include <cstdlib>

using namespace std;
using namespace trimesh;

// #define CONV_THRESH 1e-4 // square rooted
// #define CONV_THRESH 1e-6
#define CONV_THRESH 1e-8f
#define ABS_ERR_THRESH 1e-6f
#define MAX_ITR 100
#define LAMBDA_INIT 1.0f // For LM algorithm

#define RST_SCALING 1e8f
typedef RTree<int, int, 3, double, 6> RStree;

RStree* build_vertex_rst(const TriMesh* mesh)
{
	RStree* rst = new RStree;
	int ver[3];
	for (int i = 0; i < mesh->vertices.size(); i++) { // vertex id
		for (int j = 0; j < 3; j++) { // dimension
			ver[j] = int(mesh->vertices[i][j] * RST_SCALING);
		}
		rst->InsertVertex(ver, i);
	}
	return rst;
}

RStree* build_face_rst(const TriMesh* mesh)
{
	RStree* rst = new RStree;
	int triver[3][3];
	for (int i = 0; i < mesh->faces.size(); i++) { // face id
		for (int j = 0; j < 3; j++) { // vertex id in face
			for (int k = 0; k < 3; k++) { // dimension
				triver[j][k] = int(mesh->vertices[mesh->faces[i][j]][k] * RST_SCALING);
			}
		}
		rst->InsertFace(triver, i);
	}
	return rst;
}

void write_mesh(const TriMesh* mesh, string filename, const xform& xf)
{
	TriMesh* mesh_tmp = new TriMesh;
	for (auto &v: mesh->vertices) {
		mesh_tmp->vertices.push_back(v);
	}
	for (auto &f: mesh->faces) {
		mesh_tmp->faces.push_back({f[0], f[1], f[2]});
	}
	apply_xform(mesh_tmp, xf);
	mesh_tmp->write(filename);
}

double cal_err(const TriMesh* mesh_data, const xform& xf)
{
	double sum_pt_dist = 0.0;
	for (auto &v: mesh_data->vertices) {
		sum_pt_dist += sq_dist(v, xf * v);
	}
	return sqrt(sum_pt_dist / (double) mesh_data->vertices.size());
}

// Converts pairs of random numbers in [0,1] to
// uniformly-distributed points on the sphere
template <class T = float>
static inline Vec<3,T> rnd2sph(const T &u, const T &v)
{
	T phi = u * T(M_2PI);
	T z = v * 2 - 1;
	T xy = sqrt(T(1) - sqr(z));
	return Vec<3,T>(xy * cos(phi), xy * sin(phi), z);
}

vec tri_normal(TriMesh* mesh, int tri_idx)
{
	// the direct result
	return mesh->trinorm(tri_idx);

	// set<int> concerned_tri_idxes;
	// concerned_tri_idxes.insert(tri_idx);
	// for (auto &pt_idx: mesh->faces[tri_idx]) {
	// 	for (auto &adj_f: mesh->adjacentfaces[pt_idx]) {
	// 		concerned_tri_idxes.insert(adj_f);
	// 	}
	// }
	// vec sum_normal;
	// for (auto &f: concerned_tri_idxes) {
	// 	sum_normal += mesh->trinorm(f);
	// }
	// normalize(sum_normal);
	// return sum_normal;
}


struct ICP_Stat
{
	double time; // running time
	vector<double> per_itr_err;
	vector<xform> per_itr_xf;
	double best_err = INFTY;
	int best_itr = -1;
	void add_itr(double err, xform xf)
	{
		per_itr_err.push_back(err);
		per_itr_xf.push_back(xf);
		if (err < best_err) {
			best_err = err;
			best_itr = per_itr_err.size() - 1;
		}
	}
};


class ICP_Matching
{
public:
	struct Corr_Element
	{
		int v_id;
		point v;
		int f_id;
		point fv[3];
		point n;
		Corr_Element() {
			v_id = -1;
			f_id = -1;
		}
	};

	struct Corr_Element_4D
	{
		point4 v;
	};

	enum Var_Corr { VC_V2V, VC_V2NN, VC_TC2TC, VC_TC2NN, VC_NN2NN, VC_V2V_4D = 99 };
	enum Var_Align { VA_P2P, VA_P2L, VA_SYMM, VA_VOL, VA_P2L_LM, VA_P2P_4D = 99 };

	string path_model;
	string path_data;

	ICP_Matching(): path_model(""), path_data("") {}
	ICP_Matching(string p_model, string p_data): path_model(p_model), path_data(p_data) {}

	TriMesh* mesh_model;
	TriMesh* mesh_data;

	int num_v_model;
	int num_v_data;
	int num_f_model;
	int num_f_data;

	KDtree* kd_model;

	RStree* rsv_model;
	RStree* rsf_model;

	vector<point> tcs_model;
	KDtree* kd_tc_model;

	// // TODO: use interpolation method to find the closest triangle
	// vector<point> interpols_model;
	// vector<point> kd_interpol_model;

	vector<point> tcs_data;

	vector<vec> tnorms_model;
	vector<vec> tnorms_data;

	void load()
	{
		mesh_model = TriMesh::read(path_model);
		mesh_data = TriMesh::read(path_data);

		// Put mesh_data at origin, with unit variance
		point com = point_center_of_mass(mesh_data->vertices);
		trans(mesh_model, -com);
		trans(mesh_data, -com);

		float C[3][3];
		point_covariance(mesh_data->vertices, C);
		float s = 1.0f / sqrt(C[0][0] + C[1][1] + C[2][2]);

		scale(mesh_model, s);
		scale(mesh_data, s);

		mesh_model->need_bsphere();
		mesh_data->need_bsphere();

		mesh_model->need_faces();
		mesh_model->need_normals();
		mesh_model->need_neighbors();
		mesh_model->need_adjacentfaces();

		mesh_data->need_faces();
		mesh_data->need_normals();
		mesh_data->need_neighbors();
		mesh_data->need_adjacentfaces();

		num_v_model = mesh_model->vertices.size();
		num_v_data = mesh_data->vertices.size();

		num_f_model = mesh_model->faces.size();
		num_f_data = mesh_data->faces.size();

		kd_model = new KDtree(mesh_model->vertices);

		rsv_model = build_vertex_rst(mesh_model);
		rsf_model = build_face_rst(mesh_model);

		tcs_model.clear();
		tcs_model.reserve(num_f_model);
		for (int i = 0; i < num_f_model; i++) {
			tcs_model.push_back(mesh_model->centroid(i));
		}

		kd_tc_model = new KDtree(tcs_model);

		tcs_data.clear();
		tcs_data.reserve(num_f_data);
		for (int i = 0; i < num_f_data; i++) {
			tcs_data.push_back(mesh_data->centroid(i));
		}

		tnorms_model.clear();
		tnorms_model.reserve(num_f_model);
		for (int i = 0; i < num_f_model; i++) {
			tnorms_model.push_back(tri_normal(mesh_model, i));
		}

		tnorms_data.clear();
		tnorms_data.reserve(num_f_data);
		for (int i = 0; i < num_f_data; i++) {
			tnorms_data.push_back(tri_normal(mesh_data, i));
		}
	}

private:
	// Find the nearest vertex in {mesh_model} to {q}
	int nn_vertex_model(const point& q)
	{
		// if (rsv_model) {
		// 	const int q_int[3] { int(q[0] * RST_SCALING), int(q[1] * RST_SCALING), int(q[2] * RST_SCALING) };
		// 	const int nn_vid = rsv_model->nnVertexSearch(q_int);
		// 	if (nn_vid >= 0) {
		// 		return nn_vid;
		// 	}
		// }
		if (kd_model) {
			auto nn = kd_model->closest_to_pt(q);
			if (nn) {
				return (const point *) nn - &(mesh_model->vertices[0]);
			}
		}

		// // if kd_model not provided or kd_model search failed
		// int min_i = -1;
		// double min_sq_dist = INFTY;
		// for (int i = 0; i < num_v_model; i++) {
		// 	double new_sq_dist = sq_dist(q, mesh_model->vertices[i]);
		// 	if (min_sq_dist > new_sq_dist) {
		// 		min_sq_dist = new_sq_dist;
		// 		min_i = i;
		// 		// cout << min_sq_dist << " " << min_i << endl;
		// 	}
		// }
		// return min_i;

		return -1;
	}

	int nn_face_by_tc_model(const point &q)
	{
		if (kd_tc_model) {
			auto nn = kd_tc_model->closest_to_pt(q);
			if (nn) {
				return ((const point *) nn - &(tcs_model[0]));
			}
		}

		return -1;
	}

	int nn_face_by_nn_model_bf(const point &q, point &nn_ret)
	{
		// brute-force method
		double min_sq_dist = INFTY;
		int nn_f_i;
		point fv[3];
		for (int i = 0; i < num_f_model; i++) {
			// cout << "For the concerned face #" << i << endl;
			for (int j = 0; j < 3; j++) {
				fv[j] = mesh_model->vertices[mesh_model->faces[i][j]];
			}
			const vec &tn = tnorms_model[i];
			point tmp_res;
			double new_sq_dist = sq_dist_p2f(q, fv, tn, tmp_res);

			if (min_sq_dist > new_sq_dist) {
				min_sq_dist = new_sq_dist;
				nn_f_i = i;
				nn_ret = tmp_res;
			}
		}
		return nn_f_i;
	}

	int nn_face_by_nn_model(const point &q, point &nn_ret)
	{
		// // Method: using R-tree, not finished, do not activate it
		// if (rsf_model) {
		// 	const int center[3] = { int(q[0] * RST_SCALING), int(q[1] * RST_SCALING), int(q[2] * RST_SCALING) };
		// 	vector<int> ret_arr;
		// 	knnVertexSearch(center, 1, ret_arr, SqDistReal());
		// }

		// // A very approximate but fast approach
		// auto nn = kd_tc_model->closest_to_pt(q);
		// if (!nn) {
		// 	return -1;
		// }
		// int match_ind = (const point *) nn - &(tcs_model[0]);
		// assert(match_ind >= 0 && match_ind < num_f_model);
		// point fv[3];
		// for (int j = 0; j < 3; j++) {
		// 	fv[j] = mesh_model->vertices[mesh_model->faces[match_ind][j]];
		// }
		// sq_dist_p2f(q, fv, nn_ret);
		// return match_ind;

		// The above method but with slightly more nn
		const int k = 2;
		vector<const float *> knn;
		kd_tc_model->find_k_closest_to_pt(knn, k, q);
		double min_sq_dist = INFTY;
		int nn_f_i;
		point fv[3];
		for (int i = 0; i < k; i++) {
			int match_ind = (const point *) knn[i] - &(tcs_model[0]);
			for (int j = 0; j < 3; j++) {
				fv[j] = mesh_model->vertices[mesh_model->faces[match_ind][j]];
			}
			const vec &tn = tnorms_model[match_ind];
			point tmp_res;
			double new_sq_dist = sq_dist_p2f(q, fv, tn, tmp_res);
			if (min_sq_dist > new_sq_dist) {
				min_sq_dist = new_sq_dist;
				nn_f_i = match_ind;
				nn_ret = tmp_res;
			}
		}
		return nn_f_i;

		// // An approximate but fast approach
		// auto nn = kd_model->closest_to_pt(q);
		// if (!nn) {
		// 	return -1;
		// }
		// int match_ind = (const point *) nn - &(mesh_model->vertices[0]);
		// assert(match_ind >= 0 && match_ind < num_v_model);
		// // in case the resultant adjacent faces list is empty, find the next NN instead
		// int k = 2;
		// while (mesh_model->adjacentfaces[match_ind].empty()) {
		// 	// cout << "k-NN loop for k = " << k << endl;
		// 	vector<const float *> knn;
		// 	kd_model->find_k_closest_to_pt(knn, k, q);
		// 	match_ind = (const point *) knn[k - 1] - &(mesh_model->vertices[0]);
		// 	k++;
		// }
		// // cout << "Matched index: " << match_ind << endl;
		// // First, we include the several faces ``near'' NN into ``concerned set''
		// set<int> concerned_faces_id;
		// for (auto &f_id: mesh_model->adjacentfaces[match_ind]) {
		// 	concerned_faces_id.insert(f_id);
		// }
		// // // enable below for more accurate result
		// // for (auto &nb_v_id: mesh_model->neighbors[match_ind]) {
		// // 	for (auto &f_id: mesh_model->adjacentfaces[nb_v_id]) {
		// // 		concerned_faces_id.insert(f_id);
		// // 	}
		// // }
		// // cout << concerned_faces_id.size() << endl;
		// // Next, we search for NN point and face among the concerned faces
		// double min_sq_dist = INFTY;
		// int nn_f_i;
		// point fv[3];
		// for (auto &i: concerned_faces_id) {
		// 	// cout << "For the concerned face #" << i << endl;
		// 	for (int j = 0; j < 3; j++) {
		// 		fv[j] = mesh_model->vertices[mesh_model->faces[i][j]];
		// 	}
		// 	point tmp_res;
		// 	double new_sq_dist = sq_dist_p2f(q, fv, tmp_res);
		// 	if (min_sq_dist > new_sq_dist) {
		// 		min_sq_dist = new_sq_dist;
		// 		nn_f_i = i;
		// 		nn_ret = tmp_res;
		// 	}
		// }
		// return nn_f_i;
	}

	int nn_face_by_tnn_model_bf(const point (&fv)[3], const vec &tn, point &nn1_ret, point &nn2_ret, double &sq_dist_ret)
	{
		double min_sq_dist = INFTY;
		point min_nn1, min_nn2;
		int min_face_i = -1;
		point fv_model[3];
		for (int i = 0; i < num_f_model; i++) {
			// cout << "For the concerned face #" << i << endl;
			for (int j = 0; j < 3; j++) {
				fv_model[j] = mesh_model->vertices[mesh_model->faces[i][j]];
			}
			const vec &tn_model = tnorms_model[i];
			point tmp_nn1, tmp_nn2;
			// double time_consumption[3] = { 0.0, 0.0, 0.0 };
			// double tmp_sq_dist = nn_triangle(fv, fv_model, tn, tn_model, tmp_nn1, tmp_nn2, time_consumption);
			double tmp_sq_dist = nn_triangle(fv, fv_model, tn, tn_model, tmp_nn1, tmp_nn2);
			if (min_sq_dist > tmp_sq_dist) {
				min_sq_dist = tmp_sq_dist;
				min_nn1 = tmp_nn1;
				min_nn2 = tmp_nn2;
				min_face_i = i;
			}

			if (min_sq_dist <= 0.0) {
				break;
			}
		}

		nn1_ret = min_nn1;
		nn2_ret = min_nn2;
		sq_dist_ret = min_sq_dist;

		return min_face_i;
	}

	// int nn_face_by_tnn_model(const point (&fv)[3], const vec &tn, point &nn1_ret, point &nn2_ret, double &sq_dist_ret, double time_consumption[3])
	int nn_face_by_tnn_model(const point (&fv)[3], const vec &tn, point &nn1_ret, point &nn2_ret, double &sq_dist_ret)
	{
		// // First method that considers around 10 concerned faces
		// set<int> concerned_faces_id;
		// for (int i = 0; i < 3; i++) {
		// 	point q = fv[i];
		// 	auto nn = kd_model->closest_to_pt(q);
		// 	if (!nn) {
		// 		continue;
		// 	}
		// 	int match_ind = (const point *) nn - &(mesh_model->vertices[0]);
		// 	assert(match_ind >= 0 && match_ind < num_v_model);
		// 	int k = 2;
		// 	while (mesh_model->adjacentfaces[match_ind].empty()) {
		// 		// cout << "k-NN loop for k = " << k << endl;
		// 		vector<const float *> knn;
		// 		kd_model->find_k_closest_to_pt(knn, k, q);
		// 		match_ind = (const point *) knn[k - 1] - &(mesh_model->vertices[0]);
		// 		k++;
		// 	}
		// 	for (auto &f_id: mesh_model->adjacentfaces[match_ind]) {
		// 		concerned_faces_id.insert(f_id);
		// 	}
		// 	// // enable below for more accurate result
		// 	// for (auto &nb_v_id: mesh_model->neighbors[match_ind]) {
		// 	// 	for (auto &f_id: mesh_model->adjacentfaces[nb_v_id]) {
		// 	// 		concerned_faces_id.insert(f_id);
		// 	// 	}
		// 	// }
		// }

		// // Second method that considers one face, it has a great error from the brute-force
		// set<int> concerned_faces_id;
		// point centroid = (fv[0] + fv[1] + fv[2]) / 3.0;
		// auto nn = kd_tc_model->closest_to_pt(centroid);
		// if (!nn) {
		// 	return -1;
		// }
		// int match_ind = (const point *) nn - &(tcs_model[0]);
		// assert(match_ind >= 0 && match_ind < num_f_model);
		// concerned_faces_id.insert(match_ind);

		// Third method, the above that considers more faces
		set<int> concerned_faces_id;
		point centroid = (fv[0] + fv[1] + fv[2]) / 3.0;
		const int k = 2;
		vector<const float *> knn;
		kd_tc_model->find_k_closest_to_pt(knn, k, centroid);
		for (int i = 0; i < k; i++) {
			int match_ind = (const point *) knn[i] - &(tcs_model[0]);
			assert(match_ind >= 0 && match_ind < num_f_model);
			concerned_faces_id.insert(match_ind);
		}

		// cout << "Num concerned faces: " << concerned_faces_id.size() << endl;

		// loop through the concerned faces to find the nearest face
		double min_sq_dist = INFTY;
		point min_nn1, min_nn2;
		int min_face_i = -1;
		point fv_model[3];
		for (auto &i: concerned_faces_id) {
			// cout << "For the concerned face #" << i << endl;
			for (int j = 0; j < 3; j++) {
				fv_model[j] = mesh_model->vertices[mesh_model->faces[i][j]];
			}
			const vec &tn_model = tnorms_model[i];
			point tmp_nn1, tmp_nn2;
			// double tmp_sq_dist = nn_triangle(fv, fv_model, tn, tn_model, tmp_nn1, tmp_nn2, time_consumption);
			double tmp_sq_dist = nn_triangle(fv, fv_model, tn, tn_model, tmp_nn1, tmp_nn2);
			if (min_sq_dist > tmp_sq_dist) {
				min_sq_dist = tmp_sq_dist;
				min_nn1 = tmp_nn1;
				min_nn2 = tmp_nn2;
				min_face_i = i;
			}
			if (min_sq_dist <= 0.0) {
				break;
			}
		}
		nn1_ret = min_nn1;
		nn2_ret = min_nn2;
		sq_dist_ret = min_sq_dist;
		return min_face_i;
	}

	double corr_dist(const vector<Corr_Element>& elements_data, const vector<Corr_Element>& elements_model, int corr_size, int va)
	{
		double sum = 0.0;
		for (int i = 0; i < corr_size; i++) {
			switch (va) {
			case VA_P2P:
				sum += sq_dist(elements_data[i].v, elements_model[i].v);
				break;
			case VA_P2L:
			case VA_P2L_LM:
				sum += sq((elements_data[i].v - elements_model[i].v) DOT elements_model[i].n);
				break;
			case VA_SYMM:
				sum += sq((elements_data[i].v - elements_model[i].v) DOT (elements_data[i].n + elements_model[i].n));
				break;
			case VA_VOL:
				sum += proj_volume(elements_data[i].fv, elements_model[i].fv);
				break;
			}
		}

		return sqrt(sum / (double) corr_size);
	}

	int find_corr_v2v(const xform& xf_data, vector<Corr_Element>& elements_data, vector<Corr_Element>& elements_model)
	{
		auto nxf_data = norm_xf(xf_data);
		int count = 0;
		for (int i = 0; i < num_v_data; i++) {
			Corr_Element el_data;
			el_data.v_id = i;
			el_data.v = xf_data * mesh_data->vertices[i];
			el_data.n = nxf_data * mesh_data->normals[i];

			Corr_Element el_model;
			el_model.v_id = nn_vertex_model(el_data.v);
			if (el_model.v_id < 0) {
				continue;
			}
			el_model.v = mesh_model->vertices[el_model.v_id];
			el_model.n = mesh_model->normals[el_model.v_id];

			elements_data.push_back(el_data);
			elements_model.push_back(el_model);
			count++;

			// cout << "Corr added: data(#" << el_data.v_id << ") model(#" << el_model.v_id << ")" << endl;
		}

		return count;
	}

	int find_corr_v2nn(const xform& xf_data, vector<Corr_Element>& elements_data, vector<Corr_Element>& elements_model)
	{
		auto nxf_data = norm_xf(xf_data);
		int count = 0;
		int count_inconsistency = 0;
		for (int i = 0; i < num_v_data; i++) {
			Corr_Element el_data;
			el_data.v_id = i;
			el_data.v = xf_data * mesh_data->vertices[i];
			el_data.n = nxf_data * mesh_data->normals[i];

			Corr_Element el_model; // pay attention: we do not need el_model.v_id

			// const int nn_face_id_bf = nn_face_by_nn_model_bf(el_data.v, el_model.v);
			// el_model.n = tnorms_model[nn_face_id_bf];

			const int nn_face_id = nn_face_by_nn_model(el_data.v, el_model.v);
			// if (nn_face_id_bf != nn_face_id) {
			// 	// cout << "approx(" << nn_face_id << ") brute-force(" << nn_face_id_bf << ")" << endl;
			// 	count_inconsistency++;
			// }
			if (nn_face_id < 0) {
				continue;
			}
			el_model.n = tnorms_model[nn_face_id];
			// for (int j = 0; j < 3; j++) {
			// 	el_model.fv[j] = mesh_model->vertices[mesh_model->faces[nn_face_id][j]];
			// }

			elements_data.push_back(el_data);
			elements_model.push_back(el_model);
			count++;

			// cout << "Corr added: data(#" << el_data.v_id << ") model(#" << el_model.v_id << ")" << endl;
		}
		// cout << "Inconsistency(" << count_inconsistency << ") ";

		return count;
	}

	int find_corr_tc2tc(const xform& xf_data, vector<Corr_Element>& elements_data, vector<Corr_Element>& elements_model)
	{
		auto nxf_data = norm_xf(xf_data);
		int count = 0;
		for (int i = 0; i < num_f_data; i++) {
			Corr_Element el_data;
			el_data.v = xf_data * tcs_data[i];
			el_data.n = nxf_data * tnorms_data[i];
			for (int j = 0; j < 3; j++) {
				el_data.fv[j] = xf_data * mesh_data->vertices[mesh_data->faces[i][j]];
			}

			Corr_Element el_model;
			int nn_face_id = nn_face_by_tc_model(el_data.v);
			if (nn_face_id < 0) {
				continue;
			}
			el_model.v = tcs_model[nn_face_id];
			el_model.n = tnorms_model[nn_face_id];
			for (int j = 0; j < 3; j++) {
				el_model.fv[j] = mesh_model->vertices[mesh_model->faces[nn_face_id][j]];
			}

			elements_data.push_back(el_data);
			elements_model.push_back(el_model);
			count++;

			// cout << "Corr added: data(#" << el_data.v_id << ") model(#" << el_model.v_id << ")" << endl;
		}

		return count;
	}

	int find_corr_tc2nn(const xform& xf_data, vector<Corr_Element>& elements_data, vector<Corr_Element>& elements_model)
	{
		auto nxf_data = norm_xf(xf_data);
		int count = 0;
		for (int i = 0; i < num_f_data; i++) {
			Corr_Element el_data;
			el_data.v = xf_data * tcs_data[i];
			el_data.n = nxf_data * tnorms_data[i];
			for (int j = 0; j < 3; j++) {
				el_data.fv[j] = xf_data * mesh_data->vertices[mesh_data->faces[i][j]];
			}

			Corr_Element el_model;
			int nn_face_id = nn_face_by_nn_model(el_data.v, el_model.v);
			if (nn_face_id < 0) {
				continue;
			}
			el_model.n = tnorms_model[nn_face_id];
			for (int j = 0; j < 3; j++) {
				el_model.fv[j] = mesh_model->vertices[mesh_model->faces[nn_face_id][j]];
			}

			elements_data.push_back(el_data);
			elements_model.push_back(el_model);
			count++;

			// cout << "Corr added: data(#" << el_data.v_id << ") model(#" << el_model.v_id << ")" << endl;
		}

		return count;
	}

	// int find_corr_nn2nn(const xform& xf_data, vector<Corr_Element>& elements_data, vector<Corr_Element>& elements_model, double time_consumption[3])
	int find_corr_nn2nn(const xform& xf_data, vector<Corr_Element>& elements_data, vector<Corr_Element>& elements_model)
	{
		auto nxf_data = norm_xf(xf_data);
		int count = 0;
		// int count_inconsistency = 0;
		for (int i = 0; i < num_f_data; i++) {
			Corr_Element el_data;
			el_data.n = nxf_data * tnorms_data[i];
			for (int j = 0; j < 3; j++) {
				el_data.fv[j] = xf_data * mesh_data->vertices[mesh_data->faces[i][j]];
			}

			Corr_Element el_model;

			double sq_dist;

			// int nn_face_id_bf = nn_face_by_tnn_model_bf(el_data.fv, el_data.n, el_data.v, flag_data, el_model.v, el_model.v_id, sq_dist);
			// if (nn_face_id_bf < 0 || sq_dist <= 0.0) {
			// 	continue;
			// }
			// el_model.n = tnorms_model[nn_face_id_bf];

			// int nn_face_id = nn_face_by_tnn_model(el_data.fv, el_data.n, el_data.v, el_model.v, sq_dist, time_consumption);
			int nn_face_id = nn_face_by_tnn_model(el_data.fv, el_data.n, el_data.v, el_model.v, sq_dist);

			// if (nn_face_id_bf != nn_face_id) {
			// 	// cout << "approx(" << nn_face_id << ") brute-force(" << nn_face_id_bf << ")" << endl;
			// 	count_inconsistency++;
			// }

			if (nn_face_id < 0 || sq_dist <= 0.0) {
				continue;
			}

			// // the coin check will remove around 20% of total correspondences, however, it does not help to increase accuracy
			// bool coin = false;
			// for (int j = 0; j < elements_data.size(); j++) {
			// 	if (approx_eqt(el_data.v, elements_data[j].v) && approx_eqt(el_model.v, elements_model[j].v)) {
			// 		// cout << "coin to (" << j << ") for face id: " << i << endl;
			// 		coin = true;
			// 		break;
			// 	}
			// }
			// if (coin) {
			// 	continue;
			// }

			el_model.n = tnorms_model[nn_face_id];

			// cout << i << ": " << sq_dist << endl;
			elements_data.push_back(el_data);
			elements_model.push_back(el_model);
			count++;

			// cout << "Corr added: data(#" << el_data.v_id << ") model(#" << el_model.v_id << ")" << endl;
		}
		// cout << "Inconsistency(" << count_inconsistency << ") ";

		return count;
	}

	int find_corr_v2v_4d(const xform& xf_data, vector<Corr_Element_4D>& elements_data, vector<Corr_Element_4D>& elements_model)
	{
		auto nxf_data = norm_xf(xf_data);
		int count = 0;
		for (int i = 0; i < num_v_data; i++) {
			auto v_data = xf_data * mesh_data->vertices[i];
			auto n_data = nxf_data * mesh_data->normals[i];

			int v_id_model = nn_vertex_model(v_data);
			if (v_id_model < 0) {
				continue;
			}
			auto v_model = mesh_model->vertices[v_id_model];
			auto n_model = mesh_model->normals[v_id_model];

			Corr_Element_4D el_data;
			Corr_Element_4D el_model;
			for (int i = 0; i < 3; i++) {
				el_data.v[i] = v_data[i];
				el_model.v[i] = v_model[i];
			}

			double theta = acos(n_data DOT n_model);
			el_data.v[3] = sqrt(sq_dist(v_data, v_model)) * tan(theta / 3.0);

			el_model.v[3] = 0.0;

			// cout << "Corr added: data(#" << el_data.v_id << ") model(#" << el_model.v_id << ")" << endl;
		}

		return count;
	}

	int find_corr_gt(const xform& xf_data, vector<Corr_Element>& elements_data, vector<Corr_Element>& elements_model)
	{
		auto nxf_data = norm_xf(xf_data);
		int count = 0;
		for (int i = 0; i < num_v_data; i++) {
			Corr_Element el_data;
			el_data.v_id = i;
			el_data.v = xf_data * mesh_data->vertices[i];
			el_data.n = nxf_data * mesh_data->normals[i];

			Corr_Element el_model;
			// el_model.v_id = i;
			el_data.v = mesh_data->vertices[i];
			el_data.n = mesh_data->normals[i];
			// el_model.v = mesh_model->vertices[i];
			// el_model.n = mesh_model->normals[i];

			elements_data.push_back(el_data);
			elements_model.push_back(el_model);
			count++;

			// cout << "Corr added: data(#" << el_data.v_id << ") model(#" << el_model.v_id << ")" << endl;
		}

		return count;
	}

	// int find_corr(const xform& xf_data, vector<Corr_Element>& elements_data, vector<Corr_Element>& elements_model, int vc, double time_consumption[3])
	int find_corr(const xform& xf_data, vector<Corr_Element>& elements_data, vector<Corr_Element>& elements_model, int vc)
	{
		int count_corr;

		switch (vc) {
		case VC_V2V:
			count_corr = find_corr_v2v(xf_data, elements_data, elements_model);
			break;
		case VC_V2NN:
			count_corr = find_corr_v2nn(xf_data, elements_data, elements_model);
			break;
		case VC_TC2TC:
			count_corr = find_corr_tc2tc(xf_data, elements_data, elements_model);
			break;
		case VC_TC2NN:
			count_corr = find_corr_tc2nn(xf_data, elements_data, elements_model);
			break;
		case VC_NN2NN:
			// count_corr = find_corr_nn2nn(xf_data, elements_data, elements_model, time_consumption);
			count_corr = find_corr_nn2nn(xf_data, elements_data, elements_model);
			break;
		// case VC_V2V_4D:
		// 	count_corr = find_corr_v2v_4d(xf_data, elements_data_4d, elements_model_4d);
		// 	break;
		default:
			count_corr = find_corr_gt(xf_data, elements_data, elements_model);
			break;
		}

		return count_corr;
	}

	bool align_v2v(const vector<Corr_Element>& elements_data, const vector<Corr_Element>& elements_model, xform& xf_incremental)
	{
		int n = elements_data.size();

		// calculate centroids
		point centroid_data, centroid_model;
		for (int i = 0; i < n; i++) {
			centroid_data += elements_data[i].v;
			centroid_model += elements_model[i].v;
		}
		centroid_data /= (float) n;
		centroid_model /= (float) n;

		double U[3][3] = { { 0 } };

		for (int i = 0; i < n; i++) {
			dvec3 v_model = dvec3(elements_model[i].v - centroid_model);
			dvec3 v_data = dvec3(elements_data[i].v - centroid_data);
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++)
					U[j][k] += v_model[j] * v_data[k];
		}
		// print_matrix(U);

		double s[3], V[3][3];
		svd<double,3,3>(U, s, V);
		// cout << "s: " << s[0] << " " << s[1] << " " << s[2] << endl;
		for (int i = 0; i < 3; i++) {
			if (s[i] < 1e-6)
				return false;
		}

		if ((determ(U) < 0.0) ^ (determ(V) < 0.0)) {
			V[2][0] = -V[2][0];
			V[2][1] = -V[2][1];
			V[2][2] = -V[2][2];
		}
		xf_incremental = xform::trans(centroid_model) *
		          		 xform::fromarray(U) * transp(xform::fromarray(V)) *
		          		 xform::trans(-centroid_data);

		return true;
	}

	bool align_v2n(const vector<Corr_Element>& elements_data, const vector<Corr_Element>& elements_model, xform& xf_incremental)
	{
		int n = elements_data.size();

		// calculate centroids
		point centroid_data, centroid_model;
		for (int i = 0; i < n; i++) {
			centroid_data += elements_data[i].v;
			centroid_model += elements_model[i].v;
		}
		centroid_data /= (float) n;
		centroid_model /= (float) n;

		float scale = 0.0f;
		for (int i = 0; i < n; i++) {
			scale += sq_dist(elements_data[i].v, centroid_data);
			scale += sq_dist(elements_model[i].v, centroid_model);
		}
		scale = sqrt(scale / (2 * n));
		scale = 1.0f / scale;

		double A[6][6] = { { 0 } }, b[6] = { 0 };

		// cout << "scale: " << scale << endl;

		for (int i = 0; i < n; i++) {
			dvec3 p_data = dvec3(scale * (elements_data[i].v - centroid_data));
			dvec3 p_model = dvec3(scale * (elements_model[i].v - centroid_model));
			dvec3 n = dvec3(elements_model[i].n);
			dvec3 c = p_data CROSS n;
			dvec3 d = p_model - p_data;

			double x[6] = { c[0], c[1], c[2], n[0], n[1], n[2] };
			double dn = d DOT n;

			for (int j = 0; j < 6; j++) {
				b[j] += dn * x[j];
				for (int k = j; k < 6; k++)
					A[j][k] += x[j] * x[k];
			}
		}

		// Make matrix symmetric
		for (int j = 0; j < 6; j++)
			for (int k = 0; k < j; k++)
				A[j][k] = A[k][j];

		// Eigen-decomposition and inverse
		double eval[6], einv[6];
		eigdc<double,6>(A, eval);
		for (int i = 0; i < 6; i++) {
			// cout << "eval[" << i << "]: " << eval[i] << endl;
			if (eval[i] < 1e-6)
				return false;
			else
				einv[i] = 1.0 / eval[i];
		}

		// Solve system
		eigmult<double,6>(A, einv, b);

		// Extract rotation and translation
		dvec3 rot(b[0], b[1], b[2]), trans(b[3], b[4], b[5]);
		double rotangle = len(rot);
		trans *= 1.0 / scale;

		xform R = xform::rot(rotangle, rot);
		xf_incremental = xform::trans(centroid_model) *
		          		 xform::trans(trans) * R *
		          		 xform::trans(-centroid_data);

		return true;
	}

	bool align_v2n_lm(const vector<Corr_Element>& elements_data, const vector<Corr_Element>& elements_model, xform& xf_incremental, double lambda)
	{
		int n = elements_data.size();

		// calculate centroids
		point centroid_data, centroid_model;
		for (int i = 0; i < n; i++) {
			centroid_data += elements_data[i].v;
			centroid_model += elements_model[i].v;
		}
		centroid_data /= (float) n;
		centroid_model /= (float) n;

		float scale = 0.0f;
		for (int i = 0; i < n; i++) {
			scale += sq_dist(elements_data[i].v, centroid_data);
			scale += sq_dist(elements_model[i].v, centroid_model);
		}
		scale = sqrt(scale / (2 * n));
		scale = 1.0f / scale;

		double A[6][6] = { { 0 } }, b[6] = { 0 };

		// cout << "scale: " << scale << endl;

		for (int i = 0; i < n; i++) {
			dvec3 p_data = dvec3(scale * (elements_data[i].v - centroid_data));
			dvec3 p_model = dvec3(scale * (elements_model[i].v - centroid_model));
			dvec3 n = dvec3(elements_model[i].n);
			dvec3 c = p_data CROSS n;
			dvec3 d = p_model - p_data;

			double x[6] = { c[0], c[1], c[2], n[0], n[1], n[2] };
			double dn = d DOT n;

			for (int j = 0; j < 6; j++) {
				b[j] += dn * x[j];
				for (int k = j; k < 6; k++)
					A[j][k] += x[j] * x[k];
			}
		}

		// Make matrix symmetric
		for (int j = 0; j < 6; j++)
			for (int k = 0; k < j; k++)
				A[j][k] = A[k][j];

		// L-M regularization.  This is the "scale the diagonal" variant,
		// as opposed to the "add a multiple of the identity" variant.
		for (int j = 0; j < 6; j++)
			A[j][j] *= (1.0 + lambda);

		// Eigen-decomposition and inverse
		double eval[6], einv[6];
		eigdc<double,6>(A, eval);
		for (int i = 0; i < 6; i++) {
			if (eval[i] < 1e-6)
				return false;
			else
				einv[i] = 1.0 / eval[i];
		}

		// Solve system
		eigmult<double,6>(A, einv, b);

		// Extract rotation and translation
		dvec3 rot(b[0], b[1], b[2]), trans(b[3], b[4], b[5]);
		double rotangle = len(rot);
		trans *= 1.0 / scale;

		xform R = xform::rot(rotangle, rot);
		xf_incremental = xform::trans(centroid_model) *
		          		 xform::trans(trans) * R *
		          		 xform::trans(-centroid_data);

		double delta = 0.0;
		for (int i = 0; i < n; i++) {
			dvec3 v_data = dvec3(elements_data[i].v);
			dvec3 v_model = dvec3(elements_model[i].v);
			dvec3 n = dvec3(elements_model[i].n);
			delta += sq((v_model - v_data) DOT n);
			v_data = xf_incremental * v_data;
			delta -= sq((v_model - v_data) DOT n);
		}
		// cout << "delta: " << delta << endl;
		return (delta >= 0.0);
	}

	// Symmetric point-to-plane alignment.
	bool align_symm(const vector<Corr_Element>& elements_data, const vector<Corr_Element>& elements_model, xform& xf_incremental)
	{
		int n = elements_data.size();

		// calculate centroids
		point centroid_data, centroid_model;
		for (int i = 0; i < n; i++) {
			centroid_data += elements_data[i].v;
			centroid_model += elements_model[i].v;
		}
		centroid_data /= (float) n;
		centroid_model /= (float) n;

		float scale = 0.0f;
		for (int i = 0; i < n; i++) {
			scale += sq_dist(elements_data[i].v, centroid_data);
			scale += sq_dist(elements_model[i].v, centroid_model);
		}
		scale = sqrt(scale / (2 * n));
		scale = 1.0f / scale;

		double A[6][6] = { { 0 } }, b[6] = { 0 };

		for (int i = 0; i < n; i++) {
			dvec3 p_data = dvec3(scale * (elements_data[i].v - centroid_data));
			dvec3 p_model = dvec3(scale * (elements_model[i].v - centroid_model));
			dvec3 n = dvec3(elements_data[i].n + elements_model[i].n);
			dvec3 p = p_data + p_model;
			dvec3 c = p CROSS n;
			dvec3 d = p_model - p_data;

			double x[6] = { c[0], c[1], c[2], n[0], n[1], n[2] };
			double dn = d DOT n;

			for (int j = 0; j < 6; j++) {
				b[j] += dn * x[j];
				for (int k = j; k < 6; k++)
					A[j][k] += x[j] * x[k];
			}
		}

		// Make matrix symmetric
		for (int j = 1; j < 6; j++)
			for (int k = 0; k < j; k++)
				A[j][k] = A[k][j];

		// Eigen-decomposition and inverse
		double eval[6], einv[6];
		eigdc<double,6>(A, eval);
		for (int i = 0; i < 6; i++) {
			if (eval[i] < 1e-6)
				return false;
			else
				einv[i] = 1.0 / eval[i];
		}

		// Solve system
		eigmult<double,6>(A, einv, b);

		// Extract rotation and translation
		dvec3 rot(b[0], b[1], b[2]), trans(b[3], b[4], b[5]);
		double rotangle = atan(len(rot));
		trans *= 1.0 / scale;
		trans *= cos(rotangle);

		xform R = xform::rot(rotangle, rot);
		xf_incremental = xform::trans(centroid_model) *
		        	 R * xform::trans(trans) * R *
		        		 xform::trans(-centroid_data);

		return true;
	}

	bool align_v2v_4d(const vector<Corr_Element_4D>& elements_data_4d, const vector<Corr_Element_4D>& elements_model_4d, xform& xf_incremental)
	{
		int n = elements_data_4d.size();

		// calculate centroids
		point4 centroid_data, centroid_model;
		for (int i = 0; i < n; i++) {
			centroid_data += elements_data_4d[i].v;
			centroid_model += elements_model_4d[i].v;
		}
		centroid_data /= (float) n;
		centroid_model /= (float) n;

		double U[4][4] = { { 0 } };

		for (int i = 0; i < n; i++) {
			dvec4 v_model = dvec4(elements_model_4d[i].v - centroid_model);
			dvec4 v_data = dvec4(elements_data_4d[i].v - centroid_data);
			for (int j = 0; j < 4; j++)
				for (int k = 0; k < 4; k++)
					U[j][k] += v_model[j] * v_data[k];
		}
		// print_matrix(U);

		double s[4], V[4][4];
		svd<double,4,4>(U, s, V);
		// cout << "s: " << s[0] << " " << s[1] << " " << s[2] << " " << s[3] << endl;
		for (int i = 0; i < 4; i++) {
			if (s[i] < 1e-6)
				return false;
		}

		if ((determ(U) < 0.0) ^ (determ(V) < 0.0)) { // ???
			V[3][0] = -V[3][0];
			V[3][1] = -V[3][1];
			V[3][2] = -V[3][2];
			V[3][3] = -V[3][3];
		}

		Eigen::Matrix<double, 5, 5> ct_m = Eigen::Matrix<double, 5, 5>::Zero();
		Eigen::Matrix<double, 5, 5> ct_d_neg = Eigen::Matrix<double, 5, 5>::Zero();
		for (int i = 0; i < 4; i++) {
			ct_m(i, i) = 1.0;
			ct_m(i, 4) = centroid_model[i];
			ct_d_neg(i, i) = 1.0;
			ct_d_neg(i, 4) = -centroid_data[i];
		}
		ct_m(5, 5) = 1.0;
		ct_d_neg(5, 5) = 1.0;

		Eigen::Matrix<double, 5, 5> u = Eigen::Matrix<double, 5, 5>::Zero();
		Eigen::Matrix<double, 5, 5> v_transp = Eigen::Matrix<double, 5, 5>::Zero();
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				u(i, j) = U[i][j];
				v_transp(i, j) = V[j][i];
			}
		}

		auto ret = ct_m * u * v_transp * ct_d_neg;

		// xf_incremental = xform::trans(centroid_model) *
		//           		 xform::fromarray(U) * transp(xform::fromarray(V)) *
		//           		 xform::trans(-centroid_data);

		return true;
	}

public:
	// Registering mesh_data to mesh_model
	double icp(const xform& init_xf, int num_itr, int vc, int va, xform& icp_xf, ICP_Stat& stat, bool verbose = false)
	{
		icp_xf = init_xf;
		orthogonalize(icp_xf);

		double lambda = LAMBDA_INIT;
		double best_err = INFTY;
		int iter_of_best_err = 0;

		double time_consumption[3] = { 0.0, 0.0, 0.0 };

		for (int i = 0; i < num_itr; i++) {

			if (verbose) {
				cout << "Itr(" << (i + 1) << "): ";
			}

			vector<Corr_Element> elements_data;
			vector<Corr_Element> elements_model;

			// vector<Corr_Element_4D> elements_data_4d;
			// vector<Corr_Element_4D> elements_model_4d;

			// int count_corr = find_corr(icp_xf, elements_data, elements_model, vc, time_consumption);
			int count_corr = find_corr(icp_xf, elements_data, elements_model, vc);

			if (verbose) {
				cout << "Corr(" << count_corr << ") " << corr_dist(elements_data, elements_model, count_corr, va) << ", ";
			}

			xform xf_incremental;
			bool aligned = false;

			switch (va) {
			case VA_P2P:
				aligned = align_v2v(elements_data, elements_model, xf_incremental);
				break;
			case VA_P2L:
				aligned = align_v2n(elements_data, elements_model, xf_incremental);
				break;
			case VA_SYMM:
				aligned = align_symm(elements_data, elements_model, xf_incremental);
				break;
			case VA_P2L_LM:
				aligned = align_v2n_lm(elements_data, elements_model, xf_incremental, lambda);
				break;
			// case VA_P2P_4D:
			// 	aligned = align_v2v_4d(elements_data_4d, elements_model_4d, xf_incremental);
			// 	break;
			}

			if (aligned) {
				icp_xf = xf_incremental * icp_xf;
				orthogonalize(icp_xf);

				double updated_err = cal_err(mesh_data, icp_xf);

				if (verbose) {
					cout << "Upd-err " << updated_err << endl;
				}

				stat.add_itr(updated_err, icp_xf);

				// if (updated_err < best_err) {
				// 	if (best_err - updated_err <= CONV_THRESH || updated_err <= ABS_ERR_THRESH) {
				// 		cout << "Converge in " << (i + 1) << " iterations" << endl;
				// 		return updated_err;
				// 	} else {
				// 		best_err = updated_err;
				// 		iter_of_best_err = i + 1;
				// 	}
				// }
				// if (i - iter_of_best_err > 49) {
				// 	return best_err;
				// }

				lambda /= 2.0;

			} else {
				cout << "Itr " << (i + 1) << " fail aligning, continue." << endl;
				stat.add_itr(stat.per_itr_err.back(), stat.per_itr_xf.back());

				if (1) { // TODO: if for LM methods
					lambda *= 2.0;
				}
			}
		}

		cout << "Time consumption: " << time_consumption[0] << " " << time_consumption[1] << " " << time_consumption[2] << "(s)" << endl;
	}

	double get_corr_dist(const xform &xf, int vc, int va)
	{
		vector<Corr_Element> elements_data;
		vector<Corr_Element> elements_model;
		double time_consumption[3] = { 0.0, 0.0, 0.0 };
		// int count_corr = find_corr(xf, elements_data, elements_model, vc, time_consumption);
		int count_corr = find_corr(xf, elements_data, elements_model, vc);
		cout << "(" << count_corr << ")";
		return corr_dist(elements_data, elements_model, count_corr, va);
	}
};

#endif
