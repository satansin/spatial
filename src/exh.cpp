#include "point.h"
#include "circumcenter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

#define MAX_ARRAY_SIZE 50000

vector<double> radiis;

int dump_array(ofstream &ofs) {
	int size = radiis.size();
	for (auto &i: radiis) {
		ofs << i << endl;
	}
	radiis.clear();
	return size;
}

long global_counter = 0;
void next_glb_idx(long total, ofstream &ofs) {
	long base = total / 100;
	if (global_counter % base == 0 && global_counter > 0) {
		if (total > MAX_ARRAY_SIZE) {
			int piece_size = dump_array(ofs);
			cout << piece_size << " items written\n";
		}
		cout << (global_counter / base) << "%...\n";
	}
	global_counter++;
}

void combination_tetra(long res_size, Pt3D list[], Pt3D* tetra[], int start, int end, int index, ofstream &ofs) {
	if (index == 4) {
		double r = circumradi_3d(*tetra[0], *tetra[1], *tetra[2], *tetra[3]);
		next_glb_idx(res_size, ofs);
		radiis.push_back(r);

		// print_pt3d(tetra[0]);
		// cout << endl;
		// print_pt3d(tetra[1]);
		// cout << endl;
		// print_pt3d(tetra[2]);
		// cout << endl;
		// print_pt3d(tetra[3]);
		// cout << radius << endl;
		return;
	}

	for (int i = start; i <= end && end - i >= 3 - index; i++) {
		tetra[index] = &list[i];
		combination_tetra(res_size, list, tetra, i + 1, end, index + 1, ofs);
	}
}

int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "missing output file" << endl;
		return 1;
	}

	ifstream input_ply;
	input_ply.open("bun_zipper_res4.ply");
	string line;
	int i;
	for (i = 0; i < 4; i++) {
		getline(input_ply, line);
	}
	int size = stoi(line.substr(line.find_last_of(' ')));
	string output_file;

	if (argc == 2) {
		output_file = argv[1];
	} else if (argc > 2) {
		output_file = argv[2];
		size = atoi(argv[1]);
		cout << "size adjusted to " << size << endl;
	}

	for (i = 0; i < 8; i++) {
		getline(input_ply, line);
	}

	Pt3D point_list[size];
	double dummy;
	for (i = 0; i < size; i++) {
		input_ply >> point_list[i].x;
		input_ply >> point_list[i].y;
		input_ply >> point_list[i].z;
		input_ply >> dummy;
		input_ply >> dummy;
	}
	input_ply.close();

	ofstream output_radi(output_file);
	if (!output_radi) {
		cerr << "cannot open output file " << output_file << endl;
		return 1;
	}

	long perm_size = size;
	perm_size *= (size - 1) * (size - 2) * (size - 3) / 24;
	cout << "permutation size is " << perm_size << endl;

	Pt3D* tetra[4];
	combination_tetra(perm_size, point_list, tetra, 0, size - 1, 0, output_radi);
	if (!radiis.empty()) {
		int remaining_size = dump_array(output_radi);
		cout << "remaining " << remaining_size << " items written" << endl;
	}

	output_radi.close();

	// double r;
	// Pt3D test = circumcenter_3d(point_list[5], point_list[7], point_list[8], point_list[9], r);
	// print_pt3d(&test);
	// cout << endl << r << endl << endl;
	// print_pt3d(&point_list[5]);
	// cout << endl;
	// print_pt3d(&point_list[7]);
	// cout << endl;
	// print_pt3d(&point_list[8]);
	// cout << endl;
	// print_pt3d(&point_list[9]);
	// cout << endl;
	// cout << circumradi_3d(point_list[5], point_list[7], point_list[8], point_list[9]) << endl;
}
