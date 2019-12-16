#!/bin/bash

source ../common/config/dir_loc.sh

# # index on synthetic datasets (origin)
# ./index.out "$DIR_SYNTHETIC"/data/pcd-001M.ply 150.0 600.0 602.0 "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-001M.idx
# ./index.out "$DIR_SYNTHETIC"/data/pcd-002M.ply 150.0 600.0 602.0 "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-002M.idx
# ./index.out "$DIR_SYNTHETIC"/data/pcd-005M.ply 150.0 600.0 602.0 "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-005M.idx
# ./index.out "$DIR_SYNTHETIC"/data/pcd-020M.ply 150.0 600.0 602.0 "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-020M.idx
# ./index.out "$DIR_SYNTHETIC"/data/pcd-100M.ply 150.0 600.0 602.0 "$DIR_SYNTHETIC"/index/origin/synthetic-pcd-100M.idx

# index on KITTI datasets ?
# ./index.out "$DIR_KITTI"/kitti-join005.ply 0.2 1.5 1.6 "$DIR_INDEX"/kitti-join005.idx -debug #-show_prog_bar

# index on redwood datasets
# ./out/index.out "$DIR_DB"/redwood-01-livingroom.ply "$DIR_RSTREE"/redwood-01-livingroom.rstree.1 32 60 60.3 "$DIR_INDEX"/redwood-01-livingroom.idx -show_prog_bar #-test
# ./index.out "$DIR_REDWOOD"/redwood-02-d507.ply 12 60 60.3 "$DIR_INDEX"/redwood-02.idx -show_prog_bar #-test

# ./out/index.out "$DIR_DB"/redwood-01-livingroom.ply "$DIR_RSTREE"/redwood-01-livingroom.rstree.1 20 150 60.3 "$DIR_INDEX"/redwood-01-livingroom.side.idx -show_prog_bar

# ./out/index.out "$DIR_DB"/redwood-02-office.ply "$DIR_RSTREE"/redwood-02-office.rstree.1 20 150 60.3 "$DIR_INDEX"/redwood-02-office.side.idx -show_prog_bar


## Generate grids by grid-size and radius

# ./out/grid.out "$DIR_DB"/obj_scans_2/ 20 100 250 "$DIR_INDEX"/obj_scans_2/obj_scans_2.100.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/obj_scans_2/ 20 200 250 "$DIR_INDEX"/obj_scans_2/obj_scans_2.200.grid -show_prog_bar

# ./out/grid.out "$DIR_DB"/obj_scans_17/ 20 100 250 "$DIR_INDEX"/obj_scans_17/obj_scans_17.100.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/obj_scans_17/ 20 200 250 "$DIR_INDEX"/obj_scans_17/obj_scans_17.200.grid -show_prog_bar

# ./out/grid.out "$DIR_DB"/obj_scans_100/ 20 100 250 "$DIR_INDEX"/obj_scans_100/obj_scans_100.100.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/obj_scans_100/ 20 200 250 "$DIR_INDEX"/obj_scans_100/obj_scans_100.200.grid -show_prog_bar


## Build R*-tree over the generated grids

# ./out/index.out "$DIR_DB"/obj_scans_2/ "$DIR_INDEX"/obj_scans_2/obj_scans_2.100.grid "$DIR_INDEX"/obj_scans_2/obj_scans_2.100.idx
# ./out/index.out "$DIR_DB"/obj_scans_2/ "$DIR_INDEX"/obj_scans_2/obj_scans_2.200.grid "$DIR_INDEX"/obj_scans_2/obj_scans_2.200.idx

# ./out/index.out "$DIR_DB"/obj_scans_2/ "$DIR_INDEX"/obj_scans_2/obj_scans_2.100.grid "$DIR_INDEX"/obj_scans_2/obj_scans_2.100.int.idx
# ./out/index.out "$DIR_DB"/obj_scans_2/ "$DIR_INDEX"/obj_scans_2/obj_scans_2.200.grid "$DIR_INDEX"/obj_scans_2/obj_scans_2.200.int.idx


# ./out/index.out "$DIR_DB"/obj_scans_17/ "$DIR_INDEX"/obj_scans_17/obj_scans_17.100.grid "$DIR_INDEX"/obj_scans_17/obj_scans_17.100.idx
# ./out/index.out "$DIR_DB"/obj_scans_17/ "$DIR_INDEX"/obj_scans_17/obj_scans_17.200.grid "$DIR_INDEX"/obj_scans_17/obj_scans_17.200.idx

# ./out/index.out "$DIR_DB"/obj_scans_17/ "$DIR_INDEX"/obj_scans_17/obj_scans_17.100.grid "$DIR_INDEX"/obj_scans_17/obj_scans_17.100.int.idx
# ./out/index.out "$DIR_DB"/obj_scans_17/ "$DIR_INDEX"/obj_scans_17/obj_scans_17.200.grid "$DIR_INDEX"/obj_scans_17/obj_scans_17.200.int.idx


./out/index.out "$DIR_DB"/obj_scans_100/ "$DIR_INDEX"/obj_scans_100/obj_scans_100.100.grid "$DIR_INDEX"/obj_scans_100/obj_scans_100.100.int.idx
./out/index.out "$DIR_DB"/obj_scans_100/ "$DIR_INDEX"/obj_scans_100/obj_scans_100.200.grid "$DIR_INDEX"/obj_scans_100/obj_scans_100.200.int.idx