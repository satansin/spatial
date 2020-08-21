#include <iostream>
#include <fstream>
#include <string>

#include "mesh.h"
#include "pcr_adv.h"

using namespace std;

int main(int argc, char **argv) {

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " database_path grid_filename" << endl;
		exit(1);
	}

    int argi = 0;
    string db_path = argv[(++argi)];
    string grid_filename = argv[(++argi)];

    cout << "Reading database files from " << db_path << endl;
    DB_Meshes db_meshes;
    int num_meshes = db_meshes.read_from_path(db_path);
    cout << "Total no. meshes: " << num_meshes << endl << endl;

    cout << "Loading DB structure from " << grid_filename << endl;
    Struct_DB s_db;
    s_db.read(grid_filename, &db_meshes);

    string output_binary = get_bin_filename(grid_filename);
    cout << "Write binary to " << output_binary << endl << endl;
    s_db.save_bin(output_binary);

}