#include <string>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <stack>

#include "c_rtree.h"

using namespace std;

bool C_RTree::m_info_created = false;
rtree_info C_RTree::m_info = { };

// private
string C_RTree::get_rst_filename(const string mesh_filename) const {
	return (mesh_filename + ".rst.0");
}

string C_RTree::get_info_filename(Usage usage) const {
	string ret = "";
	switch (usage) {
	case PCD:
		ret = "../common/config/rstree.pcd.config";
		break;
	case IDX:
		ret = "../common/config/rstree.idx.config";
		break;
	}
	return ret;
}

void C_RTree::read_rstree_info(const string info_filename) {
    ifstream ifs(info_filename);
    ifs >> m_info.m >> m_info.M >> m_info.dim >> m_info.reinsert_p;
    ifs.close();
}

R_TYPE* C_RTree::convert_pt(Pt3D* p) const {
    R_TYPE* ret = (R_TYPE *) malloc(sizeof(R_TYPE) * 3);
    ret[0] = (int) (p->x * RSTREE_SCALE);
    ret[1] = (int) (p->y * RSTREE_SCALE);
    ret[2] = (int) (p->z * RSTREE_SCALE);
    return ret;
}


// public
C_RTree::C_RTree(Usage usage) {
	if (!m_info_created) {
		read_rstree_info(get_info_filename(usage));
		m_info_created = true;
	}
}

double C_RTree::nn_sphere(Pt3D* p, double r, int* ret, const unordered_set<int>& excl_id_set, int k) {
	auto query = convert_pt(p);
    NN_type *nn;
    int real_k = excl_id_set.size() + k;

    if (r == 0.0) {
    	k_NN_search(m_root, query, real_k, &nn, &m_info);
    } else {
    	long long sq_r_long = (long long) (r * r * RSTREE_SCALE * RSTREE_SCALE);
    	k_NN_search_sphere(m_root, query, real_k, &nn, &m_info, sq_r_long);
    }

    NN_type *nn_copy = nn;

    stack<NN_type*> s;
    for (int i = 0; i < real_k; i++) {
    	s.push(nn);
    	nn = nn->next;
    }

    double nn1_dist;
    int i = 0;
    while (i < k) {
    	auto top = s.top();
    	s.pop();
    	if (excl_id_set.find(top->oid) == excl_id_set.end()) { // not in the exclusive list
    		if (i == 0) {
    			nn1_dist = (double) sqrt((double) top->dist / (double) RSTREE_SCALE / (double) RSTREE_SCALE);
    		}
    		ret[i] = top->oid;
    		i++;
    	}
    }

    NN_freeChain(nn_copy);

    return nn1_dist;
}

void C_RTree::read_from_mesh(const string mesh_filename) {
	read_rtree(&m_root, get_rst_filename(mesh_filename).c_str(), &m_info);
}

void C_RTree::range_sphere_min_max(Pt3D* p, double r_min, double r_max, vector<int>& ret, const unordered_set<int>& excl_id_set) {
	auto query = convert_pt(p);
	RangeReturn_type* rr = NULL;

	const long long sq_dist_min_long = (long long)(r_min * r_min * RSTREE_SCALE * RSTREE_SCALE);
	const long long sq_dist_max_long = (long long)(r_max * r_max * RSTREE_SCALE * RSTREE_SCALE);

	// cout << sq_dist_min_long << endl;
	// cout << sq_dist_max_long << endl;

	sphere_search(m_root, query, sq_dist_min_long, sq_dist_max_long, &rr, &m_info);

	while (rr) {
		if (excl_id_set.find(rr->oid) == excl_id_set.end()) { // not in the exclusive list
			// ret.push_back({ rr->oid, rr->dist });
			ret.push_back(rr->oid);
		}
		RangeReturn_type* prev = rr->prev;
		free(rr);
		rr = prev;
	}

	free(query);
}

void C_RTree::range_sphere_dist_err(Pt3D* p, double dist, double err, vector<int>& ret, const unordered_set<int>& excl_id_set) {
	range_sphere_min_max(p, dist - err, dist + err, ret, excl_id_set);
}

void C_RTree::nn_sphere_range(Pt3D* p, double sq_dist, double err, vector<int>& ret, int excl_id_list[], int excl_id_num) {
	auto query = convert_pt(p);
	RangeReturn_type* rr = NULL;

	long long sq_dist_long;
	if (sq_dist == 0.0) {
		sq_dist_long = 0;
	} else {
		sq_dist_long = (long long) (sq_dist * RSTREE_SCALE * RSTREE_SCALE);
	}
	double span;
	if (err == 0.0) {
		span = 0.0;
	} else {
		span = err * 2 * RSTREE_SCALE;
	}

	NN_range_search(m_root, query, &rr, &m_info, sq_dist_long, span, excl_id_list, excl_id_num);

	while (rr) {
		// ret.push_back({ rr->oid, rr->dist });
		ret.push_back(rr->oid);
		RangeReturn_type* next = rr->next;
		free(rr);
		rr = next;
	}

}

// int C_RTree::window_query_bundle(node_type *r_root, rtree_info* r_info, const vector<Entry*>& e_list, double err, double bbox_min[], double bbox_max[],
// 	vector<BundleReturn_type*>& ret, int& page_accessed) {

// 	int e_count = e_list.size();

// 	R_TYPE** query = (R_TYPE **) malloc(sizeof(R_TYPE *) * e_count);
// 	for (int i = 0; i < e_count; i++) {
// 		R_TYPE* ind_query = (R_TYPE *) malloc(sizeof(R_TYPE) * 6);
// 		for (int j = 0; j < 6; j++) {
// 			ind_query[j] = e_list[i]->sides[j];
// 		}
// 		query[i] = ind_query;
// 	}

// 	BundleReturn_type* br = NULL;

// 	double* min = (double *) malloc(sizeof(double) * 6);
// 	double* max = (double *) malloc(sizeof(double) * 6);
// 	for (int i = 0; i < 6; i++) {
// 		min[i] = bbox_min[i];
// 		max[i] = bbox_max[i];
// 	}

// 	page_accessed = rectangle_search_bundle(r_root, e_count, query, err, min, max, &br, r_info);

// 	for (int i = 0; i < e_count; i++) {
// 		free(query[i]);
// 	}
// 	free(query);
// 	free(min);
// 	free(max);

// 	int num = 0;

// 	while (br) {
// 		num++;
// 		ret.push_back(br);
// 		br = br->prev;
// 	}

// 	return num;
// }

void read_rtrees_from_db_meshes(DB_Meshes* db_meshes, vector<C_RTree>& ret, bool verbose) {
    for (int i = 0; i < db_meshes->size(); i++) {
        string db_rtree_filename = db_meshes->get_mesh(i)->get_filename();
        if (verbose) cout << "Reading R-tree for " << db_rtree_filename << endl;

        C_RTree rtree;
        rtree.read_from_mesh(db_rtree_filename);
        ret.push_back(rtree);
    }
}