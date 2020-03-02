#include "point.h"
#include "trans.h"
#include "util.h"
#include "pcr_adv.h"
#include "mesh.h"
#include "c_rtree.h"
#include "ProgressBar.hpp"
#include "struct_q.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <set>

using namespace std;
using namespace trimesh;

struct Dist_pair {
	int p1, p2;
	double d;	
};

bool comp_dist_pair(const Dist_pair& a, const Dist_pair& b) {
  return (a.d < b.d);
}

struct Extracted_pair {
	int p1, p2;
	Pt3D e, f;
	Extracted_pair(int i1, int i2) {
		this->p1 = i1;
		this->p2 = i2;
	}
	void fill(const Mesh* mesh, double r12, double r34) {
		auto pt1 = mesh->get_pt(p1);
		auto pt2 = mesh->get_pt(p2);
		e = (pt2 - pt1) * r12 + pt1;
		f = (pt2 - pt1) * r34 + pt1;
	}
};

struct Congr_base {
    int bp[4];
    // int db_id;
    Congr_base(int bp1, int bp2, int bp3, int bp4) {
        this->bp[0] = bp1;
        this->bp[1] = bp2;
        this->bp[2] = bp3;
        this->bp[3] = bp4;
        // this->db_id = db_id;
    }
};

const double ZERO = 0.001;
const double FINE_ZERO = 0.000001;
const int MAX_TRIAL = 50000;

inline bool eq(double a, double b) { return abs(a - b) <= ZERO; };
inline bool eq_zero(double x) { return abs(x) <= ZERO; };
inline bool eqf(double a, double b) { return abs(a - b) <= FINE_ZERO; };

bool solver(double a[3], double b[3], double& x, double &y) {
	double div = a[0] * b[1] - a[1] * b[0];
	x = (a[1] * b[2] - a[2] * b[1]) / div;
	y = (b[0] * a[2] - b[2] * a[0]) / div;
	return true;
}

