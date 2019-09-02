#include "TriMesh.h"
#include "point.h"
#include "tetra_meas.h"
#include "pcr_adv.h"
#include "RTree.h"
#include "ProgressBar.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <string>
using namespace std;
using namespace trimesh;

double min_dist(Pt3D p, Cell c, double w) {
    double x_dim, y_dim, z_dim;
    if (p.x < c.x * w) {
        x_dim = c.x * w - p.x;
    } else if (p.x > (c.x + 1) * w) {
        x_dim = p.x - (c.x + 1) * w;
    } else {
        x_dim = 0;
    }
    if (p.y < c.y * w) {
        y_dim = c.y * w - p.y;
    } else if (p.y > (c.y + 1) * w) {
        y_dim = p.y - (c.y + 1) * w;
    } else {
        y_dim = 0;
    }
    if (p.z < c.z * w) {
        z_dim = c.z * w - p.z;
    } else if (p.z > (c.z + 1) * w) {
        z_dim = p.z - (c.z + 1) * w;
    } else {
        z_dim = 0;
    }
    return sqrt(x_dim * x_dim + y_dim * y_dim + z_dim * z_dim);
}

double max_dist(Pt3D p, Cell c, double w) {
    double x_dim = max(abs(p.x - c.x * w), abs(p.x - (c.x + 1) * w));
    double y_dim = max(abs(p.y - c.y * w), abs(p.y - (c.y + 1) * w));
    double z_dim = max(abs(p.z - c.z * w), abs(p.z - (c.z + 1) * w));
    return sqrt(x_dim * x_dim + y_dim * y_dim + z_dim * z_dim);
}

void add_ann_pts_in_cell(PtwID p, Cell c, double ann_min, double ann_max, vector<PtwID>& v) {
    for (auto &i: c.list) {
        double d = eucl_dist(i.pt, p.pt);
        if (d - ann_min >= 0 && d - ann_max <= 0) {
            v.push_back(i);
        }
    }
}

vector<PtwID> get_ann_pts(PtwID p, double ann_min, double ann_max, const Grid* g) {
    vector<PtwID> ann_pts;

    int outer_id_min[3] = {
        get_cell_id(p.pt.x - ann_max, g->w),
        get_cell_id(p.pt.y - ann_max, g->w),
        get_cell_id(p.pt.z - ann_max, g->w)
    };
    int outer_id_max[3] = {
        get_cell_id(p.pt.x + ann_max, g->w),
        get_cell_id(p.pt.y + ann_max, g->w),
        get_cell_id(p.pt.z + ann_max, g->w)
    };

    for (int i = outer_id_min[0]; i <= outer_id_max[0]; i++) {
        for (int j = outer_id_min[1]; j <= outer_id_max[1]; j++) {
            for (int k = outer_id_min[2]; k <= outer_id_max[2]; k++) {
                int cell_id[3] = { i, j, k };
                int key = get_cell_key(cell_id, g->cell_id_min, g->cell_id_max);

                auto it = g->cells_map.find(key);
                if (it != g->cells_map.end()) {
                    // cell found
                    if (max_dist(p.pt, it->second, g->w) - ann_min >= 0 &&
                        min_dist(p.pt, it->second, g->w) - ann_max <= 0) {
                        add_ann_pts_in_cell(p, it->second, ann_min, ann_max, ann_pts);
                    }
                }
            }
        }
    }

    return ann_pts;
}

Entry cal_index_entry(PtwID p, double ann_min, double ann_max, const Grid* g) {

	vector<PtwID> ann_pts = get_ann_pts(p, ann_min, ann_max, g);

	int hit_size = ann_pts.size();
    // cout << "Pt #" << p.id << " has " << hit_size << " ann pts" << endl;
	if (hit_size < 3) {
		Entry fail;
		fail.repre = p;
		fail.fail = true;
		return fail;
	}

	Entry prem;
	prem.meas = -1;
    prem.fail = false;
	for (int i = 0; i < hit_size; i++) {
		for (int j = i + 1; j < hit_size; j++) {
			for (int k = j + 1; k < hit_size; k++) {
				auto ratio_set = get_ratio_set_vol(p.pt,
					ann_pts[i].pt, ann_pts[j].pt, ann_pts[k].pt);
				if (ratio_set.ratio - prem.meas > 0) {
                    prem.set(p, ann_pts[i], ann_pts[j], ann_pts[k], ratio_set.volume, ratio_set.ratio);
				}
			}
		}
	}
    sort_remai(prem);
    // double bc = eucl_dist(prem.remai[0].pt, prem.remai[1].pt);
    // double bd = eucl_dist(prem.remai[0].pt, prem.remai[2].pt);
    // double cd = eucl_dist(prem.remai[1].pt, prem.remai[2].pt);
    // if (!(cd >= bd && bd >= bc)) {
    //     prem.fail = true;
    // }

	return prem;
}

