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


### copy recons to comp

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


### from raw to db - obj_scans

# mkdir -p "$DIR_DB"/obj_scans/obj_scans_1_full/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_2_full/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_3_full/
# mkdir -p "$DIR_DB"/obj_scans/obj_scans_4_full/
# ./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_1/ "$DIR_DB"/obj_scans/obj_scans_1_full/ 1000 -batch
# ./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_2/ "$DIR_DB"/obj_scans/obj_scans_2_full/ 1000 -batch
# ./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_3/ "$DIR_DB"/obj_scans/obj_scans_3_full/ 1000 -batch
# ./out/filter.out "$DIR_RAW"/obj_scans/obj_scans_4/ "$DIR_DB"/obj_scans/obj_scans_4_full/ 1000 -batch


### build rstree index for db - obj_scans

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



### extract queries from db - indoor_scans

# for i in {1..4} # i->resolution
# do
# 	mkdir -p "$DIR_QUERY"/indoor_scans/comp_"$i"/

# 	for j in {1..8} # j->query id
# 	do
# 		mkdir -p "$DIR_QUERY"/indoor_scans/comp_"$i"/q_0"$j"/
# 	done

# 	for k in {0..4} # k->noisy level
# 	do
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply	 1400  2400  2000  3000   500  1450 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_01/q_01."$k".ply
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply -1220     0  2250  3500 -1850  -400 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_02/q_02."$k".ply
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  -200   800  3500  5000 -3000 -1900 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_03/q_03."$k".ply
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   700  3000  2000  4800 -4000 -2000 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_04/q_04."$k".ply
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  1500  3000   500  2000   700  3000 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_05/q_05."$k".ply
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  2800  3800  2400  4100 -2300  -800 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_06/q_06."$k".ply
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  1000  1450  2100  2700   600  1800 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_07/q_07."$k".ply
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply -3000     0   800  3300   200  3000 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_08/q_08."$k".ply
# 	done
# done

# for i in {5..7} # i->resolution
# do
# 	mkdir -p "$DIR_QUERY"/indoor_scans/comp_"$i"/

# 	for j in {1..8} # j->query id
# 	do
# 		mkdir -p "$DIR_QUERY"/indoor_scans/comp_"$i"/q_0"$j"/
# 	done

# 	for k in {0..4} # k->noisy level
# 	do
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply	 1400  2400  2000  3000   500  1450 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_01/q_01."$k".ply
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply -1220     0  2250  3500 -1850  -400 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_02/q_02."$k".ply
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  -200   800  3500  5000 -3000 -1900 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_03/q_03."$k".ply
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply   700  3000  2000  4800 -4000 -2000 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_04/q_04."$k".ply
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  1500  3000   500  2000   700  3000 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_05/q_05."$k".ply
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  2800  3800  2400  4100 -2300  -800 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_06/q_06."$k".ply
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply  1000  1450  2100  2700   600  1800 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_07/q_07."$k".ply
# 		./out/extract_query.out "$DIR_DB"/indoor_scans/comp_"$i"/recon_bedroom_"$i".ply -3000     0   800  3300   200  3000 "$k" "$DIR_QUERY"/indoor_scans/comp_"$i"/q_08/q_08."$k".ply
# 	done
# done


### build rstree index for query - indoor_scans

# for i in {1..7} # i->resolution
# do
# 	for j in {1..8} # j->query id
# 	do
# 		for k in {0..4}
# 		do
# 			./out/pre_index.out "$DIR_QUERY"/indoor_scans/comp_"$i"/q_0"$j"/q_0"$j"."$k".ply
# 		done
# 	done
# done


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