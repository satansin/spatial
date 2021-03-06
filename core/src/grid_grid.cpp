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

void cal_index_entry(PtwID* p, double min, const Mesh* mesh_p, C_RTree* r_p, bool debug_mode, Entry* prem_entry) {

	PtwID a, b, c, h;

    if (debug_mode) cout << TAB << "Pt #" << p->id << endl;

    // start looking for the first subsidiary pt a
    // if (debug_mode) timer_start();

    int nn_a;
    auto nn_d_a = r_p->nn_sphere(&(p->pt), min, &nn_a);

    // if (debug_mode) cout << TABTAB << "First pt #" << nn_a << " dist=" << nn_d_a << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    if (nn_a >= 0) {
     	a = PtwID(nn_a, mesh_p);
    } else {
    	return;
    }

    // start looking for the help pt h
    // if (debug_mode) timer_start();

    auto m = middle_pt(&p->pt, &a.pt);

    double d_pm = eucl_dist(&p->pt, &m);
    int nn_h;
    auto nn_d_h = r_p->nn_sphere(&m, d_pm, &nn_h, { p->id, a.id });

    // if (debug_mode) cout << TABTAB << "Help pt #" << nn_h << " dist=" << nn_d_h << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    if (nn_h >= 0) {
     	h = PtwID(nn_h, mesh_p);
    } else {
    	return;
    }

    // if (debug_mode) { cout << TABTAB; printf("hp=%f, ha=%f\n", eucl_dist(h.pt, p->pt), eucl_dist(h.pt, a.pt)); }

    // start looking for the rest two pts b and c
    // if (debug_mode) timer_start();

    Pt3D b_est, c_est;
    auto got_b_c = get_est_b_c(&m, &(a.pt), &(h.pt), b_est, c_est);
    if (!got_b_c) {
    	return;
    }

    int nn_b;
    auto nn_d_b = r_p->nn_sphere(&b_est, 0.0, &nn_b, { p->id, a.id });
    if (nn_b >= 0) {
     	b = PtwID(nn_b, mesh_p);
    } else {
    	return;
    }

    int nn_c;
    auto nn_d_c = r_p->nn_sphere(&c_est, 0.0, &nn_c, { p->id, a.id, b.id });
    if (nn_c >= 0) {
     	c = PtwID(nn_c, mesh_p);
    } else {
    	return;
    }

    // if (debug_mode) cout << TABTAB << "Second pt #" << nn_b << " dist=" << nn_d_b
    // 	<< ", third pt #" << nn_c << " dist=" << nn_d_c << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    // get the ratio set
    auto ratio_set = get_ratio_set_vol(&p->pt, &a.pt, &b.pt, &c.pt);

    if (ratio_set.ratio - prem_entry->meas > 0) {
    	prem_entry->set(*p, a, b, c, ratio_set.volume, ratio_set.ratio, h);
    	prem_entry->fail = false;
    }

}

// void performance_test(TriMesh *mesh, int n, KDtree *kd, node_type *r_root, rtree_info* aInfo) {
//     srand(time(NULL));
//     R_TYPE *query;
//     query = (R_TYPE *) malloc(sizeof(R_TYPE) * aInfo->dim);
//     for (int i = 0; i < 100; ++i) {
//         auto test_pt = mesh->vertices[rand() % n];
//         for (int j = 0; j < aInfo->dim; j++) {
//             query[j] = test_pt[j];
//         }
//         timer_start();
//         auto nn_kd = kd->closest_to_pt(test_pt);
//         auto t_kd = timer_end(MILLISECOND);

//         NN_type *NNresult;
//         timer_start();
//         k_NN_search(r_root, query, 1, &NNresult, aInfo);
//         auto t_r = timer_end(MILLISECOND);

//         cout << t_kd << " v.s. " << t_r << endl;
//     }
// }

void gridify_test(double w, DB_Meshes* db_meshes) {
    double delta = 0.3 * w;
    int num_trials = 11;
    Grid g;

    for (int counter = 0; counter < num_trials; counter++) {
        double trial_w = w + counter * delta;
        g.set_width(trial_w);
        cout << "\nTest gridify the point cloud in 10 random runs..." << endl;
        for (int i = 0; i < 10; i++) {
            auto mesh = db_meshes->get_mesh(rand() % db_meshes->size());
            g.gridify(mesh);

            cout << "\tGrid size: " << trial_w << endl;
            cout << "\tTotal # cells: " << g.cells_map.size() << endl;
            cout << "\tAvg # pts per cell: " << ((double) mesh->size()) / ((double) g.cells_map.size()) << endl;
        }
    }

}

