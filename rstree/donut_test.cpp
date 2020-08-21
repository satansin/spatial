#include <iostream>
#include <ctime>
#include <cstdlib>
extern "C" {
#include "donut.h"
}
using namespace std;

inline float random_float(float max) {
	return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max));
}

int main(int argc, char** argv) {
	srand(time(NULL));

	Box box;
	box.a[0] = 420;
	box.a[1] = 339;
	box.a[2] = 145;
	box.b[0] = 549;
	box.b[1] = 450;
	box.b[2] = 359;

	Circle cir;
	cir.o[0] = 500;
	cir.o[1] = 500;
	cir.o[2] = 500;
	cir.r = 150;
	cir.n[0] = 12;
	cir.n[1] = 17;
	cir.n[2] = -7;

	pre_compute(&cir);
	// double min_dist = min_dist_donut(&box, &cir);
	// cout << "Min dist: " << min_dist << endl;
}

// void f() {

//     double min_dist_sq = numeric_limits<double>::max();
//     double max_dist_sq = 0.0;
//     double tmp_dist_sq;
//     Pt3D min_pt, max_pt;
//     double intv = 0.01;

//     double x, y, z;

//     for (x = a[0]; x <= b[0]; x += (b[0] - a[0])) {
//         for (y = a[1]; y <= b[1]; y += intv) {
//             for (z = a[2]; z <= b[2]; z += intv) {
//                 Pt3D tmp_pt = {x, y, z};
//                 tmp_dist_sq = donut_dist_sq(&tmp_pt, &o, &n, r, sq_r);
//                 if (tmp_dist_sq < min_dist_sq) {
//                     min_dist_sq = tmp_dist_sq;
//                     min_pt = tmp_pt;
//                 }
//                 if (tmp_dist_sq > max_dist_sq) {
//                     max_dist_sq = tmp_dist_sq;
//                     max_pt = tmp_pt;
//                 }
//             }
//         }
//     }

//     for (x = a[0]; x <= b[0]; x += intv) {
//         for (y = a[1]; y <= b[1]; y += (b[1] - a[1])) {
//             for (z = a[2]; z <= b[2]; z += intv) {
//                 Pt3D tmp_pt = {x, y, z};
//                 tmp_dist_sq = donut_dist_sq(&tmp_pt, &o, &n, r, sq_r);
//                 if (tmp_dist_sq < min_dist_sq) {
//                     min_dist_sq = tmp_dist_sq;
//                     min_pt = tmp_pt;
//                 }
//                 if (tmp_dist_sq > max_dist_sq) {
//                     max_dist_sq = tmp_dist_sq;
//                     max_pt = tmp_pt;
//                 }
//             }
//         }
//     }

//     for (x = a[0]; x <= b[0]; x += intv) {
//         for (y = a[1]; y <= b[1]; y += intv) {
//             for (z = a[2]; z <= b[2]; z += (b[2] - a[2])) {
//                 Pt3D tmp_pt = {x, y, z};
//                 tmp_dist_sq = donut_dist_sq(&tmp_pt, &o, &n, r, sq_r);
//                 if (tmp_dist_sq < min_dist_sq) {
//                     min_dist_sq = tmp_dist_sq;
//                     min_pt = tmp_pt;
//                 }
//                 if (tmp_dist_sq > max_dist_sq) {
//                     max_dist_sq = tmp_dist_sq;
//                     max_pt = tmp_pt;
//                 }
//             }
//         }
//     }

//     cout << "Min pt: (" << min_pt.x << ", " << min_pt.y << ", " << min_pt.z << ")" << endl;
//     cout << "Min dist: " << sqrt(min_dist_sq) << endl;
//     cout << "Max pt: (" << max_pt.x << ", " << max_pt.y << ", " << max_pt.z << ")" << endl;
//     cout << "Max dist: " << sqrt(max_dist_sq) << endl;

// }