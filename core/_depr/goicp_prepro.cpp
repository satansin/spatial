#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>

#include "mesh.h"

using namespace std;

int main(int argc, char** argv) {

    const int num_resol = 4;

    const string db_paths[num_resol] = {
        "/rwproject/kdd-db/hliubs/10_data_spatial/3dor/db/indoor_scans/comp_1",
        "/rwproject/kdd-db/hliubs/10_data_spatial/3dor/db/indoor_scans/comp_5",
        "/rwproject/kdd-db/hliubs/10_data_spatial/3dor/db/indoor_scans/comp_6",
        "/rwproject/kdd-db/hliubs/10_data_spatial/3dor/db/indoor_scans/comp_7"
    };
    const string query_paths[num_resol] = {
        "/rwproject/kdd-db/hliubs/10_data_spatial/3dor/query/indoor_scans_pro_noise/comp_1",
        "/rwproject/kdd-db/hliubs/10_data_spatial/3dor/query/indoor_scans_pro_noise/comp_5",
        "/rwproject/kdd-db/hliubs/10_data_spatial/3dor/query/indoor_scans_pro_noise/comp_6",
        "/rwproject/kdd-db/hliubs/10_data_spatial/3dor/query/indoor_scans_pro_noise/comp_7"
    };

    // double global_scales[num_resol];

    for (int rs = 0; rs < num_resol; rs++) {

    	const string db_path = db_paths[rs];
        const string query_path = query_paths[rs];

        cout << "Reading database files from " << db_path << endl;
        DB_Meshes db_meshes;
        const int num_meshes = db_meshes.read_from_path(db_path);
        cout << "Total no. meshes: " << num_meshes << endl << endl;

        // double max_scale_over_db = -1.0;
        // for (int i = 0; i < db_meshes.size(); i++) {
        //     auto mesh_p = db_meshes.get_mesh(i);
        //     const double max_scale = mesh_p->box_max_scale();
        //     if (max_scale_over_db < max_scale) {
        //         max_scale_over_db = max_scale;
        //     }
        // }

        // const double global_scale = 0.999 / max_scale_over_db;
        // global_scales[rs] = global_scale;

        const string db_txt_folder = db_path + "_txt/";

        const string cmd_mkdir_db = "mkdir -p " + db_txt_folder;
        system(cmd_mkdir_db.c_str());

        for (int i = 0; i < db_meshes.size(); i++) {
            auto mesh_p = db_meshes.get_mesh(i);

            const string db_output = db_txt_folder + "db_" + to_string(i) + "_o.txt";
            ofstream db_ofs(db_output);
            const int n = mesh_p->size();
            db_ofs << n << endl;

            // mesh_p->scale_by(global_scale);

            for (int j = 0; j < mesh_p->size(); j++) {
                auto pt = mesh_p->get_pt(j);
                db_ofs << pt.x << " " << pt.y << " " << pt.z << endl;
            }

            db_ofs.close();
        }

        const string query_txt_folder = query_path + "_txt/";

        const string cmd_mkdir_query = "mkdir -p " + query_txt_folder;
        system(cmd_mkdir_query.c_str());

        for (int id = 1; id <= 8; id++) {
            for (int nl = 0; nl < 6; nl++) {
                const string query_ply = query_path + "/q_0" + to_string(id) + "." + to_string(nl) + ".ply";

                cout << "Reading query mesh from " << query_ply << endl;
                Mesh mesh_q;
                mesh_q.read_from_path(query_ply);
                cout << endl;

                const string query_txt = query_txt_folder + "/q_0" + to_string(id) + "_o." + to_string(nl) + ".txt";
                ofstream query_ofs(query_txt);
                const int m = mesh_q.size();
                query_ofs << m << endl;

                // mesh_q.scale_by(global_scale);

                vector<Pt3D> query_pts;
                for (int i = 0; i < mesh_q.size(); i++) {
                    auto pt = mesh_q.get_pt(i);
                    query_pts.push_back(pt);
                }

                random_shuffle(query_pts.begin(), query_pts.end());

                for (auto &pt: query_pts) {
                    query_ofs << pt.x << " " << pt.y << " " << pt.z << endl;
                }

                query_ofs.close();
            }
        }

    }

    // for (int i = 0; i < num_resol; i++) {
    //     cout << "Global Scale for #" << i << ": " << global_scales[i] << endl;
    // }

}