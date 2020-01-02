#include <iostream>
#include <vector>
#include <cstdlib>
#include <unordered_map>
#include "TriMesh.h"
extern "C" {
    #include "rtree.h"
}
using namespace std;
using namespace trimesh;

rtree_info read_rstree_info(string filename) {
	ifstream ifs(filename);
	rtree_info ret;
	ifs >> ret.m >> ret.M >> ret.dim >> ret.reinsert_p;
	ifs.close();
	return ret;
}

string get_foldername(string path) {
    string ret;
    if (path[path.length() - 1] != '/') {
        ret = path + "/";
    } else {
        ret = path;
    }
    return ret;
}

int read_db_mesh_batch(string db_path, vector<TriMesh*>& db_meshes, vector<string>& db_filenames) {
    string db_folder = get_foldername(db_path);
    string meta_filename = db_folder + "meta.txt";

    ifstream ifs(db_folder + "meta.txt");
    if (!ifs) {
    	cerr << "Fail reading meta file " << meta_filename << endl;
    	return 0;
    }

    int num;
    ifs >> num;

    int id;
    string s_file;
    for (int i = 0; i < num; i++) {
        ifs >> id >> s_file;
        db_meshes.push_back(TriMesh::read(s_file));
        db_filenames.push_back(s_file);
    }

    ifs.close();

    return num;
}

int main(int argc, char* argv[]) {

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " input_filename [-batch]" << endl;
		exit(1);
	}

	bool batch_mode = false;
    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-batch") {
            batch_mode = true;
        }
    }

	string input_filename = argv[1];

    vector<TriMesh*> db_meshes;
    vector<string> db_filenames;

    if (batch_mode) {
        cout << "Num of meshes for batch: " << read_db_mesh_batch(input_filename, db_meshes, db_filenames) << endl;
    } else {
        db_meshes.push_back(TriMesh::read(input_filename));
    }

	vector<rtree_info> info;
	info.push_back(read_rstree_info("../common/config/rstree.pcd.config"));

	int id, n;
	TriMesh* mesh;
	R_TYPE** data;
	node_type* root;
	string realname;
	for (id = 0; id < db_meshes.size(); id++) {
		mesh = db_meshes[id];
		n = mesh->vertices.size();

		data = (R_TYPE **) malloc(sizeof(R_TYPE *) * n);
	    for (int i = 0; i < n; i++) {
	        data[i] = (R_TYPE *) malloc(sizeof(R_TYPE) * 3);
	        for (int j = 0; j < 3; j++) {
	            data[i][j] = (int) (mesh->vertices[i][j] * 1e5);
	        }
	    }

	    if (batch_mode) {
	    	realname = db_filenames[id] + ".rst";
	    } else {
	    	realname = input_filename + ".rst";
	    }

		for (int i = 0; i < info.size(); i++) {
			// build R-tree for mesh points
		    cout << "Start building R-tree for mesh #" << id << " of info #" << i << "..." << endl;
		    // timer_start();
		    build_tree(&root, data, n, &info[i]);
		    // cout << "Build R-tree in " << timer_end(SECOND) << "(s)" << endl;

		    // save R-tree
		    // timer_start();
		    string realname_idx(realname + "." + to_string(i));
		    save_rtree(root, realname_idx.c_str(), &info[i]);
		    cout << "Save R-tree to " << realname_idx /*<< " in " << timer_end(SECOND) << "(s)"*/ << endl;

		    free_tree(root, &info[i]);
		    cout << "R-tree freed" << endl;
		}

	}

}