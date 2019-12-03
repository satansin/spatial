#!/bin/bash

source ../common/config/dir_loc.sh

# ./filter.out "$DIR_REDW"/raw/iml_hotel_umd_slac_mesh.ply "$DIR_REDW"/redwood-01-hotel.ply 1000
# ./filter.out "$DIR_REDW"/redwood/raw/iml_mit_32_d507_slac_mesh.ply "$DIR_REDW"/redwood-02-d507.ply 1000
# ./filter.out "$DIR_REDW"/raw/sun_livingroom1_slac.ply "$DIR_REDW"/redwood-03-living.ply 1000

# ./out/filter.out "$DIR_RAW"/livingroom.ply "$DIR_DB"/redwood-01-livingroom.ply 1000
# ./out/filter.out "$DIR_RAW"/office.ply "$DIR_DB"/redwood-02-office.ply 1000

./out/filter.out "$DIR_RAW"/obj_scans "$DIR_DB"/obj_scans 100 -batch