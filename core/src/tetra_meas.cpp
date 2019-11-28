#include "tetra_meas.h"
#include "point.h"
#include "TriMesh.h"
#include <iostream>
#include <cmath>
#include <Dense>
using namespace std;
using namespace trimesh;
using namespace Eigen;

static const double PI = 3.14159265359;

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
	Pt3D ab = b - a;
	Pt3D e = middle_pt(a, b);
	Pt3D ac = c - a;
	Pt3D f = middle_pt(a, c);
	Pt3D ad = d - a;
	Pt3D g = middle_pt(a, d);
	Matrix3d m;
	m << ab.x, ab.y, ab.z, ac.x, ac.y, ac.z, ad.x, ad.y, ad.z;
	Vector3d p(
		ab.x * e.x + ab.y * e.y + ab.z * e.z,
		ac.x * f.x + ac.y * f.y + ac.z * f.z,
		ad.x * g.x + ad.y * g.y + ad.z * g.z);
	Vector3d o = m.inverse() * p;
	return {o(0), o(1), o(2)};
}

Ratio_set get_ratio_set_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d) {
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

Ratio_set_vol get_ratio_set_vol(Pt3D a, Pt3D b, Pt3D c, Pt3D d) {
	Ratio_set_vol ret;
	ret.volume = volume_3d(a, b, c, d);
	ret.ratio = 8.162097 * ret.volume / bounding_vol_3d(a, b, c, d);
	return ret;
}

Ratio_set_area get_ratio_set_area(Pt3D a, Pt3D b, Pt3D c) {
	Ratio_set_area ret;
	auto ab = b - a, ac = c - a, bc = c - b;
	double a_cross_mode = cross_prd(b - a, c - a).mode();
	ret.area = 0.5 * a_cross_mode;
	double circum_r = 0.5 * ab.mode() * ac.mode() * bc.mode() / a_cross_mode;
	double circum_area = PI * circum_r * circum_r;
	ret.ratio = 2.418399 * ret.area / circum_area;
	return ret;
}

double volume_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d) {
	Vector3d ab(b.x - a.x, b.y - a.y, b.z - a.z);
	Vector3d ac(c.x - a.x, c.y - a.y, c.z - a.z);
	Vector3d ad(d.x - a.x, d.y - a.y, d.z - a.z);
	return abs(ab.dot(ac.cross(ad))) / 6.0;
}

double circumradi_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d) {
	Pt3D ab = {b.x - a.x, b.y - a.y, b.z - a.z};
	Pt3D ac = {c.x - a.x, c.y - a.y, c.z - a.z};
	Pt3D ad = {d.x - a.x, d.y - a.y, d.z - a.z};

	Pt3D fac1 = cross_prd(ac, ad) * ab.sq_mode();
	Pt3D fac2 = cross_prd(ad, ab) * ac.sq_mode();
	Pt3D fac3 = cross_prd(ab, ac) * ad.sq_mode();
	double vol_by_6 = abs(dot_prd(ab, cross_prd(ac, ad)));

	double rad = (fac1 + fac2 + fac3).mode();
	rad /= (2.0 * vol_by_6);

	return rad;
}

double bounding_radi_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d) {
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

double bounding_vol_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d) {
	double r = bounding_radi_3d(a, b, c, d);
	return r * r * r * PI * 4.0 / 3.0;
}

void get_plane_params(Pt3D p, Pt3D q, Pt3D r, double& a, double& b, double& c) {
	Matrix3d m;
	m << p.x, p.y, p.z, q.x, q.y, q.z, r.x, r.y, r.z;
	Vector3d one(1, 1, 1);
	Vector3d n = m.inverse() * one;
	a = n(0);
	b = n(1);
	c = n(2);
}

void get_nearest_farest(Pt3D o, Pt3D p, Pt3D q, Pt3D r, double drift, Pt3D& nearest, Pt3D& farest) {
	double a, b, c;
	get_plane_params(p, q, r, a, b, c);
	double length = sqrt(a * a + b * b + c * c);
	a = a / length * drift;
	b = b / length * drift;
	c = c / length * drift;
	nearest = { o.x + a, o.y + b, o.z + c };
	farest = { o.x - a, o.y - b, o.z - c };
}

void cal_range(Pt3D a, Pt3D b, Pt3D c, Pt3D d, double drift,
	double& low_vol, double& high_vol, double& low_ratio, double& high_ratio) {

	Pt3D a_n, b_n, c_n, d_n, a_f, b_f, c_f, d_f;
	get_nearest_farest(a, b, c, d, drift, a_n, a_f);
	get_nearest_farest(b, a, c, d, drift, b_n, b_f);
	get_nearest_farest(c, a, b, d, drift, c_n, c_f);
	get_nearest_farest(d, a, b, c, drift, d_n, d_f);
	low_vol = volume_3d(a_n, b_n, c_n, d_n) - 0.05;
	high_vol = volume_3d(a_f, b_f, c_f, d_f) + 0.05;

	double b_r = bounding_radi_3d(a, b, c, d);
	double low_b_r = b_r - drift;
	double high_b_r = b_r + drift;
	double low_b_v = low_b_r * low_b_r * low_b_r * PI * 4.0 / 3.0;
	double high_b_v = high_b_r * high_b_r * high_b_r * PI * 4.0 / 3.0;
	low_ratio = 8.162097 * low_vol / high_b_v;
	high_ratio = 8.162097 * high_vol / low_b_v;
}