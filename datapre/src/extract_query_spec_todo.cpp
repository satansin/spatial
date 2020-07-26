#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "KDtree.h"
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
using namespace std;
using namespace trimesh;

const double PI = 3.14159265359;

double eucl_dist(point p, point q) {
	return sqrt((p[0] - q[0]) * (p[0] - q[0]) + (p[1] - q[1]) * (p[1] - q[1]) + (p[2] - q[2]) * (p[2] - q[2]));
}

double nn_dist(point p, vector<point> vertices) {
	double nn_dist = numeric_limits<double>::max();
	for (int i = 0; i < vertices.size(); i++) {
		double tmp_dist = eucl_dist(p, vertices[i]);
		if (tmp_dist - nn_dist < 0) {
			nn_dist = tmp_dist;
		}
	}
	return nn_dist;
}

double nn_dist(point p, trimesh::KDtree* kdtree_p) {
	auto nn = kdtree_p->closest_to_pt(&p[0]);
	point nn_p(nn[0], nn[1], nn[2]);
	return eucl_dist(nn_p, p);
}

double rand_double_in_range(double low, double high) {
	double r = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	return (low + r * (high - low));
}

point rand_pt(double x1, double x2, double y1, double y2, double z1, double z2) {
	return point(rand_double_in_range(x1, x2), rand_double_in_range(y1, y2), rand_double_in_range(z1, z2));
}

string get_foldername(const string path) {
	string ret;
	if (path[path.length() - 1] != '/') {
		ret = path + "/";
	} else {
		ret = path;
	}
	return ret;
}

int read_random_db_mesh(const string path, TriMesh*& db_mesh) {
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
    const int rand_id = rand() % num;

	int id;
	string s_file;

    for (int i = 0; i < num; i++) {
        ifs >> id >> s_file;
        if (i == rand_id) {
        	db_mesh = TriMesh::read(s_file);
        	cout << "Reading mesh #" << id << ": " << s_file << endl;
        	break;
        }
    }

    ifs.close();

    return rand_id;
}

int main(int argc, char** argv) {
	srand(time(NULL));

	if (argc < 9) {
		cerr << "Usage: " << argv[0] << " db_path window_x1 window_x2 window_y1 window_y2 window_z1 window_z2 output_filename" << endl;
		exit(1);
	}

	int argi = 0;
	const string db_path = argv[(++argi)];
	const double r_x1 = atof(argv[(++argi)]);
	const double r_x2 = atof(argv[(++argi)]);
	const double r_y1 = atof(argv[(++argi)]);
	const double r_y2 = atof(argv[(++argi)]);
	const double r_z1 = atof(argv[(++argi)]);
	const double r_z2 = atof(argv[(++argi)]);
	const string output_filename = argv[(++argi)];

	cout << endl;

    TriMesh* db_mesh = NULL;
    int db_mesh_id = 0;

    if (batch) {
    	db_mesh_id = read_random_db_mesh(db_path, db_mesh);
    } else {
    	db_mesh = TriMesh::read(db_path);
    }

    if (!db_mesh) {
    	cerr << "Fail reading DB mesh" << endl;
    	exit(1);
    }


    KDtree* db_kd = new KDtree(db_mesh->vertices);

	TriMesh* query_mesh = new TriMesh;
	vector<int> idmap;
	vector<double> driftmap;

	if (!batch) {
		printf("Filtering points in range ([%f, %f], [%f, %f], [%f, %f])\n",
			r_x1, r_x2, r_y1, r_y2, r_z1, r_z2);
	}
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

    // // rounding eta and epsilon
    // double real_eta = min(eta, 0.99999);
    // double real_epsilon = max(epsilon, 0.00001);
    // double sigma = real_epsilon / gsl_cdf_ugaussian_Qinv((1.0 - real_eta) / 2.0);

    float sigma = 0.0;

    if (noise_lvl > 0) {

	    // query_mesh->need_bbox();
	    // auto query_bbox = query_mesh->bbox;
	    // double max_dim_range = -1.0;
	    // double dim_range;
	    // for (int i = 0; i < 3; i++) {
	    // 	dim_range = query_bbox.max[i] - query_bbox.min[i];
	    // 	if (dim_range > max_dim_range) {
	    // 		max_dim_range = dim_range;
	    // 	}
	    // }

	    // sigma = max_dim_range * 0.0001 * noise_lvl;

    	switch (noise_unit) {
    	case 's':
    		sigma = 0.1 * noise_lvl;
    		break;
    	case 'm':
    		sigma = 0.3 * noise_lvl;
    		break;
    	case 'l':
    	default:
    		sigma = (float) noise_lvl;
    	}
    }

    const gsl_rng_type* RNG_TYPE;
    gsl_rng* rng;

	gsl_rng_env_setup();

	RNG_TYPE = gsl_rng_default;
	rng = gsl_rng_alloc(RNG_TYPE);

	// // for test
	// for (int i = 0; i < 100; i++) {
	// 	cout << gsl_ran_gaussian(rng, sigma) << endl;
	// }

	// int real_inlier_count = 0;
	double ground_truth_err_linear = 0.0;
	double ground_truth_err_quadratic = 0.0;

	for (int i = 0; i < query_mesh->vertices.size(); i++) {
		if (noise_lvl == 0) {
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

	rot(query_mesh, rand_double_in_range(0, 2 * PI),
		vec(rand_double_in_range(-1, 1), rand_double_in_range(-1, 1), rand_double_in_range(-1, 1)));
	trans(query_mesh, -mesh_center_of_mass(query_mesh));

	// cout << "Real inlier rate: " << ((double) real_inlier_count / m) << endl; 

	cout << "Writing query point cloud to " << output_filename << endl;
	query_mesh->write(output_filename);

	string info_filename = output_filename + ".info";
	cout << "Writing info file to " << info_filename << endl;
	ofstream info_ofs(info_filename);
	info_ofs << setprecision(10);
	// info_ofs << m << " " << epsilon << " " << eta << " " << db_mesh_id << endl;
	// info_ofs << real_inlier_count << " " << ground_truth_err_linear << " " << ground_truth_err_quadratic << endl;
	info_ofs << m << " " << sigma << " " << db_mesh_id << endl;
	info_ofs << ground_truth_err_linear << " " << ground_truth_err_quadratic << endl;
	for (int i = 0; i < idmap.size(); i++) {
		info_ofs << i << " " << idmap[i] << " " << driftmap[i] << endl;
	}
	info_ofs.close();
}