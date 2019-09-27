#include "point.h"
#include <iostream>
#include <cmath>

void print_pt(Pt2D* p) {
	std::cout << "(" << p->x << ", " << p->y << ")";
}

void print_pt(Pt3D* p) {
	std::cout << "(" << p->x << ", " << p->y << ", " << p->z << ")";
}

float sq_dist(const float* p, const float* q, int dim) {
	double ret = 0.0;
	for (int i = 0; i < dim; i++) {
		ret += ((p[i] - q[i]) * (p[i] - q[i]));
	}
	return ret;
}

float eucl_dist(const float* p, const float* q, int dim) {
	return sqrt(sq_dist(p, q, dim));
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

double dot_prd(Pt3D a, Pt3D b) {
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

Pt3D cross_prd(Pt3D a, Pt3D b) {
	return {a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x};
}