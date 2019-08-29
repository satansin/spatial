#include "TriMesh.h"
#include "TriMesh_algo.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ios>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <vector>
using namespace std;
using namespace trimesh;

struct Translation {
	int x, y, z;
};

Translation trans_table[] = {
	{ 0, 0, 0 },
	{ 1, 0, 0 }, { -1, 0, 0 },
	{ 0, 1, 0 }, { 0, -1, 0 },
	{ 0, 0, 1 }, { 0, 0, -1 },
	{ 1, 1, 0 }, { -1, 1, 0 }, { 1, -1, 0 }, { -1, -1, 0 },
	{ 0, 1, 1 }, { 0, -1, 1 }, { 0, 1, -1 }, { 0, -1, -1 },
	{ 1, 0, 1 }, { -1, 0, 1 }, { 1, 0, -1 }, { -1, 0, -1 },
	{ 1, 1, 1 }, { -1, 1, 1 }, { 1, -1, 1 }, { 1, 1, -1 },
	{ -1, -1, 1 }, { -1, 1, -1 }, { 1, -1, -1 }, { -1, -1, -1 },
	{ 2, 0, 0 }, { -2, 0, 0 },
	{ 0, 2, 0 }, { 0, -2, 0 },
	{ 0, 0, 2 }, { 0, 0, -2 },
	{ 2, 1, 0 }, { -1, 1, 0 }, { 1, -1, 0 }, { -1, -1, 0 }
};

const int TRAINING_SIZE = 7481;
const int TESTING_SIZE = 7518;

string get_indexed_filename(string leading, int index, string suffix) {
	ostringstream out;
	out << leading << internal << setfill('0') << setw(6) << index << suffix;
	return out.str();
}

int main(int argc, char** argv) {
	srand(time(NULL));

	string input_folder =
		"/rwproject/kdd-db/hliubs/spatial/3dor/KITTI/data_object_velodyne/training/ply/";

	int concat_num = atoi(argv[1]);
	if (concat_num > (sizeof(trans_table) / sizeof(trans_table[0]))) {
		cerr << "Too large number of concat!" << endl;
		exit(1);
	}
	int ran_list[concat_num];
	string filename_list[concat_num];
	vector<TriMesh*> mesh_list;
	double max_bsphere_rad = -1;

	cout << endl;
	cout << "Selecting " << concat_num << " KITTI point clouds randomly" << endl;
	for (int i = 0; i < concat_num; i++) {
		ran_list[i] = rand() % TRAINING_SIZE;
		filename_list[i] = get_indexed_filename(input_folder, ran_list[i], ".ply");
		TriMesh* m = TriMesh::read(filename_list[i]);
		mesh_list.push_back(m);
		m->need_bsphere();
		if (m->bsphere.r - max_bsphere_rad > 0) {
			max_bsphere_rad = m->bsphere.r;
		}

		cout << "#" << i << ": " << filename_list[i] << endl;
	}

	vec trans_list[concat_num];
	for (int i = 0; i < concat_num; i++) {
		vec t(trans_table[i].x * 4 * max_bsphere_rad,
			  trans_table[i].y * 4 * max_bsphere_rad,
			  trans_table[i].z * 4 * max_bsphere_rad);
		trans_list[i] = t;
		trans(mesh_list[i], t);
	}

	string output_filename = argv[2];
	join(mesh_list)->write(output_filename);

	cout << endl << "Joined point cloud written to " << output_filename << endl;

	string outinfo_filename = output_filename + ".info";
	ofstream outinfo_ofs(outinfo_filename);

	outinfo_ofs << concat_num << endl;
	for (int i = 0; i < concat_num; i++) {
		outinfo_ofs << filename_list[i] << endl;
		outinfo_ofs << trans_list[i][0] << endl;
		outinfo_ofs << trans_list[i][1] << endl;
		outinfo_ofs << trans_list[i][2] << endl;
	}
	outinfo_ofs.close();

	cout << "Join information written to " << outinfo_filename << endl;

}