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

class Query_Context {
public:
    int force_cell;
    unordered_set<int> force_pts;
    bool stop_once;

    string db_path;
    string grid_filename;
    string query_filename;
    double delta;
    string algorithm;
    double epsilon;
    int num_exe;
    int is_small;

    bool write_stat;
    string stat_filename;

    void read_param(int argc, char** argv) {

        if (argc < 9) {
            cerr << "Usage: " << argv[0]
                 << " database_path"
                 << " grid_filename"
                 << " query_filename"
                 << " delta"
                 << " algorithm"
                 << " epsilon"
                 << " num_exe"
                 << " is_small"
                 << " [-force_cell=...] [-force_pt=...]* [-stat=...]" << endl;
            exit(1);
        }

        force_cell = -1;
        force_pts.clear();
        write_stat = false;
        stat_filename = "";
        for (int i = 0; i < argc; i++) {
            string argv_str(argv[i]);
            if (argv_str.rfind("-force_cell", 0) == 0)
                force_cell = atoi(argv[i] + 12);
            else if (argv_str.rfind("-force_pt", 0) == 0)
                force_pts.insert(atoi(argv[i] + 10));
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
        algorithm = argv[(++argi)];
        epsilon = atof(argv[(++argi)]);
        num_exe = atoi(argv[(++argi)]);
        is_small = atoi(argv[(++argi)]);
    }

    DB_Meshes db_meshes;
    Struct_DB s_db;
    IndexTree tree;
    Mesh mesh_q;
    C_RTree r_q;
    Struct_Q s_q;

    int ground_truth_db_mesh_id;
    Mesh* ground_truth_mesh;
    int ground_truth_mapping_db[4];
    int ground_truth_mapping_q[4];

    // GoICP** goicp;
        
    vector<ICP3D<float>*> m_icps;
    vector<float*> m_data_icps;
    float* m_query_icp;

    bool load() {

        cout << "Reading database files from " << db_path << endl;
        const int num_meshes = db_meshes.read_from_path(db_path);
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

        // load the index entries tree
        string idx_filename = get_idx_filename(grid_filename);
        cout << "Loading index entries from " << idx_filename << endl;
        if (!tree.Load(idx_filename.c_str())) {
            cout << "Error loading index entries tree" << endl;
            return false;
        }

        cout << endl;

        cout << "Reading query mesh from " << query_filename << endl;
        mesh_q.read_from_path(query_filename);

        // load the query R-tree
        cout << "Reading query R-tree" << endl;
        r_q.read_from_mesh(query_filename);

        // load the query structure
        if (!s_q.read(query_filename + ".info")) {
            cout << "Error loading query structure" << endl;
            return false;
        }

        double q_diam = mesh_q.get_bsphere_d();
        delta *= (q_diam * (double) mesh_q.size());
        cout << "Diameter of query mesh: " << q_diam << ", thus delta is finally set to delta * " << q_diam << " * Q-Size(" << mesh_q.size() << ") = " << delta << endl;
        
        // goicp = new GoICP*[num_meshes];
        // for (int i = 0; i < num_meshes; i++) {
        //     goicp[i] = new GoICP;
        //     loadGoICP(db_meshes.get_mesh(i), &mesh_q, delta, *goicp[i]);

        //     // Build Distance Transform
        //     cout << endl << "Building Distance Transform of #" << i << "..." << endl;
        //     goicp[i]->BuildDT();

        //     goicp[i]->Initialize();
        // }
        // cout << endl;

        for (int i = 0; i < db_meshes.size(); i++) {
            auto new_icp = new ICP3D<float>();
            // new_icp->err_diff_def = err_diff;
            new_icp->err_diff_def = 1; //TODO
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
        m_query_icp = (float *) calloc(3 * mesh_q.size(), sizeof(float));
        int i, j;
        for (i = 0, j = 0; i < mesh_q.size(); i++) {
            m_query_icp[j++] = mesh_q.get_pt(i)->x;
            m_query_icp[j++] = mesh_q.get_pt(i)->y;
            m_query_icp[j++] = mesh_q.get_pt(i)->z;
        }

        return true;
    }
};

Query_Context qc;

double run_icp(Entry_Pair* ep) {
    double init_dist = qc.db_meshes.cal_corr_err(&qc.mesh_q, ep->id_db, &ep->xf);
    cout << "Initial distance: " << init_dist << endl;
    double r_array[9] {
        ep->xf.r11, ep->xf.r12, ep->xf.r13,
        ep->xf.r21, ep->xf.r22, ep->xf.r23,
        ep->xf.r31, ep->xf.r32, ep->xf.r33
    };
    ICP_Matrix R_icp(3, 3, r_array);
    double t_array[3] {
        ep->xf.tx, ep->xf.ty, ep->xf.tz
    };
    ICP_Matrix t_icp(3, 1, t_array);
    double updated_err = qc.m_icps[ep->id_db]->Run(qc.m_query_icp, qc.mesh_q.size(), R_icp, t_icp);
    cout << "Updated distance with ICP-only: " << updated_err << endl;

    return updated_err;
}

struct Proposal_Stat {
    double cal_entries_time;
    double retrieve_congr_time;
    int num_entries;
    // int num_hits;
    int nar_success;
};

void nn_sphere_range(Pt3D* q, double sq_dist, double err, vector<int>& ret, int excl_id_list[] = {}, int excl_id_num = 0) {

    qc.r_q.nn_sphere_range(q, sq_dist, err, ret, excl_id_list, excl_id_num);
}

void nn_sphere_range_and_show(Pt3D* q, double sq_dist, double err, vector<int>& ret, string nav = "", int excl_id_list[] = {}, int excl_id_num = 0) {

    qc.r_q.nn_sphere_range(q, sq_dist, err, ret, excl_id_list, excl_id_num);
    
    cout << nav << "nn_sphere_range-------RR:";
    for (auto &it: ret) {
        // cout << " " << s_q->id_q_to_str(it.oid) << "=" << (sqrt(it.dist) / RSTREE_SCALE);
        cout << " " << qc.s_q.id_q_to_str(it);
    }
    cout << endl;
}

// #ifdef TEST_MODE
// int cal_entries_3nn_sim(PtwID* q, vector<Entry*>& v_ret, vector<int>& cheat_list_id, vector<double>& cheat_list_dist)
// #else
int cal_entries_3nn_sim(PtwID* q, vector<Entry*>& v_ret, Proposal_Stat& ps)
// #endif
{
    v_ret.clear();

    cout.precision(10);

    int ret = 0;
    double err = qc.epsilon * 2;

    vector<int> range;
    #if VERBOSE > 1
        nn_sphere_range_and_show(q->pt, sq(qc.s_db.ann_min - err), err, range);
    #else
        nn_sphere_range(q->pt, sq(qc.s_db.ann_min - err), err, range);
    #endif

    int range_size = range.size();
    if (range.size() < 3) {
        return 0;
    }

    unordered_set<int> range_set;
    for (auto &it: range) {
        range_set.insert(it);
    }
    if (range_set.find(qc.ground_truth_mapping_q[1]) != range_set.end() &&
        range_set.find(qc.ground_truth_mapping_q[2]) != range_set.end() &&
        range_set.find(qc.ground_truth_mapping_q[3]) != range_set.end()) {
            cout << "Remaining points in range!" << endl;
            ps.nar_success = 1;
    }

    vector<PtwID> range_pts;
    for (int &id: range) {
        range_pts.emplace_back(id, &qc.mesh_q);
    }

    // #ifdef TEST_MODE
    //     Entry* q_entry = new Entry(q, PtwID(cheat_list_id[1], &qc.mesh_q), PtwID(cheat_list_id[2], &qc.mesh_q), PtwID(cheat_list_id[3], &qc.mesh_q));
    //     q_entry->fill_sides();
    //     v_ret.push_back(q_entry);
    // #else
        // loop for all the 3-combinations
        for (int i = 0; i < range_size; i++) {
            for (int j = 0; j < range_size; j++) {
                for (int k = 0; k < range_size; k++) {
                	if (i != j && i != k && j != k) {
	                    Entry* q_entry = new Entry(q, &range_pts[i], &range_pts[j], &range_pts[k]);
	                    q_entry->fill_sides();
	                    v_ret.push_back(q_entry);
	                }
                }
            }
        }
    // #endif
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

void prob_donut_range(Pt3D* o, Pt3D* n, double r/*?*/, double err, vector<int>& ret) {

    int q_donut_nn;
    double q_donut_dist;
    if (qc.is_small) {
        q_donut_dist = donut_nn_quick(o, n, r, &qc.mesh_q, &qc.r_q, &q_donut_nn);
    } else {
        q_donut_dist = donut_nn(o, n, r, &qc.mesh_q, &qc.r_q, &q_donut_nn);
    }
    if (q_donut_nn < 0 || q_donut_nn >= qc.mesh_q.size()) {
        return;
    }

    auto donut_nn_pt = PtwID(q_donut_nn, &qc.mesh_q);
    double L = eucl_dist(donut_nn_pt.pt, o);

    vector<int> donut_range_cand;
    qc.r_q.range_sphere_min_max(o, L - 0.01, L + err * 2.0, donut_range_cand);

    double sq_r = sq(r);
    for (auto &i: donut_range_cand) {
        auto i_pt = PtwID(i, &qc.mesh_q);
        double donut_dist = sqrt(donut_dist_sq(i_pt.pt, o, n, r, sq_r));
        if (donut_dist <= q_donut_dist + err * 2.0) {
            ret.push_back(i);
        }
    }
}

void prob_donut_range_and_show(Pt3D* o, Pt3D* n, double r/*?*/, double err, vector<int>& ret, string nav = "") {

    prob_donut_range(o, n, r, err, ret);
    
    cout << nav << "donut_range-------RR:";
    for (auto &it: ret) {
        // cout << " " << s_q->id_q_to_str(it.oid) << "=" << (sqrt(it.dist) / RSTREE_SCALE);
        cout << " " << qc.s_q.id_q_to_str(it);
    }
    cout << endl;
}

int cal_entries_donut(PtwID* q, vector<Entry*>& v_ret, Proposal_Stat& ps) {

    v_ret.clear();
    
    cout.precision(10);

    int ret = 0;
    double err = qc.epsilon * 2;

    vector<int> range_a, range_b, range_c;
    PtwID ten_a, ten_b, ten_c;

    #if VERBOSE > 1
        nn_sphere_range_and_show(q->pt, sq(qc.s_db.ann_min - err), err, range_a);
    #else
        nn_sphere_range(q->pt, sq(qc.s_db.ann_min - err), err, range_a);
    #endif

    for (auto &it_a: range_a) {
        if (it_a == qc.ground_truth_mapping_q[1]) {
            cout << "Second point in range!" << endl;
            ps.nar_success = 1;
        }
    }

    for (auto &it_a: range_a) {

        if (it_a < 0)
            continue;

        #if VERBOSE > 2
            cout << endl << "For a = " << qc.s_q.id_q_to_str(it_a) << endl;
        #endif

        ten_a.set(it_a, &qc.mesh_q);
        auto qa = *(q->pt) - *(ten_a.pt);

        Pt3D m;
        middle_pt(q->pt, ten_a.pt, m);
        double d_pm = eucl_dist(q->pt, &m);
        double r_donut = d_pm * 1.73205080757;

        #if VERBOSE > 2
            prob_donut_range_and_show(&m, &qa, r_donut, err, range_b, "");
        #else
            prob_donut_range(&m, &qa, r_donut, err, range_b);
        #endif

        for (auto &it_b: range_b) {

            if (it_b < 0)
                continue;

            #if VERBOSE > 3
                cout << endl << TAB << "For b = " << qc.s_q.id_q_to_str(it_b) << endl;
            #endif

            ten_b.set(it_b, &qc.mesh_q);

            Pt3D ten_b_est, ten_c_est;
            auto got_b_c = get_est_b_c(&m, ten_a.pt, ten_b.pt, ten_b_est, ten_c_est);
            if (!got_b_c) {
                continue;
            }

            int excl_qab[3] = { q->id, ten_a.id, ten_b.id };

            #if VERBOSE > 3
                nn_sphere_range_and_show(&ten_c_est, 0.0, err, range_c, TAB, excl_qab, 3);
            #else
                nn_sphere_range(&ten_c_est, 0.0, err, range_c, excl_qab, 3);
            #endif

            for (auto &it_c: range_c) {

                if (it_c < 0)
                    continue;

                #if VERBOSE > 4
                    cout << endl << TABTAB << "For c = " << qc.s_q.id_q_to_str(it_c) << endl;
                #endif

                ten_c.set(it_c, &qc.mesh_q);

                // currently the ratio set (including the volume and volume ratio) is not required
                // since the indexing key are the side length
                // auto ratio_set = get_ratio_set_vol(q->pt, ten_a.pt, ten_b.pt, ten_c.pt);
                // Entry* q_entry = new Entry(*q, ten_a, ten_b, ten_c, ratio_set.volume, ratio_set.ratio, ten_h);

                Entry* q_entry = new Entry(q, &ten_a, &ten_b, &ten_c);

                q_entry->fill_sides();

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

    if (abs(e->sides[0] - f->sides[0]) > err) {
        return false;
    }
    if (abs(e->sides[1] - f->sides[1]) > err) {
        return false;
    }
    if (abs(e->sides[2] - f->sides[2]) > err) {
        return false;
    }

    return true;
}

int retrieve_congr_entry(vector<Entry*>& e_list, vector<Entry_Pair*>& ret) {

    double corr_err = max(0.001, 2 * qc.epsilon);

    int total_nhits = 0;

    int index = 0;

    for (auto &e: e_list) {

        #ifdef _CLR
            e->set_color(qc.color_q[e->repre->id].clr);
        #endif

        vector<int> rr_ret;
        #ifdef _CLR
            int rr_num = window_query(&qc.tree, e, corr_err, qc.color_err, rr_ret);
        #else
            int rr_num = window_query(&qc.tree, e, corr_err, rr_ret);
        #endif

        #if VERBOSE > 4
            cout << "For entry #" << index << ", get " << rr_num << " window query results" << endl;
        #endif

	    for (auto &hit: rr_ret) {
            int hit_key = hit;

	        auto f = qc.s_db.get_entry(hit_key);

            #ifdef _IDX3
                if (!recheck_congr(e, f, corr_err)) {
                	continue;
                }
            #endif

            total_nhits++;
	        auto new_pair = new Entry_Pair(e, f, qc.s_db.get_grid_id_by_global_cell_id(hit_key));
	        ret.push_back(new_pair);

            #if VERBOSE > 4
                cout << "Found pair:" << endl << new_pair->to_str(10) << endl;
            #endif
	    }

        index++;
    }

    return total_nhits;
}

void load_ground_truth(PtwID* q) {

    qc.ground_truth_db_mesh_id = qc.s_q.get_db_mesh_id();
    qc.ground_truth_mesh = qc.db_meshes.get_mesh(qc.ground_truth_db_mesh_id);
    cout << "Query mapped to DB mesh: #" << qc.ground_truth_db_mesh_id << endl;

    qc.ground_truth_mapping_q[0] = q->id;
    qc.ground_truth_mapping_db[0] = qc.s_q.get_id_db_by_q(q->id);

    assert(qc.ground_truth_mapping_db[0] >= 0);
    assert(qc.s_db.look_up_repre_index(qc.ground_truth_mapping_db[0], qc.ground_truth_db_mesh_id));

    for (int i = 0; i < 3; i++) {
        qc.ground_truth_mapping_db[i + 1] = qc.s_db.get_remai_id(qc.ground_truth_mapping_db[0], i, qc.ground_truth_db_mesh_id);
        qc.ground_truth_mapping_q[i + 1] = qc.s_q.get_id_q_by_db(qc.ground_truth_mapping_db[i + 1]);
    }

    cout << "Ground truth DB entry:";
    for (int i = 0; i < 4; i++) {
        cout << " " << qc.ground_truth_mapping_db[i];
    }
    cout << endl;

    cout << "Ground truth Q entry:";
    for (int i = 0; i < 4; i++) {
        cout << " " << qc.ground_truth_mapping_q[i];
    }
    cout << endl;
}

double rand_double_in_range(double low, double high) {
	double r = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	return (low + r * (high - low));
}

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

    void set_coll_map(vector<Entry_Pair*>& nn_v_pairs, double t_nn_dist) {
    	coll_map.reserve(min(static_cast<int>(nn_v_pairs.size()), qc.db_meshes.size()));
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
    void check_coll_filter(vector<Entry_Pair*>& v_pairs, vector<Entry_Pair*>& v_ret) {
        double nn_dist_thresh = nn_dist + 2 * qc.epsilon + 0.01;
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

void pair_proposal(PtwID* q, vector<Entry*>& v_entries, vector<Entry_Pair*>& v_pairs, Proposal_Stat& ps) {

    #if VERBOSE > 0
        cout << endl << "Calculating entry list for query pt #" << q->id << " with min=" << qc.s_db.ann_min << endl;
    #endif

    timer_start();

    if (qc.algorithm == "3nn-adv") {
        cal_entries_3nn_sim(q, v_entries, ps);
    } else if (qc.algorithm == "donut-adv") {
        cal_entries_donut(q, v_entries, ps);
    }

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

    retrieve_congr_entry(v_entries, v_pairs);

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
}

bool iter(Exec_stat& stat) {

    stat = (const struct Exec_stat) { 0 };

    timer_start();

    PtwID q;
    vector<Entry*> v_entries;
    vector<Entry_Pair*> v_pairs;
    Proposal_Stat prop_stat = (const struct Proposal_Stat){ 0 };

    do {
        int selected_pt_id = rand() % qc.mesh_q.size();

        if (qc.force_cell >= 0) {
            selected_pt_id = qc.force_cell;
        }

        q.set(selected_pt_id, &qc.mesh_q);

        // try to find non-boundary points
        Pt3D q_mbs_center;
        qc.mesh_q.get_bsphere_o(q_mbs_center);
        // cout << "Eucl dist: " << eucl_dist(q.pt, &q_mbs_center) << ", to compare with " << (qc.mesh_q.get_bsphere_r() - qc.s_db.ann_min) << endl;
        double sel_ratio; // a ratio to avoid too boundary cases
        if (qc.is_small && qc.algorithm == "donut-adv") {
        	sel_ratio = 0.8;
        } else {
        	sel_ratio = 0.5;
        }
        // cout << "sel_ratio: " << sel_ratio << endl;
        while (eucl_dist(q.pt, &q_mbs_center) > sel_ratio * (qc.mesh_q.get_bsphere_r() - qc.s_db.ann_min)) { // set to 0.5 such that the largest dataset obtain the best performance
            // cout << "Boundary! reselect." << endl;
            selected_pt_id = rand() % qc.mesh_q.size();
            q.set(selected_pt_id, &qc.mesh_q);
        }

        load_ground_truth(&q);

        pair_proposal(&q, v_entries, v_pairs, prop_stat);

    } while (false); // (v_pairs.size() > 50000); // reselect when exceeding 50000, this helps to accelerate for large return set

    stat.prop_time = timer_end(SECOND);
    stat.cal_entries_time = prop_stat.cal_entries_time;
    stat.retrieve_congr_time = prop_stat.retrieve_congr_time;
    stat.num_entries = prop_stat.num_entries;
    stat.num_init_pairs = v_pairs.size();
    stat.num_nar_success = prop_stat.nar_success;

    // cout << "First propose in " << stat.prop_time << "(s), " << stat.num_entries << " entries, " << stat.num_init_pairs << " pairs" << endl;

    vector<Entry_Pair*>* ptr_v_pairs = &v_pairs;

    // int nn_q_id;
    // double nn_q_d;
    // PtwID nn_q;
    // Proposal_Stat nn_prop_stat = (const struct Proposal_Stat){ 0 };
    // CPQ_Detector detector;
    // vector<Entry*> nn_v_entries;
    // vector<Entry_Pair*> nn_v_pairs;

    // vector<Entry_Pair*> v_pairs_ret;

    // if (qc.use_cf) {

    //     timer_start();

    // 	nn_q_d = qc.r_q.nn_sphere(q.pt, 0.01, &nn_q_id);

    //     nn_q.set(nn_q_id, &qc.mesh_q);

    //     #if VERBOSE > 0
    //        cout << "Pair proposal for query NN pt " << qc.s_q.id_q_to_str(nn_q_id) << " of dist = " << nn_q_d << endl;
    //     #endif

    //     #ifdef TEST_MODE
    //         vector<int> nn_test_ret_id;
    //         vector<double> nn_test_ret_dist;
    //         if (!test_verification_pt(&nn_q, nn_test_ret_id, nn_test_ret_dist, true)) {
    //             cout << "Test failed because the selected query pt cannot be mapped to a repre pt in DB!" << endl;
    //             timer_end(SECOND);
    //             return false;
    //         }
    //     #endif

    //     timer_start();
    //     pair_proposal(&nn_q, nn_v_entries, nn_v_pairs, nn_prop_stat);
    //     stat.cpq_prop_time = timer_end(SECOND);

    //     if (nn_v_pairs.size() > 0) {

    //         // timer_start();
    //         detector.set_coll_map(nn_v_pairs, nn_q_d);
    //         // cout << "set_coll_map time: " << timer_end(SECOND) << endl;

    //         // timer_start();
    //         // cout << "check_coll_filter from size " << v_pairs.size() << endl;
    //         detector.check_coll_filter(v_pairs, v_pairs_ret);
    //         // cout << "check_coll_filter time: " << timer_end(SECOND) << endl;
        
    //         #if VERBOSE > 0
    //             cout << "Collision complete, coll count = " << v_pairs_ret.size() << endl;
    //             #if VERBOSE > 4
    //                 for (auto &p: v_pairs_ret)
    //                     cout << p->to_str(10) << endl;
    //                 cout << endl;
    //             #endif
    //         #endif

    //         ptr_v_pairs = &v_pairs_ret;
    //     }

    //     stat.cpq_total_time = timer_end(SECOND);
    //     stat.prop_excpq_time = stat.prop_time;
    //     stat.prop_time += stat.cpq_total_time;
    //     stat.cpq_ovh_time = stat.cpq_total_time - stat.cpq_prop_time;

    // }

    stat.veri_size = ptr_v_pairs->size();

    timer_start();
    timer_start();

    bool found_one_iter = false;
    double first_time_iter = 0.0;

    for (auto &h: *ptr_v_pairs) {
        h->cal_xf();
        // cout << "Cal dist for\n" << h->e_query->to_str() << "\nand\n" << h->e_database->to_str() << "\n in db obj#" << h->id_db << endl;
        h->init_dist = qc.db_meshes.cal_corr_err(&qc.mesh_q, h->id_db, &h->xf, qc.delta);

        // if (h->init_dist <= qc.delta) {
        if (h->init_dist >= 0) {
            // print_trans(&h->xf);
            cout << "Dist: " << h->init_dist << "(MSE=" << (h->init_dist / (double) qc.mesh_q.size() / qc.mesh_q.get_bsphere_d()) << ")" << endl;
            cout << "Matched with DB #" << h->id_db << ": " << qc.db_meshes.get_mesh(h->id_db)->get_filename() << endl;
            // cout << "Matched pair:" << endl;
            // cout << h->to_str(10) << endl;
            stat.num_verified++;
            if (stat.num_verified == 1 && !found_one_iter) {
                first_time_iter = timer_end(SECOND);
                found_one_iter = true;
            }
        }
    }

    if (!found_one_iter) {

        if (qc.ground_truth_mapping_q[1] >= 0 && qc.ground_truth_mapping_q[2] >= 0 && qc.ground_truth_mapping_q[3] >= 0) {

            PtwID gt_1(qc.ground_truth_mapping_q[1], &qc.mesh_q);
            PtwID gt_2(qc.ground_truth_mapping_q[2], &qc.mesh_q);
            PtwID gt_3(qc.ground_truth_mapping_q[3], &qc.mesh_q);
            auto gt_e = new Entry(&q, &gt_1, &gt_2, &gt_3);
            gt_e->fill_sides();

            auto gt_f = qc.s_db.get_entry_trim(qc.ground_truth_mapping_db[0], qc.ground_truth_db_mesh_id);

            auto gt_pair = new Entry_Pair(gt_e, gt_f, qc.ground_truth_db_mesh_id);

            cout << "Run ICP for the ground-truth pair:" << endl;

            double updated_err = run_icp(gt_pair);

            if (updated_err <= qc.delta) {
                cout << "Dist: " << updated_err << "(MSE=" << (updated_err / (double) qc.mesh_q.size() / qc.mesh_q.get_bsphere_d()) << ")" << endl;
                cout << "Matched with DB #" << qc.ground_truth_db_mesh_id << ": " << qc.db_meshes.get_mesh(qc.ground_truth_db_mesh_id)->get_filename() << endl;
                stat.num_verified++;
                if (stat.num_verified == 1 && !found_one_iter) {
                    first_time_iter = timer_end(SECOND);
                    found_one_iter = true;
                }
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

    if (stat.num_verified > 0)
        return true;
    else
        return false;
}

void exec(Exec_stat& stat) {

    int k_m = 8;

    Exec_stat stats[k_m];

    int num_itr = 0;
    bool find_accept = false;

    for (int t = 0; t < k_m; t++) {

        find_accept = iter(stats[t]);
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

    Exec_stat tmp_stats[qc.num_exe];
    Exec_stat tmp_sum_stat;
    int real_exec_times = 0;

    for (int exec_i = 0; exec_i < qc.num_exe; exec_i++) {

        cout << "Execution #" << (exec_i + 1) << ": " << endl;
        exec(tmp_stats[exec_i]);
        cout << endl;

        #if VERBOSE >= 0
            print_stat(tmp_stats[exec_i]);
            cout << endl;
        #endif

        real_exec_times++;

        if (real_exec_times == 5 || real_exec_times == 10 || real_exec_times == 20) {
        	get_sum_stat(tmp_stats, real_exec_times, tmp_sum_stat);
        	if (tmp_sum_stat.user_time > 100.0) {
        		break;
        	}
        }
	}

	Exec_stat avg_stat;
	get_avg_stat(tmp_stats, real_exec_times, avg_stat);

    if (qc.write_stat) {
    	write_stat(avg_stat, qc.stat_filename);
    }

    cout << "Average stat over all the executions: " << endl << endl;
    print_stat(avg_stat);
    cout << endl;

}