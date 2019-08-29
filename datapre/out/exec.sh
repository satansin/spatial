#!/bin/bash

# # 1. Generate synthetic database point clouds
# ./pcg.out 1000000 17320.5 /rwproject/kdd-db/hliubs/spatial/3dor/synthetic/pcd-001M.ply
# ./pcg.out 2000000 21823.84 /rwproject/kdd-db/hliubs/spatial/3dor/synthetic/pcd-002M.ply
# ./pcg.out 5000000 29617.65 /rwproject/kdd-db/hliubs/spatial/3dor/synthetic/pcd-005M.ply
# ./pcg.out 20000000 47015.1 /rwproject/kdd-db/hliubs/spatial/3dor/synthetic/pcd-020M.ply
# ./pcg.out 100000000 80394.7 /rwproject/kdd-db/hliubs/spatial/3dor/synthetic/pcd-100M.ply

# # 2. Generate joined KITTI database point clouds and related info files
# ./kitti_join.out 5 /rwproject/kdd-db/hliubs/spatial/3dor/KITTI/kitti-join005.ply
# ./kitti_join.out 10 /rwproject/kdd-db/hliubs/spatial/3dor/KITTI/kitti-join010.ply
# ./kitti_join.out 20 /rwproject/kdd-db/hliubs/spatial/3dor/KITTI/kitti-join020.ply

# # 3. Generate redwood database point clouds
# ./filter.out /rwproject/kdd-db/hliubs/spatial/3dor/redwood/raw/iml_hotel_umd_slac_mesh.ply /rwproject/kdd-db/hliubs/spatial/3dor/redwood/redwood-01-hotel.ply 1000
# ./filter.out /rwproject/kdd-db/hliubs/spatial/3dor/redwood/raw/iml_mit_32_d507_slac_mesh.ply /rwproject/kdd-db/hliubs/spatial/3dor/redwood/redwood-02-d507.ply 1000

# 4. Extract queries
./extract_query.out \
/rwproject/kdd-db/hliubs/spatial/3dor/synthetic/pcd-001M.ply \
0 1500 0 1500 0 1500 \
2000 0.05 0.9 \
/rwproject/kdd-db/hliubs/spatial/3dor/synthetic/pcd-001M-q1.ply