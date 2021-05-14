#ifndef __MATHUTIL_H_
#define __MATHUTIL_H_

#include "util.h"

#include "Vec.h"
#include "XForm.h"

#include <Eigen/Dense>

#include <cmath>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace trimesh;

#define INFTY 3.3e33

#define NUM_TEST 100

#define APPROX_ZERO 1e-6

inline int approx_sgn(double a, double zero = APPROX_ZERO)
{
	if (a > zero) {
		return 1;
	} else if (a < -zero) {
		return -1;
	} else {
		return 0;
	}
}

inline bool approx_eqt(double a, double b, double zero = APPROX_ZERO)
{
    return (abs(a - b) <= zero);
}

inline bool approx_eqt(point2 a, point2 b, double zero = APPROX_ZERO)
{
    return (approx_eqt(a[0], b[0], zero) && approx_eqt(a[1], b[1], zero));
}

inline bool approx_eqt(point a, point b, double zero = APPROX_ZERO)
{
    return (approx_eqt(a[0], b[0], zero) && approx_eqt(a[1], b[1], zero) && approx_eqt(a[2], b[2], zero));
}

inline float ran_f_0_1()
{
	return ((float) rand() / RAND_MAX);
}

// return a random float in (-scaling, scaling)
inline float ran_f(float scaling = 1.0)
{
    return (2.0 * scaling * (float) rand() / RAND_MAX - scaling);
}

// return a random point in (-1, 1)
inline point ran_p(float scaling = 1.0)
{
    return point(ran_f(scaling), ran_f(scaling), ran_f(scaling));
}

inline double sq(double v)
{
	return v*v;
}

inline void print_matrix(const double (&A)[3][3])
{
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cout << " " << A[i][j];
        }
        cout << endl;
    }
}

inline double determ(const double (&A)[2][2])
{
    return (A[0][0] * A[1][1] - A[0][1] * A[1][0]);
}

inline double determ(const double (&A)[3][3])
{
	return A[0][0] * (A[1][1] * A[2][2] - A[1][2] * A[2][1]) +
	       A[0][1] * (A[1][2] * A[2][0] - A[1][0] * A[2][2]) +
	       A[0][2] * (A[1][0] * A[2][1] - A[1][1] * A[2][0]);
}

inline double determ(const double (&A)[4][4])
{
    Eigen::Matrix4d M;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            M(i, j) = A[i][j];
        }
    }
    return M.determinant();
}

// Function to get adjoint of A[N][N] in adj[N][N].
inline void adjoint(const double (&A)[3][3], double adj[3][3])
{
	adj[0][0] = A[1][1] * A[2][2] - A[1][2] * A[2][1];
	adj[0][1] = A[0][2] * A[2][1] - A[0][1] * A[2][2];
	adj[0][2] = A[0][1] * A[1][2] - A[0][2] * A[1][1];

	adj[1][0] = A[2][0] * A[1][2] - A[1][0] * A[2][2];
	adj[1][1] = A[0][0] * A[2][2] - A[0][2] * A[2][0];
	adj[1][2] = A[0][2] * A[1][0] - A[0][0] * A[1][2];

	adj[2][0] = A[1][0] * A[2][1] - A[1][1] * A[2][0];
	adj[2][1] = A[0][1] * A[2][0] - A[0][0] * A[2][1];
	adj[2][2] = A[0][0] * A[1][1] - A[0][1] * A[1][0];
}

inline bool inverse(const double (&A)[2][2], double inverse[2][2])
{
    double det_A = determ(A);
    if (det_A == 0.0) {
        cout << "Singular 2x2 matrix, can't find its inverse" << endl;
        return false;
    }

    inverse[0][0] =   A[1][1] / det_A;
    inverse[0][1] = - A[0][1] / det_A;
    inverse[1][0] = - A[1][0] / det_A;
    inverse[1][1] =   A[0][0] / det_A;

    return true;
}

inline bool inverse(const double (&A)[3][3], double inverse[3][3])
{
	double det_A = determ(A);
	if (det_A == 0.0) {
        cout << "Singular 3x3 matrix, can't find its inverse" << endl;
        return false;
	}

    // Find adjoint
    double adj[3][3];
    adjoint(A, adj);

    // Get inverse
    for (int i = 0; i < 3; i++) {
    	for (int j = 0; j < 3; j++) {
    		inverse[i][j] = adj[i][j] / det_A;
    	}
    }

    return true;
}


inline const xform to_xform_matrix(double yaw, double pitch, double roll, double x, double y, double z)
{
    float cy = cos(yaw);
    float sy = sin(yaw);
    float cp = cos(pitch);
    float sp = sin(pitch);
    float cr = cos(roll);
    float sr = sin(roll);

    return xform(
        cy*cp, cy*sp*sr-sy*cr, cy*sp*cr+sy*sr,   x,
        sy*cp, sy*sp*sr+cy*cr, sy*sp*cr-cy*sr,   y,
          -sp,          cp*sr,          cp*cr,   z,
          0.0,            0.0,            0.0, 1.0
    );
}

void show_yprxyz(xform& xf)
{

}


struct Quaternion
{
    double w, x, y, z;
};

Quaternion to_quaternion(double yaw, double pitch, double roll) // yaw (Z), pitch (Y), roll (X)
{
    // Abbreviations for the various angular functions
    double cy = cos(yaw * 0.5);
    double sy = sin(yaw * 0.5);
    double cp = cos(pitch * 0.5);
    double sp = sin(pitch * 0.5);
    double cr = cos(roll * 0.5);
    double sr = sin(roll * 0.5);

    Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;
}


