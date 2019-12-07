#!/bin/bash

source ../common/config/dir_loc.sh


### from raw to db

# ./filter.out "$DIR_REDW"/raw/iml_hotel_umd_slac_mesh.ply "$DIR_REDW"/redwood-01-hotel.ply 1000
# ./filter.out "$DIR_REDW"/redwood/raw/iml_mit_32_d507_slac_mesh.ply "$DIR_REDW"/redwood-02-d507.ply 1000
# ./filter.out "$DIR_REDW"/raw/sun_livingroom1_slac.ply "$DIR_REDW"/redwood-03-living.ply 1000

# ./out/filter.out "$DIR_RAW"/livingroom.ply "$DIR_DB"/redwood-01-livingroom.ply 1000
# ./out/filter.out "$DIR_RAW"/office.ply "$DIR_DB"/redwood-02-office.ply 1000

# ./out/filter.out "$DIR_RAW"/obj_scans "$DIR_DB"/obj_scans 1000 -batch
# ./out/filter.out "$DIR_RAW"/obj_scans "$DIR_DB"/obj_scans_100 1000 -batch


### extract queries from db

# ./out/extract_query_manual.out "$DIR_DB"/redwood-01-livingroom.ply 1200 2500 0 1160 -500 2100 0 1.0 "$DIR_QUERY"/redwood-01-q1.ply
# ./out/extract_query_manual.out "$DIR_DB"/redwood-01-livingroom.ply 1200 2500 0 1160 -500 2100 0.3 1.0 "$DIR_QUERY"/redwood-01-q2.ply
# ./out/extract_query_manual.out "$DIR_DB"/redwood-01-livingroom.ply 1200 2500 0 1160 -500 2100 0 0.8 "$DIR_QUERY"/redwood-01-q3.ply
# ./out/extract_query_manual.out "$DIR_DB"/redwood-01-livingroom.ply 1200 2500 0 1160 -500 2100 0.3 0.8 "$DIR_QUERY"/redwood-01-q4.ply

# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans/	0	1.0	"$DIR_QUERY"/obj_scans/q1.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans/	0.1	1.0	"$DIR_QUERY"/obj_scans/q2.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans/	0	0.9	"$DIR_QUERY"/obj_scans/q3.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans/	0.1	0.9	"$DIR_QUERY"/obj_scans/q4.ply

# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_100/	0	1.0	"$DIR_QUERY"/obj_scans_100/q1.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_100/	0.1	1.0	"$DIR_QUERY"/obj_scans_100/q2.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_100/	0	0.9	"$DIR_QUERY"/obj_scans_100/q3.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_100/	0.1	0.9	"$DIR_QUERY"/obj_scans_100/q4.ply


### build rstree index

# ./out/pre_index.out "$DIR_DB"/redwood-01-livingroom.ply "$DIR_DB"/redwood-01-livingroom.ply.rstree
# ./out/pre_index.out "$DIR_DB"/redwood-02-office.ply "$DIR_DB"/redwood-02-office.ply.rstree

# ./out/pre_index.out "$DIR_QUERY"/redwood-01-q1.ply "$DIR_QUERY"/redwood-01-q1.ply.rstree
# ./out/pre_index.out "$DIR_QUERY"/redwood-01-q2.ply "$DIR_QUERY"/redwood-01-q2.ply.rstree
# ./out/pre_index.out "$DIR_QUERY"/redwood-01-q3.ply "$DIR_QUERY"/redwood-01-q3.ply.rstree
# ./out/pre_index.out "$DIR_QUERY"/redwood-01-q4.ply "$DIR_QUERY"/redwood-01-q4.ply.rstree

# ./out/pre_index.out "$DIR_DB"/obj_scans/ -batch

# ./out/pre_index.out "$DIR_QUERY"/obj_scans/q1.ply "$DIR_QUERY"/obj_scans/q1.ply.rstree
# ./out/pre_index.out "$DIR_QUERY"/obj_scans/q2.ply "$DIR_QUERY"/obj_scans/q2.ply.rstree
# ./out/pre_index.out "$DIR_QUERY"/obj_scans/q3.ply "$DIR_QUERY"/obj_scans/q3.ply.rstree
# ./out/pre_index.out "$DIR_QUERY"/obj_scans/q4.ply "$DIR_QUERY"/obj_scans/q4.ply.rstree

./out/pre_index.out "$DIR_DB"/obj_scans_100/ -batch

./out/pre_index.out "$DIR_QUERY"/obj_scans_100/q1.ply "$DIR_QUERY"/obj_scans_100/q1.ply.rstree
./out/pre_index.out "$DIR_QUERY"/obj_scans_100/q2.ply "$DIR_QUERY"/obj_scans_100/q2.ply.rstree
./out/pre_index.out "$DIR_QUERY"/obj_scans_100/q3.ply "$DIR_QUERY"/obj_scans_100/q3.ply.rstree
./out/pre_index.out "$DIR_QUERY"/obj_scans_100/q4.ply "$DIR_QUERY"/obj_scans_100/q4.ply.rstree