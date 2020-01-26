#!/bin/bash

source ../common/config/dir_loc.sh


## query for indoor scan - recon_bedroom

# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/ "$DIR_INDEX"/indoor_scans/recon_bedroom_1/recon_bedroom_1.016.080.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_01/q_01.0.ply 1000 #-test
# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/ "$DIR_INDEX"/indoor_scans/recon_bedroom_1/recon_bedroom_1.016.080.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_01/q_01.1.ply 40000 #-test
./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/ "$DIR_INDEX"/indoor_scans/recon_bedroom_1/recon_bedroom_1.016.080.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_01/q_01.2.ply 40000 #-test -verbose=0 -force_cell=311 #-force_pt=15454 #-force_pt=18095
# gdb -ex 'file ./out/query.out' -ex 'run /rwproject/kdd-db/hliubs/10_data_spatial/3dor/db/indoor_scans/recon_bedroom_1/ /rwproject/kdd-db/hliubs/10_data_spatial/3dor/index/indoor_scans/recon_bedroom_1/recon_bedroom_1.016.080.grid /rwproject/kdd-db/hliubs/10_data_spatial/3dor/query/indoor_scans/recon_bedroom_1/q_01/q_01.2.ply 40000 -test -verbose=1 -force_cell=311 -force_pt=15454'
# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/ "$DIR_INDEX"/indoor_scans/recon_bedroom_1/recon_bedroom_1.016.080.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_01/q_01.3.ply 150000 #-test

# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/ "$DIR_INDEX"/indoor_scans/recon_bedroom_2/recon_bedroom_2.024.120.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_01/q_01.0.ply 1000 #-test -verbose=0 -force_cell=435
# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/ "$DIR_INDEX"/indoor_scans/recon_bedroom_2/recon_bedroom_2.024.120.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_01/q_01.1.ply 20000 #-test
# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/ "$DIR_INDEX"/indoor_scans/recon_bedroom_2/recon_bedroom_2.024.120.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_01/q_01.2.ply 40000 #-test
# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/ "$DIR_INDEX"/indoor_scans/recon_bedroom_2/recon_bedroom_2.024.120.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_01/q_01.3.ply 60000 #-test

# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/ "$DIR_INDEX"/indoor_scans/recon_bedroom_3/recon_bedroom_3.030.150.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_01/q_01.0.ply 1000 #-test
# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/ "$DIR_INDEX"/indoor_scans/recon_bedroom_3/recon_bedroom_3.030.150.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_01/q_01.1.ply 10000 #-test
# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/ "$DIR_INDEX"/indoor_scans/recon_bedroom_3/recon_bedroom_3.030.150.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_01/q_01.2.ply 20000 #-test
# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/ "$DIR_INDEX"/indoor_scans/recon_bedroom_3/recon_bedroom_3.030.150.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_01/q_01.3.ply 30000 #-test

# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/ "$DIR_INDEX"/indoor_scans/recon_bedroom_4/recon_bedroom_4.040.200.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_01/q_01.0.ply 1000 #-test
# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/ "$DIR_INDEX"/indoor_scans/recon_bedroom_4/recon_bedroom_4.040.200.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_01/q_01.1.ply 6000 #-test
# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/ "$DIR_INDEX"/indoor_scans/recon_bedroom_4/recon_bedroom_4.040.200.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_01/q_01.2.ply 15000 #-test
# ./out/query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/ "$DIR_INDEX"/indoor_scans/recon_bedroom_4/recon_bedroom_4.040.200.grid "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_01/q_01.3.ply 20000 #-test


## query for object scans 2

# ./out/query.out "$DIR_DB"/obj_scans_2/ "$DIR_INDEX"/obj_scans_2/obj_scans_2.100.grid "$DIR_INDEX"/obj_scans_2/obj_scans_2.100.int.idx.4 "$DIR_QUERY"/obj_scans_2/q1.ply 1000 -verbose=0 #-test -force_cell=2716 #-force_pt=161891
# ./out/query.out "$DIR_DB"/obj_scans_2/ "$DIR_INDEX"/obj_scans_2/obj_scans_2.100.grid "$DIR_INDEX"/obj_scans_2/obj_scans_2.100.int.idx.4 "$DIR_QUERY"/obj_scans_2/q2.ply 300000 -verbose=0 #-test -force_cell=2270 #-force_pt=46371
# gdb -ex 'file ./out/query.out' -ex 'run /project/kdd/hliubs/data/spatial/3dor/db/obj_scans_2/ /project/kdd/hliubs/data/spatial/3dor/index/obj_scans_2/obj_scans_2.new.100.0.idx /project/kdd/hliubs/data/spatial/3dor/query/obj_scans_2/q2.ply 300000 -verbose=1 -test -force_cell=2270 -force_pt=46371'

# ./out/query.out "$DIR_DB"/obj_scans_2/ "$DIR_INDEX"/obj_scans_2/obj_scans_2.200.grid "$DIR_INDEX"/obj_scans_2/obj_scans_2.200.int.idx.4 "$DIR_QUERY"/obj_scans_2/q1.ply 1000 -verbose=0 #-test
# ./out/query.out "$DIR_DB"/obj_scans_2/ "$DIR_INDEX"/obj_scans_2/obj_scans_2.200.grid "$DIR_INDEX"/obj_scans_2/obj_scans_2.200.int.idx.4 "$DIR_QUERY"/obj_scans_2/q2.ply 300000 -verbose=0 #-test

# ./out/query.out "$DIR_DB"/obj_scans_2/ "$DIR_INDEX"/obj_scans_2/obj_scans_2.100.grid "$DIR_INDEX"/obj_scans_2/obj_scans_2.100.int.idx.4 "$DIR_QUERY"/obj_scans_2_test/q.000.090.ply 1000 #-test


