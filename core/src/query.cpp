#include "TriMesh.h"
#include "KDtree.h"
#include "point.h"
#include "tetra_meas.h"
#include "pcr_adv.h"
#include "RTree.h"
#include "share.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <unordered_map>
#include <unordered_set>
using namespace std;
using namespace trimesh;

vector<Entry> cal_entries(PtwID q, Pt3D pts_q[], int m, double min, double max) {
    vector<Entry> ret;

    // vector<PtwID> ann_pts;
    // for (int i_s = 0; i_s < m; i_s++) {
    //     Pt3D s = pts_q[i_s];
    //     double d = eucl_dist(s, q.pt);
    //     if (d - min >= 0 && d - max <= 0) {
    //         ann_pts.push_back({ i_s, s });
    //     }
    // }
    // int hit_size = ann_pts.size();
    // if (hit_size < 3) {
    //     return ret;
    // }

    // for (int i = 0; i < hit_size; i++) {
    //     for (int j = i + 1; j < hit_size; j++) {
    //         for (int k = j + 1; k < hit_size; k++) {
    //             auto ratio_set = get_ratio_set_vol(q.pt,
    //                 ann_pts[i].pt, ann_pts[j].pt, ann_pts[k].pt);
    //             Entry e;
    //             e.fail = false;
    //             e.repre = q;
    //             e.remai[0] = ann_pts[i];
    //             e.remai[1] = ann_pts[j];
    //             e.remai[2] = ann_pts[k];
    //             e.vol = ratio_set.volume;
    //             e.meas = ratio_set.ratio;
    //             sort_remai(e);
    //             ret.push_back(e);
    //         }
    //     }
    // }
    return ret;
}

void nn_sphere_range_and_show(const Struct_Q* s_q, Pt3D* q, node_type *r_root, rtree_info* r_info, double sq_dist, double err,
    vector<RangeReturn_type*>& ret, bool verbose = false, string nav = "", const unordered_set<int>& excl_id_set = {}) {

    auto nn = nn_sphere_range(q, r_root, r_info, sq_dist, err, ret, excl_id_set);
    if (verbose) {
        cout << nav << "nn_sphere_range-------NN: " << s_q->id_q_to_str(nn[0]->oid) << endl;
        cout << nav << "nn_sphere_range-------RR:";
        for (auto &it: ret) {
            cout << " " << s_q->id_q_to_str(it->oid);
        }
        cout << endl;
    }
    nn_sphere_free(nn);
}

int cal_entries_new(PtwID q, double min, const Struct_Q* s_q, const TriMesh* mesh_q, node_type *r_root_q, rtree_info* r_info,
    vector<Entry*>& v_ret, bool debug_mode) {

    cout.precision(12);

    int ret = 0;
    double epsilon = s_q->epsilon;

    vector<RangeReturn_type*> range_a, range_h, range_b, range_c;
    PtwID ten_a, ten_h, ten_b, ten_c;

    nn_sphere_range_and_show(s_q, &(q.pt), r_root_q, r_info, sq(min), epsilon * 2, range_a, true);

    for (auto &it_a: range_a) {
        if (it_a->oid < 0)
            continue;

        cout << endl << "For a = " << s_q->id_q_to_str(it_a->oid) << endl;
        ten_a = PtwID(it_a->oid, mesh_q);

        auto m = middle_pt(q.pt, ten_a.pt);
        float d_pm = eucl_dist(q.pt, m);

        nn_sphere_range_and_show(s_q, &m, r_root_q, r_info, sq(d_pm), epsilon * 2, range_h, true, "", { q.id, ten_a.id });

        for (auto &it_h: range_h) {
            if (it_h->oid < 0)
                continue;

            cout << endl << TAB << "For h = " << s_q->id_q_to_str(it_h->oid) << endl;
            ten_h = PtwID(it_h->oid, mesh_q);

            Pt3D ten_b_est, ten_c_est;
            auto got_b_c = get_est_b_c(&m, &(ten_a.pt), &(ten_h.pt), ten_b_est, ten_c_est);
            if (!got_b_c) {
                continue;
            }

            nn_sphere_range_and_show(s_q, &ten_b_est, r_root_q, r_info, 0.0, epsilon * 2, range_b, true, TAB, { q.id, ten_a.id });

            for (auto &it_b: range_b) {
                if (it_b->oid < 0)
                    continue;

                cout << endl << TABTAB << "For b = " << s_q->id_q_to_str(it_b->oid) << endl;
                ten_b = PtwID(it_b->oid, mesh_q);

                nn_sphere_range_and_show(s_q, &ten_c_est, r_root_q, r_info, 0.0, epsilon * 2, range_c, true, TABTAB, { q.id, ten_a.id, ten_b.id });

                for (auto &it_c: range_c) {
                    if (it_c->oid < 0)
                        continue;

                    cout << endl << TABTABTAB << "For c = " << s_q->id_q_to_str(it_b->oid) << endl;
                    ten_c = PtwID(it_c->oid, mesh_q);

                    // get the ratio set
                    auto ratio_set = get_ratio_set_vol(q.pt, ten_a.pt, ten_b.pt, ten_c.pt);

                    v_ret.push_back(new Entry(q, ten_a, ten_b, ten_c, ratio_set.volume, ratio_set.ratio, ten_h));

                    ret++;
                }

                range_free(range_c);
            }

            range_free(range_b);
        }

        range_free(range_h);
    }

    range_free(range_a);

    return ret;
}

