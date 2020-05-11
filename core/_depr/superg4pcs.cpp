#include "point.h"
#include "trans.h"
#include "util.h"
#include "pcr_basic.h"
#include "RTree.h"
#include "mesh.h"
#include "c_rtree.h"
#include "ProgressBar.hpp"
#include "struct_q.h"
#include "goicp.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <set>

using namespace std;
using namespace trimesh;

bool iter(DB_Meshes* db_meshes, vector<C_RTree>& db_rtrees, Mesh* mesh_q, Struct_Q* s_q, double delta, double epsilon, GoICP goicp[], Exec_stat& stat) {

    timer_start();

    int ran[4];
    double l, d;
    double la, mu;
    Pt3D e, f;
    double angle; // cos(theta)
    Pt3D bq[4];

    vector<Congr_base> ret;

    double iter_prop_time = 0.0, iter_veri_time = 0.0;
    bool iter_found_one = false;
    double iter_first_time = 0.0;

    vector<Congr_base*> ret_left_icp_only, ret_left_goicp;

    int iter_num_verified = 0;

    if (!test_3d_coplanar(&mesh_q, &query_rtree, &s_q, ran, d, l, la, mu, e, f, angle)) {
        timer_end(SECOND);
        return false;
    }

    // testing
    // ran1 = 61;
    // ran2 = 25;
    // ran3 = 40;
    // ran4 = 60;
    // cal_intersection(&mesh_q, ran1, ran2, ran3, ran4, la, mu, e, angle);
    // cout << "la: " << la << endl;
    // cout << "mu: " << mu << endl;
    // cout << "angle: " << angle << endl;

    for (int i = 0; i < 4; i++) {
        bq[i] = mesh_q.get_pt(ran[i]);
    }

    for (int i = 0; i < db_meshes.size(); i++) {
        match_base_in_db_mesh(db_meshes.get_mesh(i), &db_rtrees[i], d, l, epsilon, la, mu, angle, i, ret);
    }

    iter_prop_time = timer_end(SECOND);

    timer_start();
    timer_start();

    // Step 1: calculate transformation, initial distance, and leave those for step 2&3
    for (auto &r: ret) {
        Pt3D bp[4];
        for (int j = 0; j < 4; j++) {
            bp[j] = db_meshes->get_mesh(r.db_id)->get_pt(r.bp[j]);
        }
        r.xf = cal_trans(bq, bp, 4);
        r.init_dist = db_meshes->cal_corr_err(mesh_q, r.db_id, &r.xf); // TODO: use another unimplemented interface
        // cout << "Initial distance: " << r.init_dist << endl;

        if (r.init_dist <= sq(delta)) {
            iter_num_verified++;
            if (iter_num_verified == 1 && !iter_found_one) {
                iter_first_time = timer_end(SECOND);
                iter_found_one = true;
            }
        } else {
            ret_left_icp_only.push_back(&r);
        }
    }

    // Step 2: perform the ICP-only check
    for (auto &r: ret_left_icp_only) {
        double updated_err = goicp[r->db_id].ICP(&r->xf); // ICP-only
        // cout << "Updated distance with ICP-only: " << updated_err << endl;

        if (updated_err <= sq(delta)) {
            iter_num_verified++;
            if (iter_num_verified == 1 && !iter_found_one) {
                iter_first_time = timer_end(SECOND);
                iter_found_one = true;
            }
        } else {
            ret_left_goicp.push_back(r);
        }
    }

    // // Step 3: perform GoICP
    // for (auto &r: ret_left_goicp) {
    //  goicp[r->db_id].fly_init();
    //  goicp[r->db_id].OuterBnB(); // TODO: return false?

    //  // TODO
    // }

    if (!iter_found_one) {
        iter_first_time = timer_end(SECOND);
    }

    iter_veri_time += timer_end(SECOND);

    stat.prop_time += iter_prop_time;
    stat.veri_time += iter_veri_time;
    stat.user_time += (iter_prop_time + iter_veri_time);
    stat.first_verified_time += (iter_prop_time + iter_first_time);
    stat.num_verified = iter_num_verified;

    cout << "# verified: " << stat.num_verified << endl;

    stat.veri_size += ret.size();
    stat.num_icp_only += ret_left_icp_only.size();
    stat.num_goicp += ret_left_goicp.size();

    if (iter_num_verified > 0)
        return true;
    else
        return false;
}

bool exec(DB_Meshes* db_meshes, vector<C_RTree>& db_rtrees, Mesh* mesh_q, Struct_Q* s_q, double delta, double epsilon, GoICP goicp[], Exec_stat& stat) {

    bool find_accept = false;

#ifdef DET
    const int k_m = 1;
#else
    const int k_m = 9;
#endif

    for (int t = 0; t < k_m; t++) {

        find_accept = iter(db_meshes, db_rtrees, mesh_q, s_q, delta, epsilon, goicp, stat);

        if (find_accept)
            break;
    }

    return find_accept;
}

