#include <iostream>
#include "tetra_meas.h"
#include "TriMesh.h"
#include "pcr_adv.h"
using namespace std;
using namespace trimesh;

int main(int argc, char **argv) {

	TriMesh *mesh_p = TriMesh::read("/csproject/kdd/hliubs/data/spatial/3dor/db/redwood-01-livingroom.ply");
	auto pa = pt(mesh_p->vertices[1556056]);
	auto pb = pt(mesh_p->vertices[1552565]);
	auto pc = pt(mesh_p->vertices[1534709]);
	auto pd = pt(mesh_p->vertices[1549099]);
	auto pratio_set = get_ratio_set_vol(pa, pb, pc, pd);
	cout << pratio_set.volume << endl;
	cout << pratio_set.ratio << endl;
	cout << eucl_dist(pa, pb) << endl;
	cout << eucl_dist(pa, pc) << endl;
	cout << eucl_dist(pa, pd) << endl;
	cout << eucl_dist(pb, pc) << endl;
	cout << eucl_dist(pb, pd) << endl;
	cout << eucl_dist(pc, pd) << endl;

	TriMesh *mesh_q = TriMesh::read("/csproject/kdd/hliubs/data/spatial/3dor/query/redwood-01-q2.ply");
	auto qa = pt(mesh_q->vertices[135843]);
	auto qb = pt(mesh_q->vertices[135212]);
	auto qc = pt(mesh_q->vertices[132053]);
	auto qd = pt(mesh_q->vertices[134604]);
	auto qratio_set = get_ratio_set_vol(qa, qb, qc, qd);
	cout << qratio_set.volume << endl;
	cout << qratio_set.ratio << endl;
	cout << eucl_dist(qa, qb) << endl;
	cout << eucl_dist(qa, qc) << endl;
	cout << eucl_dist(qa, qd) << endl;
	cout << eucl_dist(qb, qc) << endl;
	cout << eucl_dist(qb, qd) << endl;
	cout << eucl_dist(qc, qd) << endl;
}