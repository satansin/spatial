#include "TriMesh.h"
#include "TriMesh_algo.h"
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

double rand_double_in_range(double low, double high) {
	double r = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	return (low + r * (high - low));
}

point rand_pt(double x1, double x2, double y1, double y2, double z1, double z2) {
	return point(rand_double_in_range(x1, x2), rand_double_in_range(y1, y2), rand_double_in_range(z1, z2));
}

int main(int argc, char** argv) {
	srand(time(NULL));

	string db_filename = argv[1];

	double r_x1 = atof(argv[2]);
	double r_x2 = atof(argv[3]);
	double r_y1 = atof(argv[4]);
	double r_y2 = atof(argv[5]);
	double r_z1 = atof(argv[6]);
	double r_z2 = atof(argv[7]);

	int m = atoi(argv[8]);
	double epsilon_m = atof(argv[9]);
	double eta = atof(argv[10]);

	cout << endl;

	TriMesh *db_mesh = TriMesh::read(db_filename);
	TriMesh *query_mesh = new TriMesh;

	vector<int> idmap;

	printf("Filtering points in range ([%f, %f], [%f, %f], [%f, %f])\n", r_x1, r_x2, r_y1, r_y2, r_z1, r_z2);
	for (int i = 0; i < db_mesh->vertices.size(); i++) {
		point v = db_mesh->vertices[i];
		if (v[0] - r_x1 >= 0 && v[0] - r_x2 <= 0 &&
			v[1] - r_y1 >= 0 && v[1] - r_y2 <= 0 &&
			v[2] - r_z1 >= 0 && v[2] - r_z2 <= 0) {
			query_mesh->vertices.push_back(v);
			idmap.push_back(i);
		}
	}

	int in_range_count = query_mesh->vertices.size();
	printf("Found %d pts in range\n", in_range_count);

	int target_inlier_count = m * eta;
	printf("Target inlier count is %d, thus %d pts to be removed\n",
		target_inlier_count, max(in_range_count - target_inlier_count, 0));

	while (target_inlier_count < in_range_count) {
		// remove an in-range point randomly
		int ran = rand() % in_range_count;
		query_mesh->vertices.erase(query_mesh->vertices.begin() + ran);
		idmap.erase(idmap.begin() + ran);

		in_range_count--;
	}

	double ground_truth_err_linear = 0.0;
	double ground_truth_err_quadratic = 0.0;
	if (epsilon_m > 0) {
		for (int i = 0; i < query_mesh->vertices.size(); i++) {
			double target_drifting_dist = rand_double_in_range(0.0, epsilon_m);
			ground_truth_err_linear += target_drifting_dist;
			ground_truth_err_quadratic += target_drifting_dist * target_drifting_dist;

			double a = rand_double_in_range(0.0, 1.0) - 0.5,
				   b = rand_double_in_range(0.0, 1.0) - 0.5,
				   c = rand_double_in_range(0.0, 1.0) - 0.5;
			double norm = sqrt(a * a + b * b + c * c);
			double ran_scale = target_drifting_dist / norm;
			a *= ran_scale; b *= ran_scale; c *= ran_scale;

			query_mesh->vertices[i][0] += a;
			query_mesh->vertices[i][1] += b;
			query_mesh->vertices[i][2] += c;
		}
	}

	int outlier_to_add = m - target_inlier_count;
	if (outlier_to_add > 0)	{
		printf("Adding %d outliers\n", outlier_to_add);
	}
	while (outlier_to_add > 0) {
		// add a random noise point
		point n;
		double dist = -1.0;
		do {
			n = rand_pt(r_x1, r_x2, r_y1, r_y2, r_z1, r_z2);
			dist = nn_dist(n, db_mesh->vertices);
		} while (dist <= epsilon_m);

		query_mesh->vertices.push_back(n);
		idmap.push_back(-1);
		ground_truth_err_linear += dist;
		ground_truth_err_quadratic += dist * dist;

		outlier_to_add--;
	}

	rot(query_mesh, rand_double_in_range(0, 2 * PI),
		vec(rand_double_in_range(-1, 1), rand_double_in_range(-1, 1), rand_double_in_range(-1, 1)));
	trans(query_mesh, -mesh_center_of_mass(query_mesh));

	string output_filename = argv[11];
	cout << "Writing query point cloud to " << output_filename << endl;
	query_mesh->write(output_filename);

	string map_filename = output_filename + ".map";
	cout << "Writing map file to " << map_filename << endl;
	ofstream map_ofs(map_filename);
	for (int &i: idmap)	{
		map_ofs << i << endl;
	}
	map_ofs.close();

	string info_filename = output_filename + ".info";
	cout << "Writing info file to " << info_filename << endl;
	ofstream info_ofs(info_filename);
	info_ofs << ground_truth_err_linear << " " << ground_truth_err_quadratic << endl;
	info_ofs.close();
}