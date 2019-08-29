#include "TriMesh.h"
using namespace std;
using namespace trimesh;

int main(int argc, char** argv) {
	string input_filename = argv[1];
	string output_filename = argv[2];
	double scale_val = atof(argv[3]);

	TriMesh* input_mesh = TriMesh::read(input_filename);
	TriMesh* output_mesh = new TriMesh;

	for (int i = 0; i < input_mesh->vertices.size(); i++) {
		auto v = input_mesh->vertices[i];
		v[0] *= scale_val;
		v[1] *= scale_val;
		v[2] *= scale_val;
		output_mesh->vertices.push_back(v);
	}

	output_mesh->write(output_filename);
}