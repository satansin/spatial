#include "pcr_adv.h"
#include "util.h"
#include "RTree.h"
#include "ProgressBar.hpp"
#include <iostream>
using namespace std;

int main(int argc, char **argv) {

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " database_path grid_filename [-show_prog_bar]" << endl;
        exit(1);
    }

    bool show_prog_bar = false;
    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-show_prog_bar")
            show_prog_bar = true;
    }

    int argi = 0;
    string db_path = argv[(++argi)];
    string grid_filename = argv[(++argi)];

    cout << "Parameters:" << endl;
    cout << "db_path = " << db_path << endl;
    cout << "grid_filename = " << grid_filename << endl;
    cout << endl;

    cout << "Reading database files from " << db_path << endl;
    DB_Meshes db_meshes;
    int num_meshes = db_meshes.read_from_path(db_path);
    cout << "Total no. meshes: " << num_meshes << endl << endl;

    cout << "Loading DB structure from " << grid_filename << endl;
    Struct_DB s_db;
    s_db.read(grid_filename, &db_meshes);

    int num_cells = s_db.get_total_cells_count();
    cout << "Total no. cells: " << num_cells << endl << endl;

    cout << "DB index parameters:" << endl;
    cout << "w = " << s_db.w << endl;
    cout << "ann_min = " << s_db.ann_min << endl;
    cout << "ann_mid = " << s_db.ann_mid << endl;
    cout << "ann_max = " << s_db.ann_max << endl;
    cout << "ang_min = " << s_db.ang_min << endl;

    timer_start();

    auto entries = s_db.get_entries();

    ////////////////////////// Toggle_1 /////////////////////////////////////
    IndexTree tree;
    ProgressBar bar(num_cells, 70);
    ////////////////////////// Toggle_1 /////////////////////////////////////

    ////////////////////////// Toggle_2 /////////////////////////////////////
    // R_TYPE** tree_data = (R_TYPE**) malloc(sizeof(R_TYPE *) * num_cells);
    ////////////////////////// Toggle_2 /////////////////////////////////////

    int insert_count = 0;

    for (int i = 0; i < num_cells; i++) {

        if (show_prog_bar) {
            ++bar;
            bar.display();
        }

        auto e = entries[i];
        if (e->fail) {
            continue;
        }

        ////////////////////////// Toggle_1 /////////////////////////////////////
        int box_min[INDEX_DIM], box_max[INDEX_DIM];
        e->get_index_box(box_min, box_max);
        
        tree.Insert(box_min, box_max, i);
        ////////////////////////// Toggle_1 /////////////////////////////////////

        insert_count++;

        ////////////////////////// Toggle_2 /////////////////////////////////////
        // tree_data[i] = (R_TYPE *) malloc(sizeof(R_TYPE) * INDEX_DIM);
        // for (int dim_i = 0; dim_i < INDEX_DIM; dim_i++) {
        //     tree_data[i][dim_i] = (int) (e->sides[dim_i] * 1e5);
        // }
        ////////////////////////// Toggle_2 /////////////////////////////////////

    }

    if (show_prog_bar) {
        bar.done();
    }

    ////////////////////////// Toggle_1 /////////////////////////////////////

    tree.SortDim0();

    string outidx_filename = get_idx_filename(grid_filename);

    cout << "Saving the index to " << outidx_filename << endl;
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

    double user_time = timer_end(SECOND);
    cout << "Index built and saved in " << user_time << "(s)" << endl;
    cout << "In total " << insert_count << " entries inserted" << endl;

    string outidx_stat_filename = outidx_filename + ".istat";
    ofstream ofs_stat(outidx_stat_filename);

    ofs_stat << "num_insert = " << insert_count << endl;
    ofs_stat << "user_time = " << user_time << endl;

    ofs_stat.close();

}