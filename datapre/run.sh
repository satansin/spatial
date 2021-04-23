#!/bin/bash

source ../common/config/dir_loc.sh

LD_LIBRARY_PATH=/usr/local/GNU/gsl/lib/
export LD_LIBRARY_PATH


### from raw to db - indoor_scans

	# # full resolution 0: not supported due to file too large
	# for i in 5_4 5_3 5_2 5_1 1_9 1_8 1_7 1_6 1_5 # 5_5 # {1..7} # i->resolution
	# do
	# 	mkdir -p "$DIR_DB"/indoor_scans/recon_bedroom_"$i"/
	# 	./out/filter.out "$DIR_RAW"/indoor_scans/recon_bedroom_"$i".ply "$DIR_DB"/indoor_scans/recon_bedroom_"$i"/recon_bedroom_"$i".ply 1000

	# 	mkdir -p "$DIR_DB"/indoor_scans/recon_boardroom_"$i"/
	# 	./out/filter.out "$DIR_RAW"/indoor_scans/recon_boardroom_"$i".ply "$DIR_DB"/indoor_scans/recon_boardroom_"$i"/recon_boardroom_"$i".ply 1000

	# 	mkdir -p "$DIR_DB"/indoor_scans/recon_loft_"$i"/
	# 	./out/filter.out "$DIR_RAW"/indoor_scans/recon_loft_"$i".ply "$DIR_DB"/indoor_scans/recon_loft_"$i"/recon_loft_"$i".ply 1000
	# done

### build rstree index for db - indoor_scans

	# for i in 5_5 # 1 5 6 7 # i->resolution
	# do
	# 	./out/pre_index.out "$DIR_DB"/indoor_scans/recon_bedroom_"$i"/recon_bedroom_"$i".ply
	# 	./out/pre_index.out "$DIR_DB"/indoor_scans/recon_boardroom_"$i"/recon_boardroom_"$i".ply
	# 	./out/pre_index.out "$DIR_DB"/indoor_scans/recon_loft_"$i"/recon_loft_"$i".ply
	# done

### copy recons to comp - indoor_scans

	# for i in 5_4 5_3 5_2 5_1 1_9 1_8 1_7 1_6 1_5 # 5_5 # {1..7} # i->resolution
	# do
	# 	mkdir -p "$DIR_DB"/indoor_scans/comp_"$i"/

	# 	ln -s "$DIR_DB"/indoor_scans/recon_bedroom_"$i"/recon_bedroom_"$i".ply				"$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply
	# 	ln -s "$DIR_DB"/indoor_scans/recon_boardroom_"$i"/recon_boardroom_"$i".ply			"$DIR_DB"/indoor_scans/comp_"$i"/recon_boardroom_"$i".ply
	# 	ln -s "$DIR_DB"/indoor_scans/recon_loft_"$i"/recon_loft_"$i".ply					"$DIR_DB"/indoor_scans/comp_"$i"/recon_loft_"$i".ply
	# 	echo -e "3\n0 $DIR_DB/indoor_scans/comp_$i/recon_bedroom_$i.ply\n1 $DIR_DB/indoor_scans/comp_$i/recon_boardroom_$i.ply\n2 $DIR_DB/indoor_scans/comp_$i/recon_loft_$i.ply" > "$DIR_DB"/indoor_scans/comp_"$i"/meta.txt
	# 	ln -s "$DIR_DB"/indoor_scans/recon_bedroom_"$i"/recon_bedroom_"$i".ply.rst.0		"$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply.rst.0
	# 	ln -s "$DIR_DB"/indoor_scans/recon_boardroom_"$i"/recon_boardroom_"$i".ply.rst.0	"$DIR_DB"/indoor_scans/comp_"$i"/recon_boardroom_"$i".ply.rst.0
	# 	ln -s "$DIR_DB"/indoor_scans/recon_loft_"$i"/recon_loft_"$i".ply.rst.0				"$DIR_DB"/indoor_scans/comp_"$i"/recon_loft_"$i".ply.rst.0
	# done

