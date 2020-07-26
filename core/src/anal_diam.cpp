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
        cerr << "Usage: " << argv[0] << " database_path(or ply_file -single)" << endl;
        exit(1);
    }

    bool single = false;
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
        if (argv_str == "-single") {
            single = true;
        }
    }

    string db_path = argv[1];
    cout << "Reading database files from " << db_path << endl;

    if (single) {
        Mesh mesh;
        mesh.read_from_path(db_path);
        cout << mesh.get_bsphere_d() << endl;
    } else {

        DB_Meshes db_meshes;
        int num_meshes = db_meshes.read_from_path(db_path);
        cout << "Total no. meshes: " << num_meshes << endl << endl;

        cout << endl;

        double sum = 0.0;

        for (int i = 0; i < num_meshes; i++) {
            // if (i > 99 && i % 100 != 0) {
            //     continue;
            // }
        	auto mesh = db_meshes.get_mesh(i);
            double diam = mesh->get_bsphere_d();
            cout << diam << endl;

            sum += diam;
        }

        cout << "Avg: " << (sum / (double) num_meshes) << endl;
    }

}