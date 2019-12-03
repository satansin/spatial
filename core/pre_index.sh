#!/bin/bash

source ../common/config/dir_loc.sh

# ./out/pre_index.out "$DIR_DB"/redwood-01-livingroom.ply "$DIR_RSTREE"/redwood-01-livingroom.rstree
# ./out/pre_index.out "$DIR_DB"/redwood-02-office.ply "$DIR_RSTREE"/redwood-02-office.rstree

# ./out/pre_index.out "$DIR_QUERY"/redwood-01-q1.ply "$DIR_RSTREE"/redwood-01-q1.rstree
# ./out/pre_index.out "$DIR_QUERY"/redwood-01-q2.ply "$DIR_RSTREE"/redwood-01-q2.rstree
# ./out/pre_index.out "$DIR_QUERY"/redwood-01-q3.ply "$DIR_RSTREE"/redwood-01-q3.rstree
# ./out/pre_index.out "$DIR_QUERY"/redwood-01-q4.ply "$DIR_RSTREE"/redwood-01-q4.rstree

./out/pre_index.out "$DIR_DB"/obj_scans/ "$DIR_RSTREE"/obj_scans/ -batch