int main(int argc, char **argv) {

#ifdef DET
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " database_path query_filename delta [-stat=...]" << endl;
        exit(1);
    }
#else
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " database_path query_filename delta epsilon [-stat=...]" << endl;
        exit(1);
    }
#endif

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
    string query_filename = argv[(++argi)];

    double delta = atof(argv[(++argi)]);

#ifdef DET
    double epsilon = delta;
#else
    double epsilon = atof(argv[(++argi)]);
#endif

    cout << "delta: " << delta << endl;
    cout << "epsilon: " << epsilon << endl;
    cout << endl;

    srand(time(NULL));


    cout << "Reading database files from " << db_path << endl;
    DB_Meshes db_meshes;
    int num_meshes = db_meshes.read_from_path(db_path);
    cout << "Total no. meshes: " << num_meshes << endl << endl;

    db_meshes.build_kd();

    cout << "Reading database R-trees..." << endl;
    vector<C_RTree> db_rtrees;
    read_rtrees_from_db_meshes(&db_meshes, db_rtrees);
    
    cout << endl;

    cout << "Reading query mesh from " << query_filename << endl;
    Mesh mesh_q;
    mesh_q.read_from_path(query_filename);
    cout << endl;

    // load the query R-tree
    C_RTree query_rtree;
    query_rtree.read_from_mesh(query_filename);

    // load the query structure
    Struct_Q s_q;
    if (!s_q.read(query_filename + ".info")) {
        cout << "Error loading query structure" << endl;
        exit(1);
    }

    GoICP goicp[num_meshes];
    for (int i = 0; i < num_meshes; i++) {
        loadGoICP(db_meshes.get_mesh(i), &mesh_q, delta, goicp[i]);

        // Build Distance Transform
        cout << endl << "Building Distance Transform of #" << i << "..." << endl;
        goicp[i].BuildDT();

        goicp[i].Initialize();
    }
    cout << endl;


    const int exec_times = 3;
    double aggr_prop_time = 0, aggr_veri_time = 0, aggr_user_time = 0, aggr_first_verified_time = 0;
    int aggr_veri_size = 0;
    int aggr_num_fail = 0;

    for (int exec_i = 0; exec_i < exec_times; exec_i++) {
        cout << "Execution #" << exec_i << ": " << endl << endl;
        Exec_stat stat = (const struct Exec_stat){ 0 };
        bool exec_success = exec(&db_meshes, db_rtrees, &mesh_q, &s_q, delta, epsilon, goicp, stat);
        cout << endl;

        cout << "Total number of candidate transformations: " << stat.veri_size << endl;
        cout << "Final number of valid transformations: " << stat.num_verified << endl;
        cout << "Total time: " << stat.user_time << "(s)" << endl;
        cout << "Proposal time: " << stat.prop_time << "(s)" << endl;
        cout << "Verification time: " << stat.veri_time << "(s)" << endl;
        cout << "First verified time: " << stat.first_verified_time << "(s)" << endl;

        aggr_prop_time += stat.prop_time;
        aggr_veri_time += stat.veri_time;
        aggr_user_time += stat.user_time;
        aggr_veri_size += stat.veri_size;
        aggr_first_verified_time += stat.first_verified_time;

        if (!exec_success)
            aggr_num_fail++;
    }

    double aver_prop_time = aggr_prop_time / (double) exec_times;
    double aver_veri_time = aggr_veri_time / (double) exec_times;
    double aver_user_time = aggr_user_time / (double) exec_times;
    double aver_veri_size = (double) aggr_veri_size / (double) exec_times;
    double aver_first_verified_time = aggr_first_verified_time / (double) exec_times;

    if (write_stat) {
        ofstream stat_ofs;
        stat_ofs.open(stat_filename, ofstream::out | ofstream::app);
        stat_ofs << aver_prop_time << "\t";
        stat_ofs << aver_veri_time << "\t";
        stat_ofs << aver_user_time << "\t";
        stat_ofs << aver_veri_size << "\t";
        stat_ofs << aver_first_verified_time << "\t";
        stat_ofs.close();
    }

    cout << endl;
    cout << "Average proposal time: " << aver_prop_time << endl;
    cout << "Average verification time: " << aver_veri_time << endl;
    cout << "Average user time: " << aver_user_time << endl;
    cout << "Average # candidate transformations: " << aver_veri_size << endl;
    cout << "Average find first time: " << aver_first_verified_time << endl;
    cout << "Failed exec: " << aggr_num_fail << endl;

}
