#include "TriMesh.h"
#include "KDtree.h"
#include "point.h"
#include "tetra_meas.h"
#include "trans.h"
#include "pcr_adv.h"
#include "RTree.h"
extern "C" {
    #include "rtree.h"
}
#include "ProgressBar.hpp"
#include "share.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <ctime>
#include <cstdlib>
using namespace std;
using namespace trimesh;

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

// For a 3D circle centered at o, normal vector oa, and radius r
// fill b with the nn point to the circle
// fill d with the nn point on the circle
// return the smallest distance
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

Entry* cal_index_entry_new(PtwID p, double min, const TriMesh* mesh_p, node_type *r_root, rtree_info* r_info, bool debug_mode) {

	Entry* ret = new Entry();

    if (debug_mode) cout << TAB << "Pt #" << p.id << endl;

    // start looking for the first subsidiary pt a
    if (debug_mode) timer_start();

    // R_TYPE *query_a;
    // convert_pt(&(p.pt), query_a);
    // NN_type *nn_a;
    // k_NN_search_sphere(r_root, query_a, 1, &nn_a, r_info, min * min);

    auto nn_a = nn_sphere(&(p.pt), r_root, r_info, min * min)[0];

    if (debug_mode) cout << TABTAB << "First pt #" << nn_a->oid << " dist=" << sqrt(nn_a->dist) << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    // start looking for the help pt h
    if (debug_mode) timer_start();

    auto a = PtwID(nn_a->oid, mesh_p);
    auto m = middle_pt(p.pt, a.pt);

    // R_TYPE *query_m;
    // convert_pt(&m, query_m);
    // NN_type *nn_m;
    // k_NN_search_sphere(r_root, query_m, 3, &nn_m, r_info, d_pm * d_pm);

    float d_pm = eucl_dist(p.pt, m);
    auto nn_h = nn_sphere(&m, r_root, r_info, d_pm * d_pm, { p.id, a.id })[0];

    // for (; nn_m != NULL && (nn_m->oid == p.id || nn_m->oid == a.id); nn_m = nn_m->next) {}
    // Pt3D h = pt(mesh_p->vertices[nn_m->oid]);

    if (debug_mode) cout << TABTAB << "Help pt #" << nn_h->oid << " dist=" << sqrt(nn_h->dist) << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    auto h = PtwID(nn_h->oid, mesh_p);

    if (debug_mode) { cout << TABTAB; printf("hp=%f, ha=%f\n", eucl_dist(h.pt, p.pt), eucl_dist(h.pt, a.pt)); }

    // // start looking for the second subsidiary pt b
    // if (debug_mode) timer_start();

    // double r = d_pm * 1.732050807569;
    // auto ma = a.pt - m;
    // auto mh = h.pt - m;
    // auto u = m + ma * dot_prd(ma, mh) / (d_pm * d_pm); // d_ma = d_pm
    // auto uh = h.pt - u;
    // auto b_est = m + uh * (r / uh.mode());

    // R_TYPE *query_b;
    // convert_pt(&b_est, query_b);
    // NN_type *nn_b;
    // k_NN_search(r_root, query_b, 3, &nn_b, r_info);

    // for (; nn_b != NULL && (nn_b->oid == p.id || nn_b->oid == a.id); nn_b = nn_b->next);
    // PtwID b = PtwID(nn_b->oid, pt(mesh_p->vertices[nn_b->oid]));

    // if (debug_mode) cout << TABTAB << "Second pt #" << nn_b->oid << " dist=" << sqrt(nn_b->dist) << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    // // start looking for the third subsidiary pt c
    // if (debug_mode) timer_start();

    // Pt3D std_coor[3] = { {0, 0, 0}, {r, 0, 0}, {0, 0, d_pm} }; // put m, b_est, a in a righthand coordinate system
    // Pt3D real_coor[3] = { m, b_est, a.pt };
    // auto xf = cal_trans(std_coor, real_coor, 3);
    // Pt3D c_est = trans_pt(&xf, { r / 3.0, r * 0.94280904, 0 }); // rotate b_est arccos(1/3) towards y+ surrounding m

    // R_TYPE *query_c;
    // convert_pt(&c_est, query_c);
    // NN_type *nn_c;
    // k_NN_search(r_root, query_c, 4, &nn_c, r_info);

    // for (; nn_c != NULL && (nn_c->oid == p.id || nn_c->oid == a.id || nn_c->oid == b.id); nn_c = nn_c->next);
    // PtwID c = PtwID(nn_c->oid, pt(mesh_p->vertices[nn_c->oid]));

    // if (debug_mode) cout << TABTAB << "Third pt #" << nn_c->oid << " dist=" << sqrt(nn_c->dist) << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    // start looking for the rest two pts b and c
    if (debug_mode) timer_start();

    Pt3D b_est, c_est;
    auto got_b_c = get_est_b_c(&m, &(a.pt), &(h.pt), b_est, c_est);

    if (!got_b_c) {
    	ret->fail = true;
    	return ret;
    }

    auto nn_b = nn_sphere(&b_est, r_root, r_info, 0.0, { p.id, a.id })[0];
    auto b = PtwID(nn_b->oid, mesh_p);

    auto nn_c = nn_sphere(&c_est, r_root, r_info, 0.0, { p.id, a.id, b.id })[0];
    auto c = PtwID(nn_c->oid, mesh_p);

    // get the ratio set
    auto ratio_set = get_ratio_set_vol(p.pt, a.pt, b.pt, c.pt);
    ret->set(p, a, b, c, ratio_set.volume, ratio_set.ratio);

    return ret;

}