### extract queries from db - indoor_scans (Spec)

	# mkdir -p "$DIR_QUERY"/indoor_scans_spec/

	# for i in 5_4 5_3 5_2 5_1 1_9 1_8 1_7 1_6 1_5 # 5_5 # 1 5 6 7 # i->resolution
	# do
	# 	CURR_DIR="$DIR_QUERY"/indoor_scans_spec/comp_"$i"
	# 	mkdir -p "$CURR_DIR"

	# 	if [ $i = 1 ]; then
	# 		nu="s"
	# 	elif [ $i = 5 ]; then
	# 		nu="m"
	# 	elif [ $i = 6 ] || [ $i = 7 ]; then
	# 		nu="l"
	# 	else
	# 		nu="m"
	# 	fi

	# 	for k in {0..4}; do # k->noise level
	# 		./out/extract_query_spec.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   0  1400  2400   2000  3000   500  1450 "$k" "$CURR_DIR"/q_01."$k".ply -noise_unit=$nu
	# 		./out/extract_query_spec.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   0 -1220     0   2250  3500 -1850  -400 "$k" "$CURR_DIR"/q_02."$k".ply -noise_unit=$nu
	# 		./out/extract_query_spec.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   0  -200   800   3500  5000 -3000 -1900 "$k" "$CURR_DIR"/q_03."$k".ply -noise_unit=$nu
	# 		./out/extract_query_spec.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   0   700  3000   2000  4800 -4000 -2000 "$k" "$CURR_DIR"/q_04."$k".ply -noise_unit=$nu
	# 		./out/extract_query_spec.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   0  1500  3000    500  2000   700  3000 "$k" "$CURR_DIR"/q_05."$k".ply -noise_unit=$nu
	# 		./out/extract_query_spec.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   0  2800  3800   2400  4100 -2300  -800 "$k" "$CURR_DIR"/q_06."$k".ply -noise_unit=$nu
	# 		./out/extract_query_spec.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   0  1000  1450   2100  2700   600  1800 "$k" "$CURR_DIR"/q_07."$k".ply -noise_unit=$nu
	# 		./out/extract_query_spec.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   0 -3000     0    800  3300   200  3000 "$k" "$CURR_DIR"/q_08."$k".ply -noise_unit=$nu
	# 		./out/extract_query_spec.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_boardroom_"$i".ply 1  1500  3000 -10000 10000 -2000  -500 "$k" "$CURR_DIR"/q_09."$k".ply -noise_unit=$nu
	# 		./out/extract_query_spec.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_boardroom_"$i".ply 1 -1200  1000    700  2500   700 10000 "$k" "$CURR_DIR"/q_10."$k".ply -noise_unit=$nu
	# 	done
	# done

	# for i in 5_5 5_4 5_3 5_2 5_1 5 1_9 1_8 1_7 1_6 1_5
	# do
	# 	for k in {0..4}
	# 	do
	# 		./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_01."$k".ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_01."$k".ply.trans
	# 		./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_02."$k".ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_02."$k".ply.trans
	# 		./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_03."$k".ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_03."$k".ply.trans
	# 		./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_04."$k".ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_04."$k".ply.trans
	# 		./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_05."$k".ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_05."$k".ply.trans
	# 		./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_06."$k".ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_06."$k".ply.trans
	# 		./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_07."$k".ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_07."$k".ply.trans
	# 		./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_08."$k".ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_08."$k".ply.trans
	# 		./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_boardroom_"$i".ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_09."$k".ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_09."$k".ply.trans
	# 		./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_boardroom_"$i".ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_10."$k".ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_10."$k".ply.trans
	# 	done
	# done

	# for i in 5_4 5_3 5_2 5_1 1_9 1_8 1_7 1_6
	# do
	# 	./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_01.1.ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_01.1.ply.trans
	# done

	# for i in 5_5 1_5
	# do
	# 	./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_01.1.ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_01.1.ply.trans
	# 	./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_02.1.ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_02.1.ply.trans
	# 	./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_06.1.ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_06.1.ply.trans
	# 	./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_10.1.ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_10.1.ply.trans
	# 	./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_01.0.ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_01.0.ply.trans
	# 	./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_01.2.ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_01.2.ply.trans
	# 	./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_01.3.ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_01.3.ply.trans
	# 	./out/dye.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_01.4.ply "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_01.4.ply.trans
	# done

	# CURR_DIR="$DIR_QUERY"/indoor_scans_spec/comp_7/q_01_rn/
	# mkdir -p "$CURR_DIR"
	# for k in {1..4}; do
	# 	./out/extract_query_spec.out "$DIR_DB"/indoor_scans/comp_7/recon_bedroom_7.ply   0  1400  2400   2000  3000   500  1450 "$k" "$CURR_DIR"/q_01."$k".ply -noise_unit=l -num=10
	# done


	## for test
	# ./out/extract_query_spec.out "$DIR_DB"/indoor_scans/comp_1/recon_boardroom_1.ply 1  -1200 1000 700 2500 700 10000 0 test.ply

