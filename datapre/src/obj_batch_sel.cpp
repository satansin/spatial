#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unistd.h>
#include <unordered_set>

#include "util.h"

using namespace std;


void sel_rand_numbers_from_range(int range_start, int range_end, int num, const unordered_set<int>& spec_set, vector<int>& ret) {
	vector<int> range;
	for (int i = range_start; i <= range_end; i++) {
		range.push_back(i);
	}
	ret.clear();
	if (num == 1) {
		ret.push_back(range[0]);
	} else if (num > 1) {
		while (ret.size() < num && range.size() > 0) {
			int ran = rand() % range.size();
			if (spec_set.find(ran) != spec_set.end()) {
				continue; // avoid the spec_set
			}
			ret.push_back(range[ran]);
			range.erase(range.begin() + ran);
		}
		sort(ret.begin(), ret.end());
	}
}

string create_symb(string last_name, string db_full_folder, string db_tar_folder) {
	string full_file = db_full_folder + last_name;
	string full_file_rst = full_file + ".rst.0";
	string tar_file = db_tar_folder + last_name;
	string tar_file_rst = tar_file + ".rst.0";

	if (symlink(full_file.c_str(), tar_file.c_str()) == 0) {
		clog << "Symbolic link " << tar_file << " created" << endl;
	} else {
		cerr << "Error creating symbolic link " << tar_file << " for " << full_file << endl;
	}

	if (symlink(full_file_rst.c_str(), tar_file_rst.c_str()) == 0) {
		clog << "Symbolic link " << tar_file_rst << " created" << endl;
	} else {
		cerr << "Error creating symbolic link " << tar_file_rst << " for " << full_file_rst << endl;
	}

	return tar_file;
}

int main(int argc, char* argv[]) {

	srand(time(0));

	if (argc < 8) {
		cerr << "Usage: " << argv[0] << " db_origin_path db_full_path db_tar_path num_obj num_rep num_spec" << endl;
		exit(1);
	}

	string db_origin_path(argv[1]);
	string db_full_path(argv[2]);
	string spec_path(argv[3]);
	string db_tar_path(argv[4]);
	int num_obj = atoi(argv[5]);
	int num_rep = atoi(argv[6]);
	int num_spec = atoi(argv[7]);

	string db_full_folder = get_foldername(db_full_path);
	string db_tar_folder = get_foldername(db_tar_path);
	string spec_folder = get_foldername(spec_path);

	vector<string> origin_names;
	int origin_num = read_db_mesh_names(db_origin_path, origin_names);
	
	ofstream ofs_meta(db_tar_folder + "meta.txt");
	int num_total = num_obj * num_rep;
	ofs_meta << num_total << endl;

	const unordered_set<int> spec_set = { 176, 231, 48, 228, 373 };
	const unordered_set<int> empty_set = {};

	int global_id = 0;

	vector<int> rand_sel_objs;
	sel_rand_numbers_from_range(0, origin_num - 1, num_obj - spec_set.size(), spec_set, rand_sel_objs);

	for (auto &i: rand_sel_objs) {
		vector<int> rand_sel_reps;
		sel_rand_numbers_from_range(0, 9999, num_rep, empty_set, rand_sel_reps);

		for (auto &j: rand_sel_reps) {
			string j_str = to_string(j);
			string last_name = origin_names[i] + string(fix_len - j_str.length(), '0') + j_str + ".ply";

			string tar_file = create_symb(last_name, db_full_folder, db_tar_folder);

			ofs_meta << (global_id++) << " " << tar_file << endl;
		}
	}

	for (auto &i: spec_set) {
		for (int j = 0; j < num_spec; j++) {
			string j_str = to_string(j);
			string last_name = origin_names[i] + "s" + string(2 - j_str.length(), '0') + j_str + ".ply";

			string tar_file = create_symb(last_name, spec_folder, db_tar_folder);

			ofs_meta << (global_id++) << " " << tar_file << endl;
		}

		vector<int> rand_sel_reps;
		sel_rand_numbers_from_range(0, 9999, num_rep - num_spec, empty_set, rand_sel_reps);

		for (auto &j: rand_sel_reps) {
			string j_str = to_string(j);
			string last_name = origin_names[i] + string(fix_len - j_str.length(), '0') + j_str + ".ply";

			string tar_file = create_symb(last_name, db_full_folder, db_tar_folder);

			ofs_meta << (global_id++) << " " << tar_file << endl;
		}
	}

	ofs_meta.close();

}