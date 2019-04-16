//#include "trimesh2/include/TriMesh.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
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

void print_pt3d(Pt3D* p) {
	cout << "(" << p->x << ", " << p->y << ", " << p->z << ")";
}

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

Pt3D circumcenter_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d, double& ret_radi) {
	Pt3D ab = {b.x - a.x, b.y - a.y, b.z - a.z};
	Pt3D e = {(a.x + b.x) * 0.5, (a.y + b.y) * 0.5, (a.z + b.z) * 0.5};
	Pt3D ac = {c.x - a.x, c.y - a.y, c.z - a.z};
	Pt3D f = {(a.x + c.x) * 0.5, (a.y + c.y) * 0.5, (a.z + c.z) * 0.5};
	Pt3D ad = {d.x - a.x, d.y - a.y, d.z - a.z};
	Pt3D g = {(a.x + d.x) * 0.5, (a.y + d.y) * 0.5, (a.z + d.z) * 0.5};
	double m[3][3] = {ab.x, ab.y, ab.z, ac.x, ac.y, ac.z, ad.x, ad.y, ad.z};
	double inv_m[3][3] = {0.0};
	inverse_matrix_3d(m, inv_m);
	Pt3D center = matrix_mul_pt_3d(inv_m, {
		ab.x * e.x + ab.y * e.y + ab.z * e.z,
		ac.x * f.x + ac.y * f.y + ac.z * f.z,
		ad.x * g.x + ad.y * g.y + ad.z * g.z});
	ret_radi = eucl_dist(a, center);
	cout << eucl_dist(b, center) << endl;
	cout << eucl_dist(c, center) << endl;
	cout << eucl_dist(d, center) << endl;
	return center;
}

Pt3D sum_pt(Pt3D a, Pt3D b) {
	return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Pt3D scale_pt(double scalar, Pt3D p) {
	return {p.x * scalar, p.y * scalar, p.z * scalar};
}

double dot_prd(Pt3D a, Pt3D b) {
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

Pt3D cross_prd(Pt3D a, Pt3D b) {
	return {a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x};
}

double sq_mode(Pt3D p) {
	return (p.x * p.x + p.y * p.y + p.z * p.z);
}

double mode(Pt3D p) {
	return sqrt(sq_mode(p));
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

long global_counter = 0;
void next_glb_idx(long total) {
	long base = total / 100;
	if (global_counter % base == 0) {
		cout << (global_counter / base) << "%...\n";
	}
	global_counter++;
}

void combination_tetra(long res_size, vector<double> radi, Pt3D list[], Pt3D* tetra[], int start, int end, int index) {
	if (index == 4) {
		radi.push_back(circumradi_3d(*tetra[0], *tetra[1], *tetra[2], *tetra[3]));
		next_glb_idx(res_size);

		// print_pt3d(tetra[0]);
		// cout << endl;
		// print_pt3d(tetra[1]);
		// cout << endl;
		// print_pt3d(tetra[2]);
		// cout << endl;
		// print_pt3d(tetra[3]);
		// cout << radius << endl;
		return;
	}

	for (int i = start; i <= end && end - i >= 3 - index; i++) {
		tetra[index] = &list[i];
		combination_tetra(res_size, radi, list, tetra, i + 1, end, index + 1);
	}
}

int main(int argc, char **argv) {
	ifstream input_ply;
	input_ply.open("bun_zipper_res4.ply");
	string line;
	int i;
	for (i = 0; i < 4; i++) {
		getline(input_ply, line);
	}
	int size = stoi(line.substr(line.find_last_of(' ')));
	cout << size << endl;

	if (argc > 1) {
		size = atoi(argv[1]);
		cout << "size adjusted to " << size << endl;
	}

	for (i = 0; i < 8; i++) {
		getline(input_ply, line);
	}

	Pt3D point_list[size];
	double dummy;
	for (i = 0; i < size; i++) {
		input_ply >> point_list[i].x;
		input_ply >> point_list[i].y;
		input_ply >> point_list[i].z;
		input_ply >> dummy;
		input_ply >> dummy;
	}
	input_ply.close();

	long perm_size = size;
	perm_size *= (size - 1) * (size - 2) * (size - 3) / 24;
	cout << perm_size << endl;

	vector<double> radiis;
	Pt3D* tetra[4];
	combination_tetra(perm_size, radiis, point_list, tetra, 0, size - 1, 0);

	// double r;
	// Pt3D test = circumcenter_3d(point_list[5], point_list[7], point_list[8], point_list[9], r);
	// print_pt3d(&test);
	// cout << endl << r << endl << endl;
	// print_pt3d(&point_list[5]);
	// cout << endl;
	// print_pt3d(&point_list[7]);
	// cout << endl;
	// print_pt3d(&point_list[8]);
	// cout << endl;
	// print_pt3d(&point_list[9]);
	// cout << endl;
	// cout << circumradi_3d(point_list[5], point_list[7], point_list[8], point_list[9]) << endl;

	//// calling library
	// const char *filename = "bun_zipper_res4.ply";
	// TriMesh *m = TriMesh::read(filename);
	// if (!m) {
	// 	exit(1);
	// }
}
