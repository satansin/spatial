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

#ifdef TEST_MODE
    const int exec_times = 1;
#else
    const int exec_times = 50;
#endif

class Query_Context {
public:
    int force_cell;
    unordered_set<int> force_pts;
    bool write_stat;
    bool stop_once;
    bool small_set;
    string stat_filename;
    bool sort_entry;
    bool simple;

    string db_path;
    string grid_filename;
    string query_filename;
    double delta;
    bool spec_delta; // when PROB, delta might be specified or automatically assigned
    double epsilon;

    double cos_phi; // in query this threshold is slightly larger

    void read_param(int argc, char** argv) {
        force_cell = -1;
        force_pts.clear();
        stop_once = false;
        small_set = false;
        sort_entry = false;
        simple = false;
        write_stat = false;
        spec_delta = false;
        stat_filename = "";
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
            } else if (argv_str == "-sort_entry")
                sort_entry = true;
            else if (argv_str == "-simple")
            	simple = true;
            else if (argv_str == "-small")
                small_set = true;
            else if (argv_str.rfind("-delta", 0) == 0) {
                delta = atof(argv[i] + 7);
                spec_delta = true;
            }
        }

        int argi = 0;

        db_path = argv[(++argi)];
        grid_filename = argv[(++argi)];
        query_filename = argv[(++argi)];

        #ifdef PROB
            epsilon = atof(argv[(++argi)]);
        #else
            delta = atof(argv[(++argi)]);
            epsilon = delta;
        #endif
    }

    DB_Meshes db_meshes;
    Struct_DB s_db;
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

        string grid_bin_filename = get_bin_filename(grid_filename);

        // load the DB structure
        cout << "Loading DB structure from " << grid_bin_filename << endl;
        if (!s_db.read_bin(grid_bin_filename, &db_meshes)) {
            cout << "Error loading DB structure" << endl;
            return false;
        }
        cout << "Total no. cells: " << s_db.get_total_cells_count() << endl;

        cos_phi = cos(s_db.ang_min * PI / 180.0);

        string idx_filename = get_idx_filename(grid_filename);

        // load the index entries tree
        cout << "Loading index entries from " << idx_filename << endl;
        if (!tree.Load(idx_filename.c_str())) {
            cout << "Error loading index entries tree" << endl;
            return false;
        }

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

        #ifdef PROB
            if (!spec_delta) {
                double q_diam = mesh_q.get_bsphere_d();
                // delta = q_diam * 0.001;
                delta = sq(s_q.sigma);
                cout << "Diameter of query mesh: " << q_diam << ", thus delta is set to " << delta << endl;
            }
        #endif

        delta *= (double) mesh_q.size();
        cout << "Final SSE by number of query: " << delta << endl;

        // goicp = new GoICP*[num_meshes];
        // for (int i = 0; i < num_meshes; i++) {
        //     goicp[i] = new GoICP;
        //     loadGoICP(db_meshes.get_mesh(i), &mesh_q, delta, *goicp[i]);

        //     // Build Distance Transform
        //     cout << endl << "Building Distance Transform of #" << i << "..." << endl;
        //     goicp[i]->BuildDT();

        //     goicp[i]->Initialize();
        // }
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

    qc->r_q.range_sphere_min_max(q, rad - 2 * qc->epsilon, qc->s_db.ann_min + 6 * qc->epsilon, ret, new_excl_id_set);
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

    qc->r_q.range_sphere_min_max(q, rad - 2 * qc->epsilon, qc->s_db.ann_min + 6 * qc->epsilon, ret, new_excl_id_set);

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

// i < j
int get_th_index(int L, int i, int j) {
    int start_index = (2 * L - i - 1) * i / 2;
    int added_index = j - i - 1;
    return (start_index + added_index);
}

// #ifdef TEST_MODE
// int cal_entries_3nn(PtwID* q, Query_Context* qc, vector<Entry*>& v_ret, vector<int>& cheat_list_id, vector<double>& cheat_list_dist)
// #else
int cal_entries_3nn_nonsim(PtwID* q, Query_Context* qc, vector<Entry*>& v_ret)
// #endif
{

    v_ret.clear();

    cout.precision(10);

    int ret = 0;
    double err = qc->epsilon * 2;

    vector<int> range;
    #if VERBOSE > 0
        nn_sphere_range_and_show(q->pt, sq(qc->s_db.ann_min - err), err, qc, range);
    #else
        nn_sphere_range(q->pt, sq(qc->s_db.ann_min - err), err, qc, range);
    #endif

    int range_size = range.size();
    if (range.size() < 3) {
        return 0;
    }

    vector<PtwID> range_pts;
    vector<Pt3D> range_qa;
    for (int &id: range) {
        auto pt = PtwID(id, &qc->mesh_q);
        range_pts.push_back(pt);
        range_qa.push_back(*pt.pt - *q->pt);
    }
    vector<double> cos_th;
    for (int i = 0; i < range_size; i++) {
        for (int j = i + 1; j < range_size; j++) {
            cos_th.push_back(cos_theta(&range_qa[i], &range_qa[j]));
        }
    }

    // #ifdef TEST_MODE
    //     Entry* q_entry = new Entry(q, PtwID(cheat_list_id[1], &qc->mesh_q), PtwID(cheat_list_id[2], &qc->mesh_q), PtwID(cheat_list_id[3], &qc->mesh_q));
    //     q_entry->fill_sides();
    //     v_ret.push_back(q_entry);
    // #else
        // loop for all the 3-combinations
        for (int i = 0; i < range_size; i++) {
            for (int j = i + 1; j < range_size; j++) {
                for (int k = j + 1; k < range_size; k++) {

                    double cos_th_ij = cos_th[get_th_index(range_size, i, j)]; // the angle of i & j
                    double cos_th_ik = cos_th[get_th_index(range_size, i, k)]; // the angle of i & k
                    double cos_th_jk = cos_th[get_th_index(range_size, j, k)]; // the angle of j & k
                    if (cos_th_ij > qc->cos_phi || cos_th_ik > qc->cos_phi || cos_th_jk > qc->cos_phi) {
                        continue; // if th < phi, skip it
                    }

                    Entry* q_entry = new Entry(q, &range_pts[i], &range_pts[j], &range_pts[k]);
                    q_entry->fill_sides();

                    v_ret.push_back(q_entry);

                }
            }
        }
    // #endif
}

