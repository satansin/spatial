#ifndef __C_RTREE_H_
#define __C_RTREE_H_

#include <string>
#include <fstream>
#include <vector>
#include <cstdio>
#include <unordered_set>

extern "C" {
    #include "rtree.h"
}

#include "point.h"
#include "mesh.h"
#include "util.h"

struct ID_Dist {
    int oid;
    long long dist;
};

class C_RTree {

public:
	enum Usage {
		PCD, IDX
	};

private:
	static bool m_info_created;
	static rtree_info m_info;
	node_type* m_root;

	static const int RSTREE_SCALE = 1e5;

private:
	std::string get_rst_filename(const std::string mesh_filename) const;

	std::string get_info_filename(Usage usage) const;

	void read_rstree_info(const std::string info_filename);

	R_TYPE* convert_pt(Pt3D* p) const;
	
	R_TYPE* convert_donut(Pt3D* o, double r, Pt3D* n) const;

public:
	C_RTree(Usage usage = PCD);
	~C_RTree();

	void read_from_mesh(const std::string mesh_filename);
	void read_comb(FILE *fp);

	double donut_nn(Pt3D* o, double r, Pt3D* n, int* ret, const std::unordered_set<int>& excl_id_set = {}, int k = 1);

	double nn_sphere(Pt3D* p, double r, int* ret, const std::unordered_set<int>& excl_id_set = {}, int k = 1);

	void range_sphere_min_max(Pt3D* p, double r_min, double r_max, std::vector<int>& ret, const std::unordered_set<int>& excl_id_set = {});
	void range_sphere_dist_err(Pt3D* p, double dist, double err, std::vector<int>& ret, const std::unordered_set<int>& excl_id_set = {});

	void nn_sphere_range(Pt3D* p, double sq_dist, double err, std::vector<int>& ret, int excl_id_list[] = {}, int excl_id_num = 0);

	// int window_query_bundle(node_type *r_root, rtree_info* r_info, const vector<Entry*>& e_list, double err, double bbox_min[], double bbox_max[],
	// vector<BundleReturn_type*>& ret, int& page_accessed);

};

void read_rtrees_comb(const std::string db_path, int part_i, int part_size, std::vector<C_RTree*>& ret);
void read_rtrees_from_db_meshes(DB_Meshes* db_meshes, std::vector<C_RTree*>& ret, bool verbose = true);

#endif