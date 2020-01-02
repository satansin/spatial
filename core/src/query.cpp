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

    NN_type** nn;
    if (verbose) {
        nn = nn_sphere_range_verbose(q, r_root, r_info, sq_dist, err, nav, ret, excl_id_set);
    } else {
        nn = nn_sphere_range(q, r_root, r_info, sq_dist, err, ret, excl_id_set);
    }
    if (verbose) {
        cout << nav << "nn_sphere_range-------NN: " << s_q->id_q_to_str(nn[0]->oid) << "=" << (sqrt(nn[0]->dist) / RSTREE_SCALE) << endl;
        cout << nav << "nn_sphere_range-------RR:";
        for (auto &it: ret) {
            cout << " " << s_q->id_q_to_str(it->oid) << "=" << (sqrt(it->dist) / RSTREE_SCALE);
        }
        cout << endl;
    }
    nn_sphere_free(nn);
}

string get_string_hash(Entry* e) {
    stringstream ss;
    ss << setw(7) << setfill('0') << e->remai[0]->id;
    ss << setw(7) << setfill('0') << e->remai[1]->id;
    ss << setw(7) << setfill('0') << e->remai[2]->id;
    return ss.str();
}

long long get_number_hash(Entry* e) {
    long long hash;
    int sum1 = e->remai[0]->id + e->remai[1]->id;
    hash = sum1 * (sum1 + 1) / 2 + e->remai[1]->id;
    long long sum2 = hash + e->remai[2]->id;
    hash = sum2 * (sum2 + 1) / 2 + e->remai[2]->id;
    return hash;
}

bool insert_entry(Entry* new_entry, vector<Entry*>& v_ret, unordered_set<long long>& hash_ret) {
    // string new_hash = get_string_hash(new_entry);
    long long new_hash = get_number_hash(new_entry);
    if (hash_ret.insert(new_hash).second) {
        v_ret.push_back(new_entry);
        return true;
    } else {
        return false;
    }
    
}

