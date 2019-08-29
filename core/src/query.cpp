#include "TriMesh.h"
#include "point.h"
#include "tetra_meas.h"
#include "pcr_adv.h"
#include "share.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;
using namespace trimesh;

const double epsilon_m = 0.1;

vector<Entry> cal_entries(PtwID q, Pt3D pts_q[], int m, double min, double max) {
	vector<Entry> ret;

	vector<PtwID> ann_pts;
	for (int i_s = 0; i_s < m; i_s++) {
		Pt3D s = pts_q[i_s];
		double d = eucl_dist(s, q.pt);
		if (d - min >= 0 && d - max <= 0) {
			ann_pts.push_back({ i_s, s });
		}
	}
	int hit_size = ann_pts.size();
	if (hit_size < 3) {
		return ret;
	}

	for (int i = 0; i < hit_size; i++) {
		for (int j = i + 1; j < hit_size; j++) {
			for (int k = j + 1; k < hit_size; k++) {
				auto ratio_set = get_ratio_set_vol(q.pt,
					ann_pts[i].pt, ann_pts[j].pt, ann_pts[k].pt);
				Entry e;
				e.fail = false;
				e.repre = q;
				e.remai[0] = ann_pts[i];
				e.remai[1] = ann_pts[j];
				e.remai[2] = ann_pts[k];
				e.vol = ratio_set.volume;
				e.meas = ratio_set.ratio;
				sort_remai(e);
				ret.push_back(e);
			}
		}
	}
	return ret;
}

bool check_congr(Entry e, Entry f) {
	double e_edges[5], f_edges[5];
	e_edges[0] = eucl_dist(e.repre.pt, e.remai[0].pt);
	e_edges[1] = eucl_dist(e.repre.pt, e.remai[1].pt);
	e_edges[2] = eucl_dist(e.remai[0].pt, e.remai[1].pt);
	e_edges[3] = eucl_dist(e.repre.pt, e.remai[2].pt);
	e_edges[4] = eucl_dist(e.remai[0].pt, e.remai[2].pt);
	f_edges[0] = eucl_dist(f.repre.pt, f.remai[0].pt);
	f_edges[1] = eucl_dist(f.repre.pt, f.remai[1].pt);
	f_edges[2] = eucl_dist(f.remai[0].pt, f.remai[1].pt);
	f_edges[3] = eucl_dist(f.repre.pt, f.remai[2].pt);
	f_edges[4] = eucl_dist(f.remai[0].pt, f.remai[2].pt);
	for (int i = 0; i < 5; i++)	{
		if (abs(e_edges[i] - f_edges[i]) > 2 * epsilon_m) {
			return false;
		}
	}
	return true;
}

vector<Entry_Pair> look_up_index(Entry e, vector<Entry> index_list) {
	vector<Entry_Pair> ret;
	double low_vol, high_vol, low_meas, high_meas;
	cal_range(e.repre.pt, e.remai[0].pt, e.remai[1].pt, e.remai[2].pt, epsilon_m,
		low_vol, high_vol, low_meas, high_meas);

	for (Entry &f: index_list) {
		if (f.vol >= low_vol &&	f.vol <= high_vol &&
			f.meas >= low_meas && f.meas <= high_meas && check_congr(e, f)) {
			Entry_Pair p(e, f);
			ret.push_back(p);
		}
	}
	return ret;
}

