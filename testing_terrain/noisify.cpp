#include "mathutil.h"

#include "TriMesh.h"
#include "KDtree.h"

#include <iostream>
#include <string>

#include <gsl/gsl_cdf.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

using namespace std;
using namespace trimesh;

#define NN_K 6

double get_sigma(const TriMesh* mesh, const KDtree* kd)
{
	double sum = 0.0;
	for (auto &v: mesh->vertices) {
		vector<const float *> knn;
		kd->find_k_closest_to_pt(knn, NN_K + 1, v); // include v itself
		double median = 0.5 * (sqrt(sq_dist(v, *((const point *) knn[3]))) + sqrt(sq_dist(v, *((const point *) knn[4]))));
		sum += median / 3.0;
	}

	return (sum / double(mesh->vertices.size()));
}

vec get_rand_direction(const vec &normal, int noise_type)
{
	if (noise_type == 0) {
		return vec(normal);
	} else if (noise_type == 1) {
		if (normal[0] != 0) {
			float y = ran_f(), z = ran_f();
			float x = (- y * normal[1] - z * normal[2]) / normal[0];
			return vec(x, y, z);
		}
		if (normal[1] != 0) {
			float x = ran_f(), z = ran_f();
			float y = (- x * normal[0] - z * normal[2]) / normal[1];
			return vec(x, y, z);
		}
		if (normal[2] != 0) {
			float x = ran_f(), y = ran_f();
			float z = (- x * normal[0] - y * normal[1]) / normal[2];
			return vec(x, y, z);
		}
	} else if (noise_type == 2) {
		return ran_p();
	}

	return vec(0, 0, 0);
}

int main(int argc, char* argv[])
{
	const string input_filename = argv[1];
	const int noise_type = atoi(argv[2]);
	int num_gen = 100;
	string output_prefix = "";

	if (argc >= 4) {
		num_gen = atoi(argv[3]);
	}

	if (argc >= 5) {
		output_prefix = argv[4];
	}

	auto mesh = TriMesh::read(input_filename);
	mesh->need_faces();
	mesh->need_normals();

	auto kd = new KDtree(mesh->vertices);

    const gsl_rng_type* RNG_TYPE;
    gsl_rng* rng;
	gsl_rng_env_setup();
	RNG_TYPE = gsl_rng_default;
	rng = gsl_rng_alloc(RNG_TYPE);
	
	double sigma = get_sigma(mesh, kd);

	for (int i = 1; i <= num_gen; i++) {
		TriMesh* gen_mesh = new TriMesh;

		for (int v = 0; v < mesh->vertices.size(); v++) {
			vec direction = get_rand_direction(mesh->normals[v], noise_type);
			double gs_ran = abs(gsl_ran_gaussian(rng, sigma));
			vec drift_vec = normalize(direction, gs_ran);

			gen_mesh->vertices.push_back(mesh->vertices[v] + drift_vec);
		}

		for (int f = 0; f < mesh->faces.size(); f++) {
			gen_mesh->faces.push_back(mesh->faces[f]);
		}

		gen_mesh->need_normals();

		gen_mesh->write("ply_ascii:" + output_prefix + to_string(i) + ".ply");
	}
}