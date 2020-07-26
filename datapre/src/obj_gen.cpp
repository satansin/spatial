#include "TriMesh.h"
#include "TriMesh_algo.h"
#include <ctime>
#include <cstdlib>
#include <string>
#include <vector>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "util.h"

using namespace std;
using namespace trimesh;

int read_db_meshes(const string path, vector<TriMesh*>& db_meshes, vector<string>& db_mesh_names) {
	auto db_folder = get_foldername(path);
	ifstream ifs(db_folder + "meta.txt");

	if (!ifs) {
		cerr << "Fail reading meta info" << endl;
		exit(1);
	} else {
		cout << "Reading meta.txt..." << endl;
	}

	int num;
    ifs >> num;

	int id;
	string s_file;

    for (int i = 0; i < num; i++) {
        ifs >> id >> s_file;
    	db_meshes.push_back(TriMesh::read(s_file));
    	string fn = get_filename(s_file);
    	// cout << fn.substr(0, fn.length() - 4) << endl;
    	db_mesh_names.push_back(fn.substr(0, fn.length() - 4));
    	cout << "Reading mesh #" << id << ": " << s_file << endl;
    }

    ifs.close();

    return num;
}

const double sigma = 200;

int main(int argc, char** argv) {
	srand(time(NULL));

	if (argc < 5) {
		cerr << "Usage: " << argv[0] << " db_path num_per_ext output_path -batch" << endl;
		exit(1);
	}

	bool batch = false;
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
        if (argv_str == "-batch")
            batch = true;
    }

    if (!batch) {
    	cerr << "Non-batch not supported!" << endl;
    	exit(1);
    }

	int argi = 0;
	const string db_path = argv[(++argi)];
	const double num_per_ext = stod(argv[(++argi)]);
	const string output_path = argv[(++argi)];

	const string output_foldername = get_foldername(output_path);

	vector<TriMesh*> db_meshes;
	vector<string> db_mesh_names;
	int num_meshes = read_db_meshes(db_path, db_meshes, db_mesh_names);

	const gsl_rng_type* RNG_TYPE;
	gsl_rng* rng;
	gsl_rng_env_setup();
	RNG_TYPE = gsl_rng_default;
	rng = gsl_rng_alloc(RNG_TYPE);

	
	// // for test
	// for (int i = 0; i < 100; i++) {
	// 	cout << abs(gsl_ran_gaussian(rng, sigma)) << endl;
	// }
	// exit(0);

	int global_id = 0;
	const int total = num_meshes * num_per_ext;

	ofstream ofs_meta(output_foldername + "meta.txt");
	ofs_meta << total << endl;

	for (int i = 0; i < num_meshes; i++) {

		cout << "Processing mesh #" << i << endl;

		string first_output_filename = output_foldername + db_mesh_names[i] + string(fix_len, '0') + ".ply";
		db_meshes[i]->write(first_output_filename);
		ofs_meta << (global_id++) << " " << first_output_filename << endl;

		for (int j = 1; j < num_per_ext; j++) {

			TriMesh* ext = new TriMesh;

			for (int k = 0; k < db_meshes[i]->vertices.size(); k++) {

				auto v = db_meshes[i]->vertices[k];

				double target_drifting_dist = abs(gsl_ran_gaussian(rng, sigma));

				double a = rand_double_in_range(0.0, 1.0) - 0.5,
					   b = rand_double_in_range(0.0, 1.0) - 0.5,
					   c = rand_double_in_range(0.0, 1.0) - 0.5;
				double norm = sqrt(a * a + b * b + c * c);
				double ran_scale = target_drifting_dist / norm;
				a *= ran_scale; b *= ran_scale; c *= ran_scale;

				v[0] += a;
				v[1] += b;
				v[2] += c;
				// cout << a << " " << b << " " << c << endl;

				ext->vertices.push_back(v);

				// double dist = nn_dist(ext->vertices[i], db_kd);
				// ground_truth_err_linear += dist;
				// ground_truth_err_quadratic += dist * dist;
			}
			
			string j_str = to_string(j);
			string output_filename = output_foldername + db_mesh_names[i] + string(fix_len - j_str.length(), '0') + j_str + ".ply";
			ext->write(output_filename);
			ofs_meta << (global_id++) << " " << output_filename << endl;

			delete ext;
		}
	}

	ofs_meta.close();

}