int main(int argc, char **argv) {
	// if (argc < 4) {
	// 	cerr << "Usage: " << argv[0] << " w min max" << endl;
	// 	exit(1);
	// }

	cout << "Reading dababase file" << endl;
	string database_filename = "../data/t2.ply";
	TriMesh *mesh_p = TriMesh::read(database_filename);

	cout << "Loading index entries" << endl;
    string idx_filename = "../data/t2.idx";
    ifstream idx_ifs(idx_filename);

    vector<Entry> index_list;
    while (!idx_ifs.eof()) {
    	int id_repre, id_0, id_1, id_2;
    	double vol, meas;
    	bool fail;
    	idx_ifs >> id_repre >> id_0 >> id_1 >> id_2 >> vol >> meas >> fail;
    	if (fail) {
    		continue;
    	}

    	Entry e;
    	e.repre.id = id_repre;
    	e.repre.pt = pt(mesh_p->vertices[id_repre]);
    	e.remai[0].id = id_0;
    	e.remai[0].pt = pt(mesh_p->vertices[id_0]);
    	e.remai[1].id = id_1;
    	e.remai[1].pt = pt(mesh_p->vertices[id_1]);
    	e.remai[2].id = id_2;
    	e.remai[2].pt = pt(mesh_p->vertices[id_2]);
    	e.vol = vol;
    	e.meas = meas;
    	e.fail = fail;

    	index_list.push_back(e);
    }

    cout << "Reading query file" << endl;
	string query_filename = "../data/t2_query.ply";
	TriMesh *mesh_q = TriMesh::read(query_filename);

	cout << "Reading id maps" << endl;
	string idmap_filename = "../data/t2_id";
	ifstream idmap_ifs(idmap_filename);

	int m = mesh_q->vertices.size();
	Pt3D pts_q[m];
	int idmap[m];
    for (int i = 0; i < m; i++) {
    	pts_q[i] = pt(mesh_q->vertices[i]);
    	idmap_ifs >> idmap[i];
    }

    // double w = atof(argv[1]);
    double w = 17.5;
    cout << "Query voxelized of grid size " << w << endl;
    vector<Cell> cells;
    for (int i = 0; i < m; i++) {
    	int x = (int) floor(pts_q[i].x / w);
    	int y = (int) floor(pts_q[i].y / w);
    	int z = (int) floor(pts_q[i].z / w);

    	bool is_existing = false;
    	for (int j = 0; j < cells.size(); j++) {
    		if (cells[j].x == x && cells[j].y == y && cells[j].z == z) {
    			is_existing = true;
    			cells[j].list.push_back({ i, pts_q[i] });
    		}
    	}

    	if (!is_existing) {
    		Cell new_cell;
    		new_cell.x = x;
    		new_cell.y = y;
    		new_cell.z = z;
    		new_cell.list.push_back({ i, pts_q[i] });
    		cells.push_back(new_cell);
    	}
    }

    cout << "Total number of cells: " << cells.size() << endl;

    // cout << "Displaying info of each cell:" << endl;
    // for (int i = 0; i < cells.size(); i++) {
    // 	cout << cells[i].x << " "
    // 		 << cells[i].y << " "
    // 		 << cells[i].z << " "
    // 		 << cells[i].list.size() << endl;
    // }
    cout << endl;

    // double min = atof(argv[2]);
    // double max = atof(argv[3]);
    double min = 20 - epsilon_m;
    double max = 20.5 + epsilon_m;

    int selected_cell_id = 3;
    Cell selected_cell = cells[selected_cell_id];
    cout << "Selecting cell #" << selected_cell_id
    	 << " with " << selected_cell.list.size() << " pts" << endl;

    vector<Entry_Pair> hit_list;
    for (int i = 0; i < selected_cell.list.size(); i++) {
    	PtwID q = selected_cell.list[i];

    	// if (q.id != 12) {
    	// 	continue;
    	// }

    	auto list = cal_entries(q, pts_q, m, min, max);
    	// cout << "Calculating entry list for query pt #" << q.id << endl;
    	// cout << "Size of the entry list: " << list.size() << endl;

    	for (Entry &e: list) {
    		// if (e.remai[0].id != 160 || e.remai[1].id != 167 || e.remai[2].id != 343) {
    		// 	continue;
    		// }
    		vector<Entry_Pair> tmp_list = look_up_index(e, index_list);
    		hit_list.insert(hit_list.end(), tmp_list.begin(), tmp_list.end());
    	}
    }
    cout << endl;

    cout << "Returned hit size is " << hit_list.size() << endl;
    for (Entry_Pair &h: hit_list) {
    	h.cal_xf();
    	double err = cal_err(mesh_q, mesh_p, h.xf);
    	if (err < 0.2) {
    		cout << h.to_str() << endl << err << endl;
    	}
    }
}