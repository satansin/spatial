#!/bin/bash

source ../common/config/dir_loc.sh

./out/extract_query_manual.out "$DIR_DB"/redwood-01-livingroom.ply 1200 2500 0 1160 -500 2100 0 1.0 "$DIR_QUERY"/redwood-01-q1.ply
./out/extract_query_manual.out "$DIR_DB"/redwood-01-livingroom.ply 1200 2500 0 1160 -500 2100 0.3 1.0 "$DIR_QUERY"/redwood-01-q2.ply
./out/extract_query_manual.out "$DIR_DB"/redwood-01-livingroom.ply 1200 2500 0 1160 -500 2100 0 0.8 "$DIR_QUERY"/redwood-01-q3.ply
./out/extract_query_manual.out "$DIR_DB"/redwood-01-livingroom.ply 1200 2500 0 1160 -500 2100 0.3 0.8 "$DIR_QUERY"/redwood-01-q4.ply