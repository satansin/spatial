#!/bin/bash

source ../common/config/dir_loc.sh


# ./out/query_donut_prob3.out     "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans_donut/comp_1/comp_1.300.grid "$DIR_QUERY"/indoor_scans_spec/comp_1/q_01.0.ply 1 0.48 -force_cell=23942
# ./out/query_donut_prob3_cpq.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans_donut/comp_1/comp_1.300.grid "$DIR_QUERY"/indoor_scans_spec/comp_1/q_01.0.ply 1 0.48

# ./out/query_donut_prob3_cpq.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_INDEX"/indoor_scans_donut/comp_7/comp_7.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_7/q_01.1.ply 10 -small
# gdb -ex 'file ./out/query_donut_prob3_cpq.out' -ex 'run /ssd/hliubs/3dor_exp/db/indoor_scans/comp_7/ /ssd/hliubs/3dor_exp/index/indoor_scans_donut/comp_7/comp_7.300.grid /ssd/hliubs/3dor_exp/query/indoor_scans_spec/comp_7/q_01.0.ply 1 10'

# ./out/query_det3_cpq.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_INDEX"/indoor_scans_gt/comp_7/comp_7.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_7/q_02.0.ply 0.001 -small -force_cell=14
# ./out/query_det3_cpq.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_INDEX"/indoor_scans_gt/comp_6/comp_6.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_6/q_01.0.ply 0.001 -small -force_cell=14
# ./out/query_donut_det3_cpq.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_INDEX"/indoor_scans_donut/comp_7/comp_7.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_7/q_01.0.ply 0.005 -small -force_cell=15
# gdb -ex 'file ./out/query_det3_cpq.out' -ex 'run /ssd/hliubs/3dor_exp/db/indoor_scans/comp_7/ /ssd/hliubs/3dor_exp/index/indoor_scans_gt/comp_7/comp_7.200.grid /ssd/hliubs/3dor_exp/query/indoor_scans_spec/comp_7/q_01.0.ply 0.005 -small'
# gdb -ex 'file ./out/query_donut_det3_cpq.out' -ex 'run /ssd/hliubs/3dor_exp/db/indoor_scans/comp_7/ /ssd/hliubs/3dor_exp/index/indoor_scans_donut/comp_7/comp_7.200.grid /ssd/hliubs/3dor_exp/query/indoor_scans_spec/comp_7/q_01.0.ply 0.005 -small -force_cell=15'

# attention: size = 6/7 donut: activate -small
# ./out/query_donut_prob3.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6.050.grid "$DIR_QUERY"/indoor_scans_spec/comp_6/q_01.1.ply 32 4 -small

# ./out/query_donut_prob3_cpq.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans_donut/comp_1/comp_1.200.grid "$DIR_QUERY"/indoor_scans_rand_alt/comp_1/d_1000/q_0.1.ply 0.48 #-delta=1 #-force_cell=36755 > z.log

# ./out/query_donut_prob3.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6.350.grid "$DIR_QUERY"/indoor_scans_spec/comp_6/q_01.1.ply 32 4 -small

# ./out/query_donut_prob3_cpq.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_6/q_01.1.ply 3.87 -small
# ./out/query_donut_prob3.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_5/q_01.0.ply 1.4 -delta=0.1

# ./out/query_donut_prob3_cpq.out "$DIR_DB"/indoor_scans/comp_5_5/ "$DIR_INDEX"/indoor_scans_donut/comp_5_5/comp_5_5.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_5_5/q_01.1.ply "$DIR_QUERY"/indoor_scans_spec/comp_5_5/q_01.1.ply 1.6 -delta=0.1

# test for different sampling rate
# ./out/query_donut_prob3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_donut_color/comp_5/comp_5.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_5_5/q_01.1.ply "$DIR_QUERY"/indoor_scans_spec/comp_5_5/q_01.1.ply 3.2 -delta=1
./out/query_donut_prob3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_donut_color/comp_5/comp_5.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_1_5/q_01.1.ply "$DIR_QUERY"/indoor_scans_spec/comp_1_5/q_01.1.ply 3.2 -delta=1
# ./out/query_donut_prob3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_donut_color/comp_5/comp_5.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_5/q_01.1.ply "$DIR_QUERY"/indoor_scans_spec/comp_5/q_01.1.ply 3.2 -delta=1

# ./out/query_donut_prob3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_donut_color/comp_5/comp_5.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_5/q_01.1.ply "$DIR_QUERY"/indoor_scans_spec/comp_5/q_01.1.ply 5 -delta=0.1

