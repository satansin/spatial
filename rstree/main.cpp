#include <iostream>
#include <ctime>
#include <cstdlib>
extern "C" {
#include "rtree.h"
}
using namespace std;

inline float random_float(float max) {
	return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max));
}

R_TYPE** make_data(int no_data, rtree_info* aInfo) {
	R_TYPE **data = (R_TYPE **) malloc(sizeof(R_TYPE *) * no_data);

	int i, j;
	for(i = 0; i < no_data; i++) {
        data[i] = (R_TYPE *) malloc(sizeof(R_TYPE) * aInfo->dim);
	}
	
	for(i = 0; i < no_data; i++) {
		for (j = 0; j < aInfo->dim; j++) {
			data[i][j] = random_float(1000.0);
		}
	}

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

	if (save_mode || query_mode) {
		no_data = atoi(argv[2]);
		data = make_data(no_data, &aInfo);

		build_tree(&root, data, no_data, &aInfo);
	} else {
		read_rtree(&root, tmp_save_filename, &aInfo);
	}

	if (save_mode) {
		save_rtree(root, tmp_save_filename, &aInfo);
	}
	
	if (query_mode) {
		R_TYPE *query;
		query = (R_TYPE *) malloc(sizeof(R_TYPE) * aInfo.dim);
		for (j = 0; j < aInfo.dim; j++)	{
			query[j] = 500.0;
		}

		NN_type *NNresult_1;
		NN_type *NNresult_2;
		k_NN_search(root, query, 1, &NNresult_1, &aInfo);
		k_NN_search_sphere(root, query, 1, &NNresult_2, &aInfo, 10000.0);

		cout << sqrt(NNresult_1->dist) << endl;
		cout << sqrt(NNresult_2->dist) << endl;
	}


}