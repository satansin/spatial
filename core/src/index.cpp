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

string get_foldername(string path) {
    string ret;
    if (path[path.length() - 1] != '/') {
        ret = path + "/";
    } else {
        ret = path;
    }
    return ret;
}

int read_db_mesh_batch(string db_path, unordered_map<int, TriMesh*>& db_meshes) {
    string db_folder = get_foldername(db_path);

    ifstream ifs(db_folder + "meta.txt");

    int num;
    ifs >> num;

    int id;
    string s_file;
    for (int i = 0; i < num; i++) {
        ifs >> id >> s_file;
        db_meshes[id] = TriMesh::read(s_file);
    }

    ifs.close();

    return num;
}

void read_db_rstree_batch(string rstree_path, const unordered_map<int, TriMesh*>& db_meshes, rtree_info* db_rtree_info, unordered_map<int, node_type*>& db_roots) {
    string rstree_folder = get_foldername(rstree_path);

    for (auto &p: db_meshes) {
        node_type* a_root;
        string s_file = rstree_folder + "id." + to_string(p.first) + ".rstree.1";

        read_rtree(&a_root, s_file.c_str(), db_rtree_info);

        db_roots[p.first] = a_root;
    }
}

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

void cal_index_entry_new(PtwID p, double min, const TriMesh* mesh_p, node_type *r_root, rtree_info* r_info, bool debug_mode, Entry* prem_entry) {

	PtwID a, b, c, h;

    if (debug_mode) cout << TAB << "Pt #" << p.id << endl;

    // start looking for the first subsidiary pt a
    if (debug_mode) timer_start();

    auto nn_a = nn_sphere(&(p.pt), r_root, r_info, sq(min))[0];

    if (debug_mode) cout << TABTAB << "First pt #" << nn_a->oid << " dist=" << sqrt(nn_a->dist) << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    // start looking for the help pt h
    if (debug_mode) timer_start();

    if (nn_a->oid >= 0) {
     	a = PtwID(nn_a->oid, mesh_p);
    } else {
    	return;
    }

    auto m = middle_pt(p.pt, a.pt);

    float d_pm = eucl_dist(p.pt, m);
    auto nn_h = nn_sphere(&m, r_root, r_info, sq(d_pm), { p.id, a.id })[0];

    if (debug_mode) cout << TABTAB << "Help pt #" << nn_h->oid << " dist=" << sqrt(nn_h->dist) << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    if (nn_h->oid >= 0) {
     	h = PtwID(nn_h->oid, mesh_p);
    } else {
    	return;
    }

    if (debug_mode) { cout << TABTAB; printf("hp=%f, ha=%f\n", eucl_dist(h.pt, p.pt), eucl_dist(h.pt, a.pt)); }

    // start looking for the rest two pts b and c
    if (debug_mode) timer_start();

    Pt3D b_est, c_est;
    auto got_b_c = get_est_b_c(&m, &(a.pt), &(h.pt), b_est, c_est);
    if (!got_b_c) {
    	return;
    }

    auto nn_b = nn_sphere(&b_est, r_root, r_info, 0.0, { p.id, a.id })[0];
    if (nn_b->oid >= 0) {
     	b = PtwID(nn_b->oid, mesh_p);
    } else {
    	return;
    }

    auto nn_c = nn_sphere(&c_est, r_root, r_info, 0.0, { p.id, a.id, b.id })[0];
    if (nn_c->oid >= 0) {
     	c = PtwID(nn_c->oid, mesh_p);
    } else {
    	return;
    }

    if (debug_mode) cout << TABTAB << "Second pt #" << nn_b->oid << " dist=" << sqrt(nn_b->dist)
    	<< ", third pt #" << nn_c->oid << " dist=" << sqrt(nn_c->dist) << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    // get the ratio set
    auto ratio_set = get_ratio_set_vol(p.pt, a.pt, b.pt, c.pt);

    if (ratio_set.ratio - prem_entry->meas > 0) {
    	prem_entry->set(p, a, b, c, ratio_set.volume, ratio_set.ratio, h);
    	prem_entry->fail = false;
    }

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
        cout << "\tTotal # cells: " << g.cells_map.size() << endl;
        cout << "\tAvg # pts per cell: " << ((double) mesh->vertices.size()) / ((double) g.cells_map.size()) << endl;
    }

}