## query for object scans 17

# ./out/query.out "$DIR_DB"/obj_scans_17/ "$DIR_INDEX"/obj_scans_17/obj_scans_17.100.grid "$DIR_INDEX"/obj_scans_17/obj_scans_17.100.int.idx.4 "$DIR_QUERY"/obj_scans_17/q1.ply 1000 -verbose=0 #-test
# ./out/query.out "$DIR_DB"/obj_scans_17/ "$DIR_INDEX"/obj_scans_17/obj_scans_17.100.grid "$DIR_INDEX"/obj_scans_17/obj_scans_17.100.int.idx.4 "$DIR_QUERY"/obj_scans_17/q2.ply 200000 -verbose=0 #-test

# ./out/query.out "$DIR_DB"/obj_scans_17/ "$DIR_INDEX"/obj_scans_17/obj_scans_17.200.grid "$DIR_INDEX"/obj_scans_17/obj_scans_17.200.int.idx.4 "$DIR_QUERY"/obj_scans_17/q1.ply 1000 -verbose=0 #-test
# ./out/query.out "$DIR_DB"/obj_scans_17/ "$DIR_INDEX"/obj_scans_17/obj_scans_17.200.grid "$DIR_INDEX"/obj_scans_17/obj_scans_17.200.int.idx.4 "$DIR_QUERY"/obj_scans_17/q2.ply 200000 -verbose=0 #-test

# ./out/query.out "$DIR_DB"/obj_scans_17/ "$DIR_INDEX"/obj_scans_17/obj_scans_17.100.grid "$DIR_INDEX"/obj_scans_17/obj_scans_17.100.int.idx.4 "$DIR_QUERY"/obj_scans_17_test/q.010.090.ply 200000 #-test


## query for object scans 100

# ./out/query.out "$DIR_DB"/obj_scans_100/ "$DIR_INDEX"/obj_scans_100/obj_scans_100.100.grid "$DIR_INDEX"/obj_scans_100/obj_scans_100.100.int.idx.4 "$DIR_QUERY"/obj_scans_100/q1.ply 1000 -verbose=0 #-test
# ./out/query.out "$DIR_DB"/obj_scans_100/ "$DIR_INDEX"/obj_scans_100/obj_scans_100.100.grid "$DIR_INDEX"/obj_scans_100/obj_scans_100.100.int.idx.4 "$DIR_QUERY"/obj_scans_100/q2.ply 1000 -verbose=0 #-test



















# ./query.out "$DIR_SYNTHETIC"/data/pcd-001M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-001M.idx "$DIR_SYNTHETIC"/queries/varied_n_1/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-001M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-001M.idx "$DIR_SYNTHETIC"/queries/varied_n_1/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-001M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-001M.idx "$DIR_SYNTHETIC"/queries/varied_n_1/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-001M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-001M.idx "$DIR_SYNTHETIC"/queries/varied_n_1/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-001M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-001M.idx "$DIR_SYNTHETIC"/queries/varied_n_1/05.ply

# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_n_2/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_n_2/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_n_2/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_n_2/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_n_2/05.ply

# ./query.out "$DIR_SYNTHETIC"/data/pcd-005M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-005M.idx "$DIR_SYNTHETIC"/queries/varied_n_3/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-005M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-005M.idx "$DIR_SYNTHETIC"/queries/varied_n_3/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-005M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-005M.idx "$DIR_SYNTHETIC"/queries/varied_n_3/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-005M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-005M.idx "$DIR_SYNTHETIC"/queries/varied_n_3/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-005M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-005M.idx "$DIR_SYNTHETIC"/queries/varied_n_3/05.ply

# ./query.out "$DIR_SYNTHETIC"/data/pcd-020M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-020M.idx "$DIR_SYNTHETIC"/queries/varied_n_4/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-020M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-020M.idx "$DIR_SYNTHETIC"/queries/varied_n_4/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-020M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-020M.idx "$DIR_SYNTHETIC"/queries/varied_n_4/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-020M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-020M.idx "$DIR_SYNTHETIC"/queries/varied_n_4/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-020M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-020M.idx "$DIR_SYNTHETIC"/queries/varied_n_4/05.ply

# ./query.out "$DIR_SYNTHETIC"/data/pcd-100M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-100M.idx "$DIR_SYNTHETIC"/queries/varied_n_5/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-100M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-100M.idx "$DIR_SYNTHETIC"/queries/varied_n_5/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-100M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-100M.idx "$DIR_SYNTHETIC"/queries/varied_n_5/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-100M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-100M.idx "$DIR_SYNTHETIC"/queries/varied_n_5/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-100M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-100M.idx "$DIR_SYNTHETIC"/queries/varied_n_5/05.ply


# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_1/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_1/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_1/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_1/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_1/05.ply

# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_2/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_2/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_2/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_2/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_2/05.ply

# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_3/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_3/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_3/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_3/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_3/05.ply

# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_4/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_4/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_4/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_4/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_4/05.ply

# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_5/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_5/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_5/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_5/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_m_5/05.ply


# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_1/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_1/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_1/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_1/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_1/05.ply

# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_2/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_2/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_2/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_2/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_2/05.ply

# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_3/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_3/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_3/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_3/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_3/05.ply

# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_4/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_4/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_4/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_4/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_4/05.ply

# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_5/01.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_5/02.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_5/03.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_5/04.ply
# ./query.out "$DIR_SYNTHETIC"/data/pcd-002M.ply "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx "$DIR_SYNTHETIC"/queries/varied_eps_5/05.ply
