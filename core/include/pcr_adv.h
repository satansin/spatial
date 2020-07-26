#ifndef __PCR_ADV_H_
#define __PCR_ADV_H_

#include "point.h"
#include "trans.h"
#include "mesh.h"
#include "RTree.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <iostream>

using namespace std;

const int RSTREE_SCALE = 1e5;

const double PI = 3.14159265;

#ifdef IDX_3
	const int INDEX_DIM = 3;
#else
	const int INDEX_DIM = 6;
#endif

typedef RTree<int, int, INDEX_DIM, double, 36> IndexTree;
typedef RTree<int, int, INDEX_DIM, double, 6> AuxTree;

typedef RTree<int, int, 3, double, 6> CollTree;

struct PtwID {
	int id;
	Pt3D* pt;
	void set(int id, Pt3D* pt) {
		this->id = id;
		this->pt = pt;
	}
	void set(int id, Mesh* mesh) {
		set(id, mesh->get_pt(id));
	}
	PtwID() {
		this->id = -1;
	}
	PtwID(int id, Pt3D* pt) {
		set(id, pt);
	}
	PtwID(int id, Mesh* mesh) {
		set(id, mesh);
	}
	PtwID(PtwID* from) {
		set(from->id, from->pt);
	}
};

struct Box3D_Int {
	int min_id[3], max_id[3];
	bool empty;
	void set(int min_id_x, int min_id_y, int min_id_z, int max_id_x, int max_id_y, int max_id_z, bool empty) {
		min_id[0] = min_id_x;
		min_id[1] = min_id_y;
		min_id[2] = min_id_z;
		max_id[0] = max_id_x;
		max_id[1] = max_id_y;
		max_id[2] = max_id_z;
		this->empty = empty;
	}
	Box3D_Int() {}
	Box3D_Int(int min_id_x, int min_id_y, int min_id_z, int max_id_x, int max_id_y, int max_id_z, bool empty) {
		set(min_id_x, min_id_y, min_id_z, max_id_x, max_id_y, max_id_z, empty);
	}
	Box3D_Int& operator=(const Box3D_Int& b) {
        set(b.min_id[0], b.min_id[1], b.min_id[2], b.max_id[0], b.max_id[1], b.max_id[2], b.empty);
        return *this;
    }
	void expand(int by_scale = 1) {
		for (int i = 0; i < 3; i++) {
			min_id[i] -= by_scale;
			max_id[i] += by_scale;
		}
	}
	bool includes(const int* id) const {
		if (empty) {
			return false;
		}
		for (int i = 0; i < 3; i++) {
			if (id[i] < min_id[i] || id[i] > max_id[i]) {
				return false;
			}
		}
		return true;
	}
	int width() const {
		if (empty) {
			return 0;
		} else {
			return (max_id[0] - min_id[0] + 1);
		}
	}
	int size() const {
		if (empty){
			return 0;
		} else {
			return (max_id[0] - min_id[0] + 1) * (max_id[1] - min_id[1] + 1) * (max_id[2] - min_id[2] + 1);
		}
	}
	void list_cell_id(vector<int>& list) const {
		list.clear();
		for (int i = min_id[0]; i <= max_id[0]; i++) {
            for (int j = min_id[1]; j <= max_id[1]; j++) {
                for (int k = min_id[2]; k <= max_id[2]; k++) {
                	list.push_back(i);
                	list.push_back(j);
                	list.push_back(k);
                }
            }
        }
	}
	string to_str() const {
		return "([" + to_string(min_id[0]) + ", " + to_string(max_id[0]) + "], [" +
					  to_string(min_id[1]) + ", " + to_string(max_id[1]) + "], [" +
					  to_string(min_id[2]) + ", " + to_string(max_id[2]) + "])";
	}
	void save(ostream& os) const {
		os << min_id[0] << " " << min_id[1] << " " << min_id[2] << " "
	       << max_id[0] << " " << max_id[1] << " " << max_id[2];
	}
	void read(istream& is) {
		is >> min_id[0] >> min_id[1] >> min_id[2]
	       >> max_id[0] >> max_id[1] >> max_id[2];
	}
};

