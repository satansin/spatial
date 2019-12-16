void add_ann_pts_in_cell(const Cell* c, PtwID p, double ann_min, double ann_max, vector<PtwID>& v) {
    for (auto &i: c->list) {
        double d = eucl_dist(i.pt, p.pt);
        if (d - ann_min >= 0 && d - ann_max <= 0) {
            v.push_back(i);
        }
    }
}

vector<PtwID> get_ann_pts(const Cell* c, PtwID p, double ann_min, double ann_max, const Grid* g, bool debug_mode) {
    vector<PtwID> ann_pts;

    Box3D_Int outer_box_int = g->get_outer_box(p.pt.x - ann_max, p.pt.y - ann_max, p.pt.z - ann_max,
        p.pt.x + ann_max, p.pt.y + ann_max, p.pt.z + ann_max);
    // cout << "Outer int box: " << outer_box_int.to_str() << endl;

    Box3D_Int inner_box_int(c->x, c->y, c->z, c->x, c->y, c->z, true); // initially empty
    while (true) {
        Box3D_Int tmp_scaled_box = inner_box_int;
        // attemp to scale the box by 1
        if (tmp_scaled_box.empty) {
            tmp_scaled_box.empty = false; // first if it is empty, make it non-empty
        } else {
            tmp_scaled_box.expand();
        }
        if (max_dist(p.pt, Box3D(tmp_scaled_box, g->w)) < ann_min) {
            inner_box_int = tmp_scaled_box;
            continue;
        } else {
            break;
        }
    }
    // cout << "Inter int box: " << inner_box_int.size() << endl;

    int intersecting_cells_count = 0;
    double inner_time = 0.0;

    vector<int> list;
    outer_box_int.list_cell_id(list);
    for (int* i = &list[0]; i < &list[list.size() - 1]; i += 3) {
        if (inner_box_int.includes(i)) {
            continue;
        }
        auto cell_ptr = g->find_cell(i);
        if (cell_ptr) {
            Box3D cell_box(cell_ptr, g->w);
            if (max_dist(p.pt, cell_box) - ann_min >= 0 &&
                min_dist(p.pt, cell_box) - ann_max <= 0) {
                if (debug_mode) intersecting_cells_count++;
                if (debug_mode) timer_start();
                add_ann_pts_in_cell(cell_ptr, p, ann_min, ann_max, ann_pts);
                if (debug_mode) inner_time += timer_end(MILLISECOND);
            }
        }
    }
    if (debug_mode) cout << TABTAB << "Actual time of adding ann pts: " << inner_time << endl;

    return ann_pts;
}

bool test_index_entry(const Cell* c, PtwID p, double ann_min, double ann_max, const Grid* g, int th, bool debug_mode) {
    vector<PtwID> ann_pts = get_ann_pts(c, p, ann_min, ann_max, g, debug_mode);
    int hit_size = ann_pts.size();
    if (hit_size < th) {
        return false;
    } else {
        return true;
    }
}

Entry cal_index_entry_depr(const Cell* c, PtwID p, double ann_min, double ann_max, const Grid* g, bool debug_mode) {
    if (debug_mode) cout << TAB << "Pt #" << p.id << endl;

    if (debug_mode) timer_start();
    vector<PtwID> ann_pts = get_ann_pts(c, p, ann_min, ann_max, g, debug_mode);
    if (debug_mode) cout << TABTAB << "Total time of get_ann_pts: " << timer_end(MILLISECOND) << "(ms)" << endl;

    int hit_size = ann_pts.size();

    if (debug_mode) cout << TABTAB << "# ann pts: " << hit_size << endl;
    
    if (hit_size < 3) {
        Entry fail;
        *(fail.repre) = p;
        fail.fail = true;
        return fail;
    }

    if (debug_mode) timer_start();

    Entry prem;
    prem.meas = -1;
    prem.fail = false;
    for (int i = 0; i < hit_size; i++) {
        for (int j = i + 1; j < hit_size; j++) {
            for (int k = j + 1; k < hit_size; k++) {
                auto ratio_set = get_ratio_set_vol(p.pt,
                    ann_pts[i].pt, ann_pts[j].pt, ann_pts[k].pt);
                if (ratio_set.ratio - prem.meas > 0) {
                    prem.set(p, ann_pts[i], ann_pts[j], ann_pts[k], ratio_set.volume, ratio_set.ratio);
                }
            }
        }
    }
    sort_remai(prem);
    // double bc = eucl_dist(prem.remai[0].pt, prem.remai[1].pt);
    // double bd = eucl_dist(prem.remai[0].pt, prem.remai[2].pt);
    // double cd = eucl_dist(prem.remai[1].pt, prem.remai[2].pt);
    // if (!(cd >= bd && bd >= bc)) {
    //     prem.fail = true;
    // }

    if (debug_mode) cout << TABTAB << "Time of calculating prem entry: " << timer_end(MILLISECOND) << "(ms)" << endl;

    return prem;
}