int cal_entries_new(PtwID q, double min, const Struct_Q* s_q, const TriMesh* mesh_q, node_type *r_root_q, rtree_info* r_info,
    vector<Entry*>& v_ret, int verbose = 0) {

    v_ret.clear();
    
    unordered_set<long long> hash_ret;

    cout.precision(10);

    int ret = 0;
    // double epsilon = s_q->epsilon;
    double err = s_q->epsilon * 2;

    vector<RangeReturn_type*> range_a, range_h, range_b, range_c;
    PtwID ten_a, ten_h, ten_b, ten_c;

    nn_sphere_range_and_show(s_q, &(q.pt), r_root_q, r_info, sq(min - err), err, range_a, (verbose > 0));

    for (auto &it_a: range_a) {
        if (it_a->oid < 0)
            continue;

        if (verbose > 1)
        	cout << endl << "For a = " << s_q->id_q_to_str(it_a->oid) << endl;

        ten_a = PtwID(it_a->oid, mesh_q);

        auto m = middle_pt(q.pt, ten_a.pt);
        float d_pm = eucl_dist(q.pt, m);

        nn_sphere_range_and_show(s_q, &m, r_root_q, r_info, sq(d_pm - err), err, range_h, (verbose > 1), "", { q.id, ten_a.id });

        for (auto &it_h: range_h) {
            if (it_h->oid < 0)
                continue;

            if (verbose > 2)
            	cout << endl << TAB << "For h = " << s_q->id_q_to_str(it_h->oid) << endl;

            ten_h = PtwID(it_h->oid, mesh_q);

            Pt3D ten_b_est, ten_c_est;
            auto got_b_c = get_est_b_c(&m, &(ten_a.pt), &(ten_h.pt), ten_b_est, ten_c_est);
            if (!got_b_c) {
                continue;
            }

            nn_sphere_range_and_show(s_q, &ten_b_est, r_root_q, r_info, 0.0, err, range_b, (verbose > 2), TAB, { q.id, ten_a.id });

            for (auto &it_b: range_b) {
                if (it_b->oid < 0)
                    continue;

                if (verbose > 3)
                	cout << endl << TABTAB << "For b = " << s_q->id_q_to_str(it_b->oid) << endl;

                ten_b = PtwID(it_b->oid, mesh_q);

                nn_sphere_range_and_show(s_q, &ten_c_est, r_root_q, r_info, 0.0, err, range_c, (verbose > 3), TABTAB, { q.id, ten_a.id, ten_b.id });

                for (auto &it_c: range_c) {
                    if (it_c->oid < 0)
                        continue;

                    if (verbose > 4)
                    	cout << endl << TABTABTAB << "For c = " << s_q->id_q_to_str(it_c->oid) << endl;

                    ten_c = PtwID(it_c->oid, mesh_q);

                    // get the ratio set
                    // auto ratio_set = get_ratio_set_vol(q.pt, ten_a.pt, ten_b.pt, ten_c.pt);

                    // if (ten_a.id == 135212 && ten_b.id == 132053 && ten_c.id == 134604) { // for test only

                    // Entry* q_entry = new Entry(q, ten_a, ten_b, ten_c, ratio_set.volume, ratio_set.ratio, ten_h);
                    Entry* q_entry = new Entry(q, ten_a, ten_b, ten_c, ten_h);
					q_entry->fill_sides();
					q_entry->sort_sides();

                    if (insert_entry(q_entry, v_ret, hash_ret)) {
                        if (verbose > 4)
                        	cout << TABTABTAB << "Include query entry: " << q_entry->to_str(10) << endl;

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

////////////////////////// Toggle_1 /////////////////////////////////////
int retrieve_congr_entry(vector<Entry*>& e_list, double epsilon, IndexTree* tree, const Struct_DB* s_db,
    vector<Entry_Pair*>& ret, int& total_page_accessed, bool verbose = false) {
////////////////////////// Toggle_1 /////////////////////////////////////

////////////////////////// Toggle_2 /////////////////////////////////////
// int retrieve_congr_entry(vector<Entry*>& e_list, double epsilon, node_type* tree, rtree_info* r_info, const Struct_DB* s_db,
//     vector<Entry_Pair*>& ret, int& total_page_accessed, bool verbose = false) {
////////////////////////// Toggle_2 /////////////////////////////////////

    double corr_epsilon = max(0.001, 2 * epsilon);

    int total_nhits = 0;

    for (auto &e: e_list) {

    	if (verbose)
    		cout << "For entry:" << endl << e->to_str(10) << endl;

        const int HIGHEST = 6;
        int *page_accessed = new int[HIGHEST];
        for (int i = 0; i < HIGHEST; i++) {
            page_accessed[i] = 0;
        }

        ////////////////////////// Toggle_1 /////////////////////////////////////
        vector<int> rr_ret;
        total_nhits += window_query(tree, e->sides, corr_epsilon, rr_ret, page_accessed);
        ////////////////////////// Toggle_1 /////////////////////////////////////

        ////////////////////////// Toggle_2 /////////////////////////////////////
        // vector<RangeReturn_type*> rr_ret;
    	// total_nhits += window_query(tree, r_info, e->sides, corr_epsilon, rr_ret, page_accessed);
        ////////////////////////// Toggle_2 /////////////////////////////////////

        // cout << "Page accessed:";
        // for (int i = 0; i < HIGHEST; i++) {
        //     cout << "\t" << page_accessed[i];
        //     total_page_accessed += page_accessed[i];
        // }
        // cout << endl;

        delete[] page_accessed;

	    for (auto &hit: rr_ret) {
            ////////////////////////// Toggle_1 /////////////////////////////////////
            int hit_key = hit;
            ////////////////////////// Toggle_1 /////////////////////////////////////

            ////////////////////////// Toggle_2 /////////////////////////////////////
	        // int hit_key = hit->oid;
            ////////////////////////// Toggle_2 /////////////////////////////////////
	        Entry* f = s_db->get_entry(hit_key);
	        Entry_Pair* new_pair = new Entry_Pair(e, f, s_db->get_grid_id_by_global_cell_id(hit_key));
	        ret.push_back(new_pair);

	        if (verbose)
	            cout << "Found pair:" << endl << new_pair->to_str(10) << endl;
	    }
    }

    return total_nhits;
}


////////////////////////// Toggle_1 /////////////////////////////////////
int retrieve_congr_entry_bundle(vector<Entry*>& e_list, double epsilon, IndexTree* tree, const Struct_DB* s_db,
    vector<Entry_Pair*>& ret, bool verbose = false) {
////////////////////////// Toggle_1 /////////////////////////////////////

////////////////////////// Toggle_2 /////////////////////////////////////
// int retrieve_congr_entry_bundle(vector<Entry*>& e_list, double epsilon, node_type* tree, rtree_info* r_info, const Struct_DB* s_db,
//     vector<Entry_Pair*>& ret, int& total_page_accessed, bool verbose = false) {
////////////////////////// Toggle_2 /////////////////////////////////////

    double corr_epsilon = max(0.001, 2 * epsilon);

    // RTree<int, int, INDEX_DIM, double, 6> aux_tree;
    AuxTree aux_tree;
    Entry* e;
    int box_min[INDEX_DIM], box_max[INDEX_DIM];
    for (int i = 0; i < e_list.size(); i++) {
        e = e_list[i];
        for (int j = 0; j < INDEX_DIM; j++) {
            box_min[j] = (int) ((e->sides[j] - corr_epsilon) * RSTREE_SCALE);
            box_max[j] = (int) ((e->sides[j] + corr_epsilon) * RSTREE_SCALE);
        }
        aux_tree.Insert(box_min, box_max, i);
    }

    auto aux_root = aux_tree.GetRoot();
    cout << "Highest of aux_root: " << aux_root->m_level << "(" << e_list.size() << ")" << endl;
    if (aux_root->m_level != 4) {
        return -1;
    }

    // auto root = tree->GetRoot();
    // // cout << "Highest of root: " << root->m_level << endl;

    // vector<pair<int, int>> search_ret;

    // int ovlp_min[INDEX_DIM], ovlp_max[INDEX_DIM];
    // for (int i = 0; i < INDEX_DIM; i++) {
    //    ovlp_min[i] = numeric_limits<int>::min();
    //    ovlp_max[i] = numeric_limits<int>::max();
    // }

    // node_search_bundle(root, aux_root, ovlp_min, ovlp_max, search_ret);

    // for (auto &r: search_ret) {
    //     Entry* e = e_list[r.first];
    //     Entry* f = s_db->get_entry(r.second);
    //     Entry_Pair* new_pair = new Entry_Pair(e, f, s_db->get_grid_id_by_global_cell_id(r.second));
    //     ret.push_back(new_pair);

    //     if (verbose)
    //         cout << "Found pair:" << endl << new_pair->to_str(10) << endl;
    // }

    // return search_ret.size();

}

bool test_verification_cell(const Cell* c, const Struct_Q* s_q, const Struct_DB* s_db, bool verbose = false) {
    int matching_repre_point_count = 0, matching_all_point_count = 0;
    const int db_mesh_id = s_q->get_db_mesh_id();

    // loop through all points inside the cell
    for (auto &p: c->list) {
        // get point ID mapped in DB
        const int mapping = s_q->get_id_mapping(p.id);
        if (mapping >= 0) {
            if (s_db->look_up_repre_index(mapping, db_mesh_id)) { // if mapping is a repre point in DB
                if (verbose)
                	cout << "Query pt #" << p.id << " matched with a repre. pt #" << mapping << endl;
                
                matching_repre_point_count++;

                // check whether the remaining and help points appear in query
                bool all_appearing = true;
                for (int i = 0; i < 3; i++) {
                    const int remai_id = s_db->get_remai_id(mapping, i, db_mesh_id);
                    if (!s_q->look_up_id_db(remai_id)) {
                        if (verbose)
                        	cout << "But remai[" << i << "] (db#" << remai_id << ") not appearing in the query" << endl;
                        
                        all_appearing = false;
                    }
                }
                int help_id = s_db->get_help_id(mapping, db_mesh_id);
                if (!s_q->look_up_id_db(help_id)) {
                    if (verbose)
                    	cout << "But help pt (db#" << help_id << ") not appearing in the query" << endl;
                    all_appearing = false;
                }

                if (all_appearing) {
                    matching_all_point_count++;
                }
            }
        }
    }
    if (verbose)
    	cout << "Matching repre: " << matching_repre_point_count << endl;
    if (verbose)
    	cout << "Matching all: " << matching_all_point_count << endl;

    if (matching_all_point_count > 0) { // if at least 1 all_matching point found
        return true;
    } else {
        return false;
    }
}

void test_verification(const Struct_Q* s_q, const Struct_DB* s_db, bool verbose = false) {
    cout << "Test verification..." << endl;

    int counter = 0, matched_cells_count = 0;

    // loop through all query cells
    for (auto it = s_q->g_q->cells_map.begin(); it != s_q->g_q->cells_map.end(); it++) {
        if (verbose)
        	cout << "For query cell #" << counter << endl;

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
        cerr << "Usage: " << argv[0] << " database_path grid_filename query_filename delta [-test] [-verbose=...] [-force_cell=...] [-force_pt=...]*" << endl;
        exit(1);
    }

    int verbose = 0, force_cell = -1;
    unordered_set<int> force_pts;
    bool test_mode = false, batch_mode = false;
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
        if (argv_str == "-test")
            test_mode = true;
        else if (argv_str.rfind("-verbose", 0) == 0)
        	verbose = atoi(argv[i] + 9);
        else if (argv_str.rfind("-force_cell", 0) == 0)
        	force_cell = atoi(argv[i] + 12);
        else if (argv_str.rfind("-force_pt", 0) == 0)
            force_pts.insert(atoi(argv[i] + 10));
    }

    int argi = 0;
    string db_path = argv[(++argi)];
    string grid_filename = argv[(++argi)];
    ////////////////////////// Toggle_1 /////////////////////////////////////
    string idx_filename = grid_filename + ".idx.t";
    ////////////////////////// Toggle_1 /////////////////////////////////////

    ////////////////////////// Toggle_2 /////////////////////////////////////
    // string idx_filename = grid_filename + ".idx.4";
    ////////////////////////// Toggle_2 /////////////////////////////////////
    string query_filename = argv[(++argi)];

    double delta = atof(argv[(++argi)]);

    srand(time(NULL));

    timer_start();

    cout << "Reading database files from " << db_path << endl;
    vector<TriMesh*> db_meshes;
    int num_meshes = read_db_mesh_batch(db_path, db_meshes);
    cout << "Total no. meshes: " << num_meshes << endl << endl;

    vector<KDtree*> db_kds;
    for (auto &t: db_meshes)
        db_kds.push_back(new KDtree(t->vertices));

    // // no need to read db rstree?
    // rtree_info db_rtree_info = { 5, 10, 3, 7 };
    // node_type *root_p;
    // cout << "Loading R-tree for DB points..." << endl;
    // read_rtree(&root_p, db_rstree_filename.c_str(), &db_rtree_info);

    // load the DB structure
    cout << "Loading DB structure from " << grid_filename << endl;
    Struct_DB s_db;
    s_db.read(grid_filename, db_meshes);
    cout << "Total no. cells: " << s_db.get_total_cells_count() << endl << endl;

    // load the index entries tree
    cout << "Loading index entries from " << idx_filename << endl;

    ////////////////////////// Toggle_1 /////////////////////////////////////
    IndexTree tree;
    tree.Load(idx_filename.c_str());
    ////////////////////////// Toggle_1 /////////////////////////////////////

    ////////////////////////// Toggle_2 /////////////////////////////////////
    // rtree_info idx_rtree_info = read_rstree_info("../common/config/rstree.idx.config");
    // cout << "R-tree config: m=" << idx_rtree_info.m << ", M=" << idx_rtree_info.M << ", dim=" << idx_rtree_info.dim << ", reinsert=" << idx_rtree_info.reinsert_p << endl << endl;
    // node_type* tree;
    // read_rtree(&tree, idx_filename.c_str(), &idx_rtree_info);
    ////////////////////////// Toggle_2 /////////////////////////////////////

    cout << "Reading query mesh from " << query_filename << endl;
    TriMesh *mesh_q = TriMesh::read(query_filename);

    rtree_info query_rtree_info = read_rstree_info("../common/config/rstree.pcd.config");
    node_type *root_q;

    // load the query R-tree
    cout << "Loading R-tree for query points..." << endl;
    read_rtree(&root_q, string(query_filename + ".rst.0").c_str(), &query_rtree_info);

    // load the query structure
    Struct_Q s_q;
    s_q.read(query_filename + ".info");

    cout << "Total I/O time: " << timer_end(SECOND) << "(s)" << endl;

    cout << endl;

    const int exec_times = 20;
    double exec_prop_time = 0, exec_veri_time = 0;
    int exec_veri_num = 0;

    for (int exec_i = 0; exec_i < exec_times; exec_i++) {

	    timer_start();

	    int total_num_verification = 0;
	    double total_time = 0.0, proposal_time = 0.0, verification_time = 0.0;
        double time_p1 = 0.0, time_p2 = 0.0;

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

	    // // test verification
	    // if (test_mode) {
	    //     test_verification(&s_q, &s_db, false);
	    // }

	    cout << endl;

	    const int k_m = 10;
	    int verified_size = 0;
	    for (int t = 0; t < k_m; t++) {
	        int selected_cell_id;
	        Cell* selected_cell;
	        if (test_mode)
	        	selected_cell = get_random_q_cell(s_q.g_q, selected_cell_id, force_cell);
	        else
	        	selected_cell = get_random_q_cell(s_q.g_q, selected_cell_id);

	        cout << "Selecting cell #" << selected_cell_id
	             << " with " << selected_cell->list.size() << " pts" << endl;

	        // if (test_mode) {
	        //     if(!test_verification_cell(selected_cell, &s_q, &s_db, true)) {
	        //         continue;
	        //     }
	        // }

	        vector<Entry_Pair*> v_pairs;

	        for (auto &q: selected_cell->list) {
                // under test mode AND force pts list not empty AND q is not a force pt -> continue
                // that is, only execute force pts
                if (test_mode && (!force_pts.empty()) && (force_pts.find(q.id) == force_pts.end()))
                    continue;
	            
	            if (verbose > 0)
	            	cout << endl << "Calculating entry list for query pt #" << q.id << endl;

	            timer_start();

	            vector<Entry*> v_entries;
	            int num_entries = cal_entries_new(q, s_db.get_ann_min(), &s_q, mesh_q, root_q, &query_rtree_info, v_entries, verbose);

                double time_incre_p1 = timer_end(SECOND);
                time_p1 += time_incre_p1;

	            if (verbose > 0)
	            	cout << endl << "Size of the entry list for query pt #" << q.id << ": " << num_entries << endl;
                if (verbose > 4) {
                    cout << endl;
                    for (auto &e: v_entries)
                        cout << e->to_str(10) << endl;
                }

	            timer_start();

                int total_page_accessed = 0;

                ////////////////////////// Toggle_1 /////////////////////////////////////
                // int num_hits = retrieve_congr_entry(v_entries, s_q.epsilon, &tree, &s_db, v_pairs, total_page_accessed, (verbose > 4));
                int num_hits;
                // if (num_entries > 600) {
                    num_hits = retrieve_congr_entry_bundle(v_entries, s_q.epsilon, &tree, &s_db, v_pairs, (verbose > 4));
                // } else {
                    // num_hits = retrieve_congr_entry(v_entries, s_q.epsilon, &tree, &s_db, v_pairs, total_page_accessed, (verbose > 4));
                // }
                ////////////////////////// Toggle_1 /////////////////////////////////////

                ////////////////////////// Toggle_2 /////////////////////////////////////
	            // int num_hits = retrieve_congr_entry(v_entries, s_q.epsilon, tree, &idx_rtree_info, &s_db, v_pairs, total_page_accessed, (verbose > 4));
             //    // int num_hits = retrieve_congr_entry_bundle(v_entries, s_q.epsilon, tree, &idx_rtree_info, &s_db, v_pairs, total_page_accessed, (verbose > 4));
                ////////////////////////// Toggle_2 /////////////////////////////////////

                double time_incre_p2 = timer_end(SECOND);
                time_p2 += time_incre_p2;

	            if (verbose > 0)
	            	cout << endl << "Size of the hit list for query pt #" << q.id << ": " << num_hits << endl;

                // cout << "Total page accessed: " << total_page_accessed << endl;
	        }

	        cout << endl << "Total hit size is " << v_pairs.size() << endl << endl;
	        if (verbose > 4) {
	        	for (auto &p: v_pairs) {
	        		cout << p->to_str(10) << endl;
	        	}
	        }

	        total_num_verification += v_pairs.size();

	        timer_start();
	        verified_size = 0;
	        for (auto &h: v_pairs) {
	            // cout << h->to_str() << endl;
	            h->cal_xf();

	            double result = cal_corr_err(mesh_q, db_kds[h->id_db], &h->xf, delta);
	            if (result > 0) {
	                // cout << "Accept:" << endl << h->to_str(10) << endl << result << endl;
	                verified_size++;
	            }
	        }
	        verification_time += timer_end(SECOND);

	        if (verified_size > 0)
	            break;

	        if (test_mode)
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

        cout << "cal entries in " << time_p1 << "(s)" << endl;
        cout << "retrieve congr in " << time_p2 << "(s)" << endl << endl;

	    exec_prop_time += proposal_time;
	    exec_veri_time += verification_time;
	    exec_veri_num += total_num_verification;

	    if (test_mode)
	    	break;
	}

    if (!test_mode) {
    	cout << endl << "Average proposal time: " << (exec_prop_time / exec_times) << endl;
    	cout << endl << "Average verification time: " << (exec_veri_time / exec_times) << endl;
    	cout << endl << "Average number of candidate transformations: " << ((double) exec_veri_num / exec_times) << endl << endl;
    }

}