inline void make_matrix(const point &a, const point & b, const point & c, double A[3][3])
{
    for (int i = 0; i < 3; i++) {
        A[i][0] = a[i];
        A[i][1] = b[i];
        A[i][2] = c[i];
    }
}

inline point2 matrix_prd(const double (&A)[2][2], const point2 &p)
{
    return point2(
        A[0][0] * p[0] + A[0][1] * p[1],
        A[1][0] * p[0] + A[1][1] * p[1]
    );
}

inline point matrix_prd(const double (&A)[3][3], const point &p)
{
    return point(
        A[0][0] * p[0] + A[0][1] * p[1] + A[0][2] * p[2],
        A[1][0] * p[0] + A[1][1] * p[1] + A[1][2] * p[2],
        A[2][0] * p[0] + A[2][1] * p[1] + A[2][2] * p[2]
    );
}

inline double sq_norm(const point &p)
{
    return (sq(p[0]) + sq(p[1]) + sq(p[2]));
}

inline double sq_dist(const point &p, const point & q)
{
    return (sq(p[0] - q[0]) + sq(p[1] - q[1]) + sq(p[2] - q[2]));
}

inline point normalize(const point &p, double to = 1.0)
{
    double length = sqrt(sq_norm(p));
    double ratio = to / length;
    return (ratio * p);
}

inline vec tri_norm(const point (&tv)[3])
{
	return normalize((tv[1] - tv[0]) CROSS (tv[2] - tv[0]));
}

bool check_same_side(const point &p, const point &q, const point &s, const point &t)
{
	const vec st = t - s;
	const point cp = (st CROSS (p - s));
	const point cq = (st CROSS (q - s));
	if ((cp DOT cq) >= 0) {
		return true;
	} else {
		return false;
	}
}

bool check_pt_inside_tri_2d(const point &q, const point (&tv)[3])
{
    const vec ab = tv[1] - tv[0];
    const vec ac = tv[2] - tv[0];
    const vec aq = q - tv[0];
    double U_1[2][2];
    const int comb[3][2] = { { 0, 1 }, { 0, 2 }, { 1, 2 } };
    for (int i = 0; i < 3; i++) {
        const int s1 = comb[i][0];
        const int s2 = comb[i][1];
        const double U[2][2] = { { ab[s1], ac[s1] }, { ab[s2], ac[s2] } };
        const point2 aq2(aq[s1], aq[s2]);
        if (!approx_eqt(point2(0, 0), aq2, 1e-9) && inverse(U, U_1)) {
            const point2 ret_params = matrix_prd(U_1, aq2);
            if (0 <= ret_params[0] && ret_params[0] <= 1 && 0 <= ret_params[1] && ret_params[1] <= 1 && ret_params[0] + ret_params[1] <= 1) {
                return true;
            } else {
                return false;
            }
        }
    }

    // // Below: a faster way
    // if (check_same_side(q, tv[0], tv[1], tv[2]) && check_same_side(q, tv[1], tv[0], tv[2]) && check_same_side(q, tv[2], tv[0], tv[1])) {
    // 	return true;
    // } else {
    // 	return false;
    // }
}

// Return the projection point from q to the plane of triangle tv
// let abc be the triangle tv, and let r be the return
// then ar = alpha * ab + beta * ac
// point get_proj_p2f(const point &q, const point (&tv)[3], double& alpha, double& beta)
point get_proj_p2f(const point &q, const point (&tv)[3], const vec &tn, double &sq_proj_dist)
{
    // // get projection of q to plane defined by tv[3]
    // // old method with alpha and beta parameters, depr
    // point ab = tv[1] - tv[0];
    // point ac = tv[2] - tv[0];
    // point cr = ab CROSS ac;
    // point aq = q - tv[0];
    // // cout << ab << endl;
    // // cout << ac << endl;
    // // cout << cr << endl;
    // // cout << aq << endl;
    // double U[3][3];
    // make_matrix(ab, ac, -cr, U);
    // // print_matrix(U);
    // double U_1[3][3];
    // bool inversed = inverse(U, U_1);
    // assert(inversed);
    // auto ret_params = matrix_prd(U_1, aq);
    // alpha = ret_params[0];
    // beta = ret_params[1];
    // return (alpha * ab + beta * ac + tv[0]);

    double gamma_p = tn[0] * (tv[0][0] - q[0]) + tn[1] * (tv[0][1] - q[1]) + tn[2] * (tv[0][2] - q[2]);
    double sq_norm_tn = sq_norm(tn);
    double gamma = gamma_p / sq_norm_tn;
    sq_proj_dist = gamma_p * gamma;
    return (q + gamma * tn);
}

// Return whether q's projection to the plane of triangle tv is inside tv
// proj is set to the projection point
// bool check_proj_inside_p2f(const point &q, const point (&tv)[3], point& proj, double& alpha, double& beta)
bool check_proj_inside_p2f(const point &q, const point (&tv)[3], const vec &tn, point& proj, double &sq_proj_dist, double current_sq_min = INFTY)
{
    // // old method with alpha and beta parameters, depr
    // proj = get_proj_p2f(q, tv, alpha, beta);
    // if (0.0 <= alpha && alpha <= 1.0 && 0.0 <= beta && beta <= 1.0 && alpha + beta <= 1.0) {
    //     return true;
    // } else {
    //     return false;
    // }

    proj = get_proj_p2f(q, tv, tn, sq_proj_dist);
    if (sq_proj_dist > current_sq_min) {
        return false;
    } else {
        return check_pt_inside_tri_2d(proj, tv);
    }
}