### build rstree index for query - indoor_scans (Spec)

	# for i in 5_5 5_4 5_3 5_2 5_1 1_9 1_8 1_7 1_6 1_5; do # 1 5 6 7; do # i->resolution
	# 	for j in {1..10}; do # j->query id
	# 		for k in {0..4}; do # k->noise level
	# 			if [ $j -lt 10 ]; then
	# 				./out/pre_index.out "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_0"$j"."$k".ply
	# 			else
	# 				./out/pre_index.out "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_"$j"."$k".ply
	# 			fi
	# 		done
	# 	done
	# done

	# for i in 7; do # i->resolution
	# 	for k in {1..4}; do # k->noise level
	# 		for j in {0..9}; do
	# 			./out/pre_index.out "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_01_rn/q_01."$k".ply."$j"
	# 		done
	# 	done
	# done

	# for i in 5_4 5_3 5_2 5_1 1_9 1_8 1_7 1_6
	# do
	# 	./out/pre_index.out "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_01.1.ply
	# done

	# for i in 5_5 1_5
	# do
	# 	./out/pre_index.out "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_01.1.ply
	# 	./out/pre_index.out "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_02.1.ply
	# 	./out/pre_index.out "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_06.1.ply
	# 	./out/pre_index.out "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_10.1.ply
	# 	./out/pre_index.out "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_01.0.ply
	# 	./out/pre_index.out "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_01.2.ply
	# 	./out/pre_index.out "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_01.3.ply
	# 	./out/pre_index.out "$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_01.4.ply
	# done

### extract queries from db - indoor_scans (Rand)

	## TODO: change back from alt to original
	# for i in 1; do # 5 6 7; do # i->resolution

	# 	# mkdir -p "$DIR_QUERY"/indoor_scans_rand/comp_"$i"/
	# 	mkdir -p "$DIR_QUERY"/indoor_scans_rand_alt/comp_"$i"/

	# 	if [ $i = 1 ]; then
	# 		nu="s"
	# 	elif [ $i = 5 ]; then
	# 		nu="m"
	# 	else
	# 		nu="l"
	# 	fi

	# 	for d in 1000; do # 1500 2000 2500 3000; do
	# 		mkdir -p "$DIR_QUERY"/indoor_scans_rand_alt/comp_"$i"/d_"$d"/
	# 		./out/extract_query_rand.out "$DIR_DB"/indoor_scans/comp_"$i"/ "$d" 10 "$DIR_QUERY"/indoor_scans_rand_alt/comp_"$i"/d_"$d" -noise_unit=$nu # pay attention, do not add '/' in the end of output path
	# 	done

	# done

### build rstree index for query - indoor_scans (Rand)

	## TODO: change back from alt to original
	# for i in 1; do # 5 6 7; do # i->resolution
	# 	for d in 1000; do # 1500 2000 2500 3000; do
	# 		for j in {0..9}; do # j->query id
	# 			for k in {0..4}; do # k->noise level
	# 				./out/pre_index.out "$DIR_QUERY"/indoor_scans_rand_alt/comp_"$i"/d_"$d"/q_"$j"."$k".ply
	# 			done
	# 		done
	# 	done
	# done







# mkdir -p "$DIR_OBJ_DB"/
# mkdir -p "$DIR_OBJ_QUERY"/
# spec_list=( 05842 06284 00942 06274 08719 ) # version 1: 214, 400, 601, 801, 1025; resp. id: 176, 231, 48, 228, 373
spec_list=( 07216 08279 09937 00074 06192 ) # version 2: 40, 80, 120, 161, 199; resp. id: 300, 340, 432, 13, 213
spec_rm_list=( 01392 06127 07216 08330 06144
			   01190 00546 01666 08279 00080
			   07697 08281 08627 09937 00007
			   06070 00074 00535 00578 08710
			   08765 00011 07090 06192 06112 )

