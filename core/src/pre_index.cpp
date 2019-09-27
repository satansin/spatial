#include <iostream>
#include <vector>
#include "TriMesh.h"
extern "C" {
    #include "rtree.h"
}
#include "share.h"
using namespace std;
using namespace trimesh;

int main(int argc, char* argv[]) {

	if (argc < 3) {
		cerr << "Usage: " << argv[0] << " input_filename output_filename" << endl;
		exit(1);
	}

	string input_filename = argv[1];
	string output_filename = argv[2];

	TriMesh *mesh = TriMesh::read(input_filename);
	int n = mesh->vertices.size();

	vector<rtree_info> info = {
		{ 5, 10, 3, 7 },
		{ 10, 20, 3, 15 },
		{ 15, 30, 3, 22 },
		{ 20, 40, 3, 30 },
		{ 25, 50, 3, 37 }
	};

	R_TYPE **data = (R_TYPE **) malloc(sizeof(R_TYPE *) * n);
    for(int i = 0; i < n; i++) {
        data[i] = (R_TYPE *) malloc(sizeof(R_TYPE) * 3);
        for (int j = 0; j < 3; j++) {
            data[i][j] = mesh->vertices[i][j];
        }
    }
	
	node_type *root;

	for (int i = 0; i < info.size(); i++) {
		// build R-tree for mesh points
	    cout << "Start building R-tree for info #" << (i + 1) << "..." << endl;
	    timer_start();
	    build_tree(&root, data, n, &info[i]);
	    cout << "Build R-tree in " << timer_end(SECOND) << "(s)" << endl;

	    string realname = output_filename + "." + to_string(i + 1);
	    // save R-tree
	    timer_start();
	    save_rtree(root, realname.c_str(), &info[i]);
	    cout << "Save R-tree in " << timer_end(SECOND) << "(s)" << endl;

	    free_tree(root, &info[i]);
	    cout << "R-tree freed" << endl;
	}
}