struct Cell {
	int global_id;
	int x, y, z;
	vector<int> list;
	Cell() {}
	Cell(int x, int y, int z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	void set_global_id(int global_id) {
		this->global_id = global_id;
	}
	// void add_pt(PtwID p) {
	// 	list.push_back(p);
	// }
	// void add_pt(int id, Pt3D pt) {
	// 	list.push_back(PtwID(id, pt));
	// }
	void add_pt(int id) {
		list.push_back(id);
	}
	// PtwID find_nn(Pt3D* q) const {
	// 	PtwID nn;
	// 	double nn_dist = numeric_limits<double>::max();
	// 	for (auto &p: list) {
	// 		double tmp_dist = eucl_dist(p.pt, q);
	// 		if (tmp_dist - nn_dist < 0) {
	// 			nn = p;
	// 			nn_dist = tmp_dist;
	// 		}
	// 	}
	// 	return nn;
	// }
	string to_str() const {
		string s =
			to_string(global_id) + " " +
			to_string(x) + " " +
			to_string(y) + " " +
			to_string(z) + " " +
			to_string(list.size());
		for (auto &p: list) {
		 	s = s + " " + to_string(p);
		}
		return s;
	}
};

struct Box3D {
	double min[3], max[3];
	Box3D() {}
	Box3D(const Cell* c, double w, int scale = 0) {
		min[0] = (c->x - scale) * w;
		min[1] = (c->y - scale) * w;
		min[2] = (c->z - scale) * w;
		max[0] = (c->x + 1 + scale) * w;
		max[1] = (c->y + 1 + scale) * w;
		max[2] = (c->z + 1 + scale) * w;
	}
	Box3D(Box3D_Int box_int, double w) {
		for (int i = 0; i < 3; i++){
			min[i] = box_int.min_id[i] * w;
			max[i] = (box_int.max_id[i] + 1) * w;
		}
	}
	string to_str() const {
		return "([" + to_string(min[0]) + ", " + to_string(max[0]) + "], [" +
					  to_string(min[1]) + ", " + to_string(max[1]) + "], [" +
					  to_string(min[2]) + ", " + to_string(max[2]) + "])";
	}
};

double min_dist(Pt3D p, Box3D b) {
    double x_dim, y_dim, z_dim;
    if (p.x < b.min[0]) {
        x_dim = b.min[0] - p.x;
    } else if (p.x > b.max[0]) {
        x_dim = p.x - b.max[0];
    } else {
        x_dim = 0;
    }
    if (p.y < b.min[1]) {
        y_dim = b.min[1] - p.y;
    } else if (p.y > b.max[1]) {
        y_dim = p.y - b.max[1];
    } else {
        y_dim = 0;
    }
    if (p.z < b.min[2]) {
        z_dim = b.min[2] - p.z;
    } else if (p.z > b.max[2]) {
        z_dim = p.z - b.max[2];
    } else {
        z_dim = 0;
    }
    return sqrt(x_dim * x_dim + y_dim * y_dim + z_dim * z_dim);
}

double max_dist(Pt3D p, Box3D b) {
    double x_dim = max(abs(p.x - b.min[0]), abs(p.x - b.max[0]));
    double y_dim = max(abs(p.y - b.min[1]), abs(p.y - b.max[1]));
    double z_dim = max(abs(p.z - b.min[2]), abs(p.z - b.max[2]));
    return sqrt(x_dim * x_dim + y_dim * y_dim + z_dim * z_dim);
}

int get_cell_id(double val, double w) {
   	return (int) floor(val / w);
}

const int* get_cell_id(Pt3D pt, double w) {
	static int* cell_id = new int[3];
	cell_id[0] = get_cell_id(pt.x, w);
	cell_id[1] = get_cell_id(pt.y, w);
	cell_id[2] = get_cell_id(pt.z, w);
    return cell_id;
}

int get_cell_key(const int* cell_id, Box3D_Int* box) {
    int span[3] = { box->max_id[0] - box->min_id[0] + 1, box->max_id[1] - box->min_id[1] + 1, box->max_id[2] - box->min_id[2] + 1 };
    return ( (cell_id[0] - box->min_id[0]) * span[1] * span[2] +
             (cell_id[1] - box->min_id[1]) * span[2] +
             (cell_id[2] - box->min_id[2]) );
}

struct Grid {
	double w;
	Box3D_Int* cells_box;
    unordered_map<int, Cell*> cells_map;
    // int cells_count;
    Grid() {
    	cells_box = new Box3D_Int();
    }
    Grid(double w) {
    	cells_box = new Box3D_Int();
    	this->w = w;
    }
    void set_width(double w) {
    	this->w = w;
    }
    const Cell* find_cell(const int* cell_id) const {
    	int key = get_cell_key(cell_id, cells_box);
    	auto it = cells_map.find(key);
        if (it != cells_map.end()) {
        	return it->second;
        } else {
        	return nullptr;
        }
    }
    // void gridify(Mesh* mesh) {
    // 	int n = mesh->size();
    // 	for (int i = 0; i < 3; i++) {
    //         cells_box->min_id[i] = get_cell_id(mesh->get_box_min(i), w);
    //         cells_box->max_id[i] = get_cell_id(mesh->get_box_max(i), w);
    //     }
    // 	cells_map.clear();
    //     for (int i = 0; i < n; i++) {
    //     	auto pt3d = mesh->get_pt(i);
    //         auto cell_id = get_cell_id(pt3d, w);
    //         int key = get_cell_key(cell_id, cells_box);
    //         auto it = cells_map.find(key);
    //         if (it != cells_map.end()) {
    //             // cell already exists
    //             it->second->add_pt(i, pt3d);
    //         } else {
    //             Cell* new_cell = new Cell(cell_id[0], cell_id[1], cell_id[2]);
    //             new_cell->add_pt(i, pt3d);
    //             cells_map[key] = new_cell;
    //         }
    //     }
    //     // cells_count = cells_map.size();
    // }
	Box3D_Int get_outer_box(double min_x, double min_y, double min_z, double max_x, double max_y, double max_z) const {
		return Box3D_Int{
	        get_cell_id(min_x, w),
	        get_cell_id(min_y, w),
	        get_cell_id(min_z, w),
	        get_cell_id(max_x, w),
	        get_cell_id(max_y, w),
	        get_cell_id(max_z, w),
	        false // default non-empty
		};
	}
	// PtwID relocate(const double* pt) const {
	// 	Pt3D pt3d = { pt[0], pt[1], pt[2] };
	// 	auto cell_id = get_cell_id(pt3d, w);
	// 	auto cell_found = find_cell(cell_id);
	// 	if (cell_found) {
	// 		return cell_found->find_nn(pt3d);
	// 	} else {
	// 		return {-1, Pt3D()};
	// 	}
	// }
    // PtwID get_nn(Pt3D q) {
    //     int cell_id[3] = {
    //         get_cell_id(q.x, w),
    //         get_cell_id(q.y, w),
    //         get_cell_id(q.z, w)
    //     };
    //     Box3D_Int box(cell_id[0], cell_id[1], cell_id[2], cell_id[0], cell_id[1], cell_id[2], false);
    //     do {
    //     	Box3D_Int prev_box = box;
    //     	box.expand();
    // 		for (int i = box.min_id[0]; i <= box.max_id[0]; i++) {
    //     		for (int j = box.min_id[1]; j <= box.max_id[1]; j++) {
    //         		for (int k = box.min_id[2]; k <= box.max_id[2]; k++) {
    //     				if (prev_box.get_width() > 1 && prev_box.includes(cell_id)) {
    //     					continue;
    //     				}
    //     			}
    //     		}
    //     	}
    //     } while (true);
    // }
    virtual ~Grid() {
    	delete cells_box;
    	for (auto &it: cells_map) {
    		delete it.second;
    	}
    }
};

struct Entry {
	PtwID* repre; // p or q
	PtwID* remai[3]; // a, b, c
	double vol;
	double meas;
	bool fail;
	PtwID* help; // h
	double sides[6]; // pa, pb, pc, ab, ac, bc

