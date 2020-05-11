#!/bin/bash

source ../common/config/dir_loc.sh

# ./out/goicp.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_QUERY"/indoor_scans_pro_noise/comp_6/q_01.0.ply 10
./out/goicp.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_QUERY"/indoor_scans_pro_noise/comp_7/q_01.0.ply 10

# ./../goicp/build/GoICP "$DIR_DB"/indoor_scans/comp_6_txt/db_0_o.txt "$DIR_QUERY"/indoor_scans_pro_noise/comp_6_txt/q_01_o.0.txt -1 goicp_config.txt output.txt
# ./../goicp/build/GoICP "$DIR_DB"/indoor_scans/comp_7_txt/db_0_o.txt "$DIR_QUERY"/indoor_scans_pro_noise/comp_7_txt/q_01_o.0.txt -1 goicp_config.txt output.txt