### from raw to db - obj_scans

	# # copy raw objects to origin/, the selected density is 40~200 points per object, scale multiplied by 1000
	# mkdir -p "$DIR_OBJ_DB"/origin/
	# ./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_8/ "$DIR_OBJ_DB"/origin/ 1000 -batch

	# # expand the original 441 objects to 441 * 10000 objects, and save them to all/
	# mkdir -p "$DIR_OBJ_DB"/all/
	# ./out/obj_gen.out "$DIR_OBJ_DB"/origin/ 10000 "$DIR_OBJ_DB"/all/ -batch

	# # copy raw dense objects to z_dense/origin/, the selected density is 800~4000 points per object, approx. 20 times standard objects, scale multiplied by 1000
	# mkdir -p "$DIR_OBJ_DB"/z_dense/origin/
	# ./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_dense/ "$DIR_OBJ_DB"/z_dense/origin/ 1000 -batch

	# # similarly expand the dense original 441 objects to 441 * 100 objects
	# mkdir -p "$DIR_OBJ_DB"/z_dense/all/
	# ./out/obj_gen.out "$DIR_OBJ_DB"/z_dense/origin/ 100 "$DIR_OBJ_DB"/z_dense/all/ -batch

	# # copy raw dense different sampling rate objects, +1~+5 is density ratio 1.2~1.4~2, -1~-5 is density ratio 0.9~0.8~0.5
	# for i in {1..5}; do
	# 	mkdir -p "$DIR_OBJ_DB"/z_dense/origin+"$i"/
	# 	./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_dense+"$i"/ "$DIR_OBJ_DB"/z_dense/origin+"$i"/ 1000 -batch

	# 	mkdir -p "$DIR_OBJ_DB"/z_dense/origin-"$i"/
	# 	./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_dense-"$i"/ "$DIR_OBJ_DB"/z_dense/origin-"$i"/ 1000 -batch
	# done

### pre-process the spec queries

	# # make 50 objects that are very similar to each spec object, first one is exactly the spec object
	# # 50 is because later we need to add 50 similar objects together as the query answer
	# mkdir -p "$DIR_OBJ_DB"/sel_queries/
	# prop=0.1
	# sigma=1
	# num=49
	# for sp_id in "${spec_list[@]}"; do
	# 	./out/noise.out "$DIR_OBJ_DB"/origin/"$sp_id".ply "$prop" "$sigma" "$num" "$DIR_OBJ_DB"/sel_queries/"$sp_id"
	# done

	# # the same for rm sets, which is, for each spec, we pick some approx. same size query to diminish the error of single object
	# mkdir -p "$DIR_OBJ_DB"/sel_queries_rm/
	# prop=0.1
	# sigma=1
	# num=49
	# for sp_id in "${spec_rm_list[@]}"; do
	# 	./out/noise.out "$DIR_OBJ_DB"/origin/"$sp_id".ply "$prop" "$sigma" "$num" "$DIR_OBJ_DB"/sel_queries_rm/"$sp_id"
	# done

	# # the same for dense sets, but we only need 5 very similar objects
	# mkdir -p "$DIR_OBJ_DB"/z_dense/sel_queries/
	# prop=0.1
	# sigma=1
	# num=4
	# for sp_id in "${spec_list[@]}"; do
	# 	./out/noise.out "$DIR_OBJ_DB"/z_dense/origin/"$sp_id".ply "$prop" "$sigma" "$num" "$DIR_OBJ_DB"/z_dense/sel_queries/"$sp_id"
	# done

	# # the same for dense different sampling rate sets
	# for i in {1..5}; do
	# 	mkdir -p "$DIR_OBJ_DB"/z_dense/sel_queries+"$i"/
	# 	mkdir -p "$DIR_OBJ_DB"/z_dense/sel_queries-"$i"/
	# 	prop=0.1
	# 	sigma=1
	# 	num=4
	# 	for sp_id in "${spec_list[@]}"; do
	# 		./out/noise.out "$DIR_OBJ_DB"/z_dense/origin+"$i"/"$sp_id".ply "$prop" "$sigma" "$num" "$DIR_OBJ_DB"/z_dense/sel_queries+"$i"/"$sp_id"
	# 		./out/noise.out "$DIR_OBJ_DB"/z_dense/origin-"$i"/"$sp_id".ply "$prop" "$sigma" "$num" "$DIR_OBJ_DB"/z_dense/sel_queries-"$i"/"$sp_id"
	# 	done
	# done

	# # the same for rm sets
	# mkdir -p "$DIR_OBJ_DB"/z_dense/sel_queries_rm/
	# prop=0.1
	# sigma=1
	# num=4
	# for sp_id in "${spec_rm_list[@]}"; do
	# 	./out/noise.out "$DIR_OBJ_DB"/z_dense/origin/"$sp_id".ply "$prop" "$sigma" "$num" "$DIR_OBJ_DB"/z_dense/sel_queries_rm/"$sp_id"
	# done

	# for i in {1..5}; do
	# 	mkdir -p "$DIR_OBJ_DB"/z_dense/sel_queries_rm+"$i"/
	# 	mkdir -p "$DIR_OBJ_DB"/z_dense/sel_queries_rm-"$i"/
	# 	prop=0.1
	# 	sigma=1
	# 	num=4
	# 	for sp_id in "${spec_rm_list[@]}"; do
	# 		./out/noise.out "$DIR_OBJ_DB"/z_dense/origin+"$i"/"$sp_id".ply "$prop" "$sigma" "$num" "$DIR_OBJ_DB"/z_dense/sel_queries_rm+"$i"/"$sp_id"
	# 		./out/noise.out "$DIR_OBJ_DB"/z_dense/origin-"$i"/"$sp_id".ply "$prop" "$sigma" "$num" "$DIR_OBJ_DB"/z_dense/sel_queries_rm-"$i"/"$sp_id"
	# 	done
	# done

