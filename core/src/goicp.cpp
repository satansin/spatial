#include <string>
#include <iostream>

#include "point.h"
#include "struct_q.h"
#include "util.h"

#include "goicp.h"

using namespace std;

int main(int argc, char **argv) {

    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " database_path query_filename delta [-stat=...]" << endl;
        exit(1);
    }

    bool write_stat = false;
    string stat_filename = "";
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
        if (argv_str.rfind("-stat", 0) == 0) {
            write_stat = true;
            stat_filename = string(argv[i] + 6);
        }
    }

    int argi = 0;
    string db_path = argv[(++argi)];
    cout << "Input db_path: " << db_path << endl;
    string query_filename = argv[(++argi)];
    cout << "Input query_filename: " << query_filename << endl;

    double delta = atof(argv[(++argi)]);
    cout << "Input delta: " << delta << endl;


    cout << "Reading database files from " << db_path << endl;
    DB_Meshes db_meshes;
    int num_meshes = db_meshes.read_from_path(db_path);
    cout << "Total no. meshes: " << num_meshes << endl << endl;

    db_meshes.build_kd();

    cout << "Reading query mesh from " << query_filename << endl;
    Mesh mesh_q;
    mesh_q.read_from_path(query_filename);
    cout << endl;

    // load the query structure
    Struct_Q s_q;
    s_q.read(query_filename + ".info");

    cout << "delta: " << delta << endl << endl;

    GoICP goicp;
    loadGoICP(db_meshes.get_mesh(0), &mesh_q, delta, goicp);
    cout << endl;
    goicp.printParams();

    // Build Distance Transform
	cout << endl << "Building Distance Transform..." << endl;
	goicp.BuildDT();

    goicp.Initialize();

    timer_start();
	goicp.OuterBnB();
	double query_time = timer_end(SECOND);

	Trans trans_opt = {
		goicp.optR.val[0][0], goicp.optR.val[0][1], goicp.optR.val[0][2],
		goicp.optR.val[1][0], goicp.optR.val[1][1], goicp.optR.val[1][2],
		goicp.optR.val[2][0], goicp.optR.val[2][1], goicp.optR.val[2][2],
		goicp.optT.val[0][0], goicp.optT.val[1][0], goicp.optT.val[2][0]
	};
	unordered_set<int> retrieved_corr;
	db_meshes.retrieve(&mesh_q, 0, retrieved_corr, &trans_opt);
	double corr_err = sqrt(db_meshes.cal_corr_err(&mesh_q, 0, &trans_opt));
	double ovl_rate = s_q.overlapping_rate(retrieved_corr);

    if (write_stat) {
        ofstream stat_ofs;
        stat_ofs.open(stat_filename, ofstream::out | ofstream::app);
        stat_ofs << query_time << "\t";
        stat_ofs << corr_err << "\t";
        stat_ofs << ovl_rate << "\t";
        stat_ofs.close();
    }

	cout << "Query time: " << query_time << endl;
	cout << "Corr distance: " << corr_err << endl;
	cout << "Overlapping rate: " << ovl_rate << endl;

	goicp.Clear();
}