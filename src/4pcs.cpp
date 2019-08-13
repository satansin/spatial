#include "TriMesh.h"
#include "Vec.h"
#include "point.h"
#include "trans.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
using namespace std;
using namespace trimesh;

const double ZERO = 0.1;
const double FINE_ZERO = 0.000001;
const int MAX_TRIAL = 1000;

inline Pt3D pt(point p) { return {p[0], p[1], p[2]}; };
inline bool eq(double a, double b) { return abs(a - b) <= ZERO; };
inline bool eqf(double a, double b) { return abs(a - b) <= FINE_ZERO; };

int main(int argc, char **argv) {
    srand(time(NULL));

	string database_filename = "../data/t2.ply";
	string query_filename = "../data/t2_query.ply";

	TriMesh *mesh_p = TriMesh::read(database_filename);
	TriMesh *mesh_q = TriMesh::read(query_filename);

    // int m = mesh_q->vertices.size();
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

    // string idmap_filename = "../data/t2_id";
    // ifstream idmap_ifs(idmap_filename);
    // int id_db;
    // vector<int> idmap;
    // while (idmap_ifs >> id_db) {
    //     idmap.push_back(id_db);
    // }

    // double d12_q = eucl_dist(bq1, bq2);
    // cout << "dist between " << ran1 << "(" << idmap[ran1] << ") and "
    //      << ran2 << "(" << idmap[ran2] << "): "
    //      << d12_q << " in query" << endl;

    // double d34_q = eucl_dist(bq3, bq4);
    // cout << "dist between " << ran3 << "(" << idmap[ran3] << ") and "
    //      << ran4 << "(" << idmap[ran4] << "): "
    //      << d34_q << " in query" << endl;

    // int n = mesh_p->vertices.size();
    // for (int i = 0; i < n; i++) {
    //     auto pi = pt(mesh_p->vertices[i]);
    //     for (int j = i + 1; j < n; j++) {
    //         auto pj = pt(mesh_p->vertices[j]);
    //         double dij_p = eucl_dist(pi, pj);
    //         if(eqf(dij_p, d12_q) || eqf(dij_p, d34_q)) {
    //             cout << "dist between " << i << " and " << j << ": "
    //                  << dij_p << " in database" << endl;
    //         }
    //     }
    // }

    cout << "Here we first use an example to test the transformation derivation:" << endl;
    Pt3D q[4], p[4];
    q[0] = pt(mesh_q->vertices[41]);
    q[1] = pt(mesh_q->vertices[368]);
    q[2] = pt(mesh_q->vertices[70]);
    q[3] = pt(mesh_q->vertices[405]);
    p[0] = pt(mesh_p->vertices[835]);
    p[1] = pt(mesh_p->vertices[5177]);
    p[2] = pt(mesh_p->vertices[1394]);
    p[3] = pt(mesh_p->vertices[5543]);
    Trans t = cal_trans(q, p, 4);
    print_trans(&t);
}