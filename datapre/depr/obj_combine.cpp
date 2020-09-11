#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "KDtree.h"
#include "util.h"

#include <unordered_set>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <limits>

using namespace std;
using namespace trimesh;

int main(int argc, char** argv) {
	srand(time(NULL));

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " db_path [-num_parts=...] [-part_i=...]" << endl;
		exit(1);
	}

	int num_parts = 1;
	int part_i = -1;
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
        if (argv_str.rfind("-num_parts", 0) == 0)
        	num_parts = stoi(argv[i] + 11);
        else if (argv_str.rfind("-part_i", 0) == 0)
        	part_i = stoi(argv[i] + 8);
    }

    if (num_parts > 1) {
	    if (part_i < 0 || part_i >= num_parts) {
	    	cerr << "Incorrect part index" << endl;
	    	exit(1);
	    }
	} else {
		part_i = 0;
	}

	int argi = 0;
	const string db_path = argv[(++argi)];
	const string db_folder = get_foldername(db_path);

    vector<string> db_filenames;
	int num_meshes = read_db_mesh_filenames(db_path, db_filenames);

    int part_size = num_meshes / num_parts;

    vector<TriMesh*> db_meshes;
    if (num_parts > 1) {
		cout << "Process part #" << part_i << endl;
	}
	read_db_mesh_batch(db_filenames, part_i, part_size, db_meshes);

	TriMesh* combined_mesh = new TriMesh;
	for (auto &m: db_meshes) {
		for (auto &p: m->vertices) {
			combined_mesh->vertices.push_back(p);
		}
	}

	string output_filename = db_folder + "combined.ply";
	if (num_parts > 1) {
		output_filename = output_filename + "." + to_string(part_i);
	}
	cout << "Write combined mesh to " << output_filename << endl;
	combined_mesh->write(output_filename);

	string output_metafile = db_folder + "combined.meta";
	if (num_parts > 1) {
		output_metafile = output_metafile + "." + to_string(part_i);
		if (part_i == 0) {
			ofstream sum_meta_ofs(db_folder + "combined.meta");
			sum_meta_ofs << num_parts << endl;
			sum_meta_ofs.close();
		}
	}
	int sum_pts = 0;
	ofstream meta_ofs(output_metafile);
	if (num_parts == 1) {
		meta_ofs << 1 << endl;
	}
	for (auto &m: db_meshes) {
		meta_ofs << m->vertices.size() << endl;
		sum_pts += m->vertices.size();
	}
	meta_ofs.close();

	cout << "Sum pts: " << sum_pts << endl;

	ofstream sum_ofs;
	sum_ofs.open("test.txt", ofstream::out | ofstream::app);
	for (int i = 0; i < argc; i++) {
		sum_ofs << " " << argv[i];
	}
	sum_ofs << " " << sum_pts << endl;
	sum_ofs.close();

}