// Return the SQ dist between point q to line segment st
// let x be the nn
// then lambda is set such that sx = lambda * st
// lambda will be in [0, 1]
// lambda = 0 means x = s
// lambda  =1 means x = t
double sq_dist_p2l(const point &q, const point &s, const point &t, double& lambda)
{
    point st = t - s;
    point sq = q - s;
    lambda = (st DOT sq) / sq_norm(st);
    if (0 <= lambda && lambda <= 1) {
        return sq_norm(lambda * st - sq);
    } else if (lambda < 0) { // nn point is s
        lambda = 0;
        return sq_norm(sq);
    } else if (lambda > 1) { // nn point is t
        lambda = 1;
        return sq_norm(q - t);
    }
}

double sq_dist_p2l_bf(const point &q, const point &s, const point &t, double& lambda)
{
    double min_sq_dist = INFTY;
    const int num_steps = 10000;
    const double step = 1.0 / (double) num_steps;
    for (int i = 0; i <= num_steps; i++) {
        const double tmp_l = (double) i * step;
        const double tmp_sq_dist = sq_dist(q, s + tmp_l * (t - s));
        if (min_sq_dist > tmp_sq_dist) {
            min_sq_dist = tmp_sq_dist;
            lambda = tmp_l;
        }
    }
    return min_sq_dist;
}

// Return the SQ dist between point q to triangle tv[3]
// res is set to the NN point on the triangle to q
double sq_dist_p2f(const point &q, const point (&tv)[3], const vec tn, point& res)
{
    // get projection of q to plane defined by tv[3]
    // if projection inside triangle, then the projection distance must be the shortest
    point proj;
    // double alpha, beta;
    // if (check_proj_inside_p2f(q, tv, proj, alpha, beta)) {
    double sq_proj_dist;
    if (check_proj_inside_p2f(q, tv, tn, proj, sq_proj_dist)) {
        // proj inside triangle
        res = proj;
        return sq_proj_dist;
    }

    // then, the shortest distance must appear on edge
    double min_sq_dist = INFTY;
    const int comb[3][2] = { { 0, 1 }, { 0, 2 }, { 1, 2 } };
    for (int i = 0; i < 3; i++) {
        double tmp_l;
        const double tmp_sq_dist = sq_dist_p2l(q, tv[comb[i][0]], tv[comb[i][1]], tmp_l);
        if (min_sq_dist > tmp_sq_dist) {
            min_sq_dist = tmp_sq_dist;
            if (approx_eqt(tmp_l, 0)) {
                res = tv[comb[i][0]];
            } else if (approx_eqt(tmp_l, 1)) {
                res = tv[comb[i][1]];
            } else {
                res = tv[comb[i][0]] + tmp_l * (tv[comb[i][1]] - tv[comb[i][0]]);
            }
        }
    }

    return min_sq_dist;

    /* below: not working!! */

    // // find the farthest triangle vertex to q, and only check q to the line of the rest two points
    // double v_dists[3];
    // double v_dist_max = -1;
    // int v_dist_max_i = -1;
    // for (int i = 0; i < 3; i++) {
    //     v_dists[i] = sq_dist(q, tv[i]);
    //     if (v_dist_max < v_dists[i]) {
    //         v_dist_max = v_dists[i];
    //         v_dist_max_i = i;
    //     }
    // }
    // // cout << "max index: " << v_dist_max_i << endl;
    // int s, t;
    // switch(v_dist_max_i) {
    //     case 0: s = 1; t = 2; break;
    //     case 1: s = 0; t = 2; break;
    //     case 2: s = 0; t = 1; break;
    // }

    // point st = tv[t] - tv[s];
    // point sx = proj - tv[s];
    // double l = (st DOT sx) / (st DOT st);
    // // cout << "l: " << l << endl;

    // if (l <= 0.0) {
    //     assert(v_dists[s] <= v_dists[t]);
    //     // proj near s
    //     res = tv[s];
    //     return v_dists[s];
    // } else if (l >= 1.0) {
    //     assert(v_dists[s] >= v_dists[t]);
    //     // proj near t
    //     res = tv[t];
    //     return v_dists[t];
    // } else {
    //     res = l * st + tv[s];
    //     return sq_dist(q, res);
    // }
}

double sq_dist_p2f_bf(const point &q, const point (&tv)[3], point& res)
{
    double min_sq_dist = INFTY;
    const int num_steps = 1000;
    const double step = 1.0 / (double) num_steps;
    for (int i = 0; i <= num_steps; i++) {
        for (int j = 0; j <= num_steps - i; j++) {
            const double tmp_al = (double) i * step;
            const double tmp_be = (double) j * step;
            const point tmp_res = tv[0] + tmp_al * (tv[1] - tv[0]) + tmp_be * (tv[2] - tv[0]);
            const double tmp_sq_dist = sq_dist(q, tmp_res);
            if (min_sq_dist > tmp_sq_dist) {
                min_sq_dist = tmp_sq_dist;
                res = tmp_res;
            }
        }
    }
    return min_sq_dist;
}

