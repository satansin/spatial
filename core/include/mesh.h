#ifndef __MESH_H_
#define __MESH_H_

#include <vector>
#include <fstream>
#include <cassert>
#include <limits>
#include <unordered_set>

#include "TriMesh.h"
#include "Vec.h"
#include "Box.h"
#include "KDtree.h"
#include "TriMesh_algo.h"
// #include "jly_icp3d.hpp"

#include "point.h"
#include "trans.h"

struct PtwID;

class Mesh {

private:
	int m_id;
	trimesh::TriMesh* m_mesh;
	int m_size;
	std::string m_filename;

	std::vector<Pt3D> m_pt_list;
	std::vector<PtwID> m_ptwid_list;

	friend class DB_Meshes;

	bool m_bbox_built;
	bool m_bsphere_built;

	void build_bbox();
	void build_bsphere();

public:
	Mesh(int id);

	Mesh() : Mesh(0) { }

	~Mesh();

	Mesh(const Mesh* mesh);

	void free_mesh();

	void read_from_path(const std::string s_file);

	int size() const;

	Pt3D* get_pt(int idx);
	PtwID* get_ptwid(int idx);

	const std::string get_filename() const;

	double get_box_min(int i); 
	double get_box_max(int i);

	double get_bsphere_r();
	double get_bsphere_d();
	void get_bsphere_o(Pt3D& o);

	void centralize();

	double box_max_scale();

	void scale_by(double s);

	static PtwID DEFAULT_PT;

	void insert_pt(Pt3D pt);

	void write(std::string filename);

private:
	static void pt(trimesh::point p, Pt3D& ret) {
		ret.x = p[0];
		ret.y = p[1];
		ret.z = p[2];
	}
};

class DB_Meshes {

private:
	std::vector<Mesh*> m_db_meshes;
	int m_total;
	int m_size;

	bool m_kd_built;
	std::vector<trimesh::KDtree*> m_db_kds;

	// bool m_icp_init;
	// std::vector<ICP3D<float>*> m_icps;
	// std::vector<float*> m_data_icps;

private:
	void read_normal(std::ifstream& ifs_meta, const std::string folder);
	void read_combined(std::ifstream& ifs_meta, std::ifstream& ifs_combined, std::string folder);

public:
	DB_Meshes();
	
	~DB_Meshes();

	void free_mesh();

	int read_from_path(const std::string db_path);
	int read_from_file(const std::string db_file);

	void build_kd();

	bool kd_built() const;

	void build_icp(double err_diff);

	bool icp_built() const;

	int size() const;

	int total() const;

	Mesh* get_mesh(int id);

	// const KDtree* get_kd(int id) {
	// 	assert(id >= 0 && id < m_size);
	// 	return m_db_kds[id];
	// }
	
	// it returns SSE
	double cal_corr_err(Mesh* mesh_q, int id, Trans* xf = NULL, double stop_at = std::numeric_limits<double>::max());
	// it returns SSE, and will calculate the transformed box (cube)
	double cal_corr_err(Mesh* mesh_q, int id, Trans* xf, double box_min[3], double box_max[3]);
	// it returns error for single point (not squared error!!)
	double cal_corr_err(double x, double y, double z, int id);

	void retrieve(Mesh* mesh_q, int id, std::unordered_set<int>& ret, Trans* xf = NULL);

};

struct PtwID {
	int id;
	Pt3D* pt;
	void set(int id, Pt3D* pt) {
		this->id = id;
		this->pt = pt;
	}
	void set(int id, Mesh* mesh) {
		set(id, mesh->get_pt(id));
	}
	PtwID() {
		this->id = -1;
	}
	PtwID(int id, Pt3D* pt) {
		set(id, pt);
	}
	PtwID(int id, Mesh* mesh) {
		set(id, mesh);
	}
	PtwID(PtwID* from) {
		set(from->id, from->pt);
	}
};

#endif