	void set(PtwID* repre, PtwID* remai_0, PtwID* remai_1, PtwID* remai_2, double vol, double meas) {
		this->repre = new PtwID(repre);
		this->remai[0] = new PtwID(remai_0);
		this->remai[1] = new PtwID(remai_1);
		this->remai[2] = new PtwID(remai_2);
		this->vol = vol;
		this->meas = meas;
	}
	void set(PtwID* help) {
		this->help = new PtwID(help);
	}
	void set(PtwID* repre, PtwID* remai_0, PtwID* remai_1, PtwID* remai_2, double vol, double meas, PtwID* help) {
		set(repre, remai_0, remai_1, remai_2, vol, meas);
		set(help);
	}
	void set(int repre, int remai_0, int remai_1, int remai_2, double vol, double meas, Mesh* mesh) {
		this->repre = new PtwID(repre, mesh);
		this->remai[0] = new PtwID(remai_0, mesh);
		this->remai[1] = new PtwID(remai_1, mesh);
		this->remai[2] = new PtwID(remai_2, mesh);
		this->vol = vol;
		this->meas = meas;
	}
	void set(int help, Mesh* mesh) {
		this->help = new PtwID(help, mesh);
	}
	void set(int repre, int remai_0, int remai_1, int remai_2, double vol, double meas, int help, Mesh* mesh) {
		set(repre, remai_0, remai_1, remai_2, vol, meas, mesh);
		set(help, mesh);
	}
	Entry() {
		this->repre = new PtwID();
		this->remai[0] = new PtwID();
		this->remai[1] = new PtwID();
		this->remai[2] = new PtwID();
        vol = -1;
    	meas = -1;
		this->help = new PtwID();
    	fail = false;
    	for (int i = 0; i < 6; i++) {
    		sides[i] = 0;
    	}
	}
	Entry(PtwID* repre, PtwID* remai_0, PtwID* remai_1, PtwID* remai_2, double vol, double meas, PtwID* help) {
		set(repre, remai_0, remai_1, remai_2, vol, meas, help);
		fail = false;
	}
	Entry(PtwID* repre, PtwID* remai_0, PtwID* remai_1, PtwID* remai_2, PtwID* help) {
		set(repre, remai_0, remai_1, remai_2, -1, -1, help);
		fail = false;
	}
	Entry(PtwID* repre, PtwID* remai_0, PtwID* remai_1, PtwID* remai_2) {
		set(repre, remai_0, remai_1, remai_2, -1, -1);
		this->help = new PtwID();
		fail = false;
	}
	Entry(istream& is, Mesh* mesh_db) {
		int repre_id, help_id;
		int remai_id[3];
	    double vol, meas;
	    bool l_fail;
		is >> repre_id;
		for (int i = 0; i < 3; i++) {
			is >> remai_id[i];
		}
		is >> vol >> meas >> l_fail >> help_id;
		if (!l_fail) {
			if (help_id < 0) {
				set(repre_id, remai_id[0], remai_id[1], remai_id[2], vol, meas, mesh_db);
				this->help = new PtwID;
			} else {
				set(repre_id, remai_id[0], remai_id[1], remai_id[2], vol, meas, help_id, mesh_db);
			}
		}
	    this->fail = l_fail;

		for (int i = 0; i < 6; i++) {
			is >> sides[i];
		}
	}
	void get_index_box(double err, int box_min[INDEX_DIM], int box_max[INDEX_DIM]) {
		#ifdef IDX_3
			// #ifdef _3NN
				// use 3-side length as index keys
				box_min[0] = (int) ((sides[3] - err) * RSTREE_SCALE);
				box_max[0] = (int) ((sides[3] + err) * RSTREE_SCALE);
				box_min[1] = (int) ((sides[4] - err) * RSTREE_SCALE);
				box_max[1] = (int) ((sides[4] + err) * RSTREE_SCALE);
				box_min[2] = (int) ((sides[5] - err) * RSTREE_SCALE);
				box_max[2] = (int) ((sides[5] + err) * RSTREE_SCALE);
			// #else
			// 	// use 3-side length as index keys
			// 	box_min[0] = (int) ((sides[0] - err) * RSTREE_SCALE);
			// 	box_max[0] = (int) ((sides[0] + err) * RSTREE_SCALE);
			// 	box_min[1] = (int) ((sides[3] - err) * RSTREE_SCALE);
			// 	box_max[1] = (int) ((sides[3] + err) * RSTREE_SCALE);
			// 	box_min[2] = (int) ((sides[4] - err) * RSTREE_SCALE);
			// 	box_max[2] = (int) ((sides[4] + err) * RSTREE_SCALE);
			// #endif
		#else
			// use 6-side length as index keys
			for (int i = 0; i < INDEX_DIM; i++) {
				box_min[i] = (int) ((sides[i] - err) * RSTREE_SCALE);
				box_max[i] = (int) ((sides[i] + err) * RSTREE_SCALE);
			}
		#endif
	}
	void get_index_box(int box_min[INDEX_DIM], int box_max[INDEX_DIM]) {
		get_index_box(0.0, box_min, box_max);
	}
	void fill_sides() {
		sides[0] = eucl_dist(repre->pt, remai[0]->pt);
		sides[1] = eucl_dist(repre->pt, remai[1]->pt);
		sides[2] = eucl_dist(repre->pt, remai[2]->pt);
		sides[3] = eucl_dist(remai[0]->pt, remai[1]->pt);
		sides[4] = eucl_dist(remai[0]->pt, remai[2]->pt);
		sides[5] = eucl_dist(remai[1]->pt, remai[2]->pt);
	}
	void sort_sides() {
		int prim = primary_remai();
		if (prim != 0) {
			swap(remai[0], remai[prim]);
			swap(sides[0], sides[prim]);
			swap(sides[5], sides[5 - prim]);
		}
		auto pa = *remai[0]->pt - *repre->pt;
		auto pb = *remai[1]->pt - *repre->pt;
		auto pc = *remai[2]->pt - *repre->pt;
		Pt3D pb_cross_pc; cross_prd(&pb, &pc, pb_cross_pc);
		double dec = dot_prd(&pa, &pb_cross_pc);
		if (dec > 0) {
			swap(remai[1], remai[2]);
			swap(sides[1], sides[2]);
			swap(sides[3], sides[4]);
		}
	}
	// // depr: not needed:
	// void sort_sides_3nn() {
	// 	if (sides[4] < sides[3]) {
	// 		swap(remai[1], remai[2]);
	// 		swap(sides[1], sides[2]);
	// 		swap(sides[3], sides[4]);
	// 	}
	// 	if (sides[5] < sides[4]) {
	// 		swap(remai[0], remai[1]);
	// 		swap(sides[0], sides[1]);
	// 		swap(sides[4], sides[5]);
	// 		if (sides[4] < sides[3]) {
	// 			swap(remai[1], remai[2]);
	// 			swap(sides[1], sides[2]);
	// 			swap(sides[3], sides[4]);
	// 		}
	// 	}
	// }
	string to_str(int precision = 6) const {
		stringstream ss;
		ss << setprecision(precision);
		ss << repre->id << " "
		   << remai[0]->id << " "
		   << remai[1]->id << " "
		   << remai[2]->id << " "
		   << vol << " "
		   << meas << " "
		   << fail << " "
		   << help->id << " ";
		for (int i = 0; i < 6; i++) {
			ss << sides[i];
			if (i < 5)
				ss << " ";
		}
		return ss.str();
	}
	virtual ~Entry() {
		delete repre;
        for (int i = 0; i < 3; i++) {
        	delete remai[i];
        }
		delete help;
	}

private:
	int primary_remai() const {
		if (sides[0] <= sides[1])
			if (sides[0] <= sides[2])
				return 0;
			else
				return 2;
		else
			if (sides[1] <= sides[2])
				return 1;
			else
				return 2;
	}
	// // bad design! not used, but as a reference
	// int primary_remai_3nn() const {
	// 	if (sides[3] <= sides[4])
	// 		if (sides[3] <= sides[5])
	// 			return 2; // sides[3] (ab) is the shortest -> remai[2] (c) is primary
	// 		else
	// 			return 0; // sides[5] (bc) is the shortest -> remai[0] (a) is primary
	// 	else
	// 		if (sides[4] <= sides[5])
	// 			return 1; // sides[4] ac is the shortest -> remai[1] (b) is primary
	// 		else
	// 			return 0; // sides[5] (bc) is the shortest -> remai[0] (a) is primary
	// }
};

// // depr: not used?
// void sort_remai(Entry& e) {
//     double bc = eucl_dist(&e.remai[0]->pt, &e.remai[1]->pt);
//     double bd = eucl_dist(&e.remai[0]->pt, &e.remai[2]->pt);
//     double cd = eucl_dist(&e.remai[1]->pt, &e.remai[2]->pt);
//     if (cd < bd) {
//         swap(e.remai[0], e.remai[1]);
//         swap(cd, bd);
//     }
//     if (bc > cd) {
//         swap(e.remai[0], e.remai[2]);
//         swap(e.remai[1], e.remai[2]);
//     } else if (bc > bd) {
//         swap(e.remai[1], e.remai[2]);
//     }
// }

struct Struct_DB {