void performance_test(TriMesh *mesh, int n, KDtree *kd, node_type *r_root, rtree_info* aInfo) {
    srand(time(NULL));
    R_TYPE *query;
    query = (R_TYPE *) malloc(sizeof(R_TYPE) * aInfo->dim);
    for (int i = 0; i < 100; ++i) {
        auto test_pt = mesh->vertices[rand() % n];
        for (int j = 0; j < aInfo->dim; j++) {
            query[j] = test_pt[j];
        }
        timer_start();
        auto nn_kd = kd->closest_to_pt(test_pt);
        auto t_kd = timer_end(MILLISECOND);

        NN_type *NNresult;
        timer_start();
        k_NN_search(r_root, query, 1, &NNresult, aInfo);
        auto t_r = timer_end(MILLISECOND);

        cout << t_kd << " v.s. " << t_r << endl;
    }
}

void gridify_test(double w, TriMesh* mesh) {
    double delta = 0.3 * w;
    int num_trials = 11;
    Grid g;

    for (int counter = 0; counter < num_trials; counter++) {
        double trial_w = w + counter * delta;
        g.set_width(trial_w);
        cout << "\nTest gridify the point cloud..." << endl;
        g.gridify(mesh);

        cout << "\tGrid size: " << trial_w << endl;
        cout << "\tTotal # cells: " << g.cells_count << endl;
        cout << "\tAvg # pts per cell: " << ((double) mesh->vertices.size()) / ((double) g.cells_count) << endl;
    }

}

