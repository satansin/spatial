#!/bin/bash

source ../common/config/dir_loc.sh

LD_LIBRARY_PATH=/usr/local/GNU/gsl/lib/
export LD_LIBRARY_PATH


### from raw to db

# mkdir -p "$DIR_DB"/indoor_scans/recon_bedroom_0/
# mkdir -p "$DIR_DB"/indoor_scans/recon_bedroom_1/
# mkdir -p "$DIR_DB"/indoor_scans/recon_bedroom_2/
# mkdir -p "$DIR_DB"/indoor_scans/recon_bedroom_3/
# mkdir -p "$DIR_DB"/indoor_scans/recon_bedroom_4/
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_bedroom_0.ply "$DIR_DB"/indoor_scans/recon_bedroom_0/recon_bedroom_0.ply 1000 # cannot process due to file too large?
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_bedroom_1.ply "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 1000
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_bedroom_2.ply "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 1000
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_bedroom_3.ply "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 1000
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_bedroom_4.ply "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 1000

# mkdir -p "$DIR_DB"/indoor_scans/recon_boardroom_0/
# mkdir -p "$DIR_DB"/indoor_scans/recon_boardroom_1/
# mkdir -p "$DIR_DB"/indoor_scans/recon_boardroom_2/
# mkdir -p "$DIR_DB"/indoor_scans/recon_boardroom_3/
# mkdir -p "$DIR_DB"/indoor_scans/recon_boardroom_4/
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_boardroom_0.ply "$DIR_DB"/indoor_scans/recon_boardroom_0/recon_boardroom_0.ply 1000 # cannot process due to file too large?
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_boardroom_1.ply "$DIR_DB"/indoor_scans/recon_boardroom_1/recon_boardroom_1.ply 1000
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_boardroom_2.ply "$DIR_DB"/indoor_scans/recon_boardroom_2/recon_boardroom_2.ply 1000
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_boardroom_3.ply "$DIR_DB"/indoor_scans/recon_boardroom_3/recon_boardroom_3.ply 1000
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_boardroom_4.ply "$DIR_DB"/indoor_scans/recon_boardroom_4/recon_boardroom_4.ply 1000

# mkdir -p "$DIR_DB"/indoor_scans/recon_loft_0/
# mkdir -p "$DIR_DB"/indoor_scans/recon_loft_1/
# mkdir -p "$DIR_DB"/indoor_scans/recon_loft_2/
# mkdir -p "$DIR_DB"/indoor_scans/recon_loft_3/
# mkdir -p "$DIR_DB"/indoor_scans/recon_loft_4/
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_loft_0.ply "$DIR_DB"/indoor_scans/recon_loft_0/recon_loft_0.ply 1000 # cannot process due to file too large?
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_loft_1.ply "$DIR_DB"/indoor_scans/recon_loft_1/recon_loft_1.ply 1000
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_loft_2.ply "$DIR_DB"/indoor_scans/recon_loft_2/recon_loft_2.ply 1000
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_loft_3.ply "$DIR_DB"/indoor_scans/recon_loft_3/recon_loft_3.ply 1000
# ./out/filter.out "$DIR_RAW"/indoor_scans/recon_loft_4.ply "$DIR_DB"/indoor_scans/recon_loft_4/recon_loft_4.ply 1000


# mkdir -p "$DIR_DB"/obj_scans/obj_scans_1_full/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_2_full/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_3_full/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_4_full/
# ./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_1/ "$DIR_DB"/obj_scans/obj_scans_1_full/ 1000 -batch
# ./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_2/ "$DIR_DB"/obj_scans/obj_scans_2_full/ 1000 -batch
# ./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_3/ "$DIR_DB"/obj_scans/obj_scans_3_full/ 1000 -batch
# ./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_4/ "$DIR_DB"/obj_scans/obj_scans_4_full/ 1000 -batch



### build rstree index for db

