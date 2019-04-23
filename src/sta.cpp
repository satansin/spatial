#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ios>
#include <sstream>
using namespace std;

vector<double> radiis;

string get_indexed_filename(string leading, int index) {
	ostringstream out;
	out << leading << internal << setfill('0') << setw(3) << index;
	return out.str();
}

void read_array(string output_name, int index=-1) {
	string file_name;
	if (index < 0) {
		file_name = output_name;
	} else {
		file_name = get_indexed_filename(output_name, index);
	}

	ifstream ifs(file_name);
	if (!ifs) {
		cerr << "cannot open " << file_name << endl;
		exit(1);
	}

	cout << "reading " << file_name << endl;

	radiis.clear();

	string line;
	while (getline(ifs, line)) {
		radiis.push_back(stod(line));
	}

	ifs.close();
}

int main(int argc, char** argv) {
	if (argc < 2) {
		cerr << "missing input file" << endl;
		exit(1);
	}

	const int SIZE_HISTO = 1000;
	long histogram[SIZE_HISTO + 1] = {0};

	read_array(argv[1]);
	for (auto &r: radiis) {
		if (r - 1.0 > 0) {
			histogram[SIZE_HISTO]++;
		} else {
			histogram[(int)(r * SIZE_HISTO)]++;
		}
	}

	for (int i = 1; i <= 100; i++) {
		read_array(argv[1], i);
		for (auto &r: radiis) {
			if (r - 1.0 > 0) {
				histogram[SIZE_HISTO]++;
			} else {
				histogram[(int)(r * SIZE_HISTO)]++;
			}
		}
	}

	for (int i = 0; i <= SIZE_HISTO; i++) {
		cout << histogram[i] << endl;
	}

}