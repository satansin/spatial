#include "TriMesh.h"
#include "KDtree.h"
#include "point.h"
#include "tetra_meas.h"
#include "pcr_adv.h"
#include "RTree.h"
#include "util.h"
#include "c_rtree.h"
#include "struct_q.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <unordered_map>
#include <unordered_set>

#ifndef VERBOSE
#define VERBOSE (-1)
#endif

using namespace std;
using namespace trimesh;

void get_potential_set(const Struct_Q* s_q, Pt3D* q, C_RTree* r_q, double min, double epsilon,
    vector<int>& ret, int excl_id_list[] = {}, int excl_id_num = 0) {

    r_q->nn_sphere_range(q, sq(min + 2 * epsilon), 4 * epsilon, ret, excl_id_list, excl_id_num);

    unordered_set<int> new_excl_id_set;
    for (int i = 0; i < excl_id_num; i++) {
        new_excl_id_set.insert(excl_id_list[i]);
    }
    for (auto &v: ret) {
        new_excl_id_set.insert(v);
    }

    r_q->range_sphere_min_max(q, min - 2 * epsilon, min + 6 * epsilon, ret, new_excl_id_set);
}

void get_potential_set_and_show(const Struct_Q* s_q, Pt3D* q, C_RTree* r_q, double min, double epsilon,
    vector<int>& ret, string nav = "", int excl_id_list[] = {}, int excl_id_num = 0) {

    r_q->nn_sphere_range(q, sq(min + 2 * epsilon), 4 * epsilon, ret, excl_id_list, excl_id_num);

    unordered_set<int> new_excl_id_set;
    for (int i = 0; i < excl_id_num; i++) {
        new_excl_id_set.insert(excl_id_list[i]);
    }
    for (auto &v: ret) {
        new_excl_id_set.insert(v);
    }

    r_q->range_sphere_min_max(q, min - 2 * epsilon, min + 6 * epsilon, ret, new_excl_id_set);

    cout << nav << "Potential set:";
    for (auto &it: ret) {
        // cout << " " << s_q->id_q_to_str(it.oid) << "=" << (sqrt(it.dist) / RSTREE_SCALE);
        cout << " " << s_q->id_q_to_str(it);
    }
    cout << endl;
}

void nn_sphere_range(const Struct_Q* s_q, Pt3D* q, C_RTree* r_q, double sq_dist, double err,
    vector<int>& ret, int excl_id_list[] = {}, int excl_id_num = 0) {

    r_q->nn_sphere_range(q, sq_dist, err, ret, excl_id_list, excl_id_num);
}

void nn_sphere_range_and_show(const Struct_Q* s_q, Pt3D* q, C_RTree* r_q, double sq_dist, double err,
    vector<int>& ret, string nav = "", int excl_id_list[] = {}, int excl_id_num = 0) {

    r_q->nn_sphere_range(q, sq_dist, err, ret, excl_id_list, excl_id_num);
    
    cout << nav << "nn_sphere_range-------RR:";
    for (auto &it: ret) {
        // cout << " " << s_q->id_q_to_str(it.oid) << "=" << (sqrt(it.dist) / RSTREE_SCALE);
        cout << " " << s_q->id_q_to_str(it);
    }
    cout << endl;
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
    long long new_hash = get_number_hash(new_entry); // number-hash is faster than string-hash
    if (hash_ret.insert(new_hash).second) {
        v_ret.push_back(new_entry);
        return true;
    } else {
        return false;
    }
    
}

