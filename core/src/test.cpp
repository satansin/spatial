#include <iostream>
#include <fstream>

#include "mesh.h"

using namespace std;

int main() {

	string db_path = "/rwproject/kdd-db/hliubs/10_data_spatial/3dor/db/indoor_scans/comp_6";
    cout << "Reading database files from " << db_path << endl;
    DB_Meshes db_meshes;
    int num_meshes = db_meshes.read_from_path(db_path);
    cout << "Total no. meshes: " << num_meshes << endl << endl;


    string query_filename = "/rwproject/kdd-db/hliubs/10_data_spatial/3dor/query/indoor_scans/comp_6/q_01/q_01.0.ply";
    cout << "Reading query mesh from " << query_filename << endl;
    Mesh mesh_q;
    mesh_q.read_from_path(query_filename);
    cout << endl;

    auto mesh_p = db_meshes.get_mesh(0);

    double global_scale;

    string model_output = "test_model.txt";
    ofstream ofs_model(model_output);
    int n = mesh_p->size();
    ofs_model << n << endl;

    global_scale = 1.0 / mesh_p->box_max_scale();
    mesh_p->scale_by(global_scale);
    for (int i = 0; i < n; i++) {
    	auto pt = mesh_p->get_pt(i);
    	ofs_model << pt.x << " " << pt.y << " " << pt.z << endl;
    }

    ofs_model.close();

    string data_output = "test_data.txt";
    ofstream ofs_data(data_output);
    int m = mesh_q.size();
    ofs_data << m << endl;

    mesh_q.scale_by(global_scale);
    for (int i = 0; i < mesh_q.size(); i++) {
    	auto pt = mesh_q.get_pt(i);
    	ofs_data << pt.x << " " << pt.y << " " << pt.z << endl;
    }

    ofs_data.close();
}