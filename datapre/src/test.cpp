#include <iostream>
#include "TriMesh.h"
using namespace std;
using namespace trimesh;

int main(int argc, char** argv) {

	TriMesh* input_mesh = TriMesh::read("/csproject/kdd/hliubs/data/spatial/3dor/db/obj_scans/00004.ply");

	cout << fixed << setprecision(25);

	for (auto &v: input_mesh->vertices) {
		cout << v[0] << "\t" << v[1] << "\t" << v[2] << endl;
	}
}