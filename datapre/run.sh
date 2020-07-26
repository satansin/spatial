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

	# for i in {1..7} # i->resolution
	# do
	# 	./out/pre_index.out "$DIR_DB"/indoor_scans/recon_bedroom_"$i"/ -batch
	# 	./out/pre_index.out "$DIR_DB"/indoor_scans/recon_boardroom_"$i"/ -batch
	# 	./out/pre_index.out "$DIR_DB"/indoor_scans/recon_loft_"$i"/ -batch
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

### extract queries from db - indoor_scans (Spec) TODO???

	# for i in 1 5 6 7 # i->resolution
	# do
	# 	mkdir -p "$DIR_QUERY"/indoor_scans_spec/comp_"$i"/

	# 	for k in {0..5} # k->noise level
	# 	do
	# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply	 1400  2400  2000  3000   500  1450 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_01."$k".ply -noise_unit=$nu
	# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply -1220     0  2250  3500 -1850  -400 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_02."$k".ply -noise_unit=$nu
	# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  -200   800  3500  5000 -3000 -1900 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_03."$k".ply -noise_unit=$nu
	# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   700  3000  2000  4800 -4000 -2000 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_04."$k".ply -noise_unit=$nu
	# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  1500  3000   500  2000   700  3000 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_05."$k".ply -noise_unit=$nu
	# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  2800  3800  2400  4100 -2300  -800 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_06."$k".ply -noise_unit=$nu
	# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  1000  1450  2100  2700   600  1800 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_07."$k".ply -noise_unit=$nu
	# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply -3000     0   800  3300   200  3000 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_08."$k".ply -noise_unit=$nu
	# 	done
	# done

### build rstree index for query - indoor_scans

	# for i in 1 5 6 7 # i->resolution
	# do
	# 	for j in {1..8} # j->query id
	# 	do
	# 		for k in {0..5} # k->noise level
	# 		do
	# 			./out/pre_index.out "$DIR_QUERY"/indoor_scans/comp_"$i"/q_0"$j"."$k".ply
	# 		done
	# 	done
	# done

### extract queries from db - indoor_scans (proportional noise)

	# for i in 1 5 6 7 # i->resolution
	# do
	# 	mkdir -p "$DIR_QUERY"/indoor_scans_pro_noise/comp_"$i"/

	# 	if [ $i = 1 ]
	# 	then
	# 		nu="s"
	# 	elif [ $i = 5 ]
	# 	then
	# 		nu="m"
	# 	else
	# 		nu="l"
	# 	fi

	# 	for k in {0..5} # k->noise level
	# 	do
	# 		./out/extract_query_pro.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply	 1400  2400  2000  3000   500  1450 "$k" "$DIR_QUERY"/indoor_scans_pro_noise/comp_"$i"/q_01."$k".ply -noise_unit=$nu
	# 		./out/extract_query_pro.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply -1220     0  2250  3500 -1850  -400 "$k" "$DIR_QUERY"/indoor_scans_pro_noise/comp_"$i"/q_02."$k".ply -noise_unit=$nu
	# 		./out/extract_query_pro.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  -200   800  3500  5000 -3000 -1900 "$k" "$DIR_QUERY"/indoor_scans_pro_noise/comp_"$i"/q_03."$k".ply -noise_unit=$nu
	# 		./out/extract_query_pro.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   700  3000  2000  4800 -4000 -2000 "$k" "$DIR_QUERY"/indoor_scans_pro_noise/comp_"$i"/q_04."$k".ply -noise_unit=$nu
	# 		./out/extract_query_pro.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  1500  3000   500  2000   700  3000 "$k" "$DIR_QUERY"/indoor_scans_pro_noise/comp_"$i"/q_05."$k".ply -noise_unit=$nu
	# 		./out/extract_query_pro.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  2800  3800  2400  4100 -2300  -800 "$k" "$DIR_QUERY"/indoor_scans_pro_noise/comp_"$i"/q_06."$k".ply -noise_unit=$nu
	# 		./out/extract_query_pro.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  1000  1450  2100  2700   600  1800 "$k" "$DIR_QUERY"/indoor_scans_pro_noise/comp_"$i"/q_07."$k".ply -noise_unit=$nu
	# 		./out/extract_query_pro.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply -3000     0   800  3300   200  3000 "$k" "$DIR_QUERY"/indoor_scans_pro_noise/comp_"$i"/q_08."$k".ply -noise_unit=$nu
	# 	done
	# done

	# ./out/extract_query_pro.out "$DIR_DB"/indoor_scans/comp_5/recon_bedroom_5.ply	-10000 10000 -10000 10000 -10000 10000 0 "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_00.0.ply -noise_unit=m

### build rstree index for query - indoor_scans (proportional noise)

	# for i in 1 5 6 7 # i->resolution
	# do
	# 	for j in {1..8} # j->query id
	# 	do
	# 		for k in {0..5} # k->noise level
	# 		do
	# 			./out/pre_index.out "$DIR_QUERY"/indoor_scans_pro_noise/comp_"$i"/q_0"$j"."$k".ply
	# 		done
	# 	done
	# done

	# ./out/pre_index.out "$DIR_QUERY"/indoor_scans_pro_noise/comp_5/q_00.0.ply


### from raw to db - obj_scans

	# mkdir -p "$DIR_DB"/object_scans/origin/
	# ./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_7/ "$DIR_DB"/object_scans/origin/ 1000 -batch

	# mkdir -p "$DIR_DB"/object_scans/all/
	# ./out/obj_gen.out "$DIR_DB"/object_scans/origin/ 10000 "$DIR_DB"/object_scans/all/ -batch

### build rstree index for db - obj_scans

	# for i in {0..440}; do
	# 	./out/pre_index.out "$DIR_DB"/object_scans/all/ "$i" -batch -nb=441
	# done

### Make soft links to select varied no. of object scans

	# mkdir -p "$DIR_OBJ_DB"/sel_100i/
	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/origin/ "$DIR_OBJ_DB"/all/ "$DIR_OBJ_DB"/sel_100i/ 100 1

	# mkdir -p "$DIR_OBJ_DB"/sel_001k/
	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/origin/ "$DIR_OBJ_DB"/all/ "$DIR_OBJ_DB"/sel_001k/ 250 4

	# mkdir -p "$DIR_OBJ_DB"/sel_010k/
	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/origin/ "$DIR_OBJ_DB"/all/ "$DIR_OBJ_DB"/sel_010k/ 400 25

	# mkdir -p "$DIR_OBJ_DB"/sel_100k/
	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/origin/ "$DIR_OBJ_DB"/all/ "$DIR_OBJ_DB"/sel_100k/ 400 250

	# mkdir -p "$DIR_OBJ_DB"/sel_001m/
	# ./out/obj_batch_sel.out "$DIR_OBJ_DB"/origin/ "$DIR_OBJ_DB"/all/ "$DIR_OBJ_DB"/sel_001m/ 400 2500


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