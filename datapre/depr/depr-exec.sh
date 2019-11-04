#!/bin/bash

DIR_SYN="/rwproject/kdd-db/hliubs/spatial/3dor/synthetic"
DIR_KITTI="/rwproject/kdd-db/hliubs/spatial/3dor/KITTI"

SYN_D_1=17320.5
SYN_N_1=1000000
SYN_D_2=21823.8
SYN_N_2=2000000
SYN_D_3=29617.7
SYN_N_3=5000000
SYN_D_4=47015.1
SYN_N_4=20000000
SYN_D_5=80394.7
SYN_N_5=100000000
SYN_N_D=$SYN_N_2

SYN_W_1=1100
SYN_M_1=1000
SYN_W_2=1300
SYN_M_2=2000
SYN_W_3=1800
SYN_M_3=5000
SYN_W_4=2800
SYN_M_4=20000
SYN_W_5=4800
SYN_M_5=100000
SYN_W_D=$SYN_W_2
SYN_M_D=$SYN_M_2

SYN_ETA_1=1.0
SYN_ETA_2=0.9
SYN_ETA_3=0.8
SYN_ETA_4=0.7
SYN_ETA_5=0.6
SYN_ETA_D=$SYN_ETA_2

SYN_EPS_1=0
SYN_EPS_2=0.05
SYN_EPS_3=0.1
SYN_EPS_4=0.15
SYN_EPS_5=0.2
SYN_EPS_D=$SYN_EPS_3

# # 1. Generate synthetic database point clouds (deprecated)
# ./pcg.out $SYN_N_1 $SYN_D_1 "$DIR_SYN"/data/pcd-001M.ply
# ./pcg.out $SYN_N_2 $SYN_D_2 "$DIR_SYN"/data/pcd-002M.ply
# ./pcg.out $SYN_N_3 $SYN_D_3 "$DIR_SYN"/data/pcd-005M.ply
# ./pcg.out $SYN_N_4 $SYN_D_4 "$DIR_SYN"/data/pcd-020M.ply
# ./pcg.out $SYN_N_5 $SYN_D_5 "$DIR_SYN"/data/pcd-100M.ply

# # 2. Generate joined KITTI database point clouds and related info files (deprecated)
# ./kitti_join.out 10 /rwproject/kdd-db/hliubs/spatial/3dor/KITTI/kitti-join010.ply
# ./kitti_join.out 20 /rwproject/kdd-db/hliubs/spatial/3dor/KITTI/kitti-join020.ply
# ./kitti_join.out 30 /rwproject/kdd-db/hliubs/spatial/3dor/KITTI/kitti-join030.ply
# ./kitti_join.out 40 /rwproject/kdd-db/hliubs/spatial/3dor/KITTI/kitti-join040.ply
# ./kitti_join.out 50 /rwproject/kdd-db/hliubs/spatial/3dor/KITTI/kitti-join050.ply

# # 3. Generate redwood database point clouds
# ./filter.out /rwproject/kdd-db/hliubs/spatial/3dor/redwood/raw/iml_hotel_umd_slac_mesh.ply /rwproject/kdd-db/hliubs/spatial/3dor/redwood/redwood-01-hotel.ply 1000
# ./filter.out /rwproject/kdd-db/hliubs/spatial/3dor/redwood/raw/iml_mit_32_d507_slac_mesh.ply /rwproject/kdd-db/hliubs/spatial/3dor/redwood/redwood-02-d507.ply 1000

# # 4. Extract queries for synthetic datasets

# mkdir -p "$DIR_SYN"/queries/

# # 4.1 Varied n, fixed m, eta, epsilon_m

# mkdir -p "$DIR_SYN"/queries/varied_n_1/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-001M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_1/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-001M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_1/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-001M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_1/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-001M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_1/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-001M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_1/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_n_2/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_2/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_2/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_2/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_2/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_2/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_n_3/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-005M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_3/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-005M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_3/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-005M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_3/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-005M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_3/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-005M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_3/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_n_4/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-020M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_4/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-020M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_4/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-020M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_4/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-020M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_4/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-020M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_4/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_n_5/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-100M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_5/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-100M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_5/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-100M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_5/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-100M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_5/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-100M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_n_5/05.ply

# # 4.2 Varied m, fixed n, eta, epsilon_m

