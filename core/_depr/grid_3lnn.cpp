#include "point.h"
#include "tetra_meas.h"
#include "pcr_adv.h"
#include "c_rtree.h"
#include "ProgressBar.hpp"
#include "util.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <vector>
#include <unordered_map>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;

struct Entry_Stat {
    int global_id;
    Entry_Stat(int id) {
        global_id = id;
        for (int i = 0; i < 3; i++) {
            num_discarded[i] = 0;
            nn_dist[i] = 0.0;
        }
    }

	int num_discarded[3];
	double nn_dist[3];
};

inline bool verify_angle(Pt3D* pa, Pt3D* pb, double cos_min_ang) {
    double ct = cos_theta(pa, pb);
    if (ct > cos_min_ang) {
        return false;
    } else {
        return true;
    }
}

// order: max -> mid -> min (c -> b -> a)
void cal_3lnn_entry(PtwID* p, double ann_min, double ann_mid, double ann_max, double cos_min_ang, Mesh* mesh_p, C_RTree* r_p, bool debug_mode, Entry* prem_entry, Entry_Stat& es) {

    int nn_c;
    r_p->nn_sphere(p->pt, ann_max, &nn_c);

    PtwID a, b, c;
    c = PtwID(nn_c, mesh_p);
    auto pc = *c.pt - *p->pt;

    const int TENT = 20;
    
    bool got_b = false;
    int nn_b[TENT];
    r_p->nn_sphere(p->pt, ann_mid, nn_b, {}, TENT);
    for (int i = 0; i < TENT; i++) {
        PtwID t = PtwID(nn_b[i], mesh_p);
        auto pt = *t.pt - *p->pt;
        if (pt.mode() >= ann_max) {
            break;
        }
        if (verify_angle(&pc, &pt, cos_min_ang)) {
            b = t;
            got_b = true;
            break;
        } else {
            es.num_discarded[1]++;
        }
    }
    if (!got_b) {
        return;
    }

    auto pb = *b.pt - *p->pt;
    
    bool got_a = false;
    int nn_a[TENT];
    r_p->nn_sphere(p->pt, ann_min, nn_a, {}, TENT);
    for (int i = 0; i < TENT; i++) {
        PtwID t = PtwID(nn_a[i], mesh_p);
        auto pt = *t.pt - *p->pt;
        if (pt.mode() >= ann_mid) {
            break;
        }
        if (verify_angle(&pc, &pt, cos_min_ang) && verify_angle(&pb, &pt, cos_min_ang)) {
            a = t;
            got_a = true;
            break;
        } else {
            es.num_discarded[0]++;
        }
    }
    if (!got_a) {
        return;
    }

    // get the ratio set
    auto ratio_set = get_ratio_set_vol(p->pt, a.pt, b.pt, c.pt);
    prem_entry->set(p, &a, &b, &c, ratio_set.volume, ratio_set.ratio);
    prem_entry->fail = false;

}

// order: min -> mid -> max
void cal_3lnn_entry_sim(PtwID* p, double ann_min, double ann_mid, double ann_max, Mesh* mesh_p, C_RTree* r_p, bool debug_mode, Entry* prem_entry, Entry_Stat& es) {

    int nn[3];
    r_p->nn_sphere(p->pt, ann_min, &nn[0]);
    r_p->nn_sphere(p->pt, ann_mid, &nn[1]);
    r_p->nn_sphere(p->pt, ann_max, &nn[2]);

    if (nn[0] == nn[1] || nn[0] == nn[2] || nn[1] == nn[2]) {
        return;
    }

    PtwID a, b, c;
    a = PtwID(nn[0], mesh_p);
    b = PtwID(nn[1], mesh_p);
    c = PtwID(nn[2], mesh_p);

    // get the ratio set
    auto ratio_set = get_ratio_set_vol(p->pt, a.pt, b.pt, c.pt);
    prem_entry->set(p, &a, &b, &c, ratio_set.volume, ratio_set.ratio);
    prem_entry->fail = false;

}

