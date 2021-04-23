#include "point.h"
#include "trans.h"
#include "util.h"
#include "c_rtree.h"
#include "RTree.h"
#include "struct_q.h"
#include "goicp.h"
#include "pcr_adv.h"
#include "pair_dist.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <set>
#include <unordered_map>

using namespace std;
using namespace trimesh;

struct S_color {
    double clr[3];
};
// #ifdef _GEN
//     // const double COLOR_THRESH = 0.015; // for superg4pcs dsr queries
//     const double COLOR_THRESH = 0.05;
// #else
//     // const double COLOR_THRESH = 0.02; // for super4pcs dsr queries
//     const double COLOR_THRESH = 0.05; // for super4pcs dsr queries
// #endif

class Query_Context {
public:
    int force_cell;
    unordered_set<int> force_pts;
    bool stop_once;
    int num_parts;
    bool dirty_obj_dataset_flag;

    string db_path;
    string grid_filename; // 0=no_index
    string query_filename;
    string question;
    double delta;
    int k;
    string require;
    string algorithm;
    double epsilon;
    double color_err;
    int num_exe;
    int is_small;
    double donut_rad;

    bool write_stat;
    string stat_filename;

    #ifdef _TK
        int k;
        double delta_0;
        double epsilon_0;
    #endif

    void read_param(int argc, char** argv) {

        if (argc < 14) {
            cerr << "Usage: " << argv[0]
                 << " database_path"
                 << " grid_filename"
                 << " query_filename"
                 << " question(delta_sim/top_k)"
                 << " delta"
                 << " k"
                 << " require(det/prob)"
                 << " algorithm"
                 << " epsilon"
                 << " color_err"
                 << " num_exe"
                 << " is_small"
                 << " donut_rad"
                 << " [-num_parts=...] [-stop_once] [-force_cell=...] [-force_pt=...]* [-stat=...]" << endl;
            exit(1);
        }

        dirty_obj_dataset_flag = false;
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
            else if (argv_str.rfind("-num_parts", 0) == 0) {
                num_parts = atoi(argv[i] + 11);
                dirty_obj_dataset_flag = true;
            }
            else if (argv_str.rfind("-stat", 0) == 0) {
                write_stat = true;
                stat_filename = string(argv[i] + 6);
            }
        }

        int argi = 0;

        db_path = argv[(++argi)];
        grid_filename = argv[(++argi)];
        query_filename = argv[(++argi)];

        question = argv[(++argi)];
        delta = atof(argv[(++argi)]);
        k = atoi(argv[(++argi)]);
        require = argv[(++argi)];
        algorithm = argv[(++argi)];
        epsilon = atof(argv[(++argi)]);
        color_err = atof(argv[(++argi)]);
        num_exe = atoi(argv[(++argi)]);
        is_small = atoi(argv[(++argi)]);
        donut_rad = atof(argv[(++argi)]);
    }

    DB_Meshes db_meshes;
    int num_meshes;
    vector<C_RTree*> db_rtrees;
    Pair_Dist_Hash pd_hash;
    Mesh mesh_q;
    C_RTree r_q;
    // Struct_Q s_q;
    #ifdef _CLR
        vector<S_color> color_q;
        S_color* bq_clr[4];
    #endif

    // GoICP** goicp;
        
    vector<ICP3D<float>*> m_icps;
    vector<float*> m_data_icps;
    float* m_query_icp;
    
    // GoICP goicp[3];

    bool load() {

        cout << "Reading database files from " << db_path << endl;
        num_meshes = db_meshes.read_from_path(db_path);
        if (num_meshes < 0) {
            cerr << "Error reading database files" << endl;
            exit(1);
        }
        cout << "Total no. meshes: " << num_meshes << endl << endl;

        cout << "Building KDtrees for DB meshes" << endl;
        db_meshes.build_kd();

        cout << "Reading database R-trees..." << endl;
        if (num_parts > 0) {
            const int num_meshes_per_part = num_meshes / num_parts;
            for (int i = 0; i < num_parts; i++) {
                read_rtrees_comb(db_path, i, num_meshes_per_part, db_rtrees);
            }
        } else {
            read_rtrees_from_db_meshes(&db_meshes, db_rtrees);
        }

        if (grid_filename != "0") {
            cout << "Loading PD Hash..." << endl;
            pd_hash.read_bin(grid_filename);
        }

        cout << endl;

        cout << "Reading query mesh from " << query_filename << endl;
        mesh_q.read_from_path(query_filename);
        cout << endl;

        // load the query R-tree
        cout << "Reading query R-tree" << endl;
        r_q.read_from_mesh(query_filename);

        // // load the query structure
        // Struct_Q s_q;
        // if (!s_q.read(query_filename + ".info")) {
        //     cout << "Error loading query structure" << endl;
        //     exit(1);
        // }

        #ifdef _CLR
            // load the query colors
            cout << "Reading query color file" << endl;
            ifstream ifs_color_q(query_filename + ".color");
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

        double q_diam = mesh_q.get_bsphere_d();
        delta *= (q_diam * (double) mesh_q.size());
        cout << "Diameter of query mesh: " << q_diam << ", thus delta is finally set to delta * " << q_diam << " * Q-Size(" << mesh_q.size() << ") = " << delta << endl;
        
        if (require == "det") {
            epsilon = sqrt(delta); // for deterministic queries, epsilon is set according to delta
            cout << "Epsilon is finally set to sqrt(delta) = " << epsilon << endl;
        }

        for (int i = 0; i < db_meshes.size(); i++) {
            auto new_icp = new ICP3D<float>();
            // new_icp->err_diff_def = err_diff;
            new_icp->err_diff_def = 0.1; //TODO
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

        // for (int i = 0; i < num_meshes; i++) {
        //  loadGoICP(&db_meshes, i, &mesh_q, delta, goicp[i]);
        //     loadGoICP(&db_meshes, i, &mesh_q, sse, delta, &goicp[i], verbose, use_dt, dt_size);

        //  goicp[i].Initialize();
        // }
        // cout << endl;

        return true;
    }
};

Query_Context qc;

S_color get_p_color(Pt3D* p, Mesh* mesh_p) {
    S_color p_color;
    p_color.clr[0] = (p->x - mesh_p->get_box_min(0)) / (mesh_p->get_box_max(0) - mesh_p->get_box_min(0));
    p_color.clr[1] = (p->y - mesh_p->get_box_min(1)) / (mesh_p->get_box_max(1) - mesh_p->get_box_min(1));
    p_color.clr[2] = (p->z - mesh_p->get_box_min(2)) / (mesh_p->get_box_max(2) - mesh_p->get_box_min(2));
    return p_color;
}
bool same_color(S_color* c1, S_color* c2) {
    for (int i = 0; i < 3; i++) {
        if (abs(c1->clr[i] - c2->clr[i]) > qc.color_err) {
            return false;
        }
    }
    return true;
}