### make soft links to select / create varied no. of object scans

	# mkdir -p "$DIR_OBJ_DB"/1_sel_100i/
	# mkdir -p "$DIR_OBJ_DB"/2_sel_001k/
	# mkdir -p "$DIR_OBJ_DB"/3_sel_010k/
	# mkdir -p "$DIR_OBJ_DB"/4_sel_100k/
	# mkdir -p "$DIR_OBJ_DB"/5_sel_001m/

	# # 1: 95*1+5*(1+0), 2: 245*4+5*(2+2), 3: 395*25+5*(5+20), 4: 395*250+5*(10+240), 5: 395*2500+5*(50+2450)
	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/origin/ "$DIR_OBJ_DB"/all/ "$DIR_OBJ_DB"/sel_queries/ "$DIR_OBJ_DB" # need remake

	# mkdir -p "$DIR_OBJ_DB"/2_sel_001k_rm/

	# # 225*4+25*(2+2)
	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/origin/ "$DIR_OBJ_DB"/all/ "$DIR_OBJ_DB"/sel_queries_rm/ "$DIR_OBJ_DB" # need remake

	# mkdir -p "$DIR_OBJ_DB"/2_sel_001k_vk/

	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/origin/ "$DIR_OBJ_DB"/all/ "$DIR_OBJ_DB"/sel_queries/ "$DIR_OBJ_DB" # need remake

	# mkdir -p "$DIR_OBJ_DB"/z_dense/1_sel_100i/
	# mkdir -p "$DIR_OBJ_DB"/z_dense/2_sel_001k/
	# mkdir -p "$DIR_OBJ_DB"/z_dense/3_sel_010k/
	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/z_dense/origin/ "$DIR_OBJ_DB"/z_dense/all/ "$DIR_OBJ_DB"/z_dense/sel_queries/ "$DIR_OBJ_DB"/z_dense/ # need remake

	# mkdir -p "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/
	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/z_dense/origin/ "$DIR_OBJ_DB"/z_dense/all/ "$DIR_OBJ_DB"/z_dense/sel_queries_rm/ "$DIR_OBJ_DB"/z_dense/ # need remake

### build rstree index for db

	# ./out/obj_pre_index.out "$DIR_OBJ_DB"/1_sel_100i/
	# ./out/obj_pre_index.out "$DIR_OBJ_DB"/2_sel_001k/
	# ./out/obj_pre_index.out "$DIR_OBJ_DB"/3_sel_010k/
	# ./out/obj_pre_index.out "$DIR_OBJ_DB"/4_sel_100k/
	# for i in {0..9}; do
	# 	./out/obj_pre_index.out "$DIR_OBJ_DB"/5_sel_001m/ -part_i="$i"
	# done

	# ./out/obj_pre_index.out "$DIR_OBJ_DB"/2_sel_001k_rm/

	# ./out/obj_pre_index.out "$DIR_OBJ_DB"/2_sel_001k_vk/

	# ./out/obj_pre_index.out "$DIR_OBJ_DB"/z_dense/1_sel_100i/
	# ./out/obj_pre_index.out "$DIR_OBJ_DB"/z_dense/2_sel_001k/
	# ./out/obj_pre_index.out "$DIR_OBJ_DB"/z_dense/3_sel_010k/
	# ./out/obj_pre_index.out "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/
	
