#!/bin/bash

source ../common/config/dir_loc.sh

# ./out/pre_index.out "$DIR_DB"/redwood-01-livingroom.ply "$DIR_DB"/redwood-01-livingroom.ply.rstree
# ./out/pre_index.out "$DIR_DB"/redwood-02-office.ply "$DIR_DB"/redwood-02-office.ply.rstree

# ./out/pre_index.out "$DIR_QUERY"/redwood-01-q1.ply "$DIR_QUERY"/redwood-01-q1.ply.rstree
# ./out/pre_index.out "$DIR_QUERY"/redwood-01-q2.ply "$DIR_QUERY"/redwood-01-q2.ply.rstree
# ./out/pre_index.out "$DIR_QUERY"/redwood-01-q3.ply "$DIR_QUERY"/redwood-01-q3.ply.rstree
# ./out/pre_index.out "$DIR_QUERY"/redwood-01-q4.ply "$DIR_QUERY"/redwood-01-q4.ply.rstree

# ./out/pre_index.out "$DIR_DB"/obj_scans/ "$DIR_RSTREE"/obj_scans/ -batch

./out/pre_index.out "$DIR_QUERY"/obj_scans/q1.ply "$DIR_QUERY"/obj_scans/q1.ply.rstree
./out/pre_index.out "$DIR_QUERY"/obj_scans/q2.ply "$DIR_QUERY"/obj_scans/q2.ply.rstree
./out/pre_index.out "$DIR_QUERY"/obj_scans/q3.ply "$DIR_QUERY"/obj_scans/q3.ply.rstree
./out/pre_index.out "$DIR_QUERY"/obj_scans/q4.ply "$DIR_QUERY"/obj_scans/q4.ply.rstree