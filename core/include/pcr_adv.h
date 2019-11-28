#ifndef __PCR_ADV_H
#define __PCR_ADV_H

#include "TriMesh.h"
#include "point.h"
#include "trans.h"
extern "C" {
    #include "rtree.h"
}
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <stack>

using namespace std;
using namespace trimesh;

const char* TAB = "  ";
const char* TABTAB = "    ";
const char* TABTABTAB = "      ";

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
};

struct Cell {
	int x, y, z;
	vector<PtwID> list;
	Cell() {}
	Cell(int x, int y, int z) {
		this->x = x;
		this->y = y;
		this->z = z;
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
    int cells_count;
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
        cells_count = cells_map.size();
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
	Entry() {
        this->repre = new PtwID;
        for (int i = 0; i < 3; i++) {
        	this->remai[i] = new PtwID;
        }
        vol = -1;
    	meas = -1;
    	fail = false;
    	this->help = new PtwID;
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
	Entry(PtwID repre, PtwID remai_0, PtwID remai_1, PtwID remai_2, double vol, double meas, PtwID help) {
        this->repre = new PtwID;
        for (int i = 0; i < 3; i++) {
        	this->remai[i] = new PtwID;
        }
    	this->help = new PtwID;
		set(repre, remai_0, remai_1, remai_2, vol, meas, help);
	}
	// make it compatible with old method
	void set(PtwID repre, PtwID remai_0, PtwID remai_1, PtwID remai_2, double vol, double meas) {
		*(this->repre) = repre;
		*(this->remai[0]) = remai_0;
		*(this->remai[1]) = remai_1;
		*(this->remai[2]) = remai_2;
		this->vol = vol;
		this->meas = meas;
	}
	string to_str() const {
		return
			to_string(repre->id) + " " +
			to_string(remai[0]->id) + " " +
			to_string(remai[1]->id) + " " +
			to_string(remai[2]->id) + " " +
			to_string(vol) + " " +
			to_string(meas) + " " +
			to_string(fail) + " " +
			to_string(help->id);
	}
	virtual ~Entry() {
		delete repre;
        for (int i = 0; i < 3; i++) {
        	delete remai[i];
        }
		delete help;
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

struct Struct_DB {
	Grid* g_db;
	double ann_min;
	double ann_max;
	unordered_map<int, Entry*> entries_map;
	unordered_map<int, int> reverse_entries_map;
	unordered_set<int> repre_id_set;

	void insert_entry(int key, Entry* e) {
		entries_map[key] = e;
	}

	void read(string filename, TriMesh* mesh_db) {
	    ifstream ifs(filename);

	    ifs >> this->g_db->w >> this->ann_min >> this->ann_max >> this->g_db->cells_count;
	    ifs >> this->g_db->cells_box->min_id[0] >> this->g_db->cells_box->min_id[1] >> this->g_db->cells_box->min_id[2]
	        >> this->g_db->cells_box->max_id[0] >> this->g_db->cells_box->max_id[1] >> this->g_db->cells_box->max_id[2];

	    for (int i = 0; i < this->g_db->cells_count; i++) {
	        int key, x, y, z, list_size;
	        ifs >> key >> x >> y >> z >> list_size;
	        
	        Cell* c = new Cell(x, y, z);
	        for (int j = 0; j < list_size; j++) {
	            int pt_id; ifs >> pt_id;
	            c->add_pt(pt_id, pt(mesh_db->vertices[pt_id]));
	        }
	        this->g_db->cells_map[key] = c;

	        int repre_id, remai_0_id, remai_1_id, remai_2_id, help_id;
	        double vol, meas;
	        bool fail;
	        ifs >> repre_id >> remai_0_id >> remai_1_id >> remai_2_id >> vol >> meas >> fail >> help_id;

	        this->repre_id_set.insert(repre_id);
	        this->reverse_entries_map[repre_id] = key;

	        Entry *e = new Entry;
	        e->set(PtwID(repre_id, mesh_db),
	               PtwID(remai_0_id, mesh_db),
	               PtwID(remai_1_id, mesh_db),
	               PtwID(remai_2_id, mesh_db),
	               vol, meas,
	               PtwID(help_id, mesh_db));
	        e->fail = fail;
	        this->entries_map[key] = e;
	    }

	    ifs.close();
	}

	Entry* get_entry(int key) const {
		return entries_map.at(key);
	}

	bool look_up_repre_index(int repre_id) const {
	    auto got = this->repre_id_set.find(repre_id);
	    if (got != this->repre_id_set.end()) {
	        return true;
	    }
	    return false;
	}

	int get_remai_id(int repre_id, int i) const {
		return this->entries_map.at(this->reverse_entries_map.at(repre_id))->remai[i]->id;
	}

	int get_help_id(int repre_id) const {
		return this->entries_map.at(this->reverse_entries_map.at(repre_id))->help->id;
	}

	void set_ann(double min, double max) {
		this->ann_min = min;
		this->ann_max = max;
	}

	void save(string filename) const {
		ofstream ofs(filename);

		// write grid headers
		ofs << this->g_db->w << " " << this->ann_min << " " << this->ann_max << " " << this->g_db->cells_count << endl;
        ofs << this->g_db->cells_box->min_id[0] << " " << this->g_db->cells_box->min_id[1] << " " << this->g_db->cells_box->min_id[2] << " "
            << this->g_db->cells_box->max_id[0] << " " << this->g_db->cells_box->max_id[1] << " " << this->g_db->cells_box->max_id[2] << endl;

        for (auto &it: this->g_db->cells_map) {
        	ofs << it.first << " " << it.second->to_str() << endl;
        	ofs << entries_map.at(it.first)->to_str() << endl;
        }

        ofs.close();
	}
};

struct Struct_Q {
	Grid* g_q;
	int m;
	double epsilon, eta;
	vector<int> idmap;
	unordered_set<int> idmap_set;

	void read(string info_filename, string idmap_filename, TriMesh* mesh_q) {
		this->idmap.clear();
		this->idmap_set.clear();

	    ifstream info_q_ifs(info_filename);
	    ifstream idmap_ifs(idmap_filename);

	    info_q_ifs >> this->m;
	    vector<Pt3D> pts_q;
	    for (int i = 0; i < m; i++) {
	        pts_q.push_back(pt(mesh_q->vertices[i]));
	        int db_mapping;
	        idmap_ifs >> db_mapping;
	        this->idmap.push_back(db_mapping);
	        this->idmap_set.insert(db_mapping);
	    }

	    info_q_ifs >> this->epsilon >> this->eta;

	    info_q_ifs.close();
	    idmap_ifs.close();
	}

	int get_id_mapping(int id_q) const {
		if (id_q < 0 || id_q >= this->m) {
			return -1;
		} else {
			return idmap[id_q];
		}
	}

	bool look_up_idmap(int idmap) const {
		auto got = this->idmap_set.find(idmap);
	    if (got != this->idmap_set.end()) {
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
	Trans xf;

	Entry_Pair(Entry* q, Entry* p) {
		e_query = q;
		e_database = p;
	}

	string to_str() const {
		return e_query->to_str() + " & " + e_database->to_str();
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

// inline double sq_max(double sq_min, double epsilon) {
// 	return ((epsilon == 0) ? sq_min : sq(sqrt(sq_min) + epsilon * 2.0));
// }

inline void convert_pt(Pt3D* p, R_TYPE*& ret) {
    ret = (R_TYPE *) malloc(sizeof(R_TYPE) * 3);
    ret[0] = p->x;
    ret[1] = p->y;
    ret[2] = p->z;
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
    	k_NN_search_sphere(r_root, query, real_k, &nn, r_info, sq_dist);
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

	sphere_search(r_root, query, sq_dist_min, sq_dist_max, &rr, r_info);

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

	double dist = sqrt(nn_ret[0]->dist);
	double min = sq(dist - err);
	double max = sq(dist + err);

	range_sphere(p, r_root, r_info, min, max, ret, excl_id_set);

	return nn_ret;
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

#endif