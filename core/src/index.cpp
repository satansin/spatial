#include "pcr_adv.h"
#include "share.h"
#include <iostream>
using namespace std;

const int INDEX_DIM = 6;

int main(int argc, char **argv) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " database_path grid_filename index_filename" << endl;
        exit(1);
    }

    int argi = 0;
    string db_path = argv[(++argi)];
    string grid_filename = argv[(++argi)];
    string idx_filename = argv[(++argi)];

    cout << "Reading database files from " << db_path << endl;
    vector<TriMesh*> db_meshes;
    int num_meshes = read_db_mesh_batch(db_path, db_meshes);
    cout << "Total no. meshes: " << num_meshes << endl << endl;

    cout << "Loading DB structure from " << grid_filename << endl;
    Struct_DB s_db;
    s_db.read(grid_filename, db_meshes);

    int num_cells = s_db.get_total_cells_count();
    cout << "Total no. cells: " << num_cells << endl << endl;

    R_TYPE** tree_data = (R_TYPE**) malloc(sizeof(R_TYPE *) * num_cells);
    auto entries = s_db.get_entries();

    for (int i = 0; i < num_cells; i++) {

        // double box_min[2] = { prem_entry->vol, prem_entry->meas },
        //     box_max[2] = { prem_entry->vol, prem_entry->meas };
        // tree.Insert(box_min, box_max, key);
        // tree.Insert(prem_entry->sides, prem_entry->sides, global_cell_id);
        tree_data[i] = (R_TYPE *) malloc(sizeof(R_TYPE) * INDEX_DIM);
        for (int dim_i = 0; dim_i < INDEX_DIM; dim_i++) {
            tree_data[i][dim_i] = (int) (entries[i]->sides[dim_i] * 1e5);
        }

    }

    // RTree<int, double, 2> tree;
    // RTree<int, double, 6> tree; // edge index
    vector<rtree_info> idx_rtree_infos = {
		  { 5,	10,	INDEX_DIM,	7	}
		, { 10,	20,	INDEX_DIM,	15	}
		, { 15,	30,	INDEX_DIM,	22	}
		, { 20,	40,	INDEX_DIM,	30	}
		, { 25,	50,	INDEX_DIM,	37	}
		, { 30,	60,	INDEX_DIM,	45	}
	};

    node_type* tree;

    for (int i = 0; i < idx_rtree_infos.size(); i++) {
        if (i != 4)
            continue;
        
    	cout << "Building tree for info #" << i << endl;

	    timer_start();
	    build_tree(&tree, tree_data, num_cells, &idx_rtree_infos[i]);
	    cout << "Built the tree in " << timer_end(SECOND) << "(s)" << endl;

	    string outidx_filename = idx_filename + "." + to_string(i);
    	timer_start();
        // tree.Save(outidx_filename.c_str());
        save_rtree(tree, outidx_filename.c_str(), &idx_rtree_infos[i]);
        cout << "Tree saved to " << outidx_filename << " in " << timer_end(SECOND) << "(s)" << endl;

        free_tree(tree, &idx_rtree_infos[i]);
		cout << "Tree freed" << endl << endl;
    }

    for (int i = 0; i < num_cells; i++) {
    	free(tree_data[i]);
    }
    free(tree_data);

}