# mkdir -p "$DIR_SYN"/queries/varied_m_1/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_1 $SYN_M_1 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_1/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_1 $SYN_M_1 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_1/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_1 $SYN_M_1 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_1/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_1 $SYN_M_1 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_1/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_1 $SYN_M_1 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_1/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_m_2/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_2 $SYN_M_2 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_2/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_2 $SYN_M_2 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_2/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_2 $SYN_M_2 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_2/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_2 $SYN_M_2 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_2/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_2 $SYN_M_2 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_2/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_m_3/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_3 $SYN_M_3 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_3/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_3 $SYN_M_3 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_3/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_3 $SYN_M_3 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_3/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_3 $SYN_M_3 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_3/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_3 $SYN_M_3 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_3/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_m_4/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_4 $SYN_M_4 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_4/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_4 $SYN_M_4 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_4/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_4 $SYN_M_4 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_4/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_4 $SYN_M_4 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_4/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_4 $SYN_M_4 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_4/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_m_5/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_5 $SYN_M_5 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_5/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_5 $SYN_M_5 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_5/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_5 $SYN_M_5 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_5/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_5 $SYN_M_5 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_5/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_5 $SYN_M_5 $SYN_EPS_D $SYN_ETA_D "$DIR_SYN"/queries/varied_m_5/05.ply

# # 4.3 Varied eta, fixed n, m, epsilon_m

# mkdir -p "$DIR_SYN"/queries/varied_eta_1/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_1 "$DIR_SYN"/queries/varied_eta_1/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_1 "$DIR_SYN"/queries/varied_eta_1/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_1 "$DIR_SYN"/queries/varied_eta_1/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_1 "$DIR_SYN"/queries/varied_eta_1/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_1 "$DIR_SYN"/queries/varied_eta_1/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_eta_2/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_2 "$DIR_SYN"/queries/varied_eta_2/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_2 "$DIR_SYN"/queries/varied_eta_2/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_2 "$DIR_SYN"/queries/varied_eta_2/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_2 "$DIR_SYN"/queries/varied_eta_2/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_2 "$DIR_SYN"/queries/varied_eta_2/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_eta_3/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_3 "$DIR_SYN"/queries/varied_eta_3/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_3 "$DIR_SYN"/queries/varied_eta_3/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_3 "$DIR_SYN"/queries/varied_eta_3/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_3 "$DIR_SYN"/queries/varied_eta_3/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_3 "$DIR_SYN"/queries/varied_eta_3/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_eta_4/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_4 "$DIR_SYN"/queries/varied_eta_4/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_4 "$DIR_SYN"/queries/varied_eta_4/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_4 "$DIR_SYN"/queries/varied_eta_4/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_4 "$DIR_SYN"/queries/varied_eta_4/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_4 "$DIR_SYN"/queries/varied_eta_4/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_eta_5/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_5 "$DIR_SYN"/queries/varied_eta_5/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_5 "$DIR_SYN"/queries/varied_eta_5/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_5 "$DIR_SYN"/queries/varied_eta_5/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_5 "$DIR_SYN"/queries/varied_eta_5/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_D $SYN_ETA_5 "$DIR_SYN"/queries/varied_eta_5/05.ply

# # 4.4 Varied epsilon_m, fixed n, m, eta

# mkdir -p "$DIR_SYN"/queries/varied_eps_1/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_1 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_1/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_1 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_1/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_1 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_1/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_1 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_1/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_1 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_1/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_eps_2/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_2 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_2/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_2 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_2/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_2 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_2/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_2 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_2/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_2 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_2/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_eps_3/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_3 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_3/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_3 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_3/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_3 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_3/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_3 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_3/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_3 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_3/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_eps_4/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_4 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_4/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_4 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_4/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_4 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_4/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_4 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_4/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_4 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_4/05.ply

# mkdir -p "$DIR_SYN"/queries/varied_eps_5/

# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_5 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_5/01.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_5 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_5/02.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_5 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_5/03.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_5 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_5/04.ply
# ./extract_query_auto.out "$DIR_SYN"/data/pcd-002M.ply $SYN_W_D $SYN_M_D $SYN_EPS_5 $SYN_ETA_D "$DIR_SYN"/queries/varied_eps_5/05.ply

# 5. Extract queries for KITTI datasets

