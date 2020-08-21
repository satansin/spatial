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



### from raw to db - obj_scans

	# mkdir -p "$DIR_OBJ_DB"/origin/
	# ./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_7/ "$DIR_OBJ_DB"/origin/ 1000 -batch

	# mkdir -p "$DIR_OBJ_DB"/all/
	# ./out/obj_gen.out "$DIR_OBJ_DB"/origin/ 10000 "$DIR_OBJ_DB"/all/ -batch

### build rstree index for db - obj_scans

	# for i in {0..440}; do
	# 	./out/pre_index.out "$DIR_OBJ_DB"/all/ "$i" -batch -nb=441
	# done

### pre-process the spec queries

	# mkdir -p "$DIR_OBJ_DB"/sel_queries/
	# prop=0.1
	# sigma=1
	# num=49
	# ./out/noise.out "$DIR_OBJ_DB"/origin/05842.ply "$prop" "$sigma" "$num" "$DIR_OBJ_DB"/sel_queries/05842
	# ./out/noise.out "$DIR_OBJ_DB"/origin/06284.ply "$prop" "$sigma" "$num" "$DIR_OBJ_DB"/sel_queries/06284
	# ./out/noise.out "$DIR_OBJ_DB"/origin/00942.ply "$prop" "$sigma" "$num" "$DIR_OBJ_DB"/sel_queries/00942
	# ./out/noise.out "$DIR_OBJ_DB"/origin/06274.ply "$prop" "$sigma" "$num" "$DIR_OBJ_DB"/sel_queries/06274
	# ./out/noise.out "$DIR_OBJ_DB"/origin/08719.ply "$prop" "$sigma" "$num" "$DIR_OBJ_DB"/sel_queries/08719

### build rstree for the pre-processed spec queries

	# for i in 05842 06284 00942 06274 08719; do
	# 	for j in {0..49}; do
	# 		if [ $j -lt 10 ]; then
	# 			./out/pre_index.out "$DIR_OBJ_DB"/sel_queries/"$i"s0"$j".ply
	# 		else
	# 			./out/pre_index.out "$DIR_OBJ_DB"/sel_queries/"$i"s"$j".ply
	# 		fi
	# 	done
	# done

### make soft links to select varied no. of object scans | TODO: there is a small bug causing the spec objects repeatedly appear, and total number of links are smaller than expected, not concern much

	# mkdir -p "$DIR_OBJ_DB"/sel_100i/
	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/origin/ "$DIR_OBJ_DB"/all/ "$DIR_OBJ_DB"/sel_queries/ "$DIR_OBJ_DB"/sel_100i/ 100 1 1

	# mkdir -p "$DIR_OBJ_DB"/sel_001k/
	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/origin/ "$DIR_OBJ_DB"/all/ "$DIR_OBJ_DB"/sel_queries/ "$DIR_OBJ_DB"/sel_001k/ 250 4 2

	# mkdir -p "$DIR_OBJ_DB"/sel_010k/
	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/origin/ "$DIR_OBJ_DB"/all/ "$DIR_OBJ_DB"/sel_queries/ "$DIR_OBJ_DB"/sel_010k/ 400 25 5

	# mkdir -p "$DIR_OBJ_DB"/sel_100k/
	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/origin/ "$DIR_OBJ_DB"/all/ "$DIR_OBJ_DB"/sel_queries/ "$DIR_OBJ_DB"/sel_100k/ 400 250 10

	# mkdir -p "$DIR_OBJ_DB"/sel_001m/
	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/origin/ "$DIR_OBJ_DB"/all/ "$DIR_OBJ_DB"/sel_queries/ "$DIR_OBJ_DB"/sel_001m/ 400 2500 50

### combine each DB for efficient I/O

	# ./out/obj_combine.out "$DIR_OBJ_DB"/sel_100i/ # 600K
	# ./out/obj_combine.out "$DIR_OBJ_DB"/sel_001k/ # 5.7M
	# ./out/obj_combine.out "$DIR_OBJ_DB"/sel_010k/ # 58M
	for i in {0..4}; do
		./out/obj_combine.out "$DIR_OBJ_DB"/sel_100k/ -num_parts=5 -part_i="$i" # 578M
	done
	for i in {0..49}; do
		./out/obj_combine.out "$DIR_OBJ_DB"/sel_001m/ -num_parts=50 -part_i="$i" # 110M-120M * 50 = 6G
	done

### extract queries from db - obj_scans

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


### build rstree index for query - obj_scans

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