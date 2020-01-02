#include <iostream>
#include <cstdlib>
#include <ctime>
#include "TriMesh.h"
using namespace std;
using namespace trimesh;

int main(int argc, char** argv) {

	srand(time(0));

	TriMesh* input_mesh = TriMesh::read(argv[1]);

	cout << fixed << setprecision(25);

	for (auto &v: input_mesh->vertices) {
		int ran = rand() % 1000;
		if (ran == 0)
			cout << v[0] << "\t" << v[1] << "\t" << v[2] << endl;
	}

	cout << setprecision(12);

	input_mesh->need_bbox();
	auto bbox = input_mesh->bbox;
	for (int i = 0; i < 3; i++) {
		cout << "[" << bbox.min[i] << ", " << bbox.max[i] << "]" << endl;
	}
}