#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "KDtree.h"
#include "lineqn.h"

#include "mathutil.h"

#include <Eigen/Dense>

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <cassert>

using namespace std;
using namespace trimesh;

// #define CONV_THRESH 1e-4 // square rooted
// #define CONV_THRESH 1e-6
#define CONV_THRESH 1e-8
#define ABS_ERR_THRESH 1e-6
#define MAX_ITR 100
#define LAMBDA_INIT 1.0 // For LM algorithm

void write_mesh(const TriMesh* mesh, string filename, const xform& xf)
{
	TriMesh* mesh_tmp = new TriMesh;
	for (auto &v: mesh->vertices) {
		mesh_tmp->vertices.push_back(v);
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
	return sum_pt_dist / (double) mesh_data->vertices.size();
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
	// running time
	double time;
	vector<double> per_itr_err;
};


class ICP_Matching
{
public:
	struct Corr_Element
	{
		string m_type; // "v" or "f"
		int v_id;
		point v;
		int f_id;
		point n;
		Corr_Element(string type) {
			m_type = type;
			v_id = -1;
			f_id = -1;
		}
		string type() {
			return m_type;
		}
	};

	struct Corr_Element_4D
	{
		point4 v;
	};

	enum Var_Corr { VC_V2V, VC_V2NN, VC_TC2TC, VC_TC2NN, VC_NN2NN, VC_V2V_4D = 99 };
	enum Var_Align { VA_P2P, VA_P2L, VA_SYMM, VA_P2L_LM, VA_P2P_4D = 99 };

	string path_model;
	string path_data;

	Var_Corr variant_corr; 
	Var_Align variant_align; 

	ICP_Matching(): path_model(""), path_data(""), variant_corr(VC_V2V), variant_align(VA_P2P) {}
	ICP_Matching(string p_model, string p_data, int v_corr, int v_align): path_model(p_model), path_data(p_data), variant_corr((Var_Corr) v_corr), variant_align((Var_Align) v_align) {}

	TriMesh* mesh_model;
	TriMesh* mesh_data;

	int num_v_model;
	int num_v_data;
	int num_f_model;
	int num_f_data;

	KDtree* kd_model;

	vector<point> tcs_model;
	KDtree* kd_tc_model;

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

		// mesh_data->need_bbox();
		// cout << mesh_data->bbox.size() << endl;

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
	int nn_vertex_model(point q)
	{
		if (kd_model) {
			auto nn = kd_model->closest_to_pt(q);
			if (nn) {
				return (const point *) nn - &(mesh_model->vertices[0]);
			}
		}

		// if kd_model not provided or kd_model search failed
		int min_i = -1;
		double min_sq_dist = INFTY;
		for (int i = 0; i < num_v_model; i++) {
			double new_sq_dist = sq_dist(q, mesh_model->vertices[i]);
			if (min_sq_dist > new_sq_dist) {
				min_sq_dist = new_sq_dist;
				min_i = i;
				// cout << min_sq_dist << " " << min_i << endl;
			}
		}
		return min_i;
	}

	bool nn_point_model(point q, Corr_Element& ce_ret)
	{
		// An approximate but fast approach
		auto nn = kd_model->closest_to_pt(q);
		if (!nn) {
			return false;
		}
		int match_ind = (const point *) nn - &(mesh_model->vertices[0]);
		assert(match_ind >= 0 && match_ind < num_v_model);

		// in case the resultant adjacent faces list is empty, find the next NN instead
		int k = 2;
		while (mesh_model->adjacentfaces[match_ind].empty()) {
			// cout << "k-NN loop for k = " << k << endl;
			vector<const float *> knn;
			kd_model->find_k_closest_to_pt(knn, k, q);
			match_ind = (const point *) knn[k - 1] - &(mesh_model->vertices[0]);
			k++;
		}
		// cout << "Matched index: " << match_ind << endl;

		// First, we include the several faces ``near'' NN into ``concerned set''
		set<int> concerned_faces_id;
		for (auto &f_id: mesh_model->adjacentfaces[match_ind]) {
			concerned_faces_id.insert(f_id);
		}
		for (auto &nb_v_id: mesh_model->neighbors[match_ind]) {
			for (auto &f_id: mesh_model->adjacentfaces[nb_v_id]) {
				concerned_faces_id.insert(f_id);
			}
		}
		// cout << concerned_faces_id.size() << endl;

		// Next, we search for NN point and face among the concerned faces
		double min_sq_dist = INFTY;
		int nn_f_i;
		point fv[3];
		for (auto &i: concerned_faces_id) {
			// cout << "For the concerned face #" << i << endl;
			for (int j = 0; j < 3; j++) {
				fv[j] = mesh_model->vertices[mesh_model->faces[i][j]];
			}
			point tmp_res;
			int flag;
			double new_sq_dist = sq_dist_p2f(q, fv, tmp_res, flag);

			if (min_sq_dist > new_sq_dist) {
				min_sq_dist = new_sq_dist;
				ce_ret.v = tmp_res;
				nn_f_i = i;
				if (flag >= 0) {
					ce_ret.v_id = mesh_model->faces[i][flag];
				}
			}
		}
		ce_ret.n = tnorms_model[nn_f_i];
		return true;

		// For test only
		// auto tmp_v = ce_ret.v;

		// // An exact but very slow approach
		// double min_sq_dist_ex = INFTY;
		// point fv_ex[3];
		// for (int i = 0; i < mesh->faces.size(); i++) {
		// 	for (int j = 0; j < 3; j++) {
		// 		fv_ex[j] = mesh->vertices[mesh->faces[i][j]];
		// 	}
		// 	point tmp_res;
		// 	int flag;
		// 	double new_sq_dist = sq_dist_p2f(q, fv_ex, tmp_res, flag);

		// 	if (min_sq_dist_ex > new_sq_dist) {
		// 		min_sq_dist_ex = new_sq_dist;

		// 		ce_ret.v = tmp_res;
		// 		if (flag >= 0) {
		// 			ce_ret.v_id = mesh->faces[i][flag];
		// 		}
		// 	}
		// }
		// // return true;
		// if (tmp_v != ce_ret.v) {
		// 	cout << "NN-apprx: " << tmp_v << endl;
		// 	cout << "NN-exact: " << ce_ret.v << endl;
		// }
	}

	bool nn_centroid_model(point q, Corr_Element& ce_ret)
	{
		if (kd_tc_model) {
			auto nn = kd_tc_model->closest_to_pt(q);
			if (nn) {
				int match_ind = (const point *) nn - &(tcs_model[0]);
				ce_ret.v = tcs_model[match_ind];
				// ce_ret.n = tri_normal(mesh_model, match_ind);
				ce_ret.n = tnorms_model[match_ind];
				return true;
			}
		}

		double min_sq_dist = INFTY;
		for (int i = 0; i < num_f_model; i++) {
			double new_sq_dist = sq_dist(q, tcs_model[i]);
			if (min_sq_dist > new_sq_dist) {
				min_sq_dist = new_sq_dist;
				ce_ret.v = tcs_model[i];
				ce_ret.n = tnorms_model[i];
			}
		}
		return true;
	}

	// given a triangle, find its nearest triangle, and the distance is defined as the shortest distance between two triangles
	// if dist = 0, return false
	bool nn_triangle_model(const point (&fv)[3], point& nn1, Corr_Element& ce_ret)
	{
		set<int> concerned_faces_id;
		for (int i = 0; i < 3; i++) {
			point q = fv[i];

			auto nn = kd_model->closest_to_pt(q);
			if (!nn) {
				return false;
			}
			int match_ind = (const point *) nn - &(mesh_model->vertices[0]);
			assert(match_ind >= 0 && match_ind < num_v_model);

			int k = 2;
			while (mesh_model->adjacentfaces[match_ind].empty()) {
				// cout << "k-NN loop for k = " << k << endl;
				vector<const float *> knn;
				kd_model->find_k_closest_to_pt(knn, k, q);
				match_ind = (const point *) knn[k - 1] - &(mesh_model->vertices[0]);
				k++;
			}

			for (auto &f_id: mesh_model->adjacentfaces[match_ind]) {
				concerned_faces_id.insert(f_id);
			}
			for (auto &nb_v_id: mesh_model->neighbors[match_ind]) {
				for (auto &f_id: mesh_model->adjacentfaces[nb_v_id]) {
					concerned_faces_id.insert(f_id);
				}
			}
		}

		double min_sq_dist = INFTY;
		point min_nn1, min_nn2;
		int min_flag1, min_flag2;
		int min_face_i;
		point fv_model[3];
		for (auto &i: concerned_faces_id) {
			// cout << "For the concerned face #" << i << endl;
			for (int j = 0; j < 3; j++) {
				fv_model[j] = mesh_model->vertices[mesh_model->faces[i][j]];
			}
			point tmp_nn1, tmp_nn2;
			int tmp_flag1, tmp_flag2;
			double tmp_sq_dist = nn_triangle(fv, fv_model, tmp_nn1, tmp_nn2, tmp_flag1, tmp_flag2);
			if (min_sq_dist > tmp_sq_dist) {
				min_sq_dist = tmp_sq_dist;
				min_nn1 = tmp_nn1;
				min_nn2 = tmp_nn2;
				min_flag1 = tmp_flag1;
				min_flag2 = tmp_flag2;
				min_face_i = i;
			}
		}

		if (min_sq_dist == 0) {
			return false;
		}

		nn1 = min_nn1;
		ce_ret.v = min_nn2;
		ce_ret.n = tnorms_model[min_face_i];

		return true;
	}

	// TODO
	double cal_dist(const TriMesh* mesh_data, const TriMesh* mesh_model, const KDtree* kd_model)
	{
		// point-wise sq distance
		double sum_pt_dist = 0.0;
		int count = 0;

		if (!kd_model) {
			for (int i = 0; i < mesh_data->vertices.size(); i++) {
				double smallest = INFTY;
				for (int j = 0; j < mesh_model->vertices.size(); j++) {
					double new_dist = sq_dist(mesh_data->vertices[i], mesh_model->vertices[j]);
					if (new_dist < smallest) {
						smallest = new_dist;
					}
				}
				sum_pt_dist += smallest;
				count++;
			}
		} else {
			for (int i = 0; i < mesh_data->vertices.size(); i++) {
				auto nn = kd_model->closest_to_pt(mesh_data->vertices[i]);
				if (!nn) {
					continue;
				}

				sum_pt_dist += sq_dist(mesh_data->vertices[i], (*(point *) nn));
				count++;
			}
		}
		sum_pt_dist /= (double) count;
		// cout << "point-wise sq dist: " << sum_pt_dist << endl;

		// // centroid sq distance
		// double sum_centroid_dist = 0.0;
		// for (int i = 0; i < mesh_data->faces.size(); i++) {
		// 	// cout << mesh_data->centroid(i)[0] << " " << mesh_data->centroid(i)[1] << " " << mesh_data->centroid(i)[2] << endl;
		// 	// cout << mesh_model->centroid(i)[0] << " " << mesh_model->centroid(i)[1] << " " << mesh_model->centroid(i)[2] << endl;
		// 	double smallest = INFTY;
		// 	for (int j = 0; j < mesh_model->faces.size(); j++) {
		// 		double new_dist = sq_dist(mesh_data->centroid(i), mesh_model->centroid(j));
		// 		if (new_dist < smallest) {
		// 			smallest = new_dist;
		// 		}
		// 	}
		// 	sum_centroid_dist += smallest;
		// }
		// sum_centroid_dist /= (double) mesh_data->faces.size();
		// cout << "centroid sq dist: " << sum_centroid_dist << endl;

		// // centroid normal sq distance
		// double sum_centroid_normal_dist = 0.0;
		// for (int i = 0; i < mesh_data->faces.size(); i++) {
		// 	// cout << mesh_data->trinorm(i)[0] << " " << mesh_data->trinorm(i)[1] << " " << mesh_data->trinorm(i)[2] << endl;
		// 	// cout << sq_norm(mesh_data->trinorm(i)) << endl;
		// 	auto normalized_normal_noisy_plane = normalize(mesh_data->trinorm(i), 0.1);
		// 	auto normalized_normal_plane = normalize(mesh_model->trinorm(i), 0.1);
		// 	double smallest = INFTY;
		// 	for (int j = 0; j < mesh_model->faces.size(); j++) {
		// 		double new_dist = sq_dist(mesh_data->centroid(i) + normalized_normal_noisy_plane, mesh_model->centroid(j) + normalized_normal_plane);
		// 		if (new_dist < smallest) {
		// 			smallest = new_dist;
		// 		}
		// 	}
		// 	sum_centroid_normal_dist += smallest;
		// }
		// sum_centroid_normal_dist /= (double) mesh_data->faces.size();
		// cout << "centroid normal sq dist: " << sum_centroid_normal_dist << endl;

		return sum_pt_dist;
		// return sqrt(sum_pt_dist);
	}

	double corr_dist(const vector<Corr_Element>& elements_data, const vector<Corr_Element>& elements_model, string variant)
	{
		double sum = 0.0;
		for (int i = 0; i < elements_data.size(); i++) {
			if (variant == "v2v") {
				sum += sq_dist(elements_data[i].v, elements_model[i].v);
			} else if (variant == "v2n") {
				sum += sq((elements_data[i].v - elements_model[i].v) DOT elements_model[i].n);
			}
		}
		return (sum / (double) elements_data.size());
	}

	int find_corr_v2v(const xform& xf_data, vector<Corr_Element>& elements_data, vector<Corr_Element>& elements_model)
	{
		auto nxf_data = norm_xf(xf_data);
		int count = 0;
		for (int i = 0; i < num_v_data; i++) {
			Corr_Element el_data("v");
			el_data.v_id = i;
			el_data.v = xf_data * mesh_data->vertices[i];
			el_data.n = nxf_data * mesh_data->normals[i];

			Corr_Element el_model("v");
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
		for (int i = 0; i < num_v_data; i++) {
			Corr_Element el_data("v");
			el_data.v_id = i;
			el_data.v = xf_data * mesh_data->vertices[i];
			el_data.n = nxf_data * mesh_data->normals[i];

			Corr_Element el_model("v");
			if (!nn_point_model(el_data.v, el_model)) {
				continue;
			}

			elements_data.push_back(el_data);
			elements_model.push_back(el_model);
			count++;

			// cout << "Corr added: data(#" << el_data.v_id << ") model(#" << el_model.v_id << ")" << endl;
		}

		return count;
	}

	int find_corr_tc2tc(const xform& xf_data, vector<Corr_Element>& elements_data, vector<Corr_Element>& elements_model)
	{
		auto nxf_data = norm_xf(xf_data);
		int count = 0;
		for (int i = 0; i < num_f_data; i++) {
			Corr_Element el_data("v");
			el_data.v = xf_data * tcs_data[i];
			el_data.n = nxf_data * tnorms_data[i];

			Corr_Element el_model("v");
			if (!nn_centroid_model(el_data.v, el_model)) {
				continue;
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
			Corr_Element el_data("v");
			el_data.v = xf_data * tcs_data[i];
			el_data.n = nxf_data * tnorms_data[i];

			Corr_Element el_model("v");
			nn_point_model(el_data.v, el_model);

			elements_data.push_back(el_data);
			elements_model.push_back(el_model);
			count++;

			// cout << "Corr added: data(#" << el_data.v_id << ") model(#" << el_model.v_id << ")" << endl;
		}

		return count;
	}

	int find_corr_nn2nn(const xform& xf_data, vector<Corr_Element>& elements_data, vector<Corr_Element>& elements_model)
	{
		auto nxf_data = norm_xf(xf_data);
		int count = 0;
		for (int i = 0; i < num_f_data; i++) {
			Corr_Element el_data("v");
			el_data.n = nxf_data * tnorms_data[i];

			point fv_data[3];
			for (int j = 0; j < 3; j++) {
				fv_data[j] = xf_data * mesh_data->vertices[mesh_data->faces[i][j]];
			}

			Corr_Element el_model("v");
			if (!nn_triangle_model(fv_data, el_data.v, el_model)) {
				continue;
			}

			elements_data.push_back(el_data);
			elements_model.push_back(el_model);
			count++;

			// cout << "Corr added: data(#" << el_data.v_id << ") model(#" << el_model.v_id << ")" << endl;
		}

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
			Corr_Element el_data("v");
			el_data.v_id = i;
			el_data.v = xf_data * mesh_data->vertices[i];
			el_data.n = nxf_data * mesh_data->normals[i];

			Corr_Element el_model("v");
			el_model.v_id = i;
			el_model.v = mesh_model->vertices[i];
			el_model.n = mesh_model->normals[i];

			elements_data.push_back(el_data);
			elements_model.push_back(el_model);
			count++;

			// cout << "Corr added: data(#" << el_data.v_id << ") model(#" << el_model.v_id << ")" << endl;
		}

		return count;
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

		if ((det3(U) < 0.0) ^ (det3(V) < 0.0)) {
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

		if ((det4(U) < 0.0) ^ (det4(V) < 0.0)) { // ???
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
	double icp(const xform& init_xf, xform& icp_xf, ICP_Stat& stat)
	{
		icp_xf = init_xf;
		orthogonalize(icp_xf);

		double lambda = LAMBDA_INIT;
		double best_err = INFTY;
		int iter_of_best_err = 0;

		for (int i = 0; i < MAX_ITR; i++) {

			vector<Corr_Element> elements_data;
			vector<Corr_Element> elements_model;

			vector<Corr_Element_4D> elements_data_4d;
			vector<Corr_Element_4D> elements_model_4d;

			int count_corr;

			switch (variant_corr) {
			case VC_V2V:
				count_corr = find_corr_v2v(icp_xf, elements_data, elements_model);
				break;
			case VC_V2NN:
				count_corr = find_corr_v2nn(icp_xf, elements_data, elements_model);
				break;
			case VC_TC2TC:
				count_corr = find_corr_tc2tc(icp_xf, elements_data, elements_model);
				break;
			case VC_TC2NN:
				count_corr = find_corr_tc2nn(icp_xf, elements_data, elements_model);
				break;
			case VC_NN2NN:
				count_corr = find_corr_nn2nn(icp_xf, elements_data, elements_model);
				break;
			case VC_V2V_4D:
				count_corr = find_corr_v2v_4d(icp_xf, elements_data_4d, elements_model_4d);
				break;
			default:
				count_corr = find_corr_gt(icp_xf, elements_data, elements_model);
				break;
			}

			// cout << "Returned corr size: " << count_corr << endl;

			// double corr_d = corr_dist(elements_data, elements_model, "v2v");
			// double corr_d = corr_dist(elements_data, elements_model, "v2n");
			// cout << "Itr " << (i + 1) << " corr-pairs dist: " << corr_d << endl;

			xform xf_incremental;
			bool aligned = false;

			switch (variant_align) {
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
			case VA_P2P_4D:
				aligned = align_v2v_4d(elements_data_4d, elements_model_4d, xf_incremental);
				break;
			}

			if (aligned) {

				// apply_xform(mesh_data, aligned_xf);

				icp_xf = xf_incremental * icp_xf;
				orthogonalize(icp_xf);

				// double updated_err = cal_dist(mesh_data, mesh_model, kd_model);
				// cout << "Itr " << (i + 1) << " point-wise dist: " << updated_err << endl;

				double updated_err = cal_err(mesh_data, icp_xf);
				cout << "Itr " << (i + 1) << " err: " << updated_err << endl;

				stat.per_itr_err.push_back(updated_err);

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

				stat.per_itr_err.push_back(stat.per_itr_err.back());

				if (1) { // TODO: if for LM methods
					lambda *= 2.0;
				}

			}

		}
	}
};

#ifndef _TEST

int main(int argc, char* argv[])
{
	if (argc < 5) {
		cerr << "Usage: " << argv[0] << " filename_model filename_data variant_corr variant_align" << endl;
		exit(1);
	}

	const string filename_model(argv[1]);
	const string filename_data(argv[2]);
	int variant_corr = atoi(argv[3]);
	int variant_align = atoi(argv[4]);

	cout.precision(15);

	ICP_Matching matching(filename_model, filename_data, variant_corr, variant_align);
	matching.load();
	cout << endl;

	// cout << "Ground-truth dist: " << cal_dist(mesh_data, mesh_model, kd_model) << endl;
	// matching.mesh_model->write("ply_ascii:groundt_model.ply");
	// matching.mesh_data->write("ply_ascii:groundt_data.ply");

	cout << "Ground-truth err: " << cal_err(matching.mesh_data, xform()) << endl;

	// vector<Corr_Element> elements_data;
	// vector<Corr_Element> elements_model;
	// find_corr_v2v(mesh_data, mesh_model, kd_model, elements_data, elements_model);
	// cout << "Initl corr-dist: " << corr_dist(elements_data, elements_model, "v2n") << endl;

	// auto init_xf = to_xform_matrix(M_PI / 20.0, M_PI / 20.0, 0, 0.03, -0.02, 0);
	auto init_xf = to_xform_matrix(M_PI / 10.0, M_PI / 10.0, 0, 0.1, -0.2, 0);

	double init_err = cal_err(matching.mesh_data, init_xf);
	cout << "Initl err: " << init_err << endl;

	// write_mesh(matching.mesh_data, "ply_ascii:initial_data.ply", init_xf);

	ICP_Stat stat;
	xform final_xf;
	stat.per_itr_err.push_back(init_err);
	matching.icp(init_xf, final_xf, stat);

	// cout << "Final dist: " << cal_dist(mesh_data, mesh_model, kd_model) << endl;
	cout << "Final err: " << cal_err(matching.mesh_data, final_xf) << endl;

	// write_mesh(matching.mesh_data, "ply_ascii:aligned_data.ply", final_xf);


	ofstream ofs("err_" + to_string(variant_corr) + "_" + to_string(variant_align) + ".txt");
	for (int i = 0; i < stat.per_itr_err.size(); i++) {
		ofs << i << " " << stat.per_itr_err[i] << endl;
	}
	ofs.close();
}

#else

void toy_test()
{
	auto mesh_toy = TriMesh::read("toy.ply");
	mesh_toy->need_faces();

	apply_xform(mesh_toy, to_xform_matrix(M_PI / 4.0, M_PI / 4.0, 0, 0, 0, 0));

	mesh_toy->write("ply_ascii:toy_rot.ply");
}

int main()
{
	srand(time(NULL));
	cout.precision(15);

	test_inverse();

 //    cout << "-------------------------------" << endl;
 //    cout << "Test function get_proj_p2f" << endl;
 //    cout << "-------------------------------" << endl;
	// test_get_proj_p2f();

 //    cout << "-------------------------------" << endl;
 //    cout << "Test function sq_dist_p2l" << endl;
 //    cout << "-------------------------------" << endl;
	// test_sq_dist_p2l();

 //    cout << "-------------------------------" << endl;
 //    cout << "Test function sq_dist_p2f" << endl;
 //    cout << "-------------------------------" << endl;
	// test_sq_dist_p2f();

 //    cout << "-------------------------------" << endl;
 //    cout << "Test function sq_dist_e2e" << endl;
 //    cout << "-------------------------------" << endl;
	// test_sq_dist_e2e();

 //    cout << "-------------------------------" << endl;
 //    cout << "Test function inters_line_tri" << endl;
 //    cout << "-------------------------------" << endl;
	// test_inters_line_tri();

 //    cout << "-------------------------------" << endl;
 //    cout << "Test function nn_triangle" << endl;
 //    cout << "-------------------------------" << endl;
	// test_nn_triangle();
}

#endif