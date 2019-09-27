#include "TriMesh.h"
#include "Vec.h"
#include "point.h"
#include "trans.h"
#include "share.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
using namespace std;
using namespace trimesh;

struct Pred_coin {
	Pt3D e;
	int endpt1, endpt2;
};

const double ZERO = 0.1;
const double FINE_ZERO = 0.000001;
const int MAX_TRIAL = 1000;

inline bool eq(double a, double b) { return abs(a - b) <= ZERO; };
inline bool eqf(double a, double b) { return abs(a - b) <= FINE_ZERO; };

int main(int argc, char **argv) {
    srand(time(NULL));

	string database_filename = "../data/t2.ply";
	string query_filename = "../data/t2_query.ply";

	TriMesh *mesh_p = TriMesh::read(database_filename);
	TriMesh *mesh_q = TriMesh::read(query_filename);

    int m = mesh_q->vertices.size();
    int n = mesh_p->vertices.size();
    Pt3D pts_q[m], pts_p[n];
    for (int i = 0; i < m; i++) {
    	pts_q[i] = pt(mesh_q->vertices[i]);
    }
    for (int i = 0; i < n; i++) {
    	pts_p[i] = pt(mesh_p->vertices[i]);
    }


    // int ran1, ran2, ran3, ran4;
    // Pt3D bq1, bq2, bq3, bq4;

    // int t = 0;
    // bool found_coplanar = false;
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
    //     bq1 = pt(mesh_q->vertices[ran1]);
    //     bq2 = pt(mesh_q->vertices[ran2]);
    //     bq3 = pt(mesh_q->vertices[ran3]);

    //     // calculate the equation plane
    //     int a1 = bq2.x - bq1.x;
    //     int b1 = bq2.y - bq1.y;
    //     int c1 = bq2.z - bq1.z;
    //     int a2 = bq3.x - bq1.x;
    //     int b2 = bq3.y - bq1.y;
    //     int c2 = bq3.z - bq1.z;
    //     int a = b1 * c2 - b2 * c1;
    //     int b = a2 * c1 - a1 * c2;
    //     int c = a1 * b2 - b1 * a2;
    //     int d = (- a * bq1.x - b * bq1.y - c * bq1.z);

    //     // look for the fourth point
    //     for (int i = 0; i < m; i++) {
    //         if (i == ran1 || i == ran2 || i == ran3) // skip the repeats
    //             continue;

    //         bq4 = pt(mesh_q->vertices[i]);
    //         if(eq(a * bq4.x + b * bq4.y + c * bq4.z + d, 0)) {
    //             found_coplanar = true;
    //             ran4 = i;
    //             break;
    //         }
    //     }
    // }

    // if (!found_coplanar) {
    //     cerr << "coplanar not found" << endl;
    //     exit(1);
    // } else {
    //     cout << "coplanar found: "
    //          << ran1 << " " << ran2 << " "
    //          << ran3 << " " << ran4 << endl;
    // }

    string idmap_filename = "../data/t2_id";
    ifstream idmap_ifs(idmap_filename);
    int id_db;
    vector<int> idmap;
    while (idmap_ifs >> id_db) {
        idmap.push_back(id_db);
    }


    int ran1 = 234, ran2 = 152, ran3 = 18, ran4 = 391;
    Pt3D q[4], p[4];
    q[0] = pts_q[ran1];
    q[1] = pts_q[ran2];
    q[2] = pts_q[ran3];
    q[3] = pts_q[ran4];

    double d12_q = eucl_dist(q[0], q[1]);
    cout << "dist between " << ran1 << "(" << idmap[ran1] << ") and "
         << ran2 << "(" << idmap[ran2] << "): "
         << d12_q << " in query" << endl;

    double d34_q = eucl_dist(q[2], q[3]);
    cout << "dist between " << ran3 << "(" << idmap[ran3] << ") and "
         << ran4 << "(" << idmap[ran4] << "): "
         << d34_q << " in query" << endl;

    Pt3D q12 = {q[1].x - q[0].x, q[1].y - q[0].y, q[1].z - q[0].z};
    Pt3D q34 = {q[3].x - q[2].x, q[3].y - q[2].y, q[3].z - q[2].z};
    double r34 =
    	(q12.y * q[0].x - q12.x * q[0].y + q12.x * q[2].y - q12.y * q[2].x) / 
    	(q34.x * q12.y - q34.y * q12.x);
    double r12 = (q[2].x + r34 * q34.x - q[0].x) / q12.x;
    // cout << r12 << endl << r34 << endl;

    vector<Pred_coin> pred_coin12;
    vector<Pred_coin> pred_coin34;
    for (int i = 0; i < n; i++) {
        Pt3D pi = pts_p[i];
        for (int j = i + 1; j < n; j++) {
            Pt3D pj = pts_p[j];
            double dij_p = eucl_dist(pi, pj);
            if(abs(dij_p - d12_q) <= 0.2) {
                pred_coin12.push_back({
                	{ pi.x + (pj.x - pi.x) * r12,
                	  pi.y + (pj.y - pi.y) * r12,
                	  pi.z + (pj.z - pi.z) * r12,
                	}, i, j});
                pred_coin12.push_back({
                	{ pj.x + (pi.x - pj.x) * r12,
                	  pj.y + (pi.y - pj.y) * r12,
                	  pj.z + (pi.z - pj.z) * r12,
                	}, j, i});
            }
            if (abs(dij_p - d34_q) <= 0.2) {
                pred_coin34.push_back({
                	{ pi.x + (pj.x - pi.x) * r34,
                	  pi.y + (pj.y - pi.y) * r34,
                	  pi.z + (pj.z - pi.z) * r34,
                	}, i, j});
                pred_coin34.push_back({
                	{ pj.x + (pi.x - pj.x) * r34,
                	  pj.y + (pi.y - pj.y) * r34,
                	  pj.z + (pi.z - pj.z) * r34,
                	}, j, i});
            }
            // if (i == 3411 && j == 4079) {
            // 	cout << pred_coin12[pred_coin12.size() - 2].endpt1 << " ";
            // 	cout << pred_coin12[pred_coin12.size() - 2].endpt2 << " ";
            // 	print_pt(&pred_coin12[pred_coin12.size() - 2].e);
            // 	cout << endl;
            // 	cout << pred_coin12[pred_coin12.size() - 1].endpt1 << " ";
            // 	cout << pred_coin12[pred_coin12.size() - 1].endpt2 << " ";
            // 	print_pt(&pred_coin12[pred_coin12.size() - 1].e);
            // 	cout << endl;
            // }
            // if (i == 296 && j == 5374) {
            // 	cout << pred_coin34[pred_coin34.size() - 2].endpt1 << " ";
            // 	cout << pred_coin34[pred_coin34.size() - 2].endpt2 << " ";
            // 	print_pt(&pred_coin34[pred_coin34.size() - 2].e);
            // 	cout << endl;
            // 	cout << pred_coin34[pred_coin34.size() - 1].endpt1 << " ";
            // 	cout << pred_coin34[pred_coin34.size() - 1].endpt2 << " ";
            // 	print_pt(&pred_coin34[pred_coin34.size() - 1].e);
            // 	cout << endl;
            // }
        }
    }

    int no_e12 = pred_coin12.size();
    int no_e34 = pred_coin34.size();
    cout << "matching set of q1q2: " << no_e12 << endl
    	 << "matching set of q3q4: " << no_e34 << endl;

    double d13_q = eucl_dist(q[0], q[2]);
    int sta = 0;
    for (int i = 0; i < no_e12; i++) {
    	for (int j = 0; j < no_e34; j++) {
    		if (eucl_dist(pred_coin12[i].e, pred_coin34[j].e) <= 0.5) {
    			double d13_p = eucl_dist(pts_p[pred_coin12[i].endpt1], pts_p[pred_coin34[j].endpt1]);
    			if (abs(d13_p - d13_q) <= 0.2) {
    				sta++;
	    			// cout << "coin found: "
	    			// 	 << pred_coin12[i].endpt1 << "-" << pred_coin12[i].endpt2
	    			// 	 << " and "
	    			// 	 << pred_coin34[j].endpt1 << "-" << pred_coin34[j].endpt2 << endl;
	    			p[0] = pts_p[pred_coin12[i].endpt1];
	    			p[1] = pts_p[pred_coin12[i].endpt2];
	    			p[2] = pts_p[pred_coin34[j].endpt1];
	    			p[3] = pts_p[pred_coin34[j].endpt2];

				    Trans xf = cal_trans(q, p, 4);
				    // print_trans(&xf);

                    // cout << cal_err(pts_q, m, pts_p, n, xf) << endl;
    			}
    		}
    	}
    }

    // cout << sta << endl;

}