# ./out/query_donut_prob3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_5/q_01.1.ply "$DIR_QUERY"/indoor_scans_spec/comp_5/q_01.1.ply 1.4 -delta=0.1

# ./out/query_donut_prob3_cpq.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_6/q_01.1.ply "$DIR_QUERY"/indoor_scans_spec/comp_6/q_01.1.ply 3.87 -delta=0.1

# ./out/query_donut_prob3_cpq.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_5/q_01.1.ply 1.4 -delta=0.01 #-force_cell=2756
# ./out/query_donut_prob3_cpq.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_6/q_01.1.ply 1.4 -delta=20 -small


# gdb -ex 'file ./out/grid_gt.out' -ex 'run /data/hliubs/3dor_exp/db/object_scans/1_sel_100i/ 200 test.grid -num_parts=1'
# gdb -ex 'file ./out/index3.out'  -ex 'run /data/hliubs/3dor_exp/db/object_scans/1_sel_100i/     test.grid'

# ./out/query_prob6_cpq.out "$DIR_OBJ_DB"/1_sel_100i/ "$DIR_OBJ_INDEX"/object_scans_gt/1_sel_100i/1_sel_100i.200.grid "$DIR_OBJ_QUERY"/spec/q_01.0.ply 24 -delta=1 #eps=12?
# ./out/query_donut_prob6_cpq.out "$DIR_OBJ_DB"/1_sel_100i/ "$DIR_OBJ_INDEX"/object_scans_donut/1_sel_100i/1_sel_100i.1000.grid "$DIR_OBJ_QUERY"/spec/q_02.1.ply 24 -small
# ./out/query_3nn_prob6_cpq.out "$DIR_OBJ_DB"/1_sel_100i/ "$DIR_OBJ_INDEX"/object_scans_3nn/1_sel_100i/1_sel_100i.200.015.grid "$DIR_OBJ_QUERY"/spec_01/00942s00.ply 24 -delta=1

# ./out/query_det6_cpq.out "$DIR_OBJ_DB"/1_sel_100i/ "$DIR_OBJ_INDEX"/object_scans_gt/1_sel_100i/1_sel_100i.1000.grid "$DIR_OBJ_QUERY"/spec/q_01.0.ply 0.005 #eps=12?

# ./out/query_prob6_cpq.out "$DIR_OBJ_DB"/2_sel_001k/ "$DIR_OBJ_INDEX"/object_scans_gt/2_sel_001k/2_sel_001k.200.grid "$DIR_OBJ_QUERY"/spec/q_01.0.ply 24 -delta=1 -small
# ./out/query_donut_prob6_cpq.out "$DIR_OBJ_DB"/2_sel_001k/ "$DIR_OBJ_INDEX"/object_scans_donut/2_sel_001k/2_sel_001k.1000.grid "$DIR_OBJ_QUERY"/spec/q_02.1.ply 24 -small
# ./out/query_3nn_prob6_cpq.out "$DIR_OBJ_DB"/2_sel_001k/ "$DIR_OBJ_INDEX"/object_scans_3nn/2_sel_001k/2_sel_001k.200.015.grid "$DIR_OBJ_QUERY"/spec/q_03.0.ply 24 -delta=1

# ./out/query_det6_cpq.out "$DIR_OBJ_DB"/2_sel_001k/ "$DIR_OBJ_INDEX"/object_scans_gt/2_sel_001k/2_sel_001k.1000.grid "$DIR_OBJ_QUERY"/spec/q_01.0.ply 0.001 #eps=12?

# ./out/query_prob6_cpq.out "$DIR_OBJ_DB"/3_sel_010k/ "$DIR_OBJ_INDEX"/object_scans_gt/3_sel_010k/3_sel_010k.300.grid "$DIR_OBJ_QUERY"/spec/q_02.1.ply 24 -small
# ./out/query_donut_prob6_cpq.out "$DIR_OBJ_DB"/3_sel_010k/ "$DIR_OBJ_INDEX"/object_scans_donut/3_sel_010k/3_sel_010k.1000.grid "$DIR_OBJ_QUERY"/spec/q_02.0.ply 24 -small -delta=10
# gdb -ex 'file ./out/query_donut_prob6_cpq.out' -ex 'run /data/hliubs/3dor_exp/db/object_scans/3_sel_010k/ /data/hliubs/3dor_exp/index/object_scans/object_scans_donut/3_sel_010k/3_sel_010k.1000.grid /data/hliubs/3dor_exp/query/object_scans/spec/q_02.1.ply 24 -small'
# ./out/query_3nn_prob6_cpq.out "$DIR_OBJ_DB"/3_sel_010k/ "$DIR_OBJ_INDEX"/object_scans_3nn/3_sel_010k/3_sel_010k.1000.015.grid "$DIR_OBJ_QUERY"/rand/rand_3/q_9.4.ply 24 -small