// #ifdef TEST_MODE
// int cal_entries_3nn_sim(PtwID* q, Query_Context* qc, vector<Entry*>& v_ret, vector<int>& cheat_list_id, vector<double>& cheat_list_dist)
// #else
int cal_entries_3nn_sim(PtwID* q, Query_Context* qc, vector<Entry*>& v_ret)
// #endif
{

    v_ret.clear();

    cout.precision(10);

    int ret = 0;
    double err = qc->epsilon * 2;

    vector<int> range;
    #if VERBOSE > 0
        nn_sphere_range_and_show(q->pt, sq(qc->s_db.ann_min - err), err, qc, range);
    #else
        nn_sphere_range(q->pt, sq(qc->s_db.ann_min - err), err, qc, range);
    #endif

    int range_size = range.size();
    if (range.size() < 3) {
        return 0;
    }

    vector<PtwID> range_pts;
    for (int &id: range) {
        auto pt = PtwID(id, &qc->mesh_q);
        range_pts.push_back(pt);
    }

    // #ifdef TEST_MODE
    //     Entry* q_entry = new Entry(q, PtwID(cheat_list_id[1], &qc->mesh_q), PtwID(cheat_list_id[2], &qc->mesh_q), PtwID(cheat_list_id[3], &qc->mesh_q));
    //     q_entry->fill_sides();
    //     v_ret.push_back(q_entry);
    // #else
        // loop for all the 3-combinations
        for (int i = 0; i < range_size; i++) {
            for (int j = i + 1; j < range_size; j++) {
                for (int k = j + 1; k < range_size; k++) {

                    Entry* q_entry = new Entry(q, &range_pts[i], &range_pts[j], &range_pts[k]);
                    q_entry->fill_sides();

                    v_ret.push_back(q_entry);

                }
            }
        }
    // #endif
}

int cal_entries_3nn(PtwID* q, Query_Context* qc, vector<Entry*>& v_ret) {
	if (qc->simple) {
		return cal_entries_3nn_sim(q, qc, v_ret);
	} else {
		return cal_entries_3nn_nonsim(q, qc, v_ret);
	}
}

int cal_entries_3lnn_nonsim(PtwID* q, Query_Context* qc, vector<Entry*>& v_ret) {

    v_ret.clear();

    cout.precision(10);

    int ret = 0;
    double err = qc->epsilon * 2;

    vector<int> range_a, range_b, range_c;
    #if VERBOSE > 0
        nn_sphere_range_and_show(q->pt, sq(qc->s_db.ann_min - err), err, qc, range_a);
        nn_sphere_range_and_show(q->pt, sq(qc->s_db.ann_mid - err), err, qc, range_b);
        nn_sphere_range_and_show(q->pt, sq(qc->s_db.ann_max - err), err, qc, range_c);
    #else
        nn_sphere_range(q->pt, sq(qc->s_db.ann_min - err), err, qc, range_a);
        nn_sphere_range(q->pt, sq(qc->s_db.ann_mid - err), err, qc, range_b);
        nn_sphere_range(q->pt, sq(qc->s_db.ann_max - err), err, qc, range_c);
    #endif

    vector<PtwID> range_pts_a, range_pts_b, range_pts_c;
    vector<Pt3D> range_qa, range_qb, range_qc;
    for (int &id: range_a) {
        auto pt = PtwID(id, &qc->mesh_q);
        range_pts_a.push_back(pt);
        range_qa.push_back(*pt.pt - *q->pt);
    }
    for (int &id: range_b) {
        auto pt = PtwID(id, &qc->mesh_q);
        range_pts_b.push_back(pt);
        range_qb.push_back(*pt.pt - *q->pt);
    }
    for (int &id: range_c) {
        auto pt = PtwID(id, &qc->mesh_q);
        range_pts_c.push_back(pt);
        range_qc.push_back(*pt.pt - *q->pt);
    }

    // loop for all the 3-combinations of (a, b, c)
    for (int i = 0; i < range_a.size(); i++) {
        for (int j = 0; j < range_b.size(); j++) {
            for (int k = 0; k < range_c.size(); k++) {

                double cos_th_ij = cos_theta(&range_qa[i], &range_qb[j]); // the angle of i & j
                double cos_th_ik = cos_theta(&range_qa[i], &range_qc[k]); // the angle of i & k
                double cos_th_jk = cos_theta(&range_qb[j], &range_qc[k]); // the angle of j & k
                // cout << cos_th_ij << " " << cos_th_ik << " " << cos_th_jk << endl;
                if (cos_th_ij > qc->cos_phi || cos_th_ik > qc->cos_phi || cos_th_jk > qc->cos_phi) {
                    continue; // if th < phi, skip it
                }

                Entry* q_entry = new Entry(q, &range_pts_a[i], &range_pts_b[j], &range_pts_c[k]);
                q_entry->fill_sides();

                v_ret.push_back(q_entry);
            }
        }
    }
}

