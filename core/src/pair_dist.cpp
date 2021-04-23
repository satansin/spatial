#include "point.h"
#include "tetra_meas.h"
#include "pcr_adv.h"
#include "c_rtree.h"
#include "ProgressBar.hpp"
#include "util.h"
#include "pair_dist.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv) {

    srand(time(0));

    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " database_path outgrid_filename bin_size [-show_prog_bar] [-debug]" << endl;
        exit(1);
    }

    bool show_prog_bar = false, debug_mode = false;
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
             if (argv_str.rfind("-show_prog_bar", 0) == 0) show_prog_bar = true;
        else if (argv_str.rfind("-debug", 0) == 0)         debug_mode = true;
    }

    int argi = 0;
    const string db_path = argv[++argi];
    const string outgrid_filename = argv[++argi];
    const double bin_size = atof(argv[++argi]);

    cout << "Parameters:" << endl;
    cout << "db_path = " << db_path << endl;
    cout << "bin_size = " << bin_size << endl;
    cout << "outgrid_filename = " << outgrid_filename << endl;
    cout << endl;

    timer_start();

    cout << "Reading database files from " << db_path << endl;
    DB_Meshes db_meshes;
    const int num_meshes = db_meshes.read_from_path(db_path);
    if (num_meshes < 0) {
        cerr << "Error reading database files" << endl;
        exit(1);
    }
    cout << "Total no. meshes: " << num_meshes << endl;

    const int n = db_meshes.total();
    cout << "Total points: " << n << endl << endl;

    cout << endl;

    const double i_time = timer_end(SECOND);

    cout << "Total no. pts: " << n << endl << endl;
    cout << "Total I/- time: " << i_time << "(s)" << endl;


    timer_start();

    PtwID p_i, p_j;
    Mesh* mesh_p;
    ProgressBar* bar;
    int i, j;
    Pair_Dist_Hash pd_hash(bin_size);

    for (int mesh_id = 0; mesh_id < num_meshes; mesh_id++) {
        mesh_p = db_meshes.get_mesh(mesh_id);

        if (num_meshes < 1000 || (num_meshes >= 1000 && mesh_id % 1000 == 0)) {
            cout << "Processing mesh #" << mesh_id << endl << endl;
        }

        if (show_prog_bar) {    
            bar = new ProgressBar(mesh_p->size(), 70);
        }

        int mesh_p_size = mesh_p->size();
        long long total_pairs = ((long long) mesh_p_size) * (mesh_p_size - 1) / 2;
        long long counter = 0;

        for (i = 0; i < mesh_p_size; i++) {
        	for (j = i + 1; j < mesh_p_size; j++) {
	            if (show_prog_bar) {
	                ++(*bar);
	                bar->display();
	            }
	            counter++;
	            if (total_pairs >= 1e8 && (counter % (total_pairs / 100) == 0)) {
	            	cout << (counter / (total_pairs / 100)) << "%" << endl;
	            }

            	p_i.set(i, mesh_p);
            	p_j.set(j, mesh_p);
            	double dist = eucl_dist(p_i.pt, p_j.pt);

            	pd_hash.insert(mesh_id, p_i.id, p_j.id, dist);
            }
        }

        if (debug_mode) {
            break;
        }

        if (show_prog_bar) {
            bar->done();
            delete bar;
        }
    }

    // pd_hash.write(outgrid_filename);
    pd_hash.write_bin(outgrid_filename);

    const double user_time = timer_end(SECOND);

    cout << "Total user time: " << user_time << "(s)" << endl;
    cout << "Total num bins: " << pd_hash.m_bin_map.size() << endl;
    cout << endl;


    string outgrid_stat_filename = outgrid_filename + ".gstat";
    ofstream ofs_stat(outgrid_stat_filename);

    ofs_stat << "I_time=" << i_time << endl;
    ofs_stat << "user_time=" << user_time << endl;

    ofs_stat.close();

}