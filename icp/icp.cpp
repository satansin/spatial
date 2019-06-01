#include "point.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <Dense>
#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "ICP.h"

using namespace std;
using namespace Eigen;
using namespace trimesh;

#define MAX_COM_LENGTH 100

struct Trans {
	double r11;
	double r12;
	double r13;
	double r21;
	double r22;
	double r23;
	double r31;
	double r32;
	double r33;
	double tx;
	double ty;
	double tz;
};

Trans* create_trans(double al, double be, double ga, double tx, double ty, double tz) {
	Trans* ret = new Trans;
	double cal = cos(al);
	double sal = sin(al);
	double cbe = cos(be);
	double sbe = sin(be);
	double cga = cos(ga);
	double sga = sin(ga);
	ret->r11 = cal * cbe;
	ret->r12 = cal * sbe * sga - sal * cga;
	ret->r13 = cal * sbe * cga + sal * sga;
	ret->r21 = sal * cbe;
	ret->r22 = sal * sbe * sga + cal * cga;
	ret->r23 = sal * sbe * cga - cal * sga;
	ret->r31 = sbe * (-1);
	ret->r32 = cbe * sga;
	ret->r33 = cbe * cga;
	ret->tx = tx;
	ret->ty = ty;
	ret->tz = tz;
	return ret;
}

Pt3D trans_pt(const Trans* t, const Pt3D* p) {
	Pt3D t_p = {0.0};
	t_p.x = t->r11 * p->x + t->r12 * p->y + t->r13 * p->z + t->tx;
	t_p.y = t->r21 * p->x + t->r22 * p->y + t->r23 * p->z + t->ty;
	t_p.z = t->r31 * p->x + t->r32 * p->y + t->r33 * p->z + t->tz;
	return t_p;
}

int main(int argc, char** argv) {

	// test code
	// double al = 0;
	// double be = 0;
	// double ga = 0.5236;
	// double tx = 1.1;
	// double ty = 1.5;
	// double tz = 2.2;
	// Trans* tr = create_trans(al, be, ga, tx, ty, tz);
	// cout << "The transformation matrix is:" << endl;
	// cout << tr->r11 << ", " << tr->r12 << ", " << tr->r13 << ", " << tr->tx << endl;
	// cout << tr->r21 << ", " << tr->r22 << ", " << tr->r23 << ", " << tr->ty << endl;
	// cout << tr->r31 << ", " << tr->r32 << ", " << tr->r33 << ", " << tr->tz << endl;
	// cout << 0 << ", " << 0 << ", " << 0 << ", " << 1 << endl << endl;

	TriMesh *mesh1 = TriMesh::read("1.ply");
	TriMesh *mesh2 = TriMesh::read("2.ply");

	xform xf1;
	// string xffilename1 = xfname(filename1);
	// xf1.read(xffilename1);

	xform xf2;
	// string xffilename2 = xfname(filename2);
	// xf2.read(xffilename2);

	KDtree *kd1 = new KDtree(mesh1->vertices);
	KDtree *kd2 = new KDtree(mesh2->vertices);
	vector<float> weights1, weights2;

	float err = ICP(mesh1, mesh2, xf1, xf2, kd1, kd2, weights1, weights2, 0.0f);

	apply_xform(mesh2, xf2);

	TriMesh *mesh2_margin = new TriMesh;
	for (auto &v: mesh2->vertices) {
		const float *nn = kd1->closest_to_pt((const float *) v, err);
		if (nn == NULL) {
			mesh2_margin->vertices.push_back(v);
		}
	}
	cout << "After filtering, " << mesh2_margin->vertices.size() << " vertices left" << endl;

	vector<TriMesh *> meshes;
	meshes.push_back(mesh1);
	meshes.push_back(mesh2_margin);

	join(meshes)->write("join.ply");

}
