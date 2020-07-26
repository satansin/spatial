#include "pcr_adv.h"
#include "util.h"
#include <iostream>
using namespace std;

int main(int argc, char **argv) {

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " database_path grid_filename" << endl;
        exit(1);
    }

    int argi = 0;
    string db_path = argv[(++argi)];
    string grid_filename = argv[(++argi)];

    cout << "Parameters:" << endl;
    cout << "db_path = " << db_path << endl;
    cout << "grid_filename = " << grid_filename << endl;
    cout << endl;

    cout << "Reading database files from " << db_path << endl;
    DB_Meshes db_meshes;
    int num_meshes = db_meshes.read_from_path(db_path);
    cout << "Total no. meshes: " << num_meshes << endl << endl;

    cout << "Loading DB structure from " << grid_filename << endl;
    Struct_DB s_db;
    s_db.read(grid_filename, &db_meshes);

    int num_cells = s_db.get_total_cells_count();
    cout << "Total no. cells: " << num_cells << endl << endl;

    cout << "DB index parameters:" << endl;
    cout << "w = " << s_db.w << endl;
    cout << "ann_min = " << s_db.ann_min << endl;
    cout << "ann_mid = " << s_db.ann_mid << endl;
    cout << "ann_max = " << s_db.ann_max << endl;
    cout << "ang_min = " << s_db.ang_min << endl;

    auto entries = s_db.get_entries();

    cout << "Start analyzing the regularity" << endl;

    double sum = 0.0;
    const int NUM_BINS = 100;
    int histo[NUM_BINS] = { 0 };

    for (int i = 0; i < num_cells; i++) {

        auto e = entries[i];
        if (e->fail) {
            continue;
        }

        sum += e->meas;

        int hit = (int) (e->meas * NUM_BINS);
        if (hit >= 0 && hit < NUM_BINS) {
            histo[hit]++;
        }

    }

    double mean = sum / num_cells;

    double histo_d[NUM_BINS];
    double histo_sum = 0.0;
    for (int i = 0; i < NUM_BINS; i++) {
        histo_d[i] = (double) histo[i] / (double) num_cells * 100.0;
        // cout << histo_d[i] << endl;
        histo_sum += histo_d[i];
    }

    double histo_avg = histo_sum / num_cells;
    double histo_var = 0.0;
    for (int i = 0; i < NUM_BINS; i++) {
        histo_var += (histo_d[i] - histo_avg) * (histo_d[i] - histo_avg);
    }
    histo_var /= NUM_BINS;

    string reg_stat_filename = grid_filename + ".rstat";

    cout << "Finish analyzing, writing mean and histogram to " << reg_stat_filename << endl;

    ofstream ofs_stat(reg_stat_filename);
    ofs_stat << "mean=" << mean << endl;
    ofs_stat << "num_bins=" << NUM_BINS << endl;
    ofs_stat << "histo_var=" << histo_var << endl;
    for (int i = 0; i < NUM_BINS; i++) {
        ofs_stat << histo_d[i] << endl;
    }

    ofs_stat.close();

}