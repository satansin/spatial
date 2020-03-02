#ifndef __POINT_H
#define __POINT_H

#include <iostream>
#include <cmath>

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
	const Pt3D operator-(const Pt3D& q) const {
		return { x - q.x, y - q.y, z - q.z };
	}
	const Pt3D operator+(const Pt3D& q) const {
		return { x + q.x, y + q.y, z + q.z };
	}
	const Pt3D operator*(double scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}
	const Pt3D operator/(double scalar) const {
		return { x / scalar, y / scalar, z / scalar };
	}
	double mode() const {
		return sqrt(this->sq_mode());
	}
	double sq_mode() const {
		return (x * x + y * y + z * z);
	}
};

inline double sq(double d) {
	return (d * d);
}

inline Pt3D middle_pt(const Pt3D* p, const Pt3D* q) {
	Pt3D mid;
	mid.x = (p->x + q->x) * 0.5;
	mid.y = (p->y + q->y) * 0.5;
	mid.z = (p->z + q->z) * 0.5;
	return mid;
}

inline void print_pt(const Pt2D* p) {
	std::cout << "(" << p->x << ", " << p->y << ")";
}

inline void print_pt(const Pt3D* p) {
	std::cout << "(" << p->x << ", " << p->y << ", " << p->z << ")";
}

inline double sq_dist(const double* p, const double* q, int dim = 3) {
	double ret = 0.0;
	for (int i = 0; i < dim; i++) {
		ret += ((p[i] - q[i]) * (p[i] - q[i]));
	}
	return ret;
}

inline double eucl_dist(const double* p, const double* q, int dim = 3) {
	return sqrt(sq_dist(p, q, dim));
}

inline double sq_dist(const float* p, const float* q, int dim = 3) {
	double ret = 0.0;
	for (int i = 0; i < dim; i++) {
		ret += ((p[i] - q[i]) * (p[i] - q[i]));
	}
	return ret;
}

inline double eucl_dist(const float* p, const float* q, int dim = 3) {
	return sqrt(sq_dist(p, q, dim));
}

inline double sq_dist(const Pt3D* p, const Pt3D* q) {
	return ((p->x - q->x) * (p->x - q->x) +
			(p->y - q->y) * (p->y - q->y) +
			(p->z - q->z) * (p->z - q->z));
}

inline double eucl_dist(const Pt2D* p, const Pt2D* q) {
	return sqrt((p->x - q->x) * (p->x - q->x) +
				(p->y - q->y) * (p->y - q->y));
}

inline double eucl_dist(const Pt3D* p, const Pt3D* q) {
	return sqrt(sq_dist(p, q));
}

inline double dot_prd(const Pt3D* a, const Pt3D* b) {
	return (a->x * b->x + a->y * b->y + a->z * b->z);
}

inline const Pt3D cross_prd(const Pt3D* a, const Pt3D* b) {
	return {a->y * b->z - a->z * b->y,
			a->z * b->x - a->x * b->z,
			a->x * b->y - a->y * b->x};
}

#endif