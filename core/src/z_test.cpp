#include "TriMesh.h"
#include <iostream>
#include <string>

using namespace std;
using namespace trimesh;

int main(int argc, char* argv[]) {
	string db_path_1(argv[1]);
	string db_path_2(argv[2]);
	string output_file_1(argv[3]);
	string output_file_2(argv[4]);

	TriMesh* mesh_1 = TriMesh::read(db_path_1 + "combined.ply.0");

	TriMesh* mesh_2 = new TriMesh;
	for (int i = 0; i < 10; i++) {
		TriMesh* mesh = TriMesh::read(db_path_2 + "combined.ply." + to_string(i));
		for (int j = 0; j < mesh->vertices.size(); j++) {
			mesh_2->vertices.push_back(mesh->vertices[j]);
		}
	}

	mesh_1->write(output_file_1);
	mesh_2->write(output_file_2);

	// TriMesh* mesh_3 = new TriMesh;
	// for (int i = 101350; i < 101350 + 80; i++) {
	// 	mesh_3->vertices.push_back(mesh_2->vertices[i]);
	// }

	// mesh_3->write(output_file);
}