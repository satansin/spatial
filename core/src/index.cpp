#include "pcr_adv.h"
#include "share.h"
#include "RTree.h"
#include "ProgressBar.hpp"
#include <iostream>
using namespace std;

int main(int argc, char **argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " database_path grid_filename" << endl;
        exit(1);
    }

    int argi = 0;
    string db_path = argv[(++argi)];
    string grid_filename = argv[(++argi)];
    // string idx_filename = argv[(++argi)];
    string idx_filename = grid_filename + ".idx";

    cout << "Reading database files from " << db_path << endl;
    vector<TriMesh*> db_meshes;
    int num_meshes = read_db_mesh_batch(db_path, db_meshes);
    cout << "Total no. meshes: " << num_meshes << endl << endl;

    cout << "Loading DB structure from " << grid_filename << endl;
    Struct_DB s_db;
    s_db.read(grid_filename, db_meshes);

    int num_cells = s_db.get_total_cells_count();
    cout << "Total no. cells: " << num_cells << endl << endl;

    auto entries = s_db.get_entries();

    ////////////////////////// Toggle_1 /////////////////////////////////////
    IndexTree tree;
    ProgressBar bar(num_cells, 70);
    ////////////////////////// Toggle_1 /////////////////////////////////////

    ////////////////////////// Toggle_2 /////////////////////////////////////
    // R_TYPE** tree_data = (R_TYPE**) malloc(sizeof(R_TYPE *) * num_cells);
    ////////////////////////// Toggle_2 /////////////////////////////////////

    for (int i = 0; i < num_cells; i++) {

        auto e = entries[i];

        ////////////////////////// Toggle_1 /////////////////////////////////////
        ++bar;
        bar.display();

        // use side length as index keys
        int box_min[INDEX_DIM], box_max[INDEX_DIM];
        for (int j = 0; j < INDEX_DIM; j++) {
            box_min[j] = (int) (e->sides[j] * 1e5);
            box_max[j] = (int) (e->sides[j] * 1e5);
        }

        // // use vol and meas as index keys
        // double box_min[INDEX_DIM], box_max[INDEX_DIM];
        // box_min[0] = e->vol;
        // box_max[0] = e->vol;
        // box_min[1] = e->meas;
        // box_max[1] = e->meas;
        
        tree.Insert(box_min, box_max, i);
        ////////////////////////// Toggle_1 /////////////////////////////////////

        ////////////////////////// Toggle_2 /////////////////////////////////////
        // tree_data[i] = (R_TYPE *) malloc(sizeof(R_TYPE) * INDEX_DIM);
        // for (int dim_i = 0; dim_i < INDEX_DIM; dim_i++) {
        //     tree_data[i][dim_i] = (int) (e->sides[dim_i] * 1e5);
        // }
        ////////////////////////// Toggle_2 /////////////////////////////////////

    }

    ////////////////////////// Toggle_1 /////////////////////////////////////
    bar.done();

    tree.SortDim0();

    string outidx_filename = idx_filename + ".t";
    timer_start();
    tree.Save(outidx_filename.c_str());
    ////////////////////////// Toggle_1 /////////////////////////////////////

    ////////////////////////// Toggle_2 /////////////////////////////////////
 //    vector<rtree_info> idx_rtree_infos = {
	// 	  { 5,	10,	INDEX_DIM,	7	}
	// 	, { 10,	20,	INDEX_DIM,	15	}
	// 	, { 15,	30,	INDEX_DIM,	22	}
	// 	, { 20,	40,	INDEX_DIM,	30	}
	// 	, { 25,	50,	INDEX_DIM,	37	}
	// 	, { 30,	60,	INDEX_DIM,	45	}
	// };

 //    node_type* tree;

 //    for (int i = 0; i < idx_rtree_infos.size(); i++) {
 //        if (i != 4)
 //            continue;
        
 //    	cout << "Building tree for info #" << i << endl;

	//     timer_start();
	//     build_tree(&tree, tree_data, num_cells, &idx_rtree_infos[i]);
	//     cout << "Built the tree in " << timer_end(SECOND) << "(s)" << endl;

	//     string outidx_filename = idx_filename + "." + to_string(i);
 //    	timer_start();
 //        save_rtree(tree, outidx_filename.c_str(), &idx_rtree_infos[i]);
 //        cout << "Tree saved to " << outidx_filename << " in " << timer_end(SECOND) << "(s)" << endl;

 //        free_tree(tree, &idx_rtree_infos[i]);
	// 	cout << "Tree freed" << endl << endl;
 //    }

 //    for (int i = 0; i < num_cells; i++) {
 //    	free(tree_data[i]);
 //    }
 //    free(tree_data);
    ////////////////////////// Toggle_2 /////////////////////////////////////

}