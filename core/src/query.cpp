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

    NN_type** nn;
    if (verbose) {
        nn = nn_sphere_range_verbose(q, r_root, r_info, sq_dist, err, ret, excl_id_set);
    } else {
        nn = nn_sphere_range(q, r_root, r_info, sq_dist, err, ret, excl_id_set);
    }
    if (verbose) {
        cout << nav << "nn_sphere_range-------NN: " << s_q->id_q_to_str(nn[0]->oid) << "=" << sqrt(nn[0]->dist) << endl;
        cout << nav << "nn_sphere_range-------RR:";
        for (auto &it: ret) {
            cout << " " << s_q->id_q_to_str(it->oid) << "=" << sqrt(it->dist);
        }
        cout << endl;
    }
    nn_sphere_free(nn);
}

bool insert_entry(vector<Entry*>& v_ret, Entry* new_entry) {
    for (auto &v: v_ret) {
        if (new_entry->repre->id == v->repre->id &&
            new_entry->remai[0]->id == v->remai[0]->id &&
            new_entry->remai[1]->id == v->remai[1]->id &&
            new_entry->remai[2]->id == v->remai[2]->id) {

            return false; // skip for repeating entry
        }
    }

    v_ret.push_back(new_entry);
    
}

int cal_entries_new(PtwID q, double min, const Struct_Q* s_q, const TriMesh* mesh_q, node_type *r_root_q, rtree_info* r_info,
    vector<Entry*>& v_ret, bool debug_mode) {

    v_ret.clear();

    cout.precision(12);

    int ret = 0;
    double epsilon = s_q->epsilon;

    vector<RangeReturn_type*> range_a, range_h, range_b, range_c;
    PtwID ten_a, ten_h, ten_b, ten_c;

    nn_sphere_range_and_show(s_q, &(q.pt), r_root_q, r_info, sq(min), epsilon * 2, range_a, true);

    for (auto &it_a: range_a) {
        if (it_a->oid < 0)
            continue;

        // cout << endl << "For a = " << s_q->id_q_to_str(it_a->oid) << endl;
        ten_a = PtwID(it_a->oid, mesh_q);

        auto m = middle_pt(q.pt, ten_a.pt);
        float d_pm = eucl_dist(q.pt, m);

        nn_sphere_range_and_show(s_q, &m, r_root_q, r_info, sq(d_pm), epsilon * 2, range_h, debug_mode, "", { q.id, ten_a.id });

        for (auto &it_h: range_h) {
            if (it_h->oid < 0)
                continue;

            if (debug_mode) cout << endl << TAB << "For h = " << s_q->id_q_to_str(it_h->oid) << endl;
            ten_h = PtwID(it_h->oid, mesh_q);

            Pt3D ten_b_est, ten_c_est;
            auto got_b_c = get_est_b_c(&m, &(ten_a.pt), &(ten_h.pt), ten_b_est, ten_c_est);
            if (!got_b_c) {
                continue;
            }

            nn_sphere_range_and_show(s_q, &ten_b_est, r_root_q, r_info, 0.0, epsilon * 2, range_b, debug_mode, TAB, { q.id, ten_a.id });

            for (auto &it_b: range_b) {
                if (it_b->oid < 0)
                    continue;

                if (debug_mode) cout << endl << TABTAB << "For b = " << s_q->id_q_to_str(it_b->oid) << endl;
                ten_b = PtwID(it_b->oid, mesh_q);

                nn_sphere_range_and_show(s_q, &ten_c_est, r_root_q, r_info, 0.0, epsilon * 2, range_c, debug_mode, TABTAB, { q.id, ten_a.id, ten_b.id });

                for (auto &it_c: range_c) {
                    if (it_c->oid < 0)
                        continue;

                    if (debug_mode) cout << endl << TABTABTAB << "For c = " << s_q->id_q_to_str(it_c->oid) << endl;
                    ten_c = PtwID(it_c->oid, mesh_q);

                    // get the ratio set
                    auto ratio_set = get_ratio_set_vol(q.pt, ten_a.pt, ten_b.pt, ten_c.pt);

                    // if (ten_a.id == 135212 && ten_b.id == 132053 && ten_c.id == 134604) { // for test only

                    Entry* q_entry = new Entry(q, ten_a, ten_b, ten_c, ratio_set.volume, ratio_set.ratio, ten_h);

                    if (insert_entry(v_ret, q_entry)) {

                        if (debug_mode) cout << TABTABTAB << "Include query entry: " << q_entry->to_str() << endl;
                        ret++;
                    }

                	// } // for test only
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

// void retrieve_congr_entry(Entry* e, double epsilon, RTree<int, double, 2>* tree,
//     const Struct_DB* s_db, vector<Entry_Pair*>& ret) {

//     double low[2], high[2];
//     cal_range(e->repre->pt, e->remai[0]->pt, e->remai[1]->pt, e->remai[2]->pt, epsilon,
//         low[0], high[0], low[1], high[1]);

//     // print_pt(&(e->repre->pt));
//     // cout << endl;
//     // print_pt(&(e->remai[0]->pt));
//     // cout << endl;
//     // print_pt(&(e->remai[1]->pt));
//     // cout << endl;
//     // print_pt(&(e->remai[2]->pt));
//     // cout << endl;

//     cout << low[0] << endl;
//     cout << high[0] << endl;
//     cout << low[1] << endl;
//     cout << high[1] << endl;

//     int nhits = tree->Search(low, high, tree_search_callback);

//     for (int &hit_key: tree_search_return) {
//         Entry* f = s_db->get_entry(hit_key);
//         // cout << "tree search result: " << f->to_str() << endl;
//         if (check_congr(e, f, epsilon)) {
//             Entry_Pair* new_pair = new Entry_Pair(e, f);
//             // cout << "Found pair: " << new_pair->to_str() << endl;
//             ret.push_back(new_pair);
//         }
//     }
//     tree_search_return.clear();
// }

void retrieve_congr_entry(Entry* e, double epsilon, RTree<int, double, 6>* tree,
    const Struct_DB* s_db, vector<Entry_Pair*>& ret) {

	e->fill_sides();
	e->sort_sides();

	double low[6], high[6];
	double corr_epsilon = max(0.001, epsilon);
	for (int i = 0; i < 6; i++) {
		low[i] = e->sides[i] - 2 * epsilon;
		high[i] = e->sides[i] + 2 * epsilon;

		// cout << "[" << low[i] << ", " << high[i] << "]" << endl;
	}

	int nhits = tree->Search(low, high, tree_search_callback);

	for (int &hit_key: tree_search_return) {
        Entry* f = s_db->get_entry(hit_key);
        // if (!(f->repre->id == 942286 || f->repre->id == 942288 || f->repre->id == 942438 || f->repre->id == 942440 || f->repre->id == 942441)) {
        //     continue;
        // }
        Entry_Pair* new_pair = new Entry_Pair(e, f);
        // cout << "Found pair: " << new_pair->to_str() << endl;
        ret.push_back(new_pair);
    }
    tree_search_return.clear();
}

bool test_verification_cell(const Cell* c, const Struct_Q* s_q, const Struct_DB* s_db, bool verbose) {
    int matching_repre_point_count = 0, matching_all_point_count = 0;
    const int db_mesh_id = s_q->get_db_mesh_id();

    // loop through all points inside the cell
    for (auto &p: c->list) {
        // get point ID mapped in DB
        const int mapping = s_q->get_id_mapping(p.id);
        if (mapping >= 0) {
            if (s_db->look_up_repre_index(mapping, db_mesh_id)) { // if mapping is a repre point in DB
                if (verbose) cout << "Query pt #" << p.id << " matched with a repre. pt #" << mapping << endl;
                matching_repre_point_count++;

                // check whether the remaining and help points appear in query
                bool all_appearing = true;
                for (int i = 0; i < 3; i++) {
                    const int remai_id = s_db->get_remai_id(mapping, i, db_mesh_id);
                    if (!s_q->look_up_id_db(remai_id)) {
                        if (verbose) cout << "But remai[" << i << "] (db#" << remai_id << ") not appearing in the query" << endl;
                        all_appearing = false;
                    }
                }
                int help_id = s_db->get_help_id(mapping, db_mesh_id);
                if (!s_q->look_up_id_db(help_id)) {
                    if (verbose) cout << "But help pt (db#" << help_id << ") not appearing in the query" << endl;
                    all_appearing = false;
                }

                if (all_appearing) {
                    matching_all_point_count++;
                }
            }
        }
    }
    if (verbose) cout << "Matching repre: " << matching_repre_point_count << endl;
    if (verbose) cout << "Matching all: " << matching_all_point_count << endl;

    if (matching_all_point_count > 0) { // if at least 1 all_matching point found
        return true;
    } else {
        return false;
    }
}

void test_verification(const Struct_Q* s_q, const Struct_DB* s_db, bool verbose) {
    cout << "Test verification..." << endl;

    int counter = 0, matched_cells_count = 0;

    // loop through all query cells
    for (auto it = s_q->g_q->cells_map.begin(); it != s_q->g_q->cells_map.end(); it++) {
        if (verbose) cout << "For query cell #" << counter << endl;

        if (test_verification_cell(it->second, s_q, s_db, verbose)) {
            matched_cells_count++;
        }
        counter++;
    }
    cout << "Summary: " << matched_cells_count << "/" << s_q->g_q->cells_map.size() << " cells are matched" << endl;

}

Cell* get_random_q_cell(const Grid* g_q, int& selected_cell_id, int cheating = -1) {
    if (cheating < 0) {
        selected_cell_id = rand() % g_q->cells_map.size();
    } else {
        selected_cell_id = cheating;
    }
    auto ptr = g_q->cells_map.begin();
    for (int i = 0; i < selected_cell_id; i++, ptr++);
    return ptr->second;
}

int main(int argc, char **argv) {
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " database_filename db_rstree_filename index_filename query_filename q_rstree_filename delta [-batch] [-test] [-debug]" << endl;
        exit(1);
    }

    bool test_mode = false, debug_mode = false, batch_mode = false;
    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-test") {
            test_mode = true;
        } else if (string(argv[i]) == "-debug") {
            debug_mode = true;
        } else if (string(argv[i]) == "-batch") {
            batch_mode = true;
        }
    }

    int argi = 0;
    string database_filename = argv[(++argi)];
    string db_rstree_filename = argv[(++argi)];
    string idx_filename = argv[(++argi)];
    string query_filename = argv[(++argi)];
    string query_rstree_filename = argv[(++argi)];

    double delta = atof(argv[(++argi)]);

    srand(time(NULL));

    timer_start();

    cout << "Reading database file..." << endl;
    unordered_map<int, TriMesh*> db_meshes;
    if (batch_mode) {
        read_db_mesh_batch(database_filename, db_meshes);
    } else {
        db_meshes[0] = TriMesh::read(database_filename);
    }
    // TriMesh *mesh_p = TriMesh::read(database_filename);

    unordered_map<int, KDtree*> db_kds;
    for (auto &p: db_meshes) {
        db_kds[p.first] = new KDtree(p.second->vertices);

    }
    
    // rtree_info db_rtree_info = { 5, 10, 3, 7 };
    // node_type *root_p;

    // // load the database R-tree
    // cout << "Loading R-tree for DB points..." << endl;
    // read_rtree(&root_p, db_rstree_filename.c_str(), &db_rtree_info);

    // load the index entries tree
    cout << "Loading index entries..." << endl;
    // RTree<int, double, 2> tree;
    RTree<int, double, 6> tree;
    tree.Load(idx_filename.c_str());

    // load the DB structure
    cout << "Loading DB structure..." << endl;
    Struct_DB s_db;
    // s_db.g_db = new Grid();
    s_db.read(idx_filename + ".grid", db_meshes);

    cout << "Reading query file..." << endl;
    TriMesh *mesh_q = TriMesh::read(query_filename);

    rtree_info query_rtree_info = { 5, 10, 3, 7 };
    node_type *root_q;

    // load the database R-tree
    cout << "Loading R-tree for query points..." << endl;
    read_rtree(&root_q, query_rstree_filename.c_str(), &query_rtree_info);

    // load the query structure
    Struct_Q s_q;
    s_q.read(query_filename + ".info");

    cout << "Total I/O time: " << timer_end(SECOND) << "(s)" << endl;

    cout << endl;

    int total_num_verification = 0;
    double total_time = 0.0, proposal_time = 0.0, verification_time = 0.0;
    timer_start();

    double w_q = 3.464102 * s_db.get_w();
    cout << "Query voxelized of grid size " << w_q << endl;

    s_q.g_q = new Grid(w_q);
    s_q.g_q->gridify(mesh_q);

    cout << "Total number of query cells: " << s_q.g_q->cells_map.size() << endl;

    // cout << "Displaying info of each query cell:" << endl;
    // for (auto it = s_q.g_q->cells_map.begin(); it != s_q.g_q->cells_map.end(); it++) {
    //     cout << it->second->x << " "
    //          << it->second->y << " "
    //          << it->second->z << " "
    //          << it->second->list.size() << endl;
    // }
    // cout << endl;

    // test verification
    if (test_mode) {
        test_verification(&s_q, &s_db, debug_mode);
    }

    cout << endl;

    int k_m = 10;
    int verified_size = 0;
    for (int t = 0; t < k_m; t++) {
        int selected_cell_id;
        Cell* selected_cell = get_random_q_cell(s_q.g_q, selected_cell_id, 9103);
        cout << "Selecting cell #" << selected_cell_id
             << " with " << selected_cell->list.size() << " pts" << endl;

        if (test_mode) {
            if(!test_verification_cell(selected_cell, &s_q, &s_db, true)) {
                continue;
            }
        }

        vector<Entry_Pair*> v_pairs;

        for (auto &q: selected_cell->list) {
            if (!(q.id == 258119 || q.id == 259183 || q.id == 259185 || q.id == 259190 || q.id == 259765 || q.id == 262949)) {
                continue;
            }
            
            cout << endl << "Calculating entry list for query pt #" << q.id << endl;

            vector<Entry*> v_entries;
            int num_entries = cal_entries_new(q, s_db.get_ann_min(), &s_q, mesh_q, root_q, &query_rtree_info, v_entries, debug_mode);

            cout << endl << "Size of the entry list: " << num_entries << endl;

            for (auto &e: v_entries) {
                retrieve_congr_entry(e, s_q.epsilon, &tree, &s_db, v_pairs);
            }
        }

        cout << endl << "Returned hit size is " << v_pairs.size() << endl << endl;
        if (debug_mode) {
        	for (auto &p: v_pairs) {
        		cout << p->to_str() << endl;
        	}
        }

        total_num_verification += v_pairs.size();

        timer_start();
        verified_size = 0;
        for (auto &h: v_pairs) {
            // cout << h->to_str() << endl;
            h->cal_xf();

            double result = cal_corr_err(mesh_q, db_kds[0], &h->xf, delta);
            if (result > 0) {
                cout << "Accept: " << h->to_str() << endl
                     << result << endl;
                verified_size++;
            }
        }
        verification_time += timer_end(SECOND);

        if (verified_size > 0) {
            break;
        }

        break;

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