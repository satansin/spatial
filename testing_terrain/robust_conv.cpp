#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <cstdarg>
#include <unistd.h>

#include "TriMesh.h"
#include "TriMesh_algo.h"

#include "mathutil.h"
#include "matching.h"

using namespace std;
using namespace trimesh;

#define NTRIALS 100

#define MAX_ROTAT 60.0f
#define NUM_ROTAT 3
#define UNIT_ROTAT 20
#define MAX_TRANS 0.5f
#define NUM_TRANS 5
#define UNIT_TRANS 10

void invoke_system(const char *fmt, ...)
{
	char com[1000];
	va_list args;
	va_start(args, fmt);
	vsnprintf(com, sizeof(com), fmt, args);
	va_end(args);

	int rc = system(com);
	if (rc != 0) {
		cerr << "Error running: " << com << endl;
	}
}

int main(int argc, char* argv[])
{
	if (argc < 1) {
		cerr << "Usage: " << argv[0] << " [-fm(filename_model)=model.ply] [-fd(filename_data)=data.ply] [-vc(variant_corr)=0] [-va(variant_align)=0] [-ni(num_itr)="
			 << MAX_ITR << "] [-nt(num_trial)=" << NTRIALS << "] [-id(idx_data_mult)=0] /*[-fo(filename_output)=errs.dat]*/" << endl;
		exit(1);
	}

	string filename_model = "model.ply";
	string filename_data = "data.ply";

	int variant_corr = 0;
	int variant_align = 0;
	int num_itr = MAX_ITR;
	int num_trial = NTRIALS;

	int idx_data_mult = 0;
	// string output_filename = "errs.dat";

    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
        if (argv_str.rfind("-fm", 0) == 0)
            filename_model = string(argv[i] + 4);
        else if (argv_str.rfind("-fd", 0) == 0)
            filename_data = string(argv[i] + 4);

        else if (argv_str.rfind("-vc", 0) == 0)
            variant_corr = atoi(argv[i] + 4);
        else if (argv_str.rfind("-va", 0) == 0)
            variant_align = atoi(argv[i] + 4);

        else if (argv_str.rfind("-ni", 0) == 0)
            num_itr = atoi(argv[i] + 4);
        else if (argv_str.rfind("-nt", 0) == 0)
            num_trial = atoi(argv[i] + 4);

        else if (argv_str.rfind("-id", 0) == 0)
            idx_data_mult = atoi(argv[i] + 4);
    }

    if (idx_data_mult > 0) {
    	const string suffix = "_" + to_string(idx_data_mult) + filename_data.substr(filename_data.length() - 4, 4);
    	filename_data.replace(filename_data.length() - 4, 4, suffix);
    }

	cout << "----------------------------------------" << endl;
	cout << "Parameters" << endl;
	cout << "Model: " << filename_model << endl;
	cout << "Data: " << filename_data << endl;
	cout << "Variant Correspondence: " << variant_corr << endl;
	cout << "Variant Alignment: " << variant_align << endl;
	cout << "Num of Iterations: " << num_itr << endl;
	cout << "Num of Trials: " << num_trial << endl;
	cout << "Index of Multi Data: " << idx_data_mult << endl;
	cout << "----------------------------------------" << endl;

	cout.precision(15);

	// srand(time(NULL));
	srand(934746273); // use fixed srand to ensure every run has the same initial values

	ICP_Matching matching(filename_model, filename_data); //, variant_corr, variant_align);
	matching.load();
	cout << endl;

	// pre-generated initial rot-axis and trans-dir for each trial
	vec rotaxis_list[num_trial];
	vec transdir_list[num_trial];
	for (int i = 0; i < num_trial; i++) {
		rotaxis_list[i] = rnd2sph(ran_f_0_1(), ran_f_0_1());
		transdir_list[i] = rnd2sph(ran_f_0_1(), ran_f_0_1());
	}

	const float max_angle = radians(MAX_ROTAT);
	const float max_trans = MAX_TRANS * (matching.mesh_model->bsphere.r + matching.mesh_data->bsphere.r);
	for (int r = 0; r <= NUM_ROTAT; r++) {
		for (int t = 0; t <= NUM_TRANS; t++) {
			if (r == 0 && t == 0) {
				continue; // skip the 0, 0 case
			}

			const int r_int = r * UNIT_ROTAT, t_int = t * UNIT_TRANS;
			cout << "Angle " << r_int << " deg, trans " << t_int << "%" << endl;

			const string rt_folder = "r" + to_string(r_int) + "_t" + to_string(t_int);
			invoke_system("mkdir -p %s", rt_folder.c_str());
			chdir(rt_folder.c_str());

			const float this_angle = (float(r) / float(NUM_ROTAT)) * max_angle;
			const float this_trans = (float(t) / float(NUM_TRANS)) * max_trans;

			double sum_best_err = 0.0;
			double best_err_list[num_trial];

			for (int trial = 0; trial < num_trial; trial++) {
				cout << "Trial #" << (trial + 1) << endl;

				invoke_system("mkdir -p %d", trial + 1);
				chdir(to_string(trial + 1).c_str());

				const xform rot = xform::rot(this_angle, rotaxis_list[trial]);
				const vec trans = transdir_list[trial] * this_trans;
				// xform init_xf = xform::trans(trans + rotcenter) * rot * xform::trans(-rotcenter);
				const xform xf = xform::trans(trans) * rot;

				const double init_err = cal_err(matching.mesh_data, xf);

				cout << "Initial err: " << init_err << endl;

				// Now perform ICP
				ICP_Stat stat;
				xform final_xf;
				stat.add_itr(init_err, xf);
				matching.icp(xf, num_itr, variant_corr, variant_align, final_xf, stat);

				cout << "Aligned err: " << cal_err(matching.mesh_data, final_xf) << endl;

				ofstream ofs_errs("errs" + (idx_data_mult > 0 ? ("_" + to_string(idx_data_mult)) : "") + ".dat");
				ofs_errs.precision(12);
				for (int i = 0; i < stat.per_itr_err.size(); i++) {
					ofs_errs << i << " " << stat.per_itr_err[i] << endl;
				}
				ofs_errs.close();

				ofstream ofs_best("best" + (idx_data_mult > 0 ? ("_" + to_string(idx_data_mult)) : "") + ".dat");
				ofs_best.precision(12);
				ofs_best << stat.best_err << endl;
				ofs_best << stat.per_itr_xf[stat.best_itr] << endl;
				ofs_best.close();

				ofstream ofs_init("init" + (idx_data_mult > 0 ? ("_" + to_string(idx_data_mult)) : "") + ".dat");
				ofs_init.precision(12);
				ofs_init << stat.per_itr_err[0] << endl;
				ofs_init << stat.per_itr_xf[0] << endl;
				ofs_init.close();

				sum_best_err += stat.best_err;
				best_err_list[trial] = stat.best_err;

				chdir("..");
			}

			const double avg = sum_best_err / double(num_trial);
			const double med = (num_trial % 2 == 0) ? 0.5 * (best_err_list[num_trial / 2] + best_err_list[num_trial / 2 - 1]) : best_err_list[num_trial / 2];

			ofstream ofs_stat("stat" + (idx_data_mult > 0 ? ("_" + to_string(idx_data_mult)) : "") + ".dat");
			ofs_stat.precision(12);
			ofs_stat << avg << endl;
			ofs_stat << med << endl;

			chdir("..");
		}
	}
}
