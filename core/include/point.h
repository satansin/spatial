#ifndef __POINT_H
#define __POINT_H

#include "TriMesh.h"

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
	Pt3D operator-(const Pt3D& q) const {
		return { x - q.x, y - q.y, z - q.z };
	}
	Pt3D operator+(const Pt3D& q) const {
		return { x + q.x, y + q.y, z + q.z };
	}
	Pt3D operator*(double scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}
	Pt3D operator/(double scalar) const {
		return { x / scalar, y / scalar, z / scalar };
	}
	double mode() const {
		return sqrt(this->sq_mode());
	}
	double sq_mode() const {
		return (x * x + y * y + z * z);
	}
};

inline Pt3D pt(trimesh::point p) {
	return {p[0], p[1], p[2]};
}

inline Pt3D get_pt(const trimesh::TriMesh *mesh, int id) {
	return pt(mesh->vertices[id]);
}

inline Pt3D middle_pt(Pt3D p, Pt3D q) {
	return { (p.x + q.x) * 0.5, (p.y + q.y) * 0.5, (p.z + q.z ) * 0.5 };
}

void print_pt(Pt2D* p);

void print_pt(Pt3D* p);

float sq_dist(const float* p, const float* q, int dim);

float eucl_dist(const float* p, const float* q, int dim = 3);

double eucl_dist(Pt2D p, Pt2D q);

double eucl_dist(Pt3D p, Pt3D q);

double dot_prd(Pt3D a, Pt3D b);

Pt3D cross_prd(Pt3D a, Pt3D b);

#endif