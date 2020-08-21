#ifndef __POINT_H
#define __POINT_H

#include <iostream>
#include <cmath>

static const double PI = 3.1415926535897932384626;
static const double TWO_PI = PI * 2.0;
static const double HALF_PI = PI * 0.5;

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
	const Pt3D operator-() const {
		return { -x, -y, -z };
	}
	double mode() const {
		return sqrt(this->sq_mode());
	}
	double sq_mode() const {
		return (x * x + y * y + z * z);
	}
	void unitify() {
		double norm = this->mode();
		if (norm != 0.0) {
			x /= norm;
			y /= norm;
			z /= norm;
		}
	}
};

inline double sq(double d) {
	return (d * d);
}

inline void middle_pt(const Pt3D* p, const Pt3D* q, Pt3D& ret) {
	ret.x = (p->x + q->x) * 0.5;
	ret.y = (p->y + q->y) * 0.5;
	ret.z = (p->z + q->z) * 0.5;
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

inline void cross_prd(const Pt3D* a, const Pt3D* b, Pt3D& ret) {
	ret.x = a->y * b->z - a->z * b->y;
	ret.y = a->z * b->x - a->x * b->z;
	ret.z = a->x * b->y - a->y * b->x;
}

inline double cos_theta(const Pt3D* pa, const Pt3D* pb) {
	return dot_prd(pa, pb) / (pa->mode() * pb->mode());
}

inline bool get_perp_unit(const Pt3D* p, Pt3D& ret) {
	if (p->x != 0.0) {
		ret.x = - (p->y + p->z) / p->x;
		ret.y = 1.0;
		ret.z = 1.0;
	} else if (p->y != 0.0) {
		ret.x = 1.0;
		ret.y = - (p->x + p->z) / p->y;
		ret.z = 1.0;
	} else if (p->z != 0.0) {
		ret.x = 1.0;
		ret.y = 1.0;
		ret.z = - (p->x + p->y) / p->z;
	} else {
		return false;
	}
	ret.unitify();
	return true;
}

#endif