// Return the SQ dist between line segment ab to cd
// let e and f be the nn points on ab and cd
// then alpha and beta are set such that ae = alpha * ab, cf = beta * cd
// both alpha and beta will be in [0, 1]
double sq_dist_e2e(const point &a, const point &b, const point &c, const point &d, double& alpha, double& beta)
{
    const vec cd = d - c;
    const vec ca = a - c;
    const vec ab = b - a;
    const double ab_dot_cd = (ab DOT cd);
    const double sq_norm_ab = sq_norm(ab);
    const double sq_norm_cd = sq_norm(cd);
    const double ab_dot_ca = (ab DOT ca);

    if (sq_norm_ab * sq_norm_cd - sq(ab_dot_cd) != 0.0) {
        beta = (sq_norm_ab * (cd DOT ca) - (ab_dot_ca) * (ab_dot_cd)) / (sq_norm_ab * sq_norm_cd - sq(ab_dot_cd));
        alpha = (beta * ab_dot_cd - ab_dot_ca) / sq_norm_ab;

        if (0 <= alpha && alpha <= 1 && 0 <= beta && beta <= 1) {
            return sq_norm(alpha * ab + a - beta * cd - c);
        }
    }

    double lambda;
    double tmp_sq_dist;
    double min_sq_dist = INFTY;

    tmp_sq_dist = sq_dist_p2l(a, c, d, lambda);
    if (tmp_sq_dist < min_sq_dist) {
        min_sq_dist = tmp_sq_dist;
        alpha = 0;
        beta = lambda;
    }

    tmp_sq_dist = sq_dist_p2l(b, c, d, lambda);
    if (tmp_sq_dist < min_sq_dist) {
        min_sq_dist = tmp_sq_dist;
        alpha = 1;
        beta = lambda;
    }
    
    tmp_sq_dist = sq_dist_p2l(c, a, b, lambda);
    if (tmp_sq_dist < min_sq_dist) {
        min_sq_dist = tmp_sq_dist;
        alpha = lambda;
        beta = 0;
    }
    
    tmp_sq_dist = sq_dist_p2l(d, a, b, lambda);
    if (tmp_sq_dist < min_sq_dist) {
        min_sq_dist = tmp_sq_dist;
        alpha = lambda;
        beta = 1;
    }

    return min_sq_dist;
}
bool sq_dist_e2e_bounded(const point &a, const point &b, const point &c, const point &d, double& alpha, double& beta, double &sq_dist_e2e, double current_sq_min = INFTY)
{
    const vec cd = d - c;
    const vec ca = a - c;
    const vec ab = b - a;
    const double ab_dot_cd = (ab DOT cd);
    const double sq_norm_ab = sq_norm(ab);
    const double sq_norm_cd = sq_norm(cd);
    const double ab_dot_ca = (ab DOT ca);

    if (sq_norm_ab * sq_norm_cd - sq(ab_dot_cd) != 0.0) {
        beta = (sq_norm_ab * (cd DOT ca) - (ab_dot_ca) * (ab_dot_cd)) / (sq_norm_ab * sq_norm_cd - sq(ab_dot_cd));
        alpha = (beta * ab_dot_cd - ab_dot_ca) / sq_norm_ab;

        double sq_dist_lb = sq_norm(alpha * ab + a - beta * cd - c);
        if (sq_dist_lb > current_sq_min) {
            return false;
        } else {
            if (0 <= alpha && alpha <= 1 && 0 <= beta && beta <= 1) {
                sq_dist_e2e = sq_dist_lb;
                return true;
            }
        }

    }

    double lambda;
    double tmp_sq_dist;
    double min_sq_dist = INFTY;;

    tmp_sq_dist = sq_dist_p2l(a, c, d, lambda);
    if (tmp_sq_dist < min_sq_dist) {
        min_sq_dist = tmp_sq_dist;
        alpha = 0;
        beta = lambda;
    }

    tmp_sq_dist = sq_dist_p2l(b, c, d, lambda);
    if (tmp_sq_dist < min_sq_dist) {
        min_sq_dist = tmp_sq_dist;
        alpha = 1;
        beta = lambda;
    }
    
    tmp_sq_dist = sq_dist_p2l(c, a, b, lambda);
    if (tmp_sq_dist < min_sq_dist) {
        min_sq_dist = tmp_sq_dist;
        alpha = lambda;
        beta = 0;
    }
    
    tmp_sq_dist = sq_dist_p2l(d, a, b, lambda);
    if (tmp_sq_dist < min_sq_dist) {
        min_sq_dist = tmp_sq_dist;
        alpha = lambda;
        beta = 1;
    }

    if (min_sq_dist < current_sq_min) {
        sq_dist_e2e = min_sq_dist;
        return true;
    } else {
        return false;
    }
}

double sq_dist_e2e_bf(const point &a, const point &b, const point &c, const point &d, double& alpha, double& beta)
{
    double min_sq_dist = INFTY;
    const int num_steps = 1000;
    const double step = 1.0 / (double) num_steps;
    for (int i = 0; i <= num_steps; i++) {
        for (int j = 0; j <= num_steps; j++) {
            const double tmp_al = (double) i * step;
            const double tmp_be = (double) j * step;
            const double tmp_sq_dist = sq_dist(a + tmp_al * (b - a), c + tmp_be * (d - c));
            if (min_sq_dist > tmp_sq_dist) {
                min_sq_dist = tmp_sq_dist;
                alpha = tmp_al;
                beta = tmp_be;
            }
        }
    }

    return min_sq_dist;
}

