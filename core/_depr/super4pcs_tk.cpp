#include "point.h"
#include "trans.h"
#include "util.h"
// #include "mesh.h"
#include "c_rtree.h"
#include "RTree.h"
#include "struct_q.h"
#include "goicp.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <set>
#include <unordered_map>

using namespace std;
using namespace trimesh;

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
const double THRESHOLD_LIST[5] = { 0.01, 10, 0.01, 0.01, 0.01 }; // prob: dense point set (obj) & comp_7, !!don't change this!!
// const double THRESHOLD_LIST[5] = { 0.01, 10, 0.01, 0.01, 0.1 }; // det values
// const double THRESHOLD_LIST[5] = { 0.01, 1, 0.01, 0.01, 0.01 }; // prob: comp_6, !!don't change this!! -> now this is also for comp_7
// const double THRESHOLD_LIST[5] = { 0.01, 1, 0.1, 0.05, 0.2 }; // det values
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
    Extracted_pair(int i1, int i2) {
        this->p1 = i1;
        this->p2 = i2;
    }
    void fill(Mesh* mesh, double r) {
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

bool cal_3d_intersection(Mesh* mesh_q, int tab[4], double& l, double& la, double& mu, Pt3D& e, Pt3D& f, double& angle) {
    auto q1 = *(mesh_q->get_pt(tab[0]));
    auto q2 = *(mesh_q->get_pt(tab[1]));
    auto q3 = *(mesh_q->get_pt(tab[2]));
    auto q4 = *(mesh_q->get_pt(tab[3]));
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

    e = q1 + q12 * la;
    f = q3 + q34 * mu;

    l = eucl_dist(&e, &f);

    auto eq1 = q1 - e;
    auto fq3 = q3 - f;
    angle = dot_prd(&eq1, &fq3) / (eq1.mode() * fq3.mode());

    return true;
}

bool cal_3d_intersection_new(Mesh* mesh_q, int tab[4], double& l, double& la, double& mu, Pt3D& e, Pt3D& f, double& angle) {
    
    auto q1 = *(mesh_q->get_pt(tab[0]));
    auto q2 = *(mesh_q->get_pt(tab[1]));
    auto q3 = *(mesh_q->get_pt(tab[2]));
    auto q4 = *(mesh_q->get_pt(tab[3]));
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

    e = q1 + q12 * la;
    f = q3 + q34 * mu;

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

bool cal_intersection(Mesh* mesh_q, int ran1, int ran2, int ran3, int ran4, double& la, double& mu, Pt3D& e, double& angle) {
	auto q1 = *(mesh_q->get_pt(ran1));
	auto q2 = *(mesh_q->get_pt(ran2));
	auto q3 = *(mesh_q->get_pt(ran3));
	auto q4 = *(mesh_q->get_pt(ran4));
	mu = ((q3.x - q1.x) * (q2.y - q1.y) - (q3.y - q1.y) * (q2.x - q1.x))
	   / ((q4.y - q3.y) * (q2.x - q1.x) - (q4.x - q3.x) * (q2.y - q1.y));

	// cout << mu << endl;
	if (mu < 0 || mu > 1) {
		return false;
	}

	e = q3 + (q4 - q3) * mu;

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

bool test_3d_coplanar(Mesh* mesh_q, C_RTree* r_q, Struct_Q* s_q, int* ran, double& d, double& l, double& la, double& mu, Pt3D& e, Pt3D& f, double& angle) {

    bool found_coplanar = false;

    int m = mesh_q->size();

    if (m <= 400) {
        bool found = false;
        vector<Dist_pair> pairwise_dists;
        for (int i = 0; i < m; i++) {
            for (int j = i + 1; j < m; j++) {
                auto q_i = mesh_q->get_pt(i), q_j = mesh_q->get_pt(j);
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
                if (cal_3d_intersection_new(mesh_q, tab, l, la, mu, e, f, angle)) {
                // if (brufc_find_la_mu(mesh_q, tab, l, la, mu, e, f, angle)) {
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

            auto bq1 = mesh_q->get_pt(ran[0]);
            auto bq2 = mesh_q->get_pt(ran[1]);

            d = eucl_dist(bq1, bq2);

            for (int j = 0; j < 100; j++) {
                do {
                    ran[2] = rand() % m;
                } while (ran[2] == ran[0] || ran[2] == ran[1]);

                auto bq3 = mesh_q->get_pt(ran[2]);
                vector<int> bq4_cand_set;
                r_q->range_sphere_dist_err(bq3, d, THRESHOLD_LIST[1], bq4_cand_set); // for object dataset
                if (bq4_cand_set.empty()) {
                    continue;
                }

                for (auto &i: bq4_cand_set) {
                    ran[3] = i;
                    if (cal_3d_intersection_new(mesh_q, ran, l, la, mu, e, f, angle)) {
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

    // cout << "coplanar found: "
    //      << s_q->id_q_to_str(ran[0]) << " " << s_q->id_q_to_str(ran[1]) << " "
    //      << s_q->id_q_to_str(ran[2]) << " " << s_q->id_q_to_str(ran[3]) << endl;
    // cout << "d: " << d << endl;
    // cout << "l: " << l << endl;
    // cout << "la: " << la << endl;
    // cout << "mu: " << mu << endl;
    // cout << "angle: " << angle << endl;

    return true;
}

bool test_coplanar(Mesh* mesh_q, Struct_Q* s_q, int* ran, double& d12_q, double& d34_q, double& la, double& mu, Pt3D& e, double& angle) {

    Pt3D bq[4];
    int m = mesh_q->size();
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

        // get the first 3 base points
        for (int i = 0; i < 3; i++) {
            bq[i] = *(mesh_q->get_pt(ran[i]));
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

            bq[3] = *(mesh_q->get_pt(i));
            double dist = (a * bq[3].x + b * bq[3].y + c * bq[3].z + d) / div;

            if((abs(dist) <= THRESHOLD_LIST[0]) && cal_intersection(mesh_q, ran[0], ran[1], ran[2], i, la, mu, e, angle)) {
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

int match_base_in_db_mesh_gen(Mesh* mesh_p, C_RTree* r_tree, double d, double l, double epsilon, double la, double mu, double angle, int db_id, vector<Congr_base>& ret) {

    double err = max(2.0 * epsilon, 0.001);

    vector<int> intersection_ret;
    vector<Extracted_pair_gen> eps;
    int index = 0;
    int n = mesh_p->size();

    RTree<int, double, 3, double> e_tree;
    RTree<int, double, 3, double> f_tree;

    for (int i = 0; i < n; i++) {

        auto p = mesh_p->get_pt(i);

        intersection_ret.clear();
        r_tree->range_sphere_dist_err(p, d, err, intersection_ret);

        // if (db_id == 0 && (i == 963 || i == 973 || i == 959 || i == 966)) {
        //     cout << "For i = " << i << ":";
        //     for (auto &v: intersection_ret) {
        //         cout << " " << v;
        //     }
        //     cout << endl;
        // }

        for (auto &r: intersection_ret) {
            eps.emplace_back(i, r, mesh_p, la, mu);
            Extracted_pair_gen& ep = eps[eps.size() - 1];

            double ebox_min[3] { ep.e.x, ep.e.y, ep.e.z };
            double ebox_max[3] { ep.e.x, ep.e.y, ep.e.z };
            e_tree.Insert(ebox_min, ebox_max, index);

            double fbox_min[3] { ep.f.x, ep.f.y, ep.f.z };
            double fbox_max[3] { ep.f.x, ep.f.y, ep.f.z };
            f_tree.Insert(fbox_min, fbox_max, index);

            index++;
        }
    }

    // cout << "Num of matching d: " << index << endl;

    double sq_dist_min = max(0.0, l - err);
    sq_dist_min = sq_dist_min * sq_dist_min;
    double sq_dist_max = l + err;
    sq_dist_max = sq_dist_max * sq_dist_max;

    int num_matched = 0;

    for (auto &e_ep: eps) {

        intersection_ret.clear();
        double e_arr[3] { e_ep.e.x, e_ep.e.y, e_ep.e.z };
        f_tree.SphereSearch(e_arr, sq_dist_min, sq_dist_max, intersection_ret);

        if (!intersection_ret.empty()) {

            auto ep1 = *(mesh_p->get_pt(e_ep.p1)) - e_ep.e;
            double ep1_mode = ep1.mode();

            for (auto &i: intersection_ret) {

                auto ef = eps[i].f - e_ep.e;
                auto fp3 = *(mesh_p->get_pt(eps[i].p1)) - eps[i].f;

                // cout << dot_prd(ep1, ef) << endl;

                // if (e_ep.p1 == 856 && e_ep.p2 == 882 && eps[i].p1 == 854 && eps[i].p2 == 874 && db_id == 0) {
                // 	cout << (dot_prd(&ep1, &ef) / (ep1.mode() * ef.mode())) << endl;
                // 	cout << (dot_prd(&ep1, &fp3)) / (ep1_mode * fp3.mode() - angle) << endl;
                // }

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

int match_base_in_db_mesh(Mesh* mesh_p, C_RTree* r_tree, double d12_q, double d34_q, double epsilon, double la, double mu, double angle, int db_id, vector<Congr_base>& ret) {
    
    vector<int> intersection_ret;
    unordered_map<int, vector<Extracted_pair>> ep12;
    unordered_map<int, vector<Extracted_pair>> ep34;
    int count12 = 0, count34 = 0;
    int n = mesh_p->size();

    double err = max(0.01, 2 * epsilon);
    for (int i = 0; i < n; i++) {

        auto p = mesh_p->get_pt(i);

        intersection_ret.clear();
        r_tree->range_sphere_dist_err(p, d12_q, err, intersection_ret);

        if (!intersection_ret.empty()) {
            vector<Extracted_pair> ep12_v;
            ep12_v.clear();
            for (auto &r: intersection_ret) {
                Extracted_pair ep(i, r);
                ep.fill(mesh_p, la);
                ep12_v.push_back(ep);
                count12++;
            }
            ep12[i] = ep12_v;
        }

        intersection_ret.clear();
        r_tree->range_sphere_dist_err(p, d34_q, err, intersection_ret);

        if (!intersection_ret.empty()) {
            vector<Extracted_pair> ep34_v;
            ep34_v.clear();
            for (auto &r: intersection_ret) {
                Extracted_pair ep(i, r);
                ep.fill(mesh_p, mu);
                ep34_v.push_back(ep);
                count34++;
            }
            ep34[i] = ep34_v;
        }
    }

    // cout << "Num of matching 12: " << count12 << endl;
    // cout << "Num of matching 34: " << count34 << endl;

    double l34 = d34_q * mu;

    int num_matched = 0;

    for (auto &v12: ep12) {
        for (auto &s12: v12.second) {

            auto e12 = s12.e;

            intersection_ret.clear();
            r_tree->range_sphere_dist_err(&e12, l34, err, intersection_ret);

            for (auto &i: intersection_ret) {

                auto iPt = mesh_p->get_pt(i);
                auto p1 = mesh_p->get_pt(v12.first);
                auto e12_iPt = *iPt - e12;
                auto e12_p1 = *p1 - e12;
                double cosIPt = (dot_prd(&e12_iPt, &e12_p1)) / (e12_iPt.mode() * e12_p1.mode());

                // if (s12.p1 == 1009 && s12.p2 == 852 && db_id == 0) {
                // 	cout << (cosIPt - angle) << endl;
                // }

                if (abs(cosIPt - angle) > THRESHOLD_LIST[2]) {
                    continue;
                }

                auto got = ep34.find(i);
                if (got == ep34.end()) {
                    // not found
                    continue;
                }

                for (auto &s34: got->second) {
                    double dist = eucl_dist(&e12, &s34.e);

	                // if (s12.p1 == 1009 && s12.p2 == 852 && s34.p1 == 859 && s34.p2 == 990 && db_id == 0) {
	                // 	cout << dist << endl;
	                // }

                    if (dist <= err) {
                        ret.push_back(Congr_base(s12.p1, s12.p2, s34.p1, s34.p2, db_id));
                        num_matched++;
                    }
                }
            }
        }
    }

    return num_matched;
}

bool iter(DB_Meshes* db_meshes, vector<C_RTree*>& db_rtrees, Mesh* mesh_q, Struct_Q* s_q, C_RTree* query_rtree, double delta, double epsilon,
        GoICP goicp[], vector<ICP3D<float>*>& m_icps, float* m_query_icp, Exec_stat& stat) {

    timer_start();

    int ran[4];
    double la, mu;
    Pt3D e;
    double angle; // cos(theta)
    #ifdef _GEN
        double l, d;
        Pt3D f;
    #else
        double d12_q, d34_q;
    #endif

    vector<Congr_base> ret;

    double iter_prop_time = 0.0, iter_veri_time = 0.0;

    vector<Congr_base*> ret_left_goicp;

    int iter_num_verified = 0;

    #ifdef _GEN
        bool found_coplanar = test_3d_coplanar(mesh_q, query_rtree, s_q, ran, d, l, la, mu, e, f, angle);
    #else
        bool found_coplanar = test_coplanar(mesh_q, s_q, ran, d12_q, d34_q, la, mu, e, angle);
    #endif

    if (!found_coplanar) {
        timer_end(SECOND);
        return false;
    }

    // ran[0] = 56;
    // ran[1] = 3;
    // ran[2] = 9;
    // ran[3] = 52;
    cout << ran[0] << " " << ran[1] << " " << ran[2] << " " << ran[3] << endl;

    Pt3D* bq_ptr[4];
    for (int i = 0; i < 4; i++) {
        bq_ptr[i] = mesh_q->get_pt(ran[i]);
    }

    for (int i = 0; i < db_meshes->size(); i++) {
        // cout << "Match base in db#" << i << endl;
        int ret_size;
        #ifdef _GEN
            ret_size = match_base_in_db_mesh_gen(db_meshes->get_mesh(i), db_rtrees[i], d, l, epsilon, la, mu, angle, i, ret);
        #else
            ret_size = match_base_in_db_mesh(db_meshes->get_mesh(i), db_rtrees[i], d12_q, d34_q, epsilon, la, mu, angle, i, ret);
        #endif
        // cout << "Returned matched: " << ret_size << endl;
    }

    // if (ran[0] == 7 && ran[1] == 23 && ran[2] == 5 && ran[3] == 18) {
    //     ret.emplace_back(856, 882, 854, 874, 0);
    //     cout << "total: (" << ret.size() << ")" << endl;
    //     for (auto &r: ret) {
    //         if (r.db_id == 0)
    //             cout << r.db_id << ": " << r.bp[0] << " " << r.bp[1] << " " << r.bp[2] << " " << r.bp[3] << endl;
    //     }
    // }

    iter_prop_time = timer_end(SECOND);

    timer_start();

    // // perform the ICP-only check
    // for (auto &r: ret) {
    //     Pt3D* bp_ptr[4];
    //     for (int j = 0; j < 4; j++) {
    //         bp_ptr[j] = db_meshes->get_mesh(r.db_id)->get_pt(r.bp[j]);
    //     }
    //     cal_trans(bq_ptr, bp_ptr, 4, r.xf);

    //     double init_dist = db_meshes->cal_corr_err(mesh_q, r.db_id, &r.xf);
    //     // cout << "Initial distance: " << init_dist << endl;
    //     double r_array[9] {
    //         r.xf.r11, r.xf.r12, r.xf.r13,
    //         r.xf.r21, r.xf.r22, r.xf.r23,
    //         r.xf.r31, r.xf.r32, r.xf.r33
    //     };
    //     ICP_Matrix R_icp(3, 3, r_array);
    //     double t_array[3] {
    //         r.xf.tx, r.xf.ty, r.xf.tz
    //     };
    //     ICP_Matrix t_icp(3, 1, t_array);
    //     double updated_err = m_icps[r.db_id]->Run(m_query_icp, mesh_q->size(), R_icp, t_icp);
    //     // cout << "Updated distance with ICP-only: " << updated_err << endl;

    //     if (updated_err <= delta) {
    //         iter_num_verified++;
    //     } else {
    //         ret_left_goicp.push_back(&r);
    //     }
    // }

    // for large datasets: not doing ICP, use direct check only:
    for (auto &h: ret) {
        Pt3D* bp_ptr[4];
        for (int j = 0; j < 4; j++) {
            bp_ptr[j] = db_meshes->get_mesh(h.db_id)->get_pt(h.bp[j]);
        }
        cal_trans(bq_ptr, bp_ptr, 4, h.xf);

        // cout << "Cal dist for\n" << h.e_query->to_str() << "\nand\n" << h.e_database->to_str() << "\n in db obj#" << h->id_db << endl;
        h.init_dist = db_meshes->cal_corr_err(mesh_q, h.db_id, &h.xf, delta); // TODO: simplified method, stops when error is larger
        // cout << "Initial distance: " << h->init_dist << " for pair: " << h->to_str(10) << endl;

        if (h.init_dist >= 0) {
            // cout << "Matched with DB #" << h->id_db << ": " << qc->db_meshes.get_mesh(h->id_db)->get_filename() << endl;
            iter_num_verified++;
        }
    }

    // // Step 3: perform GoICP
    // for (auto &r: ret_left_goicp) {
    //  goicp[r->db_id].fly_init();
    //  goicp[r->db_id].OuterBnB(); // TODO: return false?

    //  // TODO
    // }

    iter_veri_time += timer_end(SECOND);

    stat.prop_time += iter_prop_time;
    stat.veri_time += iter_veri_time;
    stat.user_time += (iter_prop_time + iter_veri_time);
    stat.num_verified = iter_num_verified;

    cout << "# verified: " << stat.num_verified << endl;

    stat.veri_size += ret.size();
    stat.num_goicp += ret_left_goicp.size();

    if (iter_num_verified > 0)
        return true;
    else
        return false;
}

#ifdef _PROB
bool exec(DB_Meshes* db_meshes, vector<C_RTree*>& db_rtrees, Mesh* mesh_q, Struct_Q* s_q, C_RTree* query_rtree, double delta_0, double epsilon_0, double fix_epsilon,
        GoICP goicp[], vector<ICP3D<float>*>& m_icps, float* m_query_icp, int k, Exec_stat& stat)
#else
bool exec(DB_Meshes* db_meshes, vector<C_RTree*>& db_rtrees, Mesh* mesh_q, Struct_Q* s_q, C_RTree* query_rtree, double delta_0, double epsilon_0,
        GoICP goicp[], vector<ICP3D<float>*>& m_icps, float* m_query_icp, int k, Exec_stat& stat)
#endif
{

	double delta = delta_0;
    #ifdef _PROB
        double epsilon = fix_epsilon;
        int k_m = 12;
    #else
        double epsilon = epsilon_0;
        int k_m = 1;
    #endif
	int num_itr = 0;

    while (true) {

    	cout << "Iteration #" << (++num_itr) << " with delta: " << delta << ", epsilon " << epsilon << ": " << endl;

        for (int t = 0; t < k_m; t++) {
    	    if (iter(db_meshes, db_rtrees, mesh_q, s_q, query_rtree, delta, epsilon, goicp, m_icps, m_query_icp, stat)) {
                break;
            }
        }

        if (stat.num_verified >= k)
            break;

        #ifndef _PROB
            epsilon *= 2.0;
        #endif
        delta *= 4.0;
    }

    stat.num_iterations = num_itr;

    return true;
}

int main(int argc, char **argv) {

    #ifdef _PROB
        if (argc < 3) {
            cerr << "Usage: " << argv[0] << " database_path query_filename [-k=...] [-stat=...] [-num_parts=...]" << endl;
            exit(1);
        }
    #else
        if (argc < 4) {
            cerr << "Usage: " << argv[0] << " database_path query_filename delta [-k=...] [-stat=...] [-num_parts=...]" << endl;
            exit(1);
        }
    #endif

    int k = 1;
    bool write_stat = false;
    string stat_filename = "";
    int num_parts = 0;
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
        if (argv_str.rfind("-stat", 0) == 0) {
            write_stat = true;
            stat_filename = string(argv[i] + 6);
        } else if (argv_str.rfind("-num_parts", 0) == 0) {
            num_parts = atoi(argv[i] + 11);
        } else if (argv_str.rfind("-k", 0) == 0) {
        	k = atoi(argv[i] + 3);
        }
    }

    int argi = 0;
    string db_path = argv[(++argi)];
    string query_filename = argv[(++argi)];

    #ifdef _PROB
        double fix_epsilon = atof(argv[(++argi)]);
        cout << "fix_epsilon: " << fix_epsilon << endl;
    #endif

    cout << "db_path: " << db_path << endl;
    cout << "query_filename: " << query_filename << endl;
    cout << "k: " << k << endl;
    cout << endl;

    srand(time(NULL));


    cout << "Reading database files from " << db_path << endl;
    DB_Meshes db_meshes;
    int num_meshes = db_meshes.read_from_path(db_path);
    if (num_meshes < 0) {
        cerr << "Error reading database files" << endl;
        exit(1);
    }
    cout << "Total no. meshes: " << num_meshes << endl << endl;

    cout << "Building KDtrees for DB meshes" << endl;
    db_meshes.build_kd();

    cout << "Reading database R-trees..." << endl;
    vector<C_RTree*> db_rtrees;
    if (num_parts > 0) {
        const int num_meshes_per_part = num_meshes / num_parts;
        for (int i = 0; i < num_parts; i++) {
            read_rtrees_comb(db_path, i, num_meshes_per_part, db_rtrees);
        }
    } else {
        read_rtrees_from_db_meshes(&db_meshes, db_rtrees);
    }

    cout << endl;

    cout << "Reading query mesh from " << query_filename << endl;
    Mesh mesh_q;
    mesh_q.read_from_path(query_filename);
    cout << endl;

    // load the query structure
    Struct_Q s_q;
    if (!s_q.read(query_filename + ".info")) {
        cout << "Error loading query structure" << endl;
        exit(1);
    }

    // load the query R-tree
    C_RTree query_rtree;
    query_rtree.read_from_mesh(query_filename);

    vector<ICP3D<float>*> m_icps;
    vector<float*> m_data_icps;
    float* m_query_icp;

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

    GoICP goicp[3];
    // for (int i = 0; i < num_meshes; i++) {
    // 	loadGoICP(&db_meshes, i, &mesh_q, delta, goicp[i]);
    //     loadGoICP(&db_meshes, i, &mesh_q, sse, delta, &goicp[i], verbose, use_dt, dt_size);

    // 	goicp[i].Initialize();
    // }
    // cout << endl;

    double q_diam = mesh_q.get_bsphere_d();
    #ifdef _PROB
        double mse_0 = 0.001 * q_diam;
    #else
        double mse_0 = 0.0001 * q_diam;
    #endif
    double sse_0 = mse_0 * (double) mesh_q.size();
    double epsilon_0 = sqrt(sse_0);
    cout << "Diameter of query: " << q_diam << endl;
    cout << "MSE_0: " << mse_0 << endl;
    cout << "SSE_0: " << sse_0 << endl;
    cout << "epsilon_0: " << epsilon_0 << endl << endl;

    const int exec_times = 10;
    int real_exec_times = 0;
    double aggr_prop_time = 0, aggr_veri_time = 0, aggr_user_time = 0;
    double aggr_veri_size = 0, aggr_num_itr = 0; 

    for (int exec_i = 0; exec_i < exec_times; exec_i++) {
    	cout << "Execution #" << (exec_i + 1) << ": " << endl << endl;
    	Exec_stat stat = (const struct Exec_stat){ 0 };
        #ifdef _PROB
            exec(&db_meshes, db_rtrees, &mesh_q, &s_q, &query_rtree, sse_0, epsilon_0, fix_epsilon, goicp, m_icps, m_query_icp, k, stat);
        #else
            exec(&db_meshes, db_rtrees, &mesh_q, &s_q, &query_rtree, sse_0, epsilon_0, goicp, m_icps, m_query_icp, k, stat);
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
        aggr_num_itr += stat.num_iterations;
        
        real_exec_times++;

        if (real_exec_times <= 5 || real_exec_times == 10 || real_exec_times == 20) {
        // if (real_exec_times == 10 || real_exec_times == 20) {
            if (aggr_user_time > 100.0) {
                break;
            }
        }
    }

    double aver_prop_time = aggr_prop_time / (double) real_exec_times;
    double aver_veri_time = aggr_veri_time / (double) real_exec_times;
    double aver_user_time = aggr_user_time / (double) real_exec_times;
    double aver_veri_size = aggr_veri_size / (double) real_exec_times;
    double aver_num_itr = aggr_num_itr / (double) real_exec_times;

    if (write_stat) {
        ofstream stat_ofs(stat_filename);
        stat_ofs << "user_time=" << aver_user_time << endl;
        stat_ofs << "prop_time=" << aver_prop_time << endl;
        stat_ofs << "veri_time=" << aver_veri_time << endl;
        stat_ofs << "veri_size=" << aver_veri_size << endl;
        stat_ofs << "num_itr=" << aver_num_itr << endl;
        stat_ofs << "num_exec=" << real_exec_times << endl;
        stat_ofs.close();
    }

    cout << endl;
    cout << "Average proposal time: " << aver_prop_time << endl;
    cout << "Average verification time: " << aver_veri_time << endl;
    cout << "Average user time: " << aver_user_time << endl;
    cout << "Average # candidate transformations: " << aver_veri_size << endl;

}