int main(int argc, char **argv) {

    srand(time(0));

    if (argc < 6) {
        cerr << "Usage: " << argv[0] << " database_path ann_mid ann_thn (ang_min) output_grid_filename [-simple] [-show_prog_bar] [-debug]" << endl;
        exit(1);
    }

    bool show_prog_bar = false, debug_mode = false, simple = false;
    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-show_prog_bar")
            show_prog_bar = true;
        else if (string(argv[i]) == "-debug")
            debug_mode = true;
        else if (string(argv[i]) == "-simple")
            simple = true;
    }

    int argi = 0;
    string db_path = argv[++argi];
    double w = 0.0;
    double ann_mid = atof(argv[++argi]);
    double ann_thn = atof(argv[++argi]);
    double ann_min = ann_mid - ann_thn;
    double ann_max = ann_mid + ann_thn;
    double ang_min = 0.0;
    if (!simple) {
        ang_min = atof(argv[++argi]);
    }
    string outgrid_filename = argv[++argi];

    cout << "Parameters:" << endl;
    cout << "db_path = " << db_path << endl;
    cout << "w = " << w << endl;
    cout << "ann_min = " << ann_min << endl;
    cout << "ann_mid = " << ann_mid << endl;
    cout << "ann_max = " << ann_max << endl;
    cout << "ang_min = " << ang_min << endl;
    cout << "outgrid_filename = " << outgrid_filename << endl;
    cout << endl;

    timer_start();

    cout << "Reading database files from " << db_path << endl;
    DB_Meshes db_meshes;
    int num_meshes = db_meshes.read_from_path(db_path);
    cout << "Total no. meshes: " << num_meshes << endl << endl;

    int n = db_meshes.total();

    cout << "Reading database R-trees..." << endl;
    vector<C_RTree> db_rtrees;
    read_rtrees_from_db_meshes(&db_meshes, db_rtrees);
    
    cout << endl;

    double i_time = timer_end(SECOND);

    cout << "Total no. pts: " << n << endl << endl;
    cout << "Total I/- time: " << i_time << "(s)" << endl;


    timer_start();

    ofstream ofs(outgrid_filename);

    vector<Entry_Stat> v_es;

    // write grid headers
    ofs << w << " " << ann_min << " " << ann_mid << " " << ann_max << " " << ang_min << " " << num_meshes << " " << db_meshes.total() << endl;

    for (int i = 0; i < num_meshes; i++) {
        ofs << i << " 0 0 0 0 0 0" << endl;
    }

    int fail_count = 0;
    int global_cell_id = 0;

    cout << setiosflags(ios::fixed) << setprecision(2);

    const double cos_phi = cos(ang_min * PI / 180.0);

    for (int mesh_id = 0; mesh_id < db_meshes.size(); mesh_id++) {
        auto mesh_p = db_meshes.get_mesh(mesh_id);
        auto r_p = db_rtrees[mesh_id];

        cout << "Processing mesh #" << mesh_id << endl << endl;

        ProgressBar bar(mesh_p->size(), 70);

        for (int i = 0; i < mesh_p->size(); i++) {
            if (show_prog_bar) {
                ++bar;
                bar.display();
            }

            PtwID p = PtwID(i, mesh_p);

            auto prem_entry = new Entry();
            prem_entry->fail = true;

            Entry_Stat es(global_cell_id);

            if (simple) {
                cal_3lnn_entry_sim(&p, ann_min, ann_mid, ann_max, mesh_p, &r_p, debug_mode, prem_entry, es);
            } else {
                cal_3lnn_entry(&p, ann_min, ann_mid, ann_max, cos_phi, mesh_p, &r_p, debug_mode, prem_entry, es);
            }

            if (prem_entry->fail) {
                if (debug_mode) cout << TAB << "Fail in finding prem entry" << endl;
                fail_count++;
            } else {
                if (debug_mode) cout << TAB << "Prem entry: " << prem_entry->to_str() << endl;

                // for edge index
                prem_entry->fill_sides();
            }

            // s_db.append_entry(prem_entry);

            ofs << mesh_id << " " << i << " " << global_cell_id << " 0 0 0 1 " << i << endl;
            ofs << mesh_id << " " << prem_entry->to_str(12) << endl;

            v_es.push_back(es);

            global_cell_id++;

        }

        if (show_prog_bar) {
            bar.done();
        }
    }

    ofs.close();

    double user_time = timer_end(SECOND);

    cout << resetiosflags(ios::fixed) << setprecision(6);

    cout << "Total # of failed cells: " << fail_count << endl << endl;

    cout << "Total user time: " << user_time << "(s)" << endl << endl;


    string outgrid_stat_filename = outgrid_filename + ".gstat";
    ofstream ofs_stat(outgrid_stat_filename);

    ofs_stat << "num_pts = " << n << endl;
    ofs_stat << "num_fail = " << fail_count << endl;
    ofs_stat << "I_time = " << i_time << endl;
    ofs_stat << "user_time = " << user_time << endl;

    if (!simple) {
        for (auto &es: v_es) {
            ofs_stat << es.global_id;
            for (int j = 0; j < 3; j++) {
                ofs_stat << " " << es.num_discarded[j];
            }
            ofs_stat << endl;
        }
    }

    ofs_stat.close();

}