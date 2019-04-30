#include "point.h"
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "missing output file" << endl;
		exit(1);
	}
	ofstream output_ply(argv[1]);
	if (!output_ply.is_open()) {
		cerr << "error open output file" << endl;
		exit(1);
	}

	ifstream input_ply;
	input_ply.open("../data/bun_zipper_res4.ply");
	if (!input_ply.is_open()) {
		cerr << "error open input file" << endl;
		exit(1);
	}

	cout << "reading bun_zipper_res4.ply..." << endl;
	string line;
	int i;
	for (i = 0; i < 4; i++) {
		getline(input_ply, line);
		output_ply << line << endl;
	}
	int size = stoi(line.substr(line.find_last_of(' ')));

	for (i = 0; i < 8; i++) {
		getline(input_ply, line);
		output_ply << line << endl;
	}

	float num;
	for (i = 0; i < size; i++) {
		input_ply >> num;
		num *= 1000;
		output_ply << num << " ";
		input_ply >> num;
		num *= 1000;
		output_ply << num << " ";
		input_ply >> num;
		num *= 1000;
		output_ply << num << " ";
		input_ply >> num;
		output_ply << num << " ";
		input_ply >> num;
		output_ply << num << " ";
		output_ply << endl;
	}

	while (getline(input_ply, line)) {
		if (line.find_first_not_of(' ') != string::npos) {
			// lines with only spaces are skipped
			output_ply << line << endl;
		}
	}
	cout << "written to " << argv[1] << endl;

	input_ply.close();
	output_ply.close();
}