int cal_entries(PtwID q, double min, const Struct_Q* s_q, const Mesh* mesh_q, C_RTree* r_q, vector<Entry*>& v_ret) {

    v_ret.clear();
    
    unordered_set<long long> hash_ret;

    cout.precision(10);

    int ret = 0;
    double err = s_q->epsilon * 2;

    vector<int> range_a, range_h, range_b, range_c;
    PtwID ten_a, ten_h, ten_b, ten_c;

#if VERBOSE > 0
    #ifdef ACC
    nn_sphere_range_and_show(s_q, &(q.pt), r_q, sq(min - err), err, range_a);
    #else
    get_potential_set_and_show(s_q, &(q.pt), r_q, min, s_q->epsilon, range_a);
    #endif
#else
    #ifdef ACC
    nn_sphere_range(s_q, &(q.pt), r_q, sq(min - err), err, range_a);
    #else
    get_potential_set(s_q, &(q.pt), r_q, min, s_q->epsilon, range_a);
    #endif
#endif

    for (auto &it_a: range_a) {

        if (it_a < 0)
            continue;

#if VERBOSE > 1
        cout << endl << "For a = " << s_q->id_q_to_str(it_a) << endl;
#endif

        // ten_a = PtwID(it_a.oid, mesh_q);
        ten_a = PtwID(it_a, mesh_q);

        auto m = middle_pt(&q.pt, &ten_a.pt);
        double d_pm = eucl_dist(&q.pt, &m);

        int excl_qa[2] = { q.id, ten_a.id };

#if VERBOSE > 1
    #ifdef ACC
        nn_sphere_range_and_show(s_q, &m, r_q, sq(d_pm - err), err, range_h, "", excl_qa, 2);
    #else
        get_potential_set_and_show(s_q, &m, r_q, d_pm, s_q->epsilon, range_h, "", excl_qa, 2);
    #endif
#else
    #ifdef ACC
        nn_sphere_range(s_q, &m, r_q, sq(d_pm - err), err, range_h, excl_qa, 2);
    #else
        get_potential_set(s_q, &m, r_q, d_pm, s_q->epsilon, range_h, excl_qa, 2);
    #endif
#endif

        for (auto &it_h: range_h) {

            if (it_h < 0)
                continue;

#if VERBOSE > 2
            cout << endl << TAB << "For h = " << s_q->id_q_to_str(it_h) << endl;
#endif

            ten_h = PtwID(it_h, mesh_q);

            Pt3D ten_b_est, ten_c_est;
            auto got_b_c = get_est_b_c(&m, &(ten_a.pt), &(ten_h.pt), ten_b_est, ten_c_est);
            if (!got_b_c) {
                continue;
            }

#if VERBOSE > 2
            nn_sphere_range_and_show(s_q, &ten_b_est, r_q, 0.0, err, range_b, TAB, excl_qa, 2);
#else
            nn_sphere_range(s_q, &ten_b_est, r_q, 0.0, err, range_b, excl_qa, 2);
#endif

            for (auto &it_b: range_b) {

                if (it_b < 0)
                    continue;

#if VERBOSE > 3
                cout << endl << TABTAB << "For b = " << s_q->id_q_to_str(it_b) << endl;
#endif

                ten_b = PtwID(it_b, mesh_q);

                int excl_qab[3] = { q.id, ten_a.id, ten_b.id };

#if VERBOSE > 3
                nn_sphere_range_and_show(s_q, &ten_c_est, r_q, 0.0, err, range_c, TABTAB, excl_qab, 3);
#else
                nn_sphere_range(s_q, &ten_c_est, r_q, 0.0, err, range_c, excl_qab, 3);
#endif

                for (auto &it_c: range_c) {

                    if (it_c < 0)
                        continue;

#if VERBOSE > 4
                    cout << endl << TABTABTAB << "For c = " << s_q->id_q_to_str(it_c) << endl;
#endif

                    ten_c = PtwID(it_c, mesh_q);

                    // currently the ratio set (including the volume and volume ratio) is not required
                    // since the indexing key are the side length
                    // auto ratio_set = get_ratio_set_vol(q.pt, ten_a.pt, ten_b.pt, ten_c.pt);
                    // Entry* q_entry = new Entry(q, ten_a, ten_b, ten_c, ratio_set.volume, ratio_set.ratio, ten_h);
                    
                    Entry* q_entry = new Entry(q, ten_a, ten_b, ten_c, ten_h);

                    if (insert_entry(q_entry, v_ret, hash_ret)) {
#if VERBOSE > 4
                        cout << TABTABTAB << "Include query entry: " << q_entry->to_str(10) << endl;
#endif

						q_entry->fill_sides();
						q_entry->sort_sides();

                        ret++;
                    }
                }
                range_c.clear();
            }
            range_b.clear();
        }
        range_h.clear();
    }
    range_a.clear();

    return ret;
}