int cal_entries_3lnn_sim(PtwID* q, Query_Context* qc, vector<Entry*>& v_ret) {

    v_ret.clear();

    cout.precision(10);

    int ret = 0;
    double err = qc->epsilon * 2;

    vector<int> range_a, range_b, range_c;
    #if VERBOSE > 0
        nn_sphere_range_and_show(q->pt, sq(qc->s_db.ann_min - err), err, qc, range_a);
        nn_sphere_range_and_show(q->pt, sq(qc->s_db.ann_mid - err), err, qc, range_b);
        nn_sphere_range_and_show(q->pt, sq(qc->s_db.ann_max - err), err, qc, range_c);
    #else
        nn_sphere_range(q->pt, sq(qc->s_db.ann_min - err), err, qc, range_a);
        nn_sphere_range(q->pt, sq(qc->s_db.ann_mid - err), err, qc, range_b);
        nn_sphere_range(q->pt, sq(qc->s_db.ann_max - err), err, qc, range_c);
    #endif

    vector<PtwID> range_pts_a, range_pts_b, range_pts_c;
    for (int &id: range_a) {
        auto pt = PtwID(id, &qc->mesh_q);
        range_pts_a.push_back(pt);
    }
    for (int &id: range_b) {
        auto pt = PtwID(id, &qc->mesh_q);
        range_pts_b.push_back(pt);
    }
    for (int &id: range_c) {
        auto pt = PtwID(id, &qc->mesh_q);
        range_pts_c.push_back(pt);
    }

    // loop for all the 3-combinations of (a, b, c)
    for (int i = 0; i < range_a.size(); i++) {
        for (int j = 0; j < range_b.size(); j++) {
            for (int k = 0; k < range_c.size(); k++) {

                Entry* q_entry = new Entry(q, &range_pts_a[i], &range_pts_b[j], &range_pts_c[k]);
                q_entry->fill_sides();

                v_ret.push_back(q_entry);
            }
        }
    }
}

