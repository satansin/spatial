#include <iostream>
#include <fstream>
#include "tetra_meas.h"
#include "TriMesh.h"
#include "pcr_adv.h"
using namespace std;
using namespace trimesh;

int main(int argc, char **argv) {

	cout << setprecision(10);

	// TriMesh *mesh_p = TriMesh::read("/project/kdd/hliubs/data/spatial/3dor/db/obj_scans/00013.ply");
	// auto pa = pt(mesh_p->vertices[112990]);
	// auto pb = pt(mesh_p->vertices[107179]);
	// auto pc = pt(mesh_p->vertices[146031]);
	// auto pd = pt(mesh_p->vertices[107267]);
	// auto pratio_set = get_ratio_set_vol(pa, pb, pc, pd);
	// cout << pratio_set.volume << endl;
	// cout << pratio_set.ratio << endl;
	// cout << eucl_dist(pa, pb) << endl;
	// cout << eucl_dist(pa, pc) << endl;
	// cout << eucl_dist(pa, pd) << endl;
	// cout << eucl_dist(pb, pc) << endl;
	// cout << eucl_dist(pb, pd) << endl;
	// cout << eucl_dist(pc, pd) << endl;

	// TriMesh *mesh_q = TriMesh::read("/project/kdd/hliubs/data/spatial/3dor/query/obj_scans/q1.ply");
	// auto qa = pt(mesh_q->vertices[112990]);
	// auto qb = pt(mesh_q->vertices[107179]);
	// auto qc = pt(mesh_q->vertices[146031]);
	// auto qd = pt(mesh_q->vertices[107267]);
	// auto qratio_set = get_ratio_set_vol(qa, qb, qc, qd);
	// cout << qratio_set.volume << endl;
	// cout << qratio_set.ratio << endl;
	// cout << eucl_dist(qa, qb) << endl;
	// cout << eucl_dist(qa, qc) << endl;
	// cout << eucl_dist(qa, qd) << endl;
	// cout << eucl_dist(qb, qc) << endl;
	// cout << eucl_dist(qb, qd) << endl;
	// cout << eucl_dist(qc, qd) << endl;

    // cout << "Reading database file..." << endl;
    // unordered_map<int, TriMesh*> db_meshes;
    // int num = read_db_mesh_batch("/project/kdd/hliubs/data/spatial/3dor/db/obj_scans_100/", db_meshes);

    // ofstream ofs("/project/kdd/hliubs/data/spatial/3dor/db/obj_scans_100/stat.txt");
    // ofs << setprecision(10);

    // for (int i = 0; i < num; i++) {
    // 	auto mesh = db_meshes[i];
    // 	ofs << mesh->vertices.size() << "\t";

    // 	mesh->need_bbox();
    // 	auto bbox = mesh->bbox;
    // 	ofs << (bbox.max[0] - bbox.min[0]) << "\t";
    // 	ofs << (bbox.max[1] - bbox.min[1]) << "\t";
    // 	ofs << (bbox.max[2] - bbox.min[2]) << "\n";
    // }

    // ofs.close();

}