### extract queries from db - obj_scans (Spec)

	# # copy spec(s00) to query, q_01, ..., q_05
	# mkdir -p "$DIR_OBJ_QUERY"/spec/
	# mkdir -p "$DIR_OBJ_QUERY"/spec_dense/
	# for nl in {0..4}; do
	# 	for i in {0..4}; do
	# 		./out/obj_query_spec.out "$DIR_OBJ_DB"/sel_queries/"${spec_list[i]}"s00.ply         "$nl" "$DIR_OBJ_QUERY"/spec/q_0"$((i+1))"."$nl".ply
	# 		./out/obj_query_spec.out "$DIR_OBJ_DB"/z_dense/sel_queries/"${spec_list[i]}"s00.ply "$nl" "$DIR_OBJ_QUERY"/spec_dense/q_0"$((i+1))"."$nl".ply
	# 	done
	# done

	# # copy spec(s00)_rm to query, q_010, q_011, ..., q_014, q_020, ..., q_024, ..., q_050, ..., q_054
	# mkdir -p "$DIR_OBJ_QUERY"/spec_rm/
	# mkdir -p "$DIR_OBJ_QUERY"/spec_rm_dense/
	# for nl in 0 1; do
	# 	for i in {0..4}; do
	# 		for j in {0..4}; do
	# 			./out/obj_query_spec.out "$DIR_OBJ_DB"/sel_queries_rm/"${spec_rm_list[(($i*5+$j))]}"s00.ply         "$nl" "$DIR_OBJ_QUERY"/spec_rm/q_0"$((i+1))$j"."$nl".ply
	# 			./out/obj_query_spec.out "$DIR_OBJ_DB"/z_dense/sel_queries_rm/"${spec_rm_list[(($i*5+$j))]}"s00.ply "$nl" "$DIR_OBJ_QUERY"/spec_rm_dense/q_0"$((i+1))$j"."$nl".ply
	# 		done
	# 	done
	# done

	# for d in {1..5}; do
	# 	mkdir -p "$DIR_OBJ_QUERY"/spec_dense+"$d"/
	# 	mkdir -p "$DIR_OBJ_QUERY"/spec_dense-"$d"/
	# 	for nl in {0..4}; do
	# 		for i in {0..4}; do
	# 			./out/obj_query_spec.out "$DIR_OBJ_DB"/z_dense/sel_queries+"$d"/"${spec_list[i]}"s00.ply "$nl" "$DIR_OBJ_QUERY"/spec_dense+"$d"/q_0"$((i+1))"."$nl".ply
	# 			./out/obj_query_spec.out "$DIR_OBJ_DB"/z_dense/sel_queries-"$d"/"${spec_list[i]}"s00.ply "$nl" "$DIR_OBJ_QUERY"/spec_dense-"$d"/q_0"$((i+1))"."$nl".ply
	# 		done
	# 	done

	# 	mkdir -p "$DIR_OBJ_QUERY"/spec_rm_dense+"$d"/
	# 	mkdir -p "$DIR_OBJ_QUERY"/spec_rm_dense-"$d"/
	# 	for nl in 0 1; do
	# 		for i in {0..4}; do
	# 			for j in {0..4}; do
	# 				./out/obj_query_spec.out "$DIR_OBJ_DB"/z_dense/sel_queries_rm+"$d"/"${spec_rm_list[(($i*5+$j))]}"s00.ply "$nl" "$DIR_OBJ_QUERY"/spec_rm_dense+"$d"/q_0"$((i+1))$j"."$nl".ply
	# 				./out/obj_query_spec.out "$DIR_OBJ_DB"/z_dense/sel_queries_rm-"$d"/"${spec_rm_list[(($i*5+$j))]}"s00.ply "$nl" "$DIR_OBJ_QUERY"/spec_rm_dense-"$d"/q_0"$((i+1))$j"."$nl".ply
	# 			done
	# 		done
	# 	done
	# done

	# for nl in {0..4}; do
	# 	for i in {0..4}; do
	# 		./out/dye.out "$DIR_OBJ_DB"/z_dense/sel_queries/"${spec_list[i]}"s00.ply "$DIR_OBJ_QUERY"/spec_dense/q_0"$((i+1))"."$nl".ply \
	# 		"$DIR_OBJ_QUERY"/spec_dense/q_0"$((i+1))"."$nl".ply.trans
	# 	done
	# done
	# for nl in 0 1; do
	# 	for i in {0..4}; do
	# 		for j in {0..4}; do
	# 			./out/dye.out "$DIR_OBJ_DB"/z_dense/sel_queries_rm/"${spec_rm_list[(($i*5+$j))]}"s00.ply "$DIR_OBJ_QUERY"/spec_rm_dense/q_0"$((i+1))$j"."$nl".ply \
	# 			"$DIR_OBJ_QUERY"/spec_rm_dense/q_0"$((i+1))$j"."$nl".ply.trans
	# 		done
	# 	done
	# done

	# for d in {1..5}; do
	# 	for nl in {0..4}; do
	# 		for i in {0..4}; do
	# 			./out/dye.out "$DIR_OBJ_DB"/z_dense/sel_queries+"$d"/"${spec_list[i]}"s00.ply "$DIR_OBJ_QUERY"/spec_dense+"$d"/q_0"$((i+1))"."$nl".ply \
	# 			"$DIR_OBJ_QUERY"/spec_dense+"$d"/q_0"$((i+1))"."$nl".ply.trans
	# 			./out/dye.out "$DIR_OBJ_DB"/z_dense/sel_queries-"$d"/"${spec_list[i]}"s00.ply "$DIR_OBJ_QUERY"/spec_dense-"$d"/q_0"$((i+1))"."$nl".ply \
	# 			"$DIR_OBJ_QUERY"/spec_dense-"$d"/q_0"$((i+1))"."$nl".ply.trans
	# 		done
	# 	done
	# 	for nl in 0 1; do
	# 		for i in {0..4}; do
	# 			for j in {0..4}; do
	# 				./out/dye.out "$DIR_OBJ_DB"/z_dense/sel_queries_rm+"$d"/"${spec_rm_list[(($i*5+$j))]}"s00.ply "$DIR_OBJ_QUERY"/spec_rm_dense+"$d"/q_0"$((i+1))$j"."$nl".ply \
	# 				"$DIR_OBJ_QUERY"/spec_rm_dense+"$d"/q_0"$((i+1))$j"."$nl".ply.trans
	# 				./out/dye.out "$DIR_OBJ_DB"/z_dense/sel_queries_rm-"$d"/"${spec_rm_list[(($i*5+$j))]}"s00.ply "$DIR_OBJ_QUERY"/spec_rm_dense-"$d"/q_0"$((i+1))$j"."$nl".ply \
	# 				"$DIR_OBJ_QUERY"/spec_rm_dense-"$d"/q_0"$((i+1))$j"."$nl".ply.trans
	# 			done
	# 		done
	# 	done
	# done

	# for nl in {0..4}; do
	# 	for i in {0..4}; do
	# 		./out/dye.out "$DIR_OBJ_DB"/z_dense/sel_queries/"${spec_list[i]}"s00.ply "$DIR_OBJ_QUERY"/spec_dense_recon_dense/q_0"$((i+1))"."$nl".ply \
	# 		"$DIR_OBJ_QUERY"/spec_dense/q_0"$((i+1))"."$nl".ply.trans
	# 	done
	# done
	# for d in {1..5}; do
	# 	for nl in {0..4}; do
	# 		for i in {0..4}; do
	# 			./out/dye.out "$DIR_OBJ_DB"/z_dense/sel_queries+"$d"/"${spec_list[i]}"s00.ply "$DIR_OBJ_QUERY"/spec_dense_recon_dense+"$d"/q_0"$((i+1))"."$nl".ply \
	# 			"$DIR_OBJ_QUERY"/spec_dense+"$d"/q_0"$((i+1))"."$nl".ply.trans
	# 			./out/dye.out "$DIR_OBJ_DB"/z_dense/sel_queries-"$d"/"${spec_list[i]}"s00.ply "$DIR_OBJ_QUERY"/spec_dense_recon_dense-"$d"/q_0"$((i+1))"."$nl".ply \
	# 			"$DIR_OBJ_QUERY"/spec_dense-"$d"/q_0"$((i+1))"."$nl".ply.trans
	# 		done
	# 	done
	# done