int main(int argc, char **argv) {

    srand(time(0));

    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " database_path w ann_min output_grid_filename [-show_prog_bar] [-debug]" << endl;
        exit(1);
    }

    bool show_prog_bar = false, debug_mode = false;
    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-show_prog_bar")
            show_prog_bar = true;
        else if (string(argv[i]) == "-debug")
            debug_mode = true;
    }

    int argi = 0;
    string db_path = argv[++argi];
    double w = atof(argv[++argi]);
    double ann_min = atof(argv[++argi]);
    // double ann_max = atof(argv[++argi]);
    double ann_max = 0.0;
    string outgrid_filename = argv[++argi];

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

    cout << "Total no. pts: " << n << endl << endl;
    cout << "Total I/- time: " << timer_end(SECOND) << "(s)" << endl;

    // performance_test(mesh_p, n, kd_p, root, &db_rtree_info);

    // gridify_test(w, db_meshes);

    timer_start();

    Struct_DB s_db;
    s_db.set_w(w);
    s_db.set_ann(ann_min, ann_max);

    cout << "\nGridify the point cloud..." << endl;
    timer_start();

    for (int mesh_id = 0; mesh_id < num_meshes; mesh_id++) {
        Grid* g = new Grid(w);
        g->gridify(db_meshes.get_mesh(mesh_id));
        s_db.append_grid(g);
    }

    cout << "Gridify finished in " << timer_end(SECOND) << "(s)" << endl;

    int num_cells = s_db.get_total_cells_count();

    cout << "Grid size: " << w << endl;
    cout << "Total # cells: " << num_cells << endl;
    cout << "Avg # pts per cell: " << ((double) n) / ((double) num_cells) << endl << endl;

    int fail_count = 0;
    int global_cell_id = 0;

    cout << setiosflags(ios::fixed) << setprecision(2);

    auto db_grids = s_db.get_grids();
    for (int mesh_id = 0; mesh_id < db_grids.size(); mesh_id++) {
        auto g = db_grids[mesh_id];
        auto mesh_p = db_meshes.get_mesh(mesh_id);
        auto r_p = db_rtrees[mesh_id];

        cout << "Processing mesh #" << mesh_id << endl << endl;

        ProgressBar bar(g->cells_map.size(), 70);

        for (auto it = g->cells_map.begin(); it != g->cells_map.end(); it++) {
            if (show_prog_bar) {
                ++bar;
                bar.display();
            }

            if (global_cell_id >= num_cells) {
                cout << "Iteration of mesh #" << mesh_id << " terminated due to global cell id exceeding total no. cells" << endl;
                break;
            }

            auto c = it->second;
            c->set_global_id(global_cell_id);

            if (debug_mode) {
                printf("Processing cell #%d (%d, %d, %d) with %d pts\n", global_cell_id, c->x, c->y, c->z, c->list.size());
            }

            auto prem_entry = new Entry();
            prem_entry->fail = true;

            // if (global_cell_id < 50) { // uncomment it for testing
            for (auto &p: c->list) {
                cal_index_entry(&p, ann_min, mesh_p, &r_p, debug_mode, prem_entry);
            }
            // } // uncomment it for testing

            if (prem_entry->fail) {
                if (debug_mode) cout << TAB << "Fail in finding prem entry" << endl;
                fail_count++;
            } else {
                if (debug_mode) cout << TAB << "Prem entry: " << prem_entry->to_str() << endl;

                // for edge index
                prem_entry->fill_sides();
                prem_entry->sort_sides();
            }

            s_db.append_entry(prem_entry);

            global_cell_id++;

        }

        if (show_prog_bar) {
            bar.done();
        }
    }

    cout << resetiosflags(ios::fixed) << setprecision(6);

    cout << "Total # of failed cells: " << fail_count << endl << endl;

    cout << "Total user time: " << timer_end(SECOND) << "(s)" << endl << endl;

    timer_start();

    if (!debug_mode) {
    	timer_start();
        s_db.save(outgrid_filename);
        cout << "Grid saved in " << timer_end(SECOND) << "(s)" << endl;
    }

    cout << "Total -/O time: " << timer_end(SECOND) << "(s)" << endl << endl;
}