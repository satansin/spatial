#!/bin/bash

# ./index.out /rwproject/kdd-db/hliubs/spatial/3dor/synthetic/pcd-001M.ply \
# 250.0 800.0 802.0 /rwproject/kdd-db/hliubs/spatial/3dor/index/synthetic-pcd-001M.idx

./query.out /rwproject/kdd-db/hliubs/spatial/3dor/synthetic/pcd-001M.ply \
/rwproject/kdd-db/hliubs/spatial/3dor/index/synthetic-pcd-001M.idx \
/rwproject/kdd-db/hliubs/spatial/3dor/synthetic/pcd-001M-q1.ply 0.05 0.9