int main(int argc, char **argv) {
	if (argc < 6) {
		cerr << "Usage: " << argv[0] << " database_filename w ann_min ann_max output_index_filename [-test]" << endl;
		exit(1);
	}

    bool test_mode = false;
    if (argc == 7 && string(argv[6]) == "-test") {
        test_mode = true;
    }

	string database_filename = argv[1];
	TriMesh *mesh_p = TriMesh::read(database_filename);

	int n = mesh_p->vertices.size();
	vector<Pt3D> pts_p;
    for (point &p: mesh_p->vertices) {
    	pts_p.push_back(pt(p));
    }

    double w = atof(argv[2]);
    double delta = 0.3 * w;
    int num_trials = test_mode ? 11 : 1;
    Grid g;

    for (int counter = 0; counter < num_trials; counter++) {
        double trial_w = w + counter * delta;
        g.set_width(trial_w);
        cout << "Gridify the point cloud..." << endl;
        g.gridify(mesh_p);

        cout << "Grid size: " << trial_w << endl;
        cout << "Total # cells: " << g.cells_count << endl;
        cout << "Avg # pts per cell: " << ((double) n) / ((double) g.cells_count) << endl;
    }

    // auto t1 = chrono::high_resolution_clock::now();

    double ann_min = atof(argv[3]);
    double ann_max = atof(argv[4]);
    vector<Entry> entry_list;

    int failed_entry_count = 0;
    cout << "Calculating index entry for every point..." << endl;
    ProgressBar bar(n, 70);
    for (int i = 0; i < n; i++) {
        ++bar;

        PtwID p(i, pts_p[i]);
        Entry e = cal_index_entry(p, ann_min, ann_max, &g);
        entry_list.push_back(e);
        if (e.fail) {
            failed_entry_count++;
        }
        // if (i >= 100) {
        //     break;
        // }

        bar.display();
    }
    bar.done();
    cout << failed_entry_count << " out of " << n << " failed" << endl;

    // string outidx_filename = argv[5];
    // string outgrid_filename = outidx_filename + ".grid";
    // ofstream outgrid_ifs(outgrid_filename);

    // // write grid headers
    // outgrid_ifs << w << " " << ann_min << " " << ann_max << " " << cells_count << endl;
    // outgrid_ifs << cell_id_min[0] << " " << cell_id_min[1] << " " << cell_id_min[2] << " "
    //             << cell_id_max[0] << " " << cell_id_max[1] << " " << cell_id_max[2] << endl;

    // RTree<int, double, 2> tree;

    // int fail_count = 0;
    // int counter = 0;
    // int check_point = cells_count / 100;
    // int exec_point = 57376;
    // for (auto it = cells_map.begin(); it != cells_map.end(); it++) {
    //     counter++;
    //     // if (counter != exec_point) {
    //     //     continue;
    //     // }
    //     int key = it->first;
    // 	Cell c = it->second;
    //     // cout << "Processing cell: " << c.x << ", " << c.y << ", " << c.z << endl;
        
    //     // output per-cell information
    //     outgrid_ifs << key << " " << c.to_str() << endl;;

    // 	Entry prem_entry;
    //     prem_entry.fail = true;
    //     for (PtwID &p: c.list) {
    // 		Entry e = cal_index_entry(p, &cells_map, ann_min, ann_max, w, cell_id_min, cell_id_max);
    // 		// find premium entry having the largest meas
    //         if (e.meas > prem_entry.meas && e.fail == false) {
    // 			prem_entry = e;
    // 		}
    // 	}

    //     // output per-cell premium entry
    //     outgrid_ifs << prem_entry.to_str() << endl;

    //     if (prem_entry.fail) {
    //         fail_count++;
    //     } else {
    //         double box_min[2] = { prem_entry.vol, prem_entry.meas },
    //                box_max[2] = { prem_entry.vol, prem_entry.meas };
    //         tree.Insert(box_min, box_max, key);
    //     }

    //     if (counter % check_point == 0) {
    //         cout << (counter / check_point) << "/100" << endl;
    //     }
    // }
    // cout << "Total # of failed cells: " << fail_count << endl;

    // outgrid_ifs.close();
    // tree.Save(outidx_filename.c_str());

    // auto t2 = chrono::high_resolution_clock::now();
    // chrono::duration<double, milli> span = t2 - t1;
    // cout << "Total time: " << span.count() << endl;
}