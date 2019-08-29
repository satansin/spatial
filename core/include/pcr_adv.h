#ifndef __PCR_ADV_H
#define __PCR_ADV_H

#include "point.h"
#include "trans.h"
#include <vector>
#include <string>

struct PtwID {
	int id;
	Pt3D pt;
};

struct Cell {
	int x, y, z;
	std::vector<PtwID> list;
};

struct Entry {
	PtwID repre;
	PtwID remai[3];
	double vol;
	double meas;
	bool fail;

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