#!/bin/bash

source ../common/config/dir_loc.sh

# ./out/super4pcs_det.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans_pro_noise/comp_7/q_01.0.ply 10
# ./out/super4pcs_det.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_QUERY"/indoor_scans/comp_6/q_01.0.ply 0.2
# ./out/super4pcs.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_QUERY"/indoor_scans/comp_5/q_01.0.ply 100

# ./out/super4pcs.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans/comp_7/q_01.1.ply 500
# ./out/super4pcs.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_QUERY"/indoor_scans/comp_6/q_01.1.ply 5000

# ./out/superg4pcs.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans/comp_7/q_01.0.ply 100
# ./out/superg4pcs.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_QUERY"/indoor_scans/comp_6/q_01.0.ply 100
# ./out/superg4pcs.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_QUERY"/indoor_scans/comp_5/q_01.0.ply 100
# ./out/superg4pcs_det.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans_pro_noise/comp_7/q_01.0.ply 10


# ./out_tmp/superg4pcs_prob.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans_pro_noise/comp_7/q_01.0.ply 20 10
# ./out_tmp/superg4pcs_prob.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_QUERY"/indoor_scans_pro_noise/comp_6/q_01.1.ply 20 1
./out_tmp/super4pcs_prob.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_QUERY"/indoor_scans_pro_noise/comp_6/q_01.1.ply 20 1