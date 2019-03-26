#include <iostream>
#include <cmath>
using namespace std;

struct Pt2D
{
	double x;
	double y;
};

struct Pt3D
{
	double x;
	double y;
	double z;
};

double eucl_dist(Pt2D p, Pt2D q) {
	return sqrt((p.x - q.x) * (p.x - q.x) +
				(p.y - q.y) * (p.y - q.y));
}

double eucl_dist(Pt3D p, Pt3D q) {
	return sqrt((p.x - q.x) * (p.x - q.x) +
				(p.y - q.y) * (p.y - q.y) +
				(p.z - q.z) * (p.z - q.z));
}

Pt2D matrix_mul_pt_2d(double m[][2], Pt2D p) {
	return {m[0][0] * p.x + m[0][1] * p.y,
			m[1][0] * p.x + m[1][1] * p.y};
}

Pt3D matrix_mul_pt_3d(double m[][3], Pt3D p) {
	return {m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z,
			m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z,
			m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z};
}

double det_2d(double m[][2]) {
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

double det_3d(double m[][3]) {
	return m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[3][0] + m[1][0] * m[2][1] * m[0][2]
		 - m[0][2] * m[1][1] * m[2][0] - m[0][1] * m[1][0] * m[2][2] - m[0][0] * m[1][2] * m[2][1];
}

void inverse_matrix_2d(double s[][2], double t[][2]) {
	double inverse_det = 1.0 / det_2d(s);
	t[0][0] = inverse_det * s[1][1];
	t[1][1] = inverse_det * s[0][0];
	t[0][1] = inverse_det * (-1) * s[0][1];
	t[1][0] = inverse_det * (-1) * s[1][0];
}

void inverse_matrix_3d(double s[3][3], double t[][3]) {
	double inverse_det = 1.0 / det_3d(s);
	double t00[2][2] = {s[1][1], s[1][2], s[2][1], s[2][2]};
	t[0][0] = inverse_det * det_2d(t00);
	double t01[2][2] = {s[0][2], s[0][1], s[2][2], s[2][1]};
	t[0][1] = inverse_det * det_2d(t01);
	double t02[2][2] = {s[0][1], s[0][2], s[1][1], s[1][2]};
	t[0][2] = inverse_det * det_2d(t02);
	double t10[2][2] = {s[1][2], s[1][0], s[2][2], s[2][0]};
	t[1][0] = inverse_det * det_2d(t10);
	double t11[2][2] = {s[0][0], s[0][2], s[2][0], s[2][2]};
	t[1][1] = inverse_det * det_2d(t11);
	double t12[2][2] = {s[0][2], s[0][0], s[1][2], s[1][0]};
	t[1][2] = inverse_det * det_2d(t12);
	double t20[2][2] = {s[1][0], s[1][1], s[2][0], s[2][1]};
	t[2][0] = inverse_det * det_2d(t20);
	double t21[2][2] = {s[0][1], s[0][0], s[2][1], s[2][0]};
	t[2][1] = inverse_det * det_2d(t21);
	double t22[2][2] = {s[0][0], s[0][1], s[1][0], s[1][1]};
	t[2][2] = inverse_det * det_2d(t22);
}

Pt2D circumcenter_2d(Pt2D a, Pt2D b, Pt2D c) {
	Pt2D ab = {b.x - a.x, b.y - a.y};
	Pt2D d = {(a.x + b.x) * 0.5, (a.y + b.y) * 0.5};
	Pt2D ac = {c.x - a.x, c.y - a.y};
	Pt2D e = {(a.x + c.x) * 0.5, (a.y + c.y) * 0.5};
	double m[2][2] = {ab.x, ab.y, ac.x, ac.y};
	double inv_m[2][2] = {0.0};
	inverse_matrix_2d(m, inv_m);
	return matrix_mul_pt_2d(inv_m, {
		ab.x * d.x + ab.y * d.y,
		ac.x * e.x + ac.y * e.y});
}

Pt3D circumcenter_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d) {
	Pt3D ab = {b.x - a.x, b.y - a.y, b.z - a.z};
	Pt3D e = {(a.x + b.x) * 0.5, (a.y + b.y) * 0.5, (a.z + b.z) * 0.5};
	Pt3D ac = {c.x - a.x, c.y - a.y, c.z - a.z};
	Pt3D f = {(a.x + c.x) * 0.5, (a.y + c.y) * 0.5, (a.z + c.z) * 0.5};
	Pt3D ad = {d.x - a.x, d.y - a.y, d.z - a.z};
	Pt3D g = {(a.x + d.x) * 0.5, (a.y + d.y) * 0.5, (a.z + d.z) * 0.5};
	double m[3][3] = {ab.x, ab.y, ab.z, ac.x, ac.y, ac.z, ad.x, ad.y, ad.z};
	double inv_m[3][3] = {0.0};
	inverse_matrix_3d(m, inv_m);
	return matrix_mul_pt_3d(inv_m, {
		ab.x * e.x + ab.y * e.y + ab.z * e.z,
		ac.x * f.x + ac.y * f.y + ac.z * f.z,
		ad.x * g.x + ad.y * g.y + ad.z * g.z});
}

int main(int argc, char** argv) {
	Pt2D a = {0, 0};
	Pt2D b = {8, 0};
	Pt2D c = {5, 6};
	Pt2D o1 = circumcenter_2d(a, b, c);
	cout << "(" << o1.x << ", " << o1.y << ")" << endl;
	cout << eucl_dist(o1, a) << endl;
	cout << eucl_dist(o1, b) << endl;
	cout << eucl_dist(o1, c) << endl;

	Pt3D p = {0, 0, 0};
	Pt3D q = {8, 0, 0};
	Pt3D r = {5, 6, 0};
	Pt3D s = {2, 2, 5};
	Pt3D o2 = circumcenter_3d(p, q, r, s);
	cout << "(" << o2.x << ", " << o2.y << ", " << o2.z << ")" << endl;
	cout << eucl_dist(o2, p) << endl;
	cout << eucl_dist(o2, q) << endl;
	cout << eucl_dist(o2, r) << endl;
	cout << eucl_dist(o2, s) << endl;
}