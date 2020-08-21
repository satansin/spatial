#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
extern "C" {
#include "rtree.h"
#include "donut.h"
}
using namespace std;

inline float random_float(float max) {
	return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max));
}

R_TYPE** make_data(int no_data, rtree_info* aInfo, string datafile = "") {
	R_TYPE **data = (R_TYPE **) malloc(sizeof(R_TYPE *) * no_data);

	int i, j;
	for(i = 0; i < no_data; i++) {
        data[i] = (R_TYPE *) malloc(sizeof(R_TYPE) * aInfo->dim);
	}

	ifstream data_ifs;
	if (datafile.length() != 0) {
		data_ifs.open(datafile);
		cout << "Read points from " << datafile << endl;
	}

	// ofstream test_ofs("test_data.txt");

	for(i = 0; i < no_data; i++) {
		for (j = 0; j < aInfo->dim; j++) {
			if (datafile.length() != 0) {
				data_ifs >> data[i][j];
			} else {
				data[i][j] = random_float(10000.0);
				// test_ofs << data[i][j] << endl;
			}
		}
	}

	// test_ofs.close();

	data_ifs.close();

	return data;
}

void cerr_usage_exit(char* arg0) {
	cerr << "Usage: " << arg0 << " [save num_data] | [read] | [query num_data]" << endl;
	exit(1);
}

int main(int argc, char** argv) {
	srand(time(NULL));

	if (argc < 2) {
		cerr_usage_exit(argv[0]);
	}

	bool save_mode = false, read_mode = false, query_mode = false;
	if (string(argv[1]) == "save") {
		save_mode = true;
	} else if (string(argv[1]) == "read") {
		read_mode = true;
	} else if (string(argv[1]) == "query") {
		query_mode = true;
	} else {
		cerr_usage_exit(argv[0]);
	}

	rtree_info aInfo = { 5, 10, 3, 7 };
	int no_data;
	R_TYPE** data;
	node_type *root;
	// char tmp_save_filename[] = "test.rstree";
	char tmp_save_filename[] = "test-01.rstree";
	int i, j;

	string test_datafile = "test_data.txt";

	if (save_mode || query_mode) {
		no_data = atoi(argv[2]);
		// data = make_data(no_data, &aInfo, test_datafile);
		data = make_data(no_data, &aInfo);

		build_tree(&root, data, no_data, &aInfo);
	} else {
		read_rtree(&root, tmp_save_filename, &aInfo);
	}

	if (save_mode) {
		save_rtree(root, tmp_save_filename, &aInfo);
	}
	
	if (query_mode) {
		// R_TYPE *query;
		// query = (R_TYPE *) malloc(sizeof(R_TYPE) * aInfo.dim);
		// for (j = 0; j < aInfo.dim; j++)	{
		// 	query[j] = 500.0;
		// }

		// NN_type *NNresult_1;
		// NN_type *NNresult_2;
		// k_NN_search(root, query, 1, &NNresult_1, &aInfo);
		// k_NN_search_sphere(root, query, 1, &NNresult_2, &aInfo, 10000.0);

		// cout << sqrt(NNresult_1->dist) << endl;
		// cout << sqrt(NNresult_2->dist) << endl;

		R_TYPE *donut_query;
		donut_query = (R_TYPE *) malloc(sizeof(R_TYPE) * 7);
		for (j = 0; j < aInfo.dim; j++)	{
			donut_query[j] = 500;
		}
		donut_query[3] = 150;
		donut_query[4] = 12;
		donut_query[5] = 17;
		donut_query[6] = -7;

		struct Circle cir;
		for (j = 0; j < 3; j++) {
			cir.o[j] = donut_query[j];
		}
		cir.r = donut_query[3];
		for (j = 0; j < 3; j++) {
			cir.n[j] = donut_query[j+4];
		}

		pre_compute(&cir);

		double brute_force_donut_dist_sq = INFINITY;
		int brute_force_donut_i;
		double brute_force_eucl_dist_sq;
		double x[3];
		for (i = 0; i < no_data; i++) {
			double e_sq = 0.0;
			for (j = 0; j < 3; j++) {
				x[j] = data[i][j];
				e_sq += (x[j] - cir.o[j]) * (x[j] - cir.o[j]);
			}
			double d_sq = donut_dist_sq(x, &cir);
			if (d_sq < brute_force_donut_dist_sq && e_sq > cir.sq_r) {
				brute_force_donut_dist_sq = d_sq;
				brute_force_donut_i = i;
				brute_force_eucl_dist_sq = e_sq;
			}
		}
		cout << "Donut NN " << sqrt(brute_force_donut_dist_sq) << ", Eucl " << sqrt(brute_force_eucl_dist_sq) << " at " << brute_force_donut_i << endl;


		NN_type *donutNNresult;
		k_Donut_NN_search(root, donut_query, 1, &donutNNresult, &aInfo);
		cout << donutNNresult->dist << " at " << donutNNresult->oid << endl;
	}

}