bool check_congr(const Entry* e, const Entry* f, double err) {

#ifdef IDX_3
    if (abs(e->sides[1] - f->sides[1]) > err) {
        return false;
    }
    if (abs(e->sides[2] - f->sides[2]) > err) {
        return false;
    }
    if (abs(e->sides[5] - f->sides[5]) > err) {
        return false;
    }
#else
    for (int i = 0; i < INDEX_DIM; i++) {
        if (abs(e->sides[i] - f->sides[i]) > err) {
            return false;
        }
    }
#endif

    return true;
}

////////////////////////// Toggle_1 /////////////////////////////////////
int retrieve_congr_entry(vector<Entry*>& e_list, double epsilon, IndexTree* tree, const Struct_DB* s_db, vector<Entry_Pair*>& ret) {
////////////////////////// Toggle_1 /////////////////////////////////////

////////////////////////// Toggle_2 /////////////////////////////////////
// int retrieve_congr_entry(vector<Entry*>& e_list, double epsilon, node_type* tree, rtree_info* r_info, const Struct_DB* s_db,
//     vector<Entry_Pair*>& ret, int& total_page_accessed) {
////////////////////////// Toggle_2 /////////////////////////////////////

    double corr_err = max(0.001, 2 * epsilon);

    int total_nhits = 0;

    int index = 0;

    for (auto &e: e_list) {

        ////////////////////////// Toggle_1 /////////////////////////////////////
        vector<int> rr_ret;
        int rr_num = window_query(tree, e, corr_err, rr_ret);
        ////////////////////////// Toggle_1 /////////////////////////////////////

        ////////////////////////// Toggle_2 /////////////////////////////////////
        // vector<RangeReturn_type*> rr_ret;
    	// total_nhits += window_query(tree, r_info, e->sides, corr_epsilon, rr_ret, page_accessed);
        ////////////////////////// Toggle_2 /////////////////////////////////////

#if VERBOSE > 4
        cout << "For entry #" << index << ", get " << rr_num << " window query results" << endl;
#endif

	    for (auto &hit: rr_ret) {
            ////////////////////////// Toggle_1 /////////////////////////////////////
            int hit_key = hit;
            ////////////////////////// Toggle_1 /////////////////////////////////////

            ////////////////////////// Toggle_2 /////////////////////////////////////
	        // int hit_key = hit->oid;
            ////////////////////////// Toggle_2 /////////////////////////////////////

	        Entry* f = s_db->get_entry(hit_key);
            if (check_congr(e, f, corr_err)) {
                total_nhits++;
    	        Entry_Pair* new_pair = new Entry_Pair(e, f, s_db->get_grid_id_by_global_cell_id(hit_key));
    	        ret.push_back(new_pair);

#if VERBOSE > 0
                cout << "Found pair:" << endl << new_pair->to_str(10) << endl;
#endif
            }
	    }

        index++;
    }

    return total_nhits;
}