int main(int argc, char **argv) {
    if (argc < 6) {
        cerr << "Usage: " << argv[0] << " database_filename db_rstree_filename w ann_min ann_max output_index_filename [-test] [-show_prog_bar] [-debug]" << endl;
        exit(1);
    }

    bool test_mode = false, show_prog_bar = false, debug_mode = false;
    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-test") {
            test_mode = true;
        } else if (string(argv[i]) == "-show_prog_bar") {
            show_prog_bar = true;
        } else if (string(argv[i]) == "-debug") {
            debug_mode = true;
        }
    }

    int argi = 0;
    string database_filename = argv[++argi];
    string db_rstree_filename = argv[++argi];
    double w = atof(argv[++argi]);
    double ann_min = atof(argv[++argi]);
    double ann_max = atof(argv[++argi]);
    string outidx_filename = argv[++argi];

    timer_start();

    TriMesh *mesh_p = TriMesh::read(database_filename);
    int n = mesh_p->vertices.size();
    // KDtree *kd_p = new KDtree(mesh_p->vertices);
    
    rtree_info db_rtree_info = { 5, 10, 3, 7 };
    node_type *root;

    // load R-tree
    cout << "Loading R-tree for DB points..." << endl;
    timer_start();
    read_rtree(&root, db_rstree_filename.c_str(), &db_rtree_info);
    cout << "Load R-tree of DB pts in " << timer_end(SECOND) << "(s)" << endl;

    cout << endl;
    cout << "Total I/O time: " << timer_end(SECOND) << "(s)" << endl;

    // performance_test(mesh_p, n, kd_p, root, &db_rtree_info);

    if (test_mode) gridify_test(w, mesh_p);

    Grid g;
    g.set_width(w);
    cout << "\nGridify the point cloud..." << endl;

    timer_start();
    g.gridify(mesh_p);
    cout << "Gridify finished in " << timer_end(SECOND) << "(s)" << endl;

    cout << "Grid size: " << w << endl;
    cout << "Total # cells: " << g.cells_count << endl;
    cout << "Avg # pts per cell: " << ((double) n) / ((double) g.cells_count) << endl;

    // exit(0);

    Struct_DB s_db;
    s_db.g_db = g;
    s_db.set_ann(ann_min, ann_max);

    // string outgrid_filename = outidx_filename + ".grid";
    // ofstream outgrid_ifs(outgrid_filename);

    // // write grid headers
    // if (!debug_mode) {
    //     outgrid_ifs << w << " " << ann_min << " " << ann_max << " " << g.cells_count << endl;
    //     outgrid_ifs << g.cells_box.min_id[0] << " " << g.cells_box.min_id[1] << " " << g.cells_box.min_id[2] << " "
    //                 << g.cells_box.max_id[0] << " " << g.cells_box.max_id[1] << " " << g.cells_box.max_id[2] << endl;
    // }
    
    RTree<int, double, 2> tree;
    int fail_count = 0;

    ProgressBar bar(g.cells_count, 70);

    for (auto it = g.cells_map.begin(); it != g.cells_map.end(); it++) {
        if (show_prog_bar) {
            ++bar;
            bar.display();
        }

        int key = it->first;
        Cell c = it->second;
        if (debug_mode) {
            printf("Processing cell (%d, %d, %d) with %d pts\n", c.x, c.y, c.z, c.list.size());
        }
        
        // // output per-cell information
        // if (!debug_mode) {
        //     outgrid_ifs << key << " " << c.to_str() << endl;
        // }

        Entry* prem_entry;
        // prem_entry.fail = true;
        // prem_entry.meas = -1;
        for (PtwID &p: c.list) {
            // Entry e = cal_index_entry(&c, p, ann_min, ann_max, &g, kd_p, debug_mode);
            Entry* e = cal_index_entry_new(p, ann_min, mesh_p, root, &db_rtree_info, debug_mode);
            // find premium entry having the largest meas
            if (e->meas > prem_entry->meas && e->fail == false) {
                prem_entry = e;
            } else {
            	delete e;
            }
        }

        if (debug_mode) {
            cout << TAB << "Prem entry: " << prem_entry->to_str() << endl;
        }

        // // output per-cell premium entry
        // if (!debug_mode && !test_mode) {
        //     outgrid_ifs << prem_entry.to_str() << endl;
        // }

        if (prem_entry->fail) {
            fail_count++;
        } else {
            double box_min[2] = { prem_entry->vol, prem_entry->meas },
                   box_max[2] = { prem_entry->vol, prem_entry->meas };
            tree.Insert(box_min, box_max, key);
        }

        if (debug_mode && distance(g.cells_map.begin(), it) >= 30) {
            break;
        }
    }

    if (show_prog_bar) {
        bar.done();
    }

    cout << "Total # of failed cells: " << fail_count << endl;

    // outgrid_ifs.close();
    // if (!debug_mode && !test_mode) {
    //     tree.Save(outidx_filename.c_str());
    // }

    cout << "Total time: " << timer_end(SECOND) << "(s)" << endl;
}