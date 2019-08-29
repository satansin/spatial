#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char **argv) {
	if (argc < 3) {
		cerr << "Usage: " << argv[0] << " input_filename output_filename" << endl;
		exit(1);
	}
	auto input_filename = argv[1];
	auto output_filename = argv[2];
	ofstream output_ply(output_filename);
	if (!output_ply.is_open()) {
		cerr << "error open output file" << endl;
		exit(1);
	}

	ifstream input_ply;
	input_ply.open(input_filename);
	if (!input_ply.is_open()) {
		cerr << "error open input file" << endl;
		exit(1);
	}

	cout << "reading " << input_filename << endl;
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
	cout << "written to " << output_filename << endl;

	input_ply.close();
	output_ply.close();
}