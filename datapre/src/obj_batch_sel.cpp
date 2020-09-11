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
#include "TriMesh.h"

using namespace std;
using namespace trimesh;


void sel_rand_numbers_from_range(int range_start, int range_end, int num, const unordered_set<int>& spec_set, vector<int>& ret) {
	vector<int> range;
	for (int i = range_start; i <= range_end; i++) {
		if (spec_set.find(i) == spec_set.end()) {
			range.push_back(i);
		}
	}
	ret.clear();
	if (num == 1) {
		ret.push_back(range[0]);
	} else if (num > 1) {
		while (ret.size() < num && range.size() > 0) {
			int ran = rand() % range.size();
			ret.push_back(range[ran]);
			range.erase(range.begin() + ran);
		}
		sort(ret.begin(), ret.end());
	}
}

void clear_mem(vector<TriMesh*>& db_meshes) {
	for (int i = 0; i < db_meshes.size(); i++) {
		if (db_meshes[i] != nullptr) {
			db_meshes[i]->clear();
			delete db_meshes[i];
			db_meshes[i] = nullptr;
		}
	}
}

void clear_mem(vector<TriMesh*>& db_meshes, int id, int size) {
	for (int i = id * size; i < (id + 1) * size; i++) {
		if (db_meshes[i] != nullptr) {
			db_meshes[i]->clear();
			delete db_meshes[i];
			db_meshes[i] = nullptr;
		}
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

void write_in_whole(string db_tar_folder, TriMesh* combined_mesh, vector<int>& mesh_sizes, vector<string>& mesh_names) {
	combined_mesh->write(db_tar_folder + "combined.ply.0");
	ofstream ofs_combined_meta(db_tar_folder + "combined.meta.0");
	for (auto &v: mesh_sizes) {
		ofs_combined_meta << v << endl;
	}
	ofs_combined_meta.close();
	ofstream ofs_combined_meta_ovrl(db_tar_folder + "combined.meta");
	ofs_combined_meta_ovrl << 1 << endl << mesh_names.size() << endl;
	ofs_combined_meta_ovrl.close();
}

void write_in_parts(string db_tar_folder, TriMesh* combined_mesh, vector<int>& mesh_sizes, vector<string>& mesh_names, int num_parts) {
	const int part_size = mesh_names.size() / num_parts;
	// cout << "Split all " << mesh_names.size() << " objects into " << num_parts << " parts, each of size " << part_size << endl;

	int from = 0;
	for (int i = 0; i < num_parts; i++) {
		// cout << "For part #" << i << ":" << endl;
		TriMesh* combined_part = new TriMesh;
		// cout << "Copy the combined_mesh to combined_part from " << (i * part_size) << " to " << ((i + 1) * part_size - 1) << endl;
		for (int j = i * part_size; j < (i + 1) * part_size; j++) {
			// cout << from << " " << mesh_sizes[j] << endl;
			append_mesh(combined_mesh, combined_part, from, mesh_sizes[j]);
			from += mesh_sizes[j];
		}
		// cout << "\"from\" is finally set to " << from << endl;
		// cout << "Size of the combined_part: " << combined_part->vertices.size() << endl;
		combined_part->write(db_tar_folder + "combined.ply." + to_string(i));
		combined_part->clear();
		delete combined_part;
		ofstream ofs_combined_meta(db_tar_folder + "combined.meta." + to_string(i));
		for (int j = i * part_size; j < (i + 1) * part_size; j++) {
			ofs_combined_meta << mesh_sizes[j] << endl;
		}
		ofs_combined_meta.close();
	}
	ofstream ofs_combined_meta_ovrl(db_tar_folder + "combined.meta");
	ofs_combined_meta_ovrl << num_parts << endl;
	for (int i = 0; i < num_parts; i++) {
		ofs_combined_meta_ovrl << part_size << endl;
	}
	ofs_combined_meta_ovrl.close();
}

void exec(int origin_num, vector<string>& origin_names, vector<TriMesh*>& full_meshes, int num_obj, int num_rep, int num_spec, string spec_folder, string db_tar_folder, int num_parts,
	const unordered_set<int>& spec_set, const unordered_set<int>& empty_set) {

	vector<int> rand_sel_objs;
	sel_rand_numbers_from_range(0, origin_num - 1, num_obj - spec_set.size(), spec_set, rand_sel_objs);

	// for (auto &v: rand_sel_objs) {
	// 	cout << " " << v;
	// }
	// cout << endl;

	// int global_id = 0;

	TriMesh* combined_mesh = new TriMesh;
	vector<string> mesh_names;
	vector<int> mesh_sizes;

	for (auto &i: rand_sel_objs) {
		vector<int> rand_sel_reps;
		sel_rand_numbers_from_range(0, 9999, num_rep, empty_set, rand_sel_reps);

		for (auto &j: rand_sel_reps) {
			string j_str = to_string(j);

			// string last_name = origin_names[i] + string(fix_len - j_str.length(), '0') + j_str + ".ply";
			// string tar_file = create_symb(last_name, db_full_folder, db_tar_folder);
			// ofs_meta << (global_id++) << " " << tar_file << endl;

			mesh_names.push_back(origin_names[i] + string(fix_len - j_str.length(), '0') + j_str + ".ply");

			append_mesh(full_meshes[i * 10000 + j], combined_mesh);
			mesh_sizes.push_back(full_meshes[i * 10000 + j]->vertices.size());
			// cout << "Combined mesh appended: " << combined_mesh->vertices.size() << endl;
		}

		vector<int>().swap(rand_sel_reps);
		// clear_mem(full_meshes, i, 10000);
	}

	for (auto &i: spec_set) {
		for (int j = 0; j < num_spec; j++) {
			string j_str = to_string(j);

			// string last_name = origin_names[i] + "s" + string(2 - j_str.length(), '0') + j_str + ".ply";
			// string tar_file = create_symb(last_name, spec_folder, db_tar_folder);
			// ofs_meta << (global_id++) << " " << tar_file << endl;

			mesh_names.push_back(origin_names[i] + "s" + string(2 - j_str.length(), '0') + j_str + ".ply");

			TriMesh* spec_mesh = TriMesh::read(spec_folder + origin_names[i] + "s" + string(2 - j_str.length(), '0') + j_str + ".ply");
			append_mesh(spec_mesh, combined_mesh);
			mesh_sizes.push_back(spec_mesh->vertices.size());
			// cout << "Combined mesh appended: " << combined_mesh->vertices.size() << endl;
			spec_mesh->clear();
			delete spec_mesh;
		}

		vector<int> rand_sel_reps;
		sel_rand_numbers_from_range(0, 9999, num_rep - num_spec, empty_set, rand_sel_reps);

		for (auto &j: rand_sel_reps) {
			string j_str = to_string(j);

			// string last_name = origin_names[i] + string(fix_len - j_str.length(), '0') + j_str + ".ply";
			// string tar_file = create_symb(last_name, db_full_folder, db_tar_folder);
			// ofs_meta << (global_id++) << " " << tar_file << endl;

			mesh_names.push_back(origin_names[i] + string(fix_len - j_str.length(), '0') + j_str + ".ply");

			append_mesh(full_meshes[i * 10000 + j], combined_mesh);
			mesh_sizes.push_back(full_meshes[i * 10000 + j]->vertices.size());
			// cout << "Combined mesh appended: " << combined_mesh->vertices.size() << endl;
		}

		vector<int>().swap(rand_sel_reps);
		// clear_mem(full_meshes, i, 10000);
	}

	ofstream ofs_meta(db_tar_folder + "meta.txt");
	ofs_meta << mesh_names.size() << endl;
	for (int i = 0; i < mesh_names.size(); i++) {
		ofs_meta << i << " " << mesh_names[i] << endl;
	}
	ofs_meta.close();

	cout << "# points of the combined mesh: " << combined_mesh->vertices.size() << endl;

	if (num_parts == 1) {
		write_in_whole(db_tar_folder, combined_mesh, mesh_sizes, mesh_names);
	} else {
		write_in_parts(db_tar_folder, combined_mesh, mesh_sizes, mesh_names, num_parts);
	}

	combined_mesh->clear();
	delete combined_mesh;

}

int main(int argc, char* argv[]) {

	srand(time(0));

	if (argc < 5) {
		cerr << "Usage: " << argv[0] << " db_origin_path db_full_path db_spec_path db_tar_path /*num_obj num_rep num_spec [-num_parts=...]*/" << endl;
		exit(1);
	}

	// int num_parts = 1;
	// for (int i = 0; i < argc; i++) {
	// 	string argv_str(argv[i]);
	// 	if (argv_str.rfind("-num_parts", 0) == 0)
	// 		num_parts = stoi(argv[i] + 11);
	// }

	const string db_origin_path(argv[1]);
	const string db_full_path(argv[2]);
	const string spec_path(argv[3]);
	const string db_tar_path(argv[4]);
	// const int num_obj = atoi(argv[5]);
	// const int num_rep = atoi(argv[6]);
	// const int num_spec = atoi(argv[7]);

	const string db_full_folder = get_foldername(db_full_path);
	const string db_tar_folder = get_foldername(db_tar_path);
	const string spec_folder = get_foldername(spec_path);

	vector<string> origin_names;
	const int origin_num = read_db_mesh_names(db_origin_path, origin_names);

	vector<TriMesh*> full_meshes;
	read_db_meshes_combined(db_full_path, full_meshes);

	// const unordered_set<int> spec_set = { 176, 231, 48, 228, 373 }; // v1
	const unordered_set<int> spec_set = { 300, 340, 432, 13, 213 }; // v2
	const unordered_set<int> empty_set = {};

	// exec(origin_num, origin_names, full_meshes, 100, 1, 1, spec_folder, db_tar_folder + "1_sel_100i/", 1, spec_set, empty_set);
	// exec(origin_num, origin_names, full_meshes, 250, 4, 2, spec_folder, db_tar_folder + "2_sel_001k/", 1, spec_set, empty_set);
	// exec(origin_num, origin_names, full_meshes, 400, 25, 5, spec_folder, db_tar_folder + "3_sel_010k/", 1, spec_set, empty_set);
	// exec(origin_num, origin_names, full_meshes, 400, 250, 10, spec_folder, db_tar_folder + "4_sel_100k/", 1, spec_set, empty_set);
	exec(origin_num, origin_names, full_meshes, 400, 2500, 50, spec_folder, db_tar_folder + "5_sel_001m/", 10, spec_set, empty_set);

	clear_mem(full_meshes);

}