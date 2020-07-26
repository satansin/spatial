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
    }

	int num_discarded;
	double nn_dist[3];
};

bool verify_angle(vector<Pt3D>& accepted_pa, Pt3D* pb, double cos_min_ang) {
    for (auto &pa: accepted_pa) {
        double ct = cos_theta(&pa, pb);
        if (ct > cos_min_ang) {
            return false;
        }
    }
    return true;
}

void cal_3nn_entry(PtwID* p, double min, double cos_min_ang, Mesh* mesh_p, C_RTree* r_p, bool debug_mode, Entry* prem_entry, Entry_Stat& es) {

	const int TENT = 40;

    int nn[TENT];
    r_p->nn_sphere(p->pt, min, nn, {}, TENT);

    PtwID a, b, c;

    int accepted = 0, discarded = 0;
    vector<Pt3D> accepted_pa;

    a = PtwID(nn[0], mesh_p);
    accepted++;
    accepted_pa.push_back(*a.pt - *p->pt);

    for (int i = 1; i < TENT; i++) {
        if (accepted >= 3) {
            break;
        }
        PtwID t = PtwID(nn[i], mesh_p);
        auto pt = *t.pt - *p->pt;
        if (verify_angle(accepted_pa, &pt, cos_min_ang)) {
            switch (accepted) {
            case 1:
                b = t;
                accepted++;
                accepted_pa.push_back(pt);
                break;
            case 2:
                c = t;
                accepted++;
                accepted_pa.push_back(pt);
                break;
            default:
                break;
            }
        } else {
            discarded++;
        }
    }

    // cout << "# accepted: " << accepted << endl;
    // cout << "# discarded: " << discarded << endl;

    // cout << "pa = " << accepted_pa[0].mode() << endl;
    // cout << "pb = " << accepted_pa[1].mode() << endl;
    // cout << "pc = " << accepted_pa[2].mode() << endl;

    // auto cos_a0pa1 = dot_prd(&pa0, &pa1) / (pa0_mode * pa1_mode);
    // auto cos_a0pa2 = dot_prd(&pa0, &pa2) / (pa0_mode * pa2_mode);
    // auto cos_a1pa2 = dot_prd(&pa1, &pa2) / (pa1_mode * pa2_mode);

    // cout << "cos_a0pa1 = " << cos_a0pa1 << endl;
    // cout << "cos_a0pa2 = " << cos_a0pa2 << endl;
    // cout << "cos_a1pa2 = " << cos_a1pa2 << endl;


    if (accepted >= 3) {
        // get the ratio set
        auto ratio_set = get_ratio_set_vol(p->pt, a.pt, b.pt, c.pt);
        prem_entry->set(p, &a, &b, &c, ratio_set.volume, ratio_set.ratio);
        prem_entry->fail = false;
    }

    es.num_discarded = discarded;
    for (int i = 0; i < 3; i++) {
        if (i < accepted_pa.size()) {
            es.nn_dist[i] = accepted_pa[i].mode();
        } else {
            es.nn_dist[i] = 0;
        }
    }

}

void cal_3nn_entry_sim(PtwID* p, double min, Mesh* mesh_p, C_RTree* r_p, bool debug_mode, Entry* prem_entry, Entry_Stat& es) {

    int nn[3];
    r_p->nn_sphere(p->pt, min, nn, {}, 3);

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

    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " database_path ann_min (ang_min) output_grid_filename [-simple] [-show_prog_bar] [-debug]" << endl;
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
    double ann_min = atof(argv[++argi]);
    double ann_mid = 0.0;
    double ann_max = 0.0;
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
                cal_3nn_entry_sim(&p, ann_min, mesh_p, &r_p, debug_mode, prem_entry, es);
            } else {
                cal_3nn_entry(&p, ann_min, cos_phi, mesh_p, &r_p, debug_mode, prem_entry, es);
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

    // for (auto &es: v_es) {
    //     ofs_stat << es.global_id;
    //     ofs_stat << " " << es.num_discarded;
    //     for (int j = 0; j < 3; j++) {
    //         ofs_stat << " " << es.nn_dist[j];
    //     }
    //     ofs_stat << endl;
    // }

    ofs_stat.close();

}