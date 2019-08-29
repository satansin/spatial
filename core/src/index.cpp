#include "TriMesh.h"
#include "point.h"
#include "tetra_meas.h"
#include "pcr_adv.h"
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
using namespace trimesh;

Entry cal_index_entry(PtwID p, Pt3D pts_p[], int n, double min, double max) {
	vector<PtwID> ann_pts;
	for (int i_s = 0; i_s < n; i_s++) {
		Pt3D s = pts_p[i_s];
		double d = eucl_dist(s, p.pt);
		if (d - min >= 0 && d - max <= 0) {
			ann_pts.push_back({ i_s, s });
		}
	}
	int hit_size = ann_pts.size();
	if (hit_size < 3) {
		Entry fail;
		fail.repre = p;
		fail.fail = true;
		return fail;
	}

	Entry prem;
	prem.fail = false;
	prem.meas = -1;
	for (int i = 0; i < hit_size; i++) {
		for (int j = i + 1; j < hit_size; j++) {
			for (int k = j + 1; k < hit_size; k++) {
				auto ratio_set = get_ratio_set_vol(p.pt,
					ann_pts[i].pt, ann_pts[j].pt, ann_pts[k].pt);
				if (ratio_set.ratio - prem.meas > 0) {
					prem.repre = p;
					prem.remai[0] = ann_pts[i];
					prem.remai[1] = ann_pts[j];
					prem.remai[2] = ann_pts[k];
					prem.vol = ratio_set.volume;
					prem.meas = ratio_set.ratio;
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
	if (argc < 5) {
		cerr << "Usage: " << argv[0] << " database_filename w min max" << endl;
		exit(1);
	}

	string database_filename = argv[1];
	TriMesh *mesh_p = TriMesh::read(database_filename);

	int n = mesh_p->vertices.size();
	Pt3D pts_p[n];
    for (int i = 0; i < n; i++) {
    	pts_p[i] = pt(mesh_p->vertices[i]);
    }

    string outidx_filename = "../data/t2.idx";
    ofstream outidx_ifs(outidx_filename);

    // double w = atof(argv[1]);
    double w = 5.0;
    vector<Cell> cells;
    for (int i = 0; i < n; i++) {
    	int x = (int) floor(pts_p[i].x / w);
    	int y = (int) floor(pts_p[i].y / w);
    	int z = (int) floor(pts_p[i].z / w);

    	bool is_existing = false;
    	for (int j = 0; j < cells.size(); j++) {
    		if (cells[j].x == x && cells[j].y == y && cells[j].z == z) {
    			is_existing = true;
    			cells[j].list.push_back({ i, pts_p[i] });
    		}
    	}

    	if (!is_existing) {
    		Cell new_cell;
    		new_cell.x = x;
    		new_cell.y = y;
    		new_cell.z = z;
    		new_cell.list.push_back({ i, pts_p[i] });
    		cells.push_back(new_cell);
    	}
    }

    cout << "Total number of cells: " << cells.size() << endl;

    // double min = atof(argv[2]);
    // double max = atof(argv[3]);
    double min = 20.0;
    double max = 20.5;

    for (int i = 0; i < cells.size(); i++) {
    	Cell c = cells[i];
    	Entry prem_entry;
        prem_entry.repre.id = -1;
        prem_entry.remai[0].id = -1;
        prem_entry.remai[1].id = -1;
        prem_entry.remai[2].id = -1;
    	prem_entry.fail = true;
        prem_entry.vol = -1;
    	prem_entry.meas = -1;
    	for (int j = 0; j < c.list.size(); j++) {
    		Entry e = cal_index_entry(c.list[j], pts_p, n, min, max);
    		if (e.meas > prem_entry.meas && e.fail == false) {
    			prem_entry = e;
    		}
    	}

    	outidx_ifs << prem_entry.to_str() << endl;
    }

    outidx_ifs.close();
}