bool check_congr(const Entry* e, const Entry* f, double epsilon) {
    epsilon = max(epsilon, 0.01);

    double e_edges[5], f_edges[5];
    e_edges[0] = eucl_dist(e->repre->pt, e->remai[0]->pt);
    e_edges[1] = eucl_dist(e->repre->pt, e->remai[1]->pt);
    e_edges[2] = eucl_dist(e->remai[0]->pt, e->remai[1]->pt);
    e_edges[3] = eucl_dist(e->repre->pt, e->remai[2]->pt);
    e_edges[4] = eucl_dist(e->remai[0]->pt, e->remai[2]->pt);
    f_edges[0] = eucl_dist(f->repre->pt, f->remai[0]->pt);
    f_edges[1] = eucl_dist(f->repre->pt, f->remai[1]->pt);
    f_edges[2] = eucl_dist(f->remai[0]->pt, f->remai[1]->pt);
    f_edges[3] = eucl_dist(f->repre->pt, f->remai[2]->pt);
    f_edges[4] = eucl_dist(f->remai[0]->pt, f->remai[2]->pt);
    for (int i = 0; i < 5; i++) {
        if (abs(e_edges[i] - f_edges[i]) > 2 * epsilon) {
            return false;
        }
    }
    return true;
}

vector<int> tree_search_return;

// TODO: improve performance by not using callback
bool tree_search_callback(int key) {
    // cout << key << ":\n";
    tree_search_return.push_back(key);
    return true;
}

void retrieve_congr_entry(Entry* e, double epsilon, RTree<int, double, 2>* tree,
    const Struct_DB* s_db, vector<Entry_Pair*>& ret) {

    double low[2], high[2];
    cal_range(e->repre->pt, e->remai[0]->pt, e->remai[1]->pt, e->remai[2]->pt, epsilon,
        low[0], high[0], low[1], high[1]);

    // print_pt(&(e->repre->pt));
    // cout << endl;
    // print_pt(&(e->remai[0]->pt));
    // cout << endl;
    // print_pt(&(e->remai[1]->pt));
    // cout << endl;
    // print_pt(&(e->remai[2]->pt));
    // cout << endl;

    // cout << low[0] << endl;
    // cout << high[0] << endl;
    // cout << low[1] << endl;
    // cout << high[1] << endl;

    int nhits = tree->Search(low, high, tree_search_callback);

    for (int &hit_key: tree_search_return) {
        Entry* f = s_db->get_entry(hit_key);
        // cout << "tree search result: " << f->to_str() << endl;
        if (check_congr(e, f, epsilon)) {
            Entry_Pair* new_pair = new Entry_Pair(e, f);
            cout << "Found pair: " << new_pair->to_str() << endl;
            ret.push_back(new_pair);
        }
    }
    tree_search_return.clear();
}