	vector<Grid*> grids;

	double w;
	double ann_min;
	double ann_mid;
	double ann_max;
	double ang_min;

	vector<Entry*> entries;

	vector<int> global_id_map;

	int total_cells_count; // accumulative

	#ifdef TEST_MODE
		vector<unordered_map<int, int>> reverse_entries_maps;
		vector<unordered_set<int>> repre_id_sets;
	#endif

	Struct_DB() {
		total_cells_count = 0;
	}

	int get_total_cells_count() const {
		return total_cells_count;
	}

	void append_grid(Grid* g) {
		grids.push_back(g);
		total_cells_count += g->cells_map.size();
	}
	const vector<Grid*>& get_grids() const {
		return grids;
	}

	void append_entry(Entry* e) {
		entries.push_back(e);
	}
	const vector<Entry*>& get_entries() const {
		return entries;
	}

	bool read(string filename, DB_Meshes* db_meshes) {
	    ifstream ifs(filename);
	    if (!ifs.is_open())
	    	return false;

	    int num_grids;
	    ifs >> w >> ann_min >> ann_mid >> ann_max >> ang_min >> num_grids >> total_cells_count;

	    grids.reserve(num_grids);
	    for (int i = 0; i < num_grids; i++) {
	    	int id;
	    	ifs >> id;

	    	Grid* g = new Grid(w);
		    ifs >> g->cells_box->min_id[0] >> g->cells_box->min_id[1] >> g->cells_box->min_id[2]
		        >> g->cells_box->max_id[0] >> g->cells_box->max_id[1] >> g->cells_box->max_id[2];

		    grids.push_back(g);
	    }
	    grids.shrink_to_fit();

	    #ifdef TEST_MODE
		    reverse_entries_maps.resize(num_grids);
		    repre_id_sets.resize(num_grids);
	    #endif

	    for (int i = 0; i < total_cells_count; i++) {
	        int grid_id, key, cell_id, x, y, z, list_size;
	        ifs >> grid_id;
	        ifs >> key;
	        ifs >> cell_id;
	        ifs >> x >> y >> z >> list_size;
	        
	        Cell* c = new Cell(x, y, z);
	        c->set_global_id(cell_id);
	        for (int j = 0; j < list_size; j++) {
	            int pt_id;
	            ifs >> pt_id;
	            // Pt3D pt = *(db_meshes->get_mesh(grid_id)->get_pt(pt_id));
	            // c->add_pt(pt_id, pt);
	            c->add_pt(pt_id);
	        }
	        grids[grid_id]->cells_map[key] = c;

	        ifs >> grid_id; // another grid_id has been saved
	        Entry *e = new Entry(ifs, db_meshes->get_mesh(grid_id));

	        entries.push_back(e);

	        global_id_map.push_back(grid_id);

	        #ifdef TEST_MODE
		        repre_id_sets[grid_id].insert(e->repre->id);
		        reverse_entries_maps[grid_id][e->repre->id] = cell_id;
	        #endif
	    }

	    ifs.close();

	    return true;
	}

