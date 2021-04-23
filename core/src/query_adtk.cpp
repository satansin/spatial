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
#include <list>
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
    string query_filename_recon;
    string query_filename_real;
    string question;
    double delta;
    int k;
    string require;
    string algorithm;
    double epsilon;
    // int use_cf; // not yet include cf_h
    double color_err;
    int num_exe;
    int is_small;
    int is_simp;

    double cheat_ratio;

    bool write_stat;
    string stat_filename;

    // double cos_phi; // in query this threshold is slightly larger

    void read_param(int argc, char** argv) {

        if (argc < 16) {
            cerr << "Usage: " << argv[0]
                 << " database_path"
                 << " grid_filename"
                 << " query_filename_recon"
                 << " query_filename_real"
                 << " question(delta_sim/top_k)"
                 << " delta"
                 << " k"
                 << " require(det/prob)"
                 << " algorithm"
                 << " epsilon"
                 // << " use_cf"
                 << " color_err"
                 << " num_exe"
                 << " is_small"
                 << " is_simp"
                 << " ch_ratio"
                 << " [-stop_once] [-force_cell=...] [-force_pt=...]* [-stat=...]" << endl;
            exit(1);
        }

        force_cell = -1;
        force_pts.clear();
        stop_once = false;
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
            }
        }

        int argi = 0;

        db_path = argv[(++argi)];
        grid_filename = argv[(++argi)];
        query_filename_recon = argv[(++argi)];
        query_filename_real = argv[(++argi)];
        // query_filename_real = query_filename_recon;

        question = argv[(++argi)];
        delta = atof(argv[(++argi)]);
        k = atoi(argv[(++argi)]);
        require = argv[(++argi)];
        algorithm = argv[(++argi)];
        epsilon = atof(argv[(++argi)]);
        // use_cf = atoi(argv[(++argi)]);
        color_err = atof(argv[(++argi)]);
        num_exe = atoi(argv[(++argi)]);
        is_small = atoi(argv[(++argi)]);
        is_simp = atoi(argv[(++argi)]);

        cheat_ratio = atof(argv[(++argi)]);

        #ifdef TEST_MODE
            num_exe = 1;
        #endif
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

        // db_meshes.free_mesh();

        // cos_phi = cos(s_db.ang_min * PI / 180.0);

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
        delta *= (q_diam * (double) mesh_q_real.size());
        cout << "Diameter of query mesh: " << q_diam << ", thus delta is finally set to delta * " << q_diam << " * Q-Size(" << mesh_q_real.size() << ") = " << delta << endl;
        
        if (require == "det") {
            epsilon = sqrt(delta); // for deterministic queries, epsilon is set according to delta
            cout << "Epsilon is finally set to sqrt(delta) = " << epsilon << endl;
        }


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
            new_icp->err_diff_def = 10; //TODO
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

Query_Context qc;

double run_icp(Entry_Pair* ep) {
    double init_dist = qc.db_meshes.cal_corr_err(&qc.mesh_q, ep->id_db, &ep->xf);
    // cout << "Initial distance: " << init_dist << endl;
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
    // cout << "Updated distance with ICP-only: " << updated_err << endl;

    return updated_err;
}

struct Top_k_list {
    list<double> dist_list;
    void init(int k, double init_delta) {
        for (int i = 0; i < k; i++) {
            dist_list.push_back(init_delta);
        }
    }
    double max_dist() {
        double ret = dist_list.back();
        return ret;
    }
    double insert(double new_dist) {
        dist_list.push_back(new_dist);
        dist_list.sort();
        dist_list.pop_back();
        cout << dist_list.size() << ":";
        for (auto &v: dist_list) {
            cout << " " << v;
        }
        cout << endl;
    }
};

void get_potential_set(Pt3D* q, double rad, double err, vector<int>& ret, int excl_id_list[] = {}, int excl_id_num = 0) {

    qc.r_q.nn_sphere_range(q, sq(rad + err), err * 2.0, ret, excl_id_list, excl_id_num);

    unordered_set<int> new_excl_id_set;
    for (int i = 0; i < excl_id_num; i++) {
        new_excl_id_set.insert(excl_id_list[i]);
    }
    for (auto &v: ret) {
        new_excl_id_set.insert(v);
    }

    qc.r_q.range_sphere_min_max(q, max(0.0, rad - err), rad + err, ret, new_excl_id_set);
}

