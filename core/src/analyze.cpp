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
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " database_filename db_rstree_filename index_filename bin_size" << endl;
        exit(1);
    }

    int argi = 0;
    string database_filename = argv[(++argi)];
    string db_rstree_filename = argv[(++argi)];
    string idx_filename = argv[(++argi)];
    const int bin_size = atoi(argv[(++argi)]);

    cout << "Reading dababase file..." << endl;
    TriMesh *mesh_p = TriMesh::read(database_filename);

    // load the DB structure
    cout << "Loading DB structure..." << endl;
    Struct_DB s_db;
    s_db.g_db = new Grid();
    s_db.read(idx_filename + ".grid", mesh_p);

    cout << "Start analyzing..." << endl << endl;

    cout << "Size of entries: " << s_db.entries_map.size() << endl;

    // get min & max vols
    double min_vol = std::numeric_limits<double>::max(), max_vol = -1;
    double min_meas = 0, max_meas = 1;
    double min_side[6] = { std::numeric_limits<double>::max() }, max_side[6] = { -1 };
    for (auto &v: s_db.entries_map) {
    	if (v.second->fail)
    		continue;

    	if (v.second->vol < min_vol)
    		min_vol = v.second->vol;
    	if (v.second->vol > max_vol)
    		max_vol = v.second->vol;

        for (int i = 0; i < 6; i++) {
            if (v.second->sides[i] < min_side[i])
                min_side[i] = v.second->sides[i];
            if (v.second->sides[i] > max_side[i])
                max_side[i] = v.second->sides[i];
        }
    }
    cout << "Max vol: " << max_vol << endl;
    cout << "Min vol: " << min_vol << endl;
    cout << "Max meas: " << max_meas << endl;
    cout << "Min meas: " << min_meas << endl;
    for (int i = 0; i < 6; i++) {
        cout << "Max side[" << i << "]: " << max_side[i] << endl;
        cout << "Min side[" << i << "]: " << min_side[i] << endl;
    }

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

    for (auto &v: s_db.entries_map) {
    	if (v.second->fail)
    		continue;

    	histo_vol[get_bin(min_vol, intv_vol, bin_size, v.second->vol)]++;
    	histo_meas[get_bin(min_meas, intv_meas, bin_size, v.second->meas)]++;

        for (int i = 0; i < 6; i++) {
            histo_side[i][get_bin(min_side[i], intv_side[i], bin_size, v.second->sides[i])]++;
        }
    }

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