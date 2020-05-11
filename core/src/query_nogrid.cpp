#include "TriMesh.h"
#include "KDtree.h"
#include "point.h"
#include "tetra_meas.h"
#include "pcr_adv.h"
#include "RTree.h"
#include "util.h"
#include "c_rtree.h"
#include "struct_q.h"
#include "goicp.h"

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

class Query_Context {
public:
    int force_cell;
    unordered_set<int> force_pts;
    bool write_stat;
    bool stop_once;
    string stat_filename;

    string db_path;
    string grid_filename;
    string query_filename;
    double delta; // TODO: change to MSE
    double epsilon;

    void read_param(int argc, char** argv) {
        force_cell = -1;
        force_pts.clear();
        write_stat = false;
        stop_once = false;
        string stat_filename = "";
        for (int i = 0; i < argc; i++) {
            string argv_str(argv[i]);
            if (argv_str.rfind("-force_cell", 0) == 0)
                force_cell = atoi(argv[i] + 12);
            else if (argv_str.rfind("-force_pt", 0) == 0)
                force_pts.insert(atoi(argv[i] + 10));
            else if (argv_str == "-stop_once")
                stop_once = true;
            else if (argv_str.rfind("-stat", 0) == 0) {
                write_stat = true;
                stat_filename = string(argv[i] + 6);
            }
        }

        int argi = 0;

        db_path = argv[(++argi)];
        grid_filename = argv[(++argi)];
        query_filename = argv[(++argi)];

        delta = atof(argv[(++argi)]);

#ifdef PROB
        epsilon = atof(argv[(++argi)]);
#else
        epsilon = delta;
#endif
    }

    DB_Meshes db_meshes;
    Struct_DB s_db;
    double ann_min;
    IndexTree tree;
    Mesh mesh_q;
    C_RTree r_q;
    Struct_Q s_q;

    GoICP** goicp;

    bool load() {

        cout << "Reading database files from " << db_path << endl;
        int num_meshes = db_meshes.read_from_path(db_path);
        cout << "Total no. meshes: " << num_meshes << endl << endl;

        db_meshes.build_kd();

        // load the DB structure
        cout << "Loading DB structure from " << grid_filename << endl;
        if (!s_db.read(grid_filename, &db_meshes)) {
            cout << "Error loading DB structure" << endl;
            return false;
        }
        cout << "Total no. cells: " << s_db.get_total_cells_count() << endl;

        ann_min = s_db.get_ann_min();

        string idx_filename = get_idx_filename(grid_filename);

        // load the index entries tree
        cout << "Loading index entries from " << idx_filename << endl;
        tree.Load(idx_filename.c_str());

        cout << endl;

        cout << "Reading query mesh from " << query_filename << endl;
        mesh_q.read_from_path(query_filename);

        // load the query R-tree
        r_q.read_from_mesh(query_filename);

        // load the query structure
        if (!s_q.read(query_filename + ".info")) {
            cout << "Error loading query structure" << endl;
            return false;
        }

        goicp = new GoICP*[num_meshes];
        for (int i = 0; i < num_meshes; i++) {
            goicp[i] = new GoICP;
            loadGoICP(db_meshes.get_mesh(i), &mesh_q, delta, *goicp[i]);

            // // Build Distance Transform
            // cout << endl << "Building Distance Transform of #" << i << "..." << endl;
            // goicp[i]->BuildDT();

            goicp[i]->Initialize();
        }
        cout << endl;

        return true;
    }
};

void get_potential_set(Pt3D* q, double rad, Query_Context* qc, vector<int>& ret, int excl_id_list[] = {}, int excl_id_num = 0) {

    qc->r_q.nn_sphere_range(q, sq(rad + 2 * qc->epsilon), 4 * qc->epsilon, ret, excl_id_list, excl_id_num);

    unordered_set<int> new_excl_id_set;
    for (int i = 0; i < excl_id_num; i++) {
        new_excl_id_set.insert(excl_id_list[i]);
    }
    for (auto &v: ret) {
        new_excl_id_set.insert(v);
    }

    qc->r_q.range_sphere_min_max(q, rad - 2 * qc->epsilon, qc->ann_min + 6 * qc->epsilon, ret, new_excl_id_set);
}

