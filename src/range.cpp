#include "TriMesh.h"
#include <iostream>
using namespace std;
using namespace trimesh;

int main(int argc, char **argv) {
	// if (argc < 9) {
	// 	cerr << "Usage: " << argv[0]
	// 		 << "range-x1 range-x2 range-y1 range-y2 range-z1 range-z2 "
	// 		 << "input_filename output_filename" << endl;
	// 	exit(1);
	// }
	// char *input_filename = argv[7];
	// char *output_filename = argv[8];
	// use hardcoded file path instead
	string input_filename = "../data/t2.ply";
	string output_filename = "../data/t2_range.ply";

	string outid_filename = "../data/t2_id";

	if (argc < 7) {
		cerr << "Usage: " << argv[0]
			 << " range-x1 range-x2 range-y1 range-y2 range-z1 range-z2" << endl;
		exit(1);
	}
	double r_x1 = atof(argv[1]);
	double r_x2 = atof(argv[2]);
	double r_y1 = atof(argv[3]);
	double r_y2 = atof(argv[4]);
	double r_z1 = atof(argv[5]);
	double r_z2 = atof(argv[6]);

	TriMesh *input_mesh = TriMesh::read(input_filename);
	TriMesh *output_mesh = new TriMesh;
	ofstream outid_ofs(outid_filename);

	for (int i = 0; i < input_mesh->vertices.size(); i++) {
		auto v = input_mesh->vertices[i];
		if (v[0] - r_x1 >= 0 && v[0] - r_x2 <= 0 &&
			v[1] - r_y1 >= 0 && v[1] - r_y2 <= 0 &&
			v[2] - r_z1 >= 0 && v[2] - r_z2 <= 0) {
			output_mesh->vertices.push_back(v);
			outid_ofs << i << endl;
		}
	}

	outid_ofs.close();
	
	output_mesh->need_faces();

	output_mesh->write(output_filename);

	cout << output_mesh->vertices.size() << " out of " << input_mesh->vertices.size()
		 << " left" << endl;

}