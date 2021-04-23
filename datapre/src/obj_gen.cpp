#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "ProgressBar.hpp"

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
	int num_meshes = read_db_meshes_n_names(db_path, db_meshes, db_mesh_names);

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

	vector<string> output_names;

	const int num_parts = 110;
	vector<int> part_sizes;

	for (int h = 0; h < num_parts; h++) {

		TriMesh* combined_mesh = new TriMesh;
		vector<int> point_sizes;

		int end = (h < num_parts - 1) ? (h * 4 + 4) : (h * 4 + 5); // such that mesh #440 is included in the last part

		for (int i = h * 4; i < end; i++) {

			cout << "Processing mesh #" << i << endl;

			string output_name = db_mesh_names[i] + string(fix_len, '0') + ".ply";
			output_names.push_back(output_name);

			append_mesh(db_meshes[i], combined_mesh);

			point_sizes.push_back(db_meshes[i]->vertices.size());

			ProgressBar bar(num_per_ext - 1, 70);

			for (int j = 1; j < num_per_ext; j++) {

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

					combined_mesh->vertices.push_back(v);

					// double dist = nn_dist(ext->vertices[i], db_kd);
					// ground_truth_err_linear += dist;
					// ground_truth_err_quadratic += dist * dist;
				}
				
				string j_str = to_string(j);
				string output_name = db_mesh_names[i] + string(fix_len - j_str.length(), '0') + j_str + ".ply";
				output_names.push_back(output_name);

				point_sizes.push_back(db_meshes[i]->vertices.size());

				++bar;
				bar.display();
			}

			bar.done();
		}

		part_sizes.push_back(point_sizes.size());

		combined_mesh->write(output_foldername + "combined.ply." + to_string(h));
		combined_mesh->clear();
		delete combined_mesh;
		
		ofstream ofs_combined_meta(output_foldername + "combined.meta." + to_string(h));
		for (auto &v: point_sizes) {
			ofs_combined_meta << v << endl;
		}
		ofs_combined_meta.close();
		vector<int>().swap(point_sizes);

	}

	ofstream ofs_meta(output_foldername + "meta.txt");
	ofs_meta << output_names.size() << endl;
	for (int i = 0; i < output_names.size(); i++) {
		ofs_meta << i << " " << output_names[i] << endl;
	}
	ofs_meta.close();

	ofstream ofs_comb_meta_ovr(output_foldername + "combined.meta");
	ofs_comb_meta_ovr << num_parts << endl;
	for (auto &v: part_sizes) {
		ofs_comb_meta_ovr << v << endl;
	}
	ofs_comb_meta_ovr.close();

	for (auto &m: db_meshes) {
		m->clear();
		delete m;
	}

}