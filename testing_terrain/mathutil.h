#ifndef __MATHUTIL_H_
#define __MATHUTIL_H_

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

inline bool approx_eqt(double a, double b, double zero = APPROX_ZERO)
{
    return (abs(a - b) <= zero);
}

inline bool approx_eqt(point a, point b, double zero = APPROX_ZERO)
{
    return (approx_eqt(a[0], b[0], zero) && approx_eqt(a[1], b[1], zero) && approx_eqt(a[2], b[2], zero));
}

// return a random float in (-1, 1)
inline float ran_f()
{
    return (2.0 * (float) rand() / RAND_MAX - 1.0);
}

// return a random point in (-1, 1)
inline point ran_p()
{
    return point(ran_f(), ran_f(), ran_f());
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

// Determinant of a 3x3 matrix - used by align_pt2pt
inline double det3(const double (&A)[3][3])
{
	return A[0][0] * (A[1][1] * A[2][2] - A[1][2] * A[2][1]) +
	       A[0][1] * (A[1][2] * A[2][0] - A[1][0] * A[2][2]) +
	       A[0][2] * (A[1][0] * A[2][1] - A[1][1] * A[2][0]);
}

inline double det4(const double (&A)[4][4])
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

inline bool inverse(const double (&A)[3][3], double inverse[3][3])
{
	double det_A = det3(A);
	if (det_A == 0.0) {
        cout << "Singular matrix, can't find its inverse" << endl;
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

// Return the projection point from q to the plane of triangle fv
// let abc be the triangle fv, and let r be the return
// then ar = alpha * ab + beta * ac
point get_proj_p2f(const point &q, const point (&fv)[3], double& alpha, double& beta)
{
    // get projection of q to plane defined by fv[3]
    point ab = fv[1] - fv[0];
    point ac = fv[2] - fv[0];
    point cr = ab CROSS ac;
    point aq = q - fv[0];
    // cout << ab << endl;
    // cout << ac << endl;
    // cout << cr << endl;
    // cout << aq << endl;

    double U[3][3];
    make_matrix(ab, ac, -cr, U);
    // print_matrix(U);

    double U_1[3][3];
    inverse(U, U_1);
    auto ret_params = matrix_prd(U_1, aq);

    alpha = ret_params[0];
    beta = ret_params[1];

    return (alpha * ab + beta * ac + fv[0]);
}

// Return whether q's projection to the plane of triangle fv is inside fv
// proj is set to the projection point
bool check_proj_inside_p2f(const point &q, const point (&fv)[3], point& proj)
{
    double alpha, beta;
    proj = get_proj_p2f(q, fv, alpha, beta);
    if (0.0 <= alpha && alpha <= 1.0 && 0.0 <= beta && beta <= 1.0 && alpha + beta <= 1.0) {
        return true;
    } else {
        return false;
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

// Return the SQ dist between point q to triangle fv[3]
// res is set to the NN point on the triangle to q
// flag is set to 0, 1 or 2 if the res = fv[0, 1 or 2], otherwise -1
double sq_dist_p2f(const point &q, const point (&fv)[3], point& res, int& flag)
{
    // get projection of q to plane defined by fv[3]
    // if projection inside triangle, then the projection distance must be the shortest
    point proj;
    if (check_proj_inside_p2f(q, fv, proj)) {
        // proj inside triangle
        res = proj;
        flag = -1;
        return sq_dist(q, proj);
    }

    // then, the shortest distance must appear on edge
    double min_sq_dist = INFTY;
    const int comb[3][2] = { { 0, 1 }, { 0, 2 }, { 1, 2 } };
    for (int i = 0; i < 3; i++) {
        double tmp_l;
        const double tmp_sq_dist = sq_dist_p2l(q, fv[comb[i][0]], fv[comb[i][1]], tmp_l);
        if (min_sq_dist > tmp_sq_dist) {
            min_sq_dist = tmp_sq_dist;
            if (tmp_l == 0) {
                res = fv[comb[i][0]];
                flag = comb[i][0];
            } else if (tmp_l == 1) {
                res = fv[comb[i][1]];
                flag = comb[i][1];
            } else {
                res = fv[comb[i][0]] + tmp_l * (fv[comb[i][1]] - fv[comb[i][0]]);
                flag = -1;
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
    //     v_dists[i] = sq_dist(q, fv[i]);
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

    // point st = fv[t] - fv[s];
    // point sx = proj - fv[s];
    // double l = (st DOT sx) / (st DOT st);
    // // cout << "l: " << l << endl;

    // if (l <= 0.0) {
    //     assert(v_dists[s] <= v_dists[t]);
    //     // proj near s
    //     res = fv[s];
    //     flag = s;
    //     return v_dists[s];
    // } else if (l >= 1.0) {
    //     assert(v_dists[s] >= v_dists[t]);
    //     // proj near t
    //     res = fv[t];
    //     flag = t;
    //     return v_dists[t];
    // } else {
    //     res = l * st + fv[s];
    //     flag = -1;
    //     return sq_dist(q, res);
    // }
}

double sq_dist_p2f_bf(const point &q, const point (&fv)[3], point& res, int& flag)
{
    double min_sq_dist = INFTY;
    int min_i, min_j;
    const int num_steps = 1000;
    const double step = 1.0 / (double) num_steps;
    for (int i = 0; i <= num_steps; i++) {
        for (int j = 0; j <= num_steps - i; j++) {
            const double tmp_al = (double) i * step;
            const double tmp_be = (double) j * step;
            const point tmp_res = fv[0] + tmp_al * (fv[1] - fv[0]) + tmp_be * (fv[2] - fv[0]);
            const double tmp_sq_dist = sq_dist(q, tmp_res);
            if (min_sq_dist > tmp_sq_dist) {
                min_sq_dist = tmp_sq_dist;
                res = tmp_res;
                min_i = i;
                min_j = j;
            }
        }
    }

    if (min_i == 0 && min_j == 0) {
        flag = 0;
    } else if (min_i == num_steps && min_j == 0) {
        flag = 1;
    } else if (min_i == 0 && min_j == num_steps) {
        flag = 2;
    } else {
        flag = -1;
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

    beta = (sq_norm_ab * (cd DOT ca) - (ab_dot_ca) * (ab_dot_cd)) / (sq_norm_ab * sq_norm_cd - sq(ab_dot_cd));
    // const double cos_theta_2 = sq(ab_dot_cd) / (sq_norm_ab * sq_norm_cd); // theta = angle between ab and cd
    // beta = (cd DOT ca) / (sq_norm_cd * (1.0 - cos_theta_2));
    alpha = (beta * ab_dot_cd - ab_dot_ca) / sq_norm_ab;

    if (0 <= alpha && alpha <= 1 && 0 <= beta && beta <= 1) {
        return sq_norm(alpha * ab + a - beta * cd - c);
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

// input: triangle abc (in order of tv), line segment de (in order of lv1, lv2)
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
    inverse(U, inv_U);

    auto params = matrix_prd(inv_U, da);
    alpha = params[0];
    beta = params[1];
    gamma = params[2];

    return (alpha * de + lv1);
}

bool check_inters_line_tri(const point (&tv)[3], const point &lv1, const point &lv2, point& inters)
{
    double alpha, beta, gamma;
    inters = inters_line_tri(tv, lv1, lv2, alpha, beta, gamma);
    if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1 && beta + gamma <= 1) {
        return true;
    } else {
        return false;
    }
}

// Return the shortest dist between triangle fv1 and fv2
// nn1 and nn2 will be set to the nn points on fv1 and fv2 respectively
// flag(1/2) is set to 0, 1 or 2 if the nn(1/2) = fv(1/2)[0, 1 or 2], otherwise -1
double nn_triangle(const point (&fv1)[3], const point (&fv2)[3], point& nn1, point& nn2, int& flag1, int& flag2)
{
    // first check if fv1 and fv2 intersects <==> at least one adge of one triangle intersects another triangle
    point inters;
    if (check_inters_line_tri(fv2, fv1[0], fv1[1], inters) || check_inters_line_tri(fv2, fv1[0], fv1[2], inters) || check_inters_line_tri(fv2, fv1[1], fv1[2], inters) ||
        check_inters_line_tri(fv1, fv2[0], fv2[1], inters) || check_inters_line_tri(fv1, fv2[0], fv2[2], inters) || check_inters_line_tri(fv1, fv2[1], fv2[2], inters)) {
        nn1 = nn2 = inters;
        flag1 = flag2 = -1;
        return 0.0;
    }

    double min_sq_dist = INFTY;

    // then check each vertex projection to another face, then check each e2e
    point proj;
    for (int i = 0; i < 3; i++) {
        if (check_proj_inside_p2f(fv1[i], fv2, proj)) {
            const double tmp_sq_dist = sq_dist(fv1[i], proj);
            if (min_sq_dist > tmp_sq_dist) {
                min_sq_dist = tmp_sq_dist;
                nn1 = fv1[i];
                nn2 = proj;
                flag1 = i;
                flag2 = -1;
                // cout << "proj hit" << endl;
            }
        }
        if (check_proj_inside_p2f(fv2[i], fv1, proj)) {
            const double tmp_sq_dist = sq_dist(fv2[i], proj);
            if (min_sq_dist > tmp_sq_dist) {
                min_sq_dist = tmp_sq_dist;
                nn1 = proj;
                nn2 = fv2[i];
                flag1 = -1;
                flag2 = i;
                // cout << "proj hit" << endl;
            }
        }
    }

    // then check each e2e
    const int comb[3][2] = { { 0, 1 }, { 0, 2 }, { 1, 2 } };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            double alpha, beta;
            const double tmp_sq_dist = sq_dist_e2e(
                fv1[comb[i][0]], fv1[comb[i][1]], fv2[comb[j][0]], fv2[comb[j][1]], alpha, beta
            );
            // cout << comb[i][0] << " " << comb[i][1] << " " << comb[j][0] << " " << comb[j][1] << ": " << tmp_sq_dist << endl;
            if (min_sq_dist > tmp_sq_dist) {
                min_sq_dist = tmp_sq_dist;
                nn1 = fv1[comb[i][0]] + alpha * (fv1[comb[i][1]] - fv1[comb[i][0]]);
                nn2 = fv2[comb[j][0]] + beta  * (fv2[comb[j][1]] - fv2[comb[j][0]]);
                if (alpha == 0) {
                    flag1 = comb[i][0];
                } else if (alpha == 1) {
                    flag1 = comb[i][1];
                } else {
                    flag1 = -1;
                }
                if (beta == 0) {
                    flag2 = comb[j][0];
                } else if (beta == 1) {
                    flag2 = comb[j][1];
                } else {
                    flag2 = -1;
                }
                // cout << "e2e hit: " << min_sq_dist << endl;
            }
        }
    }

    return min_sq_dist;
}

double nn_triangle_bf(const point (&fv1)[3], const point (&fv2)[3], point& nn1, point& nn2, int& flag1, int& flag2)
{
    double min_sq_dist = INFTY;
    int min_i, min_j, min_k, min_l;
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
                    const point tmp_nn1 = fv1[0] + tmp_al * (fv1[1] - fv1[0]) + tmp_be * (fv1[2] - fv1[0]);
                    const point tmp_nn2 = fv2[0] + tmp_la * (fv2[1] - fv2[0]) + tmp_mu * (fv2[2] - fv2[0]);
                    const double tmp_sq_dist = sq_dist(tmp_nn1, tmp_nn2);
                    if (min_sq_dist > tmp_sq_dist) {
                        min_sq_dist = tmp_sq_dist;
                        nn1 = tmp_nn1;
                        nn2 = tmp_nn2;
                        min_i = i;
                        min_j = j;
                        min_k = k;
                        min_l = l;
                    }
                }
            }
        }
    }

    // cout << min_i << " " << min_j << " " << min_k << " " << min_l << endl;

    if (min_i == 0 && min_j == 0) {
        flag1 = 0;
    } else if (min_i == num_steps && min_j == 0) {
        flag1 = 1;
    } else if (min_i == 0 && min_j == num_steps) {
        flag1 = 2;
    } else {
        flag1 = -1;
    }

    if (min_k == 0 && min_l == 0) {
        flag2 = 0;
    } else if (min_k == num_steps && min_l == 0) {
        flag2 = 1;
    } else if (min_k == 0 && min_l == num_steps) {
        flag2 = 2;
    } else {
        flag2 = -1;
    }

    return min_sq_dist;
}

// test functions
#ifdef _TEST

bool test_inverse()
{
    // double input[3][3] = {{3, 0, 2}, {2, 0, -2}, {0, 1, 1}};
    double input[3][3] = {{3, 0, 2}, {2, 0, -2}, {2, 0, -2}};
    cout << "test det: " << det3(input) << endl; // it should be 10
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
        const point fv[3] = { a, b, c };
        double al, be;
        point proj = get_proj_p2f(q, fv, al, be);

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
        const point fv[3] = { a, b, c };

        int flag;
        point nn;
        const double sq_dist = sq_dist_p2f(q, fv, nn, flag);

        int flag_bf;
        point nn_bf;
        const double sq_dist_bf = sq_dist_p2f_bf(q, fv, nn_bf, flag_bf);

        if (approx_eqt(sq_dist, sq_dist_bf, 1e-3) && approx_eqt(nn, nn_bf, 1e-3) && flag == flag_bf) {
            cout << "Passed" << endl;
        } else {
            cout << "p2f dist: " << sq_dist << ", brute-force: " << sq_dist_bf << endl;
            cout << "nn on f: "  << nn      << ", brute-force: " << nn_bf      << endl;
            cout << "flag: "     << flag    << ", brute-force: " << flag_bf    << endl;
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

        const point fv1[3] = { ran_p(), ran_p(), ran_p() };
        const point fv2[3] = { ran_p(), ran_p(), ran_p() };

        point nn1, nn2;
        int flag1, flag2;
        const double sq_dist = nn_triangle(fv1, fv2, nn1, nn2, flag1, flag2);

        point nn1_bf, nn2_bf;
        int flag1_bf, flag2_bf;
        const double sq_dist_bf = nn_triangle_bf(fv1, fv2, nn1_bf, nn2_bf, flag1_bf, flag2_bf);

        if ((approx_eqt(sq_dist, sq_dist_bf, 1e-3) && approx_eqt(nn1, nn1_bf, 1e-2) && approx_eqt(nn2, nn2_bf, 1e-2) && flag1 == flag1_bf && flag2 == flag2_bf) ||
            (sq_dist == 0.0 && approx_eqt(sq_dist, sq_dist_bf, 1e-4) && flag1 == flag1_bf && flag2 == flag2_bf)) {
            cout << "Passed" << endl;
        } else {
            cout << "triangle dist: " << sq_dist << ", brute-force: " << sq_dist_bf << endl;
            cout << "nn1: "           << nn1     << ", brute-force: " << nn1_bf     << endl;
            cout << "nn2: "           << nn2     << ", brute-force: " << nn2_bf     << endl;
            cout << "flag1: "         << flag1   << ", brute-force: " << flag1_bf   << endl;
            cout << "flag2: "         << flag2   << ", brute-force: " << flag2_bf   << endl;
        }
    }
}

#endif

#endif