struct Congr_base {
    int bp[4];
    int db_id;
    Trans xf;
    double init_dist; // SSE: to be compared to sq(delta)
    Congr_base(int bp1, int bp2, int bp3, int bp4, int db_id) {
        this->bp[0] = bp1;
        this->bp[1] = bp2;
        this->bp[2] = bp3;
        this->bp[3] = bp4;
        this->db_id = db_id;
    }
};

// threshold set:
//   super4pcs: sel-coplanar: equational distance between the 4th point and the face through the first 3 points
//   superg4pcs: sel-3D-coplanar: distance threshold of two equal line segments
//   super4pcs: match-base: angle threshold of bases
//   superg4pcs: match-base: angle threshold of bases
//   superg4pcs: an equational threshold
double THRESHOLD_LIST[5] = { 0.01, 10, 0.01, 0.01, 0.01 }; // dense obj & comp_7 & comp_6 and other comp, !!don't change this!!
// const double THRESHOLD_LIST[5] = { 0.01, 10, 0.01, 0.01, 0.1 }; // det values
// const double THRESHOLD_LIST[5] = { 0.01, 1, 0.1, 0.05, 0.2 }; // det values
// const double THRESHOLD_LIST[5] = { 0.01, 1, 0.01, 0.01, 0.1 }; // comp_5, dsr queries
void decide_thresh_set() {
    if (!qc.is_small) {
        THRESHOLD_LIST[1] = 1;
    }
    if (qc.dirty_obj_dataset_flag) {
        THRESHOLD_LIST[0] = 0.1;
    }
}

const int MAX_TRIAL = 50000;

struct Dist_pair {
    int p1, p2;
    double d;
};

bool comp_dist_pair(const Dist_pair& a, const Dist_pair& b) {
    return (a.d < b.d);
}

struct Extracted_pair_gen {
    int p1, p2;
    Pt3D e, f;
    Extracted_pair_gen(int i1, int i2, Mesh* mesh, double r12, double r34) {
        this->p1 = i1;
        this->p2 = i2;
        auto pt1 = *(mesh->get_pt(p1));
        auto pt2 = *(mesh->get_pt(p2));
        this->e = (pt2 - pt1) * r12 + pt1;
        this->f = (pt2 - pt1) * r34 + pt1;
    }
};

struct Extracted_pair {
    int p1, p2;
    Pt3D e;
    Extracted_pair(int i1, int i2, Mesh* mesh, double r) {
        this->p1 = i1;
        this->p2 = i2;
        auto pt1 = *(mesh->get_pt(p1));
        auto pt2 = *(mesh->get_pt(p2));
        e = (pt2 - pt1) * r + pt1;
    }
};

bool solver(double a[3], double b[3], double& x, double &y) {
    double div = a[0] * b[1] - a[1] * b[0];
    x = (a[1] * b[2] - a[2] * b[1]) / div;
    y = (b[0] * a[2] - b[2] * a[0]) / div;
    return true;
}

bool cal_3d_intersection(int tab[4], double& l, double& la, double& mu, double& angle) {
    auto q1 = *(qc.mesh_q.get_pt(tab[0]));
    auto q2 = *(qc.mesh_q.get_pt(tab[1]));
    auto q3 = *(qc.mesh_q.get_pt(tab[2]));
    auto q4 = *(qc.mesh_q.get_pt(tab[3]));
    auto q12 = q2 - q1;
    auto q34 = q4 - q3;

    double a, b;
    double q12_arr[3] = { q12.x, q12.y, q12.z }, q34_arr[3] = { q34.x, q34.y, q34.z };
    solver(q12_arr, q34_arr, a, b);
    // cout << a << endl << b << endl;

    double coef_1[3] = { a * q12.z - q12.x, q34.x - a * q34.z, q3.x - q1.x + q1.z - q3.z };
    double coef_2[3] = { b * q12.z - q12.y, q34.y - b * q34.z, q3.y - q1.y + q1.z - q3.z };
    double la_d, mu_d;
    solver(coef_1, coef_2, la_d, mu_d);
    // cout << la << endl << mu << endl;

    if (la_d < 0 || la_d > 1 || mu_d < 0 || mu_d > 1) {
        return false;
    }

    la = (double) la_d;
    mu = (double) mu_d;

    auto e = q1 + q12 * la;
    auto f = q3 + q34 * mu;

    l = eucl_dist(&e, &f);

    auto eq1 = q1 - e;
    auto fq3 = q3 - f;
    angle = dot_prd(&eq1, &fq3) / (eq1.mode() * fq3.mode());

    return true;
}

bool cal_3d_intersection_new(int tab[4], double& l, double& la, double& mu, double& angle, bool silent = true) {

    if (!silent) {
        // cout << "params of cal_3d_intersection_new: " << tab[0] << " " << tab[1] << " " << tab[2] << " " << tab[3] << endl;
    }
    
    auto q1 = *(qc.mesh_q.get_pt(tab[0]));
    auto q2 = *(qc.mesh_q.get_pt(tab[1]));
    auto q3 = *(qc.mesh_q.get_pt(tab[2]));
    auto q4 = *(qc.mesh_q.get_pt(tab[3]));
    auto q12 = q2 - q1;
    auto q34 = q4 - q3;

    Pt3D n;  cross_prd(&q12, &q34, n);
    Pt3D n1; cross_prd(&q12, &n, n1);
    Pt3D n2; cross_prd(&q34, &n, n2);
    auto q1_q3 = q3 - q1;
    auto q3_q1 = q1 - q3;
    la = dot_prd(&q1_q3, &n2) / dot_prd(&q12, &n2);
    mu = dot_prd(&q3_q1, &n1) / dot_prd(&q34, &n1);

    if (la < 0 || la > 1 || mu < 0 || mu > 1) {
        return false;
    }

    auto e = q1 + q12 * la;
    auto f = q3 + q34 * mu;

    l = eucl_dist(&e, &f);

    auto eq1 = q1 - e;
    auto fq3 = q3 - f;
    angle = dot_prd(&eq1, &fq3) / (eq1.mode() * fq3.mode());

    return true;
}

template<class T>
bool check_dup(T* val, int n) {
    set<T> s;
    for (int i = 0; i < n; i++) {
        if(s.insert(val[i]).second == false) {
            return true;
        }
    }
    return false;
}

