#include "tetra_meas.h"
#include "TriMesh.h"
#include <iostream>
#include <cmath>
#include <Dense>
using namespace std;
using namespace trimesh;
using namespace Eigen;

// local help function
double area_3d(double a, double b, double c){
	double s = (a + b + c) * 0.5;
	return sqrt(s * (s - a) * (s - b) * (s - c));
}

Pt2D circumcenter_2d(Pt2D a, Pt2D b, Pt2D c) {
	Pt2D ab = {b.x - a.x, b.y - a.y};
	Pt2D d = {(a.x + b.x) * 0.5, (a.y + b.y) * 0.5};
	Pt2D ac = {c.x - a.x, c.y - a.y};
	Pt2D e = {(a.x + c.x) * 0.5, (a.y + c.y) * 0.5};
	Matrix2d m;
	m << ab.x, ab.y, ac.x, ac.y;
	Vector2d p(ab.x * d.x + ab.y * d.y, ac.x * e.x + ac.y * e.y);
	Vector2d o = m.inverse() * p;
	return {o(0), o(1)};
}

Pt3D circumcenter_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d) {
	Pt3D ab = {b.x - a.x, b.y - a.y, b.z - a.z};
	Pt3D e = {(a.x + b.x) * 0.5, (a.y + b.y) * 0.5, (a.z + b.z) * 0.5};
	Pt3D ac = {c.x - a.x, c.y - a.y, c.z - a.z};
	Pt3D f = {(a.x + c.x) * 0.5, (a.y + c.y) * 0.5, (a.z + c.z) * 0.5};
	Pt3D ad = {d.x - a.x, d.y - a.y, d.z - a.z};
	Pt3D g = {(a.x + d.x) * 0.5, (a.y + d.y) * 0.5, (a.z + d.z) * 0.5};
	Matrix3d m;
	m << ab.x, ab.y, ab.z, ac.x, ac.y, ac.z, ad.x, ad.y, ad.z;
	Vector3d p(
		ab.x * e.x + ab.y * e.y + ab.z * e.z,
		ac.x * f.x + ac.y * f.y + ac.z * f.z,
		ad.x * g.x + ad.y * g.y + ad.z * g.z);
	Vector3d o = m.inverse() * p;
	return {o(0), o(1), o(2)};
}

Ratio_set get_ratio_set_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d){
	Ratio_set ret;
	Vector3d ab(b.x - a.x, b.y - a.y, b.z - a.z);
	Vector3d ac(c.x - a.x, c.y - a.y, c.z - a.z);
	Vector3d ad(d.x - a.x, d.y - a.y, d.z - a.z);
	double vol_by_6 = abs(ab.dot(ac.cross(ad)));
	double len_ab = sqrt(ab.dot(ab));
	double len_ac = sqrt(ac.dot(ac));
	double len_ad = sqrt(ad.dot(ad));
	Vector3d B = ac - ad;
	Vector3d C = ad - ab;
	Vector3d D = ab - ac;
	double len_B = sqrt(B.dot(B));
	double len_C = sqrt(C.dot(C));
	double len_D = sqrt(D.dot(D));
	ret.inradius = vol_by_6 * 0.5 / (
		area_3d(len_ab, len_ac, len_D) +
		area_3d(len_ac, len_ad, len_B) +
		area_3d(len_ad, len_ab, len_C) +
		area_3d(len_B, len_C, len_D));
	ret.circumradius = sqrt(
		(len_ab * len_B + len_ac * len_C + len_ad * len_D) *
		(len_ab * len_B + len_ac * len_C - len_ad * len_D) *
		(len_ab * len_B - len_ac * len_C + len_ad * len_D) *
		(-len_ab * len_B + len_ac * len_C + len_ad * len_D)) / (vol_by_6 * 4.0);
	ret.ratio = 3.0 * ret.inradius / ret.circumradius;
	return ret;
}

double circumradi_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d) {
	Pt3D ab = {b.x - a.x, b.y - a.y, b.z - a.z};
	Pt3D ac = {c.x - a.x, c.y - a.y, c.z - a.z};
	Pt3D ad = {d.x - a.x, d.y - a.y, d.z - a.z};

	Pt3D fac1 = scale_pt(sq_mode(ab), cross_prd(ac, ad));
	Pt3D fac2 = scale_pt(sq_mode(ac), cross_prd(ad, ab));
	Pt3D fac3 = scale_pt(sq_mode(ad), cross_prd(ab, ac));
	double vol_by_6 = abs(dot_prd(ab, cross_prd(ac, ad)));

	double rad = mode(sum_pt(sum_pt(fac1, fac2), fac3));
	rad /= (2.0 * vol_by_6);

	return rad;
}

double bounding_radi_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d){
	TriMesh *m = new TriMesh;
	m->vertices.push_back(point(a.x, a.y, a.z));
	m->vertices.push_back(point(b.x, b.y, b.z));
	m->vertices.push_back(point(c.x, c.y, c.z));
	m->vertices.push_back(point(d.x, d.y, d.z));
	m->need_bsphere();
	double ret = m->bsphere.r;
	delete m;
	return ret;
}

// int main() {
// 	Pt3D a = {0, 0, 0};
// 	Pt3D b = {0, 20, 0};
// 	Pt3D c = {20, 0, 0};
// 	Pt3D d = {0, 0, 20};
// 	cout << bounding_radi_3d(a, b, c, d) << endl;
// 	Pt3D e = circumcenter_3d(a, b, c, d);
// 	print_pt(&e);
// 	cout << endl;
// 	auto r = get_ratio_set_3d(a, b, c, d);
// 	cout << r.inradius << endl;
// 	cout << r.circumradius << endl;
// 	cout << r.ratio << endl;

// 	Pt2D x = {0, 0};
// 	Pt2D y = {20, 0};
// 	Pt2D z = {0, 20};
// 	Pt2D o = circumcenter_2d(x, y, z);
// 	print_pt(&o);
// 	cout << endl;
// }