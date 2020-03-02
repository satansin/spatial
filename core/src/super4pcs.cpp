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

using namespace std;
using namespace trimesh;

struct Extracted_pair {
	int p1, p2;
	Pt3D e;
	Extracted_pair(int i1, int i2) {
		this->p1 = i1;
		this->p2 = i2;
	}
	void fill(const Mesh* mesh, double r) {
		auto pt1 = mesh->get_pt(p1);
		auto pt2 = mesh->get_pt(p2);
		e = (pt2 - pt1) * r + pt1;
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
inline bool eqf(double a, double b) { return abs(a - b) <= FINE_ZERO; };

bool cal_intersection(Mesh* mesh_q, int ran1, int ran2, int ran3, int ran4, double& la, double& mu, Pt3D& e, double& angle) {
	auto q1 = mesh_q->get_pt(ran1);
	auto q2 = mesh_q->get_pt(ran2);
	auto q3 = mesh_q->get_pt(ran3);
	auto q4 = mesh_q->get_pt(ran4);
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

bool test_coplanar(Mesh* mesh_q, Struct_Q* s_q, int& ran1, int& ran2, int& ran3, int& ran4, double& la, double& mu, Pt3D& e, double& angle) {

    Pt3D bq1, bq2, bq3, bq4;

    int m = mesh_q->size();
    int t = 0;
    bool found_coplanar = false;
    while (t < MAX_TRIAL && !found_coplanar) {
        // gen 3 different random indices in [0, m)
        ran1 = rand() % m; ran2 = 0; ran3 = 0;
        do {
            ran2 = rand() % m;
        } while (ran2 == ran1);
        do {
            ran3 = rand() % m;
        } while (ran3 == ran1 || ran3 == ran2);

        // get the first 3 base points
        bq1 = mesh_q->get_pt(ran1);
        bq2 = mesh_q->get_pt(ran2);
        bq3 = mesh_q->get_pt(ran3);

        // calculate the equation plane
        double a1 = bq2.x - bq1.x;
        double b1 = bq2.y - bq1.y;
        double c1 = bq2.z - bq1.z;
        double a2 = bq3.x - bq1.x;
        double b2 = bq3.y - bq1.y;
        double c2 = bq3.z - bq1.z;
        double a = b1 * c2 - b2 * c1;
        double b = a2 * c1 - a1 * c2;
        double c = a1 * b2 - b1 * a2;
        double d = (- a * bq1.x - b * bq1.y - c * bq1.z);

        double div = sqrt(a * a + b * b + c * c);

        // look for the fourth point
        for (int i = 0; i < m; i++) {
            if (i == ran1 || i == ran2 || i == ran3) // skip the repeats
                continue;

            bq4 = mesh_q->get_pt(i);
            double dist = (a * bq4.x + b * bq4.y + c * bq4.z + d) / div;

            if(eq(dist, 0) && cal_intersection(mesh_q, ran1, ran2, ran3, i, la, mu, e, angle)) {
                found_coplanar = true;
                ran4 = i;
                break;
            }
        }

        if (found_coplanar) {
        	break;
        }

        t++;
    }

    if (!found_coplanar) {
        cerr << "coplanar not found" << endl;
        return false;
    } else {
        cout << "coplanar found: "
             << ran1 << " " << ran2 << " "
             << ran3 << " " << ran4 << endl;
        cout << "la: " << la << endl;
        cout << "mu: " << mu << endl;
        cout << "angle: " << angle << endl;
    }

    // cheating-verify
    if (s_q->get_id_mapping(ran1) >= 0 && s_q->get_id_mapping(ran2) >= 0 && s_q->get_id_mapping(ran3) >= 0 && s_q->get_id_mapping(ran4) >= 0) {
    	cout << "success" << endl;
    } else {
    	cout << "fail" << endl;
    }

    return true;
}

void match_base_in_db_mesh(const Mesh* mesh_p, C_RTree* r_tree, /*OTree* o_tree,*/ double d12_q, double d34_q, double epsilon, double la, double mu, double angle, vector<Congr_base>& ret) {
    vector<int> intersection_ret;
    unordered_map<int, vector<Extracted_pair>> ep12;
    unordered_map<int, vector<Extracted_pair>> ep34;
    int count12 = 0, count34 = 0;
    int n = mesh_p->size();
    // ProgressBar bar(n, 70);
    for (int i = 0; i < n; i++) {

        // ++bar;
        // bar.display();

        // if (i != 961) {
        //     continue;
        // }

        Pt3D p = mesh_p->get_pt(i);

        intersection_ret.clear();
        // o_tree->intersectingPair(p, d12_q, 2 * epsilon, mesh_p, intersection_ret);
        r_tree->range_sphere_dist_err(&p, d12_q, 2 * epsilon, intersection_ret);

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
        // o_tree->intersectingPair(p, d34_q, 2 * epsilon, mesh_p, intersection_ret);
        r_tree->range_sphere_dist_err(&p, d34_q, 2 * epsilon, intersection_ret);

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

    // bar.done();

    // cout << "Num of matching 12: " << count12 << endl;
    // cout << "Num of matching 34: " << count34 << endl;

    double l34 = d34_q * mu;

    for (auto &v12: ep12) {
        for (auto &s12: v12.second) {

            auto e12 = s12.e;

            intersection_ret.clear();
            // o_tree->intersectingPair(e12, l34, 2 * epsilon, mesh_p, intersection_ret);
            r_tree->range_sphere_dist_err(&e12, l34, 2 * epsilon, intersection_ret);

            for (auto &i: intersection_ret) {

                auto iPt = mesh_p->get_pt(i);
                auto p1 = mesh_p->get_pt(v12.first);
                auto e12_iPt = iPt - e12;
                auto e12_p1 = p1 - e12;
                double cosIPt = (dot_prd(&e12_iPt, &e12_p1)) / (e12_iPt.mode() * e12_p1.mode());

                if (abs(cosIPt - angle) > 0.1) {
                    continue;
                }

                auto got = ep34.find(i);
                if (got == ep34.end()) {
                    // not found
                    continue;
                }

                for (auto &s34: got->second) {
                    double dist = eucl_dist(&e12, &s34.e);
                    if (dist <= 5.0) {
                    // if (v12.first == 988 && v34.first == 957) {
                        // cout << dist << endl;

                        ret.push_back(Congr_base(s12.p1, s12.p2, s34.p1, s34.p2));
                    }
                }
            }
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

    int ran1, ran2, ran3, ran4;
    double la, mu;
    Pt3D e;
    double angle; // cos(theta)

    bool found = test_coplanar(&mesh_q, &s_q, ran1, ran2, ran3, ran4, la, mu, e, angle);
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
    bq[0] = mesh_q.get_pt(ran1);
    bq[1] = mesh_q.get_pt(ran2);
    bq[2] = mesh_q.get_pt(ran3);
    bq[3] = mesh_q.get_pt(ran4);

    double d12_q = eucl_dist(&bq[0], &bq[1]);
    cout << "dist between " << ran1 << "(" << s_q.get_id_mapping(ran1) << ") and "
        					<< ran2 << "(" << s_q.get_id_mapping(ran2) << "): "
         << d12_q << " in query" << endl;

    double d34_q = eucl_dist(&bq[2], &bq[3]);
    cout << "dist between " << ran3 << "(" << s_q.get_id_mapping(ran3) << ") and "
        					<< ran4 << "(" << s_q.get_id_mapping(ran4) << "): "
         << d34_q << " in query" << endl;

    double epsilon = 0.1;

    double exec_user_time = 0.0, exec_prop_time = 0.0, exec_veri_time = 0.0, exec_retr_time = 0.0;
    int exec_veri_num = 0;

    int verified_size = 0;

    vector<Congr_base> ret;
    for (int i = 0; i < db_meshes.size(); i++) {
        ret.clear();
        // match_base_in_db_mesh(db_meshes.get_mesh(i), &o_trees[i], d12_q, d34_q, epsilon, la, mu, angle, ret);
        match_base_in_db_mesh(db_meshes.get_mesh(i), &db_rtrees[i], d12_q, d34_q, epsilon, la, mu, angle, ret);

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