bool test_3d_coplanar(int* ran, double& d, double& l, double& la, double& mu, double& angle) {

    bool found_coplanar = false;

    int m = qc.mesh_q.size();

    if (m <= 400) {
        bool found = false;
        vector<Dist_pair> pairwise_dists;
        for (int i = 0; i < m; i++) {
            for (int j = i + 1; j < m; j++) {
                auto q_i = qc.mesh_q.get_pt(i), q_j = qc.mesh_q.get_pt(j);
                Dist_pair dp { i, j, eucl_dist(q_i, q_j) };
                pairwise_dists.push_back(dp);
            }
        }
        sort(begin(pairwise_dists), begin(pairwise_dists) + pairwise_dists.size(), [] (Dist_pair a, Dist_pair b) { return comp_dist_pair(a, b); });
        for (int i = 0; i < pairwise_dists.size() - 1; i++) {
            if (abs(pairwise_dists[i + 1].d - pairwise_dists[i].d) <= THRESHOLD_LIST[1]) {
                if (rand() % 3 != 0) {
                    continue;
                }
                int tab[4] = {
                    pairwise_dists[i].p1,
                    pairwise_dists[i].p2,
                    pairwise_dists[i + 1].p1,
                    pairwise_dists[i + 1].p2
                };
                if (check_dup(tab, 4)) {
                    continue;
                }
                // cout << tab[0] << "&" << tab[1] << "-" << tab[2] << "&" << tab[3] << ": " << pairwise_dists[i].d << endl;
                if (cal_3d_intersection_new(tab, l, la, mu, angle)) {
                // if (brufc_find_la_mu(qc.mesh_q, tab, l, la, mu, e, f, angle)) {
                    d = pairwise_dists[i].d;
                    for (int j = 0; j < 4; j++) {
                        ran[j] = tab[j];
                    }

                    found_coplanar = true;
                    break;
                }
            }
        }
    } else {
        for (int t = 0; t < 100; t++) {
            ran[0] = rand() % m;
            do {
                ran[1] = rand() % m;
            } while (ran[1] == ran[0]);

            auto bq1 = qc.mesh_q.get_pt(ran[0]);
            auto bq2 = qc.mesh_q.get_pt(ran[1]);

            d = eucl_dist(bq1, bq2);

            for (int j = 0; j < 100; j++) {
                do {
                    ran[2] = rand() % m;
                } while (ran[2] == ran[0] || ran[2] == ran[1]);

                auto bq3 = qc.mesh_q.get_pt(ran[2]);
                vector<int> bq4_cand_set;
                qc.r_q.range_sphere_dist_err(bq3, d, THRESHOLD_LIST[1], bq4_cand_set); // for object dataset
                if (bq4_cand_set.empty()) {
                    continue;
                }

                for (auto &i: bq4_cand_set) {
                    ran[3] = i;
                    if (cal_3d_intersection_new(ran, l, la, mu, angle)) {
                        found_coplanar = true;
                        break;
                    }
                }

                if (found_coplanar) {
                    break;
                }
            }
            if (found_coplanar) {
                break;
            }
        }
    }

    if (!found_coplanar) {
        cout << "3d coplanar not found" << endl;
        return false;
    }

    // ran[0] = 10;
    // ran[1] = 18;
    // ran[2] = 23;
    // ran[3] = 35;
    // d = eucl_dist(qc.mesh_q.get_pt(ran[0]), qc.mesh_q.get_pt(ran[1]));
    // cal_3d_intersection_new(ran, l, la, mu, angle, false);
    // cout << "superg4pcs rand points reset to " << ran[0] << " " << ran[1] << " " << ran[2] << " " << ran[3] << endl;

    // cout << "coplanar found: " << ran[0] << " " << ran[1] << " " << ran[2] << " " << ran[3] << endl;
    //      // << qc.s_q->id_q_to_str(ran[0]) << " " << qc.s_q->id_q_to_str(ran[1]) << " "
    //      // << qc.s_q->id_q_to_str(ran[2]) << " " << qc.s_q->id_q_to_str(ran[3]) << endl;
    // cout << "d: " << d << endl;
    // cout << "l: " << l << endl;
    // cout << "la: " << la << endl;
    // cout << "mu: " << mu << endl;
    // cout << "angle: " << angle << endl;

    return true;
}

bool cal_intersection(int ran1, int ran2, int ran3, int ran4, double& la, double& mu, double& angle) {

    auto q1 = *(qc.mesh_q.get_pt(ran1));
    auto q2 = *(qc.mesh_q.get_pt(ran2));
    auto q3 = *(qc.mesh_q.get_pt(ran3));
    auto q4 = *(qc.mesh_q.get_pt(ran4));
    mu = ((q3.x - q1.x) * (q2.y - q1.y) - (q3.y - q1.y) * (q2.x - q1.x))
       / ((q4.y - q3.y) * (q2.x - q1.x) - (q4.x - q3.x) * (q2.y - q1.y));

    // cout << mu << endl;
    if (mu < 0 || mu > 1) {
        return false;
    }

    auto e = q3 + (q4 - q3) * mu;

    la = (e.x - q1.x) / (q2.x - q1.x);

    // cout << la << endl;
    if (la < 0 || la > 1) {
        return false;
    }

    auto eq1 = q1 - e;
    auto eq3 = q3 - e;
    angle = (dot_prd(&eq1, &eq3)) / (eq1.mode() * eq3.mode());

    return true;
}

bool donut_select(int* ran, double& d1, double& d2, double& l, double& la, double& mu, double& angle) {

    bool found_coplanar = false;

    int query_size = qc.mesh_q.size();
    int t = 0;

    while (t < MAX_TRIAL && !found_coplanar) {

        t++;

        ran[0] = rand() % query_size;

        // ran[0] = 82; // TODO: remove
        // cout << "first point of donut set to " << ran[0] << endl;

        for (int i = 1; i < 3; i++) {
            ran[i] = -1;
        }

        PtwID p(ran[0], &qc.mesh_q);
        PtwID a, b, c;

        qc.r_q.nn_sphere(p.pt, qc.donut_rad, &ran[1]);

        if (ran[1] >= 0) {
            a.set(ran[1], &qc.mesh_q);
        } else {
            continue;
        }

        Pt3D m; middle_pt(p.pt, a.pt, m);
        auto pa = *p.pt - *a.pt;
        double d_pm = eucl_dist(p.pt, &m);
        double r_donut = d_pm * 1.73205080757;

        if (qc.is_small) {
            donut_nn_quick(&m, &pa, r_donut, &qc.mesh_q, &qc.r_q, &ran[2]);
        } else {
            donut_nn(&m, &pa, r_donut, &qc.mesh_q, &qc.r_q, &ran[2]);
        }

        if (ran[2] >= 0) {
            b.set(ran[2], &qc.mesh_q);
        } else {
            continue;
        }

        Pt3D b_est, c_est; // but b_est will not be used
        auto got_b_c = get_est_b_c(&m, a.pt, b.pt, b_est, c_est);
        if (!got_b_c) {
            continue;
        }

        qc.r_q.nn_sphere(&c_est, 0.0, &ran[3], { p.id, a.id, b.id });
        if (ran[3] >= 0 && ran[3] < query_size) {
            c.set(ran[3], &qc.mesh_q);
        } else {
            continue;
        }

        if (!cal_3d_intersection_new(ran, l, la, mu, angle)) {
            continue;
        }

        d1 = eucl_dist(p.pt, a.pt);
        d2 = eucl_dist(b.pt, c.pt);

        found_coplanar = true;
    }

    if (!found_coplanar) {
        cout << "3d coplanar not found" << endl;
        return false;
    }

    return true;
}