# ./out/query_prob6_cpq.out "$DIR_OBJ_DB"/4_sel_100k/ "$DIR_OBJ_INDEX"/object_scans_gt/4_sel_100k/4_sel_100k.1000.grid "$DIR_OBJ_QUERY"/spec/q_02.1.ply 24 -small
# ./out/query_donut_prob6_cpq.out "$DIR_OBJ_DB"/4_sel_100k/ "$DIR_OBJ_INDEX"/object_scans_donut/4_sel_100k/4_sel_100k.1000.grid "$DIR_OBJ_QUERY"/spec/q_01.1.ply 24 -small
# ./out/query_donut_prob6_cpq.out "$DIR_OBJ_DB"/4_sel_100k/ "$DIR_OBJ_INDEX"/object_scans_donut/4_sel_100k/4_sel_100k.1000.grid "$DIR_OBJ_QUERY"/spec/q_02.1.ply 24 -small
# ./out/query_donut_prob6_cpq.out "$DIR_OBJ_DB"/4_sel_100k/ "$DIR_OBJ_INDEX"/object_scans_donut/4_sel_100k/4_sel_100k.1000.grid "$DIR_OBJ_QUERY"/rand/rand_4/q_0.0.ply 24 -small -delta=1
# ./out/query_3nn_prob6_cpq.out "$DIR_OBJ_DB"/4_sel_100k/ "$DIR_OBJ_INDEX"/object_scans_3nn/4_sel_100k/4_sel_100k.200.015.grid "$DIR_OBJ_QUERY"/spec/q_02.1.ply 24 -small

# ./out/query_donut_prob6_cpq.out "$DIR_OBJ_DB"/5_sel_001m/ "$DIR_OBJ_INDEX"/object_scans_donut/5_sel_001m/5_sel_001m.1000.grid "$DIR_OBJ_QUERY"/spec/q_02.1.ply 24 -small #-delta=1
# ./out/query_donut_prob6_cpq.out "$DIR_OBJ_DB"/5_sel_001m/ "$DIR_OBJ_INDEX"/object_scans_donut/5_sel_001m/5_sel_001m.1000.grid "$DIR_OBJ_QUERY"/spec/q_02.0.ply 24 -small -delta=1
# ./out/query_donut_prob6_cpq.out "$DIR_OBJ_DB"/5_sel_001m/ "$DIR_OBJ_INDEX"/object_scans_donut/5_sel_001m/5_sel_001m.1000.grid "$DIR_OBJ_QUERY"/rand/rand_5/q_0.0.ply 24 -small -delta=1
# ./out/query_donut_prob6_cpq.out "$DIR_OBJ_DB"/5_sel_001m/ "$DIR_OBJ_INDEX"/object_scans_donut/5_sel_001m/5_sel_001m.1000.grid "$DIR_OBJ_QUERY"/rand/rand_5/q_1.0.ply 24 -small -delta=1
# ./out/query_donut_prob6_cpq.out "$DIR_OBJ_DB"/5_sel_001m/ "$DIR_OBJ_INDEX"/object_scans_donut/5_sel_001m/5_sel_001m.1000.grid "$DIR_OBJ_QUERY"/rand/rand_5/q_2.0.ply 24 -small -delta=1
# ./out/query_donut_prob6_cpq.out "$DIR_OBJ_DB"/5_sel_001m/ "$DIR_OBJ_INDEX"/object_scans_donut/5_sel_001m/5_sel_001m.1000.grid "$DIR_OBJ_QUERY"/rand/rand_5/q_3.0.ply 24 -small -delta=1

# ./out/z_test.out "$DIR_OBJ_DB"/3_sel_010k_test_1/ "$DIR_OBJ_DB"/3_sel_010k_test_2/ test_1.ply test_2.ply


# ./out/goicp.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_QUERY"/indoor_scans_pro_noise/comp_6/q_01.0.ply 10
# ./out/goicp.out "$DIR_OBJ_DB"/1_sel_100i/ "$DIR_OBJ_QUERY"/spec/q_02.1.ply -cheat=96            # -stat=test_query.1_sel_100i.q_02_1.txt > test_query.1_sel_100i.q_02_1.log # 97->q_01(07216) 96->q_02(08279) 98->q_03 99->q_04 95->q_05
# ./out/goicp.out "$DIR_OBJ_DB"/2_sel_001k/ "$DIR_OBJ_QUERY"/spec/q_02.1.ply -cheat=984 -cheat=985 -stat=test_query.2_sel_001k.q_02_1.txt > test_query.2_sel_001k.q_02_1.log



