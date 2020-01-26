#include <iostream>
#include <string>
#include <limits>
#include "pcr_adv.h"
#include "TriMesh.h"
using namespace std;
using namespace trimesh;

inline unsigned int get_bin(double min, double intv, int bin_size, double val) {
	int bin = (unsigned int) ((val - min) / intv);
	return (bin > (bin_size - 1) ? (bin_size - 1) : bin);
}

int main(int argc, char **argv) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " database_path index_filename bin_size" << endl;
        exit(1);
    }

    int argi = 0;
    string db_path = argv[(++argi)];
    string idx_filename = argv[(++argi)];
    const int bin_size = atoi(argv[(++argi)]);

    cout << "Reading database file..." << endl;

    vector<TriMesh*> db_meshes;

    int num_meshes = read_db_mesh_batch(db_path, db_meshes);

    // load the DB structure
    cout << "Loading DB structure..." << endl;
    Struct_DB s_db;
    s_db.read(idx_filename, db_meshes);

    auto db_entries = s_db.get_entries();

    cout << "Start analyzing..." << endl << endl;

    cout << "Size of entries: " << db_entries.size() << endl;

    // initialization of min & max vols
    double min_vol = std::numeric_limits<double>::max(), max_vol = -1;
    // initialization of min & max meas
    double min_meas = 0, max_meas = 1;
    // initialization of min & max side, variance 80 side, average side, and variance side
    double min_side[6], max_side[6], var_80_side[6], avg_side[6], var_side[6];
    for (int i = 0; i < 6; i++) {
        min_side[i] = std::numeric_limits<double>::max();
        max_side[i] = -1;
        var_80_side[i] = 0;
        avg_side[i] = 0;
        var_side[i] = 0;
    }

    // First pass: calculate min & max, var 80, avg
    for (auto &v: db_entries) {
    	if (v->fail)
    		continue;

    	if (v->vol < min_vol)
    		min_vol = v->vol;
    	if (v->vol > max_vol)
    		max_vol = v->vol;

        for (int i = 0; i < 6; i++) {
            if (v->sides[i] < min_side[i])
                min_side[i] = v->sides[i];
            if (v->sides[i] > max_side[i])
                max_side[i] = v->sides[i];

            var_80_side[i] += (v->sides[i] - 80) * (v->sides[i] - 80) / db_entries.size();

            avg_side[i] += v->sides[i] / db_entries.size();
        }
    }

    // print out min & max, var 80, avg
    cout << "Max vol: " << max_vol << endl;
    cout << "Min vol: " << min_vol << endl;
    cout << "Max meas: " << max_meas << endl;
    cout << "Min meas: " << min_meas << endl;
    for (int i = 0; i < 6; i++) {
        cout << "Max side[" << i << "]: " << max_side[i] << endl;
        cout << "Min side[" << i << "]: " << min_side[i] << endl;
        cout << "Var 80 side[" << i << "]: " <<var_80_side[i] << endl;
        cout << "Avg side[" << i << "]: " <<avg_side[i] << endl;
    }

    // initialization of intervals, histograms
    const double intv_vol = (max_vol - min_vol) / bin_size;
    const double intv_meas = 1.0 / bin_size;
    double intv_side[6];
    for (int i = 0; i < 6; i++) {
        intv_side[i] = (max_side[i] - min_side[i]) / bin_size;
    }
    int histo_vol[bin_size], histo_meas[bin_size], histo_side[6][bin_size];
    for (int i = 0; i < bin_size; i++) {
    	histo_vol[i] = 0;
    	histo_meas[i] = 0;
        for (int j = 0; j < 6; j++) {
            histo_side[j][i] = 0;
        }
    }

    // Second pass: calculate histograms, variance of sides
    for (auto &v: db_entries) {
    	if (v->fail)
    		continue;

    	histo_vol[get_bin(min_vol, intv_vol, bin_size, v->vol)]++;
    	histo_meas[get_bin(min_meas, intv_meas, bin_size, v->meas)]++;

        for (int i = 0; i < 6; i++) {
            histo_side[i][get_bin(min_side[i], intv_side[i], bin_size, v->sides[i])]++;

            var_side[i] += (v->sides[i] - avg_side[i]) * (v->sides[i] - avg_side[i]) / db_entries.size();
        }
    }

    // print out variance of sides
    for (int i = 0; i < 6; i++) {
        cout << "Var side[" << i << "]: " <<var_side[i] << endl;
    }

    // print out histograms
    cout << "Histogram of vol: (in " << bin_size << " bins):" << endl;
    for (int i = 0; i < bin_size; i++) {
    	cout << "[" << (min_vol + i * intv_vol) << ", " << (min_vol + (i + 1) * intv_vol);

    	if (i < bin_size - 1)
    		cout << ")";
    	else
    		cout << "]";
    		
    	cout << ": " << histo_vol[i] << endl;
    }

    cout << endl;

    cout << "Histogram of meas: (in " << bin_size << " bins):" << endl;
    for (int i = 0; i < bin_size; i++) {
    	cout << "[" << (min_meas + i * intv_meas) << ", " << (min_meas + (i + 1) * intv_meas);

    	if (i < bin_size - 1)
    		cout << ")";
    	else
    		cout << "]";
    		
    	cout << ": " << histo_meas[i] << endl;
    }

    for (int i = 0; i < 6; i++) {
        cout << endl;

        cout << "Histogram of side[" << i << "]: (in " << bin_size << " bins):" << endl;
        for (int j = 0; j < bin_size; j++) {
            cout << "[" << (min_side[i] + j * intv_side[i]) << ", " << (min_side[i] + (j + 1) * intv_side[i]);

            if (j < bin_size - 1)
                cout << ")";
            else
                cout << "]";

            cout << ": " << histo_side[i][j] << endl;
        }
    }

}