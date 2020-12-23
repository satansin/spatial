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
#include <map>
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
    string query_filename_recon;
    double delta;
    double epsilon;
    double color_err;

    double cos_phi; // in query this threshold is slightly larger

    string query_filename_real;

    #ifdef _TK
        int k;
        double delta_0;
        double epsilon_0;
    #endif

    string output_ply;

    void read_param(int argc, char** argv) {

        #ifdef _TK
            #ifdef _PROB
                if (argc < 8) {
                    cerr << "Usage: " << argv[0] << " database_path grid_filename query_filename k delta epsilon color_err output_ply [-simple (for 3NN or 3LNN)] [-sort_entry (for gt or donut)] [-stop_once] [-small] [-force_cell=...] [-force_pt=...]* [-stat=...]" << endl;
                    exit(1);
                }
            #else
                if (argc < 7) {
                    cerr << "Usage: " << argv[0] << " database_path grid_filename query_filename k delta color_err output_ply [-simple (for 3NN or 3LNN)] [-sort_entry (for gt or donut)] [-stop_once] [-small] [-force_cell=...] [-force_pt=...]* [-stat=...]" << endl;
                    exit(1);
                }
            #endif
        #else
            #ifdef _PROB
                if (argc < 7) {
                    cerr << "Usage: " << argv[0] << " database_path grid_filename query_filename delta epsilon color_err output_ply [-simple (for 3NN or 3LNN)] [-sort_entry (for gt or donut)] [-stop_once] [-small] [-force_cell=...] [-force_pt=...]* [-stat=...]" << endl;
                    exit(1);
                }
            #else
                if (argc < 6) {
                    cerr << "Usage: " << argv[0] << " database_path grid_filename query_filename delta color_err output_ply [-simple (for 3NN or 3LNN)] [-sort_entry (for gt or donut)] [-stop_once] [-small] [-force_cell=...] [-force_pt=...]* [-stat=...]" << endl;
                    exit(1);
                }
            #endif
        #endif

        force_cell = -1;
        force_pts.clear();
        stop_once = false;
        small_set = false;
        sort_entry = false;
        simple = false;
        write_stat = false;
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
        }

        int argi = 0;

        db_path = argv[(++argi)];
        grid_filename = argv[(++argi)];
        query_filename_recon = argv[(++argi)];
        // query_filename_real = argv[(++argi)];
        query_filename_real = query_filename_recon;

        #ifdef _TK
            k = atoi(argv[(++argi)]);
        #endif

        delta = atof(argv[(++argi)]);

        #ifdef _PROB
            epsilon = atof(argv[(++argi)]);
        #endif

        color_err = atof(argv[(++argi)]);

        output_ply = argv[(++argi)];
    }

    DB_Meshes db_meshes;
    Struct_DB s_db;
    IndexTree tree;
    Mesh mesh_q;
    C_RTree r_q;
    Struct_Q s_q;
    #ifdef _CLR
        struct S_color {
            double clr[3];
        };
        vector<S_color> color_q;
    #endif

    Mesh mesh_q_real;

    GoICP** goicp;
        
    vector<ICP3D<float>*> m_icps;
    vector<float*> m_data_icps;
    float* m_query_icp;

    bool load() {

        cout << "Reading database files from " << db_path << endl;
        const int num_meshes = db_meshes.read_from_file(db_path);
        if (num_meshes < 0) {
            cerr << "Error reading database files" << endl;
            return false;
        }
        cout << "Total no. meshes: " << num_meshes << endl << endl;

        // load the DB structure
        string grid_bin_filename = get_bin_filename(grid_filename);
        cout << "Loading DB structure from " << grid_bin_filename << endl;
        if (!s_db.read_bin(grid_bin_filename, &db_meshes)) {
            cerr << "Error loading DB structure" << endl;
            return false;
        }
        cout << "Total no. cells: " << s_db.get_total_cells_count() << endl;

        cout << "Building KDtrees for DB meshes" << endl;
        db_meshes.build_kd();

        // db_meshes.free_mesh();

        cos_phi = cos(s_db.ang_min * PI / 180.0);

        // load the index entries tree
        string idx_filename = get_idx_filename(grid_filename);
        cout << "Loading index entries from " << idx_filename << endl;
        if (!tree.Load(idx_filename.c_str())) {
            cout << "Error loading index entries tree" << endl;
            return false;
        }

        cout << endl;

        cout << "Reading recon query mesh from " << query_filename_recon << endl;
        mesh_q.read_from_path(query_filename_recon);

        // load the query R-tree
        cout << "Reading query R-tree" << endl;
        r_q.read_from_mesh(query_filename_recon);

        // // load the query structure
        // if (!s_q.read(query_filename_recon + ".info")) {
        //     cout << "Error loading query structure" << endl;
        //     return false;
        // }

        #ifdef _CLR
            // load the query colors
        	cout << "Reading query color file" << endl;
            ifstream ifs_color_q(query_filename_recon + ".color");
            for (int i = 0; i < mesh_q.size(); i++) {
                S_color s_c;
                int dummy;
                ifs_color_q >> dummy;
                for (int j = 0; j < 3; j++) {
                    ifs_color_q >> s_c.clr[j];
                }
                color_q.push_back(s_c);
            }
            ifs_color_q.close();
        #endif

        cout << "Reading real query mesh from " << query_filename_real << endl;
        mesh_q_real.read_from_path(query_filename_real);

        double q_diam = mesh_q_real.get_bsphere_d();
        delta *= q_diam;
        cout << "Diameter of query mesh: " << q_diam << ", thus delta is finally set to delta * " << q_diam << " = " << delta << endl;
        #ifndef _PROB
            epsilon = sqrt(delta); // for deterministic queries, epsilon is set according to delta
            cout << "Epsilon is finally set to sqrt(delta) = " << epsilon << endl;
        #endif

        #ifdef _TK
            delta_0 = delta;
            epsilon_0 = epsilon;
        #endif

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

        for (int i = 0; i < db_meshes.size(); i++) {
            auto new_icp = new ICP3D<float>();
            // new_icp->err_diff_def = err_diff;
            new_icp->err_diff_def = 0.001; //TODO
            new_icp->do_trim = false;

            auto t = db_meshes.get_mesh(i);
            int t_size = t->size();
            auto new_data_icp = (float *) calloc(3 * t_size, sizeof(float));
            int k, j;
            for(k = 0, j = 0; k < t_size; k++) {
                new_data_icp[j++] = t->get_pt(k)->x;
                new_data_icp[j++] = t->get_pt(k)->y;
                new_data_icp[j++] = t->get_pt(k)->z;
            }
            // Build ICP kdtree with model dataset
            new_icp->Build(new_data_icp, t_size);

            m_icps.push_back(new_icp);
            m_data_icps.push_back(new_data_icp);
        }
        m_query_icp = (float *) calloc(3 * mesh_q_real.size(), sizeof(float));
        int i, j;
        for (i = 0, j = 0; i < mesh_q_real.size(); i++) {
            m_query_icp[j++] = mesh_q_real.get_pt(i)->x;
            m_query_icp[j++] = mesh_q_real.get_pt(i)->y;
            m_query_icp[j++] = mesh_q_real.get_pt(i)->z;
        }

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

    qc->r_q.range_sphere_min_max(q, rad - 2 * qc->epsilon, qc->s_db.ann_min + 2 * qc->epsilon, ret, new_excl_id_set);
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

    qc->r_q.range_sphere_min_max(q, rad - 2 * qc->epsilon, qc->s_db.ann_min + 2 * qc->epsilon, ret, new_excl_id_set);

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
        nn_sphere_range_and_show(q->pt, sq(qc->s_db.ann_min - err), 1.2 * err, qc, range); // set to (1.2 * err) instead of err to ensure the accuracy
    #else
        nn_sphere_range(q->pt, sq(qc->s_db.ann_min - err), 1.2 * err, qc, range);
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

    // #ifdef TEST_MODE
    //     PtwID nn_a, nn_b, nn_c;
    // #endif

    #if VERBOSE > 1
        #ifdef _ACC
            nn_sphere_range_and_show(q->pt, sq(qc->s_db.ann_min - err), err, qc, range_a);
        #else
            get_potential_set_and_show(q->pt, qc->s_db.ann_min, qc, range_a);
        #endif
    #else
        #ifdef _ACC
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
            #ifdef _ACC
                nn_sphere_range_and_show(&m, sq(d_pm - err), err, qc, range_h, "", excl_qa, 2);
            #else
                get_potential_set_and_show(&m, d_pm, qc, range_h, "", excl_qa, 2);
            #endif
        #else
            #ifdef _ACC
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

void detm_donut_range(Pt3D* o, Pt3D* n, double r/*?*/, double err, Query_Context* qc, vector<int>& ret) { // TODO: more research

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

    // cout << "q_donut_nn: " << q_donut_nn << endl;

    auto donut_nn_pt = PtwID(q_donut_nn, &qc->mesh_q);
    double L = eucl_dist(donut_nn_pt.pt, o);

    vector<int> donut_range_cand;
    qc->r_q.range_sphere_min_max(o, L - err * 2.0, L + err * 2.0, donut_range_cand);
    // cout << "Filter dist to o between " << (L - err * 2.0) << " and " << (L + err * 2.0) << endl;
    // cout << "Dist 15: " << eucl_dist(qc->mesh_q.get_pt(15), o) << endl;

    double sq_r = sq(r);
    for (auto &i: donut_range_cand) {
        auto i_pt = PtwID(i, &qc->mesh_q);
        double donut_dist = sqrt(donut_dist_sq(i_pt.pt, o, n, r, sq_r));
        // cout << i << ": " << donut_dist << endl;
        if (donut_dist <= q_donut_dist + err * 4.0) {
            ret.push_back(i);
        }
    }
}

void detm_donut_range_and_show(Pt3D* o, Pt3D* n, double r/*?*/, double err, Query_Context* qc, vector<int>& ret, string nav = "") {

	detm_donut_range(o, n, r, err, qc, ret);

    cout << nav << "donut_range-------RR:";
    for (auto &it: ret) {
        // cout << " " << s_q->id_q_to_str(it.oid) << "=" << (sqrt(it.dist) / RSTREE_SCALE);
        cout << " " << qc->s_q.id_q_to_str(it);
    }
    cout << endl;
}

int cal_entries_donut(PtwID* q, Query_Context* qc, vector<Entry*>& v_ret) {

    v_ret.clear();
    
    cout.precision(10);

    int ret = 0;
    double err = qc->epsilon * 2;
    err = 3.0;

    vector<int> range_a, range_b, range_c;
    PtwID ten_a, ten_b, ten_c;

    #if VERBOSE > 1
        #ifdef _ACC
            nn_sphere_range_and_show(q->pt, sq(qc->s_db.ann_min - err), err, qc, range_a);
        #else
            get_potential_set_and_show(q->pt, qc->s_db.ann_min, qc, range_a);
        #endif
    #else
        #ifdef _ACC
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
            #ifdef _ACC
                prob_donut_range_and_show(&m, &qa, r_donut, err, qc, range_b, "");
            #else
                detm_donut_range_and_show(&m, &qa, r_donut, err, qc, range_b, "");
            #endif
        #else
            #ifdef _ACC
                prob_donut_range(&m, &qa, r_donut, err, qc, range_b);
            #else
                detm_donut_range(&m, &qa, r_donut, err, qc, range_b);
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

bool recheck_congr(const Entry* e, const Entry_trim* f, double err) {

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

        #ifdef _CLR
            e->set_color(qc->color_q[e->repre->id].clr);
        #endif

        vector<int> rr_ret;
        #ifdef _CLR
            int rr_num = window_query(&qc->tree, e, corr_err, qc->color_err, rr_ret);
        #else
            int rr_num = window_query(&qc->tree, e, corr_err, rr_ret);
        #endif

        #if VERBOSE > 4
            cout << "For entry #" << index << ", get " << rr_num << " window query results" << endl;
        #endif

	    for (auto &hit: rr_ret) {
            int hit_key = hit;

	        auto f = qc->s_db.get_entry(hit_key);

            #ifdef IDX_3
                if (!recheck_congr(e, f, corr_err)) {
                	continue;
                }
            #endif

            total_nhits++;
	        auto new_pair = new Entry_Pair(e, f, qc->s_db.get_grid_id_by_global_cell_id(hit_key));
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
        vector<Pt3D*> inserted_pt_list;
	    CollTree* tree;
	    Single_Coll_Tree() {
            tree = nullptr;
	    }
	    bool exists(int pt_id) {
	        if (inserted_pt_id_set.find(pt_id) == inserted_pt_id_set.end()) {
	            return false;
	        } else {
	            return true;
	        }
	    }
        void insert_single(int pt_id, Pt3D* pt) {
            int pt_box_min[3], pt_box_max[3];
            pt_box_min[0] = pt_box_max[0] = (int) (pt->x * RSTREE_SCALE);
            pt_box_min[1] = pt_box_max[1] = (int) (pt->y * RSTREE_SCALE);
            pt_box_min[2] = pt_box_max[2] = (int) (pt->z * RSTREE_SCALE);
            // cout << pt_box_min[0] << endl << pt_box_max[0] << endl << pt_box_min[1] << endl << pt_box_max[1] << endl << pt_box_min[2] << endl << pt_box_max[2] << endl;
            tree->Insert(pt_box_min, pt_box_max, 0);
        }
	    void insert(int pt_id, Pt3D* pt) {
            inserted_pt_id_set.insert(pt_id);
            inserted_pt_list.emplace_back(pt);
            if (tree != nullptr) {
                insert_single(pt_id, pt);
            } else {
                if (inserted_pt_list.size() > TREE_BUILD_THRESHOLD) {
                    // cout << "start a tree" << endl;
                    tree = new CollTree();
                    for (auto &v: inserted_pt_list) {
                        insert_single(0, v);
                    }
                }
            }
	    }
        bool check_coll(Pt3D* q, double d, double sq_d) {
            if (tree != nullptr) {
                double center[3] = { q->x, q->y, q->z };
                int query_box_min[3], query_box_max[3];
                for (int i = 0; i < 3; i++) {
                    query_box_min[i] = (int) ((center[i] - d) * RSTREE_SCALE);
                    query_box_max[i] = (int) ((center[i] + d) * RSTREE_SCALE);
                }
                bool got = tree->YoNSearch(query_box_min, query_box_max); // TODO: use a fast SphereSearch instead
                return got;
            } else {
                for (auto &v: inserted_pt_list) {
                    // cout << " check dist=" << sq_dist(v, q);
                    if (sq_dist(v, q) <= sq_d) {
                        return true;
                    }
                }
                return false;
            }
	    }
        virtual ~Single_Coll_Tree() {
            if (tree != nullptr) {
                delete tree;
            }
        }
	};

    unordered_map<int, Single_Coll_Tree*> coll_map;
    double nn_dist;

    void set_coll_map(vector<Entry_Pair*>& nn_v_pairs, double t_nn_dist, Query_Context* qc) {
    	coll_map.reserve(min(static_cast<int>(nn_v_pairs.size()), qc->db_meshes.size()));
        for (auto &i: nn_v_pairs) {
            auto got = coll_map.find(i->id_db);
            if (got == coll_map.end()) { // i->id_db not exists in coll_map
                Single_Coll_Tree* sct = new Single_Coll_Tree;
                sct->insert(i->e_database->repre->id, i->e_database->repre->pt);
                coll_map[i->id_db] = sct;
            } else {
                if (!got->second->exists(i->e_database->repre->id)) {
                    got->second->insert(i->e_database->repre->id, i->e_database->repre->pt);
                }
            }
        }

        for (auto &i: coll_map) {
            if (i.second->tree != nullptr) {
                i.second->tree->SortDim0();
            }
        }

        nn_dist = t_nn_dist;
    }

private:
    static const int TREE_BUILD_THRESHOLD = 10;

    bool check_coll(int id_db, Pt3D* pt, double nn_dist_thresh, double sq_dist_thresh) {
    	auto got = coll_map.find(id_db);
        if (got == coll_map.end()) {
            return false;
        }

        // cout << "    Got Single_Coll_Tree for db#" << id_db << endl;
        bool pass = got->second->check_coll(pt, nn_dist_thresh, sq_dist_thresh);
        // cout << endl;

        return pass;
    }

public:
    void check_coll_filter(vector<Entry_Pair*>& v_pairs, Query_Context* qc, vector<Entry_Pair*>& v_ret) {
        double nn_dist_thresh = nn_dist + 2 * qc->epsilon + 0.01;
        double sq_dist_thresh = sq(nn_dist_thresh);

        v_ret.reserve(v_pairs.size());
        for (auto &i: v_pairs) {
            // cout << "To check pt#" << i->e_database->repre->id << " in db#" << i->id_db << " with nn_dist_thresh=" << nn_dist_thresh << " sq_dist_thresh=" << sq_dist_thresh << endl;
            if (check_coll(i->id_db, i->e_database->repre->pt, nn_dist_thresh, sq_dist_thresh)) {
                // #if VERBOSE > 0
                //    cout << "Found collision: " << pi.id << endl;
                // #endif
                v_ret.push_back(i);
            }
        }
        v_ret.shrink_to_fit();
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

void pair_proposal(PtwID* q, Query_Context* qc, vector<Entry*>& v_entries, vector<Entry_Pair*>& v_pairs, Proposal_Stat& ps) {

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

bool iter_icp(Query_Context* qc, Exec_stat& stat, Trans& output_xf) {

    stat = (const struct Exec_stat) { 0 };

    timer_start();

    Trans identity_xf = {
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 0.0
    };

    double init_dist = qc->db_meshes.cal_corr_err(&qc->mesh_q_real, 0, &identity_xf);
    cout << "Initial distance: " << init_dist << endl;

    int inlier_count = 0;
    for (int i = 0; i < qc->mesh_q_real.size(); i++) {
        auto pt = qc->mesh_q_real.get_pt(i);
        double sing_dist = qc->db_meshes.cal_corr_err(pt->x, pt->y, pt->z, 0);
        if (sing_dist * sing_dist <= qc->delta) {
            inlier_count++;
        }
    }
    cout << "Inlier perc: " << ((double) inlier_count / qc->mesh_q_real.size()) << endl;

    double r_array[9] {
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0
    };
    ICP_Matrix R_icp(3, 3, r_array);
    double t_array[3] {
        0.0, 0.0, 0.0
    };
    ICP_Matrix t_icp(3, 1, t_array);
    double updated_err = qc->m_icps[0]->Run(qc->m_query_icp, qc->mesh_q_real.size(), R_icp, t_icp);
    cout << "Updated distance with ICP-only: " << updated_err << endl;

    cout << R_icp << endl;
    cout << t_icp << endl;

    output_xf = {
        R_icp.val[0][0], R_icp.val[0][1], R_icp.val[0][2],
        R_icp.val[1][0], R_icp.val[1][1], R_icp.val[1][2],
        R_icp.val[2][0], R_icp.val[2][1], R_icp.val[2][2],
        t_icp.val[0][0], t_icp.val[0][1], t_icp.val[0][2]
    };
    print_trans(&output_xf);

    inlier_count = 0;
    for (int i = 0; i < qc->mesh_q_real.size(); i++) {
        auto pt = qc->mesh_q_real.get_pt(i);
        Pt3D tr_pt;
        trans_pt(&output_xf, pt, tr_pt);
        double sing_dist = qc->db_meshes.cal_corr_err(tr_pt.x, tr_pt.y, tr_pt.z, 0);
        if (sing_dist * sing_dist <= qc->delta) {
            inlier_count++;
        }
    }
    cout << "Inlier perc: " << ((double) inlier_count / qc->mesh_q_real.size()) << endl;

    stat.user_time = timer_end(SECOND);

    return true;
}

bool iter(Query_Context* qc, Exec_stat& stat, Trans& output_xf) {

    stat = (const struct Exec_stat) { 0 };

    timer_start();

    PtwID q;
    vector<Entry*> v_entries;
    vector<Entry_Pair*> v_pairs;
    Proposal_Stat prop_stat = (const struct Proposal_Stat){ 0 };

    do {

        if (!v_entries.empty()) {
            for (auto &v: v_entries) {
                if (v != nullptr) {
                    delete v;
                    v = nullptr;
                }
            }
            vector<Entry*>().swap(v_entries);
        }
        if (!v_pairs.empty()) {
            for (auto &v: v_pairs) {
                if (v != nullptr) {
                    delete v;
                    v = nullptr;
                }
            }
            vector<Entry_Pair*>().swap(v_pairs);
        }

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
        double sel_ratio = qc->small_set ? 0.8 : 0.5;
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
            	// timer_end(SECOND);
            	// return false;
            }
        #endif

        pair_proposal(&q, qc, v_entries, v_pairs, prop_stat);

    } while (false); // (v_pairs.size() > 50000); // reselect when exceeding 50000, this helps to accelerate for large return set

    stat.prop_time = timer_end(SECOND);
    stat.cal_entries_time = prop_stat.cal_entries_time;
    stat.retrieve_congr_time = prop_stat.retrieve_congr_time;
    stat.num_entries = prop_stat.num_entries;
    stat.num_init_pairs = v_pairs.size();

    // cout << "First propose in " << stat.prop_time << "(s), " << stat.num_entries << " entries, " << stat.num_init_pairs << " pairs" << endl;

    vector<Entry_Pair*>* ptr_v_pairs = &v_pairs;

    #ifdef _CPQ

        timer_start();

        int nn_q_id;
    	double nn_q_d = qc->r_q.nn_sphere(q.pt, 0.01, &nn_q_id);

        PtwID nn_q(nn_q_id, &qc->mesh_q);

        #if VERBOSE > 0
           cout << "Pair proposal for query NN pt " << qc->s_q.id_q_to_str(nn_q_id) << " of dist = " << nn_q_d << endl;
        #endif

        vector<Entry*> nn_v_entries;
        vector<Entry_Pair*> nn_v_pairs;
        Proposal_Stat nn_prop_stat = (const struct Proposal_Stat){ 0 };

        #ifdef TEST_MODE
            vector<int> nn_test_ret_id;
            vector<double> nn_test_ret_dist;
            if (!test_verification_pt(&nn_q, qc, nn_test_ret_id, nn_test_ret_dist, true)) {
                cout << "Test failed because the selected query pt cannot be mapped to a repre pt in DB!" << endl;
                timer_end(SECOND);
                return false;
            }
        #endif

        timer_start();
        pair_proposal(&nn_q, qc, nn_v_entries, nn_v_pairs, nn_prop_stat);
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
                #if VERBOSE > 4
                    for (auto &p: v_pairs_ret)
                        cout << p->to_str(10) << endl;
                    cout << endl;
                #endif
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

    struct Xf_with_rate {
        double inlier_rate;
        Trans xf;
        Xf_with_rate(double ir, Trans p_xf) {
            inlier_rate = ir;
            xf = p_xf;
        }
        bool operator<(const Xf_with_rate& a) const {
            return inlier_rate > a.inlier_rate;
        }
    };

    vector<Xf_with_rate> xfs;

    // Step 1: calculate transformation, initial distance, and leave those for step 2&3
    for (auto &h: *ptr_v_pairs) {
        h->cal_xf();
        // cout << "Cal dist for\n" << h->e_query->to_str() << "\nand\n" << h->e_database->to_str() << "\n in db obj#" << h->id_db << endl;
        int inlier_count = 0;
        for (int i = 0; i < qc->mesh_q_real.size(); i++) {
            auto pt = qc->mesh_q_real.get_pt(i);
            Pt3D tr_pt;
            trans_pt(&h->xf, pt, tr_pt);
            double sing_dist = qc->db_meshes.cal_corr_err(tr_pt.x, tr_pt.y, tr_pt.z, h->id_db);
            if (sing_dist * sing_dist <= qc->delta) {
                inlier_count++;
            }
        }
        double inlier_rate = (double) inlier_count / qc->mesh_q_real.size();
        xfs.emplace_back(inlier_rate, h->xf);
    }

    sort(xfs.begin(), xfs.end());
    const int max_trial_icp = 3;
    for (int i = 0; i < max_trial_icp && i < xfs.size(); i++) {
        cout << "Test inlier rate: " << xfs[i].inlier_rate << endl;
        if (xfs[i].inlier_rate >= 0.1) {
            print_trans(&xfs[i].xf);

            double r_array[9] {
                xfs[i].xf.r11, xfs[i].xf.r12, xfs[i].xf.r13,
                xfs[i].xf.r21, xfs[i].xf.r22, xfs[i].xf.r23,
                xfs[i].xf.r31, xfs[i].xf.r32, xfs[i].xf.r33
            };
            ICP_Matrix R_icp(3, 3, r_array);
            double t_array[3] {
                xfs[i].xf.tx, xfs[i].xf.ty, xfs[i].xf.tz
            };
            ICP_Matrix t_icp(3, 1, t_array);
            qc->m_icps[0]->Run(qc->m_query_icp, qc->mesh_q_real.size(), R_icp, t_icp);

            Trans icp_xf = {
                R_icp.val[0][0], R_icp.val[0][1], R_icp.val[0][2],
                R_icp.val[1][0], R_icp.val[1][1], R_icp.val[1][2],
                R_icp.val[2][0], R_icp.val[2][1], R_icp.val[2][2],
                t_icp.val[0][0], t_icp.val[0][1], t_icp.val[0][2]
            };
            cout << "After ICP:" << endl;
            print_trans(&icp_xf);

            int icp_inlier_count = 0;
            for (int i = 0; i < qc->mesh_q_real.size(); i++) {
                auto pt = qc->mesh_q_real.get_pt(i);
                Pt3D tr_pt;
                trans_pt(&icp_xf, pt, tr_pt);
                double sing_dist = qc->db_meshes.cal_corr_err(tr_pt.x, tr_pt.y, tr_pt.z, 0);
                if (sing_dist * sing_dist <= qc->delta) {
                    icp_inlier_count++;
                }
            }
            double icp_inlier_rate = (double) icp_inlier_count / qc->mesh_q_real.size();
            cout << "ICP inlier perc: " << icp_inlier_rate << endl;

            if (icp_inlier_rate >= 0.7) {
                output_xf = icp_xf;
                stat.num_verified++;
                if (stat.num_verified == 1 && !found_one_iter) {
                    first_time_iter = timer_end(SECOND);
                    found_one_iter = true;
                }

                break;
            }
        }
    }

    if (!found_one_iter) {
        first_time_iter = timer_end(SECOND);
    }

    stat.veri_time = timer_end(SECOND);
    stat.first_verified_time += (stat.prop_time + first_time_iter);
    stat.user_time = (stat.prop_time + stat.veri_time);

    cout << "# verified: " << stat.num_verified << endl;

    if (!v_entries.empty()) {
        for (auto &v: v_entries) {
            if (v != nullptr) {
                delete v;
                v = nullptr;
            }
        }
        vector<Entry*>().swap(v_entries);
    }
    if (!v_pairs.empty()) {
        for (auto &v: v_pairs) {
            if (v != nullptr) {
                delete v;
                v = nullptr;
            }
        }
        vector<Entry_Pair*>().swap(v_pairs);
    }

    #ifdef _CPQ
        if (!nn_v_entries.empty()) {
            for (auto &v: nn_v_entries) {
                if (v != nullptr) {
                    delete v;
                    v = nullptr;
                }
            }
            vector<Entry*>().swap(nn_v_entries);
        }
        if (!nn_v_pairs.empty()) {
            for (auto &v: nn_v_pairs) {
                if (v != nullptr) {
                    delete v;
                    v = nullptr;
                }
            }
            vector<Entry_Pair*>().swap(nn_v_pairs);
        }
    #endif

    if (stat.num_verified > 0)
        return true;
    else
        return false;
}

void exec(Query_Context* qc, Exec_stat& stat, Trans& output_xf) {

    #ifdef _PROB
        const int k_m = 20;
    #else
        const int k_m = 1;
    #endif

    Exec_stat stats[k_m];

    int num_itr = 0;
    bool find_accept = false;

    for (int t = 0; t < k_m; t++) {

        find_accept = iter(qc, stats[t], output_xf);
        // find_accept = iter_icp(qc, stats[t], output_xf);
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

#ifdef _TK
void exec_tk(Query_Context* qc, Exec_stat& stat) {

    vector<Exec_stat> stats;

    qc->delta = qc->delta_0;
    qc->epsilon = qc->epsilon_0;

    #ifdef _PROB
        int k_m = 12;
    #else
        int k_m = 1;
    #endif
    int num_trial = 0;

    while (true) {

        cout << "Trial #" << (++num_trial) << " with delta: " << qc->delta << ", epsilon " << qc->epsilon << ": " << endl;

        Exec_stat stats_itr[k_m];
        int num_itr = 0;
        int num_accepted = 0;
        for (int t = 0; t < k_m; t++) {
            num_itr++;
            if (iter(qc, stats_itr[t])) {
                num_accepted = stats_itr[t].num_verified;
                break;
            }
        }

        Exec_stat stat_itr;
        get_sum_stat(stats_itr, num_itr, stat_itr);

        stats.push_back(stat_itr);
        // cout << stat_itr.num_verified << endl;
        // cout << qc->k << endl;

        if (num_accepted >= qc->k)
            break;

        #ifndef _PROB
            qc->epsilon *= 2.0;
        #endif
        qc->delta *= 4.0;
    }

    get_sum_stat(&stats[0], num_trial, stat);

    stat.num_iterations = num_trial;
}
#endif

void fuse_ply(Query_Context* qc, Trans* output_xf) {

    Mesh fuse_mesh(qc->db_meshes.get_mesh(0));

    for (int i = 0; i < qc->mesh_q_real.size(); i++) {
        auto pt = qc->mesh_q_real.get_pt(i);
        Pt3D tr_pt;
        trans_pt(output_xf, pt, tr_pt);
        double sing_dist = qc->db_meshes.cal_corr_err(tr_pt.x, tr_pt.y, tr_pt.z, 0);
        if (sing_dist * sing_dist > qc->delta) { // add new point to original point cloud
            fuse_mesh.insert_pt(tr_pt);
        }
    }

    fuse_mesh.write(qc->output_ply);
    cout << "Write fused mesh to " << qc->output_ply << endl;
}

int main(int argc, char **argv) {

    // for (int i = 0; i < argc; i++) {
    //     cout << argv[i] << " ";
    // }
    // cout << endl << endl;

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

    Exec_stat stat;
    Trans output_xf;
    exec(&qc, stat, output_xf);
    cout << endl;

    print_stat(stat);
    cout << endl;

    fuse_ply(&qc, &output_xf);
}