void get_potential_set_and_show(Pt3D* q, double rad, double err, vector<int>& ret, string nav = "", int excl_id_list[] = {}, int excl_id_num = 0) {

    qc.r_q.nn_sphere_range(q, sq(rad + err), err * 2.0, ret, excl_id_list, excl_id_num);

    unordered_set<int> new_excl_id_set;
    for (int i = 0; i < excl_id_num; i++) {
        new_excl_id_set.insert(excl_id_list[i]);
    }
    for (auto &v: ret) {
        new_excl_id_set.insert(v);
    }

    qc.r_q.range_sphere_min_max(q, max(0.0, rad - err), rad + err, ret, new_excl_id_set);

    cout << nav << "Potential set:";
    for (auto &it: ret) {
        // cout << " " << s_q->id_q_to_str(it.oid) << "=" << (sqrt(it.dist) / RSTREE_SCALE);
        cout << " " << qc.s_q.id_q_to_str(it);
    }
    cout << endl;
}

void get_potential_list(Pt3D* q, double rad, double err, list<int>& ret, int excl_id_list[] = {}, int excl_id_num = 0) {

    vector<int> receiver;
    get_potential_set(q, rad, err, receiver, excl_id_list, excl_id_num);
    // get_potential_set_and_show(q, rad, err, receiver, "", excl_id_list, excl_id_num);
    for (auto &v: receiver) {
        ret.push_back(v);
    }
}

void get_potential_unordered_set(Pt3D* q, double rad, double err, unordered_set<int>& ret, int excl_id_list[] = {}, int excl_id_num = 0) {

    vector<int> receiver;
    get_potential_set(q, rad, err, receiver, excl_id_list, excl_id_num);
    for (auto &v: receiver) {
        ret.insert(v);
    }
}

void get_potential_3nn_set(Pt3D* q, double rad, double err, vector<int>& ret, string nav = "") {

	int nn_sp_3[3];
	qc.r_q.nn_sphere(q, rad + err, nn_sp_3, {}, 3);

    // cout << nn_sp_3[0] << " " << nn_sp_3[1] << " " << nn_sp_3[2] << endl;

    if (nn_sp_3[0] >= 0) {
	   ret.push_back(nn_sp_3[0]);
    }
    if (nn_sp_3[1] >= 0) {
	   ret.push_back(nn_sp_3[1]);
    }

    if (nn_sp_3[2] >= 0) {
    	auto pt_nn_sp_3 = qc.mesh_q.get_pt(nn_sp_3[2]);
    	auto nn_sp_3_dist = eucl_dist(q, pt_nn_sp_3);
    	qc.r_q.range_sphere_min_max(q, nn_sp_3_dist - 0.01, nn_sp_3_dist + err, ret);
    }

    qc.r_q.range_sphere_min_max(q, max(0.0, rad - err), rad + err, ret);
}

void get_potential_3nn_set_and_show(Pt3D* q, double rad, double err, vector<int>& ret, string nav = "") {

	int nn_sp_3[3];
	qc.r_q.nn_sphere(q, rad + err, nn_sp_3, {}, 3);
	cout << nav << "Found the 3 NNs out of r+2e:";
	for (int i = 0; i < 3; i++) {
		cout << " " << nn_sp_3[i];
	}
	cout << endl;

	ret.push_back(nn_sp_3[0]);
	ret.push_back(nn_sp_3[1]);

	auto pt_nn_sp_3 = qc.mesh_q.get_pt(nn_sp_3[2]);
	auto nn_sp_3_dist = eucl_dist(q, pt_nn_sp_3);
	cout << nav << "NN dist of NN-3: " << nn_sp_3_dist << endl;

	qc.r_q.range_sphere_min_max(q, nn_sp_3_dist - 0.01, nn_sp_3_dist + err, ret);
    qc.r_q.range_sphere_min_max(q, max(0.0, rad - err), rad + err, ret);

    cout << nav << "Potential 3NN set:";
    for (auto &it: ret) {
        // cout << " " << s_q->id_q_to_str(it.oid) << "=" << (sqrt(it.dist) / RSTREE_SCALE);
        cout << " " << qc.s_q.id_q_to_str(it);
    }
    cout << endl;
}