bool check_inters_line_line(const point &a, const point &b, const point &c, const point &d, point &inters)
{
    const vec ab = b - a;
    const vec dc = c - d;
    const vec ac = c - a;

    if (sq((ab DOT dc)) == (sq_norm(ab) * sq_norm(dc))) { // ab // cd
        if (sq_norm((ab CROSS ac)) > 1e-9) { // but not co-linear
            return false;
        } else { // co-linear
            for (int i = 0; i < 3; i++) {
                if (dc[i] == 0) {
                    continue;
                }
                const double al = ac[i] / dc[i];
                const double be = (c[i] - b[i]) / dc[i];
                if (al < 0) {
                    if (be < 0) {
                        return false;
                    } else {
                        inters = c;
                        return true;
                    }
                } else if (0 <= al && al <= 1) {
                    inters = a;
                    return true;
                } else {
                    if (be <= 1) {
                        inters = d;
                        return true;
                    } else {
                        return false;
                    }
                }
            }
        }
    }

    double U_1[2][2];

    const int comb[3][2] = { { 0, 1 }, { 0, 2 }, { 1, 2 } };
    for (int i = 0; i < 3; i++) {
        const int s1 = comb[i][0];
        const int s2 = comb[i][1];

        const double U[2][2] = { { ab[s1], dc[s1] }, { ab[s2], dc[s2] } };
        const point2 ac2(ac[s1], ac[s2]);
        if (inverse(U, U_1) && !approx_eqt(point2(0, 0), ac2, 1e-9)) {
            const point2 ret_params = matrix_prd(U_1, ac2);
            if (0 <= ret_params[0] && ret_params[0] <= 1 && 0 <= ret_params[1] && ret_params[1] <= 1) {
                inters = a + ret_params[0] * ab;
                return true;
            } else {
                return false;
            }
        }
    }
}

// require: all the 5 points are coplanar
bool check_inters_line_tri_2d(const point (&tv)[3], const point &lv1, const point &lv2, point& inters)
{
    if (check_pt_inside_tri_2d(lv1, tv)) {
        inters = lv1;
        return true;
    }
    if (check_pt_inside_tri_2d(lv2, tv)) {
        inters = lv2;
        return true;
    }

    const int comb[3][2] = { { 0, 1 }, { 0, 2 }, { 1, 2 } };
    for (int i = 0; i < 3; i++) {
        const int s1 = comb[i][0];
        const int s2 = comb[i][1];

        if (check_inters_line_line(tv[s1], tv[s2], lv1, lv2, inters)) {
            return true;
        }
    }

    return false;
}

// input: triangle abc (in order of tv), line segment de (in order of lv1, lv2)
// require: de not parallel to the plane defined by abc
// return: f (intersetion)
// return: alpha (df = alpha * de)
// return: beta, gamma (af = beta * ab + gamma * ac)
point inters_line_tri(const point (&tv)[3], const point &lv1, const point &lv2, double& alpha, double& beta, double& gamma)
{
    point de = lv2 - lv1;
    point ba = tv[0] - tv[1];
    point ca = tv[0] - tv[2];
    point da = tv[0] - lv1;
    double U[3][3], inv_U[3][3];
    make_matrix(de, ba, ca, U);
    bool inversed = inverse(U, inv_U);
    assert(inversed);

    auto params = matrix_prd(inv_U, da);
    alpha = params[0];
    beta = params[1];
    gamma = params[2];

    return (alpha * de + lv1);
}

bool check_inters_line_tri(const point (&tv)[3], const point &lv1, const point &lv2, point& inters)
{
    // first check if the line is parallel to the triangle plane
    // shocked below: there must be () covering tv_cross DOT lv12, otherwise the compilation will fail
    const vec tv_cross = (tv[1] - tv[0]) CROSS (tv[2] - tv[0]);
    const vec lv12 = lv2 - lv1;
    const double tv_cross_dot_lv12 = (tv_cross DOT lv12);
    if (approx_eqt(tv_cross_dot_lv12, 0.0)) {
    	// cout << "check for [" << tv[0] << ", " << tv[1] << ", " << tv[2] << "], " << lv1 << ", " << lv2 << endl;
        // if the 5 points are not co-planar, then unlikely is intersection
        // below calculates the volume of tv and lv1
        double vol_matrix[3][3];
        make_matrix(tv[0] - lv1, tv[1] - lv1, tv[2] - lv1, vol_matrix);
        const double vol_6 = abs(determ(vol_matrix));
        if (vol_6 > 1e-6) {
            return false;
        } else {
            return check_inters_line_tri_2d(tv, lv1, lv2, inters);
        }
    }

    double alpha, beta, gamma;
    inters = inters_line_tri(tv, lv1, lv2, alpha, beta, gamma);
    if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1 && beta + gamma <= 1) {
        return true;
    } else {
        return false;
    }
}
bool check_inters_line_tri_n(const point (&tv)[3], const vec &tn, const point &lv1, const point &lv2, point& inters)
{
	auto sgn1 = approx_sgn(tn[0] * (lv1[0] - tv[0][0]) + tn[1] * (lv1[1] - tv[0][1]) + tn[2] * (lv1[2] - tv[0][2]), 1e-9);
	auto sgn2 = approx_sgn(tn[0] * (lv2[0] - tv[0][0]) + tn[1] * (lv2[1] - tv[0][1]) + tn[2] * (lv2[2] - tv[0][2]), 1e-9);
	if (sgn1 == sgn2) {
		if (sgn1 != 0) { // the case when lv1 and lv2 in different sides of face (coplanar excluded)
			return false;
		} else { // the case when the 5 points are coplanar, then we perform the 2d check
			return check_inters_line_tri_2d(tv, lv1, lv2, inters);
		}
	}

    double gamma = // which is defined to be inters - lv1 = gamma * (lv2 - lv1)
    	(tn[0] * (tv[0][0] - lv1[0]) + tn[1] * (tv[0][1] - lv1[1]) + tn[2] * (tv[0][2] - lv1[2])) /
    	(tn[0] * (lv2  [0] - lv1[0]) + tn[1] * (lv2  [1] - lv1[1]) + tn[2] * (  lv2[2] - lv1[2]));

    inters = lv1 + gamma * (lv2 - lv1);
    return check_pt_inside_tri_2d(inters, tv);
}