bool test_verification_cell(const Cell* c, const Struct_Q* s_q, const Struct_DB* s_db, bool debug_mode) {
    int matching_repre_point_count = 0, matching_all_point_count = 0;

    // loop through all points inside the cell
    for (auto &p: c->list) {
        // get point ID mapped in DB
        int mapping = s_q->get_id_mapping(p.id);
        if (mapping >= 0) {
            if (s_db->look_up_repre_index(mapping)) { // if mapping is a repre point in DB
                if (debug_mode) cout << "Query pt #" << p.id << " matched with a repre. pt #" << mapping << endl;
                matching_repre_point_count++;

                // check whether the remaining and help points appear in query
                bool all_appearing = true;
                for (int i = 0; i < 3; i++) {
                    int remai_id = s_db->get_remai_id(mapping, i);
                    if (!s_q->look_up_idmap(remai_id)) {
                        if (debug_mode) cout << "But remai[" << i << "] (db#" << remai_id << ") not appearing in the query" << endl;
                        all_appearing = false;
                    }
                }
                int help_id = s_db->get_help_id(mapping);
                if (!s_q->look_up_idmap(help_id)) {
                    if (debug_mode) cout << "But help pt (db#" << help_id << ") not appearing in the query" << endl;
                    all_appearing = false;
                }

                if (all_appearing) {
                    matching_all_point_count++;
                }
            }
        }
    }
    if (debug_mode) cout << "Matching repre: " << matching_repre_point_count << endl;
    if (debug_mode) cout << "Matching all: " << matching_all_point_count << endl;

    if (matching_all_point_count > 0) { // if at least 1 all_matching point found
        return true;
    } else {
        return false;
    }
}

void test_verification(const Struct_Q* s_q, const Struct_DB* s_db, bool debug_mode) {
    cout << "Test verification..." << endl;

    int counter = 0, matched_cells_count = 0;

    // loop through all query cells
    for (auto it = s_q->g_q->cells_map.begin(); it != s_q->g_q->cells_map.end(); it++) {
        if (debug_mode) cout << "For query cell #" << counter << endl;

        if (test_verification_cell(it->second, s_q, s_db, debug_mode)) {
            matched_cells_count++;
        }
        counter++;
    }
    cout << "Summary: " << matched_cells_count << "/" << s_q->g_q->cells_count << " cells are matched" << endl;

}

Cell* get_random_q_cell(const Grid* g_q, int& selected_cell_id) {
    selected_cell_id = 1582;//rand() % g_q->cells_count;
    auto ptr = g_q->cells_map.begin();
    for (int i = 0; i < selected_cell_id; i++, ptr++);
    return ptr->second;
}

