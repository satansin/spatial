#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <vector>

#include "TriMesh.h"
#include "TriMesh_algo.h"

#include "mathutil.h"
#include "matching.h"

using namespace std;
using namespace trimesh;

#define NUM_DATAPOINTS 17
#define NUM_FIRST_ITRS 20
#define MIN_ERR 0.01f
#define MAX_ERR 1.0f
#define NTRIALS 100

int main(int argc, char* argv[])
{
	if (argc < 3) {
		cerr << "Usage: " << argv[0] << " filename_model filename_data [variant_corr=0] [variant_align=0] [num_itr="
			 << MAX_ITR << "] [num_trial=" << NTRIALS << "] /*[num_multi_data=0]*/ [output_filename=errs.dat]" << endl;
		exit(1);
	}

	const string filename_model(argv[1]);
	const string filename_data(argv[2]);

	int variant_corr = 0;
	int variant_align = 0;
	int num_itr = MAX_ITR;
	int num_trial = NTRIALS;
	// int num_multi_data = 0;
	string output_filename = "errs.dat";

	if (argc >= 4) {
		variant_corr = atoi(argv[3]);
	}
	if (argc >= 5) {
		variant_align = atoi(argv[4]);
	}
	if (argc >= 6) {
		num_itr = atoi(argv[5]);
	}
	if (argc >= 7) {
		num_trial = atoi(argv[6]);
	}
	// if (argc >= 8) {
	// 	num_multi_data = atoi(argv[7]);
	// }
	if (argc >= 8) {
		output_filename = argv[7];
	}

	cout << "----------------------------------------" << endl;
	cout << "Parameters" << endl;
	cout << "Model: " << filename_model << endl;
	cout << "Data: " << filename_data << endl;
	cout << "Variant Correspondence: " << variant_corr << endl;
	cout << "Variant Alignment: " << variant_align << endl;
	cout << "Num of Iterations: " << num_itr << endl;
	cout << "Num of Trials: " << num_trial << endl;
	cout << "----------------------------------------" << endl;

	cout.precision(15);

	// srand(time(NULL));
	srand(934746273); // use fixed srand to ensure every run has the same initial values

	ICP_Matching matching(filename_model, filename_data, variant_corr, variant_align, num_itr);
	matching.load();
	cout << endl;

	// Start the trials
	const float factor = pow(MIN_ERR / MAX_ERR, 1.0f / (NUM_DATAPOINTS - 1));
	vector<vector<float>> errs(NUM_DATAPOINTS);

	// Initialize the result arrays
	for (int i = 0; i < NUM_DATAPOINTS; i++) {
		errs[i] = vector<float>(NUM_FIRST_ITRS + 1);
	}

	for (int trial = 0; trial < num_trial; trial++) {
		printf("%4d:   ", trial + 1); fflush(stdout);

		// All of the initial transforms for a single trial will have rotations about the same axis, and multiples of same trans
		vec rotaxis = rnd2sph(ran_f_0_1(), ran_f_0_1());
		vec transdir = rnd2sph(ran_f_0_1(), ran_f_0_1());
		float rotamount = ran_f_0_1();
		float transamount = ran_f_0_1();

		for (int x = 0; x < NUM_DATAPOINTS; x++) {
			float starting_err = MAX_ERR * pow(factor, float(x));
			rotamount *= factor;
			transamount *= factor;

			// Find a transformation that will produce point-to-point distance of starting_err
			xform xf;
			float ratio;
			int it = 0;
			do {
				xf = xform::trans(transamount * transdir) *
				     xform::rot(rotamount, rotaxis);
				float actual_err = cal_err(matching.mesh_data, xf);
				// cout << actual_err << " in itr " << it << endl;
				ratio = starting_err / actual_err;
				rotamount *= ratio;
				transamount *= ratio;
				it++;
			} while (it < 20 && (ratio < 0.9999f || ratio > 1.0001f)); // that is, until actual_err = starting_err

			xf = xform::trans(transamount * transdir) * xform::rot(rotamount, rotaxis);

			// Now perform ICP
			ICP_Stat stat;
			xform final_xf;
			stat.add_itr(starting_err, xf);
			matching.icp(xf, final_xf, stat);

			for (int i = 0; i <= NUM_FIRST_ITRS; i++) {
				errs[x][i] += stat.per_itr_err[i];
			}

			//printf("%7.2g ", actual_err); fflush(stdout);
			printf("%7.2g ", errs[x][1] / (trial+1)); fflush(stdout);
		}
		printf("\n");
	}

	ofstream ofs(output_filename);
	ofs.precision(12);
	for (auto &l: errs) {
		for (auto &v: l) {
			ofs << v / float(num_trial) << " ";
		}
		ofs << endl;
	}
	ofs.close();
}