void get_potential_3nn_list(Pt3D* q, double rad, double err, list<int>& ret, string nav = "") {

    vector<int> receiver;
    get_potential_3nn_set(q, rad, err, receiver, nav);
    for (auto &v: receiver) {
        ret.push_back(v);
    }
}

void get_potential_3nn_unordered_set(Pt3D* q, double rad, double err, unordered_set<int>& ret, string nav = "") {

    vector<int> receiver;
    get_potential_3nn_set(q, rad, err, receiver, nav);
    for (auto &v: receiver) {
        ret.insert(v);
    }
}

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

void nn_sphere_range_list(Pt3D* q, double sq_dist, double err, list<int>& ret, int excl_id_list[] = {}, int excl_id_num = 0) {

    vector<int> receiver;
    nn_sphere_range(q, sq_dist, err, receiver, excl_id_list, excl_id_num);
    // nn_sphere_range_and_show(q, sq_dist, err, receiver, "", excl_id_list, excl_id_num);
    for (auto &v: receiver) {
        ret.push_back(v);
    }
}

void nn_sphere_range_unordered_set(Pt3D* q, double sq_dist, double err, unordered_set<int>& ret, int excl_id_list[] = {}, int excl_id_num = 0) {

    vector<int> receiver;
    nn_sphere_range(q, sq_dist, err, receiver, excl_id_list, excl_id_num);
    for (auto &v: receiver) {
        ret.insert(v);
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

void detm_donut_range(Pt3D* o, Pt3D* n, double r/*?*/, double err, vector<int>& ret) { // TODO: more research

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

    // cout << "q_donut_nn: " << q_donut_nn << endl;

    auto donut_nn_pt = PtwID(q_donut_nn, &qc.mesh_q);
    double L = eucl_dist(donut_nn_pt.pt, o);

    vector<int> donut_range_cand;
    qc.r_q.range_sphere_min_max(o, max(0.0, L - err * 2.0), L + err * 2.0, donut_range_cand);
    // cout << "Filter dist to o between " << (L - err * 2.0) << " and " << (L + err * 2.0) << endl;
    // cout << "Dist 15: " << eucl_dist(qc.mesh_q.get_pt(15), o) << endl;

    double sq_r = sq(r);
    for (auto &i: donut_range_cand) {
        auto i_pt = PtwID(i, &qc.mesh_q);
        double donut_dist = sqrt(donut_dist_sq(i_pt.pt, o, n, r, sq_r));
        // cout << i << ": " << donut_dist << endl;
        if (donut_dist <= q_donut_dist + err * 4.0) {
            ret.push_back(i);
        }
    }
}

void detm_donut_range_and_show(Pt3D* o, Pt3D* n, double r/*?*/, double err, vector<int>& ret, string nav = "") {

    detm_donut_range(o, n, r, err, ret);

    cout << nav << "donut_range-------RR:";
    for (auto &it: ret) {
        // cout << " " << s_q->id_q_to_str(it.oid) << "=" << (sqrt(it.dist) / RSTREE_SCALE);
        cout << " " << qc.s_q.id_q_to_str(it);
    }
    cout << endl;
}

void detm_donut_range_list(Pt3D* o, Pt3D* n, double r/*?*/, double err, list<int>& ret) {
    
    vector<int> receiver;
    detm_donut_range(o, n, r, err, receiver);
    // detm_donut_range_and_show(o, n, r, err, receiver);
    for (auto &v: receiver) {
        ret.push_back(v);
    }
}

void detm_donut_range_unordered_set(Pt3D* o, Pt3D* n, double r/*?*/, double err, unordered_set<int>& ret) {
    
    vector<int> receiver;
    detm_donut_range(o, n, r, err, receiver);
    for (auto &v: receiver) {
        ret.insert(v);
    }
}

bool recheck_congr(const Entry* e, const Entry_trim* f, double err) {

    for (int i = 0; i < 6; i++) {
        if (abs(e->sides[i] - f->sides[i]) > err) {
            return false;
        }
    }

    return true;
}

double complete_transformation(Entry_Pair* h, double current_delta) {

    h->cal_xf();

    // cout << "Cal dist for\n" << h->e_query->to_str() << "\nand\n" << h->e_database->to_str() << "\n in db obj#" << h->id_db << endl;
    h->init_dist = qc.db_meshes.cal_corr_err(&qc.mesh_q_real, h->id_db, &h->xf, current_delta); // TODO: simplified method, stops when error is larger
    // h->init_dist = qc.db_meshes.cal_corr_err(&qc.mesh_q_real, h->id_db, &h->xf); // TODO: simplified method, stops when error is larger
    // cout << "Initial distance: " << h->init_dist << endl;// << " for pair: " << h->to_str(10) << endl;

    // if (h->init_dist <= qc.delta) {
    if (h->init_dist >= 0) {
        // print_trans(&h->xf);
        cout << "Updated dist: " << h->init_dist << "(MSE=" << (h->init_dist / (double) qc.mesh_q_real.size() / qc.mesh_q_real.get_bsphere_d()) << ")" << endl;
        return h->init_dist;
    }

    double icp_err = run_icp(h);

    if (icp_err <= current_delta) {
        cout << "Updated dist: " << icp_err << "(MSE=" << (icp_err / (double) qc.mesh_q_real.size() / qc.mesh_q_real.get_bsphere_d()) << ")" << endl;
        return icp_err;
    }

    // // Step 3: perform GoICP
    // for (auto &r: ret_left_goicp) {
    //  goicp[r->db_id].fly_init();
    //  goicp[r->db_id].OuterBnB(); // TODO: return false?

    //  // TODO
    // }

    return -1;
}

int filter_remaining(list<int>& range, const list<int>::iterator& it, const unordered_set<int>& ref) {

    list<int>::iterator ct(it);
    ct++;
    int num_remove = 0;
    while (ct != range.end()) {
        if (ref.find(*ct) == ref.end()) {
            ct = range.erase(ct);
            num_remove++;
        } else {
            ct++;
        }
    }

    return num_remove;
}

int decide_status_donut(PtwID* q, double current_err, Pt3D* m, Pt3D* qa, double r_donut, Pt3D* ten_c_est, int* excl_qab,
    list<int>& range_a, list<int>& range_b, list<int>& range_c,
    const list<int>::iterator& it_a, const list<int>::iterator& it_b, const list<int>::iterator& it_c) {

    unordered_set<int> set_a;
    get_potential_unordered_set(q->pt, qc.s_db.ann_min, current_err, set_a);
    int num_remove_a = filter_remaining(range_a, it_a, set_a);
    cout << "Remove a: " << num_remove_a << endl;
    if (set_a.find(*it_a) == set_a.end()) {
        return 1;
    }

    unordered_set<int> set_b;
    detm_donut_range_unordered_set(m, qa, r_donut, current_err, set_b);
    int num_remove_b = filter_remaining(range_b, it_b, set_b);
    cout << "Remove b: " << num_remove_b << endl;
    if (set_b.find(*it_b) == set_b.end()) {
        return 2;
    }

    unordered_set<int> set_c;
    nn_sphere_range_unordered_set(ten_c_est, 0.0, current_err, set_c, excl_qab, 3);
    int num_remove_c = filter_remaining(range_c, it_c, set_c);
    cout << "Remove c: " << num_remove_c << endl;
    if (set_c.find(*it_c) == set_c.end()) {
        return 3;
    }

    return 0;
}

int run_donut(PtwID* q, Exec_stat& stat) {

    Top_k_list tk_list;
    tk_list.init(qc.k, qc.delta);

    double current_delta = qc.delta;
    double current_err = qc.epsilon * 2.0;
    current_err *= qc.cheat_ratio;

    list<int> range_a, range_b, range_c;
    PtwID ten_a, ten_b, ten_c;

    range_a.clear();
    get_potential_list(q->pt, qc.s_db.ann_min, current_err, range_a);
    cout << "range_a size: " << range_a.size() << endl;

    bool skip_a = false, skip_b = false;

    for (auto itr_a = range_a.begin(); itr_a != range_a.end(); itr_a++) {

        auto it_a = *itr_a;

        if (it_a < 0) continue;

        ten_a.set(it_a, &qc.mesh_q);
        auto qa = *(q->pt) - *(ten_a.pt);

        Pt3D m;
        middle_pt(q->pt, ten_a.pt, m);
        double d_pm = eucl_dist(q->pt, &m);
        double r_donut = d_pm * 1.73205080757;

        range_b.clear();
        detm_donut_range_list(&m, &qa, r_donut, current_err, range_b);

        for (auto itr_b = range_b.begin(); itr_b != range_b.end(); itr_b++) {

            auto it_b = *itr_b;

            if (it_b < 0) continue;

            ten_b.set(it_b, &qc.mesh_q);

            Pt3D ten_b_est, ten_c_est;
            auto got_b_c = get_est_b_c(&m, ten_a.pt, ten_b.pt, ten_b_est, ten_c_est);
            if (!got_b_c) continue;

            int excl_qab[3] = { q->id, ten_a.id, ten_b.id };
            range_c.clear();
            nn_sphere_range_list(&ten_c_est, 0.0, current_err, range_c, excl_qab, 3);

            skip_a = false;

            for (auto itr_c = range_c.begin(); itr_c != range_c.end(); itr_c++) {

                auto it_c = *itr_c;

                if (it_c < 0) continue;

                ten_c.set(it_c, &qc.mesh_q);

                Entry* q_entry = new Entry(q, &ten_a, &ten_b, &ten_c);
                q_entry->fill_sides();

                #ifdef _CLR
                    q_entry->set_color(qc.color_q[q_entry->repre->id].clr);
                #endif

                vector<int> rr_ret;
                #ifdef _CLR
                    int rr_num = window_query(&qc.tree, q_entry, current_err, qc.color_err, rr_ret);
                #else
                    int rr_num = window_query(&qc.tree, q_entry, current_err, rr_ret);
                #endif

                skip_b = false;

                for (auto &hit: rr_ret) {

                    auto db_entry = qc.s_db.get_entry(hit);

                    if (!recheck_congr(q_entry, db_entry, current_err)) {
                        continue;
                    }

                    auto pair = new Entry_Pair(q_entry, db_entry, qc.s_db.get_grid_id_by_global_cell_id(hit));
                    double updated_delta = complete_transformation(pair, current_delta);
                    stat.veri_size++;

                    if (updated_delta >= 0) {
                        tk_list.insert(updated_delta);
                        current_delta = tk_list.max_dist();
                        current_err = sqrt(current_delta) * 2.0;
                        current_err *= qc.cheat_ratio;
                        cout << "Current delta updated to " << current_delta << endl;
                        cout << "Current epsilon updated to " << sqrt(current_delta) << endl;

                        int ret_status = decide_status_donut(q, current_err, &m, &qa, r_donut, &ten_c_est, excl_qab, range_a, range_b, range_c, itr_a, itr_b, itr_c);
                        cout << "ret_status: " << ret_status << endl;

                        if (ret_status == 1) {
                            skip_a = true;
                            skip_b = true;
                            break;
                        } else if (ret_status == 2) {
                            skip_a = false;
                            skip_b = true;
                            break;
                        } else if (ret_status == 3) {
                            skip_a = false;
                            skip_b = false;
                            break;
                        } else if (ret_status == 0) {
                            skip_a = false;
                            skip_b = false;
                            continue;
                        }
                    }
                }

                if (skip_b) {
                    break;
                }

            }

            if (skip_a) {
                break;
            }
        }
    }
}

int run_3nn(PtwID* q, Exec_stat& stat) {

    Top_k_list tk_list;
    tk_list.init(qc.k, qc.delta);

    double current_delta = qc.delta;
    double current_err = qc.epsilon * 2.0;
    current_err *= qc.cheat_ratio;

    vector<int> range;
    get_potential_3nn_set(q->pt, qc.s_db.ann_min, current_err, range);

    int range_size = range.size();
    if (range.size() < 3) {
        return 0;
    }

    vector<PtwID> range_pts;
    for (int &id: range) {
        range_pts.emplace_back(id, &qc.mesh_q);
    }

    unordered_set<int> current_range_set;
    for (auto &v: range) {
        current_range_set.insert(v);
    }

    bool skip_j = false, skip_k = false;

    // loop for all the 3-combinations
    for (int i = 0; i < range_size; i++) {
        if (current_range_set.find(i) == current_range_set.end()) {
            continue;
        }

        for (int j = 0; j < range_size; j++) {
            if (current_range_set.find(j) == current_range_set.end()) {
                continue;
            }

            skip_j = false;

            for (int k = 0; k < range_size; k++) {
                if (current_range_set.find(k) == current_range_set.end()) {
                    continue;
                }

                if (i != j && i != k && j != k) {
                    Entry* q_entry = new Entry(q, &range_pts[i], &range_pts[j], &range_pts[k]);
                    q_entry->fill_sides();

                    #ifdef _CLR
                        q_entry->set_color(qc.color_q[q_entry->repre->id].clr);
                    #endif

                    vector<int> rr_ret;
                    #ifdef _CLR
                        int rr_num = window_query(&qc.tree, q_entry, current_err, qc.color_err, rr_ret);
                    #else
                        int rr_num = window_query(&qc.tree, q_entry, current_err, rr_ret);
                    #endif

                    skip_k = false;

                    for (auto &hit: rr_ret) {

                        auto db_entry = qc.s_db.get_entry(hit);

                        if (!recheck_congr(q_entry, db_entry, current_err)) {
                            continue;
                        }

                        auto pair = new Entry_Pair(q_entry, db_entry, qc.s_db.get_grid_id_by_global_cell_id(hit));
                        double updated_delta = complete_transformation(pair, current_delta);
                        stat.veri_size++;

                        if (updated_delta >= 0) {
                            tk_list.insert(updated_delta);
                            current_delta = tk_list.max_dist();
                            current_err = sqrt(current_delta) * 2.0;
                            current_err *= qc.cheat_ratio;
                            cout << "Current delta updated to " << current_delta << endl;
                            cout << "Current epsilon updated to " << sqrt(current_delta) << endl;

                            current_range_set.clear();
                            get_potential_3nn_unordered_set(q->pt, qc.s_db.ann_min, current_err, current_range_set);

                            if (current_range_set.find(i) == current_range_set.end()) {
                                skip_j = true;
                                skip_k = true;
                                break;
                            } else if (current_range_set.find(j) == current_range_set.end()) {
                                skip_j = false;
                                skip_k = true;
                                break;
                            } else if (current_range_set.find(k) == current_range_set.end()) {
                                skip_j = false;
                                skip_k = false;
                                break;
                            } else {
                                skip_j = false;
                                skip_k = false;
                                continue;
                            }
                        }
                    }

                    if (skip_k) {
                        break;
                    }
                }
            }

            if (skip_j) {
                break;
            }
        }
    }
}

void exec(Exec_stat& stat) {

    stat = (const struct Exec_stat) { 0 };

    timer_start();

    PtwID q;

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
    if (qc.is_small) {
        if (qc.algorithm == "3nn-adv" && qc.is_simp) {
            sel_ratio = 0.5;
        } else {
            sel_ratio = 0.8;
        }
    } else {
        sel_ratio = 0.5;
    }
    // cout << "sel_ratio: " << sel_ratio << endl;
    while (eucl_dist(q.pt, &q_mbs_center) > sel_ratio * (qc.mesh_q.get_bsphere_r() - qc.s_db.ann_min)) { // set to 0.5 such that the largest dataset obtain the best performance
        // cout << "Boundary! reselect." << endl;
        selected_pt_id = rand() % qc.mesh_q.size();
        q.set(selected_pt_id, &qc.mesh_q);
    }

    if (qc.algorithm == "3nn-adv") {
        run_3nn(&q, stat);
    } else if (qc.algorithm == "donut-adv") {
        run_donut(&q, stat);
    }

    stat.user_time = timer_end(SECOND);
}

int main(int argc, char **argv) {

    qc.read_param(argc, argv);

    cout << "delta: " << qc.delta << endl;
    cout << "epsilon: " << qc.epsilon << endl;
    cout << "ct_ratio: " << qc.cheat_ratio << endl;
    cout << endl;

    srand(time(NULL));

    timer_start();

    if(!qc.load()) {
        exit(1);
    }

    auto exec_i_time = timer_end(SECOND);
    cout << "Total I/- time: " << exec_i_time << "(s)" << endl;

    cout << endl;


    Exec_stat stats[qc.num_exe];
    Exec_stat tmp_stat;
    int real_exec_times = 0;

    for (int exec_i = 0; exec_i < qc.num_exe; exec_i++) {

        cout << "Execution #" << (exec_i + 1) << ": " << endl;
        exec(stats[exec_i]);
        cout << endl;

        #if VERBOSE >= 0
            print_stat(stats[exec_i]);
            cout << endl;
        #endif

        real_exec_times++;

        if (real_exec_times <= 5 || real_exec_times == 10 || real_exec_times == 20) {
        // if (real_exec_times == 10 || real_exec_times == 20) {
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