void get_potential_set_and_show(Pt3D* q, double rad, Query_Context* qc, vector<int>& ret, string nav = "", int excl_id_list[] = {}, int excl_id_num = 0) {

    qc->r_q.nn_sphere_range(q, sq(rad + 2 * qc->epsilon), 4 * qc->epsilon, ret, excl_id_list, excl_id_num);

    unordered_set<int> new_excl_id_set;
    for (int i = 0; i < excl_id_num; i++) {
        new_excl_id_set.insert(excl_id_list[i]);
    }
    for (auto &v: ret) {
        new_excl_id_set.insert(v);
    }

    qc->r_q.range_sphere_min_max(q, rad - 2 * qc->epsilon, qc->ann_min + 6 * qc->epsilon, ret, new_excl_id_set);

    cout << nav << "Potential set:";
    for (auto &it: ret) {
        // cout << " " << s_q->id_q_to_str(it.oid) << "=" << (sqrt(it.dist) / RSTREE_SCALE);
        cout << " " << qc->s_q.id_q_to_str(it);
    }
    cout << endl;
}

void nn_sphere_range(Pt3D* q, double sq_dist, double err, Query_Context* qc, vector<int>& ret, int excl_id_list[] = {}, int excl_id_num = 0) {

    qc->r_q.nn_sphere_range(q, sq_dist, err, ret, excl_id_list, excl_id_num);

}