	Entry* get_entry(int key) const {
		return entries[key];
	}

	int get_grid_id_by_global_cell_id(int global_cell_id) const {
		return global_id_map[global_cell_id];
	}

	#ifdef TEST_MODE
	bool look_up_repre_index(int repre_id, int mesh_id) const {
	    auto got = repre_id_sets[mesh_id].find(repre_id);
	    if (got != repre_id_sets[mesh_id].end()) {
	        return true;
	    }
	    return false;
	}

	int get_remai_id(int repre_id, int i, int mesh_id) const {
		return entries[reverse_entries_maps[mesh_id].at(repre_id)]->remai[i]->id;
	}

	int get_help_id(int repre_id, int mesh_id) const {
		return entries[reverse_entries_maps[mesh_id].at(repre_id)]->help->id;
	}
	#endif

	void save(string filename) const {
		ofstream ofs(filename);

		// write grid headers TODO: if _3NN?
		#ifdef _3NN
			ofs << w << " " << ann_min << " " << ang_min << " " << grids.size() << " " << total_cells_count << endl;
		#else
			ofs << w << " " << ann_min << " " << ann_max << " " << grids.size() << " " << total_cells_count << endl;
		#endif

		for (int i = 0; i < grids.size(); i++) {
			ofs << i << " ";
			grids[i]->cells_box->save(ofs);
			ofs << endl;
	    }

		for (int i = 0; i < grids.size(); i++) {
	    	for (auto &it: grids[i]->cells_map) {
	    		ofs << i << " " << it.first << " " << it.second->to_str() << endl;
	    		ofs << i << " " << entries[it.second->global_id]->to_str(12) << endl;
	    	}
	    }

        ofs.close();
	}
};

struct Entry_Pair {
	Entry* e_query;
	Entry* e_database;
	int id_db;
	Trans xf;
	double init_dist;

