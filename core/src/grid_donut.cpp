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
	
	double nn_dist[3];
};

double donut_nn(Pt3D* o, Pt3D* n, double r, PtwID* p, Mesh* mesh_p, C_RTree* r_p, PtwID& ret) {

    vector<int> donut_nn_cand;
    r_p->range_sphere_min_max(o, r * 0.9, r * 1.1, donut_nn_cand, { p->id }); // exclude p, do not need to exclude a here since a will definitely be pruned next
    // cout << donut_nn_cand.size() << endl;

    double sq_r = sq(r);

    int donut_nn_i;
    double sq_d_min = numeric_limits<double>::max();
    for (auto &i: donut_nn_cand) {
        if (i < 0 || i >= mesh_p->size()) {
            continue;
        }
    	auto i_pt = PtwID(i, mesh_p);

    	auto pi = *i_pt.pt - *p->pt;
    	if (cos_theta(n, &pi) > 0.5) {
    		continue;
    	}

    	double sq_a = sq_dist(i_pt.pt, o);
    	double sq_n = n->sq_mode();
    	double lambda = (dot_prd(o, n) - dot_prd(i_pt.pt, n)) / sq_n;
    	double sq_h = sq(lambda) * sq_n;

    	double sq_d = sq_a + sq_r - 2.0 * r * sqrt(sq_a - sq_h);

    	if (sq_d < sq_d_min) {
    		sq_d_min = sq_d;
    		donut_nn_i = i;
    		ret = i_pt;
    	}
    }

    return sqrt(sq_d_min);

}

void cal_donut_entry(PtwID* p, double min, Mesh* mesh_p, C_RTree* r_p, bool debug_mode, Entry* prem_entry, Entry_Stat& es) {

	PtwID a, b, c;

    if (debug_mode) cout << TAB << "Pt #" << p->id << endl;

    // start looking for the first subsidiary pt a
    if (debug_mode) timer_start();

    int nn_a;
    auto nn_d_a = r_p->nn_sphere(p->pt, min, &nn_a);

    if (debug_mode) cout << TABTAB << "First pt #" << nn_a << " dist=" << nn_d_a << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    if (nn_a >= 0) {
     	a.set(nn_a, mesh_p);
     	es.nn_dist[0] = nn_d_a;
    } else {
    	return;
    }

    // start looking for the second pt b
    if (debug_mode) timer_start();

    Pt3D m; middle_pt(p->pt, a.pt, m);
    auto pa = *p->pt - *a.pt;

    double d_pm = eucl_dist(p->pt, &m);

    double r_donut = d_pm * 1.73205080757;
    double donut_nn_dist = donut_nn(&m, &pa, r_donut, p, mesh_p, r_p, b);
    es.nn_dist[1] = donut_nn_dist;

    if (!b.pt) {
        return;
    }

    if (debug_mode) cout << TABTAB << "Second pt #" << b.id << " dist=" << donut_nn_dist << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    // start looking for the third pt c
    if (debug_mode) timer_start();

    Pt3D b_est, c_est; // but b_est will not be used
    auto got_b_c = get_est_b_c(&m, a.pt, b.pt, b_est, c_est);
    if (!got_b_c) {
    	return;
    }

    int nn_c = -1;
    auto nn_d_c = r_p->nn_sphere(&c_est, 0.0, &nn_c, { p->id, a.id, b.id });
    if (nn_c >= 0 && nn_c < mesh_p->size()) {
     	c = PtwID(nn_c, mesh_p);
    	es.nn_dist[2] = nn_d_c;
    } else {
    	return;
    }

    if (debug_mode) cout << TABTAB << "Third pt #" << nn_c << " dist=" << nn_d_c << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    // get the ratio set
    auto ratio_set = get_ratio_set_vol(p->pt, a.pt, b.pt, c.pt);

    if (ratio_set.ratio - prem_entry->meas > 0) {
    	prem_entry->set(p, &a, &b, &c, ratio_set.volume, ratio_set.ratio);
    	prem_entry->fail = false;
    }

}

int main(int argc, char **argv) {

    srand(time(0));

    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " database_path ann_min output_grid_filename [-sort_entry] [-show_prog_bar] [-debug]" << endl;
        exit(1);
    }

    bool show_prog_bar = false, debug_mode = false, sort_entry = false;
    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-show_prog_bar")
            show_prog_bar = true;
        else if (string(argv[i]) == "-debug")
            debug_mode = true;
        else if (string(argv[i]) == "-sort_entry")
            sort_entry = true;
    }

    int argi = 0;
    string db_path = argv[++argi];
    double w = 0.0;
    double ann_min = atof(argv[++argi]);
    double ann_mid = 0.0;
    double ann_max = 0.0;
    double ang_min = 0.0;
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

            cal_donut_entry(&p, ann_min, mesh_p, &r_p, debug_mode, prem_entry, es);

            if (prem_entry->fail) {
                if (debug_mode) cout << TAB << "Fail in finding prem entry" << endl;
                fail_count++;
            } else {
                if (debug_mode) cout << TAB << "Prem entry: " << prem_entry->to_str() << endl;

                // for edge index
                prem_entry->fill_sides();
                if (sort_entry) {
                    prem_entry->sort_sides();
                }
            }

            // s_db.append_entry(prem_entry);

            ofs << mesh_id << " " << i << " " << global_cell_id << " 0 0 0 1 " << i << endl;
            ofs << mesh_id << " " << prem_entry->to_str(12) << endl;

            v_es.push_back(es);

            global_cell_id++;

            if (debug_mode && i >= 20) {
                break;
            }

        }

        if (show_prog_bar) {
            bar.done();
        }
    }

    ofs.close();

    double user_time = timer_end(SECOND);

    cout << resetiosflags(ios::fixed) << setprecision(6);

    cout << "Total # of failed cells: " << fail_count << endl << endl;

    cout << "Total user time: " << timer_end(SECOND) << "(s)" << endl << endl;


    string outgrid_stat_filename = outgrid_filename + ".gstat";
    ofstream ofs_stat(outgrid_stat_filename);

    ofs_stat << "num_pts = " << n << endl;
    ofs_stat << "num_fail = " << fail_count << endl;
    ofs_stat << "I_time = " << i_time << endl;
    ofs_stat << "user_time = " << user_time << endl;

    // for (auto &es: v_es) {
    //     ofs_stat << es.global_id;
    //     for (int j = 0; j < 3; j++) {
    //         ofs_stat << " " << es.nn_dist[j];
    //     }
    //     ofs_stat << endl;
    // }

    ofs_stat.close();

}