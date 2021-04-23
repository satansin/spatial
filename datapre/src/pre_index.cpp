#include <iostream>
#include <vector>
#include <cstdlib>
#include <unordered_map>
#include "TriMesh.h"
extern "C" {
    #include "rtree.h"
}

#include "util.h"

using namespace std;
using namespace trimesh;

rtree_info read_rstree_info(string filename) {
	ifstream ifs(filename);
	rtree_info ret;
	ifs >> ret.m >> ret.M >> ret.dim >> ret.reinsert_p;
	ifs.close();
	return ret;
}

void build_rtree(TriMesh* mesh, vector<rtree_info>& info, int n, string realname) {
	R_TYPE** data = (R_TYPE **) malloc(sizeof(R_TYPE *) * n);
    for (int k = 0; k < n; k++) {
        data[k] = (R_TYPE *) malloc(sizeof(R_TYPE) * 3);
        for (int l = 0; l < 3; l++) {
            data[k][l] = (int) (mesh->vertices[k][l] * 1e5);
        }
    }
	node_type* root;
	for (int k = 0; k < info.size(); k++) {
		// build R-tree for mesh points
	    // cout << "Start building R-tree for mesh #" << id << " of info #" << k << "..." << endl;
	    build_tree(&root, data, n, &info[k]);

	    // save R-tree
	    string realname_idx(realname + "." + to_string(k));
	    save_rtree(root, realname_idx.c_str(), &info[k]);
	    cout << "Save R-tree to " << realname_idx << endl;

	    free_tree(root, &info[k]);
	    // cout << "R-tree freed" << endl;
	}
	for (int k = 0; k < n; k++) {
		free(data[k]);
	}
	free(data);
}

int main(int argc, char* argv[]) {

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " input_filename batch_id [-batch] [-nb=...]" << endl;
		exit(1);
	}

	bool batch_mode = false;
	int num_batches = 1;
    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-batch") {
            batch_mode = true;
        } else if (string(argv[i]).rfind("-nb", 0) == 0) {
        	num_batches = stoi(argv[i] + 4);
        }
    }

    int batch_id = 0;
	string input_filename = argv[1];
	if (batch_mode) {
		batch_id = stoi(argv[2]);
	}

	int num_meshes = 1;
    vector<string> db_filenames;

    int batch_size = 1;

    if (batch_mode) {
    	num_meshes = read_db_mesh_filenames(input_filename, db_filenames);
	    batch_size = num_meshes / num_batches;
    } else {
        db_filenames.push_back(input_filename);
    }

	vector<rtree_info> info;
	info.push_back(read_rstree_info("/project/kdd/hliubs/proj_sync/git/spatial/common/config/rstree.pcd.config"));

    vector<TriMesh*> db_meshes;
	cout << "Process batch #" << batch_id << endl;
	read_db_mesh_batch(db_filenames, batch_id, batch_size, db_meshes);

	for (int i = 0; i < batch_size; i++) {
		TriMesh* mesh = db_meshes[i];
		int n = mesh->vertices.size();
	    string realname = db_filenames[batch_id * batch_size + i] + ".rst";
		build_rtree(mesh, info, n, realname);

		delete mesh;
	}

}