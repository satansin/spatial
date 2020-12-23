#include "TriMesh.h"
#include "TriMesh_algo.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace trimesh;

int main(int argc, char** argv) {

	if (argc < 4) {
		cerr << "Usage: " << argv[0] << " db_filename query_filename trans_filename" << endl;
		exit(1);
	}

	string db_filename = string(argv[1]);
	string query_filename = string(argv[2]);
	string trans_filename = string(argv[3]);

	TriMesh* db_mesh = TriMesh::read(db_filename);
	TriMesh* query_mesh = TriMesh::read(query_filename);

	ifstream trans_ifs(trans_filename);
	double a, b, c, d, x, y, z;
	trans_ifs >> a >> b >> c >> d >> x >> y >> z;
	trans_ifs.close();

	trans(query_mesh, vec(-x, -y, -z));
	rot(query_mesh, a, vec(-b, -c, -d));

	db_mesh->need_bbox();

	string out_filename = query_filename + ".color";
	ofstream ofs(out_filename);
	ofs << setprecision(10);

	for (int i = 0; i < query_mesh->vertices.size(); i++) {
		ofs << i;
		for (int j = 0; j < 3; j++) {
			double c = (query_mesh->vertices[i][j] - db_mesh->bbox.min[j]) / (db_mesh->bbox.max[j] - db_mesh->bbox.min[j]);
			ofs << " " << c;
		}
		ofs << endl;
	}
	ofs.close();
}