void nn_sphere_range_and_show(Pt3D* q, double sq_dist, double err, Query_Context* qc, vector<int>& ret, string nav = "", int excl_id_list[] = {}, int excl_id_num = 0) {

    qc->r_q.nn_sphere_range(q, sq_dist, err, ret, excl_id_list, excl_id_num);
    
    cout << nav << "nn_sphere_range-------RR:";
    for (auto &it: ret) {
        // cout << " " << s_q->id_q_to_str(it.oid) << "=" << (sqrt(it.dist) / RSTREE_SCALE);
        cout << " " << qc->s_q.id_q_to_str(it);
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

// test function TODO
double brute_force_nn(vector<int>& list, int cheat, Query_Context* qc, PtwID& ret) {

	PtwID cheat_pt = PtwID(cheat, &qc->mesh_q);

	double nn_dist = numeric_limits<double>::max();
	for (int i = 0; i < list.size(); i++) {
		if (i != cheat) {
			PtwID pt = PtwID(list[i], &qc->mesh_q);
			double dist = eucl_dist(&pt.pt, &cheat_pt.pt);
			if (dist < nn_dist) {
				nn_dist = dist;
				ret = pt;
			}
		}
	}

	return nn_dist;
}

#ifdef TEST_MODE
int cal_entries(PtwID q, Query_Context* qc, vector<Entry*>& v_ret, vector<int>& cheat_list_id, vector<double>& cheat_list_dist)
#else
int cal_entries(PtwID q, Query_Context* qc, vector<Entry*>& v_ret)
#endif
{

    v_ret.clear();
    
    unordered_set<long long> hash_ret;

    cout.precision(10);

    int ret = 0;
    double err = qc->epsilon * 2;

    vector<int> range_a, range_h, range_b, range_c;
    PtwID ten_a, ten_h, ten_b, ten_c;

#ifdef TEST_MODE
    PtwID nn_a, nn_b, nn_c;
#endif

#if VERBOSE > 0
    #ifdef ACC
    nn_sphere_range_and_show(&(q.pt), sq(qc->ann_min - err), err, qc, range_a);
    #else
    get_potential_set_and_show(&(q.pt), qc->ann_min, qc, range_a);
    #endif
#else
    #ifdef ACC
    nn_sphere_range(&(q.pt), sq(qc->ann_min - err), err, qc, range_a);
    #else
    get_potential_set(&(q.pt), qc->ann_min, qc, range_a);
    #endif
#endif

#ifdef TEST_MODE
    if (cheat_list_id[1] >= 0) {
    	double nn_a_dist = brute_force_nn(range_a, cheat_list_id[1], qc, nn_a);
    	cout << "The NN of ground-truth a = #" << cheat_list_id[1] << " is recognized: #" << nn_a.id << ", dist = " << nn_a_dist << endl;
    }
#endif

    for (auto &it_a: range_a) {

        if (it_a < 0)
            continue;

#ifdef TEST_MODE
        if (it_a != cheat_list_id[1])
        	continue;
#endif

#if VERBOSE > 1
        cout << endl << "For a = " << qc->s_q.id_q_to_str(it_a) << endl;
#endif

        // ten_a = PtwID(it_a.oid, mesh_q);
        ten_a = PtwID(it_a, &qc->mesh_q);

        auto m = middle_pt(&q.pt, &ten_a.pt);
        double d_pm = eucl_dist(&q.pt, &m);

        int excl_qa[2] = { q.id, ten_a.id };

#if VERBOSE > 1
    #ifdef ACC
        nn_sphere_range_and_show(&m, sq(d_pm - err), err, qc, range_h, "", excl_qa, 2);
    #else
        get_potential_set_and_show(&m, d_pm, qc, range_h, "", excl_qa, 2);
    #endif
#else
    #ifdef ACC
        nn_sphere_range(&m, sq(d_pm - err), err, qc, range_h, excl_qa, 2);
    #else
        get_potential_set(&m, d_pm, qc, range_h, excl_qa, 2);
    #endif
#endif

        for (auto &it_h: range_h) {

            if (it_h < 0)
                continue;

#ifdef TEST_MODE
	        if (it_h != cheat_list_id[4])
	        	continue;
#endif

#if VERBOSE > 2
            cout << endl << TAB << "For h = " << qc->s_q.id_q_to_str(it_h) << endl;
#endif

            ten_h = PtwID(it_h, &qc->mesh_q);

            Pt3D ten_b_est, ten_c_est;
            auto got_b_c = get_est_b_c(&m, &(ten_a.pt), &(ten_h.pt), ten_b_est, ten_c_est);
            if (!got_b_c) {
                continue;
            }

#if VERBOSE > 2
            nn_sphere_range_and_show(&ten_b_est, 0.0, err, qc, range_b, TAB, excl_qa, 2);
#else
            nn_sphere_range(&ten_b_est, 0.0, err, qc, range_b, excl_qa, 2);
#endif

#ifdef TEST_MODE
		    if (cheat_list_id[2] >= 0) {
		    	double nn_b_dist = brute_force_nn(range_b, cheat_list_id[2], qc, nn_b);
		    	cout << "The NN of ground-truth b = #" << cheat_list_id[2] << " is recognized: #" << nn_b.id << ", dist = " << nn_b_dist << endl;
		    }
#endif

            for (auto &it_b: range_b) {

                if (it_b < 0)
                    continue;

#ifdef TEST_MODE
		        if (it_b != cheat_list_id[2])
		        	continue;
#endif

#if VERBOSE > 3
                cout << endl << TABTAB << "For b = " << qc->s_q.id_q_to_str(it_b) << endl;
#endif

                ten_b = PtwID(it_b, &qc->mesh_q);

                int excl_qab[3] = { q.id, ten_a.id, ten_b.id };

#if VERBOSE > 3
                nn_sphere_range_and_show(&ten_c_est, 0.0, err, qc, range_c, TABTAB, excl_qab, 3);
#else
                nn_sphere_range(&ten_c_est, 0.0, err, qc, range_c, excl_qab, 3);
#endif

#ifdef TEST_MODE
			    if (cheat_list_id[3] >= 0) {
			    	double nn_c_dist = brute_force_nn(range_c, cheat_list_id[3], qc, nn_c);
			    	cout << "The NN of ground-truth c = #" << cheat_list_id[3] << " is recognized: #" << nn_c.id << ", dist = " << nn_c_dist << endl;
			    }
#endif

                for (auto &it_c: range_c) {

                    if (it_c < 0)
                        continue;

#ifdef TEST_MODE
			        if (it_c != cheat_list_id[3])
			        	continue;
#endif

#if VERBOSE > 4
                    cout << endl << TABTABTAB << "For c = " << qc->s_q.id_q_to_str(it_c) << endl;
#endif

                    ten_c = PtwID(it_c, &qc->mesh_q);

                    // currently the ratio set (including the volume and volume ratio) is not required
                    // since the indexing key are the side length
                    // auto ratio_set = get_ratio_set_vol(q.pt, ten_a.pt, ten_b.pt, ten_c.pt);
                    // Entry* q_entry = new Entry(q, ten_a, ten_b, ten_c, ratio_set.volume, ratio_set.ratio, ten_h);

                    // if (!(ten_a.id == 20477 && ten_b.id == 20324 && ten_c.id == 20064)) continue;
                    
                    Entry* q_entry = new Entry(q, ten_a, ten_b, ten_c, ten_h);

                    if (insert_entry(q_entry, v_ret, hash_ret)) {
#if VERBOSE > 4
                        cout << TABTABTAB << "Include query entry: " << q_entry->to_str(10) << endl;
#endif

						q_entry->fill_sides();
						// q_entry->sort_sides();

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

int retrieve_congr_entry(vector<Entry*>& e_list, Query_Context* qc, vector<Entry_Pair*>& ret) {

    double corr_err = max(0.001, 2 * qc->epsilon);

    int total_nhits = 0;

    int index = 0;

    for (auto &e: e_list) {

        vector<int> rr_ret;
        int rr_num = window_query(&qc->tree, e, corr_err, rr_ret);

#if VERBOSE > 4
        cout << "For entry #" << index << ", get " << rr_num << " window query results" << endl;
#endif

	    for (auto &hit: rr_ret) {
            int hit_key = hit;

	        Entry* f = qc->s_db.get_entry(hit_key);
            if (check_congr(e, f, corr_err)) {
                total_nhits++;
    	        Entry_Pair* new_pair = new Entry_Pair(e, f, qc->s_db.get_grid_id_by_global_cell_id(hit_key));
    	        ret.push_back(new_pair);

#if VERBOSE > 4
                cout << "Found pair:" << endl << new_pair->to_str(10) << endl;
#endif
            }
	    }

        index++;
    }

    return total_nhits;
}

bool test_verification_pt(PtwID* p, Query_Context* qc, vector<int>& ret_id, vector<double>& ret_dist, bool verbose = false) {
    const int db_mesh_id = qc->s_q.get_db_mesh_id();
    // get point ID mapped in DB
    const int mapping = qc->s_q.get_id_db_by_q(p->id);
    if (mapping >= 0) {
    	if (qc->s_db.look_up_repre_index(mapping, db_mesh_id)) { // if mapping is a repre point in DB
	    	ret_id.push_back(p->id);
	    	ret_dist.push_back(qc->s_q.get_drift_by_q(p->id));
	        if (verbose) {
	            cout << "Query pt #" << p->id << " matched with a repre. pt #" << mapping << " with drift = " << qc->s_q.get_drift_by_q(p->id) << endl;
	            cout << "Backtrace:" << endl;
	        }

	        for (int i = 0; i < 3; i++) {
	        	const int remai_id_db = qc->s_db.get_remai_id(mapping, i, db_mesh_id);
	        	const int remai_id_q = qc->s_q.get_id_q_by_db(remai_id_db);
	        	if (remai_id_q >= 0) {
	        		ret_id.push_back(remai_id_q);
	        		ret_dist.push_back(qc->s_q.get_drift_by_q(remai_id_q));
	        		if (verbose) {
	        			cout << "Query pt #" << remai_id_q << " matched with remai[" << i << "] (db#" << remai_id_db << ") with drift = " << qc->s_q.get_drift_by_q(remai_id_q) << endl;
	        		}
	        	} else {
	        		ret_id.push_back(-1);
	        		ret_dist.push_back(-1.0);
	        		if (verbose) {
	        			cout << "No query pt matched with remai[" << i << "] (db#" << remai_id_db << ")" << endl;
	        		}
	        	}
	        }

	        const int help_id_db = qc->s_db.get_help_id(mapping, db_mesh_id);
	        const int help_id_q = qc->s_q.get_id_q_by_db(help_id_db);
	        if (help_id_q >= 0) {
	        	ret_id.push_back(help_id_q);
	        	ret_dist.push_back(qc->s_q.get_drift_by_q(help_id_q));
	        	if (verbose) {
	        		cout << "Query pt #" << help_id_q << " matched with help (db#" << help_id_q << ") with drift = " << qc->s_q.get_drift_by_q(help_id_q) << endl;
	        	}
	        } else {
        		ret_id.push_back(-1);
        		ret_dist.push_back(-1.0);
        		if (verbose) {
        			cout << "No query pt matched with help (db#" << help_id_q << ")" << endl;
        		}
	        }

	        return true;
    	}
    }

    return false;
}

bool iter(Query_Context* qc, Exec_stat& stat) {

    timer_start();

    vector<Entry*> v_entries;
    vector<Entry_Pair*> v_pairs;

    double prop_time_iter = 0.0, veri_time_iter = 0.0;
    bool found_one_iter = false;
    double first_time_iter = 0.0;
    int first_veri_size_iter = 0;

    vector<Entry_Pair*> ret_left_icp_only, ret_left_goicp;

    int num_verified_iter = 0;

    int selected_pt_id = rand() % qc->mesh_q.size();
#ifdef TEST_MODE
    if (qc->force_cell >= 0) {
        selected_pt_id = qc->force_cell;
    }
#endif
    PtwID q = PtwID(selected_pt_id, &qc->mesh_q);

#ifdef TEST_MODE
    vector<int> test_ret_id;
    vector<double> test_ret_dist;
    if (!test_verification_pt(&q, qc, test_ret_id, test_ret_dist, true)) {
    	cout << "Test failed because the selected query pt cannot be mapped to a repre pt in DB!" << endl;
    	timer_end(SECOND);
    	return false;
    }
#endif

#if VERBOSE > 0
    cout << endl << "Calculating entry list for query pt #" << q.id << " with min=" << qc->ann_min << endl;
#endif

#ifdef TEST_MODE
    cal_entries(q, qc, v_entries, test_ret_id, test_ret_dist);
#else
    cal_entries(q, qc, v_entries);
#endif

#if VERBOSE > 0
    cout << endl << "Size of the entry list for query pt #" << q.id << ": " << v_entries.size() << endl;
#if VERBOSE > 4
    for (auto &e: v_entries)
        cout << e->to_str(10) << endl;
    cout << endl;
#endif
#endif

    retrieve_congr_entry(v_entries, qc, v_pairs);

#if VERBOSE > 0
    cout << endl << "Size of the hit list for query pt #" << q.id << ": " << v_pairs.size() << endl;
#if VERBOSE > 4
    for (auto &p: v_pairs)
        cout << p->to_str(10) << endl;
    cout << endl;
#endif
#endif

    prop_time_iter = timer_end(SECOND);

    timer_start();
    timer_start();

    // Step 1: calculate transformation, initial distance, and leave those for step 2&3
    for (auto &h: v_pairs) {
        h->cal_xf();
        h->init_dist = qc->db_meshes.cal_corr_err(&qc->mesh_q, h->id_db, &h->xf); // TODO: use another unimplemented interface
        cout << "Initial distance: " << h->init_dist << endl;

        if (h->init_dist <= sq(qc->delta)) {
            num_verified_iter++;
            if (num_verified_iter == 1 && !found_one_iter) {
                first_time_iter = timer_end(SECOND);
                found_one_iter = true;
            }
        } else {
            ret_left_icp_only.push_back(h);
        }
    }


#if VERBOSE > 0
    cout << "Left for ICP-only check: " << ret_left_icp_only.size() << endl;
#endif

    // // Step 2: perform the ICP-only check
    // for (auto &r: ret_left_icp_only) {
    //     double updated_err = goicp[r->id_db].ICP(&r->xf); // ICP-only
    //     cout << "Updated distance with ICP-only: " << updated_err << endl;

    //     if (updated_err <= sq(delta)) {
    //         num_verified_iter++;
    //         if (num_verified_iter == 1 && !found_one_iter) {
    //             first_time_iter = timer_end(SECOND);
    //             found_one_iter = true;
    //         }
    //     } else {
    //         ret_left_goicp.push_back(r);
    //     }
    // }

#if VERBOSE > 0
    cout << "Left for GoICP check: " << ret_left_goicp.size() << endl;
#endif

    // // Step 3: perform GoICP
    // for (auto &r: ret_left_goicp) {
    //  goicp[r->db_id].fly_init();
    //  goicp[r->db_id].OuterBnB(); // TODO: return false?

    //  // TODO
    // }

    if (!found_one_iter) {
        first_time_iter = timer_end(SECOND);
    }

    veri_time_iter += timer_end(SECOND);

    stat.prop_time += prop_time_iter;
    stat.veri_time += veri_time_iter;
    stat.user_time += (prop_time_iter + veri_time_iter);
    stat.first_verified_time += (prop_time_iter + first_time_iter);
    stat.num_verified = num_verified_iter;

    cout << "# verified: " << stat.num_verified << endl;

    stat.veri_size += v_pairs.size();
    stat.num_icp_only += ret_left_icp_only.size();
    stat.num_goicp += ret_left_goicp.size();

    if (num_verified_iter > 0)
        return true;
    else
        return false;
}

bool exec(Query_Context* qc, Exec_stat& stat) {

    bool find_accept = false;

#ifdef PROB
    const int k_m = 12;
#else
    const int k_m = 1;
#endif

    for (int t = 0; t < k_m; t++) {

        find_accept = iter(qc, stat);

        if (find_accept)
            break;
    }

    return find_accept;
}

int main(int argc, char **argv) {

#ifdef PROB
    if (argc < 6) {
        cerr << "Usage: " << argv[0] << " database_path grid_filename query_filename delta epsilon [-stop_once] [-force_cell=...] [-force_pt=...]* [-stat=...]" << endl;
        exit(1);
    }
#else
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " database_path grid_filename query_filename delta [-stop_once] [-force_cell=...] [-force_pt=...]* [-stat=...]" << endl;
        exit(1);
    }
#endif

    Query_Context qc;
    qc.read_param(argc, argv);

    cout << "delta: " << qc.delta << endl;
    cout << "epsilon: " << qc.epsilon << endl;
    cout << endl;

    srand(time(NULL));

    timer_start();

    if(!qc.load()) {
        exit(1);
    }

    auto exec_i_time = timer_end(SECOND);
    cout << "Total I/- time: " << exec_i_time << "(s)" << endl;

    cout << endl;


#ifdef TEST_MODE
    const int exec_times = 1;
#else
    const int exec_times = 100;
#endif

    double aggr_prop_time = 0, aggr_veri_time = 0, aggr_user_time = 0, aggr_first_verified_time = 0;
    int aggr_veri_size = 0;
    int aggr_num_fail = 0;

    for (int exec_i = 0; exec_i < exec_times; exec_i++) {

        cout << "Execution #" << exec_i << ": " << endl;
        Exec_stat stat = (const struct Exec_stat){ 0 };
        // bool exec_success = exec(&mesh_q, epsilon, delta, &s_q, &query_rtree, &db_meshes, &s_db, &tree, goicp, stat);
        bool exec_success = exec(&qc, stat);
        cout << endl;

#if VERBOSE >= 0
        cout << "Total number of candidate transformations: " << stat.veri_size << endl;
        cout << "Final number of valid transformations: " << stat.num_verified << endl;
        cout << "Total time: " << stat.user_time << "(s)" << endl;
        cout << "Proposal time: " << stat.prop_time << "(s)" << endl;
        cout << "Verification time: " << stat.veri_time << "(s)" << endl;
        cout << "First verified time: " << stat.first_verified_time << "(s)" << endl;
#endif

        aggr_prop_time += stat.prop_time;
        aggr_veri_time += stat.veri_time;
        aggr_user_time += stat.user_time;
        aggr_veri_size += stat.veri_size;
        aggr_first_verified_time += stat.first_verified_time;

        if (!exec_success)
            aggr_num_fail++;
	}

    double aver_prop_time = aggr_prop_time / (double) exec_times;
    double aver_veri_time = aggr_veri_time / (double) exec_times;
    double aver_user_time = aggr_user_time / (double) exec_times;
    double aver_veri_size = (double) aggr_veri_size / (double) exec_times;
    double aver_first_verified_time = aggr_first_verified_time / (double) exec_times;

    if (qc.write_stat) {
        ofstream stat_ofs;
        stat_ofs.open(qc.stat_filename, ofstream::out | ofstream::app);
        stat_ofs << aver_user_time << "\t";
        stat_ofs << aver_prop_time << "\t";
        stat_ofs << aver_veri_time << "\t";
        stat_ofs << aver_first_verified_time << "\t";
        stat_ofs << aver_veri_size << "\t";
        stat_ofs << aggr_num_fail << "\t";
        stat_ofs << exec_i_time << "\t";
        stat_ofs.close();
    }

    cout << endl;
    cout << "Average proposal time: " << aver_prop_time << endl;
    cout << "Average verification time: " << aver_veri_time << endl;
    cout << "Average user time: " << aver_user_time << endl;
    cout << "Average # candidate transformations: " << aver_veri_size << endl;
    cout << "Average find first time: " << aver_first_verified_time << endl;
    cout << "Failed exec: " << aggr_num_fail << endl;

    cout << endl;

}