bool cal_3d_intersection(Mesh* mesh_q, int tab[4], double& l, double& la, double& mu, Pt3D& e, Pt3D& f, double& angle) {
	auto q1 = mesh_q->get_pt(tab[0]);
	auto q2 = mesh_q->get_pt(tab[1]);
	auto q3 = mesh_q->get_pt(tab[2]);
	auto q4 = mesh_q->get_pt(tab[3]);
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
	
	auto q1 = mesh_q->get_pt(tab[0]);
	auto q2 = mesh_q->get_pt(tab[1]);
	auto q3 = mesh_q->get_pt(tab[2]);
	auto q4 = mesh_q->get_pt(tab[3]);
	auto q12 = q2 - q1;
	auto q34 = q4 - q3;

	auto n = cross_prd(&q12, &q34);
	auto n1 = cross_prd(&q12, &n);
	auto n2 = cross_prd(&q34, &n);
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

bool test_3d_coplanar(Mesh* mesh_q, Struct_Q* s_q, int* ran, double& d, double& l, double& la, double& mu, Pt3D& e, Pt3D& f, double& angle) {

    bool found_coplanar = false;

    int m = mesh_q->size();
    if (m <= 1000) {
    	bool found = false;
    	vector<Dist_pair> pairwise_dists;
    	for (int i = 0; i < m; i++) {
    		for (int j = i + 1; j < m; j++) {
                auto q_i = mesh_q->get_pt(i), q_j = mesh_q->get_pt(j);
    			Dist_pair dp { i, j, eucl_dist(&q_i, &q_j) };
    			pairwise_dists.push_back(dp);
    		}
    	}
    	sort(begin(pairwise_dists), begin(pairwise_dists) + pairwise_dists.size(), [] (Dist_pair a, Dist_pair b) { return comp_dist_pair(a, b); });
    	for (int i = 0; i < pairwise_dists.size() - 1; i++) {
    		if (eq(pairwise_dists[i + 1].d, pairwise_dists[i].d)) {
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
    }
    // int t = 0;
    // while (t < MAX_TRIAL && !found_coplanar) {
    //     // gen 3 different random indices in [0, m)
    //     ran1 = rand() % m; ran2 = 0; ran3 = 0;
    //     do {
    //         ran2 = rand() % m;
    //     } while (ran2 == ran1);
    //     do {
    //         ran3 = rand() % m;
    //     } while (ran3 == ran1 || ran3 == ran2);

    //     // get the first 3 base points
    //     bq1 = mesh_q->get_pt(ran1);
    //     bq2 = mesh_q->get_pt(ran2);
    //     bq3 = mesh_q->get_pt(ran3);

    //     // calculate the equation plane
    //     double a1 = bq2.x - bq1.x;
    //     double b1 = bq2.y - bq1.y;
    //     double c1 = bq2.z - bq1.z;
    //     double a2 = bq3.x - bq1.x;
    //     double b2 = bq3.y - bq1.y;
    //     double c2 = bq3.z - bq1.z;
    //     double a = b1 * c2 - b2 * c1;
    //     double b = a2 * c1 - a1 * c2;
    //     double c = a1 * b2 - b1 * a2;
    //     double d = (- a * bq1.x - b * bq1.y - c * bq1.z);

    //     double div = sqrt(a * a + b * b + c * c);

    //     // look for the fourth point
    //     for (int i = 0; i < m; i++) {
    //         if (i == ran1 || i == ran2 || i == ran3) // skip the repeats
    //             continue;

    //         bq4 = mesh_q->get_pt(i);
    //         double dist = (a * bq4.x + b * bq4.y + c * bq4.z + d) / div;

    //         if(eq(dist, 0) && cal_intersection(mesh_q, ran1, ran2, ran3, i, la, mu, e, angle)) {
    //             found_coplanar = true;
    //             ran4 = i;
    //             break;
    //         }
    //     }

    //     if (found_coplanar) {
    //     	break;
    //     }

    //     t++;
    // }

    if (!found_coplanar) {
        cerr << "coplanar not found" << endl;
        return false;
    }

    cout << "coplanar found: "
         << s_q->id_q_to_str(ran[0]) << " " << s_q->id_q_to_str(ran[1]) << " "
         << s_q->id_q_to_str(ran[2]) << " " << s_q->id_q_to_str(ran[3]) << endl;
    cout << "d: " << d << endl;
    cout << "l: " << l << endl;
    cout << "la: " << la << endl;
    cout << "mu: " << mu << endl;
    cout << "angle: " << angle << endl;

    return true;
}

void match_base_in_db_mesh(const Mesh* mesh_p, C_RTree* r_tree, /*OTree* o_tree,*/ double d, double l, double epsilon, double la, double mu, double angle, vector<Congr_base>& ret) {

	double err = max(2.0 * epsilon, 0.001);

    vector<int> intersection_ret;
    vector<Extracted_pair> eps;
    int index = 0;
    int n = mesh_p->size();

    RTree<int, double, 3, double> f_tree;

    // ProgressBar bar(n, 70);
    for (int i = 0; i < n; i++) {

        // ++bar;
        // bar.display();

        // if (i != 961) {
        //     continue;
        // }

        Pt3D p = mesh_p->get_pt(i);

        intersection_ret.clear();
        // o_tree->intersectingPair(p, d12_q, err, mesh_p, intersection_ret);
        r_tree->range_sphere_dist_err(&p, d, err, intersection_ret);

        if (!intersection_ret.empty()) {
            vector<Extracted_pair> ep_v;
            ep_v.clear();
            for (auto &r: intersection_ret) {
                Extracted_pair ep(i, r);
                ep.fill(mesh_p, la, mu);
                eps.push_back(ep);

                double fbox_min[3] { ep.f.x, ep.f.y, ep.f.z };
                double fbox_max[3] { ep.f.x, ep.f.y, ep.f.z };
                f_tree.Insert(fbox_min, fbox_max, index);

                index++;
            }
        }
    }

    // bar.done();

    cout << "Num of matching d: " << index << endl;

    double sq_dist_min = l - err;
    sq_dist_min = sq_dist_min * sq_dist_min;
    double sq_dist_max = l + err;
    sq_dist_max = sq_dist_max * sq_dist_max;

    for (auto &e_ep: eps) {
    	intersection_ret.clear();
    	double e_arr[3] { e_ep.e.x, e_ep.e.y, e_ep.e.z };
    	f_tree.SphereSearch(e_arr, sq_dist_min, sq_dist_max, intersection_ret);

    	// cout << intersection_ret.size() << endl;

    	if (intersection_ret.empty()) {
    		continue;
    	}

    	auto ep1 = mesh_p->get_pt(e_ep.p1) - e_ep.e;
    	double ep1_mode = ep1.mode();

    	for (auto &i: intersection_ret) {

    		auto ef = eps[i].f - e_ep.e;
    		auto fp3 = mesh_p->get_pt(eps[i].p1) - eps[i].f;

    		// cout << dot_prd(ep1, ef) << endl;

    		if (!eq_zero(dot_prd(&ep1, &ef) / (ep1.mode() * ef.mode()))) {
    			continue;
    		}

    		double cos_p1efp3 = (dot_prd(&ep1, &fp3)) / (ep1_mode * fp3.mode());
    		// cout << cos_p1efp3 << endl;

    		if (abs(cos_p1efp3 - angle) > 0.1) {
                continue;
            }

            ret.push_back(Congr_base(e_ep.p1, e_ep.p2, eps[i].p1, eps[i].p2));
    	}
    }

}

int main(int argc, char **argv) {

    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " database_path query_filename delta [-stat=...]" << endl;
        exit(1);
    }

    bool write_stat = false;
    string stat_filename = "";
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
        if (argv_str.rfind("-stat", 0) == 0) {
            write_stat = true;
            stat_filename = string(argv[i] + 6);
        }
    }

    int argi = 0;
    string db_path = argv[(++argi)];
    string query_filename = argv[(++argi)];

    double delta = atof(argv[(++argi)]);

    srand(time(NULL));


    cout << "Reading database files from " << db_path << endl;
    DB_Meshes db_meshes;
    int num_meshes = db_meshes.read_from_path(db_path);
    cout << "Total no. meshes: " << num_meshes << endl << endl;

    db_meshes.build_kd();

    cout << "Reading database R-trees..." << endl;
    vector<C_RTree> db_rtrees;
    read_rtrees_from_db_meshes(&db_meshes, db_rtrees);
    
    cout << endl;

    // vector<OTree> o_trees;
    // double err = 10.0;
    // cout << "Start building OcTree with err: " << err << endl;
    // for (auto &m: db_meshes) {
	   // OTree o_tree;
	   // o_tree.buildFromMesh(m, err, true);
	   // o_trees.push_back(o_tree);
    // }

    cout << "Reading query mesh from " << query_filename << endl;
    Mesh mesh_q;
    mesh_q.read_from_path(query_filename);
    cout << endl;

    // load the query structure
    Struct_Q s_q;
    s_q.read(query_filename + ".info");

    timer_start();

    int ran[4];
    double l, d;
    double la, mu;
    Pt3D e, f;
    double angle; // cos(theta)

    bool found = test_3d_coplanar(&mesh_q, &s_q, ran, d, l, la, mu, e, f, angle);
    if (!found) {
    	exit(0);
    }

    // testing
    // ran1 = 61;
    // ran2 = 25;
    // ran3 = 40;
    // ran4 = 60;
    // cal_intersection(&mesh_q, ran1, ran2, ran3, ran4, la, mu, e, angle);
    // cout << "la: " << la << endl;
    // cout << "mu: " << mu << endl;
    // cout << "angle: " << angle << endl;

    Pt3D bq[4];
    for (int i = 0; i < 4; i++) {
    	bq[i] = mesh_q.get_pt(ran[i]);
    }

    double epsilon = 0.1;

    double exec_user_time = 0.0, exec_prop_time = 0.0, exec_veri_time = 0.0, exec_retr_time = 0.0;
    int exec_veri_num = 0;

    int verified_size = 0;

    vector<Congr_base> ret;
    for (int i = 0; i < db_meshes.size(); i++) {
        ret.clear();
        // match_base_in_db_mesh(db_meshes.get_mesh(i), &o_trees[i], d12_q, d34_q, epsilon, la, mu, angle, ret);
        match_base_in_db_mesh(db_meshes.get_mesh(i), &db_rtrees[i], d, l, epsilon, la, mu, angle, ret);

        exec_veri_num += ret.size();

        timer_start();
        for (auto &r: ret) {
            Pt3D bp[4];
            for (int j = 0; j < 4; j++) {
                bp[j] = db_meshes.get_mesh(i)->get_pt(r.bp[j]);
            }
            auto xf = cal_trans(bq, bp, 4);

            double result = db_meshes.cal_corr_err(&mesh_q, i, &xf, delta);
            if (result > 0) {
                // cout << "Accept:" << endl << h->to_str(10) << endl << result << endl;
                verified_size++;
            }
        }

        exec_veri_time += timer_end(SECOND);

        // break;
    }

    cout << endl;
    cout << "Verified size: " << verified_size << endl;

    exec_user_time = timer_end(SECOND);
    exec_prop_time = exec_user_time - exec_veri_time; // TODO: also minus retrieval time

    cout << endl;
    cout << "Average proposal time: " << exec_prop_time << endl;
    cout << "Average verification time: " << exec_veri_time << endl;
    cout << "Average retrieval time: " << exec_retr_time << endl;
    cout << "Average number of candidate transformations: " << exec_veri_num << endl;
    // cout << "Failed exec: " << exec_num_fail << endl;
    cout << "Average user time: " << exec_user_time << endl;

}
