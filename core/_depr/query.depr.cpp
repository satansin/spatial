// // try to use volume and volume ratio for indexing keys
// // but the returned results are too many
// int retrieve_congr_entry_2(vector<Entry*>& e_list, double epsilon, IndexTree* tree, const Struct_DB* s_db,
//     vector<Entry_Pair*>& ret, int& total_page_accessed, bool verbose = false) {

//     double corr_epsilon = max(0.001, 2 * epsilon);

//     int total_nhits = 0;

//     double query_min[INDEX_DIM], query_max[INDEX_DIM];

//     for (auto &e: e_list) {

// 		cal_range(e->repre->pt, e->remai[0]->pt, e->remai[1]->pt, e->remai[2]->pt, corr_epsilon, query_min[0], query_max[0], query_min[1], query_max[1]);
// 		cout << "[" << query_min[0] << ", " << query_max[0] << "], [" << query_min[1] << ", " << query_max[1] << "]" << endl;

//         const int HIGHEST = 6;
//         int *page_accessed = new int[HIGHEST];
//         for (int i = 0; i < HIGHEST; i++) {
//             page_accessed[i] = 0;
//         }

// 		vector<int> search_ret;

// 		int num = tree->Search(query_min, query_max, search_ret, page_accessed);

// 		total_nhits += num;

//     }

//     return total_nhits;

// }