int main(int argc, char **argv) {
    if (argc < 6) {
        cerr << "Usage: " << argv[0] << " database_filename db_rstree_filename w ann_min ann_max output_index_filename [-batch] [-test] [-show_prog_bar] [-debug]" << endl;
        exit(1);
    }

    bool test_mode = false, show_prog_bar = false, debug_mode = false, batch_mode = false;
    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-test") {
            test_mode = true;
        } else if (string(argv[i]) == "-show_prog_bar") {
            show_prog_bar = true;
        } else if (string(argv[i]) == "-debug") {
            debug_mode = true;
        } else if (string(argv[i]) == "-batch") {
            batch_mode = true;
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

    unordered_map<int, TriMesh*> db_meshes;

    if (batch_mode) {
        read_db_mesh_batch(database_filename, db_meshes);
    } else {
        db_meshes[0] = TriMesh::read(database_filename);
    }

    long long n = 0;
    for (auto &p: db_meshes) {
        n += p.second->vertices.size();
    }

    // KDtree *kd_p = new KDtree(mesh_p->vertices);
    
    rtree_info db_rtree_info = { 5, 10, 3, 7 };

    unordered_map<int, node_type*> roots;

    // load R-tree
    cout << "Loading R-tree for DB points..." << endl;
    timer_start();
    if (batch_mode) {
        read_db_rstree_batch(db_rstree_filename, db_meshes, &db_rtree_info, roots);
    } else {
        read_rtree(&roots[0], db_rstree_filename.c_str(), &db_rtree_info);
    }
    cout << "Load R-tree of DB pts in " << timer_end(SECOND) << "(s)" << endl;

    cout << endl;
    cout << "Total I/O time: " << timer_end(SECOND) << "(s)" << endl;

    // performance_test(mesh_p, n, kd_p, root, &db_rtree_info);

    // gridify_test(w, db_meshes[0]);

    Struct_DB s_db;
    s_db.set_w(w);
    s_db.set_ann(ann_min, ann_max);

    cout << "\nGridify the point cloud..." << endl;
    timer_start();

    for (auto &p: db_meshes) {
        int mesh_id = p.first;
        TriMesh* mesh_p = p.second;

        Grid* g = new Grid(w);
        g->gridify(mesh_p);

        s_db.insert_grid(mesh_id, g);
    }

    cout << "Gridify finished in " << timer_end(SECOND) << "(s)" << endl;

    int num_cells = s_db.get_total_cells_count();

    cout << "Grid size: " << w << endl;
    cout << "Total # cells: " << num_cells << endl;
    cout << "Avg # pts per cell: " << ((long double) n) / ((double) num_cells) << endl << endl;

    // exit(0);

    
    // RTree<int, double, 2> tree;
    RTree<int, double, 6> tree; // edge index
    int fail_count = 0;

    int global_cell_index = 0;

    for (auto &v: s_db.get_grids_map()) {
        int mesh_id = v.first;
        auto g = v.second;
        auto mesh_p = db_meshes[mesh_id];
        auto root = roots[mesh_id];

        cout << "Processing mesh #" << mesh_id << endl << endl;

        ProgressBar bar(g->cells_map.size(), 70);

        for (auto it = g->cells_map.begin(); it != g->cells_map.end(); it++) {
            if (show_prog_bar) {
                ++bar;
                bar.display();
            }

            int global_id = (global_cell_index++);
            auto c = it->second;
            c->set_global_id(global_id);

            if (debug_mode) {
                printf("Processing cell #%d (%d, %d, %d) with %d pts\n", global_id, c->x, c->y, c->z, c->list.size());
            }

            auto prem_entry = new Entry();
            prem_entry->fail = true;
            // if (global_id < 50) { // uncomment it for testing
            for (auto &p: c->list) {
                // Entry e = cal_index_entry(&c, p, ann_min, ann_max, &g, kd_p, debug_mode);
                cal_index_entry_new(p, ann_min, mesh_p, root, &db_rtree_info, debug_mode, prem_entry);
            }
            // } // uncomment it for testing

            if (prem_entry->fail) {
                if (debug_mode) cout << TAB << "Fail in finding prem entry" << endl;
                fail_count++;
            } else {
                if (debug_mode) cout << TAB << "Prem entry: " << prem_entry->to_str() << endl;

                // for edge index
                prem_entry->fill_sides();
                prem_entry->sort_sides();
            }

            // double box_min[2] = { prem_entry->vol, prem_entry->meas },
            //     box_max[2] = { prem_entry->vol, prem_entry->meas };
            // tree.Insert(box_min, box_max, key);
            tree.Insert(prem_entry->sides, prem_entry->sides, global_id);

            s_db.insert_entry(global_id, prem_entry);

        }

        if (show_prog_bar) {
            bar.done();
        }
    }

    cout << "Total # of failed cells: " << fail_count << endl;

    if (!debug_mode && !test_mode) {
    	timer_start();
        tree.Save(outidx_filename.c_str());
        cout << "Tree saved in " << timer_end(SECOND) << "(s)" << endl;

    	timer_start();
        s_db.save(outidx_filename + ".grid");
        cout << "Grid saved in " << timer_end(SECOND) << "(s)" << endl;
    }

    cout << "Total time: " << timer_end(SECOND) << "(s)" << endl;
}