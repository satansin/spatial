#include "point.h"
#include "tetra_meas.h"
#include "pcr_adv.h"
#include "c_rtree.h"
#include "ProgressBar.hpp"
#include "util.h"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv) {

    srand(time(0));

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " query_filename_1 query_filename_1" << endl;
        exit(1);
    }
    
    string query_filename_1 = argv[1];
    string query_filename_2 = argv[2];

    Mesh mesh_q;
    cout << "Reading query mesh from " << query_filename_1 << endl;
    mesh_q.read_from_path(query_filename_1);

    // load the query R-tree
    C_RTree r_q;
    r_q.read_from_mesh(query_filename_1);

    Mesh mesh_q_2;
    cout << "Reading query mesh from " << query_filename_2 << endl;
    mesh_q_2.read_from_path(query_filename_2);
    
    cout << endl;

    PtwID pt_2256(2256, &mesh_q_2);
    PtwID pt_2102(2102, &mesh_q_2);
    PtwID pt_1191(1191, &mesh_q_2);
    PtwID pt_1978(1978, &mesh_q_2);

    int nn;
    PtwID nn_pt;

    PtwID nn_pt_1;
    PtwID nn_pt_2;

    r_q.nn_sphere(pt_2256.pt, 0.01, &nn);
    nn_pt.set(nn, &mesh_q);
    nn_pt_1.set(nn, &mesh_q);
    cout << "NN of 2256: " << nn << " with dist: " << eucl_dist(nn_pt.pt, pt_2256.pt) << endl;

    r_q.nn_sphere(pt_2102.pt, 0.01, &nn);
    nn_pt.set(nn, &mesh_q);
    nn_pt_2.set(nn, &mesh_q);
    cout << "NN of 2102: " << nn << " with dist: " << eucl_dist(nn_pt.pt, pt_2102.pt) << endl;

    r_q.nn_sphere(pt_1191.pt, 0.01, &nn);
    nn_pt.set(nn, &mesh_q);
    cout << "NN of 1191: " << nn << " with dist: " << eucl_dist(nn_pt.pt, pt_1191.pt) << endl;

    r_q.nn_sphere(pt_1978.pt, 0.01, &nn);
    nn_pt.set(nn, &mesh_q);
    cout << "NN of 1978: " << nn << " with dist: " << eucl_dist(nn_pt.pt, pt_1978.pt) << endl;

    cout << eucl_dist(nn_pt_1.pt, nn_pt_2.pt) << endl;


    // PtwID pt_97340(97340, &mesh_q_2);
    // PtwID pt_95769(95769, &mesh_q_2);
    // PtwID pt_89497(89497, &mesh_q_2);
    // PtwID pt_94990(94990, &mesh_q_2);

    // int nn;
    // PtwID nn_pt;

    // PtwID nn_pt_1;
    // PtwID nn_pt_2;

    // r_q.nn_sphere(pt_97340.pt, 0.01, &nn);
    // nn_pt.set(nn, &mesh_q);
    // nn_pt_1.set(nn, &mesh_q);
    // cout << "NN of 97340: " << nn << " with dist: " << eucl_dist(nn_pt.pt, pt_97340.pt) << endl;

    // r_q.nn_sphere(pt_95769.pt, 0.01, &nn);
    // nn_pt.set(nn, &mesh_q);
    // nn_pt_2.set(nn, &mesh_q);
    // cout << "NN of 95769: " << nn << " with dist: " << eucl_dist(nn_pt.pt, pt_95769.pt) << endl;

    // r_q.nn_sphere(pt_89497.pt, 0.01, &nn);
    // nn_pt.set(nn, &mesh_q);
    // cout << "NN of 89497: " << nn << " with dist: " << eucl_dist(nn_pt.pt, pt_89497.pt) << endl;

    // r_q.nn_sphere(pt_94990.pt, 0.01, &nn);
    // nn_pt.set(nn, &mesh_q);
    // cout << "NN of 94990: " << nn << " with dist: " << eucl_dist(nn_pt.pt, pt_94990.pt) << endl;

    // cout << eucl_dist(nn_pt_1.pt, nn_pt_2.pt) << endl;

}