### build rstree index for query - obj_scans (Spec)

	for j in {1..5}; do # j->query id
		for k in {0..4}; do # k->noise level
			# ./out/pre_index.out "$DIR_OBJ_QUERY"/spec/q_0"$j"."$k".ply
			# ./out/pre_index.out "$DIR_OBJ_QUERY"/spec_dense/q_0"$j"."$k".ply
			./out/pre_index.out "$DIR_OBJ_QUERY"/spec_dense_recon_dense/q_0"$j"."$k".ply
		done
	done

	for i in {1..5}; do
		for j in {0..4}; do
			for k in 0 1; do
				# ./out/pre_index.out "$DIR_OBJ_QUERY"/spec_rm/q_0"$i$j"."$k".ply
				# ./out/pre_index.out "$DIR_OBJ_QUERY"/spec_rm_dense/q_0"$i$j"."$k".ply
			done
		done
	done

	# for d in {1..5}; do
	# 	for j in {1..5}; do # j->query id
	# 		for k in {0..4}; do # k->noise level
	# 			./out/pre_index.out "$DIR_OBJ_QUERY"/spec_dense+"$d"/q_0"$j"."$k".ply
	# 			./out/pre_index.out "$DIR_OBJ_QUERY"/spec_dense-"$d"/q_0"$j"."$k".ply

	# 			./out/pre_index.out "$DIR_OBJ_QUERY"/spec_dense_recon_dense+"$d"/q_0"$j"."$k".ply
	# 			./out/pre_index.out "$DIR_OBJ_QUERY"/spec_dense_recon_dense-"$d"/q_0"$j"."$k".ply
	# 		done
	# 	done
	# 	for i in {1..5}; do
	# 		for j in {0..4}; do
	# 			for k in 0 1; do
	# 				./out/pre_index.out "$DIR_OBJ_QUERY"/spec_rm_dense+"$d"/q_0"$i$j"."$k".ply
	# 				./out/pre_index.out "$DIR_OBJ_QUERY"/spec_rm_dense-"$d"/q_0"$i$j"."$k".ply
	# 			done
	# 		done
	# 	done
	# done


