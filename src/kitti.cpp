/**
 * reference
 * http://zengzeyu.com/2018/03/15/18_3_15/kitti_bin_to_pcd/
 * https://github.com/yanii/kitti-pcl/blob/master/KITTI_README.TXT
 */

#include "TriMesh.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <ios>
#include <iomanip>
#include <sstream>
#include <vector>
using namespace std;
using namespace trimesh;

const int TRAINING_SIZE = 7481;
const int TESTING_SIZE = 8518;

string get_indexed_filename(string leading, int index, string suffix) {
	ostringstream out;
	out << leading << internal << setfill('0') << setw(6) << index << suffix;
	return out.str();
}

int main(int argc, char **argv) {
	// if (argc < 3) {
	// 	cerr << "Usage: " << argv[0] << " input_filename output_filename" << endl;
	// 	exit(1);
	// }
	// char *input_filename = argv[1];
	// char *output_filename = argv[2];
	// use hardcoded file path instead
	string input_filename = "/rwproject/kdd-db/hliubs/KITTI/data_object_velodyne/training/velodyne/";
	string output_filename = "/rwproject/kdd-db/hliubs/KITTI/data_object_velodyne/training/ply/";

	// process training data
	for (int i = 0; i < TRAINING_SIZE; i++) {	
		// allocate 4 MB buffer (only ~130*4*4 KB are needed)
		int32_t num = 1000000;
		float *data = (float *) malloc(num * sizeof(float));

		// pointers
		float *px = data + 0;
		float *py = data + 1;
		float *pz = data + 2;
		float *pr = data + 3;

		// create TriMesh object
		TriMesh *m = new TriMesh;

		// load point cloud
		string input_realname = get_indexed_filename(input_filename, i, ".bin");
		cout << "reading " << input_realname << endl;
		FILE *stream = fopen(input_realname.c_str(), "rb");
		num = fread(data, sizeof(float), num, stream) / 4;
		for (int32_t i = 0; i < num; i++) {
			// cout << *px << " " << *py << " " << *pz << " " << *pr << endl;
			m->vertices.push_back(point(*px, *py, *pz));
			px += 4; py += 4; pz += 4; pr += 4;
		}

		// triangulate the mesh
		// m->need_faces();

		// write mesh to output ply
		string output_realname = get_indexed_filename(output_filename, i, ".ply");
		cout << "writing to " << output_realname << endl;
		m->write(output_realname);

		fclose(stream);
		delete m;
		free(data);
	}
}