bool test_coplanar(int* ran, double& d12_q, double& d34_q, double& la, double& mu, double& angle) {

    Pt3D bq[4];
    int m = qc.mesh_q.size();
    int t = 0;
    bool found_coplanar = false;

    while (t < MAX_TRIAL && !found_coplanar) {
        // gen 3 different random indices in [0, m)
        ran[0] = rand() % m; ran[1] = 0; ran[2] = 0;
        do {
            ran[1] = rand() % m;
        } while (ran[1] == ran[0]);
        do {
            ran[2] = rand() % m;
        } while (ran[2] == ran[0] || ran[2] == ran[1]);

        // ran[0] = 430;
        // ran[1] = 191;
        // ran[2] = 322;
        // cout << "super4pcs rand points reset to " << ran[0] << ", " << ran[1] << ", " << ran[2] << endl;

        // get the first 3 base points
        for (int i = 0; i < 3; i++) {
            bq[i] = *(qc.mesh_q.get_pt(ran[i]));
        }
        
        // calculate the equation plane
        double a1 = bq[1].x - bq[0].x;
        double b1 = bq[1].y - bq[0].y;
        double c1 = bq[1].z - bq[0].z;
        double a2 = bq[2].x - bq[0].x;
        double b2 = bq[2].y - bq[0].y;
        double c2 = bq[2].z - bq[0].z;
        double a = b1 * c2 - b2 * c1;
        double b = a2 * c1 - a1 * c2;
        double c = a1 * b2 - b1 * a2;
        double d = (- a * bq[0].x - b * bq[0].y - c * bq[0].z);

        double div = sqrt(a * a + b * b + c * c);

        // look for the fourth point
        for (int i = 0; i < m; i++) {
            if (i == ran[0] || i == ran[1] || i == ran[2]) // skip the repeats
                continue;

            bq[3] = *(qc.mesh_q.get_pt(i));
            double dist = (a * bq[3].x + b * bq[3].y + c * bq[3].z + d) / div;

            if((abs(dist) <= THRESHOLD_LIST[0]) && cal_intersection(ran[0], ran[1], ran[2], i, la, mu, angle)) {
                found_coplanar = true;
                ran[3] = i;
                break;
            }
        }

        if (found_coplanar) {
        	break;
        }

        t++;
    }

    if (!found_coplanar) {
        cout << "coplanar not found" << endl;
        return false;
    }

    d12_q = eucl_dist(&bq[0], &bq[1]);
    d34_q = eucl_dist(&bq[2], &bq[3]);

    // cout << "coplanar found: "
    //      << s_q->id_q_to_str(ran[0]) << " " << s_q->id_q_to_str(ran[1]) << " "
    //      << s_q->id_q_to_str(ran[2]) << " " << s_q->id_q_to_str(ran[3]) << endl;
    // cout << "d12: " << d12_q << endl;
    // cout << "d34: " << d34_q << endl;
    // cout << "la: " << la << endl;
    // cout << "mu: " << mu << endl;
    // cout << "angle: " << angle << endl;

    return true;
}

void get_ep(Mesh* mesh_p, C_RTree* r_tree, double d, double ratio, vector<Extracted_pair>& ep_list) {

    int n = mesh_p->size();
    double err = max(0.01, 2 * qc.epsilon);
    vector<int> intersection_ret;

    for (int i = 0; i < n; i++) {

        auto p = mesh_p->get_pt(i);

        #ifdef _CLR
            auto p_color = get_p_color(p, mesh_p);
            if (!(same_color(&p_color, qc.bq_clr[0]) || same_color(&p_color, qc.bq_clr[1]) || same_color(&p_color, qc.bq_clr[2]) || same_color(&p_color, qc.bq_clr[3]))) {
                continue;
            }
        #endif

        intersection_ret.clear();
        r_tree->range_sphere_dist_err(p, d, err, intersection_ret);

        for (auto &r: intersection_ret) {
            ep_list.emplace_back(i, r, mesh_p, ratio);
        }
    }
}

int match_base_in_db_mesh_donut(Mesh* mesh_p, C_RTree* r_tree, double d1, double d2, double l, double la, double mu, double angle, int db_id, vector<Congr_base>& ret) {

    double err = max(2.0 * qc.epsilon, 0.001);

    vector<Extracted_pair> ep12;
    vector<Extracted_pair> ep34;
    get_ep(mesh_p, r_tree, d1, la, ep12);
    get_ep(mesh_p, r_tree, d2, mu, ep34);

    RTree<int, double, 3, double> f_tree;
    for (int i = 0; i < ep34.size(); i++) {
        double fbox_min[3] { ep34[i].e.x, ep34[i].e.y, ep34[i].e.z };
        double fbox_max[3] { ep34[i].e.x, ep34[i].e.y, ep34[i].e.z }; // since it is from ep34, then the "e" here actually is "f"
        f_tree.Insert(fbox_min, fbox_max, i);
    }

    // cout << "Num of matching 12: " << ep12.size() << endl;
    // cout << "Num of matching 34: " << ep34.size() << endl;

    vector<int> intersection_ret;

    double sq_dist_min = max(0.0, l - err);
    sq_dist_min = sq_dist_min * sq_dist_min;
    double sq_dist_max = l + err;
    sq_dist_max = sq_dist_max * sq_dist_max;

    int num_matched = 0;

    for (auto &s12: ep12) {

        intersection_ret.clear();
        double e_arr[3] { s12.e.x, s12.e.y, s12.e.z };
        f_tree.SphereSearch(e_arr, sq_dist_min, sq_dist_max, intersection_ret);

        if (!intersection_ret.empty()) {

            auto ep1 = *(mesh_p->get_pt(s12.p1)) - s12.e;
            double ep1_mode = ep1.mode();

            for (auto &i_f: intersection_ret) {

                auto s34 = ep34[i_f];

                auto ef = s34.e - s12.e; // ep34(s34) contains all the "f"s, i.e. f = s34.e
                auto fp3 = *(mesh_p->get_pt(s34.p1)) - s34.e;

                // cout << dot_prd(ep1, ef) << endl;

                if (abs(dot_prd(&ep1, &ef) / (ep1.mode() * ef.mode())) > THRESHOLD_LIST[4]) {
                    continue;
                }

                double cos_p1efp3 = (dot_prd(&ep1, &fp3)) / (ep1_mode * fp3.mode());
                // cout << cos_p1efp3 << endl;

                if (abs(cos_p1efp3 - angle) > THRESHOLD_LIST[3]) {
                    continue;
                }

                ret.emplace_back(s12.p1, s12.p2, s34.p1, s34.p2, db_id);
                num_matched++;
            }
        }
    }

    return num_matched;
}

