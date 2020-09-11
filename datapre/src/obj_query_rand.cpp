#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "KDtree.h"
#include "util.h"

#include <gsl/gsl_cdf.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
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

const int NOISE_LVL_MAX = 4;

void write_query(const string output_filename, TriMesh* query_mesh, int m, double sigma, string db_mesh_name, double ground_truth_mle, double ground_truth_mse, vector<int>& idmap, vector<double>& driftmap) {
	cout << "Writing query point cloud to " << output_filename << endl;
	query_mesh->write(output_filename);

	string info_filename = output_filename + ".info";
	cout << "Writing info file to " << info_filename << endl;
	ofstream info_ofs(info_filename);
	info_ofs << setprecision(10);
	info_ofs << m << " " << sigma << " " << db_mesh_name << endl;
	info_ofs << ground_truth_mle << " " << ground_truth_mse << endl;
	for (int i = 0; i < idmap.size(); i++) {
		info_ofs << i << " " << idmap[i] << " " << driftmap[i] << endl;
	}
	info_ofs.close();
}

void process_noise(int p_id, int noise_lvl, double bsphere_diam, char noise_unit, TriMesh* query_mesh, int m, vector<int>& idmap, string db_mesh_name, KDtree* db_kd, const string output_folder) {

	vector<double> driftmap;
	double ground_truth_err_linear = 0.0;
	double ground_truth_err_quadratic = 0.0;
	double sigma = 0.0;

	if (noise_lvl > 0) {

		sigma = bsphere_diam * 0.005;

		switch (noise_unit) {
		case 's':
			sigma = 0.1 * sigma;
			break;
		case 'm':
			sigma = 0.3 * sigma;
			break;
		case 'l':
		default:
			break;
		}

		cout << "sigma = " << sigma << endl;

		const gsl_rng_type* RNG_TYPE;
		gsl_rng* rng;

		gsl_rng_env_setup();

		RNG_TYPE = gsl_rng_default;
		rng = gsl_rng_alloc(RNG_TYPE);

		// // for test
		// for (int i = 0; i < 100; i++) {
		// 	cout << gsl_ran_gaussian(rng, sigma) << endl;
		// }

		float pro = 0.1 * noise_lvl;
		cout << "prop = " << pro << endl;

		unordered_set<int> pro_selected_set;

		while (abs((float) pro_selected_set.size() / (float) m - pro) > 0.02 &&
			   abs(pro_selected_set.size() - (int) ((float) m * pro)) > 2) {
			pro_selected_set.clear();
			for (int i = 0; i < m; i++) {
				if (static_cast <double> (rand()) / static_cast <double> (RAND_MAX) <= pro) {
					pro_selected_set.insert(i);
				}
			}
		}

		for (int i = 0; i < m; i++) {
			if (pro_selected_set.find(i) == pro_selected_set.end()) {
				// not selected, thus no noise added
				driftmap.push_back(0.0);
			} else {
				// double target_drifting_dist = rand_double_in_range(0.0, epsilon);
				double target_drifting_dist = abs(gsl_ran_gaussian(rng, sigma));
				driftmap.push_back(target_drifting_dist);

				double a = rand_double_in_range(0.0, 1.0) - 0.5,
					   b = rand_double_in_range(0.0, 1.0) - 0.5,
					   c = rand_double_in_range(0.0, 1.0) - 0.5;
				double norm = sqrt(a * a + b * b + c * c);
				double ran_scale = target_drifting_dist / norm;
				a *= ran_scale; b *= ran_scale; c *= ran_scale;

				query_mesh->vertices[i][0] += a;
				query_mesh->vertices[i][1] += b;
				query_mesh->vertices[i][2] += c;

				double dist = nn_dist(query_mesh->vertices[i], db_kd);
				ground_truth_err_linear += dist;
				ground_truth_err_quadratic += dist * dist;
			}
		}

	} else {
		for (int i = 0; i < m; i++) {
			driftmap.push_back(0.0);
		}

	}

	rot(query_mesh, rand_double_in_range(0, 2 * PI), vec(rand_double_in_range(-1, 1), rand_double_in_range(-1, 1), rand_double_in_range(-1, 1)));
	trans(query_mesh, -mesh_center_of_mass(query_mesh));

	const string output_filename = output_folder + "q_" + to_string(p_id) + "." + to_string(noise_lvl) + ".ply";
	double ground_truth_mle = ground_truth_err_linear / (double) m;
	double ground_truth_mse = ground_truth_err_quadratic / (double) m;

	write_query(output_filename, query_mesh, m, sigma, db_mesh_name, ground_truth_mle, ground_truth_mse, idmap, driftmap);

}