// Return the shortest dist between triangle tv1 and tv2
// nn1 and nn2 will be set to the nn points on tv1 and tv2 respectively
// double nn_triangle(const point (&tv1)[3], const point (&tv2)[3], const vec &tn1, const vec &tn2, point& nn1, point& nn2, double time_consumption[3])
double nn_triangle(const point (&tv1)[3], const point (&tv2)[3], const vec &tn1, const vec &tn2, point& nn1, point& nn2)
{
    const int comb[3][2] = { { 0, 1 }, { 0, 2 }, { 1, 2 } };

    // timer_start();

    // First check if tv1 and tv2 intersects <==> at least one adge of one triangle intersects another triangle\
    // Very first, check if any point-pair coincident
    for (int i = 0; i < 3; i++) {
    	for (int j = 0; j < 3; j++) {
    		if (approx_eqt(tv1[i], tv2[j], 1e-9)) {
    			nn1 = nn2 = tv1[i];
    			return 0.0;
    		}
    	}
    }

    bool bb_inters = true;
    // // First of first, if the bounding box not intersecting, bypass
    // const float tv1_bb_min[3] = { min({tv1[0][0], tv1[1][0], tv1[2][0]}), min({tv1[0][1], tv1[1][1], tv1[2][1]}), min({tv1[0][2], tv1[1][2], tv1[2][2]}) };
    // const float tv1_bb_max[3] = { max({tv1[0][0], tv1[1][0], tv1[2][0]}), max({tv1[0][1], tv1[1][1], tv1[2][1]}), max({tv1[0][2], tv1[1][2], tv1[2][2]}) };
    // const float tv2_bb_min[3] = { min({tv2[0][0], tv2[1][0], tv2[2][0]}), min({tv2[0][1], tv2[1][1], tv2[2][1]}), min({tv2[0][2], tv2[1][2], tv2[2][2]}) };
    // const float tv2_bb_max[3] = { max({tv2[0][0], tv2[1][0], tv2[2][0]}), max({tv2[0][1], tv2[1][1], tv2[2][1]}), max({tv2[0][2], tv2[1][2], tv2[2][2]}) };
    // for (int i = 0; i < 3; i++) {
    // 	if (tv1_bb_max[i] < tv2_bb_min[i] || tv1_bb_min[i] > tv2_bb_max[i]) {
    // 		bb_inters = false; // if any dimension non-intersecting, then it must be non-intersecting
    // 		break;
    // 	}
    // }

    // if (bb_inters) { // if only the bounding box not intersecting, then we perform this intersection check
	    point inters;
	    for (int i = 0; i < 3; i++) {
	        const int s1 = comb[i][0];
	        const int s2 = comb[i][1];

	        // if (check_inters_line_tri(tv2, tv1[s1], tv1[s2], inters) || check_inters_line_tri(tv1, tv2[s1], tv2[s2], inters)) { // old method, slower
	        if (check_inters_line_tri_n(tv2, tn2, tv1[s1], tv1[s2], inters) || check_inters_line_tri_n(tv1, tn1, tv2[s1], tv2[s2], inters)) {
	            nn1 = nn2 = inters;
				// time_consumption[0] += timer_end(SECOND);
	            return 0.0;
	        }
	    }
	// }

	// time_consumption[0] += timer_end(SECOND);

    double min_sq_dist = INFTY;

    // timer_start();

    // then check each vertex projection to another face, then check each e2e
    point proj;
    double sq_proj_dist;
    // double al, be;
    for (int i = 0; i < 3; i++) {
        // if (check_proj_inside_p2f(tv1[i], tv2, proj, al, be)) {
        if (check_proj_inside_p2f(tv1[i], tv2, tn2, proj, sq_proj_dist, min_sq_dist)) { // return means sq_proj_dist must be smaller than min_sq_dist
            min_sq_dist = sq_proj_dist;
            nn1 = tv1[i];
            nn2 = proj;
            // cout << "proj hit" << endl;
        }
        // if (check_proj_inside_p2f(tv2[i], tv1, proj, al, be)) {
        if (check_proj_inside_p2f(tv2[i], tv1, tn1, proj, sq_proj_dist, min_sq_dist)) {
            min_sq_dist = sq_proj_dist;
            nn1 = proj;
            nn2 = tv2[i];
            // cout << "proj hit" << endl;
        }
    }

	// time_consumption[1] += timer_end(SECOND);

    // timer_start();

    // then check each e2e
    double alpha, beta;
    double tmp_sq_e2e_dist;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            // const double tmp_sq_dist = sq_dist_e2e(tv1[comb[i][0]], tv1[comb[i][1]], tv2[comb[j][0]], tv2[comb[j][1]], alpha, beta);
            // // cout << comb[i][0] << " " << comb[i][1] << " " << comb[j][0] << " " << comb[j][1] << ": " << tmp_sq_dist << endl;
            // if (min_sq_dist > tmp_sq_dist) {
            //     min_sq_dist = tmp_sq_dist;
            //     nn1 = tv1[comb[i][0]] + alpha * (tv1[comb[i][1]] - tv1[comb[i][0]]);
            //     nn2 = tv2[comb[j][0]] + beta  * (tv2[comb[j][1]] - tv2[comb[j][0]]);
            //     // cout << "e2e hit: " << min_sq_dist << endl;
            // }

            if (sq_dist_e2e_bounded(tv1[comb[i][0]], tv1[comb[i][1]], tv2[comb[j][0]], tv2[comb[j][1]], alpha, beta, tmp_sq_e2e_dist, min_sq_dist)) {
                min_sq_dist = tmp_sq_e2e_dist;
                nn1 = tv1[comb[i][0]] + alpha * (tv1[comb[i][1]] - tv1[comb[i][0]]);
                nn2 = tv2[comb[j][0]] + beta  * (tv2[comb[j][1]] - tv2[comb[j][0]]);
            }
        }
    }

	// time_consumption[2] += timer_end(SECOND);

    return min_sq_dist;
}

