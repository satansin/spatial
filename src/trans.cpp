#include "point.h"
#include <iostream>
#include <cmath>
#include <Dense>
using namespace std;
using namespace Eigen;

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

// struct Trans {
// 	Matrix3d R;
// 	Vector3d t;
// };

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

Trans cal_trans(Pt3D* q1, Pt3D* q2, Pt3D* q3, Pt3D* s1, Pt3D* s2, Pt3D* s3) {
	// column vectors for si
	Vector3d cv_s1(s1->x, s1->y, s1->z);
	Vector3d cv_s2(s2->x, s2->y, s2->z);
	Vector3d cv_s3(s3->x, s3->y, s3->z);

	// mean of si
	Vector3d cv_smean = (cv_s1 + cv_s2 + cv_s3) / 3.0;

	// transform si into relative coordinates
	Vector3d cv_s1_prime = cv_s1 - cv_smean;
	Vector3d cv_s2_prime = cv_s2 - cv_smean;
	Vector3d cv_s3_prime = cv_s3 - cv_smean;

	// row vectors for qi
	RowVector3d rv_q1(q1->x, q1->y, q1->z);
	RowVector3d rv_q2(q2->x, q2->y, q2->z);
	RowVector3d rv_q3(q3->x, q3->y, q3->z);

	// mean of qi
	RowVector3d rv_qmean = (rv_q1 + rv_q2 + rv_q3) / 3.0;

	// transform qi into relative coordinates
	RowVector3d rv_q1_prime = rv_q1 - rv_qmean;
	RowVector3d rv_q2_prime = rv_q2 - rv_qmean;
	RowVector3d rv_q3_prime = rv_q3 - rv_qmean;

	// sum of outer products
	Matrix3d M = cv_s1_prime * rv_q1_prime + cv_s2_prime * rv_q2_prime + cv_s3_prime * rv_q3_prime;

	// calculate Q = M^T * M
	Matrix3d Q = M.transpose() * M;
	cout << Q << endl;

	// perform eigen decomposition on Q
	SelfAdjointEigenSolver<Matrix3d> solver(Q);
	if (solver.info() != Success) {
		cerr << "Error in eigen decomposition" << endl;
		exit(1);
	}

	// eigenvalues and diagonal matrix^(-0.5)
	auto eigenvalues = solver.eigenvalues();
	// cout << eigenvalues << endl;
	double l1 = eigenvalues(0, 0);
	double l2 = eigenvalues(1, 0);
	double l3 = eigenvalues(2, 0);
	Matrix3d L_neg_0pt5;
	L_neg_0pt5 <<
		1.0/sqrt(l1),            0,            0,
		           0, 1.0/sqrt(l2),            0,
		           0,            0, 1.0/sqrt(l3);

	// eigenvectors and Q^(-0.5)
	auto eigenvectors = solver.eigenvectors();
	// cout << eigenvectors << endl;
	Matrix3d Q_neg_0pt5 = eigenvectors * L_neg_0pt5 * eigenvectors.transpose();

	// rotation matrix
	Matrix3d R = M * Q_neg_0pt5;
	cout << "R =" << endl << R << endl;

	// translation
	Vector3d t = cv_smean - (R * rv_qmean.transpose());
	cout << "t =" << endl << t << endl;
	return Trans{0.0};
}

int main(int argc, char** argv) {
	double al = 0.8;
	double be = 0.7;
	double ga = 0.4;
	double tx = 1.1;
	double ty = 1.5;
	double tz = 2.2;
	Trans* tr = create_trans(al, be, ga, tx, ty, tz);
	cout << "The transformation matrix is:" << endl;
	cout << tr->r11 << ", " << tr->r12 << ", " << tr->r13 << ", " << tr->tx << endl;
	cout << tr->r21 << ", " << tr->r22 << ", " << tr->r23 << ", " << tr->ty << endl;
	cout << tr->r31 << ", " << tr->r32 << ", " << tr->r33 << ", " << tr->tz << endl;
	cout << 0 << ", " << 0 << ", " << 0 << ", " << 1 << endl << endl;

	Pt3D test1 = {3.0, 4.0, 8.0};
	Pt3D test2 = {9.0, 1.0, -3.0};
	Pt3D test3 = {7.0, 10.0, 2.0};

	cout << "Before transformation:\n";
	print_pt3d(&test1);
	cout << endl;
	print_pt3d(&test2);
	cout << endl;
	print_pt3d(&test3);
	cout << endl;

	Pt3D original_mean;
	original_mean.x = (test1.x + test2.x + test3.x) / 3.0;
	original_mean.y = (test1.y + test2.y + test3.y) / 3.0;
	original_mean.z = (test1.z + test2.z + test3.z) / 3.0;
	cout << "mean = ";
	print_pt3d(&original_mean);
	cout << endl << endl;

	Pt3D t_test1 = trans_pt(tr, &test1);
	Pt3D t_test2 = trans_pt(tr, &test2);
	Pt3D t_test3 = trans_pt(tr, &test3);

	cout << "After transformation:\n";
	print_pt3d(&t_test1);
	cout << endl;
	print_pt3d(&t_test2);
	cout << endl;
	print_pt3d(&t_test3);
	cout << endl;

	Pt3D t_mean;
	t_mean.x = (t_test1.x + t_test2.x + t_test3.x) / 3.0;
	t_mean.y = (t_test1.y + t_test2.y + t_test3.y) / 3.0;
	t_mean.z = (t_test1.z + t_test2.z + t_test3.z) / 3.0;
	cout << "mean = ";
	print_pt3d(&t_mean);
	cout << endl << endl;

	delete tr;

	cal_trans(&test1, &test2, &test3, &t_test1, &t_test2, &t_test3);
}