int match_4pcs_gen(Mesh* mesh_p, vector<Extracted_pair_gen>& eps, RTree<int, double, 3, double>* f_tree, double l, double angle, int db_id, vector<Congr_base>& ret) {

    // cout << "eps size: " << eps.size() << endl;
    // cout << "f_tree size: " << f_tree->Count() << endl;

    double err = max(2.0 * qc.epsilon, 0.001);

    double sq_dist_min = max(0.0, l - err);
    sq_dist_min = sq_dist_min * sq_dist_min;
    double sq_dist_max = l + err;
    sq_dist_max = sq_dist_max * sq_dist_max;

    int num_matched = 0;
    vector<int> intersection_ret;

    for (auto &e_ep: eps) {

        intersection_ret.clear();
        double e_arr[3] { e_ep.e.x, e_ep.e.y, e_ep.e.z };
        f_tree->SphereSearch(e_arr, sq_dist_min, sq_dist_max, intersection_ret);

        if (!intersection_ret.empty()) {

            auto ep1 = *(mesh_p->get_pt(e_ep.p1)) - e_ep.e;
            double ep1_mode = ep1.mode();

            for (auto &i: intersection_ret) {

                auto ef = eps[i].f - e_ep.e;
                auto fp3 = *(mesh_p->get_pt(eps[i].p1)) - eps[i].f;

                // cout << dot_prd(ep1, ef) << endl;

                if (abs(dot_prd(&ep1, &ef) / (ep1.mode() * ef.mode())) > THRESHOLD_LIST[4]) {
                    continue;
                }

                double cos_p1efp3 = (dot_prd(&ep1, &fp3)) / (ep1_mode * fp3.mode());
                // cout << cos_p1efp3 << endl;

                if (abs(cos_p1efp3 - angle) > THRESHOLD_LIST[3]) {
                    continue;
                }

                ret.emplace_back(e_ep.p1, e_ep.p2, eps[i].p1, eps[i].p2, db_id);
                num_matched++;
            }
        }
    }

    return num_matched;
}

int match_base_in_db_mesh_gen_dp(double d, double l, double la, double mu, double angle, vector<Congr_base>& ret) {

    double err = max(2.0 * qc.epsilon, 0.001);

    vector<Pair_Dist*> pd_ret;
    qc.pd_hash.query(max(0.0, d - err), d + err, pd_ret);

    cout << "Total number of matched pairs: " << pd_ret.size() << endl;

    unordered_map<int, vector<Extracted_pair_gen>> eps_map;
    for (auto &pd: pd_ret) {
        if (eps_map.find(pd->m_db_id) == eps_map.end()) {
            vector<Extracted_pair_gen> new_eps;
            eps_map[pd->m_db_id] = new_eps;
        }
        eps_map[pd->m_db_id].emplace_back(pd->m_ep1, pd->m_ep2, qc.db_meshes.get_mesh(pd->m_db_id), la, mu);
        eps_map[pd->m_db_id].emplace_back(pd->m_ep2, pd->m_ep1, qc.db_meshes.get_mesh(pd->m_db_id), la, mu);
    }

    double sq_dist_min = max(0.0, l - err);
    sq_dist_min = sq_dist_min * sq_dist_min;
    double sq_dist_max = l + err;
    sq_dist_max = sq_dist_max * sq_dist_max;

    int num_matched = 0;

    for (auto &v: eps_map) {
        auto db_id = v.first;
        auto& eps = v.second;
        auto mesh_p = qc.db_meshes.get_mesh(db_id);
        auto r_tree = qc.db_rtrees[db_id];
        int index = 0;

        RTree<int, double, 3, double> f_tree;
        for (auto &ep: eps) {
            double fbox_min[3] { ep.f.x, ep.f.y, ep.f.z };
            double fbox_max[3] { ep.f.x, ep.f.y, ep.f.z };
            f_tree.Insert(fbox_min, fbox_max, index);
            index++;
        }

        // cout << "Process DB #" << db_id << endl;
        int num_matched_ind = match_4pcs_gen(mesh_p, eps, &f_tree, l, angle, db_id, ret);
        // cout << "Matched with: " << num_matched_ind << endl;

        num_matched += num_matched_ind;
    }

    return num_matched;
}

int match_base_in_db_mesh_gen(Mesh* mesh_p, C_RTree* r_tree, double d, double l, double la, double mu, double angle, int db_id, vector<Congr_base>& ret) {

    double err = max(2.0 * qc.epsilon, 0.001);

    vector<int> intersection_ret;
    vector<Extracted_pair_gen> eps;
    int index = 0;
    int n = mesh_p->size();

    RTree<int, double, 3, double> f_tree;

    for (int i = 0; i < n; i++) {

        auto p = mesh_p->get_pt(i);

        #ifdef _CLR
            auto p_color = get_p_color(p, mesh_p);
            if (!(same_color(&p_color, qc.bq_clr[0]) || same_color(&p_color, qc.bq_clr[1]) || same_color(&p_color, qc.bq_clr[2]) || same_color(&p_color, qc.bq_clr[3]))) {
                continue;
            }
        #endif

        intersection_ret.clear();
        r_tree->range_sphere_dist_err(p, d, err, intersection_ret);

        // cout << intersection_ret.size() << endl;

        for (auto &r: intersection_ret) {
            eps.emplace_back(i, r, mesh_p, la, mu);
            Extracted_pair_gen& ep = eps[eps.size() - 1];

            double fbox_min[3] { ep.f.x, ep.f.y, ep.f.z };
            double fbox_max[3] { ep.f.x, ep.f.y, ep.f.z };
            f_tree.Insert(fbox_min, fbox_max, index);

            index++;
        }
    }

    // cout << "Num of matching d: " << index << endl;

    return match_4pcs_gen(mesh_p, eps, &f_tree, l, angle, db_id, ret);
}

