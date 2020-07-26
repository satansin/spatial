#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unistd.h>

#include "util.h"

using namespace std;

int read_db_mesh_names(const string path, vector<string>& db_mesh_names) {
	auto db_folder = get_foldername(path);
	string meta_file = db_folder + "meta.txt";
	ifstream ifs(meta_file);

	if (!ifs) {
		cerr << "Fail reading meta info" << endl;
		exit(1);
	} else {
		cout << "Reading " << meta_file << endl;
	}

	int num;
    ifs >> num;

	int id;
	string s_file;

    for (int i = 0; i < num; i++) {
        ifs >> id >> s_file;
    	string fn = get_filename(s_file);
    	// cout << fn.substr(0, fn.length() - 4) << endl;
    	db_mesh_names.push_back(fn.substr(0, fn.length() - 4));
    }

    ifs.close();

    return num;
}

void sel_rand_numbers_from_range(int range_start, int range_end, int num, vector<int>& ret) {
	vector<int> range;
	for (int i = range_start; i <= range_end; i++) {
		range.push_back(i);
	}
	ret.clear();
	if (num <= 1) {
		ret.push_back(range[0]);
	} else {
		while (ret.size() < num && range.size() > 0) {
			int ran = rand() % range.size();
			ret.push_back(range[ran]);
			range.erase(range.begin() + ran);
		}
		sort(ret.begin(), ret.end());
	}
}

int main(int argc, char* argv[]) {

	srand(time(0));

	if (argc < 5) {
		cerr << "Usage: " << argv[0] << " db_origin_path db_full_path db_tar_path num_obj num_rep" << endl;
		exit(1);
	}

	string db_origin_path(argv[1]);
	string db_full_path(argv[2]);
	string db_tar_path(argv[3]);
	int num_obj = atoi(argv[4]);
	int num_rep = atoi(argv[5]);

	string db_full_folder = get_foldername(db_full_path);
	string db_tar_folder = get_foldername(db_tar_path);

	vector<string> origin_names;
	int origin_num = read_db_mesh_names(db_origin_path, origin_names);
	
	ofstream ofs_meta(db_tar_folder + "meta.txt");
	int num_total = num_obj * num_rep;
	ofs_meta << num_total << endl;

	int global_id = 0;

	vector<int> rand_sel_objs;
	sel_rand_numbers_from_range(0, origin_num - 1, num_obj, rand_sel_objs);

	for (int i = 0; i < rand_sel_objs.size(); i++) {
		vector<int> rand_sel_reps;
		sel_rand_numbers_from_range(0, 9999, num_rep, rand_sel_reps);

		for (int j = 0; j < rand_sel_reps.size(); j++) {
			string j_str = to_string(rand_sel_reps[j]);
			string last_name = origin_names[rand_sel_objs[i]] + string(fix_len - j_str.length(), '0') + j_str + ".ply";
			string full_file = db_full_folder + last_name;
			string full_file_rst = full_file + ".rst.0";
			string tar_file = db_tar_folder + last_name;
			string tar_file_rst = tar_file + ".rst.0";

			if (symlink(full_file.c_str(), tar_file.c_str()) == 0) {
				cout << "Symbolic link " << tar_file << " created" << endl;
			} else {
				cout << "Error creating symbolic link " << tar_file << endl;
			}

			if (symlink(full_file_rst.c_str(), tar_file_rst.c_str()) == 0) {
				cout << "Symbolic link " << tar_file_rst << " created" << endl;
			} else {
				cout << "Error creating symbolic link " << tar_file_rst << endl;
			}

			ofs_meta << (global_id++) << " " << tar_file << endl;
		}
	}

	ofs_meta.close();

}