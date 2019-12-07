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

string get_foldername(string path) {
    string ret;
    if (path[path.length() - 1] != '/') {
        ret = path + "/";
    } else {
        ret = path;
    }
    return ret;
}

int read_db_mesh_batch(string db_path, unordered_map<int, TriMesh*>& db_meshes, unordered_map<int, string>& db_filenames) {
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
        db_meshes[id] = TriMesh::read(s_file);
        db_filenames[id] = s_file;
    }

    ifs.close();

    return num;
}

int main(int argc, char* argv[]) {

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " input_filename output_filename (required if not batch) [-batch]" << endl;
		exit(1);
	}

	bool batch_mode = false;
    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-batch") {
            batch_mode = true;
        }
    }

	string input_filename = argv[1];
	string output_filename;
	if (batch_mode)
		output_filename = input_filename;
	else
		output_filename = argv[2];

    unordered_map<int, TriMesh*> db_meshes;
    unordered_map<int, string> db_filenames;

    if (batch_mode) {
        cout << "Num of meshes for batch: " << read_db_mesh_batch(input_filename, db_meshes, db_filenames) << endl;
    } else {
        db_meshes[0] = TriMesh::read(input_filename);
    }

	vector<rtree_info> info = {
		{ 5, 10, 3, 7 }
		// , { 10, 20, 3, 15 }
		// , { 15, 30, 3, 22 }
		// , { 20, 40, 3, 30 }
		// , { 25, 50, 3, 37 }
	};

	// string output_foldername;
	// if (batch_mode) {
	// 	output_foldername = get_foldername(output_filename);
	// 	system(string("mkdir -p " + output_filename).c_str());
	// }
	
	int id, n;
	TriMesh* mesh;
	R_TYPE** data;
	node_type* root;
	string realname;
	for (auto &p: db_meshes) {
		id = p.first;
		mesh = p.second;

		n = mesh->vertices.size();

		data = (R_TYPE **) malloc(sizeof(R_TYPE *) * n);
	    for (int i = 0; i < n; i++) {
	        data[i] = (R_TYPE *) malloc(sizeof(R_TYPE) * 3);
	        for (int j = 0; j < 3; j++) {
	            data[i][j] = mesh->vertices[i][j];
	        }
	    }

	    if (batch_mode) {
	    	// realname = output_foldername + "id." + to_string(id) + ".rstree";
	    	realname = db_filenames[id] + ".rstree";
	    } else {
	    	realname = output_filename;
	    }

		for (int i = 0; i < info.size(); i++) {
			// build R-tree for mesh points
		    cout << "Start building R-tree for mesh #" << id << " of info #" << (i + 1) << "..." << endl;
		    // timer_start();
		    build_tree(&root, data, n, &info[i]);
		    // cout << "Build R-tree in " << timer_end(SECOND) << "(s)" << endl;

		    realname += string("." + to_string(i + 1));

		    // save R-tree
		    // timer_start();
		    save_rtree(root, realname.c_str(), &info[i]);
		    cout << "Save R-tree to " << realname /*<< " in " << timer_end(SECOND) << "(s)"*/ << endl;

		    free_tree(root, &info[i]);
		    cout << "R-tree freed" << endl;
		}

	}

}