### extract queries from db - obj_scans (Rand)

	# mkdir -p "$DIR_OBJ_QUERY"/rand/
	# mkdir -p "$DIR_OBJ_QUERY"/rand/rand_1/
	# mkdir -p "$DIR_OBJ_QUERY"/rand/rand_2/
	# mkdir -p "$DIR_OBJ_QUERY"/rand/rand_3/
	# mkdir -p "$DIR_OBJ_QUERY"/rand/rand_4/
	# mkdir -p "$DIR_OBJ_QUERY"/rand/rand_5/
	# ./out/obj_query_rand.out "$DIR_OBJ_DB"/1_sel_100i/ 10 "$DIR_OBJ_QUERY"/rand/rand_1
	# ./out/obj_query_rand.out "$DIR_OBJ_DB"/2_sel_001k/ 10 "$DIR_OBJ_QUERY"/rand/rand_2
	# ./out/obj_query_rand.out "$DIR_OBJ_DB"/3_sel_010k/ 10 "$DIR_OBJ_QUERY"/rand/rand_3
	# ./out/obj_query_rand.out "$DIR_OBJ_DB"/4_sel_100k/ 10 "$DIR_OBJ_QUERY"/rand/rand_4
	# ./out/obj_query_rand.out "$DIR_OBJ_DB"/5_sel_001m/ 10 "$DIR_OBJ_QUERY"/rand/rand_5

### build rstree index for query - obj_scans (Rand)

	# for i in 2; do #{1..5}; do
	# 	for j in {0..9}; do # j->query id
	# 		for k in {0..4}; do # k->noise level
	# 			./out/pre_index.out "$DIR_OBJ_QUERY"/rand/rand_"$i"/q_"$j"."$k".ply
	# 		done
	# 	done
	# done