double nn_triangle_bf(const point (&tv1)[3], const point (&tv2)[3], point& nn1, point& nn2)
{
    double min_sq_dist = INFTY;
    // int min_i, min_j, min_k, min_l;
    const int num_steps = 100;
    const double step = 1.0 / (double) num_steps;
    for (int i = 0; i <= num_steps; i++) {
        for (int j = 0; j <= num_steps - i; j++) {
            for (int k = 0; k <= num_steps; k++) {
                for (int l = 0; l <= num_steps - k; l++) {
                    const double tmp_al = (double) i * step;
                    const double tmp_be = (double) j * step;
                    const double tmp_la = (double) k * step;
                    const double tmp_mu = (double) l * step;
                    const point tmp_nn1 = tv1[0] + tmp_al * (tv1[1] - tv1[0]) + tmp_be * (tv1[2] - tv1[0]);
                    const point tmp_nn2 = tv2[0] + tmp_la * (tv2[1] - tv2[0]) + tmp_mu * (tv2[2] - tv2[0]);
                    const double tmp_sq_dist = sq_dist(tmp_nn1, tmp_nn2);
                    if (min_sq_dist > tmp_sq_dist) {
                        min_sq_dist = tmp_sq_dist;
                        nn1 = tmp_nn1;
                        nn2 = tmp_nn2;
                        // min_i = i;
                        // min_j = j;
                        // min_k = k;
                        // min_l = l;
                    }
                }
            }
        }
    }

    // cout << min_i << " " << min_j << " " << min_k << " " << min_l << endl;

    return min_sq_dist;
}

// given a triangle tv1, return the volume from it to tv2
// TODO: first solution, sampling lots of points, projecting (or some other way) them to the plane of tv2, (calculating the average distance if to normalize)
double proj_volume(const point (&tv1)[3], const point (&tv2)[3])
{
    // double sum = 0.0;
    // double count = 0;
    // const int num_steps = 1000;
    // const double step = 1.0 / (double) num_steps;
    // for (int i = 0; i <= num_steps; i++) {
    //     for (int j = 0; j <= num_steps - i; j++) {
    //         const point p = tv1[0] + (double) i * step * (tv1[1] - tv1[0]) + (double) j * step * (tv1[2] - tv1[0]);
    //         double al, be;
    //         const point proj = get_proj_p2f(p, tv2, al, be);
    //         sum += sqrt(sq_dist(p, proj));
    //         count++;
    //     }
    // }

    // return (sum / (double) count);
}

