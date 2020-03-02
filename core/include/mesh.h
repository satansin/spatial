#ifndef __MESH_H_
#define __MESH_H_

#include <vector>
#include <fstream>
#include <cassert>
#include <limits>

#include "TriMesh.h"
#include "Vec.h"
#include "Box.h"
#include "KDtree.h"
#include "TriMesh_algo.h"

#include "point.h"
#include "trans.h"

class Mesh {

private:
	int m_id;
	trimesh::TriMesh* m_mesh;
	int m_size;
	std::string m_filename;

	friend class DB_Meshes;

	bool m_bbox_built;

	void build_bbox();

public:
	Mesh(int id);

	Mesh() : Mesh(0) { }

	void read_from_path(const std::string s_file);

	int size() const;

	Pt3D get_pt(int idx) const;

	const std::string get_filename() const;

	double get_box_min(int i); 
	double get_box_max(int i);

	void centralize();

	double box_max_scale();

	void scale_by(double s);

private:
	static Pt3D pt(trimesh::point p) {
		return {p[0], p[1], p[2]};
	}
};

class DB_Meshes {

private:
	std::vector<Mesh> m_db_meshes;
	int m_total;
	int m_size;

	int m_kd_built;
	std::vector<trimesh::KDtree*> m_db_kds;

private:
	std::string get_meta_filename(const std::string path) const;

public:
	DB_Meshes();

	int read_from_path(const std::string db_path);

	void build_kd();

	bool kd_built() const;

	int size() const;

	int total() const;

	Mesh* get_mesh(int id);

	// const KDtree* get_kd(int id) {
	// 	assert(id >= 0 && id < m_size);
	// 	return m_db_kds[id];
	// }
	
	double cal_corr_err(Mesh* mesh_q, int id, Trans* xf, double stop_at = std::numeric_limits<double>::max());

};

#endif