# ./out/super4pcs_det.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans_spec/comp_7/q_01.0.ply 0.001
# ./out/superg4pcs_det.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans_spec/comp_7/q_01.0.ply 0.005
# ./out/super4pcs_det.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_QUERY"/indoor_scans/comp_6/q_01.0.ply 0.2
# ./out/super4pcs.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_QUERY"/indoor_scans/comp_5/q_01.0.ply 100

# ./out/super4pcs.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans/comp_7/q_01.1.ply 500
# ./out/super4pcs.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_QUERY"/indoor_scans/comp_6/q_01.1.ply 5000

# ./out/superg4pcs.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans/comp_7/q_01.0.ply 100
# ./out/superg4pcs.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_QUERY"/indoor_scans/comp_6/q_01.0.ply 100
# ./out/superg4pcs.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_QUERY"/indoor_scans/comp_5/q_01.0.ply 100
# ./out/superg4pcs_det.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans_pro_noise/comp_7/q_01.0.ply 10

# ./out/superg4pcs_prob.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans_pro_noise/comp_7/q_01.0.ply 20 10
# ./out/superg4pcs_prob.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_QUERY"/indoor_scans_pro_noise/comp_6/q_01.1.ply 20 1
# ./out/super4pcs_prob.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_QUERY"/indoor_scans_pro_noise/comp_6/q_01.1.ply 20 1

# ./out/super4pcs_prob.out "$DIR_OBJ_DB"/1_sel_100i/ "$DIR_OBJ_QUERY"/spec/q_01.1.ply 24 -num_parts=1
# ./out/super4pcs_prob.out "$DIR_OBJ_DB"/1_sel_100i/ "$DIR_OBJ_QUERY"/spec/q_02.1.ply 24 -num_parts=1
# ./out/super4pcs_prob.out "$DIR_OBJ_DB"/2_sel_001k/ "$DIR_OBJ_QUERY"/spec/q_02.1.ply 24 -num_parts=1

# ./out/superg4pcs_prob.out "$DIR_OBJ_DB"/1_sel_100i/ "$DIR_OBJ_QUERY"/spec/q_01.0.ply 24 -num_parts=1
# ./out/superg4pcs_prob.out "$DIR_OBJ_DB"/1_sel_100i/ "$DIR_OBJ_QUERY"/spec/q_02.4.ply 24 -num_parts=1
# ./out/superg4pcs_prob.out "$DIR_OBJ_DB"/4_sel_100k/ "$DIR_OBJ_QUERY"/spec/q_02.1.ply 24 -num_parts=1
# ./out/superg4pcs_prob.out "$DIR_OBJ_DB"/2_sel_001k/ "$DIR_OBJ_QUERY"/spec/q_02.1.ply 24 -num_parts=1
# ./out/superg4pcs_prob.out "$DIR_OBJ_DB"/3_sel_010k/ "$DIR_OBJ_QUERY"/spec/q_02.1.ply 24 -num_parts=1

# gdb -ex 'file ./out/superg4pcs_prob.out' -ex 'run /data/hliubs/3dor_exp/db/object_scans/4_sel_100k/ /data/hliubs/3dor_exp/query/object_scans/spec/q_02.1.ply 24 -num_parts=1'

# ./out/goicp.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans_rand/comp_7/d_1000/q_0.0.ply

# ./out/tk_super4pcs.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans_spec/comp_7/q_01.0.ply

# ./out/tk_super4pcs_prob.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans_spec/comp_7/q_01.1.ply 10

# ./out/tk_superg4pcs.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans_spec/comp_7/q_01.0.ply

# ./out/tk_gt3_cpq.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_INDEX"/indoor_scans_gt/comp_7/comp_7.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_7/q_01.0.ply

# ./out/tk_gt3_cpq_prob.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_INDEX"/indoor_scans_gt/comp_7/comp_7.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_7/q_01.4.ply 10

# ./out/tk_donut3_cpq_prob.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_6/q_01.1.ply 3.87

# ./out/tk_donut3_cpq_prob.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5.200.grid "$DIR_QUERY"/indoor_scans_spec/comp_5/q_01.1.ply 1.4