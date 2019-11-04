#include "TriMesh.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;
using namespace trimesh;

inline double rand_double() {
	return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
};

inline double rand_double(double d) {
	return static_cast <double> (rand()) / static_cast <double> (RAND_MAX / d);
};

int main(int argc, char **argv) {
    srand(time(NULL));

	// if (argc < 5) {
	// 	cerr << "Usage: " << argv[0]
	// 		 << "max_drift inlier_rate "
	// 		 << "input_filename output_filename" << endl;
	// 	exit(1);
	// }
	// char *input_filename = argv[3];
	// char *output_filename = argv[4];
	// use hardcoded file path instead
	string input_filename = "../data/t2_range.ply";
	string output_filename = "../data/t2_noisy.ply";

	TriMesh *input_mesh = TriMesh::read(input_filename);
	TriMesh *output_mesh = new TriMesh;

	double max_drift = 0.3;
	double inlier_rate = 1.0;
	if (argc == 3) {
		max_drift = atof(argv[1]);
		inlier_rate = atof(argv[2]);
	}

	for (int i = 0; i < input_mesh->vertices.size(); i++) {
		auto v = input_mesh->vertices[i];

		double a = rand_double() - 0.5, b = rand_double() - 0.5, c = rand_double() - 0.5;
		double norm = sqrt(a * a + b * b + c * c);
		double ran_scale = rand_double(max_drift) / norm;
		a *= ran_scale; b *= ran_scale; c *= ran_scale;

		v[0] += a; v[1] += b; v[2] += c;
		output_mesh->vertices.push_back(v);
	}
	
	output_mesh->need_faces();

	output_mesh->write(output_filename);
}