////////////////////////// Toggle_1 /////////////////////////////////////
int retrieve_congr_entry_bundle(vector<Entry*>& e_list, double epsilon, IndexTree* tree, const Struct_DB* s_db,
    vector<Entry_Pair*>& ret) {
////////////////////////// Toggle_1 /////////////////////////////////////

////////////////////////// Toggle_2 /////////////////////////////////////
// int retrieve_congr_entry_bundle(vector<Entry*>& e_list, double epsilon, node_type* tree, rtree_info* r_info, const Struct_DB* s_db,
//     vector<Entry_Pair*>& ret, int& total_page_accessed, bool verbose = false) {
////////////////////////// Toggle_2 /////////////////////////////////////

    double corr_epsilon = max(0.001, 2 * epsilon);

    AuxTree aux_tree;
    Entry* e;
    int box_min[INDEX_DIM], box_max[INDEX_DIM];
    for (int i = 0; i < e_list.size(); i++) {
        e_list[i]->get_index_box(corr_epsilon, box_min, box_max);
        aux_tree.Insert(box_min, box_max, i);
    }

    aux_tree.SortDim0();

    vector<pair<int, int>> search_ret;

    tree->SpatialJoin(&aux_tree, search_ret);

    for (auto &r: search_ret) {
        Entry* e = e_list[r.first];
        Entry* f = s_db->get_entry(r.second);
        if (check_congr(e, f, corr_epsilon)) {
            Entry_Pair* new_pair = new Entry_Pair(e, f, s_db->get_grid_id_by_global_cell_id(r.second));
            ret.push_back(new_pair);

#if VERBOSE > 0
            cout << "Found pair:" << endl << new_pair->to_str(10) << endl;
#endif
        }
    }

    return search_ret.size();

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

void test_verification(const Struct_Q* s_q, const Grid* g_q, const Struct_DB* s_db, bool verbose = false) {
    cout << "Test verification..." << endl;

    int counter = 0, matched_cells_count = 0;

    // loop through all query cells
    for (auto it = g_q->cells_map.begin(); it != g_q->cells_map.end(); it++) {
        if (verbose)
        	cout << "For query cell #" << counter << endl;

        if (test_verification_cell(it->second, s_q, s_db, verbose)) {
            matched_cells_count++;
        }
        counter++;
    }
    cout << "Summary: " << matched_cells_count << "/" << g_q->cells_map.size() << " cells are matched" << endl;

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
        cerr << "Usage: " << argv[0] << " database_path grid_filename query_filename delta [-force_cell=...] [-force_pt=...]* [-stat=...]" << endl;
        exit(1);
    }

    int verbose = -1, force_cell = -1;
    unordered_set<int> force_pts;
    bool write_stat = false;
    string stat_filename = "";
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
        if (argv_str.rfind("-verbose", 0) == 0)
        	verbose = atoi(argv[i] + 9);
        else if (argv_str.rfind("-force_cell", 0) == 0)
        	force_cell = atoi(argv[i] + 12);
        else if (argv_str.rfind("-force_pt", 0) == 0)
            force_pts.insert(atoi(argv[i] + 10));
        else if (argv_str.rfind("-stat", 0) == 0) {
            write_stat = true;
            stat_filename = string(argv[i] + 6);
        }
    }

    int argi = 0;
    string db_path = argv[(++argi)];
    string grid_filename = argv[(++argi)];
    string query_filename = argv[(++argi)];

    double delta = atof(argv[(++argi)]);

    srand(time(NULL));

    timer_start();

    cout << "Reading database files from " << db_path << endl;
    DB_Meshes db_meshes;
    int num_meshes = db_meshes.read_from_path(db_path);
    cout << "Total no. meshes: " << num_meshes << endl << endl;

    db_meshes.build_kd();

    // load the DB structure
    cout << "Loading DB structure from " << grid_filename << endl;
    Struct_DB s_db;
    s_db.read(grid_filename, &db_meshes);
    cout << "Total no. cells: " << s_db.get_total_cells_count() << endl;

    ////////////////////////// Toggle_1 /////////////////////////////////////
    string idx_filename = get_idx_filename(grid_filename);
    ////////////////////////// Toggle_1 /////////////////////////////////////

    ////////////////////////// Toggle_2 /////////////////////////////////////
    // string idx_filename = grid_filename + ".idx.4";
    ////////////////////////// Toggle_2 /////////////////////////////////////

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
    Mesh mesh_q;
    mesh_q.read_from_path(query_filename);

    // load the query R-tree
    C_RTree query_rtree;
    query_rtree.read_from_mesh(query_filename);

    // load the query structure
    Struct_Q s_q;
    s_q.read(query_filename + ".info");

    auto exec_i_time = timer_end(SECOND);
    cout << "Total I/- time: " << exec_i_time << "(s)" << endl;

    cout << endl;

    const int exec_times = 100;
    double exec_prop_time = 0, exec_veri_time = 0, exec_user_time = 0, exec_retr_time = 0;
    int exec_veri_num = 0;
    int exec_num_fail = 0;

    for (int exec_i = 0; exec_i < exec_times; exec_i++) {

	    timer_start();

	    int total_num_verification = 0;
	    double total_time = 0.0, proposal_time = 0.0, verification_time = 0.0;
	    double grid_time = 0.0;
        double time_p1 = 0.0, time_p2 = 0.0;

	    double w_q = 3.464102 * s_db.get_w();

#if VERBOSE >= 0
        cout << "Query voxelized of grid size " << w_q << endl;
#endif

	    // timer_start();
	    Grid g_q(w_q);
	    g_q.gridify(&mesh_q);
	    // cout << "Gridify completed in " << timer_end(SECOND) << "(s)" << endl;

#if VERBOSE >= 0
        cout << "Total number of query cells: " << g_q.cells_map.size() << endl;
        cout << endl;
#endif

	    // cout << "Displaying info of each query cell:" << endl;
	    // for (auto it = g_q.cells_map.begin(); it != g_q.cells_map.end(); it++) {
	    //     cout << it->second->x << " "
	    //          << it->second->y << " "
	    //          << it->second->z << " "
	    //          << it->second->list.size() << endl;
	    // }
	    // cout << endl;

// #ifdef TEST_MODE
//         test_verification(&s_q, &s_db, false);
// #endif

        bool find_accept = false;
	    const int k_m = 10;
	    int verified_size = 0;

	    for (int t = 0; t < k_m; t++) {

	        int selected_cell_id;
	        Cell* selected_cell;

#ifdef TEST_MODE
	        selected_cell = get_random_q_cell(&g_q, selected_cell_id, force_cell);
#else
	        selected_cell = get_random_q_cell(&g_q, selected_cell_id);
#endif

#if VERBOSE >= 0
            cout << "Selecting cell #" << selected_cell_id
                 << " with " << selected_cell->list.size() << " pts" << endl;
#endif

#ifdef TEST_MODE
            if(!test_verification_cell(selected_cell, &s_q, &s_db, true)) {
                continue;
            }
#endif

	        vector<Entry_Pair*> v_pairs;

	        for (auto &q: selected_cell->list) {

#ifdef TEST_MODE
                // under test mode AND force pts list not empty AND q is not a force pt -> continue
                // that is, only execute force pts
                if (!force_pts.empty() && (force_pts.find(q.id) == force_pts.end()))
                    continue;
#endif
	            
                double ann_min = s_db.get_ann_min();
#if VERBOSE > 0
	            cout << endl << "Calculating entry list for query pt #" << q.id << " with min=" << ann_min << endl;
#endif

	            timer_start();

	            vector<Entry*> v_entries;
	            int num_entries = cal_entries(q, ann_min, &s_q, &mesh_q, &query_rtree, v_entries);

                double time_incre_p1 = timer_end(SECOND);
                time_p1 += time_incre_p1;

#if VERBOSE > 0
	            cout << endl << "Size of the entry list for query pt #" << q.id << ": " << num_entries << endl;

    #if VERBOSE > 4
                for (auto &e: v_entries)
                    cout << e->to_str(10) << endl;
                cout << endl;
    #endif

#endif

	            timer_start();

                int total_page_accessed = 0;

                ////////////////////////// Toggle_1 /////////////////////////////////////
                // int num_hits = retrieve_congr_entry_bundle(v_entries, s_q.epsilon, &tree, &s_db, v_pairs);
                int num_hits = retrieve_congr_entry(v_entries, s_q.epsilon, &tree, &s_db, v_pairs);
                ////////////////////////// Toggle_1 /////////////////////////////////////

                ////////////////////////// Toggle_2 /////////////////////////////////////
	            // int num_hits = retrieve_congr_entry(v_entries, s_q.epsilon, tree, &idx_rtree_info, &s_db, v_pairs, total_page_accessed, (verbose > 4));
             //    // int num_hits = retrieve_congr_entry_bundle(v_entries, s_q.epsilon, tree, &idx_rtree_info, &s_db, v_pairs, total_page_accessed, (verbose > 4));
                ////////////////////////// Toggle_2 /////////////////////////////////////

                double time_incre_p2 = timer_end(SECOND);
                time_p2 += time_incre_p2;

#if VERBOSE > 0
	            cout << endl << "Size of the hit list for query pt #" << q.id << ": " << num_hits << endl;
#endif

                // cout << "Total page accessed: " << total_page_accessed << endl;
	        }

#if VERBOSE >= 0
            cout << endl << "Total hit size is " << v_pairs.size() << endl << endl;
	#if VERBOSE > 4
        	for (auto &p: v_pairs) {
        		cout << p->to_str(10) << endl;
        	}
	#endif
#endif

	        total_num_verification += v_pairs.size();

	        timer_start();
	        verified_size = 0;
	        for (auto &h: v_pairs) {
	            // cout << h->to_str() << endl;
	            h->cal_xf();

	            double result = db_meshes.cal_corr_err(&mesh_q, h->id_db, &h->xf, delta);
	            if (result > 0) {
	                // cout << "Accept:" << endl << h->to_str(10) << endl << result << endl;
	                verified_size++;
	            }
	        }
	        verification_time += timer_end(SECOND);

	        if (verified_size > 0) {
                find_accept = true;
	            break;
            }

#ifdef TEST_MODE
	        break;
#endif

	    }

	    total_time = timer_end(SECOND);
	    proposal_time = total_time - verification_time; // TODO: also minus retrieval time

#if VERBOSE >= 0
	    cout << endl;
	    cout << "Total number of candidate transformations: " << total_num_verification << endl;
	    cout << "Final number of valid transformations: " << verified_size << endl;
	    cout << "Total time: " << total_time << "(s)" << endl;
	    cout << "Proposal time: " << proposal_time << "(s)" << endl;
	    cout << "Verification time: " << verification_time << "(s)" << endl;

        cout << "cal entries in " << time_p1 << "(s)" << endl;
        cout << "retrieve congr in " << time_p2 << "(s)" << endl << endl;
#endif

	    exec_prop_time += proposal_time;
	    exec_veri_time += verification_time;
	    exec_veri_num += total_num_verification;
        exec_user_time += total_time;

        if (!find_accept) {
            exec_num_fail++;
        }

#ifdef TEST_MODE
	    break;
#endif
	}

    double aver_user_time = exec_user_time / exec_times;
    double aver_prop_time = exec_prop_time / exec_times;
    double aver_veri_time = exec_veri_time / exec_times;
    double aver_retr_time = exec_retr_time / exec_times;
    double aver_veri_num = (double) exec_veri_num / exec_times;

    if (write_stat) {
        ofstream stat_ofs;
        stat_ofs.open(stat_filename, ofstream::out | ofstream::app);
        stat_ofs /*<< left << setw(12) << setfill(' ')*/
            << aver_user_time << "\t";
        stat_ofs /*<< left << setw(12) << setfill(' ')*/
            << aver_prop_time << "\t";
        stat_ofs /*<< left << setw(12) << setfill(' ')*/
            << aver_veri_time << "\t";
        stat_ofs /*<< left << setw(12) << setfill(' ')*/
            << aver_retr_time << "\t";
        stat_ofs /*<< left << setw(12) << setfill(' ')*/
            << aver_veri_num << "\t";
        stat_ofs /*<< left << setw(12) << setfill(' ')*/
            << exec_num_fail << "\t";
        stat_ofs /*<< left << setw(12) << setfill(' ')*/
            << exec_i_time << "\t";
        stat_ofs.close();
    }

#ifndef TEST_MODE
    cout << endl;
    cout << "Average proposal time: " << aver_prop_time << endl;
    cout << "Average verification time: " << aver_veri_time << endl;
    cout << "Average retrieval time: " << aver_retr_time << endl;
    cout << "Average number of candidate transformations: " << aver_veri_num << endl;
    cout << "Failed exec: " << exec_num_fail << endl;
    cout << "Average user time: " << aver_user_time << endl;
#endif

    cout << endl;

}