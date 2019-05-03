#include "point.h"
#include <iostream>
#include <cmath>

void print_pt(Pt2D* p) {
	std::cout << "(" << p->x << ", " << p->y << ")";
}

void print_pt(Pt3D* p) {
	std::cout << "(" << p->x << ", " << p->y << ", " << p->z << ")";
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