	Entry_Pair(Entry* q, Entry* p, int id_db) {
		this->e_query = q;
		this->e_database = p;
		this->id_db = id_db;
	}

	string to_str(int precision = 6) const {
		return e_query->to_str(precision) + " <\n" + e_database->to_str(precision) + " >\nin DB #" + to_string(id_db);
	}

	void cal_xf() {
		Pt3D* q_array[4];
		Pt3D* p_array[4];
		q_array[0] = e_query->repre->pt;
		q_array[1] = e_query->remai[0]->pt;
		q_array[2] = e_query->remai[1]->pt;
		q_array[3] = e_query->remai[2]->pt;
		p_array[0] = e_database->repre->pt;
		p_array[1] = e_database->remai[0]->pt;
		p_array[2] = e_database->remai[1]->pt;
		p_array[3] = e_database->remai[2]->pt;
		cal_trans(q_array, p_array, 4, xf);
	}
};

////////////////////////// Toggle_1 /////////////////////////////////////
inline int window_query(IndexTree* tree, Entry* e, double err, vector<int>& ret) {
////////////////////////// Toggle_1 /////////////////////////////////////

////////////////////////// Toggle_2 /////////////////////////////////////
// inline int window_query(node_type *r_root, rtree_info* r_info, double sides[], double err,
// 	vector<RangeReturn_type*>& ret, int& page_accessed) {
////////////////////////// Toggle_2 /////////////////////////////////////

	int query_box_min[INDEX_DIM], query_box_max[INDEX_DIM];
	e->get_index_box(err, query_box_min, query_box_max);

	////////////////////////// Toggle_1 /////////////////////////////////////
	int num;
	num = tree->Search(query_box_min, query_box_max, ret);
	////////////////////////// Toggle_1 /////////////////////////////////////

	////////////////////////// Toggle_2 /////////////////////////////////////
	// RangeReturn_type* rr = NULL;
	// int num = 0;

	// page_accessed = rectangle_search(r_root, 1, &query_box_min, &query_box_max, &rr, r_info);

	// while (rr) {
	// 	num++;
	// 	ret.push_back(rr);
	// 	rr = rr->prev;
	// }
	////////////////////////// Toggle_2 /////////////////////////////////////

	return num;
}

inline bool get_est_b_c(Pt3D* m_ptr, Pt3D* a_ptr, Pt3D* h_ptr, Pt3D& b_est, Pt3D& c_est) {
	auto m = *m_ptr, a = *a_ptr, h = *h_ptr;
    auto ma = a - m;
	double d_ma_sq = ma.sq_mode();
    double d_ma = sqrt(d_ma_sq);
    double r = d_ma * 1.732050807569;
    auto mh = h - m;
    auto u = m + ma * dot_prd(&ma, &mh) / d_ma_sq;
    auto uh = h - u;
    auto d_uh = uh.mode();
    if (d_uh == 0.0)
        return false;

    b_est = m + uh * (r / uh.mode());
    auto mb_est = b_est - m;
    auto e = m + mb_est / 3.0;
    Pt3D cross; cross_prd(&ma, &mb_est, cross);

    c_est = e + cross * (0.942809041 * mb_est.mode() / cross.mode());
    
    return true;
}

inline string get_idx_filename(string grid_filename) {
	#ifdef IDX_3
		return (grid_filename + ".idx.s3");
	#else
		return (grid_filename + ".idx.s6");
	#endif
}

#endif