#!/bin/bash

source ../common/config/dir_loc.sh


# ./out/query_donut_prob3.out     "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans_donut/comp_1/comp_1.300.grid "$DIR_QUERY"/indoor_scans_spec/comp_1/q_01.0.ply 1 0.48 -force_cell=23942
# ./out/query_donut_prob3_cpq.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans_donut/comp_1/comp_1.300.grid "$DIR_QUERY"/indoor_scans_spec/comp_1/q_01.0.ply 1 0.48

# ./out/query_donut_prob3_cpq.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_INDEX"/indoor_scans_donut/comp_7/comp_7.300.grid "$DIR_QUERY"/indoor_scans_spec/comp_7/q_01.0.ply 1 10 -small
# gdb -ex 'file ./out/query_donut_prob3_cpq.out' -ex 'run /ssd/hliubs/3dor_exp/db/indoor_scans/comp_7/ /ssd/hliubs/3dor_exp/index/indoor_scans_donut/comp_7/comp_7.300.grid /ssd/hliubs/3dor_exp/query/indoor_scans_spec/comp_7/q_01.0.ply 1 10'

# attention: size = 6/7 donut: activate -small
# ./out/query_donut_prob3.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6.050.grid "$DIR_QUERY"/indoor_scans_spec/comp_6/q_01.1.ply 32 4 -small

./out/query_donut_prob3_cpq.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans_donut/comp_1/comp_1.200.grid "$DIR_QUERY"/indoor_scans_rand_alt/comp_1/d_1000/q_0.1.ply 0.48 #-delta=1 #-force_cell=36755 > z.log

# ./out/query_donut_prob3.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6.350.grid "$DIR_QUERY"/indoor_scans_spec/comp_6/q_01.1.ply 32 4 -small
