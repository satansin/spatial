#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "KDtree.h"

#include "util.h"

#include <gsl/gsl_cdf.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <limits>
#include <unordered_set>

using namespace std;
using namespace trimesh;

void exec_single(TriMesh* extr_mesh, int m, double pro, int noise_lvl, gsl_rng* rng, double sigma, KDtree* db_kd, int db_mesh_id, vector<int>& idmap, string output_filename, int index, int output_num) {

	TriMesh* query_mesh = new TriMesh;
	for (auto &v: extr_mesh->vertices) {
		query_mesh->vertices.push_back(v);
	}

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

	vector<double> driftmap;
	double ground_truth_err_linear = 0.0;
	double ground_truth_err_quadratic = 0.0;

	if (noise_lvl > 0) {
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

	double ground_truth_mle = ground_truth_err_linear / (double) m;
	double ground_truth_mse = ground_truth_err_quadratic / (double) m;

	rot(query_mesh, rand_double_in_range(0, 2 * PI),
		vec(rand_double_in_range(-1, 1), rand_double_in_range(-1, 1), rand_double_in_range(-1, 1)));
	// trans(query_mesh, -mesh_center_of_mass(query_mesh));
	query_mesh->need_bbox();
	trans(query_mesh, -(query_mesh->bbox.center()));

	string output_realname;
	if (output_num > 1) {
		output_realname = (output_num == 1) ? output_filename : (output_filename + "." + to_string(index));
	}

	cout << "Writing query point cloud to " << output_realname << endl;
	query_mesh->write(output_realname);

	string info_filename = output_realname + ".info";
	cout << "Writing info file to " << info_filename << endl;
	ofstream info_ofs(info_filename);
	info_ofs << setprecision(10);
	info_ofs << m << " " << sigma << " " << db_mesh_id << endl;
	info_ofs << ground_truth_mle << " " << ground_truth_mse << endl;
	for (int i = 0; i < idmap.size(); i++) {
		info_ofs << i << " " << idmap[i] << " " << driftmap[i] << endl;
	}
	info_ofs.close();

	delete(query_mesh);

}

int main(int argc, char** argv) {
	srand(time(NULL));

	if (argc < 11) {
		cerr << "Usage: " << argv[0] << " db_path db_mesh_id window_x1 window_x2 window_y1 window_y2 window_z1 window_z2 noise_lvl output_filename [-noise_unit=s|m|l] [-num=...]" << endl;
		exit(1);
	}

	char noise_unit = 'l';
	int output_num = 1;
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
        if (argv_str.rfind("-noise_unit", 0) == 0)
        	noise_unit = argv_str[12];
        if (argv_str.rfind("-num", 0) == 0)
        	output_num = atoi(argv[i] + 5);
    }

	int argi = 0;
	const string db_path = argv[(++argi)];
	const int db_mesh_id = stoi(argv[(++argi)]);
	const double r_x1 = atof(argv[(++argi)]);
	const double r_x2 = atof(argv[(++argi)]);
	const double r_y1 = atof(argv[(++argi)]);
	const double r_y2 = atof(argv[(++argi)]);
	const double r_z1 = atof(argv[(++argi)]);
	const double r_z2 = atof(argv[(++argi)]);
	const int noise_lvl = stoi(argv[(++argi)]);
	const string output_filename = argv[(++argi)];

    TriMesh* db_mesh = TriMesh::read(db_path);

    if (!db_mesh) {
    	cerr << "Fail reading DB mesh" << endl;
    	exit(1);
    }


    KDtree* db_kd = new KDtree(db_mesh->vertices);

	TriMesh* query_mesh = new TriMesh;
	vector<int> idmap;
	// vector<double> driftmap;

	printf("Filtering points in range ([%f, %f], [%f, %f], [%f, %f])\n",
		r_x1, r_x2, r_y1, r_y2, r_z1, r_z2);
	for (int i = 0; i < db_mesh->vertices.size(); i++) {
		auto v = db_mesh->vertices[i];
		if (v[0] - r_x1 >= 0 && v[0] - r_x2 <= 0 &&
			v[1] - r_y1 >= 0 && v[1] - r_y2 <= 0 &&
			v[2] - r_z1 >= 0 && v[2] - r_z2 <= 0) {
			
			query_mesh->vertices.push_back(v);
			idmap.push_back(i);
		
		}
	}

	int m = query_mesh->vertices.size();
	printf("Found %d pts in range\n", m);

    query_mesh->need_bsphere();
    float bsphere_diam = query_mesh->bsphere.r * 2.0;

    double sigma = bsphere_diam * 0.005;

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

	for (int i = 0; i < output_num; i++) {
		exec_single(query_mesh, m, pro, noise_lvl, rng, sigma, db_kd, db_mesh_id, idmap, output_filename, i, output_num);
	}



 //    float sigma = 0.0;
	// double ground_truth_err_linear = 0.0;
	// double ground_truth_err_quadratic = 0.0;

 //    if (noise_lvl > 0) {

	//     query_mesh->need_bsphere();
	//     float bsphere_diam = query_mesh->bsphere.r * 2.0;

	//     sigma = bsphere_diam * 0.005;

 //    	switch (noise_unit) {
 //    	case 's':
 //    		sigma = 0.1 * sigma;
 //    		break;
 //    	case 'm':
 //    		sigma = 0.3 * sigma;
 //    		break;
 //    	case 'l':
 //    	default:
 //    		break;
 //    	}

	//     cout << "sigma = " << sigma << endl;

	//     const gsl_rng_type* RNG_TYPE;
	//     gsl_rng* rng;

	// 	gsl_rng_env_setup();

	// 	RNG_TYPE = gsl_rng_default;
	// 	rng = gsl_rng_alloc(RNG_TYPE);

	// 	// // for test
	// 	// for (int i = 0; i < 100; i++) {
	// 	// 	cout << gsl_ran_gaussian(rng, sigma) << endl;
	// 	// }

	// 	float pro = 0.1 * noise_lvl;
	// 	cout << "prop = " << pro << endl;

	// 	unordered_set<int> pro_selected_set;

	// 	while (abs((float) pro_selected_set.size() / (float) m - pro) > 0.02 &&
	// 		   abs(pro_selected_set.size() - (int) ((float) m * pro)) > 2) {
	// 		pro_selected_set.clear();
	// 		for (int i = 0; i < m; i++) {
	// 			if (static_cast <double> (rand()) / static_cast <double> (RAND_MAX) <= pro) {
	// 				pro_selected_set.insert(i);
	// 			}
	// 		}
	// 	}

	// 	for (int i = 0; i < m; i++) {
	// 		if (pro_selected_set.find(i) == pro_selected_set.end()) {
	// 			// not selected, thus no noise added
	// 			driftmap.push_back(0.0);
	// 		} else {
	// 			// double target_drifting_dist = rand_double_in_range(0.0, epsilon);
	// 			double target_drifting_dist = abs(gsl_ran_gaussian(rng, sigma));
	// 			driftmap.push_back(target_drifting_dist);

	// 			double a = rand_double_in_range(0.0, 1.0) - 0.5,
	// 				   b = rand_double_in_range(0.0, 1.0) - 0.5,
	// 				   c = rand_double_in_range(0.0, 1.0) - 0.5;
	// 			double norm = sqrt(a * a + b * b + c * c);
	// 			double ran_scale = target_drifting_dist / norm;
	// 			a *= ran_scale; b *= ran_scale; c *= ran_scale;

	// 			query_mesh->vertices[i][0] += a;
	// 			query_mesh->vertices[i][1] += b;
	// 			query_mesh->vertices[i][2] += c;

	// 			double dist = nn_dist(query_mesh->vertices[i], db_kd);
	// 			ground_truth_err_linear += dist;
	// 			ground_truth_err_quadratic += dist * dist;
	// 		}
	// 	}
	// } else {
	// 	for (int i = 0; i < m; i++) {
	// 		driftmap.push_back(0.0);
	// 	}
 //    }

	// double ground_truth_mle = ground_truth_err_linear / (double) m;
	// double ground_truth_mse = ground_truth_err_quadratic / (double) m;

	// rot(query_mesh, rand_double_in_range(0, 2 * PI),
	// 	vec(rand_double_in_range(-1, 1), rand_double_in_range(-1, 1), rand_double_in_range(-1, 1)));
	// // trans(query_mesh, -mesh_center_of_mass(query_mesh));
	// query_mesh->need_bbox();
	// trans(query_mesh, -(query_mesh->bbox.center()));

	// cout << "Writing query point cloud to " << output_filename << endl;
	// query_mesh->write(output_filename);

	// string info_filename = output_filename + ".info";
	// cout << "Writing info file to " << info_filename << endl;
	// ofstream info_ofs(info_filename);
	// info_ofs << setprecision(10);
	// info_ofs << m << " " << sigma << " " << db_mesh_id << endl;
	// info_ofs << ground_truth_mle << " " << ground_truth_mse << endl;
	// for (int i = 0; i < idmap.size(); i++) {
	// 	info_ofs << i << " " << idmap[i] << " " << driftmap[i] << endl;
	// }
	// info_ofs.close();
}