int main(int argc, char **argv) {
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " database_filename db_rstree_filename index_filename query_filename q_rstree_filename [-test] [-debug]" << endl;
        exit(1);
    }

    bool test_mode = false, debug_mode = false;
    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-test") {
            test_mode = true;
        } else if (string(argv[i]) == "-debug") {
            debug_mode = true;
        }
    }

    int argi = 0;
    string database_filename = argv[(++argi)];
    string db_rstree_filename = argv[(++argi)];
    string idx_filename = argv[(++argi)];
    string query_filename = argv[(++argi)];
    string query_rstree_filename = argv[(++argi)];

    srand(time(NULL));

    timer_start();

    cout << "Reading dababase file..." << endl;
    TriMesh *mesh_p = TriMesh::read(database_filename);

    // KDtree *kdtree_p = new KDtree(mesh_p->vertices);
    
    // rtree_info db_rtree_info = { 5, 10, 3, 7 };
    // node_type *root_p;

    // // load the database R-tree
    // cout << "Loading R-tree for DB points..." << endl;
    // read_rtree(&root_p, db_rstree_filename.c_str(), &db_rtree_info);

    // load the index entries tree
    cout << "Loading index entries..." << endl;
    RTree<int, double, 2> tree;
    tree.Load(idx_filename.c_str());

    // load the DB structure
    cout << "Loading DB structure..." << endl;
    Struct_DB s_db;
    s_db.g_db = new Grid();
    s_db.read(idx_filename + ".grid", mesh_p);

    cout << "Reading query file..." << endl;
    TriMesh *mesh_q = TriMesh::read(query_filename);

    rtree_info query_rtree_info = { 5, 10, 3, 7 };
    node_type *root_q;

    // load the database R-tree
    cout << "Loading R-tree for query points..." << endl;
    read_rtree(&root_q, query_rstree_filename.c_str(), &query_rtree_info);

    // load the query structure
    Struct_Q s_q;
    s_q.read(query_filename + ".info", query_filename + ".map", mesh_q);

    cout << "Total I/O time: " << timer_end(SECOND) << "(s)" << endl;

    cout << endl;

    int total_num_verification = 0;
    double total_time = 0.0, proposal_time = 0.0, verification_time = 0.0;
    timer_start();

    double w_q = 3.464102 * s_db.g_db->w;
    cout << "Query voxelized of grid size " << w_q << endl;

    s_q.g_q = new Grid(w_q);
    s_q.g_q->gridify(mesh_q);

    cout << "Total number of query cells: " << s_q.g_q->cells_count << endl;

    if (debug_mode) {
        cout << "Displaying info of each query cell:" << endl;
        for (auto it = s_q.g_q->cells_map.begin(); it != s_q.g_q->cells_map.end(); it++) {
            cout << it->second->x << " "
                 << it->second->y << " "
                 << it->second->z << " "
                 << it->second->list.size() << endl;
        }
        cout << endl;
    }

    // test verification
    if (test_mode) {
        test_verification(&s_q, &s_db, debug_mode);
    }

    cout << endl;

    int k_m = 10;
    int verified_size = 0;
    for (int t = 0; t < k_m; t++) {
        int selected_cell_id;
        Cell* selected_cell = get_random_q_cell(s_q.g_q, selected_cell_id);
        cout << "Selecting cell #" << selected_cell_id
             << " with " << selected_cell->list.size() << " pts" << endl;

        if (debug_mode) {
            if(!test_verification_cell(selected_cell, &s_q, &s_db, true)) {
                continue;
            }
        }

        vector<Entry_Pair*> v_pairs;

        for (auto &q: selected_cell->list) {
            if (!(q.id == 67776 || q.id == 69652)) {
                continue;
            }
            // if (debug_mode)
            cout << "\nCalculating entry list for query pt #" << q.id << endl;
            vector<Entry*> v_entries;
            int num_entries = cal_entries_new(q, s_db.ann_min, &s_q, mesh_q, root_q, &query_rtree_info, v_entries, debug_mode);
            cout << endl << "Size of the entry list: " << num_entries << endl;

            for (auto &e: v_entries) {
                retrieve_congr_entry(e, s_q.epsilon, &tree, &s_db, v_pairs);
            }
        }
        break;

        cout << endl << "Returned hit size is " << v_pairs.size() << endl;
        total_num_verification += hit_list.size();

        // timer_start();
        // verified_size = 0;
        // for (auto &h: hit_list) {
        //     // cout << h.to_str() << endl;
        //     h.cal_xf();

        //     // double result = cal_corr_err(mesh_q, kdtree_p, &h.xf, 50000);
        //     // if (result > 0) {
        //     //     // cout << h.to_str() << endl
        //     //     //      << err << endl;
        //     //     verified_size++;
        //     // }
        // }
        // verification_time += timer_end(SECOND);

        // if (verified_size > 0) {
        //     break;
        // }

    }

    total_time = timer_end(SECOND);
    proposal_time = total_time - verification_time;

    cout << endl;
    cout << "Total number of candidate transformations: " << total_num_verification << endl;
    cout << "Final number of valid transformations: " << verified_size << endl;
    cout << "Total time: " << total_time << "(s)" << endl;
    cout << "Proposal time: " << proposal_time << "(s)" << endl;
    cout << "Verification time: " << verification_time << "(s)" << endl;

}