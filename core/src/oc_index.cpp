#include <iostream>
#include "OcTree.h"
#include "TriMesh.h"
using namespace std;
using namespace trimesh;

int main() {
	string database_filename = "/rwproject/kdd-db/hliubs/10_data_spatial/3dor/db/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply";

	TriMesh *mesh_p = TriMesh::read(database_filename);

	OTree o_tree;
	float err = 0.2;
	cout << "Start building OcTree with err: " << err << endl;
	o_tree.buildFromMesh(mesh_p, err);
}