int match_4pcs(Mesh* mesh_p, C_RTree* r_tree, vector<Extracted_pair>& ep12, unordered_map<int, vector<Extracted_pair*>>& ep34_map, double d34_q, double mu, double angle, int db_id, vector<Congr_base>& ret) {

    double err = max(2.0 * qc.epsilon, 0.001);

    double l34 = d34_q * mu;

    vector<int> intersection_ret;
    int num_matched = 0;

    for (auto &s12: ep12) {

        auto e12 = s12.e;
        auto p1 = mesh_p->get_pt(s12.p1);

        intersection_ret.clear();
        r_tree->range_sphere_dist_err(&e12, l34, err, intersection_ret);

        for (auto &i: intersection_ret) {

            auto iPt = mesh_p->get_pt(i);
            auto e12_iPt = *iPt - e12;
            auto e12_p1 = *p1 - e12;
            double cosIPt = (dot_prd(&e12_iPt, &e12_p1)) / (e12_iPt.mode() * e12_p1.mode());

            if (abs(cosIPt - angle) > THRESHOLD_LIST[2]) {
                continue;
            }

            auto got = ep34_map.find(i);
            if (got == ep34_map.end()) {
                // not found
                continue;
            }

            for (auto &s34: got->second) {
                double dist = eucl_dist(&e12, &s34->e);

                if (dist <= err) {
                    ret.emplace_back(s12.p1, s12.p2, s34->p1, s34->p2, db_id);
                    num_matched++;
                }
            }
        }
    }

    return num_matched;
}

int match_base_in_db_mesh_dp(double d12_q, double d34_q, double la, double mu, double angle, vector<Congr_base>& ret) {

    double err = max(2.0 * qc.epsilon, 0.001);

    vector<Pair_Dist*> pd_ret12;
    qc.pd_hash.query(max(0.0, d12_q - err), d12_q + err, pd_ret12);
    vector<Pair_Dist*> pd_ret34;
    qc.pd_hash.query(max(0.0, d34_q - err), d34_q + err, pd_ret34);

    cout << "Total number of matched pairs-12: " << pd_ret12.size() << endl;
    cout << "Total number of matched pairs-34: " << pd_ret34.size() << endl;

    // for (auto &v: pd_ret12) {
    //     cout << v->m_ep1 << " " << v->m_ep2 << " " << v->m_db_id << " " << v->m_dist << endl;
    // }
    // for (auto &v: pd_ret34) {
    //     cout << v->m_ep1 << " " << v->m_ep2 << " " << v->m_db_id << " " << v->m_dist << endl;
    // }

    unordered_map<int, vector<Extracted_pair>> ep12_map;
    unordered_map<int, vector<Extracted_pair>> ep34_map;
    for (auto &pd: pd_ret12) {
        if (ep12_map.find(pd->m_db_id) == ep12_map.end()) {
            vector<Extracted_pair> new_eps;
            ep12_map[pd->m_db_id] = new_eps;
        }
        ep12_map[pd->m_db_id].emplace_back(pd->m_ep1, pd->m_ep2, qc.db_meshes.get_mesh(pd->m_db_id), la);
        ep12_map[pd->m_db_id].emplace_back(pd->m_ep2, pd->m_ep1, qc.db_meshes.get_mesh(pd->m_db_id), la);
    }
    for (auto &pd: pd_ret34) {
        if (ep34_map.find(pd->m_db_id) == ep34_map.end()) {
            vector<Extracted_pair> new_eps;
            ep34_map[pd->m_db_id] = new_eps;
        }
        ep34_map[pd->m_db_id].emplace_back(pd->m_ep1, pd->m_ep2, qc.db_meshes.get_mesh(pd->m_db_id), mu);
        ep34_map[pd->m_db_id].emplace_back(pd->m_ep2, pd->m_ep1, qc.db_meshes.get_mesh(pd->m_db_id), mu);
    }

    int num_matched;

    for (auto &v: ep12_map) {
        int db_id = v.first;
        if (ep34_map.find(db_id) == ep34_map.end()) {
            continue;
        }
        auto ep12 = v.second;
        auto& ep34 = ep34_map[db_id];
        auto mesh_p = qc.db_meshes.get_mesh(db_id);
        auto r_tree = qc.db_rtrees[db_id];

        unordered_map<int, vector<Extracted_pair*>> ep34_by_p1_map;
        for (int i = 0; i < ep34.size(); i++) {
            if (ep34_by_p1_map.find(ep34[i].p1) == ep34_by_p1_map.end()) {
                vector<Extracted_pair*> new_list;
                ep34_by_p1_map[ep34[i].p1] = new_list;
            }
            ep34_by_p1_map[ep34[i].p1].push_back(&ep34[i]);
        }

        // cout << "Process DB #" << db_id << endl;
        int num_matched_ind = match_4pcs(mesh_p, r_tree, ep12, ep34_by_p1_map, d34_q, mu, angle, db_id, ret);
        // cout << "Matched with: " << num_matched_ind << endl;

        num_matched += num_matched_ind;
    }

    return num_matched;
}

int match_base_in_db_mesh(Mesh* mesh_p, C_RTree* r_tree, double d12_q, double d34_q, double la, double mu, double angle, int db_id, vector<Congr_base>& ret) {

    double err = max(2.0 * qc.epsilon, 0.001);

    vector<Extracted_pair> ep12;
    vector<Extracted_pair> ep34;
    get_ep(mesh_p, r_tree, d12_q, la, ep12);
    get_ep(mesh_p, r_tree, d34_q, mu, ep34);

    unordered_map<int, vector<Extracted_pair*>> ep34_map;
    for (int i = 0; i < ep34.size(); i++) {
        if (ep34_map.find(ep34[i].p1) == ep34_map.end()) {
            vector<Extracted_pair*> new_list;
            ep34_map[ep34[i].p1] = new_list;
        }
        ep34_map[ep34[i].p1].push_back(&ep34[i]);
    }

    // cout << "Num of matching 12: " << ep12.size() << endl;
    // cout << "Num of matching 34: " << ep34.size() << endl;

    return match_4pcs(mesh_p, r_tree, ep12, ep34_map, d34_q, mu, angle, db_id, ret);
}

// bool iter(DB_Meshes* db_meshes, vector<C_RTree*>& db_rtrees, Mesh* mesh_q, Struct_Q* s_q, C_RTree* query_rtree, double delta, double epsilon,
        // GoICP goicp[], vector<ICP3D<float>*>& m_icps, float* m_query_icp, Exec_stat& stat) {
