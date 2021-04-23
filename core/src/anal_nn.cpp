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

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " database_path grid_filename [-num_parts=...]" << endl;
        exit(1);
    }
    
    int num_parts = 0;
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
        if (argv_str.rfind("-num_parts", 0) == 0)
            num_parts = atoi(argv[i] + 11);
    }

    string db_path = argv[1];
    string grid_filename = argv[2];

    cout << "Reading database files from " << db_path << endl;
    DB_Meshes db_meshes;
    int num_meshes = db_meshes.read_from_path(db_path);
    cout << "Total no. meshes: " << num_meshes << endl << endl;

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

    // load the DB structure
    string grid_bin_filename = get_bin_filename(grid_filename);
    cout << "Loading DB structure from " << grid_bin_filename << endl;
    Struct_DB s_db;
    if (!s_db.read_bin(grid_bin_filename, &db_meshes)) {
        cerr << "Error loading DB structure" << endl;
        exit(1);
    }
    
    cout << endl;

    // int num_cals = 0;
    // double sum_nn = 0.0;

    // for (int i = 0; i < num_meshes; i++) {
    //     // if (i > 99 && i % 100 != 0) {
    //     //     continue;
    //     // }
    // 	auto mesh = db_meshes.get_mesh(i);
    //     int num_sampling = 1000;
    //     if (mesh->size() < 1000) {
    //         num_sampling = 10;
    //     }
    // 	for (int j = 0; j < num_sampling; j++) {
    // 		int random = rand() % mesh->size();

    // 		PtwID rand_pt(random, mesh);
    // 		int nn;
    // 		db_rtrees[i]->nn_sphere(rand_pt.pt, 0.01, &nn);
    // 		PtwID nn_pt(nn, mesh);

    // 		num_cals++;
    // 		sum_nn += eucl_dist(rand_pt.pt, nn_pt.pt);
    // 	}
    // }

    // double avg_nn = sum_nn / (double) num_cals;
    // cout << "Avg NN dist: " << avg_nn << endl;

    auto entries = s_db.get_entries();
    int entry_base = 0;

    for (int i = 0; i < num_meshes; i++) {
        auto mesh = db_meshes.get_mesh(i);
        int overlapping_p2 = 0;

        for (int j = 0; j < mesh->size(); j++) {
            auto pt_j = mesh->get_pt(j);
            int nn_j;
            db_rtrees[i]->nn_sphere(pt_j, 0.01, &nn_j);

            auto entry_j = entries[entry_base + j];
            auto entry_nn_j = entries[entry_base + nn_j];

            if (entry_j->remai[0]->id == entry_nn_j->remai[0]->id) {
                overlapping_p2++;
            }
        }

        cout << "Mesh #" << i << ": found overlapping p2 " << overlapping_p2 << endl;

        entry_base += mesh->size();
    }
}