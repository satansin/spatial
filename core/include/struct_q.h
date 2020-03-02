#include <vector>
#include <unordered_set>
#include <string>
#include <fstream>

struct Struct_Q {
	int m;
	double epsilon, eta;

private:
	int db_mesh_id;
	double gt_err_linear, gt_err_quad;
	std::vector<int> q_db_id_map;
	// std::vector<double> drift_map;
	std::unordered_set<int> id_db_set;

public:
	void read(std::string info_filename) {
		this->q_db_id_map.clear();
		this->id_db_set.clear();

	    std::ifstream info_q_ifs(info_filename);

	    int real_inlier_count;

	    info_q_ifs >> this->m >> this->epsilon >> this->db_mesh_id;
	    // this->epsilon *= 2.0;
	    this->eta = 0.68;
	    info_q_ifs >> this->gt_err_linear >> this->gt_err_quad;

	    int q_id, db_id;
	    double drift_dist;
	    for (int i = 0; i < this->m; i++) {
	        info_q_ifs >> q_id >> db_id >> drift_dist; // in almost every case, q_id range from 0 to (m - 1)
	        if (drift_dist > this->epsilon) {
	        	db_id = -1;
	        }
	        this->q_db_id_map.push_back(db_id);
	        // this->drift_map.push_back(drift_dist);
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
			return q_db_id_map[id_q];
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
		return ("#" + std::to_string(id_q) + "(#" + std::to_string(get_id_mapping(id_q)) + ")");
	}
};