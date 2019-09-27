#include "TriMesh.h"
#include "KDtree.h"
#include "point.h"
#include "tetra_meas.h"
#include "pcr_adv.h"
#include "RTree.h"
#include "share.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <unordered_map>
#include <unordered_set>
using namespace std;
using namespace trimesh;

vector<Entry> cal_entries(PtwID q, Pt3D pts_q[], int m, double min, double max) {
    vector<Entry> ret;

    vector<PtwID> ann_pts;
    for (int i_s = 0; i_s < m; i_s++) {
        Pt3D s = pts_q[i_s];
        double d = eucl_dist(s, q.pt);
        if (d - min >= 0 && d - max <= 0) {
            ann_pts.push_back({ i_s, s });
        }
    }
    int hit_size = ann_pts.size();
    if (hit_size < 3) {
        return ret;
    }

    for (int i = 0; i < hit_size; i++) {
        for (int j = i + 1; j < hit_size; j++) {
            for (int k = j + 1; k < hit_size; k++) {
                auto ratio_set = get_ratio_set_vol(q.pt,
                    ann_pts[i].pt, ann_pts[j].pt, ann_pts[k].pt);
                Entry e;
                e.fail = false;
                e.repre = q;
                e.remai[0] = ann_pts[i];
                e.remai[1] = ann_pts[j];
                e.remai[2] = ann_pts[k];
                e.vol = ratio_set.volume;
                e.meas = ratio_set.ratio;
                sort_remai(e);
                ret.push_back(e);
            }
        }
    }
    return ret;
}

bool check_congr(Entry e, Entry f, double epsilon_m) {
    double e_edges[5], f_edges[5];
    e_edges[0] = eucl_dist(e.repre.pt, e.remai[0].pt);
    e_edges[1] = eucl_dist(e.repre.pt, e.remai[1].pt);
    e_edges[2] = eucl_dist(e.remai[0].pt, e.remai[1].pt);
    e_edges[3] = eucl_dist(e.repre.pt, e.remai[2].pt);
    e_edges[4] = eucl_dist(e.remai[0].pt, e.remai[2].pt);
    f_edges[0] = eucl_dist(f.repre.pt, f.remai[0].pt);
    f_edges[1] = eucl_dist(f.repre.pt, f.remai[1].pt);
    f_edges[2] = eucl_dist(f.remai[0].pt, f.remai[1].pt);
    f_edges[3] = eucl_dist(f.repre.pt, f.remai[2].pt);
    f_edges[4] = eucl_dist(f.remai[0].pt, f.remai[2].pt);
    for (int i = 0; i < 5; i++) {
        if (abs(e_edges[i] - f_edges[i]) > 2 * epsilon_m) {
            return false;
        }
    }
    return true;
}

vector<int> tree_search_return;

// TODO: improve performance by not using callback
bool tree_search_callback(int key) {
    // cout << key << ":\n";
    tree_search_return.push_back(key);
    return true;
}

vector<Entry_Pair> look_up_index(Entry e, double epsilon_m, RTree<int, double, 2>* tree,
    unordered_map<int, Entry>* entries_map) {

    vector<Entry_Pair> ret;

    double low[2], high[2];
    cal_range(e.repre.pt, e.remai[0].pt, e.remai[1].pt, e.remai[2].pt, epsilon_m,
        low[0], high[0], low[1], high[1]);

    int nhits = tree->Search(low, high, tree_search_callback);

    for (int &hit_key: tree_search_return) {
        Entry f = (*entries_map)[hit_key];
        if (check_congr(e, f, epsilon_m)) {
            Entry_Pair p(e, f);
            ret.push_back(p);
        }
    }
    tree_search_return.clear();

    return ret;
}