bool iter(Exec_stat& stat) {

    timer_start();

    int ran[4];
    double la, mu;
    // Pt3D e;
    double angle; // cos(theta)
    double d12_q, d34_q; // for super4pcs
    double l, d;
    double d1, d2; // for donut-basic
    // Pt3D f;

    vector<Congr_base> ret;

    double iter_prop_time = 0.0, iter_veri_time = 0.0;
    bool iter_found_one = false;
    double iter_first_time = 0.0;

    vector<Congr_base*> ret_left_icp_only, ret_left_goicp;

    int iter_num_verified = 0;

    bool found_coplanar;
    if (qc.algorithm == "donut-basic") {
        found_coplanar = donut_select(ran, d1, d2, l, la, mu, angle);
    } else if (qc.algorithm == "superg4pcs-adv" || qc.algorithm == "superg4pcs-basic") {
        found_coplanar = test_3d_coplanar(ran, d, l, la, mu, angle);
    } else {
        found_coplanar = test_coplanar(ran, d12_q, d34_q, la, mu, angle);
    }

    if (!found_coplanar) {
        timer_end(SECOND);
        return false;
    }

    cout << ran[0] << " " << ran[1] << " " << ran[2] << " " << ran[3] << endl;

    Pt3D* bq_ptr[4];
    for (int i = 0; i < 4; i++) {
        bq_ptr[i] = qc.mesh_q.get_pt(ran[i]);
    }
    #ifdef _CLR
        for (int i = 0; i < 4; i++) {
            qc.bq_clr[i] = &qc.color_q[ran[i]];
        }
    #endif

    if (qc.algorithm == "super4pcs-adv") {
        match_base_in_db_mesh_dp(d12_q, d34_q, la, mu, angle, ret);
    } else if (qc.algorithm == "superg4pcs-adv") {
        match_base_in_db_mesh_gen_dp(d, l, la, mu, angle, ret);
    } else {
        for (int i = 0; i < qc.db_meshes.size(); i++) {
            // cout << "Match base in db#" << i << endl;
            int ret_size;
            if (qc.algorithm == "donut-basic") {
                ret_size = match_base_in_db_mesh_donut(qc.db_meshes.get_mesh(i), qc.db_rtrees[i], d1, d2, l, la, mu, angle, i, ret);
            } else if (qc.algorithm == "superg4pcs-basic") {
                ret_size = match_base_in_db_mesh_gen(qc.db_meshes.get_mesh(i), qc.db_rtrees[i], d, l, la, mu, angle, i, ret);
            } else if (qc.algorithm == "super4pcs-basic") {
                ret_size = match_base_in_db_mesh(qc.db_meshes.get_mesh(i), qc.db_rtrees[i], d12_q, d34_q, la, mu, angle, i, ret);
            }
            // cout << "Returned matched: " << ret_size << endl;
        }
    }

    iter_prop_time = timer_end(SECOND);

    timer_start();
    timer_start();

    cout << ret.size() << endl;

    // Step 1: calculate transformation, initial distance, and leave those for step 2&3
    for (auto &r: ret) {
        Pt3D* bp_ptr[4];
        for (int j = 0; j < 4; j++) {
            bp_ptr[j] = qc.db_meshes.get_mesh(r.db_id)->get_pt(r.bp[j]);
        }

        // if (r.bp[0] == 8188 && r.bp[1] == 8261 && r.bp[2] == 8159 && r.bp[3] == 8362) {
        //     cout << "dedicated check" << endl;
        // } else {
        //     continue;
        // }

        // #ifdef _CLR
        //     bool color_match = true;
        //     for (int j = 0; j < 4; j++) {
        //         auto p_color = get_p_color(bp_ptr[j], db_meshes->get_mesh(r.db_id));
        //         if (!same_color(&p_color, qc.bq_clr[j])) {
        //             color_match = false;
        //             break;
        //         }
        //     }
        //     if (!color_match) {
        //         continue;
        //     }
        // #endif

        // cout << "checking" << endl;
        
        cal_trans(bq_ptr, bp_ptr, 4, r.xf);
        // r.init_dist = db_meshes->cal_corr_err(&qc.mesh_q, r.db_id, &r.xf); // TODO: use another unimplemented interface
        r.init_dist = qc.db_meshes.cal_corr_err(&qc.mesh_q, r.db_id, &r.xf, qc.delta); // TODO: simplified method, stops when error is larger
        // cout << "Initial distance: " << r.init_dist << endl;

        // if (r.init_dist <= sq(delta)) {
        if (r.init_dist >= 0) {
            cout << "Dist: " << r.init_dist << "(MSE=" << (r.init_dist / (double) qc.mesh_q.size() / qc.mesh_q.get_bsphere_d()) << ")" << endl;
            cout << "Matched with DB #" << r.db_id << ": " << qc.db_meshes.get_mesh(r.db_id)->get_filename() << endl;
            iter_num_verified++;
            if (iter_num_verified == 1 && !iter_found_one) {
                iter_first_time = timer_end(SECOND);
                iter_found_one = true;
            }
        } else {
            ret_left_icp_only.push_back(&r);
        }
    }

    // // Step 2: perform the ICP-only check
    // if (!iter_found_one) {
	   //  for (auto &r: ret_left_icp_only) {
	   //      double init_dist = db_meshes->cal_corr_err(&qc.mesh_q, r->db_id, &r->xf);
	   //      // cout << "Initial distance: " << init_dist << endl;
	   //      double r_array[9] {
	   //          r->xf.r11, r->xf.r12, r->xf.r13,
	   //          r->xf.r21, r->xf.r22, r->xf.r23,
	   //          r->xf.r31, r->xf.r32, r->xf.r33
	   //      };
	   //      ICP_Matrix R_icp(3, 3, r_array);
	   //      double t_array[3] {
	   //          r->xf.tx, r->xf.ty, r->xf.tz
	   //      };
	   //      ICP_Matrix t_icp(3, 1, t_array);
	   //      double updated_err = m_icps[r->db_id]->Run(m_query_icp, mesh_q->size(), R_icp, t_icp);
	   //      // cout << "Updated distance with ICP-only: " << updated_err << endl;

	   //      if (updated_err <= delta) {
	   //          iter_num_verified++;
	   //          if (iter_num_verified == 1 && !iter_found_one) {
	   //              iter_first_time = timer_end(SECOND);
	   //              iter_found_one = true;
	   //          }
	   //          break;
	   //      } else {
	   //          ret_left_goicp.push_back(r);
	   //      }
	   //  }
    // }

    // // Step 3: perform GoICP
    // for (auto &r: ret_left_goicp) {
    //  goicp[r->db_id].fly_init();
    //  goicp[r->db_id].OuterBnB(); // TODO: return false?

    //  // TODO
    // }

    if (!iter_found_one) {
        iter_first_time = timer_end(SECOND);
    }

    iter_veri_time += timer_end(SECOND);

    stat.prop_time += iter_prop_time;
    stat.veri_time += iter_veri_time;
    stat.user_time += (iter_prop_time + iter_veri_time);
    stat.first_verified_time += (iter_prop_time + iter_first_time);
    stat.num_verified = iter_num_verified;

    cout << "# verified: " << stat.num_verified << endl;

    stat.veri_size += ret.size();
    stat.num_icp_only += ret_left_icp_only.size();
    stat.num_goicp += ret_left_goicp.size();

    if (iter_num_verified > 0)
        return true;
    else
        return false;
}

