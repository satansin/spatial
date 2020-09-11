#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>
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

// void build_rtree(TriMesh* mesh, vector<rtree_info>& info, int n, string realname) {
void build_rtree(TriMesh* mesh, rtree_info *info, FILE *fp) {
	int n = mesh->vertices.size();

	R_TYPE** data = (R_TYPE **) malloc(sizeof(R_TYPE *) * n);
    for (int k = 0; k < n; k++) {
        data[k] = (R_TYPE *) malloc(sizeof(R_TYPE) * 3);
        for (int l = 0; l < 3; l++) {
            data[k][l] = (int) (mesh->vertices[k][l] * 1e5);
        }
    }
	node_type* root;

	// build R-tree for mesh points
	// cout << "Start building R-tree for mesh #" << id << " of info #" << k << "..." << endl;
	build_tree(&root, data, n, info);

	// save R-tree
	save_rtree_comb(root, fp, info);

	free_tree(root, info);
	// cout << "R-tree freed" << endl;

	for (int k = 0; k < n; k++) {
		free(data[k]);
	}
	free(data);
}

int main(int argc, char* argv[]) {

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " input_path [-part_i=...]" << endl;
		exit(1);
	}

	int part_i = 0;
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
        if (argv_str.rfind("-part_i", 0) == 0)
        	part_i = stoi(argv[i] + 8);
    }

	const string input_path = argv[1];
	const string input_folder = get_foldername(input_path);

	vector<TriMesh*> input_meshes;
	read_db_meshes_combined_by_part(input_path, part_i, input_meshes);

	cout << input_meshes.size() << endl;

	vector<rtree_info> info;
	info.push_back(read_rstree_info("../common/config/rstree.pcd.config"));

	string realname = input_folder + "combined.ply." + to_string(part_i) + ".rst.0";
	cout << "Start building R-tree and save to " << realname << endl;

	FILE *c_fp;
	c_fp = fopen(realname.c_str(), "a");

	for (auto &m: input_meshes) {
		build_rtree(m, &info[0], c_fp);
		delete m;
	}

	fclose(c_fp);

}