# ./out/pre_index.out "$DIR_DB"/indoor_scans/recon_bedroom_1/ -batch
# ./out/pre_index.out "$DIR_DB"/indoor_scans/recon_bedroom_2/ -batch
# ./out/pre_index.out "$DIR_DB"/indoor_scans/recon_bedroom_3/ -batch
# ./out/pre_index.out "$DIR_DB"/indoor_scans/recon_bedroom_4/ -batch

# ./out/pre_index.out "$DIR_DB"/indoor_scans/recon_boardroom_1/ -batch
# ./out/pre_index.out "$DIR_DB"/indoor_scans/recon_boardroom_2/ -batch
# ./out/pre_index.out "$DIR_DB"/indoor_scans/recon_boardroom_3/ -batch
# ./out/pre_index.out "$DIR_DB"/indoor_scans/recon_boardroom_4/ -batch

# ./out/pre_index.out "$DIR_DB"/indoor_scans/recon_loft_1/ -batch
# ./out/pre_index.out "$DIR_DB"/indoor_scans/recon_loft_2/ -batch
# ./out/pre_index.out "$DIR_DB"/indoor_scans/recon_loft_3/ -batch
# ./out/pre_index.out "$DIR_DB"/indoor_scans/recon_loft_4/ -batch


# ./out/pre_index.out "$DIR_DB"/obj_scans/obj_scans_1_full/ -batch
# ./out/pre_index.out "$DIR_DB"/obj_scans/obj_scans_2_full/ -batch
# ./out/pre_index.out "$DIR_DB"/obj_scans/obj_scans_3_full/ -batch
# ./out/pre_index.out "$DIR_DB"/obj_scans/obj_scans_4_full/ -batch



### Make soft links to select varied no. of object scans

# mkdir -p "$DIR_DB"/obj_scans/obj_scans_1_100/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_1_200/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_1_300/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_1_400/
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_1_full/ "$DIR_DB"/obj_scans/obj_scans_1_100/ 100
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_1_full/ "$DIR_DB"/obj_scans/obj_scans_1_200/ 200
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_1_full/ "$DIR_DB"/obj_scans/obj_scans_1_300/ 300
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_1_full/ "$DIR_DB"/obj_scans/obj_scans_1_400/ 400

# mkdir -p "$DIR_DB"/obj_scans/obj_scans_2_100/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_2_200/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_2_300/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_2_400/
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_2_full/ "$DIR_DB"/obj_scans/obj_scans_2_100/ 100
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_2_full/ "$DIR_DB"/obj_scans/obj_scans_2_200/ 200
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_2_full/ "$DIR_DB"/obj_scans/obj_scans_2_300/ 300
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_2_full/ "$DIR_DB"/obj_scans/obj_scans_2_400/ 400

# mkdir -p "$DIR_DB"/obj_scans/obj_scans_3_100/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_3_200/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_3_300/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_3_400/
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_3_full/ "$DIR_DB"/obj_scans/obj_scans_3_100/ 100
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_3_full/ "$DIR_DB"/obj_scans/obj_scans_3_200/ 200
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_3_full/ "$DIR_DB"/obj_scans/obj_scans_3_300/ 300
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_3_full/ "$DIR_DB"/obj_scans/obj_scans_3_400/ 400

# mkdir -p "$DIR_DB"/obj_scans/obj_scans_4_100/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_4_200/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_4_300/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_4_400/
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_4_full/ "$DIR_DB"/obj_scans/obj_scans_4_100/ 100
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_4_full/ "$DIR_DB"/obj_scans/obj_scans_4_200/ 200
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_4_full/ "$DIR_DB"/obj_scans/obj_scans_4_300/ 300
# ./out/batch_sel.out "$DIR_DB"/obj_scans/obj_scans_4_full/ "$DIR_DB"/obj_scans/obj_scans_4_400/ 400



### extract queries from db

# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_0/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_1/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_2/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_3/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_4/

# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_01/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_02/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_03/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_04/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_05/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_06/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_07/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_08/

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 1400 2400 2000 3000 500 1450 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_01/q_01.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 1400 2400 2000 3000 500 1450 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_01/q_01.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 1400 2400 2000 3000 500 1450 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_01/q_01.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 1400 2400 2000 3000 500 1450 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_01/q_01.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply -1220 0 2250 3500 -1850 -400 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_02/q_02.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply -1220 0 2250 3500 -1850 -400 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_02/q_02.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply -1220 0 2250 3500 -1850 -400 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_02/q_02.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply -1220 0 2250 3500 -1850 -400 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_02/q_02.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply -200 800 3500 5000 -3000 -1900 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_03/q_03.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply -200 800 3500 5000 -3000 -1900 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_03/q_03.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply -200 800 3500 5000 -3000 -1900 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_03/q_03.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply -200 800 3500 5000 -3000 -1900 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_03/q_03.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 700 3000 2000 4800 -4000 -2000 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_04/q_04.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 700 3000 2000 4800 -4000 -2000 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_04/q_04.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 700 3000 2000 4800 -4000 -2000 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_04/q_04.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 700 3000 2000 4800 -4000 -2000 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_04/q_04.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 1500 3000 500 2000 700 3000 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_05/q_05.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 1500 3000 500 2000 700 3000 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_05/q_05.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 1500 3000 500 2000 700 3000 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_05/q_05.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 1500 3000 500 2000 700 3000 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_05/q_05.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 2800 3800 2400 4100 -2300 -800 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_06/q_06.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 2800 3800 2400 4100 -2300 -800 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_06/q_06.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 2800 3800 2400 4100 -2300 -800 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_06/q_06.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 2800 3800 2400 4100 -2300 -800 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_06/q_06.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 1000 1450 2100 2700 600 1800 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_07/q_07.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 1000 1450 2100 2700 600 1800 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_07/q_07.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 1000 1450 2100 2700 600 1800 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_07/q_07.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply 1000 1450 2100 2700 600 1800 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_07/q_07.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply -3000 0 800 3300 200 3000 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_08/q_08.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply -3000 0 800 3300 200 3000 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_08/q_08.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply -3000 0 800 3300 200 3000 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_08/q_08.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_1/recon_bedroom_1.ply -3000 0 800 3300 200 3000 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_08/q_08.3.ply

# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_01/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_02/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_03/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_04/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_05/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_06/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_07/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_08/

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 1400 2400 2000 3000 500 1450 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_01/q_01.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 1400 2400 2000 3000 500 1450 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_01/q_01.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 1400 2400 2000 3000 500 1450 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_01/q_01.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 1400 2400 2000 3000 500 1450 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_01/q_01.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply -1220 0 2250 3500 -1850 -400 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_02/q_02.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply -1220 0 2250 3500 -1850 -400 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_02/q_02.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply -1220 0 2250 3500 -1850 -400 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_02/q_02.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply -1220 0 2250 3500 -1850 -400 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_02/q_02.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply -200 800 3500 5000 -3000 -1900 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_03/q_03.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply -200 800 3500 5000 -3000 -1900 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_03/q_03.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply -200 800 3500 5000 -3000 -1900 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_03/q_03.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply -200 800 3500 5000 -3000 -1900 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_03/q_03.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 700 3000 2000 4800 -4000 -2000 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_04/q_04.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 700 3000 2000 4800 -4000 -2000 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_04/q_04.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 700 3000 2000 4800 -4000 -2000 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_04/q_04.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 700 3000 2000 4800 -4000 -2000 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_04/q_04.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 1500 3000 500 2000 700 3000 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_05/q_05.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 1500 3000 500 2000 700 3000 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_05/q_05.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 1500 3000 500 2000 700 3000 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_05/q_05.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 1500 3000 500 2000 700 3000 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_05/q_05.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 2800 3800 2400 4100 -2300 -800 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_06/q_06.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 2800 3800 2400 4100 -2300 -800 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_06/q_06.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 2800 3800 2400 4100 -2300 -800 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_06/q_06.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 2800 3800 2400 4100 -2300 -800 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_06/q_06.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 1000 1450 2100 2700 600 1800 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_07/q_07.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 1000 1450 2100 2700 600 1800 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_07/q_07.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 1000 1450 2100 2700 600 1800 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_07/q_07.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply 1000 1450 2100 2700 600 1800 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_07/q_07.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply -3000 0 800 3300 200 3000 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_08/q_08.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply -3000 0 800 3300 200 3000 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_08/q_08.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply -3000 0 800 3300 200 3000 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_08/q_08.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_2/recon_bedroom_2.ply -3000 0 800 3300 200 3000 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_08/q_08.3.ply

# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_01/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_02/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_03/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_04/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_05/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_06/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_07/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_08/

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 1400 2400 2000 3000 500 1450 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_01/q_01.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 1400 2400 2000 3000 500 1450 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_01/q_01.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 1400 2400 2000 3000 500 1450 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_01/q_01.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 1400 2400 2000 3000 500 1450 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_01/q_01.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply -1220 0 2250 3500 -1850 -400 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_02/q_02.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply -1220 0 2250 3500 -1850 -400 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_02/q_02.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply -1220 0 2250 3500 -1850 -400 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_02/q_02.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply -1220 0 2250 3500 -1850 -400 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_02/q_02.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply -200 800 3500 5000 -3000 -1900 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_03/q_03.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply -200 800 3500 5000 -3000 -1900 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_03/q_03.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply -200 800 3500 5000 -3000 -1900 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_03/q_03.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply -200 800 3500 5000 -3000 -1900 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_03/q_03.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 700 3000 2000 4800 -4000 -2000 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_04/q_04.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 700 3000 2000 4800 -4000 -2000 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_04/q_04.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 700 3000 2000 4800 -4000 -2000 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_04/q_04.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 700 3000 2000 4800 -4000 -2000 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_04/q_04.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 1500 3000 500 2000 700 3000 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_05/q_05.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 1500 3000 500 2000 700 3000 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_05/q_05.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 1500 3000 500 2000 700 3000 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_05/q_05.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 1500 3000 500 2000 700 3000 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_05/q_05.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 2800 3800 2400 4100 -2300 -800 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_06/q_06.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 2800 3800 2400 4100 -2300 -800 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_06/q_06.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 2800 3800 2400 4100 -2300 -800 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_06/q_06.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 2800 3800 2400 4100 -2300 -800 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_06/q_06.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 1000 1450 2100 2700 600 1800 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_07/q_07.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 1000 1450 2100 2700 600 1800 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_07/q_07.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 1000 1450 2100 2700 600 1800 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_07/q_07.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply 1000 1450 2100 2700 600 1800 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_07/q_07.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply -3000 0 800 3300 200 3000 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_08/q_08.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply -3000 0 800 3300 200 3000 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_08/q_08.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply -3000 0 800 3300 200 3000 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_08/q_08.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_3/recon_bedroom_3.ply -3000 0 800 3300 200 3000 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_08/q_08.3.ply

# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_01/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_02/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_03/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_04/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_05/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_06/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_07/
# mkdir -p "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_08/

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 1400 2400 2000 3000 500 1450 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_01/q_01.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 1400 2400 2000 3000 500 1450 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_01/q_01.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 1400 2400 2000 3000 500 1450 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_01/q_01.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 1400 2400 2000 3000 500 1450 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_01/q_01.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply -1220 0 2250 3500 -1850 -400 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_02/q_02.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply -1220 0 2250 3500 -1850 -400 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_02/q_02.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply -1220 0 2250 3500 -1850 -400 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_02/q_02.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply -1220 0 2250 3500 -1850 -400 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_02/q_02.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply -200 800 3500 5000 -3000 -1900 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_03/q_03.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply -200 800 3500 5000 -3000 -1900 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_03/q_03.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply -200 800 3500 5000 -3000 -1900 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_03/q_03.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply -200 800 3500 5000 -3000 -1900 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_03/q_03.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 700 3000 2000 4800 -4000 -2000 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_04/q_04.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 700 3000 2000 4800 -4000 -2000 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_04/q_04.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 700 3000 2000 4800 -4000 -2000 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_04/q_04.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 700 3000 2000 4800 -4000 -2000 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_04/q_04.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 1500 3000 500 2000 700 3000 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_05/q_05.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 1500 3000 500 2000 700 3000 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_05/q_05.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 1500 3000 500 2000 700 3000 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_05/q_05.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 1500 3000 500 2000 700 3000 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_05/q_05.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 2800 3800 2400 4100 -2300 -800 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_06/q_06.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 2800 3800 2400 4100 -2300 -800 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_06/q_06.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 2800 3800 2400 4100 -2300 -800 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_06/q_06.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 2800 3800 2400 4100 -2300 -800 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_06/q_06.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 1000 1450 2100 2700 600 1800 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_07/q_07.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 1000 1450 2100 2700 600 1800 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_07/q_07.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 1000 1450 2100 2700 600 1800 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_07/q_07.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply 1000 1450 2100 2700 600 1800 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_07/q_07.3.ply

# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply -3000 0 800 3300 200 3000 0 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_08/q_08.0.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply -3000 0 800 3300 200 3000 1 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_08/q_08.1.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply -3000 0 800 3300 200 3000 2 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_08/q_08.2.ply
# ./out/extract_query.out "$DIR_DB"/indoor_scans/recon_bedroom_4/recon_bedroom_4.ply -3000 0 800 3300 200 3000 3 "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_08/q_08.3.ply

# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_2/	0.2	1.0	"$DIR_QUERY"/obj_scans_2_test/q.020.100.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_2/	0.2	0.9	"$DIR_QUERY"/obj_scans_2_test/q.020.090.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_2/	0.2	0.8	"$DIR_QUERY"/obj_scans_2_test/q.020.080.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_2/	0.2	0.7	"$DIR_QUERY"/obj_scans_2_test/q.020.070.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_2/	0.2	0.6	"$DIR_QUERY"/obj_scans_2_test/q.020.060.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_2/	0	0.9	"$DIR_QUERY"/obj_scans_2_test/q.000.090.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_2/	0.1	0.9	"$DIR_QUERY"/obj_scans_2_test/q.010.090.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_2/	0.2	0.9	"$DIR_QUERY"/obj_scans_2_test/q.020.090.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_2/	0.3	0.9	"$DIR_QUERY"/obj_scans_2_test/q.030.090.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_2/	0.4	0.9	"$DIR_QUERY"/obj_scans_2_test/q.040.090.ply

# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_17/	0.2	1.0	"$DIR_QUERY"/obj_scans_17_test/q.020.100.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_17/	0.2	0.9	"$DIR_QUERY"/obj_scans_17_test/q.020.090.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_17/	0.2	0.8	"$DIR_QUERY"/obj_scans_17_test/q.020.080.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_17/	0.2	0.7	"$DIR_QUERY"/obj_scans_17_test/q.020.070.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_17/	0.2	0.6	"$DIR_QUERY"/obj_scans_17_test/q.020.060.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_17/	0	0.9	"$DIR_QUERY"/obj_scans_17_test/q.000.090.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_17/	0.1	0.9	"$DIR_QUERY"/obj_scans_17_test/q.010.090.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_17/	0.2	0.9	"$DIR_QUERY"/obj_scans_17_test/q.020.090.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_17/	0.3	0.9	"$DIR_QUERY"/obj_scans_17_test/q.030.090.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_17/	0.4	0.9	"$DIR_QUERY"/obj_scans_17_test/q.040.090.ply

# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_100/	0	1.0	"$DIR_QUERY"/obj_scans_100/q1.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_100/	0.1	1.0	"$DIR_QUERY"/obj_scans_100/q2.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_100/	0	0.9	"$DIR_QUERY"/obj_scans_100/q3.ply
# ./out/extract_query_batch.out	"$DIR_DB"/obj_scans_100/	0.1	0.9	"$DIR_QUERY"/obj_scans_100/q4.ply


