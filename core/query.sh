#!/bin/bash

source ../common/config/dir_loc.sh

# ./out/query.out "$DIR_DB"/redwood-01-livingroom.ply "$DIR_RSTREE"/redwood-01-livingroom.rstree.1 "$DIR_INDEX"/redwood-01-livingroom.idx "$DIR_QUERY"/redwood-01-q1.ply "$DIR_RSTREE"/redwood-01-q1.rstree.1 1500 #-debug
# ./out/query.out "$DIR_DB"/redwood-01-livingroom.ply "$DIR_RSTREE"/redwood-01-livingroom.rstree.1 "$DIR_INDEX"/redwood-01-livingroom.idx.edge "$DIR_QUERY"/redwood-01-q1.ply "$DIR_RSTREE"/redwood-01-q1.rstree.1 1500 #-debug


# ./out/query.out "$DIR_DB"/redwood-01-livingroom.ply "$DIR_RSTREE"/redwood-01-livingroom.rstree.1 "$DIR_INDEX"/redwood-01-livingroom.idx "$DIR_QUERY"/redwood-01-q2.ply "$DIR_RSTREE"/redwood-01-q2.rstree.1 30000 -debug
# ./out/query.out "$DIR_DB"/redwood-01-livingroom.ply "$DIR_RSTREE"/redwood-01-livingroom.rstree.1 "$DIR_INDEX"/redwood-01-livingroom.side.idx "$DIR_QUERY"/redwood-01-q2.ply "$DIR_RSTREE"/redwood-01-q2.rstree.1 300000 -test #-debug


# ./out/query.out "$DIR_DB"/redwood-01-livingroom.ply "$DIR_RSTREE"/redwood-01-livingroom.rstree.1 "$DIR_INDEX"/redwood-01-livingroom.idx "$DIR_QUERY"/redwood-01-q1.ply "$DIR_RSTREE"/redwood-01-q1.rstree.1 #-debug

./out/query.out "$DIR_DB"/obj_scans/ "$DIR_RSTREE"/obj_scans/ "$DIR_INDEX"/obj_scans.idx "$DIR_QUERY"/obj_scans/q1.ply "$DIR_QUERY"/obj_scans/q1.ply.rstree.1 1000 -test -batch

























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
