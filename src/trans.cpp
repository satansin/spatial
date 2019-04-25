#include "point.h"
#include <iostream>
#include <cmath>
using namespace std;

struct Trans {
	double r11;
	double r12;
	double r13;
	double r21;
	double r22;
	double r23;
	double r31;
	double r32;
	double r33;
	double tx;
	double ty;
	double tz;
};

Trans* create_trans(double al, double be, double ga, double tx, double ty, double tz) {
	Trans* ret = new Trans;
	double cal = cos(al);
	double sal = sin(al);
	double cbe = cos(be);
	double sbe = sin(be);
	double cga = cos(ga);
	double sga = sin(ga);
	ret->r11 = cal * cbe;
	ret->r12 = cal * sbe * sga - sal * cga;
	ret->r13 = cal * sbe * cga + sal * sga;
	ret->r21 = sal * cbe;
	ret->r22 = sal * sbe * sga + cal * cga;
	ret->r23 = sal * sbe * cga - cal * sga;
	ret->r31 = sbe * (-1);
	ret->r32 = cbe * sga;
	ret->r33 = cbe * cga;
	ret->tx = tx;
	ret->ty = ty;
	ret->tz = tz;
	return ret;
}

void trans_pt(Trans* t, Pt3D* p) {
	Pt3D t_p = *p;
	p->x = t->r11 * t_p.x + t->r12 * t_p.y + t->r13 * t_p.z + t->tx;
	p->y = t->r21 * t_p.x + t->r22 * t_p.y + t->r23 * t_p.z + t->ty;
	p->z = t->r31 * t_p.x + t->r32 * t_p.y + t->r33 * t_p.z + t->tz;
}

int main(int argc, char** argv) {
	double al = 0.8;
	double be = 0.3;
	double ga = 0.4;
	double tx = 1.1;
	double ty = 1.5;
	double tz = 2.2;
	Trans* tr = create_trans(al, be, ga, tx, ty, tz);

	Pt3D test1 = {3.0, 4.0, 8.0};
	Pt3D test2 = {9.0, 1.0, -3.0};
	Pt3D test3 = {7.0, 10.0, 2.0};
	cout << "Before transformation: ";
	print_pt3d(&test1);
	cout << endl;
	print_pt3d(&test2);
	cout << endl;
	print_pt3d(&test3);
	cout << endl;

	trans_pt(tr, &test);

	cout << "After transformation: ";
	print_pt3d(&test);
	cout << endl;

	delete tr;
}
