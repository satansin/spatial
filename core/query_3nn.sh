#!/bin/bash

source ../common/config/dir_loc.sh


# ./out/query_donut_prob6_cpq.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5.120.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_01.0.ply 200 1 -force_cell=2746


# ./out/query_donut_prob6.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5.120.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_01.0.ply 200 1 -force_cell=2746

# ./out/query_prob6.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_gt/comp_5/comp_5.120.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_01.0.ply 200 1 -force_cell=2746

# ./out/query_3nn_prob3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_3nn/comp_5/comp_5.120.015.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_01.0.ply 200 1 -force_cell=2746

# ./out/query_3nn_prob3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_3nn_sim/comp_5/comp_5.120.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_01.0.ply 200 1 -simple -force_cell=2746

# ./out/query_3lnn_prob3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_3lnn/comp_5/comp_5.120.030.015.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_01.0.ply 200 1 -force_cell=2746

# ./out/query_3lnn_prob3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_3lnn_sim/comp_5/comp_5.120.030.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_01.0.ply 200 1 -simple -force_cell=2746





# ./out/query_prob6.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_gt/comp_5/comp_5.120.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_01.0.ply 200 1 -force_cell=2746
# ./out/query_prob3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_gt_sorted/comp_5/comp_5.120.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_01.0.ply 200 1 -sort_entry -force_cell=2746 > zz.prob3.log
# ./out/query_prob3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_gt/comp_5/comp_5.120.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_01.0.ply 200 1 -force_cell=2746 > zz.prob3.nosort.log

# ./out/query_prob6_cpq.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_gt/comp_5/comp_5.120.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_01.0.ply 200 1 -force_cell=785


# ./out/query_donut_prob6.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5.120.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_01.0.ply 200 1 -force_cell=2746
# ./out/query_donut_prob3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5.120.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_01.0.ply 200 1 -force_cell=2746

# ./out_tmp/query_3nn_prob3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_3nn/comp_5/comp_5.120.015.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_01.0.ply 200 1 -force_cell=2746
./out_tmp/query_prob3.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_INDEX"/indoor_scans_gt/comp_6/comp_6.100.grid "$DIR_QUERY"/indoor_scans_pro_noise/comp_6/q_01.0.ply 100 4 -force_cell=2746