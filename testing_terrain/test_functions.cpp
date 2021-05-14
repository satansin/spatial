#include <iostream>
#include <fstream>
#include <string>

#include "TriMesh.h"

#include "util.h"
#include "mathutil.h"
#include "matching.h"

using namespace std;
using namespace trimesh;

void print_ground_truth_dist(ICP_Matching& matching)
{
	// cout << "Ground-truth corr dists:" << endl;
	// for (int vc = 0; vc < 5; vc++) {
	// 	 for (int va = 0; va < 3; va++) {
	// 		cout << matching.get_corr_dist(xform(), vc, va) << " ";
	// 	 }
	// 	 cout << endl;
	// }

	cout << "GT corr dists (4 0): " << matching.get_corr_dist(xform(), 4, 0) << endl;
}

void one_convergence_test(ICP_Matching& matching)
{
	// auto init_xf = to_xform_matrix(M_PI / 20.0, M_PI / 20.0, 0, 0.03, -0.02, 0);
	// auto init_xf = to_xform_matrix(M_PI / 10.0, M_PI / 10.0, 0, 0.1, -0.2, 0);

	// pay attention!!! here you need to transpose what is written in file

	const xform init_xf(
		0.944652619162, -0.232817533055, -0.231143733232, 0,
		0.208312625717, 0.969959240411, -0.125638059152, 0,
		0.253450742901, 0.0705341636568, 0.964773368559, 0,
		-0.0569876097143, -0.294359892607, -0.0578547753394, 1
	); // bun4_bun3_4_0

	// const xform init_xf(
	// 	0.944652619162, -0.232817533055, -0.231143733232, 0,
	// 	0.208312625717, 0.969959240411, -0.125638059152, 0,
	// 	0.253450742901, 0.0705341636568, 0.964773368559, 0,
	// 	-0.0452293679118, -0.23362468183, -0.0459176115692, 1
	// ); // ellipsoid4_ellipsoid3_4_0

	const double init_err = cal_err(matching.mesh_data, init_xf);
	cout << "Initl err: " << init_err << endl;

	write_mesh(matching.mesh_data, "ply_ascii:groundt_data.ply", xform());
	write_mesh(matching.mesh_model, "ply_ascii:groundt_model.ply", xform());
	write_mesh(matching.mesh_data, "ply_ascii:initial_data.ply", init_xf);

	ICP_Stat stat;
	xform final_xf;
	stat.add_itr(init_err, init_xf);

	const int n_itr = 100;
	const int vc = 0;
	const int va = 0;

	timer_start();
	matching.icp(init_xf, n_itr, vc, va, final_xf, stat, true);
	cout << "ICP finished in " << timer_end(SECOND) << " (s)" << endl;

	// cout << "Final dist: " << cal_dist(mesh_data, mesh_model, kd_model) << endl;
	cout << "Final err: " << cal_err(matching.mesh_data, final_xf) << endl;

	write_mesh(matching.mesh_data, "ply_ascii:aligned_data_" + to_string(vc) + "_" + to_string(va) + ".ply", final_xf);

	// ofstream ofs("err_" + to_string(matching.variant_corr) + "_" + to_string(matching.variant_align) + ".txt");
	// for (int i = 0; i < stat.per_itr_err.size(); i++) {
	// 	ofs << i << " " << stat.per_itr_err[i] << endl;
	// }
	// ofs.close();
}

void functions_test()
{
	// test_inverse();

	cout << "-------------------------------" << endl;
	cout << "Test function get_proj_p2f" << endl;
	cout << "-------------------------------" << endl;
	test_get_proj_p2f();

	cout << "-------------------------------" << endl;
	cout << "Test function sq_dist_p2l" << endl;
	cout << "-------------------------------" << endl;
	test_sq_dist_p2l();

	cout << "-------------------------------" << endl;
	cout << "Test function sq_dist_p2f" << endl;
	cout << "-------------------------------" << endl;
	test_sq_dist_p2f();

	cout << "-------------------------------" << endl;
	cout << "Test function sq_dist_e2e" << endl;
	cout << "-------------------------------" << endl;
	test_sq_dist_e2e();

	cout << "-------------------------------" << endl;
	cout << "Test function inters_line_tri" << endl;
	cout << "-------------------------------" << endl;
	test_inters_line_tri();

	cout << "-------------------------------" << endl;
	cout << "Test function nn_triangle" << endl;
	cout << "-------------------------------" << endl;
	test_nn_triangle();
}

void toy_test()
{
	auto mesh_toy = TriMesh::read("toy.ply");
	mesh_toy->need_faces();

	apply_xform(mesh_toy, to_xform_matrix(M_PI / 4.0, M_PI / 4.0, 0, 0, 0, 0));

	mesh_toy->write("ply_ascii:toy_rot.ply");
}

int main()
{
	srand(time(NULL));
	cout.precision(15);

	// functions_test();

	// ICP_Matching matching("data/ellipsoid4.ply", "data/ellipsoid3.ply");
	ICP_Matching matching("data/bun_zipper_res4.ply", "data/bun_zipper_res3.ply");
	matching.load();

	// print_ground_truth_dist(matching);

	one_convergence_test(matching);
}