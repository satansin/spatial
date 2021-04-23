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

    // load the DB structure
    Struct_DB s_db;
    string grid_bin_filename = get_bin_filename(grid_filename);
    cout << "Loading DB structure from " << grid_bin_filename << endl;
    if (!s_db.read_bin(grid_bin_filename, &db_meshes)) {
        cerr << "Error loading DB structure" << endl;
        return false;
    }

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
    int real_size = 0;

    for (int i = 0; i < num_cells; i++) {

        auto e = entries[i];
        if (e->fail) {
            continue;
        }

        real_size++;

        double side_sum = 0.0;
        for (int j = 0; j < 6; j++) {
            side_sum += e->sides[j];
            // cout << side_sum << endl;
        }
        const double side_avg = side_sum / 6.0;
        // cout << "side_avg: " << side_avg << endl;

        double variance = 0.0;
        for (int j = 0; j < 6; j++) {
            variance += (e->sides[j] - side_avg) * (e->sides[j] - side_avg);
        }
        variance /= 6.0;

        const double sd = sqrt(variance);
        // cout << "sd: " << sd << endl;

        sum += sd / side_avg;

    }

    double mean_sd_ratio = sum / real_size;

    string sd_ratio_stat_filename = grid_filename + ".rstat";

    cout << "Finish analyzing, writing result to " << sd_ratio_stat_filename << endl;

    ofstream ofs_stat(sd_ratio_stat_filename);
    ofs_stat << "avg_sd_ratio=" << mean_sd_ratio << endl;
    ofs_stat.close();

}