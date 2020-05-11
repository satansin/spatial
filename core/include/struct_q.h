#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <fstream>

struct Struct_Q {
	int m;
	double epsilon, eta;

private:
	int db_mesh_id;
	double gt_err_linear, gt_err_quad;
	std::vector<int> id_q_db_map;
	std::vector<double> drift_map;
	std::unordered_map<int, int> id_db_q_map;

public:
	bool read(std::string info_filename) {
		this->id_q_db_map.clear();
		this->id_db_q_map.clear();

	    std::ifstream info_q_ifs(info_filename);
	    if (!info_q_ifs.is_open())
	    	return false;

	    int real_inlier_count;

	    info_q_ifs >> this->m >> this->epsilon >> this->db_mesh_id;
	    // this->epsilon *= 2.0;
	    this->eta = 0.68;
	    info_q_ifs >> this->gt_err_linear >> this->gt_err_quad;

	    int q_id, db_id;
	    double drift_dist;
	    for (int i = 0; i < this->m; i++) {
	        info_q_ifs >> q_id >> db_id >> drift_dist; // in almost every case, q_id range from 0 to (m - 1)
	        // if (drift_dist > this->epsilon) {
	        // 	db_id = -1;
	        // }
	        this->id_q_db_map.push_back(db_id);
	        this->drift_map.push_back(drift_dist);
	        if (db_id >= 0) // true for everybody
	        	this->id_db_q_map[db_id] = q_id;
	    }

	    info_q_ifs.close();

	    return true;
	}

	int get_db_mesh_id() const {
		return this->db_mesh_id;
	}

	int get_id_db_by_q(int id_q) const {
		if (id_q < 0 || id_q >= this->m) {
			return -1;
		} else {
			return id_q_db_map[id_q];
		}
	}

	double get_drift_by_q(int id_q) const {
		if (id_q < 0 || id_q >= this->m) {
			return -1.0;
		} else {
			return drift_map[id_q];
		}
	}

	int get_id_q_by_db(int id_db) const {
		auto got = this->id_db_q_map.find(id_db);
	    if (got == this->id_db_q_map.end()) {
	        return -1;
	    }
	    return got->second;
	}

	std::string id_q_to_str(int id_q) const {
		return ("#" + std::to_string(id_q) + "(#" + std::to_string(get_id_db_by_q(id_q)) + ")");
	}

	double overlapping_rate(std::unordered_set<int>& set) const {
		int num_in_common = 0;
		for (auto& v: set) {
			if (this->id_db_q_map.find(v) != this->id_db_q_map.end()) {
				num_in_common++;
			}
		}
		return (double) num_in_common / (double) std::max(this->id_db_q_map.size(), set.size());
	}
};