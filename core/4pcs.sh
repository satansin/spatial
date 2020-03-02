#!/bin/bash

source ../common/config/dir_loc.sh

./out/super4pcs.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans/comp_7/q_01.0.ply 1000
./out/superg4pcs.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans/comp_7/q_01.0.ply 1000