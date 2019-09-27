#ifndef __TRANS_H
#define __TRANS_H

#include "point.h"

struct Trans {
	double r11, r12, r13;
	double r21, r22, r23;
	double r31, r32, r33;
	double tx, ty, tz;
};

void print_trans(const Trans* t);

Trans* create_trans(double al, double be, double ga, double tx, double ty, double tz);

Pt3D trans_pt(const Trans* t, Pt3D p);

// Trans cal_trans(Pt3D* q1, Pt3D* q2, Pt3D* q3, Pt3D* s1, Pt3D* s2, Pt3D* s3);

Trans cal_trans(const Pt3D* q, const Pt3D* p, int num);

#endif