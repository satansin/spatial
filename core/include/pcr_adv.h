#ifndef __PCR_ADV_H
#define __PCR_ADV_H

#include "TriMesh.h"
#include "point.h"
#include "trans.h"
#include <vector>
#include <string>
#include <cmath>
#include <unordered_map>

struct PtwID {
	int id;
	Pt3D pt;
	PtwID() {}
	PtwID(int id, Pt3D pt) {
		this->id = id;
		this->pt = pt;
	}
};

struct Cell {
	int x, y, z;
	std::vector<PtwID> list;
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
	std::string to_str() {
		std::string s =
			std::to_string(x) + " " +
			std::to_string(y) + " " +
			std::to_string(z) + " " +
			std::to_string(list.size());
		for (PtwID &p: list) {
		 	s = s + " " + std::to_string(p.id);
		}
		return s;
	}
};

int get_cell_id(double val, double w) {
    return (int) floor(val / w);
}

int get_cell_key(const int cell_id[], const int min_id[], const int max_id[]) {
    int span[3] = { max_id[0] - min_id[0] + 1, max_id[1] - min_id[1] + 1, max_id[2] - min_id[2] + 1 };
    return ( (cell_id[0] - min_id[0]) * span[1] * span[2] +
             (cell_id[1] - min_id[1]) * span[2] +
             (cell_id[2] - min_id[2]) );
}

struct Grid {
	double w;
	int cell_id_min[3], cell_id_max[3];
    std::unordered_map<int, Cell> cells_map;
    int cells_count;
    Grid() {}
    Grid(double w) {
    	this->w = w;
    }
    void set_width(double w) {
    	this->w = w;
    }
    void gridify(trimesh::TriMesh* mesh) {
    	int n = mesh->vertices.size();
    	mesh->need_bbox();
    	for (int zero_to_two = 0; zero_to_two < 3; zero_to_two++) {
            cell_id_min[zero_to_two] = get_cell_id(mesh->bbox.min[zero_to_two], w);
            cell_id_max[zero_to_two] = get_cell_id(mesh->bbox.max[zero_to_two], w);
        }
    	cells_map.clear();
        for (int i = 0; i < n; i++) {
        	auto pt3d = pt(mesh->vertices[i]);
            int cell_id[3] = {
                get_cell_id(pt3d.x, w),
                get_cell_id(pt3d.y, w),
                get_cell_id(pt3d.z, w)    
            };

            int key = get_cell_key(cell_id, cell_id_min, cell_id_max);

            auto it = cells_map.find(key);
            if (it != cells_map.end()) {
                // cell already exists
                it->second.add_pt(i, pt3d);
            } else {
                Cell new_cell(cell_id[0], cell_id[1], cell_id[2]);
                new_cell.add_pt(i, pt3d);
                cells_map[key] = new_cell;
            }
        }
        cells_count = cells_map.size();
    }

};

struct Entry {
	PtwID repre;
	PtwID remai[3];
	double vol;
	double meas;
	bool fail;
	Entry() {
        repre.id = -1;
        remai[0].id = -1;
        remai[1].id = -1;
        remai[2].id = -1;
        vol = -1;
    	meas = -1;
    	fail = false;
	}
	void set(PtwID repre, PtwID remai_0, PtwID remai_1, PtwID remai_2, double vol, double meas) {
		this->repre = repre;
		this->remai[0] = remai_0;
		this->remai[1] = remai_1;
		this->remai[2] = remai_2;
		this->vol = vol;
		this->meas = meas;
	}
	std::string to_str() {
		return
			std::to_string(repre.id) + " " +
			std::to_string(remai[0].id) + " " +
			std::to_string(remai[1].id) + " " +
			std::to_string(remai[2].id) + " " +
			std::to_string(vol) + " " +
			std::to_string(meas) + " " +
			std::to_string(fail);
	}
};

void sort_remai(Entry& e) {
    double bc = eucl_dist(e.remai[0].pt, e.remai[1].pt);
    double bd = eucl_dist(e.remai[0].pt, e.remai[2].pt);
    double cd = eucl_dist(e.remai[1].pt, e.remai[2].pt);
    if (cd < bd) {
        std::swap(e.remai[0], e.remai[1]);
        std::swap(cd, bd);
    }
    if (bc > cd) {
        std::swap(e.remai[0], e.remai[2]);
        std::swap(e.remai[1], e.remai[2]);
    } else if (bc > bd) {
        std::swap(e.remai[1], e.remai[2]);
    }
}

struct Entry_Pair {
	Entry e_query, e_database;
	Trans xf;

	Entry_Pair(Entry q, Entry p) {
		e_query = q;
		e_database = p;
	}

	std::string to_str() {
		return e_query.to_str() + " & " + e_database.to_str();
	}

	void cal_xf() {
		Pt3D q_array[4], p_array[4];
		q_array[0] = e_query.repre.pt;
		q_array[1] = e_query.remai[0].pt;
		q_array[2] = e_query.remai[1].pt;
		q_array[3] = e_query.remai[2].pt;
		p_array[0] = e_database.repre.pt;
		p_array[1] = e_database.remai[0].pt;
		p_array[2] = e_database.remai[1].pt;
		p_array[3] = e_database.remai[2].pt;
		xf = cal_trans(q_array, p_array, 4);
	}
};

#endif