int cal_entries_3lnn(PtwID* q, Query_Context* qc, vector<Entry*>& v_ret) {
	if (qc->simple) {
		return cal_entries_3lnn_sim(q, qc, v_ret);
	} else {
		return cal_entries_3lnn_nonsim(q, qc, v_ret);
	}
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

#ifdef TEST_MODE
double brute_force_nn(vector<int>& list, int cheat, Query_Context* qc, PtwID& ret) {
    PtwID cheat_pt = PtwID(cheat, &qc->mesh_q);

    double nn_dist = numeric_limits<double>::max();
    for (int i = 0; i < list.size(); i++) {
        if (list[i] != cheat) {
            PtwID pt = PtwID(list[i], &qc->mesh_q);
            double dist = eucl_dist(pt.pt, cheat_pt.pt);
            if (dist < nn_dist) {
                nn_dist = dist;
                ret = pt;
            }
        }
    }

    return nn_dist;
}
#endif

// #ifdef TEST_MODE
// int cal_entries(PtwID* q, Query_Context* qc, vector<Entry*>& v_ret, vector<int>& cheat_list_id, vector<double>& cheat_list_dist)
// #else
int cal_entries(PtwID* q, Query_Context* qc, vector<Entry*>& v_ret)
// #endif
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

    #if VERBOSE > 1
        #ifdef ACC
            nn_sphere_range_and_show(q->pt, sq(qc->s_db.ann_min - err), err, qc, range_a);
        #else
            get_potential_set_and_show(q->pt, qc->s_db.ann_min, qc, range_a);
        #endif
    #else
        #ifdef ACC
            nn_sphere_range(q->pt, sq(qc->s_db.ann_min - err), err, qc, range_a);
        #else
            get_potential_set(q->pt, qc->s_db.ann_min, qc, range_a);
        #endif
    #endif

    // #ifdef TEST_MODE // depr, used for identifying the nearest query tetrahedron candidate, and to test the error
    //     if (cheat_list_id[1] >= 0) {
    //         double nn_a_dist = brute_force_nn(range_a, cheat_list_id[1], qc, nn_a);
    //         cout << "The NN of ground-truth a = #" << cheat_list_id[1] << " is recognized: #" << nn_a.id << ", dist = " << nn_a_dist << endl;
    //     }
    // #endif

    for (auto &it_a: range_a) {

        if (it_a < 0)
            continue;

        // #ifdef TEST_MODE
        //     if (it_a != cheat_list_id[1])
        //         continue;
        // #endif

        // #if VERBOSE > 2
        //     cout << endl << "For a = " << qc->s_q.id_q_to_str(it_a) << endl;
        // #endif

        ten_a.set(it_a, &qc->mesh_q);

        Pt3D m;
        middle_pt(q->pt, ten_a.pt, m);
        double d_pm = eucl_dist(q->pt, &m);

        int excl_qa[2] = { q->id, ten_a.id };

        #if VERBOSE > 2
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

			// #ifdef TEST_MODE
			// 	if (it_h != cheat_list_id[4])
			// 		continue;
			// #endif

            #if VERBOSE > 3
                cout << endl << TAB << "For h = " << qc->s_q.id_q_to_str(it_h) << endl;
            #endif

            ten_h.set(it_h, &qc->mesh_q);

            Pt3D ten_b_est, ten_c_est;
            auto got_b_c = get_est_b_c(&m, ten_a.pt, ten_h.pt, ten_b_est, ten_c_est);
            if (!got_b_c) {
                continue;
            }

            #if VERBOSE > 3
                nn_sphere_range_and_show(&ten_b_est, 0.0, err, qc, range_b, TAB, excl_qa, 2);
            #else
                nn_sphere_range(&ten_b_est, 0.0, err, qc, range_b, excl_qa, 2);
            #endif

            // #ifdef TEST_MODE // depr, used for identifying the nearest query tetrahedron candidate, and to test the error
            //     if (cheat_list_id[2] >= 0) {
            //         double nn_b_dist = brute_force_nn(range_b, cheat_list_id[2], qc, nn_b);
            //         cout << "The NN of ground-truth b = #" << cheat_list_id[2] << " is recognized: #" << nn_b.id << ", dist = " << nn_b_dist << endl;
            //     }
            // #endif

            for (auto &it_b: range_b) {

                if (it_b < 0)
                    continue;

                // #ifdef TEST_MODE
                //     if (it_b != cheat_list_id[2])
                //         continue;
                // #endif

                #if VERBOSE > 4
                    cout << endl << TABTAB << "For b = " << qc->s_q.id_q_to_str(it_b) << endl;
                #endif

                ten_b.set(it_b, &qc->mesh_q);

                int excl_qab[3] = { q->id, ten_a.id, ten_b.id };

                #if VERBOSE > 4
                    nn_sphere_range_and_show(&ten_c_est, 0.0, err, qc, range_c, TABTAB, excl_qab, 3);
                #else
                    nn_sphere_range(&ten_c_est, 0.0, err, qc, range_c, excl_qab, 3);
                #endif

                // #ifdef TEST_MODE // depr, used for identifying the nearest query tetrahedron candidate, and to test the error
                //     if (cheat_list_id[3] >= 0) {
                //     	double nn_c_dist = brute_force_nn(range_c, cheat_list_id[3], qc, nn_c);
                //     	cout << "The NN of ground-truth c = #" << cheat_list_id[3] << " is recognized: #" << nn_c.id << ", dist = " << nn_c_dist << endl;
                //     }
                // #endif

                for (auto &it_c: range_c) {

                    if (it_c < 0)
                        continue;

                    // #ifdef TEST_MODE
                    //     if (it_c != cheat_list_id[3])
                    //         continue;
                    // #endif

                    #if VERBOSE > 5
                        cout << endl << TABTABTAB << "For c = " << qc->s_q.id_q_to_str(it_c) << endl;
                    #endif

                    ten_c = PtwID(it_c, &qc->mesh_q);

                    // // currently the ratio set (including the volume and volume ratio) is not required
                    // // since the indexing key are the side length
                    // auto ratio_set = get_ratio_set_vol(q->pt, ten_a.pt, ten_b.pt, ten_c.pt);
                    // Entry* q_entry = new Entry(*q, ten_a, ten_b, ten_c, ratio_set.volume, ratio_set.ratio, ten_h);

                    Entry* q_entry = new Entry(q, &ten_a, &ten_b, &ten_c, &ten_h);

                    if (insert_entry(q_entry, v_ret, hash_ret)) {
                        #if VERBOSE > 5
                            cout << TABTABTAB << "Include query entry: " << q_entry->to_str(10) << endl;
                        #endif

						q_entry->fill_sides();

                        // // no need to sort_entry anymore
                        // if (qc->sort_entry) {
                        //     q_entry->sort_sides();
                        // }

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

    // #ifdef TEST_MODE // depr, used for identifying the nearest query tetrahedron candidate, and to test the error
    //     Entry* nn_entry = new Entry(*q, nn_a, nn_b, nn_c, PtwID());
    //     nn_entry->fill_sides();
    //     insert_entry(nn_entry, v_ret, hash_ret);
    //     cout << endl << "Include the NN entry: " << nn_entry->to_str(10) << endl;
    // #endif

    return ret;
}

void prob_donut_range(Pt3D* o, Pt3D* n, double r/*?*/, double err, Query_Context* qc, vector<int>& ret) {

    int q_donut_nn;
    double q_donut_dist;
    if (qc->small_set) {
        q_donut_dist = donut_nn_quick(o, n, r, &qc->mesh_q, &qc->r_q, &q_donut_nn);
    } else {
        q_donut_dist = donut_nn(o, n, r, &qc->mesh_q, &qc->r_q, &q_donut_nn);
    }
    if (q_donut_nn < 0 || q_donut_nn >= qc->mesh_q.size()) {
        return;
    }

    auto donut_nn_pt = PtwID(q_donut_nn, &qc->mesh_q);
    double L = eucl_dist(donut_nn_pt.pt, o);

    vector<int> donut_range_cand;
    qc->r_q.range_sphere_min_max(o, L - 0.01, L + err * 2.0, donut_range_cand);

    double sq_r = sq(r);
    for (auto &i: donut_range_cand) {
        auto i_pt = PtwID(i, &qc->mesh_q);
        double donut_dist = sqrt(donut_dist_sq(i_pt.pt, o, n, r, sq_r));
        if (donut_dist <= q_donut_dist + err * 2.0) {
            ret.push_back(i);
        }
    }
}

void prob_donut_range_and_show(Pt3D* o, Pt3D* n, double r/*?*/, double err, Query_Context* qc, vector<int>& ret, string nav = "") {

    prob_donut_range(o, n, r, err, qc, ret);
    
    cout << nav << "donut_range-------RR:";
    for (auto &it: ret) {
        // cout << " " << s_q->id_q_to_str(it.oid) << "=" << (sqrt(it.dist) / RSTREE_SCALE);
        cout << " " << qc->s_q.id_q_to_str(it);
    }
    cout << endl;
}

void detm_donut_range() {

}

void detm_donut_range_and_show() {

}

int cal_entries_donut(PtwID* q, Query_Context* qc, vector<Entry*>& v_ret) {

    v_ret.clear();
    
    cout.precision(10);

    int ret = 0;
    double err = qc->epsilon * 2;

    vector<int> range_a, range_b, range_c;
    PtwID ten_a, ten_b, ten_c;

    #if VERBOSE > 1
        #ifdef ACC
            nn_sphere_range_and_show(q->pt, sq(qc->s_db.ann_min - err), err, qc, range_a);
        #else
            get_potential_set_and_show(q->pt, qc->s_db.ann_min, qc, range_a);
        #endif
    #else
        #ifdef ACC
            nn_sphere_range(q->pt, sq(qc->s_db.ann_min - err), err, qc, range_a);
        #else
            get_potential_set(q->pt, qc->s_db.ann_min, qc, range_a);
        #endif
    #endif

    for (auto &it_a: range_a) {

        if (it_a < 0)
            continue;

        #if VERBOSE > 2
            cout << endl << "For a = " << qc->s_q.id_q_to_str(it_a) << endl;
        #endif

        ten_a.set(it_a, &qc->mesh_q);
        auto qa = *(q->pt) - *(ten_a.pt);

        Pt3D m;
        middle_pt(q->pt, ten_a.pt, m);
        double d_pm = eucl_dist(q->pt, &m);
        double r_donut = d_pm * 1.73205080757;

        #if VERBOSE > 2
            #ifdef ACC
                prob_donut_range_and_show(&m, &qa, r_donut, err, qc, range_b, "");
            #else
                // detm_donut_range_and_show(); TODO
            #endif
        #else
            #ifdef ACC
                prob_donut_range(&m, &qa, r_donut, err, qc, range_b);
            #else
                // detm_donut_range(); TODO
            #endif
        #endif

        for (auto &it_b: range_b) {

            if (it_b < 0)
                continue;

            #if VERBOSE > 3
                cout << endl << TAB << "For b = " << qc->s_q.id_q_to_str(it_b) << endl;
            #endif

            ten_b.set(it_b, &qc->mesh_q);

            Pt3D ten_b_est, ten_c_est;
            auto got_b_c = get_est_b_c(&m, ten_a.pt, ten_b.pt, ten_b_est, ten_c_est);
            if (!got_b_c) {
                continue;
            }

            // if (it_a == 29287 && it_b == 36328) {
            //     cout << "fuck" << endl;
            //     print_pt(&m);
            //     cout << endl;
            //     print_pt(ten_a.pt);
            //     cout << endl;
            //     print_pt(ten_b.pt);
            //     cout << endl;
            //     print_pt(&ten_b_est);
            //     cout << endl;
            //     print_pt(&ten_c_est);
            //     cout << endl;

            //     PtwID ch_c;
            //     ch_c.set(29326, &qc->mesh_q);
            //     cout << "cheat dist: " << eucl_dist(ch_c.pt, &ten_c_est) << endl;
            // }

            int excl_qab[3] = { q->id, ten_a.id, ten_b.id };

            #if VERBOSE > 3
                nn_sphere_range_and_show(&ten_c_est, 0.0, err, qc, range_c, TAB, excl_qab, 3);
            #else
                nn_sphere_range(&ten_c_est, 0.0, err, qc, range_c, excl_qab, 3);
            #endif

            for (auto &it_c: range_c) {

                if (it_c < 0)
                    continue;

                #if VERBOSE > 4
                    cout << endl << TABTAB << "For c = " << qc->s_q.id_q_to_str(it_c) << endl;
                #endif

                ten_c.set(it_c, &qc->mesh_q);

                if (it_a == 29287 && it_b == 36328) {
                    cout << "real dist: " << eucl_dist(ten_c.pt, &ten_c_est) << endl;
                }

                // currently the ratio set (including the volume and volume ratio) is not required
                // since the indexing key are the side length
                // auto ratio_set = get_ratio_set_vol(q->pt, ten_a.pt, ten_b.pt, ten_c.pt);
                // Entry* q_entry = new Entry(*q, ten_a, ten_b, ten_c, ratio_set.volume, ratio_set.ratio, ten_h);

                Entry* q_entry = new Entry(q, &ten_a, &ten_b, &ten_c);

                q_entry->fill_sides();

                // if (qc->sort_entry) {
                //     q_entry->sort_sides();
                // }

                #if VERBOSE > 4
                    cout << TABTAB << "Include query entry: " << q_entry->to_str(10) << endl;
                #endif
                v_ret.push_back(q_entry);

                ret++;
            }
            range_c.clear();
        }
        range_b.clear();
    }
    range_a.clear();

    return ret;
}

bool recheck_congr(const Entry* e, const Entry* f, double err) {

    // #ifdef _3NN
        if (abs(e->sides[0] - f->sides[0]) > err) {
            return false;
        }
        if (abs(e->sides[1] - f->sides[1]) > err) {
            return false;
        }
        if (abs(e->sides[2] - f->sides[2]) > err) {
            return false;
        }
    // #else
    //     if (abs(e->sides[1] - f->sides[1]) > err) {
    //         return false;
    //     }
    //     if (abs(e->sides[2] - f->sides[2]) > err) {
    //         return false;
    //     }
    //     if (abs(e->sides[5] - f->sides[5]) > err) {
    //         return false;
    //     }
    // #endif

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

            #ifdef IDX_3
                if (!recheck_congr(e, f, corr_err)) {
                	continue;
                }
            #endif

            total_nhits++;
	        Entry_Pair* new_pair = new Entry_Pair(e, f, qc->s_db.get_grid_id_by_global_cell_id(hit_key));
	        ret.push_back(new_pair);

            #if VERBOSE > 4
                cout << "Found pair:" << endl << new_pair->to_str(10) << endl;
            #endif
	    }

        index++;
    }

    return total_nhits;
}

#ifdef TEST_MODE
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
#endif

double rand_double_in_range(double low, double high) {
	double r = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	return (low + r * (high - low));
}

// a test method
#ifdef TEST_MODE
Pt3D drift(Pt3D p, double tar_dist) {
	Pt3D ret;

	double a = rand_double_in_range(0.0, 1.0) - 0.5,
		   b = rand_double_in_range(0.0, 1.0) - 0.5,
		   c = rand_double_in_range(0.0, 1.0) - 0.5;
	double norm = sqrt(a * a + b * b + c * c);
	double ran_scale = tar_dist / norm;
	a *= ran_scale; b *= ran_scale; c *= ran_scale;

	ret.x = p.x + a;
	ret.y = p.y + a;
	ret.z = p.z + a;

	return ret;
}
#endif

class CPQ_Detector {

public:
	struct Single_Coll_Tree {
	    unordered_set<int> inserted_pt_id_set;
	    CollTree* tree;
	    Single_Coll_Tree() {
	        tree = new CollTree();
	    }
	    bool exists(int pt_id) {
	        if (inserted_pt_id_set.find(pt_id) == inserted_pt_id_set.end()) {
	            return false;
	        } else {
	            return true;
	        }
	    }
	    void insert(PtwID* p) {
	        int pt_box_min[3], pt_box_max[3];
	        pt_box_min[0] = pt_box_max[0] = (int) (p->pt->x * RSTREE_SCALE);
	        pt_box_min[1] = pt_box_max[1] = (int) (p->pt->y * RSTREE_SCALE);
	        pt_box_min[2] = pt_box_max[2] = (int) (p->pt->z * RSTREE_SCALE);
	        // cout << pt_box_min[0] << endl << pt_box_max[0] << endl << pt_box_min[1] << endl << pt_box_max[1] << endl << pt_box_min[2] << endl << pt_box_max[2] << endl;
	        tree->Insert(pt_box_min, pt_box_max, p->id);
	        inserted_pt_id_set.insert(p->id);
	    }
        // bool check_coll(Pt3D* q, double d, double& acc_time) {
        bool check_coll(Pt3D* q, double d) {
	        double center[3] = { q->x, q->y, q->z };
	        vector<int> v_ret;
            int query_box_min[3], query_box_max[3];
            for (int i = 0; i < 3; i++) {
                query_box_min[i] = (int) ((center[i] - d - 0.01) * RSTREE_SCALE);
                query_box_max[i] = (int) ((center[i] + d + 0.01) * RSTREE_SCALE);
            }
	        // timer_start();
            bool got = tree->YoNSearch(query_box_min, query_box_max); // TODO: use a fast SphereSearch instead
            // acc_time += timer_end(SECOND);
            return got;
	    }
        virtual ~Single_Coll_Tree() {
            delete tree;
        }
	};

    unordered_map<int, Single_Coll_Tree*> coll_map;
    double nn_dist;

    void set_coll_map(vector<Entry_Pair*>& nn_v_pairs, double t_nn_dist, Query_Context* qc) {
        for (auto &i: nn_v_pairs) {
            auto pi = PtwID(i->e_database->repre->id, qc->db_meshes.get_mesh(i->id_db));

            if (coll_map.find(i->id_db) == coll_map.end()) {
                // i->id_db not exists in coll_map
                Single_Coll_Tree* sct = new Single_Coll_Tree;
                coll_map[i->id_db] = sct;
            }
            if (!coll_map[i->id_db]->exists(pi.id)) {
                coll_map[i->id_db]->insert(&pi);
            }
        }

        for (auto &i: coll_map) {
            i.second->tree->SortDim0();
        }

        nn_dist = t_nn_dist;
    }

private:
	// bool check_coll(int id_db, Pt3D* pt, Query_Context* qc, double& acc_time, double& test_time) {
    bool check_coll(int id_db, Pt3D* pt, Query_Context* qc) {
        if (coll_map.find(id_db) == coll_map.end()) {
            return false;
        }

        // timer_start();
        // bool pass = coll_map[id_db]->check_coll(pt, nn_dist + 2 * qc->epsilon, acc_time);
        bool pass = coll_map[id_db]->check_coll(pt, nn_dist + 2 * qc->epsilon);
        // test_time += timer_end(SECOND);

        return pass;
    }

public:
    void check_coll_filter(vector<Entry_Pair*>& v_pairs, Query_Context* qc, vector<Entry_Pair*>& v_ret) {

        // double check_time = 0.0;
        // double query_time = 0.0;
        // double test_time = 0.0;

        for (auto &i: v_pairs) {
            PtwID pi(i->e_database->repre->id, qc->db_meshes.get_mesh(i->id_db));
            bool collision = false;

            // timer_start();
            // collision = check_coll(i->id_db, pi.pt, qc, query_time, test_time);
            collision = check_coll(i->id_db, pi.pt, qc);
            // check_time += timer_end(SECOND);

            if (collision) {
                // #if VERBOSE > 0
                //    cout << "Found collision: " << pi.id << endl;
                // #endif
                v_ret.push_back(i);
            }
        }

        // cout << "Total check time: " << check_time << endl;
        // cout << "Total query time: " << query_time << endl;
        // cout << "Total test time: " << test_time << endl;
    }

    void check_coll_filter_bf(vector<Entry_Pair*>& v_pairs, Query_Context* qc) {

        // for (auto itr = v_pairs.begin(); itr != v_pairs.end(); itr++) {
        //     auto i = *itr;
        //     auto pi = PtwID(i->e_database->repre->id, qc->db_meshes.get_mesh(i->id_db));
        //     bool collision = false;

        //     for (auto jtr = nn_v_pairs.begin(); jtr != nn_v_pairs.end(); jtr++) {
        //         auto j = *jtr;

        //         if (i->id_db != j->id_db) {
        //             continue;
        //         }

        //         auto pj = PtwID(j->e_database->repre->id, qc->db_meshes.get_mesh(j->id_db));

        //         double dist_ij = eucl_dist(&pi.pt, &pj.pt);
        //         if (dist_ij > nn_q_d + 2 * qc->epsilon) {
        //             continue;
        //         }

        //         // #if VERBOSE > 0
        //         //     cout << "Found collision: " << pi.id << " & " << pj.id << ", dist = " << dist_ij << endl;
        //         // #endif
        //         collision = true;
        //         break;
        //     }

        //     if (!collision) {
        //         v_pairs.erase(itr--);
        //     }
        // }
	}

    virtual ~CPQ_Detector() {
        for (auto &p: coll_map) {
            delete p.second;
        }
    }
};

struct Proposal_Stat {
    double cal_entries_time;
    double retrieve_congr_time;
    int num_entries;
    // int num_hits;
};

void pair_proposal(PtwID* q, Query_Context* qc, vector<Entry_Pair*>& v_pairs, Proposal_Stat& ps) {

	vector<Entry*> v_entries;

    #if VERBOSE > 0
        cout << endl << "Calculating entry list for query pt #" << q->id << " with min=" << qc->s_db.ann_min << endl;
    #endif

    timer_start();

    // #ifdef TEST_MODE
    //     #ifdef _3NN
    //         cal_entries_3nn(q, qc, v_entries, test_ret_id, test_ret_dist);
    //     #else
    //         cal_entries(q, qc, v_entries, test_ret_id, test_ret_dist);
    //     #endif
    // #else
        #ifdef _3NN
        	#ifdef _3LNN
                cal_entries_3lnn(q, qc, v_entries);
            #else
                cal_entries_3nn(q, qc, v_entries);
            #endif
        #else
            #ifdef _DNT
                cal_entries_donut(q, qc, v_entries);
            #else
                cal_entries(q, qc, v_entries);
            #endif
        #endif
    // #endif

    ps.cal_entries_time += timer_end(SECOND);
    ps.num_entries = v_entries.size();

    #if VERBOSE > 0
        cout << endl << "Size of the entry list for query pt #" << q->id << ": " << v_entries.size() << endl;
        #if VERBOSE > 4
            for (auto &e: v_entries)
                cout << e->to_str(10) << endl;
            cout << endl;
        #endif
    #endif

    timer_start();

    retrieve_congr_entry(v_entries, qc, v_pairs);

    ps.retrieve_congr_time += timer_end(SECOND);
    // ps.num_hits += v_pairs.size();

    #if VERBOSE > 0
        cout << endl << "Size of the hit list for query pt #" << q->id << ": " << v_pairs.size() << endl;
        #if VERBOSE > 4
            for (auto &p: v_pairs)
                cout << p->to_str(10) << endl;
            cout << endl;
        #endif
    #endif

    // #ifdef TEST_MODE // depr: testing the drift error effects
    //     double drift_dist = 0.1;
    //     for (int i = 0; i < 10; i++) {
    //     	PtwID drift_repr(v_entries[0]->repre->id, drift(v_entries[0]->repre->pt, drift_dist));
    //     	PtwID drift_rem0(v_entries[0]->remai[0]->id, drift(v_entries[0]->remai[0]->pt, drift_dist));
    //     	PtwID drift_rem1(v_entries[0]->remai[1]->id, drift(v_entries[0]->remai[1]->pt, drift_dist));
    //     	PtwID drift_rem2(v_entries[0]->remai[2]->id, drift(v_entries[0]->remai[2]->pt, drift_dist));
    //     	Entry* mock_entry_q = new Entry(drift_repr, drift_rem0, drift_rem1, drift_rem2, PtwID());
    //     	v_pairs.push_back(new Entry_Pair(mock_entry_q, v_pairs[0]->e_database, v_pairs[0]->id_db));
    //     }
    // #endif
}

bool iter(Query_Context* qc, Exec_stat& stat) {

    stat = (const struct Exec_stat) { 0 };

    timer_start();

    PtwID q;
    vector<Entry_Pair*> v_pairs;
    Proposal_Stat prop_stat = (const struct Proposal_Stat){ 0 };

    while (v_pairs.size() == 0 || v_pairs.size() > 50000) {

        v_pairs.clear();

        int selected_pt_id = rand() % qc->mesh_q.size();

        #ifdef TEST_MODE
            if (qc->force_cell >= 0) {
                selected_pt_id = qc->force_cell;
            }
        #endif

        q.set(selected_pt_id, &qc->mesh_q);

        // try to find non-boundary points
        Pt3D q_mbs_center;
        qc->mesh_q.get_bsphere_o(q_mbs_center);
        // cout << "Eucl dist: " << eucl_dist(q.pt, &q_mbs_center) << ", to compare with " << (qc->mesh_q.get_bsphere_r() - qc->s_db.ann_min) << endl;
        double sel_ratio = qc->small_set ? 0.9 : 0.5;
        while (eucl_dist(q.pt, &q_mbs_center) > sel_ratio * (qc->mesh_q.get_bsphere_r() - qc->s_db.ann_min)) { // set to 0.5 such that the largest dataset obtain the best performance
            // cout << "Boundary! reselect." << endl;
            selected_pt_id = rand() % qc->mesh_q.size();
            q.set(selected_pt_id, &qc->mesh_q);
        }

        #ifdef TEST_MODE
            vector<int> test_ret_id;
            vector<double> test_ret_dist;
            if (!test_verification_pt(&q, qc, test_ret_id, test_ret_dist, true)) {
            	cout << "Test failed because the selected query pt cannot be mapped to a repre pt in DB!" << endl;
            	timer_end(SECOND);
            	return false;
            }
        #endif

        pair_proposal(&q, qc, v_pairs, prop_stat);

    }

    stat.prop_time = timer_end(SECOND);
    stat.cal_entries_time = prop_stat.cal_entries_time;
    stat.retrieve_congr_time = prop_stat.retrieve_congr_time;
    stat.num_entries = prop_stat.num_entries;
    stat.num_init_pairs = v_pairs.size();

    vector<Entry_Pair*>* ptr_v_pairs = &v_pairs;

    #ifdef _CPQ

        timer_start();

        int nn_q_id;
    	double nn_q_d = qc->r_q.nn_sphere(q.pt, 0.01, &nn_q_id);

        PtwID nn_q(nn_q_id, &qc->mesh_q);

        #if VERBOSE > 0
           cout << "Pair proposal for query NN pt " << qc->s_q.id_q_to_str(nn_q_id) << " of dist = " << nn_q_d << endl;
        #endif

        vector<Entry_Pair*> nn_v_pairs;
        Proposal_Stat nn_prop_stat = (const struct Proposal_Stat){ 0 };

        timer_start();
        pair_proposal(&nn_q, qc, nn_v_pairs, nn_prop_stat);
        stat.cpq_prop_time = timer_end(SECOND);

        vector<Entry_Pair*> v_pairs_ret;

        if (nn_v_pairs.size() > 0) {

            CPQ_Detector detector;

            // timer_start();
            detector.set_coll_map(nn_v_pairs, nn_q_d, qc);
            // cout << "set_coll_map time: " << timer_end(SECOND) << endl;

            // timer_start();
            // cout << "check_coll_filter from size " << v_pairs.size() << endl;
            detector.check_coll_filter(v_pairs, qc, v_pairs_ret);
            // cout << "check_coll_filter time: " << timer_end(SECOND) << endl;
        
            #if VERBOSE > 0
                cout << "Collision complete, coll count = " << v_pairs_ret.size() << endl;
            #endif

            ptr_v_pairs = &v_pairs_ret;
        }

        stat.cpq_total_time = timer_end(SECOND);
        stat.prop_excpq_time = stat.prop_time;
        stat.prop_time += stat.cpq_total_time;
        stat.cpq_ovh_time = stat.cpq_total_time - stat.cpq_prop_time;

    #endif

    stat.veri_size = ptr_v_pairs->size();

    timer_start();
    timer_start();

    bool found_one_iter = false;
    double first_time_iter = 0.0;

    vector<Entry_Pair*> ret_left_icp_only;
    // Step 1: calculate transformation, initial distance, and leave those for step 2&3
    for (auto &h: *ptr_v_pairs) {
        h->cal_xf();
        // h->init_dist = qc->db_meshes.cal_corr_err(&qc->mesh_q, h->id_db, &h->xf); // TODO: use another unimplemented interface
        h->init_dist = qc->db_meshes.cal_corr_err(&qc->mesh_q, h->id_db, &h->xf, qc->delta); // TODO: simplified method, stops when error is larger
        // cout << "Initial distance: " << h->init_dist << " for pair: " << h->to_str(10) << endl;

        // if (h->init_dist <= sq(qc->delta)) {
        if (h->init_dist >= 0) {
            stat.num_verified++;
            if (stat.num_verified == 1 && !found_one_iter) {
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

    stat.num_icp_only = ret_left_icp_only.size();

    vector<Entry_Pair*> ret_left_goicp;
    // // Step 2: perform the ICP-only check
    // for (auto &r: ret_left_icp_only) {
    //     double updated_err = qc->goicp[r->id_db]->ICP(&r->xf); // ICP-only
    //     cout << "Updated distance with ICP-only: " << updated_err << endl;

    //     if (updated_err <= sq(qc->delta)) {
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

    stat.num_goicp = ret_left_goicp.size();

    // // Step 3: perform GoICP
    // for (auto &r: ret_left_goicp) {
    //  goicp[r->db_id].fly_init();
    //  goicp[r->db_id].OuterBnB(); // TODO: return false?

    //  // TODO
    // }

    if (!found_one_iter) {
        first_time_iter = timer_end(SECOND);
    }

    stat.veri_time = timer_end(SECOND);
    stat.first_verified_time += (stat.prop_time + first_time_iter);
    stat.user_time = (stat.prop_time + stat.veri_time);

    cout << "# verified: " << stat.num_verified << endl;

    if (stat.num_verified > 0)
        return true;
    else
        return false;
}

void exec(Query_Context* qc, Exec_stat& stat) {

    #ifdef PROB
        const int k_m = 20;
    #else
        const int k_m = 1;
    #endif

    Exec_stat stats[k_m];

    int num_itr = 0;
    bool find_accept = false;

    for (int t = 0; t < k_m; t++) {

        find_accept = iter(qc, stats[t]);
        num_itr++;

        if (find_accept)
            break;

        #ifdef TEST_MODE
            break;
        #endif

    }

    get_sum_stat(stats, num_itr, stat);

    stat.num_iterations = num_itr;
    // stat.success = find_accept;
    // if (!find_accept) {
    // 	stat.num_fail++;
    // }
}

int main(int argc, char **argv) {

    for (int i = 0; i < argc; i++) {
        cout << argv[i] << " ";
    }
    cout << endl << endl;

    #ifdef PROB
        if (argc < 5) {
            cerr << "Usage: " << argv[0] << " database_path grid_filename query_filename epsilon [-delta=...] [-simple (for 3NN or 3LNN)] [-sort_entry (for gt or donut)] [-stop_once] [-small] [-force_cell=...] [-force_pt=...]* [-stat=...]" << endl;
            exit(1);
        }
    #else
        if (argc < 5) {
            cerr << "Usage: " << argv[0] << " database_path grid_filename query_filename delta [-simple (for 3NN or 3LNN)] [-sort_entry (for gt or donut)] [-stop_once] [-small] [-force_cell=...] [-force_pt=...]* [-stat=...]" << endl;
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

    Exec_stat stats[exec_times];
    Exec_stat tmp_stat;
    int real_exec_times = 0;

    for (int exec_i = 0; exec_i < exec_times; exec_i++) {

        cout << "Execution #" << (exec_i + 1) << ": " << endl;
        exec(&qc, stats[exec_i]);
        cout << endl;

        #if VERBOSE >= 0
            print_stat(stats[exec_i]);
            cout << endl;
        #endif

        real_exec_times++;

        if (real_exec_times == 5 || real_exec_times == 10 || real_exec_times == 20) {
        	get_sum_stat(stats, real_exec_times, tmp_stat);
        	if (tmp_stat.user_time > 100.0) {
        		break;
        	}
        }
	}

	Exec_stat avg_stat;
	get_avg_stat(stats, real_exec_times, avg_stat);

    if (qc.write_stat) {
    	write_stat(avg_stat, qc.stat_filename);
    }

    cout << "Average stat over all the executions: " << endl << endl;
    print_stat(avg_stat);
    cout << endl;
}