### build rstree index for query

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_01/q_01.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_01/q_01.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_01/q_01.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_01/q_01.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_02/q_02.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_02/q_02.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_02/q_02.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_02/q_02.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_03/q_03.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_03/q_03.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_03/q_03.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_03/q_03.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_04/q_04.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_04/q_04.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_04/q_04.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_04/q_04.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_05/q_05.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_05/q_05.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_05/q_05.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_05/q_05.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_06/q_06.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_06/q_06.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_06/q_06.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_06/q_06.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_07/q_07.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_07/q_07.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_07/q_07.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_07/q_07.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_08/q_08.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_08/q_08.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_08/q_08.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_1/q_08/q_08.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_01/q_01.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_01/q_01.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_01/q_01.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_01/q_01.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_02/q_02.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_02/q_02.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_02/q_02.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_02/q_02.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_03/q_03.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_03/q_03.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_03/q_03.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_03/q_03.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_04/q_04.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_04/q_04.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_04/q_04.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_04/q_04.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_05/q_05.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_05/q_05.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_05/q_05.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_05/q_05.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_06/q_06.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_06/q_06.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_06/q_06.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_06/q_06.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_07/q_07.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_07/q_07.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_07/q_07.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_07/q_07.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_08/q_08.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_08/q_08.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_08/q_08.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_2/q_08/q_08.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_01/q_01.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_01/q_01.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_01/q_01.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_01/q_01.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_02/q_02.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_02/q_02.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_02/q_02.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_02/q_02.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_03/q_03.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_03/q_03.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_03/q_03.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_03/q_03.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_04/q_04.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_04/q_04.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_04/q_04.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_04/q_04.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_05/q_05.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_05/q_05.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_05/q_05.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_05/q_05.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_06/q_06.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_06/q_06.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_06/q_06.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_06/q_06.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_07/q_07.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_07/q_07.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_07/q_07.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_07/q_07.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_08/q_08.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_08/q_08.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_08/q_08.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_3/q_08/q_08.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_01/q_01.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_01/q_01.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_01/q_01.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_01/q_01.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_02/q_02.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_02/q_02.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_02/q_02.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_02/q_02.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_03/q_03.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_03/q_03.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_03/q_03.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_03/q_03.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_04/q_04.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_04/q_04.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_04/q_04.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_04/q_04.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_05/q_05.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_05/q_05.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_05/q_05.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_05/q_05.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_06/q_06.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_06/q_06.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_06/q_06.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_06/q_06.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_07/q_07.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_07/q_07.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_07/q_07.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_07/q_07.3.ply

# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_08/q_08.0.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_08/q_08.1.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_08/q_08.2.ply
# ./out/pre_index.out "$DIR_QUERY"/indoor_scans/recon_bedroom_4/q_08/q_08.3.ply

# ./out/pre_index.out "$DIR_QUERY"/obj_scans_2/q1.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_2/q2.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_2/q3.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_2/q4.ply

# ./out/pre_index.out "$DIR_QUERY"/obj_scans_2_test/q.020.100.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_2_test/q.020.090.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_2_test/q.020.080.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_2_test/q.020.070.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_2_test/q.020.060.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_2_test/q.000.090.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_2_test/q.010.090.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_2_test/q.020.090.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_2_test/q.030.090.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_2_test/q.040.090.ply

# ./out/pre_index.out "$DIR_QUERY"/obj_scans_17/q1.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_17/q2.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_17/q3.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_17/q4.ply

# ./out/pre_index.out "$DIR_QUERY"/obj_scans_17_test/q.020.100.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_17_test/q.020.090.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_17_test/q.020.080.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_17_test/q.020.070.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_17_test/q.020.060.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_17_test/q.000.090.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_17_test/q.010.090.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_17_test/q.020.090.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_17_test/q.030.090.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_17_test/q.040.090.ply

# ./out/pre_index.out "$DIR_QUERY"/obj_scans_100/q1.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_100/q2.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_100/q3.ply
# ./out/pre_index.out "$DIR_QUERY"/obj_scans_100/q4.ply