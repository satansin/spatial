#ifndef __SHARE_H
#define __SHARE_H

#include "point.h"
#include "trans.h"
#include "TriMesh.h"

double nn(Pt3D q, Pt3D* p, int n) {
	double min_dist = 99999999999;
	// int min_index = -1;
	for (int i = 0; i < n; i++) {
		double new_dist = eucl_dist(q, p[i]);
		if (new_dist < min_dist) {
			min_dist = new_dist;
			// min_index = i;
		}
	}
	return min_dist;
}

double cal_err(Pt3D pts_q[], int num_q, Pt3D pts_p[], int num_p, Trans xf) {
	double err = 0;
    // int no_inlier = 0;
    for (int i = 0; i < num_q; i++) {
    	Pt3D xf_q = trans_pt(&xf, pts_q + i);
    	double nn_err = nn(xf_q, pts_p, num_p);
    	err += nn_err;
    	// if (nn_err <= 0.1) {
    	// 	no_inlier++;
    	// }
    }
    err /= num_q;
    return err;
}

double cal_err(trimesh::TriMesh* mesh_q, trimesh::TriMesh* mesh_p, Trans xf) {
	int m = mesh_q->vertices.size();
    int n = mesh_p->vertices.size();
    Pt3D pts_q[m], pts_p[n];
    for (int i = 0; i < m; i++) {
    	pts_q[i] = pt(mesh_q->vertices[i]);
    }
    for (int i = 0; i < n; i++) {
    	pts_p[i] = pt(mesh_p->vertices[i]);
    }
    return cal_err(pts_q, m, pts_p, n, xf);
}

#endif