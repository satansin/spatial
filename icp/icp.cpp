#include "point.h"
#include <iostream>
#include <cstdlib>
#include <cstdarg>
#include <cmath>
#include <Dense>

using namespace std;
using namespace Eigen;

#define MAX_COM_LENGTH 100

void invoke_system(const char *fmt, ...)
{
	char com[MAX_COM_LENGTH];
	va_list args;
	va_start(args, fmt);
	vsnprintf(com, sizeof(com), fmt, args);
	va_end(args);

	cout << com << endl;
	int rc = system(com);
	if (rc != 0)
	{
		exit(1);
	}
}

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

Pt3D trans_pt(const Trans* t, const Pt3D* p) {
	Pt3D t_p = {0.0};
	t_p.x = t->r11 * p->x + t->r12 * p->y + t->r13 * p->z + t->tx;
	t_p.y = t->r21 * p->x + t->r22 * p->y + t->r23 * p->z + t->ty;
	t_p.z = t->r31 * p->x + t->r32 * p->y + t->r33 * p->z + t->tz;
	return t_p;
}

int main(int argc, char** argv) {

	// test code
	double al = 0;
	double be = 0;
	double ga = 0.5236;
	double tx = 1.1;
	double ty = 1.5;
	double tz = 2.2;
	Trans* tr = create_trans(al, be, ga, tx, ty, tz);
	cout << "The transformation matrix is:" << endl;
	cout << tr->r11 << ", " << tr->r12 << ", " << tr->r13 << ", " << tr->tx << endl;
	cout << tr->r21 << ", " << tr->r22 << ", " << tr->r23 << ", " << tr->ty << endl;
	cout << tr->r31 << ", " << tr->r32 << ", " << tr->r33 << ", " << tr->tz << endl;
	cout << 0 << ", " << 0 << ", " << 0 << ", " << 1 << endl << endl;

	system("./mesh_align 1.ply 2.ply");

	// TODO: read the output...

}
