#include "tetra_meas.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ios>
#include <iomanip>
#include <sstream>
#include <limits>
using namespace std;

#define MAX_ARRAY_SIZE 50000

struct Tetra_stat {
	double r_b;
	double r_c;
	double r_i;
	double ratio;
};

vector<Tetra_stat> radiis;
double r_b_max = 0;
double r_b_min = numeric_limits<double>::max();
double r_c_max = 0;
double r_c_min = numeric_limits<double>::max();
double r_i_max = 0;
double r_i_min = numeric_limits<double>::max();

void udpate_max_min(double r_b, double r_c, double r_i) {
	if (r_b - r_b_max > 0) r_b_max = r_b;
	if (r_b - r_b_min < 0) r_b_min = r_b;
	if (r_c - r_c_max > 0) r_c_max = r_c;
	if (r_c - r_c_min < 0) r_c_min = r_c;
	if (r_i - r_i_max > 0) r_i_max = r_i;
	if (r_i - r_i_min < 0) r_i_min = r_i;
}

string get_indexed_filename(string leading, int index) {
	ostringstream out;
	out << leading << internal << setfill('0') << setw(3) << index;
	return out.str();
}

int dump_array(string output_name, int index=-1) {
	string file_name;
	if (index < 0) {
		file_name = output_name;
	} else {
		file_name = get_indexed_filename(output_name, index);
	}

	ofstream ofs(file_name);
	if (!ofs) {
		cerr << "cannot open " << file_name << endl;
		exit(1);
	}

	int size = radiis.size();
	for (auto &i: radiis) {
		ofs << i.r_b << "\t" << i.r_c << "\t" << i.r_i << "\t" << i.ratio << endl;
	}
	radiis.clear();

	ofs.close();

	return size;
}

long global_counter = 0;
void next_glb_idx(long total, string output_name) {
	long base = total / 100;
	if (global_counter % base == 0 && global_counter > 0) {
		int piece_index = global_counter / base;
		if (total > MAX_ARRAY_SIZE) {
			int piece_size = dump_array(output_name, piece_index);
			cout << piece_size << " items written" << " \n";
		}
		cout << piece_index << "%...\n";
	}
	global_counter++;
}

void combination_tetra(long res_size, Pt3D list[], Pt3D* tetra[], int start, int end, int index, string output_name) {
	if (index == 4) {
		// double r = circumradi_3d(*tetra[0], *tetra[1], *tetra[2], *tetra[3]);
		auto s = get_ratio_set_3d(*tetra[0], *tetra[1], *tetra[2], *tetra[3]);
		double r_b = bounding_radi_3d(*tetra[0], *tetra[1], *tetra[2], *tetra[3]);
		next_glb_idx(res_size, output_name);
		radiis.push_back({r_b, s.circumradius, s.inradius, s.ratio});
		udpate_max_min(r_b, s.circumradius, s.inradius);

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
		combination_tetra(res_size, list, tetra, i + 1, end, index + 1, output_name);
	}
}

int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "missing output file" << endl;
		exit(1);
	}

	ifstream input_ply;
	input_ply.open("../data/t1.ply");
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

	long perm_size = size;
	perm_size *= (size - 1) * (size - 2) * (size - 3) / 24;
	cout << "permutation size is " << perm_size << endl;

	Pt3D* tetra[4];
	combination_tetra(perm_size, point_list, tetra, 0, size - 1, 0, output_file);

	if (!radiis.empty()) {
		int remaining_size = dump_array(output_file);
		cout << "remaining " << remaining_size << " items written" << endl;
	}

	ofstream max_min_output(output_file + "_maxmin");
	if (!max_min_output) {
		cerr << "cannot open " << (output_file + "_maxmin") << endl;
		exit(1);
	}
	max_min_output << "max bounding radius: " << r_b_max << endl;
	max_min_output << "min bounding radius: " << r_b_min << endl;
	max_min_output << "max circumradius: " << r_c_max << endl;
	max_min_output << "min circumradius: " << r_c_min << endl;
	max_min_output << "max inradius: " << r_i_max << endl;
	max_min_output << "max inradius: " << r_i_min << endl;
	max_min_output.close();

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