#ifdef _TK
bool exec_tk(DB_Meshes* db_meshes, vector<C_RTree*>& db_rtrees, Mesh* mesh_q, Struct_Q* s_q, C_RTree* query_rtree, int k, double delta, double epsilon,
        GoICP goicp[], vector<ICP3D<float>*>& m_icps, float* m_query_icp, Exec_stat& stat) {

    vector<Exec_stat> stats;

    double exec_delta = delta;
    double exec_epsilon = epsilon;

    int k_m;
    if (qc.require == "prob") {
        k_m = 8;
    } else {
        k_m = 1;
    }

    int num_trial = 0;

    while (true) {

        cout << "Trial #" << (++num_trial) << " with delta: " << exec_delta << ", epsilon " << exec_epsilon << ": " << endl;

        Exec_stat stat_itr;
        bool find_accept = false;
        int num_accepted = 0;
        for (int t = 0; t < k_m; t++) {
            find_accept = iter(db_meshes, db_rtrees, mesh_q, s_q, query_rtree, exec_delta, exec_epsilon, goicp, m_icps, m_query_icp, stat_itr);
            if (find_accept) {
                num_accepted = stat_itr.num_verified;
                break;
            }
        }
        stats.push_back(stat_itr);

        if (num_accepted >= k)
            break;

        #ifndef _PROB
            exec_epsilon *= 2.0;
        #endif
        exec_delta *= 4.0;
    }

    get_sum_stat(&stats[0], num_trial, stat);

    stat.num_iterations = num_trial;

    return true;

}
#endif

// bool exec(DB_Meshes* db_meshes, vector<C_RTree*>& db_rtrees, Mesh* mesh_q, Struct_Q* s_q, C_RTree* query_rtree, double delta, double epsilon,
//         GoICP goicp[], vector<ICP3D<float>*>& m_icps, float* m_query_icp, Exec_stat& stat) {
bool exec(Exec_stat& stat) {

    bool find_accept = false;

    int k_m;
    if (qc.require == "prob") {
        k_m = 8;
    } else {
        k_m = 1;
    }

    for (int t = 0; t < k_m; t++) {

        find_accept = iter(stat);

        if (find_accept)
            break;
    }

    return find_accept;
}

int main(int argc, char **argv) {

    qc.read_param(argc, argv);

    if(!qc.load()) {
        exit(1);
    }

    cout << "db_path: " << qc.db_path << endl;
    cout << "query_filename: " << qc.query_filename << endl;
    cout << "delta: " << qc.delta << endl;
    cout << "epsilon: " << qc.epsilon << endl;
    cout << "color_err: " << qc.color_err << endl;
    cout << "donut_rad: " << qc.donut_rad << endl;
    cout << endl;

    decide_thresh_set();

    srand(time(NULL));

    const int exec_times = 50;
    int real_exec_times = 0;
    double aggr_prop_time = 0, aggr_veri_time = 0, aggr_user_time = 0, aggr_first_verified_time = 0;
    int aggr_veri_size = 0;
    int aggr_num_fail = 0;

    for (int exec_i = 0; exec_i < exec_times; exec_i++) {
    	cout << "Execution #" << (exec_i + 1) << ": " << endl << endl;
    	Exec_stat stat = (const struct Exec_stat){ 0 };
        #ifdef _TK
            bool exec_success = exec_tk(&db_meshes, db_rtrees, &mesh_q, &s_q, &query_rtree, k, delta, epsilon, goicp, m_icps, m_query_icp, stat);
        #else
            bool exec_success = exec(stat);
        #endif
        cout << endl;

        // cout << "Total number of candidate transformations: " << stat.veri_size << endl;
        // cout << "Final number of valid transformations: " << stat.num_verified << endl;
        // cout << "Total time: " << stat.user_time << "(s)" << endl;
        // cout << "Proposal time: " << stat.prop_time << "(s)" << endl;
        // cout << "Verification time: " << stat.veri_time << "(s)" << endl;
        // cout << "First verified time: " << stat.first_verified_time << "(s)" << endl;
        // cout << endl;

        aggr_prop_time += stat.prop_time;
        aggr_veri_time += stat.veri_time;
        aggr_user_time += stat.user_time;
        aggr_veri_size += stat.veri_size;
        aggr_first_verified_time += stat.first_verified_time;

        if (!exec_success)
            aggr_num_fail++;

        real_exec_times++;

        // if (real_exec_times <= 5 || real_exec_times == 10 || real_exec_times == 20) {
        if (real_exec_times == 10 || real_exec_times == 20) {
            if (aggr_user_time > 100.0) {
                break;
            }
        }
    }

    double aver_prop_time = aggr_prop_time / (double) real_exec_times;
    double aver_veri_time = aggr_veri_time / (double) real_exec_times;
    double aver_user_time = aggr_user_time / (double) real_exec_times;
    double aver_veri_size = (double) aggr_veri_size / (double) real_exec_times;
    double aver_first_verified_time = aggr_first_verified_time / (double) real_exec_times;

    if (write_stat) {
        ofstream stat_ofs(qc.stat_filename);
        stat_ofs << "user_time=" << aver_user_time << endl;
        stat_ofs << "prop_time=" << aver_prop_time << endl;
        stat_ofs << "veri_time=" << aver_veri_time << endl;
        stat_ofs << "veri_size=" << aver_veri_size << endl;
        stat_ofs << "num_fail=" << aggr_num_fail << endl;
        stat_ofs << "num_exec=" << real_exec_times << endl;
        // stat_ofs << aver_prop_time << "\t";
        // stat_ofs << aver_veri_time << "\t";
        // stat_ofs << aver_user_time << "\t";
        // stat_ofs << aver_veri_size << "\t";
        // stat_ofs << aver_first_verified_time << "\t";
        stat_ofs.close();
    }

    cout << endl;
    cout << "Average proposal time: " << aver_prop_time << endl;
    cout << "Average verification time: " << aver_veri_time << endl;
    cout << "Average user time: " << aver_user_time << endl;
    cout << "Average # candidate transformations: " << aver_veri_size << endl;
    cout << "Average find first time: " << aver_first_verified_time << endl;
    cout << "Failed exec: " << aggr_num_fail << endl;
}