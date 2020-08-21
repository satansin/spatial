#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "KDtree.h"

#include "util.h"

#include <gsl/gsl_cdf.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unordered_set>

using namespace std;
using namespace trimesh;


void process_single_noise(TriMesh* input_mesh, KDtree* input_kd, double prop, double sigma, string output_filename) {

	TriMesh* new_mesh = new TriMesh;
	for (int i = 0; i < input_mesh->vertices.size(); i++) {
		auto v = input_mesh->vertices[i];
		new_mesh->vertices.push_back(v);
	}
	int m = new_mesh->vertices.size();

	double ground_truth_err_linear = 0.0;
	double ground_truth_err_quadratic = 0.0;

	if (prop > 0.0 && sigma > 0.0) {
	    const gsl_rng_type* RNG_TYPE;
	    gsl_rng* rng;

		gsl_rng_env_setup();

		RNG_TYPE = gsl_rng_default;
		rng = gsl_rng_alloc(RNG_TYPE);

		unordered_set<int> pro_selected_set;

		while (abs((float) pro_selected_set.size() / (float) m - prop) > 0.02 &&
			   abs(pro_selected_set.size() - (int) ((float) m * prop)) > 2) {
			pro_selected_set.clear();
			for (int i = 0; i < m; i++) {
				if (static_cast <double> (rand()) / static_cast <double> (RAND_MAX) <= prop) {
					pro_selected_set.insert(i);
				}
			}
		}

		for (int i = 0; i < m; i++) {
			if (pro_selected_set.find(i) != pro_selected_set.end()) {
				// noise added for the selected
				double target_drifting_dist = abs(gsl_ran_gaussian(rng, sigma));

				double a = rand_double_in_range(0.0, 1.0) - 0.5,
					   b = rand_double_in_range(0.0, 1.0) - 0.5,
					   c = rand_double_in_range(0.0, 1.0) - 0.5;
				double norm = sqrt(a * a + b * b + c * c);
				double ran_scale = target_drifting_dist / norm;
				a *= ran_scale; b *= ran_scale; c *= ran_scale;

				new_mesh->vertices[i][0] += a;
				new_mesh->vertices[i][1] += b;
				new_mesh->vertices[i][2] += c;

				double dist = nn_dist(new_mesh->vertices[i], input_kd);
				ground_truth_err_linear += dist;
				ground_truth_err_quadratic += dist * dist;
			}
		}
	}

	double ground_truth_mle = ground_truth_err_linear / (double) m;
	double ground_truth_mse = ground_truth_err_quadratic / (double) m;

	cout << "Writing output to " << output_filename << endl;
	new_mesh->write(output_filename);

	string info_filename = output_filename + ".info";
	cout << "Writing info file to " << info_filename << endl;
	ofstream info_ofs(info_filename);
	info_ofs << setprecision(10);
	info_ofs << m << " " << prop << " " << sigma << endl;
	info_ofs << ground_truth_mle << " " << ground_truth_mse << endl;
	info_ofs.close();

}

int main(int argc, char **argv) {

    srand(time(NULL));

	if (argc < 6) {
		cerr << "Usage: " << argv[0] << " input_filename prop sigma num output_filename" << endl;
		exit(1);
	}

	int argi = 0;
	const string input_filename = argv[(++argi)];
	const double prop = atof(argv[(++argi)]);
	const double sigma = atof(argv[(++argi)]);
	const int num = stoi(argv[(++argi)]);
	const string output_filename = argv[(++argi)];

	if (num > 99) {
		cerr << "Num no more than 99" << endl;
		exit(1);
	}

    TriMesh* input_mesh = TriMesh::read(input_filename);

    if (!input_mesh) {
    	cerr << "Fail reading input mesh" << endl;
    	exit(1);
    }

    KDtree* input_kd = new KDtree(input_mesh->vertices);

	process_single_noise(input_mesh, input_kd, 0.0, 0.0, output_filename + "s00.ply");

	for (int i = 1; i <= num; i++) {
		string i_str = to_string(i);
		string fn = output_filename + "s" + string(2 - i_str.length(), '0') + i_str + ".ply";
		process_single_noise(input_mesh, input_kd, prop, sigma, fn);
	}

}