// tmp function
int look_up_index(int repre_id, unordered_map<int, Entry>* entries_map) {
    for (auto it = entries_map->begin(); it != entries_map->end(); it++) {
        if (it->second.repre.id == repre_id) {
            return it->first;
        }
    }
    return -1;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " database_filename index_filename query_filename [-test]" << endl;
        exit(1);
    }

    bool test_mode = false;
    if (argc >= 5 && string(argv[4]) == "-test") {
        test_mode = true;
    }

    srand(time(NULL));

    cout << "Reading dababase file" << endl;
    string database_filename = argv[1];
    TriMesh *mesh_p = TriMesh::read(database_filename);

    KDtree *kdtree_p = new KDtree(mesh_p->vertices);

    cout << "Loading index entries" << endl;
    string idx_filename = argv[2];
    RTree<int, double, 2> tree;
    tree.Load(idx_filename.c_str());

    string grid_filename = idx_filename + ".grid";
    ifstream grid_ifs(grid_filename);

    Grid g_db;
    double ann_min_db, ann_max_db;
    grid_ifs >> g_db.w >> ann_min_db >> ann_max_db >> g_db.cells_count;
    grid_ifs >> g_db.cells_box.min_id[0] >> g_db.cells_box.min_id[1] >> g_db.cells_box.min_id[2]
             >> g_db.cells_box.max_id[0] >> g_db.cells_box.max_id[1] >> g_db.cells_box.max_id[2];

    unordered_map<int, Entry> entries_map;
    for (int i = 0; i < g_db.cells_count; i++) {
        int key, x, y, z, list_size;
        grid_ifs >> key >> x >> y >> z >> list_size;
        
        Cell c(x, y, z);
        for (int j = 0; j < list_size; j++) {
            int pt_id; grid_ifs >> pt_id;
            c.add_pt(pt_id, pt(mesh_p->vertices[pt_id]));
        }
        g_db.cells_map[key] = c;

        int repre_id, remai_0_id, remai_1_id, remai_2_id;
        double vol, meas;
        bool fail;
        grid_ifs >> repre_id >> remai_0_id >> remai_1_id >> remai_2_id >> vol >> meas >> fail;

        Entry e;
        e.set(PtwID(repre_id, pt(mesh_p->vertices[repre_id])),
              PtwID(remai_0_id, pt(mesh_p->vertices[remai_0_id])),
              PtwID(remai_1_id, pt(mesh_p->vertices[remai_1_id])),
              PtwID(remai_2_id, pt(mesh_p->vertices[remai_2_id])),
              vol, meas);
        e.fail = fail;
        entries_map[key] = e;
    }

    cout << "Reading query file" << endl;
    string query_filename = argv[3];
    TriMesh *mesh_q = TriMesh::read(query_filename);

    string query_info_filename = query_filename + ".info";
    ifstream info_q_ifs(query_info_filename);

    cout << "Reading id maps" << endl;
    string idmap_filename = query_filename + ".map";
    ifstream idmap_ifs(idmap_filename);

    int m;
    info_q_ifs >> m;
    vector<Pt3D> pts_q;
    vector<int> idmap;
    for (int i = 0; i < m; i++) {
        pts_q.push_back(pt(mesh_q->vertices[i]));
        int db_mapping; idmap_ifs >> db_mapping;
        idmap.push_back(db_mapping);
    }

    double epsilon_m, eta;
    info_q_ifs >> epsilon_m >> eta;
    cout << endl;

    int total_num_verification = 0;
    double total_time = 0.0, proposal_time = 0.0, verification_time = 0.0;
    timer_start();

    double w_q = 3.464102 * g_db.w;
    cout << "Query voxelized of grid size " << w_q << endl;

    Grid g_q(w_q);
    g_q.gridify(mesh_q);

    cout << "Total number of query cells: " << g_q.cells_count << endl;

    // cout << "Displaying info of each query cell:" << endl;
    // for (auto it = g_q.cells_map.begin(); it != g_q.cells_map.end(); it++) {
    //     cout << it->second.x << " "
    //          << it->second.y << " "
    //          << it->second.z << " "
    //          << it->second.list.size() << endl;
    // }
    // cout << endl;

    // test verification
    if (test_mode) {
        cout << "Test verification..." << endl;
        int counter = 0, matched_cells_count = 0;
        unordered_set<int> tmp_idmap_set;
        tmp_idmap_set.insert(idmap.begin(), idmap.end());
        for (auto it = g_q.cells_map.begin(); it != g_q.cells_map.end(); it++) {
            cout << "For query cell #" << counter << endl;
            int matching_repre_point_count = 0;
            int matching_all_point_count = 0;
            for (PtwID &p: it->second.list) {
                int mapping = idmap[p.id];
                if (mapping >= 0) {
                    int entry_key = look_up_index(mapping, &entries_map);
                    if (entry_key >= 0) {
                        // cout << "Query pt #" << p.id << " matched with a representative pt #" << mapping << endl;
                        matching_repre_point_count++;
                        bool all_matching = true;
                        for (int i = 0; i < 3; i++) {
                            int remai_id = entries_map[entry_key].remai[i].id;
                            if (tmp_idmap_set.find(remai_id) == tmp_idmap_set.end()) {
                                // cout << "But remai[" << i << "] of db#" << remai_id << " not appearing in the query" << endl; 
                                all_matching = false;
                            }
                        }
                        if (all_matching) {
                            matching_all_point_count++;
                        }
                    }
                }
            }
            if (matching_all_point_count > 0) {
                matched_cells_count++;
            }
            cout << "Matching repre: " << matching_repre_point_count << endl;
            cout << "Matching all: " << matching_all_point_count << endl;
            counter++;
        }
        cout << "Summary: " << matched_cells_count << "/" << g_q.cells_count << " cells are matched" << endl;
    }

    cout << endl;

    int k_m = 10;
    int verified_size = 0;
    for (int t = 0; t < k_m; t++) {
        int selected_cell_id = rand() % g_q.cells_count;
        auto ptr = g_q.cells_map.begin();
        for (int i = 0; i < selected_cell_id; i++, ptr++) { }
        Cell selected_cell = ptr->second;
        cout << "Selecting cell #" << selected_cell_id
             << " with " << selected_cell.list.size() << " pts" << endl;

        vector<Entry_Pair> hit_list;
        for (int i = 0; i < selected_cell.list.size(); i++) {
            PtwID q = selected_cell.list[i];

            auto list = cal_entries(q, &pts_q[0], m, ann_min_db - epsilon_m, ann_max_db + epsilon_m);
            // cout << "Calculating entry list for query pt #" << q.id << endl;
            // cout << "Size of the entry list: " << list.size() << endl;

            for (Entry &e: list) {
                vector<Entry_Pair> tmp_list = look_up_index(e, epsilon_m, &tree, &entries_map);
                hit_list.insert(hit_list.end(), tmp_list.begin(), tmp_list.end());
            }
        }
        cout << "Returned hit size is " << hit_list.size() << endl;
        total_num_verification += hit_list.size();

        timer_start();
        verified_size = 0;
        for (auto &h: hit_list) {
            // cout << h.to_str() << endl;
            h.cal_xf();

            double result = cal_corr_err(mesh_q, kdtree_p, &h.xf, 50000);
            if (result > 0) {
                // cout << h.to_str() << endl
                //      << err << endl;
                verified_size++;
            }
        }
        verification_time += timer_end(SECOND);

        if (verified_size > 0) {
            break;
        }

    }

    total_time = timer_end(SECOND);
    proposal_time = total_time - verification_time;

    cout << endl;
    cout << "Total number of candidate transformations: " << total_num_verification << endl;
    cout << "Final number of valid transformations: " << verified_size << endl;
    cout << "Total time: " << total_time << "(s)" << endl;
    cout << "Proposal time: " << proposal_time << "(s)" << endl;
    cout << "Verification time: " << verification_time << "(s)" << endl;

}