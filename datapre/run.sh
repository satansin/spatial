#!/bin/bash

source ../common/config/dir_loc.sh

LD_LIBRARY_PATH=/usr/local/GNU/gsl/lib/
export LD_LIBRARY_PATH


### from raw to db - indoor_scans

	# # full resolution 0: not supported due to file too large
	# for i in {1..7} # i->resolution
	# do
	# 	mkdir -p "$DIR_DB"/indoor_scans/recon_bedroom_"$i"/
	# 	./out/filter.out "$DIR_RAW"/indoor_scans/recon_bedroom_"$i".ply "$DIR_DB"/indoor_scans/recon_bedroom_"$i"/recon_bedroom_"$i".ply 1000

	# 	mkdir -p "$DIR_DB"/indoor_scans/recon_boardroom_"$i"/
	# 	./out/filter.out "$DIR_RAW"/indoor_scans/recon_boardroom_"$i".ply "$DIR_DB"/indoor_scans/recon_boardroom_"$i"/recon_boardroom_"$i".ply 1000

	# 	mkdir -p "$DIR_DB"/indoor_scans/recon_loft_"$i"/
	# 	./out/filter.out "$DIR_RAW"/indoor_scans/recon_loft_"$i".ply "$DIR_DB"/indoor_scans/recon_loft_"$i"/recon_loft_"$i".ply 1000
	# done

### build rstree index for db - indoor_scans

	# for i in 1 5 6 7 # i->resolution
	# do
	# 	./out/pre_index.out "$DIR_DB"/indoor_scans/recon_bedroom_"$i"/recon_bedroom_"$i".ply
	# 	./out/pre_index.out "$DIR_DB"/indoor_scans/recon_boardroom_"$i"/recon_boardroom_"$i".ply
	# 	./out/pre_index.out "$DIR_DB"/indoor_scans/recon_loft_"$i"/recon_loft_"$i".ply
	# done

### copy recons to comp - indoor_scans

	# for i in {1..7} # i->resolution
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

	# for i in 1 5 6 7 # i->resolution
	# do
	# 	CURR_DIR="$DIR_QUERY"/indoor_scans_spec/comp_"$i"
	# 	mkdir -p "$CURR_DIR"

	# 	if [ $i = 1 ]; then
	# 		nu="s"
	# 	elif [ $i = 5 ]; then
	# 		nu="m"
	# 	else
	# 		nu="l"
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

	## for test
	# ./out/extract_query_spec.out "$DIR_DB"/indoor_scans/comp_1/recon_boardroom_1.ply 1  -1200 1000 700 2500 700 10000 0 test.ply

### build rstree index for query - indoor_scans (Spec)

	# for i in 1 5 6 7; do # i->resolution
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

### from raw to db - obj_scans

	# mkdir -p "$DIR_OBJ_DB"/origin/
	# ./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_8/ "$DIR_OBJ_DB"/origin/ 1000 -batch

	# mkdir -p "$DIR_OBJ_DB"/all/
	# ./out/obj_gen.out "$DIR_OBJ_DB"/origin/ 10000 "$DIR_OBJ_DB"/all/ -batch

### pre-process the spec queries

	# mkdir -p "$DIR_OBJ_DB"/sel_queries/
	# prop=0.1
	# sigma=1
	# num=49
	# for sp_id in "${spec_list[@]}"; do
	# 	./out/noise.out "$DIR_OBJ_DB"/origin/"$sp_id".ply "$prop" "$sigma" "$num" "$DIR_OBJ_DB"/sel_queries/"$sp_id"
	# done

### make soft links to select / create varied no. of object scans

	# mkdir -p "$DIR_OBJ_DB"/1_sel_100i/
	# mkdir -p "$DIR_OBJ_DB"/2_sel_001k/
	# mkdir -p "$DIR_OBJ_DB"/3_sel_010k/
	# mkdir -p "$DIR_OBJ_DB"/4_sel_100k/
	# mkdir -p "$DIR_OBJ_DB"/5_sel_001m/

	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/origin/ "$DIR_OBJ_DB"/all/ "$DIR_OBJ_DB"/sel_queries/ "$DIR_OBJ_DB" # need remake

### build rstree index for db

	# ./out/obj_pre_index.out "$DIR_OBJ_DB"/1_sel_100i/
	# ./out/obj_pre_index.out "$DIR_OBJ_DB"/2_sel_001k/
	# ./out/obj_pre_index.out "$DIR_OBJ_DB"/3_sel_010k/
	# ./out/obj_pre_index.out "$DIR_OBJ_DB"/4_sel_100k/
	# for i in {0..9}; do
	# 	./out/obj_pre_index.out "$DIR_OBJ_DB"/5_sel_001m/ -part_i="$i"
	# done
	
### extract queries from db - obj_scans (Spec)

	# mkdir -p "$DIR_OBJ_QUERY"/spec/
	# for nl in {0..4}; do
	# 	for i in {0..4}; do
	# 		./out/obj_query_spec.out "$DIR_OBJ_DB"/sel_queries/"${spec_list[i]}"s00.ply "$nl" "$DIR_OBJ_QUERY"/spec/q_0"$((i+1))"."$nl".ply
	# 	done
	# done

### build rstree index for query - obj_scans (Spec)

	# for j in {1..5}; do # j->query id
	# 	for k in {0..4}; do # k->noise level
	# 		./out/pre_index.out "$DIR_OBJ_QUERY"/spec/q_0"$j"."$k".ply
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

### build rstree index for query - obj_scans (Spec)

	# for i in 2; do #{1..5}; do
	# 	for j in {0..9}; do # j->query id
	# 		for k in {0..4}; do # k->noise level
	# 			./out/pre_index.out "$DIR_OBJ_QUERY"/rand/rand_"$i"/q_"$j"."$k".ply
	# 		done
	# 	done
	# done