bool process_single(int p_id, vector<TriMesh*>& db_meshes, int num_meshes, vector<string>& db_names, char noise_unit, const string output_folder, int& m) {

	cout << "Process #" << p_id << endl;

	int rand_mesh_id = rand() % num_meshes;
	auto rand_mesh = db_meshes[rand_mesh_id];
	auto rand_kd = new KDtree(rand_mesh->vertices);

	m = rand_mesh->vertices.size();
	if (m < 40) {
		return false;
	}

	vector<TriMesh*> query_meshes;
	for (int i = 0; i <= NOISE_LVL_MAX; i++) {
		query_meshes.push_back(new TriMesh);
	}
	vector<int> idmap;

	for (int i = 0; i < rand_mesh->vertices.size(); i++) {
		auto v = rand_mesh->vertices[i];
		for (int j = 0; j <= NOISE_LVL_MAX; j++) {
			query_meshes[j]->vertices.push_back(v);
		}
		idmap.push_back(i);
	}

	query_meshes[0]->need_bsphere();
	double query_bsphere_diam = query_meshes[0]->bsphere.r * 2.0;

	for (int noise_lvl = 0; noise_lvl <= NOISE_LVL_MAX; noise_lvl++) {
		process_noise(p_id, noise_lvl, query_bsphere_diam, noise_unit, query_meshes[noise_lvl], m, idmap, db_names[rand_mesh_id], rand_kd, output_folder);
	}

	return true;
	
}

int main(int argc, char** argv) {
	srand(time(NULL));

	if (argc < 4) {
		cerr << "Usage: " << argv[0] << " db_path num_rand output_path [-noise_unit=s|m|l]" << endl;
		exit(1);
	}

	char noise_unit = 'l';
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
        if (argv_str.rfind("-noise_unit", 0) == 0)
        	noise_unit = argv_str[12];
    }

	int argi = 0;
	const string db_path = argv[(++argi)];
	// const double diam = atof(argv[(++argi)]);
	const int num_rand = atoi(argv[(++argi)]);
	const string output_path = string(argv[(++argi)]);
	const string output_folder = get_foldername(output_path);

	cout << "db_path: " << db_path << endl;
	// cout << "diam: " << diam << endl;
	cout << "num_rand: " << num_rand << endl;
	cout << "output_path: " << output_path << endl;
	cout << "noise_unit: " << noise_unit << endl;

	vector<string> db_names;
	int num_meshes = read_db_mesh_filenames(db_path, db_names); // use read_filenames instead of read_names because the names are actually stored

	vector<TriMesh*> db_meshes;
	read_db_meshes_combined(db_path, db_meshes);

	// vector<KDtree*> db_kds;
	// for (auto &m: db_meshes) {
	// 	m->need_bsphere();
	// 	db_kds.push_back(new KDtree(m->vertices));
	// }

	vector<int> num_qs;
	int acc_m = 0;
	for (int i = 0; i < num_rand; i++) {
		int m;
		while(!process_single(i, db_meshes, num_meshes, db_names, noise_unit, output_folder, m));
		num_qs.push_back(m);
		acc_m += m;
	}

	ofstream stat_ofs(output_path + ".stat");
	for (int i = 0; i < num_rand; i++) {
		stat_ofs << num_qs[i] << endl;
	}
	stat_ofs << acc_m << endl;
	stat_ofs << ((double) acc_m / (double) num_rand) << endl;

	for (auto &v: db_meshes) {
		delete v;
	}

}