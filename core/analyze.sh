#!/bin/bash

source ../common/config/dir_loc.sh

# ./out/analyze.out "$DIR_DB"/redwood-01-livingroom.ply "$DIR_RSTREE"/redwood-01-livingroom.rstree.1 "$DIR_INDEX"/redwood-01-livingroom.side.idx 20

# ./out/analyze.out "$DIR_DB"/redwood-02-office.ply "$DIR_RSTREE"/redwood-02-office.rstree.1 "$DIR_INDEX"/redwood-02-office.side.idx 20

./out/analyze.out "$DIR_DB"/obj_scans/ "$DIR_INDEX"/obj_scans.idx 20 -batch