// test functions
#ifdef _TEST

	bool test_inverse()
	{
	    // double input[3][3] = {{3, 0, 2}, {2, 0, -2}, {0, 1, 1}};
	    double input[3][3] = {{3, 0, 2}, {2, 0, -2}, {2, 0, -2}};
	    cout << "test det: " << determ(input) << endl; // it should be 10
	    double output[3][3];
	    inverse(input, output);
	    print_matrix(output);
	    // it should be 
	    // 0.2, 0.2, 0
	    // -0.2, 0.3, 1
	    // 0.2, -0.3, 0
	    return true;
	}

	void test_get_proj_p2f()
	{
	    for (int i = 0; i < NUM_TEST; i++) {
	        cout << "Test #" << (i + 1) << endl;

	        const point q = ran_p(), a = ran_p(), b = ran_p(), c = ran_p();
	        const point tv[3] = { a, b, c };
	        const vec tn = tri_norm(tv);
            double sq_proj_dist;
	        point proj = get_proj_p2f(q, tv, tn, sq_proj_dist);

	        // verification:
	        const double ind1 = ((proj - q) DOT (b - a));
	        const double ind2 = ((proj - q) DOT (c - a));
	        if (approx_eqt(ind1, 0) && approx_eqt(ind2, 0)) {
	            cout << "Passed" << endl;
	        } else {
	            cout << "q proj DOT ab (expected 0): " << ind1 << endl;
	            cout << "q proj DOT ac (expected 0): " << ind2 << endl;
	        }
	    }
	}

	void test_sq_dist_p2l()
	{
	    for (int i = 0; i < NUM_TEST; i++) {
	        cout << "Test #" << (i + 1) << endl;

	        const point q = ran_p(), s = ran_p(), t = ran_p();
	        
	        double lambda;
	        const double sq_dist = sq_dist_p2l(q, s, t, lambda);

	        double lambda_bf;
	        const double sq_dist_bf = sq_dist_p2l_bf(q, s, t, lambda_bf);

	        if (approx_eqt(sq_dist, sq_dist_bf) && approx_eqt(lambda, lambda_bf, 1e-3)) {
	            cout << "Passed" << endl;
	        } else {
	            cout << "p2l dist: " << sq_dist << ", brute-force: " << sq_dist_bf << endl;
	            cout << "lambda: "   << lambda <<  ", brute-force: " << lambda_bf  << endl;
	        }
	    }
	}

	void test_sq_dist_p2f()
	{
	    for (int i = 0; i < NUM_TEST; i++) {
	        cout << "Test #" << (i + 1) << endl;

	        const point q = ran_p(), a = ran_p(), b = ran_p(), c = ran_p();
	        const point tv[3] = { a, b, c };
	        const vec tn = tri_norm(tv);

	        point nn;
	        const double sq_dist = sq_dist_p2f(q, tv, tn, nn);

	        point nn_bf;
	        const double sq_dist_bf = sq_dist_p2f_bf(q, tv, nn_bf);

	        if (approx_eqt(sq_dist, sq_dist_bf, 1e-3) && approx_eqt(nn, nn_bf, 1e-3)) {
	            cout << "Passed" << endl;
	        } else {
	            cout << "p2f dist: " << sq_dist << ", brute-force: " << sq_dist_bf << endl;
	            cout << "nn on f: "  << nn      << ", brute-force: " << nn_bf      << endl;
	        }
	    }
	}

	void test_sq_dist_e2e()
	{
	    for (int i = 0; i < NUM_TEST; i++) {
	        cout << "Test #" << (i + 1) << endl;

	        const point a = ran_p(), b = ran_p(), c = ran_p(), d = ran_p();

	        double alpha, beta;
	        const double sq_dist = sq_dist_e2e(a, b, c, d, alpha, beta);

	        double alpha_bf, beta_bf;
	        const double sq_dist_bf = sq_dist_e2e_bf(a, b, c, d, alpha_bf, beta_bf);

	        if (approx_eqt(sq_dist, sq_dist_bf) && approx_eqt(alpha, alpha_bf, 1e-3) && approx_eqt(beta, beta_bf, 1e-3)) {
	            cout << "Passed" << endl;
	        } else {
	            cout << "e2e dist: " << sq_dist << ", brute-force: " << sq_dist_bf << endl;
	            cout << "alpha: "    << alpha   << ", brute-force: " << alpha_bf   << endl;
	            cout << "beta: "     << beta    << ", brute-force: " << beta_bf    << endl;
	        }
	    }
	}

	void test_inters_line_tri()
	{
	    for (int i = 0; i < NUM_TEST; i++) {
	        cout << "Test #" << (i + 1) << endl;

	        const point a = ran_p(), b = ran_p(), c = ran_p(), d = ran_p(), e = ran_p();
	        const point tv[3] = { a, b, c };

	        double alpha, beta, gamma;
	        const point t = inters_line_tri(tv, d, e, alpha, beta, gamma);

	        // ab, ac, at co-planar <==> ab x at // ac x at
	        vec ind1 = normalize((b - a) CROSS (t - a));
	        vec ind2 = normalize((c - a) CROSS (t - a));
	        if (ind1[0] < 0) {
	            ind1 = -ind1;
	        }
	        if (ind2[0] < 0) {
	            ind2 = -ind2;
	        }

	        // t on de <==> dt // et
	        vec ind3 = normalize(t - d);
	        vec ind4 = normalize(t - e);
	        if (ind3[0] < 0) {
	            ind3 = -ind3;
	        }
	        if (ind4[0] < 0) {
	            ind4 = -ind4;
	        }

	        if (approx_eqt(ind1, ind2, 1e-3) && approx_eqt(ind3, ind4, 1e-3)) {
	            cout << "Passed" << endl;
	        } else {
	            if (!approx_eqt(ind1, ind2, 1e-3)) {
	                cout << "Result not on plane!" << endl;
	            }
	            if (!approx_eqt(ind3, ind4, 1e-3)) {
	                cout << "Result not on line!" << endl;
	            }
	        }
	    }
	}

	void test_nn_triangle()
	{
	    for (int i = 0; i < NUM_TEST; i++) {
	        cout << "Test #" << (i + 1) << endl;

	        const point tv1[3] = { ran_p(), ran_p(), ran_p() };
	        const point tv2[3] = { ran_p(), ran_p(), ran_p() };

	        // const point tv1[3] = { point(0, 0, 1), point(0, 1, 1), point(1, 1, 1) };
	        // const point tv1[3] = { point(0.2, 0.2, 0), point(0, 3, 0), point(2, 2, 0) };
	        // const point tv1[3] = { point(-0.5, 0.5, 0), point(1, 0.5, 0), point(0, 2, 0) };
	        // const point tv2[3] = { point(0, 0, 0), point(0, 1, 0), point(1, 0, 0) };

	        point nn1, nn2;
	        const vec tn1 = tri_norm(tv1), tn2 = tri_norm(tv2);
	        double time_consumption[3] = { 0.0, 0.0, 0.0 };
	        // const double sq_dist = nn_triangle(tv1, tv2, tn1, tn2, nn1, nn2, time_consumption);
            const double sq_dist = nn_triangle(tv1, tv2, tn1, tn2, nn1, nn2);

	        point nn1_bf, nn2_bf;
	        const double sq_dist_bf = nn_triangle_bf(tv1, tv2, nn1_bf, nn2_bf);

	        if ((approx_eqt(sq_dist, sq_dist_bf, 1e-3) && approx_eqt(nn1, nn1_bf, 1e-2) && approx_eqt(nn2, nn2_bf, 1e-2)) ||
	            (sq_dist == 0.0 && approx_eqt(sq_dist, sq_dist_bf, 1e-4))) {
	            cout << "Passed" << endl;
	        } else {
	            cout << "triangle dist: " << sq_dist << ", brute-force: " << sq_dist_bf << endl;
	            cout << "nn1: "           << nn1     << ", brute-force: " << nn1_bf     << endl;
	            cout << "nn2: "           << nn2     << ", brute-force: " << nn2_bf     << endl;
	        }

	        // break;
	    }
	}

#endif

#endif