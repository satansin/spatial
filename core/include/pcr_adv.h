#ifndef __PCR_ADV_H
#define __PCR_ADV_H

#include "TriMesh.h"
#include "point.h"
#include "trans.h"
extern "C" {
    #include "rtree.h"
}
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
using namespace trimesh;

const char* TAB = "  ";
const char* TABTAB = "    ";
const char* TABTABTAB = "      ";

const int RSTREE_SCALE = 1e5;

struct PtwID {
	int id;
	Pt3D pt;
	void set(int id, Pt3D pt) {
		this->id = id;
		this->pt = pt;
	}
	PtwID() {
		this->set(-1, Pt3D());
	}
	PtwID(int id, Pt3D pt) {
		this->set(id, pt);
	}
	PtwID(int id, const TriMesh* mesh) {
		this->set(id, get_pt(mesh, id));
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
		this->set(min_id_x, min_id_y, min_id_z, max_id_x, max_id_y, max_id_z, empty);
	}
	Box3D_Int& operator=(const Box3D_Int& b) {
        this->set(b.min_id[0], b.min_id[1], b.min_id[2], b.max_id[0], b.max_id[1], b.max_id[2], b.empty);
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
		for (int i = this->min_id[0]; i <= this->max_id[0]; i++) {
            for (int j = this->min_id[1]; j <= this->max_id[1]; j++) {
                for (int k = this->min_id[2]; k <= this->max_id[2]; k++) {
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
		os << this->min_id[0] << " " << this->min_id[1] << " " << this->min_id[2] << " "
	       << this->max_id[0] << " " << this->max_id[1] << " " << this->max_id[2];
	}
	void read(istream& is) {
		is >> this->min_id[0] >> this->min_id[1] >> this->min_id[2]
	       >> this->max_id[0] >> this->max_id[1] >> this->max_id[2];
	}
};

struct Cell {
	int global_id;
	int x, y, z;
	vector<PtwID> list;
	Cell() {}
	Cell(int x, int y, int z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	void set_global_id(int global_id) {
		this->global_id = global_id;
	}
	void add_pt(PtwID p) {
		list.push_back(p);
	}
	void add_pt(int id, Pt3D pt) {
		list.push_back(PtwID(id, pt));
	}
	PtwID find_nn(Pt3D q) const {
		PtwID nn;
		double nn_dist = numeric_limits<double>::max();
		for (auto &p: this->list) {
			double tmp_dist = eucl_dist(p.pt, q);
			if (tmp_dist - nn_dist < 0) {
				nn = p;
				nn_dist = tmp_dist;
			}
		}
		return nn;
	}
	string to_str() const {
		string s =
			to_string(global_id) + " " +
			to_string(x) + " " +
			to_string(y) + " " +
			to_string(z) + " " +
			to_string(list.size());
		for (auto &p: list) {
		 	s = s + " " + to_string(p.id);
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
    	int key = get_cell_key(cell_id, this->cells_box);
    	auto it = this->cells_map.find(key);
        if (it != this->cells_map.end()) {
        	return it->second;
        } else {
        	return nullptr;
        }
    }
    void gridify(TriMesh* mesh) {
    	int n = mesh->vertices.size();
    	mesh->need_bbox();
    	for (int i = 0; i < 3; i++) {
            cells_box->min_id[i] = get_cell_id(mesh->bbox.min[i], w);
            cells_box->max_id[i] = get_cell_id(mesh->bbox.max[i], w);
        }
    	cells_map.clear();
        for (int i = 0; i < n; i++) {
        	auto pt3d = pt(mesh->vertices[i]);
            auto cell_id = get_cell_id(pt3d, this->w);
            int key = get_cell_key(cell_id, cells_box);
            auto it = cells_map.find(key);
            if (it != cells_map.end()) {
                // cell already exists
                it->second->add_pt(i, pt3d);
            } else {
                Cell* new_cell = new Cell(cell_id[0], cell_id[1], cell_id[2]);
                new_cell->add_pt(i, pt3d);
                cells_map[key] = new_cell;
            }
        }
        // cells_count = cells_map.size();
    }
	Box3D_Int get_outer_box(double min_x, double min_y, double min_z, double max_x, double max_y, double max_z) const {
		return Box3D_Int{
	        get_cell_id(min_x, this->w),
	        get_cell_id(min_y, this->w),
	        get_cell_id(min_z, this->w),
	        get_cell_id(max_x, this->w),
	        get_cell_id(max_y, this->w),
	        get_cell_id(max_z, this->w),
	        false // default non-empty
		};
	}
	PtwID relocate(const float* pt) const {
		Pt3D pt3d = { pt[0], pt[1], pt[2] };
		auto cell_id = get_cell_id(pt3d, this->w);
		auto cell_found = find_cell(cell_id);
		if (cell_found) {
			return cell_found->find_nn(pt3d);
		} else {
			return {-1, Pt3D()};
		}
	}
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
	PtwID* repre;
	PtwID* remai[3];
	double vol;
	double meas;
	bool fail;
	PtwID* help;
	double sides[6];
	// make it compatible with old method
	void set(PtwID repre, PtwID remai_0, PtwID remai_1, PtwID remai_2, double vol, double meas) {
		*(this->repre) = repre;
		*(this->remai[0]) = remai_0;
		*(this->remai[1]) = remai_1;
		*(this->remai[2]) = remai_2;
		this->vol = vol;
		this->meas = meas;
	}
	void set(PtwID repre, PtwID remai_0, PtwID remai_1, PtwID remai_2, double vol, double meas, PtwID help) {
		*(this->repre) = repre;
		*(this->remai[0]) = remai_0;
		*(this->remai[1]) = remai_1;
		*(this->remai[2]) = remai_2;
		this->vol = vol;
		this->meas = meas;
		*(this->help) = help;
	}
	Entry() {
        this->mem_alloc();
        this->vol = -1;
    	this->meas = -1;
    	this->fail = false;
    	for (int i = 0; i < 6; i++) {
    		sides[i] = 0;
    	}
	}
	Entry(PtwID repre, PtwID remai_0, PtwID remai_1, PtwID remai_2, double vol, double meas, PtwID help) {
        this->mem_alloc();
		this->set(repre, remai_0, remai_1, remai_2, vol, meas, help);
		this->fail = false;
	}
	Entry(PtwID repre, PtwID remai_0, PtwID remai_1, PtwID remai_2, PtwID help) {
        this->mem_alloc();
		this->set(repre, remai_0, remai_1, remai_2, -1, -1, help);
		this->fail = false;
	}
	void read_from(istream& is, const TriMesh* mesh_db) {
		int repre_id, help_id;
		int remai_id[3];
	    double vol, meas;
	    bool fail;
		is >> repre_id;
		for (int i = 0; i < 3; i++) {
			is >> remai_id[i];
		}
		is >> vol >> meas >> fail >> help_id;
	    this->set(
	    	PtwID(repre_id, mesh_db),
	    	PtwID(remai_id[0], mesh_db),
	    	PtwID(remai_id[1], mesh_db),
	    	PtwID(remai_id[2], mesh_db),
            vol, meas,
            PtwID(help_id, mesh_db)
        );
	    this->fail = fail;

		for (int i = 0; i < 6; i++) {
			is >> this->sides[i];
		}
	}
	void fill_sides() {
		sides[0] = eucl_dist(this->repre->pt, this->remai[0]->pt);
		sides[1] = eucl_dist(this->repre->pt, this->remai[1]->pt);
		sides[2] = eucl_dist(this->repre->pt, this->remai[2]->pt);
		sides[3] = eucl_dist(this->remai[0]->pt, this->remai[1]->pt);
		sides[4] = eucl_dist(this->remai[0]->pt, this->remai[2]->pt);
		sides[5] = eucl_dist(this->remai[1]->pt, this->remai[2]->pt);
	}
	void sort_sides() {
		int prim = this->primary_remai();
		if (prim != 0) {
			swap(this->remai[0], this->remai[prim]);
			swap(this->sides[0], this->sides[prim]);
			swap(this->sides[5], this->sides[5 - prim]);
		}

		double dec = dot_prd(this->remai[0]->pt - this->repre->pt, cross_prd(this->remai[1]->pt - this->repre->pt, this->remai[2]->pt - this->repre->pt));
		if (dec > 0) {
			swap(this->remai[1], this->remai[2]);
			swap(this->sides[1], this->sides[2]);
			swap(this->sides[3], this->sides[4]);
		}
	}
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
		if (this->sides[0] <= this->sides[1])
			if (this->sides[0] <= this->sides[2])
				return 0;
			else
				return 2;
		else
			if (this->sides[1] <= this->sides[2])
				return 1;
			else
				return 2;
	}
	void mem_alloc() {
        this->repre = new PtwID;
        for (int i = 0; i < 3; i++) {
        	this->remai[i] = new PtwID;
        }
    	this->help = new PtwID;
	}
};

void sort_remai(Entry& e) {
    double bc = eucl_dist(e.remai[0]->pt, e.remai[1]->pt);
    double bd = eucl_dist(e.remai[0]->pt, e.remai[2]->pt);
    double cd = eucl_dist(e.remai[1]->pt, e.remai[2]->pt);
    if (cd < bd) {
        swap(e.remai[0], e.remai[1]);
        swap(cd, bd);
    }
    if (bc > cd) {
        swap(e.remai[0], e.remai[2]);
        swap(e.remai[1], e.remai[2]);
    } else if (bc > bd) {
        swap(e.remai[1], e.remai[2]);
    }
}

class DB {
private:

};

struct Struct_DB {
private:
	vector<Grid*> grids;

	double w;
	double ann_min;
	double ann_max;

	vector<Entry*> entries;

	vector<unordered_map<int, int>> reverse_entries_maps;
	vector<unordered_set<int>> repre_id_sets;

	vector<int> global_id_map;

	int total_cells_count; // accumulative

public:
	Struct_DB() {
		total_cells_count = 0;
	}

	void set_w(double w) {
		this->w = w;
	}
	double get_w() const {
		return this->w;
	}

	void set_ann(double min, double max) {
		this->ann_min = min;
		this->ann_max = max;
	}
	double get_ann_min() const {
		return this->ann_min;
	}
	double get_ann_max() const {
		return this->ann_max;
	}

	int get_total_cells_count() const {
		return this->total_cells_count;
	}

	void append_grid(Grid* g) {
		grids.push_back(g);
		this->total_cells_count += g->cells_map.size();
	}
	const vector<Grid*>& get_grids() const {
		return this->grids;
	}

	void append_entry(Entry* e) {
		entries.push_back(e);
	}
	const vector<Entry*>& get_entries() const {
		return entries;
	}

	void read(string filename, const vector<TriMesh*>& db_meshes) {
	    ifstream ifs(filename);

	    int num_grids;
	    ifs >> this->w >> this->ann_min >> this->ann_max >> num_grids >> this->total_cells_count;

	    this->grids.reserve(num_grids);
	    for (int i = 0; i < num_grids; i++) {
	    	int id;
	    	ifs >> id;

	    	Grid* g = new Grid(this->w);
		    ifs >> g->cells_box->min_id[0] >> g->cells_box->min_id[1] >> g->cells_box->min_id[2]
		        >> g->cells_box->max_id[0] >> g->cells_box->max_id[1] >> g->cells_box->max_id[2];

		    this->grids.push_back(g);
	    }
	    this->grids.shrink_to_fit();

	    reverse_entries_maps.resize(num_grids);
	    repre_id_sets.resize(num_grids);

	    for (int i = 0; i < this->total_cells_count; i++) {
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
	            c->add_pt(pt_id, pt(db_meshes.at(grid_id)->vertices[pt_id]));
	        }
	        this->grids[grid_id]->cells_map[key] = c;

	        ifs >> grid_id; // another grid_id has been saved
	        Entry *e = new Entry;
	        e->read_from(ifs, db_meshes.at(grid_id));

	        this->entries.push_back(e);

	        this->repre_id_sets[grid_id].insert(e->repre->id);
	        this->reverse_entries_maps[grid_id][e->repre->id] = cell_id;

	        this->global_id_map.push_back(grid_id);
	    }

	    ifs.close();
	}

	Entry* get_entry(int key) const {
		return entries[key];
	}

	int get_grid_id_by_global_cell_id(int global_cell_id) const {
		return global_id_map[global_cell_id];
	}

	bool look_up_repre_index(int repre_id, int mesh_id) const {
	    auto got = this->repre_id_sets[mesh_id].find(repre_id);
	    if (got != this->repre_id_sets[mesh_id].end()) {
	        return true;
	    }
	    return false;
	}

	int get_remai_id(int repre_id, int i, int mesh_id) const {
		return this->entries[this->reverse_entries_maps[mesh_id].at(repre_id)]->remai[i]->id;
	}

	int get_help_id(int repre_id, int mesh_id) const {
		return this->entries[this->reverse_entries_maps[mesh_id].at(repre_id)]->help->id;
	}

	void save(string filename) const {
		ofstream ofs(filename);

		// write grid headers
		ofs << this->w << " " << this->ann_min << " " << this->ann_max << " " << this->grids.size() << " " << this->total_cells_count << endl;

		for (int i = 0; i < this->grids.size(); i++) {
			ofs << i << " ";
			this->grids[i]->cells_box->save(ofs);
			ofs << endl;
	    }

		for (int i = 0; i < this->grids.size(); i++) {
	    	for (auto &it: this->grids[i]->cells_map) {
	    		ofs << i << " " << it.first << " " << it.second->to_str() << endl;
	    		ofs << i << " " << entries[it.second->global_id]->to_str(12) << endl;
	    	}
	    }

        ofs.close();
	}
};

struct Struct_Q {
	Grid* g_q;
	int m;
	double epsilon, eta;

private:
	int db_mesh_id;
	double gt_err_linear, gt_err_quad;
	unordered_map<int, int> q_db_id_map;
	unordered_set<int> id_db_set;

public:
	void read(string info_filename) {
		this->q_db_id_map.clear();
		this->id_db_set.clear();

	    ifstream info_q_ifs(info_filename);

	    int real_inlier_count;

	    info_q_ifs >> this->m >> this->epsilon >> this->eta >> this->db_mesh_id;
	    info_q_ifs >> real_inlier_count >> this->gt_err_linear >> this->gt_err_quad;

	    int q_id, db_id;
	    for (int i = 0; i < this->m; i++) {
	        info_q_ifs >> q_id >> db_id; // in almost every case, q_id range from 0 to (m - 1)
	        this->q_db_id_map[q_id] = db_id;
	        if (db_id >= 0)
	        	this->id_db_set.insert(db_id);
	    }

	    info_q_ifs.close();
	}

	int get_db_mesh_id() const {
		return this->db_mesh_id;
	}

	int get_id_mapping(int id_q) const {
		if (id_q < 0 || id_q >= this->m) {
			return -1;
		} else {
			return q_db_id_map.at(id_q);
		}
	}

	bool look_up_id_db(int id_db) const {
		auto got = this->id_db_set.find(id_db);
	    if (got != this->id_db_set.end()) {
	        return true;
	    }
	    return false;
	}

	string id_q_to_str(int id_q) const {
		return ("#" + to_string(id_q) + "(#" + to_string(get_id_mapping(id_q)) + ")");
	}
};

struct Entry_Pair {
	Entry* e_query;
	Entry* e_database;
	int id_db;
	Trans xf;

	Entry_Pair(Entry* q, Entry* p, int id_db) {
		this->e_query = q;
		this->e_database = p;
		this->id_db = id_db;
	}

	string to_str(int precision = 6) const {
		return this->e_query->to_str(precision) + " <\n" + this->e_database->to_str(precision) + " >\nin DB #" + to_string(this->id_db);
	}

	void cal_xf() {
		Pt3D q_array[4], p_array[4];
		q_array[0] = e_query->repre->pt;
		q_array[1] = e_query->remai[0]->pt;
		q_array[2] = e_query->remai[1]->pt;
		q_array[3] = e_query->remai[2]->pt;
		p_array[0] = e_database->repre->pt;
		p_array[1] = e_database->remai[0]->pt;
		p_array[2] = e_database->remai[1]->pt;
		p_array[3] = e_database->remai[2]->pt;
		xf = cal_trans(q_array, p_array, 4);
	}
};

inline double sq(double d) {
	return (d * d);
}

inline void convert_pt(Pt3D* p, R_TYPE*& ret) {
    ret = (R_TYPE *) malloc(sizeof(R_TYPE) * 3);
    ret[0] = (int) (p->x * RSTREE_SCALE);
    ret[1] = (int) (p->y * RSTREE_SCALE);
    ret[2] = (int) (p->z * RSTREE_SCALE);
}

inline NN_type** nn_sphere(Pt3D* p, node_type *r_root, rtree_info* r_info, double sq_dist,
	const unordered_set<int>& excl_id_set = {}, int k = 1) {
	
	R_TYPE *query;
    convert_pt(p, query);
    NN_type *nn;
    int real_k = excl_id_set.size() + k;

    if (sq_dist == 0.0) {
    	k_NN_search(r_root, query, real_k, &nn, r_info);
    } else {
    	long long sq_dist_long = (long long) (sq_dist * RSTREE_SCALE * RSTREE_SCALE);
    	k_NN_search_sphere(r_root, query, real_k, &nn, r_info, sq_dist_long);
    }

    NN_type *nn_copy = nn;

    stack<NN_type*> s;
    for (int i = 0; i < real_k; i++) {
    	s.push(nn);
    	nn = nn->next;
    }

    NN_type** ret = new NN_type*[k];
    int i = 0;
    while (i < k) {
    	auto top = s.top();
    	s.pop();
    	if (excl_id_set.find(top->oid) == excl_id_set.end()) { // not in the exclusive list
    		ret[i] = (NN_type*) malloc(sizeof(NN_type));
    		memcpy(ret[i], top, sizeof(NN_type));
    		i++;
    	}
    }

    NN_freeChain(nn_copy);

	return ret;
}

inline void nn_sphere_free(NN_type** nn_ret, int k = 1) {
	for (int i = 0; i < k; i++) {
		free(nn_ret[i]);
	}
	delete[] nn_ret;
}

inline void range_sphere(Pt3D* p, node_type *r_root, rtree_info* r_info, double sq_dist_min, double sq_dist_max,
	vector<RangeReturn_type*>& ret, const unordered_set<int>& excl_id_set = {}) {

	R_TYPE *query;
    convert_pt(p, query);
	RangeReturn_type* rr = NULL;

	long long sq_dist_min_long = (long long) (sq_dist_min * RSTREE_SCALE * RSTREE_SCALE);
	long long sq_dist_max_long = (long long) (sq_dist_max * RSTREE_SCALE * RSTREE_SCALE);
	sphere_search(r_root, query, sq_dist_min_long, sq_dist_max_long, &rr, r_info);

	while (rr) {
		if (excl_id_set.find(rr->oid) == excl_id_set.end()) { // not in the exclusive list
			ret.push_back(rr);
		}
		rr = rr->prev;
	}
}

inline void range_sphere(Pt3D* p, node_type *r_root, rtree_info* r_info, long long sq_dist_min_long, long long sq_dist_max_long,
	vector<RangeReturn_type*>& ret, const unordered_set<int>& excl_id_set = {}) {

	R_TYPE *query;
    convert_pt(p, query);
	RangeReturn_type* rr = NULL;

	sphere_search(r_root, query, sq_dist_min_long, sq_dist_max_long, &rr, r_info);

	while (rr) {
		if (excl_id_set.find(rr->oid) == excl_id_set.end()) { // not in the exclusive list
			ret.push_back(rr);
		}
		rr = rr->prev;
	}
}

inline void range_free(vector<RangeReturn_type*>& range_ret) {
	for (auto &p: range_ret) {
		free(p);
	}
	range_ret.clear();
}

// for 1-nn only
inline NN_type** nn_sphere_range(Pt3D* p, node_type *r_root, rtree_info* r_info, double sq_dist, double err,
	vector<RangeReturn_type*>& ret, const unordered_set<int>& excl_id_set = {}) {

	auto nn_ret = nn_sphere(p, r_root, r_info, sq_dist, excl_id_set);

	long long min_long = nn_ret[0]->dist;
	double dist = sqrt((double) min_long);
	long long max_long = sq(dist + err * 2 * RSTREE_SCALE);

	range_sphere(p, r_root, r_info, min_long, max_long, ret, excl_id_set);

	return nn_ret;
}

// for 1-nn only
inline NN_type** nn_sphere_range_verbose(Pt3D* p, node_type *r_root, rtree_info* r_info, double sq_dist, double err, string nav,
	vector<RangeReturn_type*>& ret, const unordered_set<int>& excl_id_set = {}) {

	auto nn_ret = nn_sphere(p, r_root, r_info, sq_dist, excl_id_set);

	long long min_long = nn_ret[0]->dist;
	double dist = sqrt((double) min_long);
	long long max_long = sq(dist + err * 2 * RSTREE_SCALE);

	cout << nav << "min=" << min_long << endl;
	cout << nav << "max=" << max_long << endl;

	range_sphere(p, r_root, r_info, min_long, max_long, ret, excl_id_set);

	return nn_ret;
}

inline int window_query(node_type *r_root, rtree_info* r_info, double sides[], double err,
	vector<RangeReturn_type*>& ret, int& page_accessed) {

	R_TYPE* query_min = (R_TYPE *) malloc(sizeof(R_TYPE) * 6);
	R_TYPE* query_max = (R_TYPE *) malloc(sizeof(R_TYPE) * 6);
	for (int i = 0; i < 6; i++) {
		query_min[i] = (int) ((sides[i] - err) * RSTREE_SCALE);
		query_max[i] = (int) ((sides[i] + err) * RSTREE_SCALE);
		// cout << "[" << query_min[i] << ", " << query_max[i] << "]" << endl;
	}
	RangeReturn_type* rr = NULL;

	page_accessed = rectangle_search(r_root, 1, &query_min, &query_max, &rr, r_info);

	free(query_min);
	free(query_max);

	int num = 0;

	while (rr) {
		num++;
		ret.push_back(rr);
		rr = rr->prev;
	}

	return num;
}

inline int window_query_bundle(node_type *r_root, rtree_info* r_info, const vector<Entry*>& e_list, double err, double bbox_min[], double bbox_max[],
	vector<BundleReturn_type*>& ret, int& page_accessed) {

	int e_count = e_list.size();

	R_TYPE** query = (R_TYPE **) malloc(sizeof(R_TYPE *) * e_count);
	for (int i = 0; i < e_count; i++) {
		R_TYPE* ind_query = (R_TYPE *) malloc(sizeof(R_TYPE) * 6);
		for (int j = 0; j < 6; j++) {
			ind_query[j] = e_list[i]->sides[j];
		}
		query[i] = ind_query;
	}

	BundleReturn_type* br = NULL;

	float* min = (float *) malloc(sizeof(float) * 6);
	float* max = (float *) malloc(sizeof(float) * 6);
	for (int i = 0; i < 6; i++) {
		min[i] = bbox_min[i];
		max[i] = bbox_max[i];
	}

	page_accessed = rectangle_search_bundle(r_root, e_count, query, err, min, max, &br, r_info);

	for (int i = 0; i < e_count; i++) {
		free(query[i]);
	}
	free(query);
	free(min);
	free(max);

	int num = 0;

	while (br) {
		num++;
		ret.push_back(br);
		br = br->prev;
	}

	return num;
}

inline bool get_est_b_c(Pt3D* m_ptr, Pt3D* a_ptr, Pt3D* h_ptr, Pt3D& b_est, Pt3D& c_est) {
	auto m = *m_ptr, a = *a_ptr, h = *h_ptr;
    auto ma = a - m;
	double d_ma_sq = ma.sq_mode();
    double d_ma = sqrt(d_ma_sq);
    double r = d_ma * 1.732050807569;
    auto mh = h - m;
    auto u = m + ma * dot_prd(ma, mh) / d_ma_sq;
    auto uh = h - u;
    auto d_uh = uh.mode();
    if (d_uh == 0.0)
        return false;

    b_est = m + uh * (r / uh.mode());
    auto mb_est = b_est - m;
    auto e = m + mb_est / 3.0;
    auto cross = cross_prd(ma, mb_est);

    c_est = e + cross * (0.942809041 * mb_est.mode() / cross.mode());

	// Pt3D std_coor[3] = { {0, 0, 0}, {r, 0, 0}, {0, 0, d_ma} }; // put m, b_est, a in a righthand coordinate system
	// Pt3D real_coor[3] = { m, b_est, a.pt };
	// auto xf = cal_trans(std_coor, real_coor, 3);
	// Pt3D c_est = trans_pt(&xf, { r / 3.0, r * 0.94280904, 0 }); // rotate b_est arccos(1/3) towards y+ surrounding m
    
    return true;

}

inline rtree_info read_rstree_info(string filename) {
	ifstream ifs(filename);
	rtree_info ret;
	ifs >> ret.m >> ret.M >> ret.dim >> ret.reinsert_p;
	ifs.close();
	return ret;
}

inline string get_foldername(string path) {
    string ret;
    if (path[path.length() - 1] != '/') {
        ret = path + "/";
    } else {
        ret = path;
    }
    return ret;
}

inline int read_db_mesh_batch(string db_path, vector<TriMesh*>& db_meshes) {
    string db_folder = get_foldername(db_path);

    ifstream ifs(db_folder + "meta.txt");

    int num;
    ifs >> num;

    int id;
    string s_file;
    for (int i = 0; i < num; i++) {
        ifs >> id >> s_file;
        db_meshes.push_back(TriMesh::read(s_file));
    }

    ifs.close();

    return num;
}

inline int read_db_batch(string db_path, vector<TriMesh*>& db_meshes, rtree_info* db_rtree_info, vector<node_type*>& db_roots) {
    string db_folder = get_foldername(db_path);

    ifstream ifs(db_folder + "meta.txt");

    int num;
    ifs >> num;

    int id;
    string s_file;
    for (int i = 0; i < num; i++) {
        ifs >> id >> s_file;
        db_meshes.push_back(TriMesh::read(s_file));

        node_type* a_root;
        read_rtree(&a_root, string(s_file + ".rstree.1").c_str(), db_rtree_info);

        db_roots.push_back(a_root);
    }

    ifs.close();

    return num;
}

#endif