#include <string>
#include <iostream>

#include "point.h"
#include "struct_q.h"
#include "util.h"

#include "goicp.h"

using namespace std;

int main(int argc, char **argv) {

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " database_path query_filename delta [-stat=...] [-cheat=...]*" << endl;
        exit(1);
    }

    bool write_stat = false;
    string stat_filename;
    unordered_set<int> cheat_set;
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
        if (argv_str.rfind("-stat", 0) == 0) {
            write_stat = true;
            stat_filename = string(argv[i] + 6);
        // } else if (argv_str.rfind("-delta", 0) == 0) {
            // delta = atof(argv[i] + 7);
            // spec_delta = true;
        } else if (argv_str.rfind("-cheat", 0) == 0) {
        	cheat_set.insert(atoi(argv[i] + 7));
        }
    }

    int argi = 0;
    string db_path = argv[(++argi)];
    string query_filename = argv[(++argi)];
    double delta = atof(argv[(++argi)]);

    cout << "Input db_path: " << db_path << endl;
    cout << "Input query_filename: " << query_filename << endl;
    cout << "Input delta: " << delta << endl;
    cout << endl;

    cout << "Reading database files from " << db_path << endl;
    DB_Meshes db_meshes;
    int num_meshes = db_meshes.read_from_path(db_path);
    if (num_meshes < 0) {
        cerr << "Error reading database files" << endl;
        exit(1);
    }
    cout << "Total no. meshes: " << num_meshes << endl << endl;

    cout << "Building KDtrees for DB meshes" << endl;
    db_meshes.build_kd();

    cout << "Reading query mesh from " << query_filename << endl;
    Mesh mesh_q;
    mesh_q.read_from_path(query_filename);
    cout << endl;

    // load the query structure
    Struct_Q s_q;
    if (!s_q.read(query_filename + ".info")) {
        cout << "Error loading query structure" << endl;
        exit(1);
    }

    double q_diam = mesh_q.get_bsphere_d();
    delta *= q_diam;
    cout << "Diameter of query mesh: " << q_diam << ", thus delta is set to " << delta << endl;

    double sse = delta * (double) mesh_q.size(); // input delta is actually MSE
    cout << "Final SSE by number of query: " << sse << endl;
	
	cout << endl;


    double user_time = 0.0;
    int num_accepted = 0;

    GoICP* goicp = new GoICP[num_meshes];

    for (int i = 0; i < num_meshes; i++) {
    	// if (i != 984 && i != 985) {
    	// 	continue;
    	// }
        if (i != s_q.get_db_mesh_id()) { // trick for indoor DB
            continue;
        }

    	cout << "For mesh #" << (i + 1) << endl;

    	bool verbose = true;
    	bool use_dt = true;
    	int dt_size = 200; // or 50 for fast cheat?
    	if (cheat_set.find(i) != cheat_set.end()) {
    		dt_size = 200;
    	}
		loadGoICP(&db_meshes, i, &mesh_q, sse, delta, &goicp[i], verbose, use_dt, dt_size);

		// goicp[i].printParams();
		// cout << endl;

		goicp[i].Initialize();

		timer_start();
		double dist = goicp[i].OuterBnB();
		double query_time = timer_end(SECOND);
		if (dist <= sse) {
			cout << "Accepted in " << query_time << "(s)" << endl;
            num_accepted++;
		} else {
			cout << "Rejected in " << query_time << "(s)" << endl;
		}

		cout << endl;

		user_time += query_time;

		goicp[i].Clear();
    }

	// Trans trans_opt = {
	// 	goicp.optR.val[0][0], goicp.optR.val[0][1], goicp.optR.val[0][2],
	// 	goicp.optR.val[1][0], goicp.optR.val[1][1], goicp.optR.val[1][2],
	// 	goicp.optR.val[2][0], goicp.optR.val[2][1], goicp.optR.val[2][2],
	// 	goicp.optT.val[0][0], goicp.optT.val[1][0], goicp.optT.val[2][0]
	// };
	// unordered_set<int> retrieved_corr;
	// db_meshes.retrieve(&mesh_q, 0, retrieved_corr, &trans_opt);
	// double corr_err = sqrt(db_meshes.cal_corr_err(&mesh_q, 0, &trans_opt));
	// double ovl_rate = s_q.overlapping_rate(retrieved_corr);

	cout << "Total user time: " << user_time << endl;
    cout << "# accepted: " << num_accepted << endl;

    int num_fail = (num_accepted > 0) ? 0 : 1;

    if (write_stat) {
        ofstream stat_ofs(stat_filename);
        stat_ofs << "user_time=" << user_time << endl;
        stat_ofs << "num_fail=" << num_fail << endl;
        // stat_ofs << corr_err << "\t";
        // stat_ofs << ovl_rate << "\t";
        stat_ofs.close();
    }

	// cout << "Corr distance: " << corr_err << endl;
	// cout << "Overlapping rate: " << ovl_rate << endl;

	delete[] goicp;
}