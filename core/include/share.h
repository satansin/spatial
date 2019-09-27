#ifndef __SHARE_H
#define __SHARE_H

#include "point.h"
#include "trans.h"
#include "TriMesh.h"
#include "KDtree.h"
#include <limits>
#include <chrono>
#include <stack>

enum Time_Unit {
    MILLISECOND, SECOND, MINUTE, HOUR
};

std::stack<std::chrono::time_point<std::chrono::high_resolution_clock>> timer_stack;

void timer_start() {
    timer_stack.push(std::chrono::high_resolution_clock::now());
}

double timer_end(Time_Unit unit) {
    if (timer_stack.empty()) {
        return 0.0;
    }
    auto start = timer_stack.top();
    timer_stack.pop();
    auto end = std::chrono::high_resolution_clock::now();
    double interval = 0.0;
    switch (unit) {
        case MILLISECOND: {
            std::chrono::duration<double, std::milli> span_milli = end - start;
            interval = span_milli.count();
        }
        break;
        case SECOND: {
            std::chrono::duration<double> span_sec = end - start;
            interval = span_sec.count();
        }
        break;
        case MINUTE: {
            std::chrono::duration<double, std::ratio<60>> span_min = end - start;
            interval = span_min.count();
        }
        break;
        case HOUR: {
            std::chrono::duration<double, std::ratio<3600>> span_hour = end - start;
            interval = span_hour.count();
        }
        break;
    }
    return interval;
}

double cal_corr_err(trimesh::TriMesh* mesh_q, trimesh::KDtree* kdtree_p, Trans* xf, double stop_at = std::numeric_limits<double>::max()) {
    double err = 0.0;
    for (auto &v: mesh_q->vertices) {
        Pt3D xf_q = trans_pt(xf, pt(v));
        float pt_arr[3] = { (float) xf_q.x, (float) xf_q.y, (float) xf_q.z };
        auto nn = kdtree_p->closest_to_pt(pt_arr);
        double nn_err = eucl_dist(nn, pt_arr);
        err += nn_err;
        if (err - stop_at > 0) {
            return -1;
        }
    }
    return err;
}

#endif