For a 3D circle centered at o, normal vector oa, and radius r
fill b with the nn point to the circle
fill d with the nn point on the circle
return the smallest distance
double get_nn_circle(Pt3D o, PtwID a, double r, const Grid* g, PtwID& b, Pt3D& d) {
    b.id = -1;
    double nn_dist_sq = numeric_limits<double>::max();

    auto oa = a.pt - o;
    double d_oa = oa.mode();
    Pt3D ub;

    Box3D_Int outer_box_int = g->get_outer_box(o.x - r, o.y - r, o.z - r, o.x + r, o.y + r, o.z + r);
    Box3D_Int inner_box_int = outer_box_int;
    inner_box_int.empty = true;

    vector<int> list;
    Pt3D ob_tmp, u_tmp, ub_tmp;
    double nn_dist_sq_tmp;

    while (b.id < 0) {
        outer_box_int.list_cell_id(list);
        for (int* i = &list[0]; i < &list[list.size() - 1]; i += 3) {
            if (inner_box_int.includes(i)) {
                continue;
            }
            auto cell_ptr = g->find_cell(i);
            if (cell_ptr) {
                for (auto &b_tmp: cell_ptr->list) {
                    if (b_tmp.id == a.id) {
                        continue; // exclude the case when a = b
                    }
                    ob_tmp = b_tmp.pt - o;
                    u_tmp = o + oa * dot_prd(oa, ob_tmp) / (d_oa * d_oa); // u be b's projection on vector oa
                    ub_tmp = b_tmp.pt - u_tmp;
                    nn_dist_sq_tmp = ob_tmp.sq_mode() + r * r - 2.0 * r * ub_tmp.mode();
                    if (nn_dist_sq_tmp - nn_dist_sq < 0) {
                        b = b_tmp;
                        ub = ub_tmp;
                        nn_dist_sq = nn_dist_sq_tmp;
                    }
                }
            }
        }
        inner_box_int = outer_box_int;
        outer_box_int.expand();
    }

    d = o + ub * (r / ub.mode());

    return sqrt(nn_dist_sq);
}

Entry cal_index_entry(const Cell* c, PtwID p, double ann_min, double ann_max, const Grid* g, const KDtree* kd_p, bool debug_mode) {
    if (debug_mode) cout << TAB << "Pt #" << p.id << endl;

    if (debug_mode) timer_start();
    vector<PtwID> ann_pts = get_ann_pts(c, p, ann_min, ann_max, g, debug_mode);
    if (debug_mode) cout << TABTAB << "Total time of get_ann_pts: " << timer_end(MILLISECOND) << "(ms)" << endl;

    int hit_size = ann_pts.size();

    if (debug_mode) cout << TABTAB << "# ann pts: " << hit_size << endl;
    
    if (hit_size <= 0) {
        Entry fail;
        *(fail.repre) = p;
        fail.fail = true;
        return fail;
    }

    if (debug_mode) timer_start();

    Entry prem;
    prem.meas = -1;
    prem.fail = false;
    if (debug_mode) cout << TABTAB << "Pt b for each ann pt:";
    for (auto &a: ann_pts) {
        auto m = middle_pt(a.pt, p.pt);
        double d_ma = eucl_dist(m, a.pt);
        double r = d_ma * 1.732050807569;
        PtwID b;
        Pt3D d1;
        double nn_dist = get_nn_circle(m, a, r, g, b, d1);
        if (debug_mode) printf(" (%d, %f)", b.id, nn_dist);

        Pt3D std_coor[3] = { {0, 0, 0}, {r, 0, 0}, {0, 0, d_ma} };
        Pt3D real_coor[3] = { m, d1, a.pt };
        auto xf = cal_trans(std_coor, real_coor, 3);
        Pt3D d2 = trans_pt(&xf, { r / 3.0, r * 0.94280904, 0 });

        float pt_arr[3] = { (float) d2.x, (float) d2.y, (float) d2.z };
        auto nn_d2 = kd_p->closest_to_pt(pt_arr);
        auto c = g->relocate(nn_d2);

        auto ratio_set = get_ratio_set_vol(p.pt, a.pt, b.pt, c.pt);
        if (ratio_set.ratio - prem.meas > 0) {
            prem.set(p, a, b, c, ratio_set.volume, ratio_set.ratio);
        }
    }
    if (debug_mode) cout << endl;

    if (debug_mode) cout << TABTAB << "Time of calculating prem entry: " << timer_end(MILLISECOND) << "(ms)" << endl;

    return prem;
}