#!/bin/bash

source ../common/config/dir_loc.sh


## Create folders for indoor_scans

mkdir -p "$DIR_INDEX"/indoor_scans_3nn/

mkdir -p "$DIR_INDEX"/indoor_scans_3nn/comp_7/
mkdir -p "$DIR_INDEX"/indoor_scans_3nn/comp_6/
mkdir -p "$DIR_INDEX"/indoor_scans_3nn/comp_5/
mkdir -p "$DIR_INDEX"/indoor_scans_3nn/comp_1/

mkdir -p "$DIR_INDEX"/indoor_scans_3nn_sim/

mkdir -p "$DIR_INDEX"/indoor_scans_3nn_sim/comp_7/
mkdir -p "$DIR_INDEX"/indoor_scans_3nn_sim/comp_6/
mkdir -p "$DIR_INDEX"/indoor_scans_3nn_sim/comp_5/
mkdir -p "$DIR_INDEX"/indoor_scans_3nn_sim/comp_1/


# for r in 050 100 150 200 250 300 350 400 450 500
# do
# 	./out/grid_3nn.out "$DIR_DB"/indoor_scans/comp_7/ "$r" "$DIR_INDEX"/indoor_scans_3nn_sim/comp_7/comp_7."$r".grid -show_prog_bar -simple
# 	./out/index3.out   "$DIR_DB"/indoor_scans/comp_7/      "$DIR_INDEX"/indoor_scans_3nn_sim/comp_7/comp_7."$r".grid -show_prog_bar

# 	for a in 015 #030 045 060 075
# 	do
# 		./out/grid_3nn.out "$DIR_DB"/indoor_scans/comp_7/ "$r" "$a" "$DIR_INDEX"/indoor_scans_3nn/comp_7/comp_7."$r"."$a".grid -show_prog_bar
# 		./out/index3.out   "$DIR_DB"/indoor_scans/comp_7/           "$DIR_INDEX"/indoor_scans_3nn/comp_7/comp_7."$r"."$a".grid -show_prog_bar
# 	done
# done


# for r in 050 100 150 200 250 300 350 400 450 500
# do
# 	./out/grid_3nn.out "$DIR_DB"/indoor_scans/comp_6/ "$r" "$DIR_INDEX"/indoor_scans_3nn_sim/comp_6/comp_6."$r".grid -show_prog_bar -simple
# 	./out/index3.out   "$DIR_DB"/indoor_scans/comp_6/      "$DIR_INDEX"/indoor_scans_3nn_sim/comp_6/comp_6."$r".grid -show_prog_bar

# 	for a in 015 030 #045 060 075
# 	do
# 		./out/grid_3nn.out "$DIR_DB"/indoor_scans/comp_6/ "$r" "$a" "$DIR_INDEX"/indoor_scans_3nn/comp_6/comp_6."$r"."$a".grid -show_prog_bar
# 		./out/index3.out   "$DIR_DB"/indoor_scans/comp_6/           "$DIR_INDEX"/indoor_scans_3nn/comp_6/comp_6."$r"."$a".grid -show_prog_bar
# 	done
# done


# for r in 400 500 600 700 800 900 1000 #030 060 090 120 150 180 210 240 270 300
# do
# 	# ./out/grid_3nn.out   "$DIR_DB"/indoor_scans/comp_5/ "$r" "$DIR_INDEX"/indoor_scans_3nn_sim/comp_5/comp_5."$r".grid -show_prog_bar -simple
# 	# ./out/index6.out     "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_3nn_sim/comp_5/comp_5."$r".grid -show_prog_bar -simple
# 	# ./out/index3_3nn.out "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_3nn_sim/comp_5/comp_5."$r".grid -show_prog_bar -simple

# 	# ./out/reg_anal.out   "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_3nn_sim/comp_5/comp_5."$r".grid

# 	for a in 015 #030 #045 060 075
# 	do
# 		./out/grid_3nn.out "$DIR_DB"/indoor_scans/comp_5/ "$r" "$a" "$DIR_INDEX"/indoor_scans_3nn/comp_5/comp_5."$r"."$a".grid -show_prog_bar
# 		./out/reg_anal.out "$DIR_DB"/indoor_scans/comp_5/           "$DIR_INDEX"/indoor_scans_3nn/comp_5/comp_5."$r"."$a".grid
# 		./out/index6.out   "$DIR_DB"/indoor_scans/comp_5/           "$DIR_INDEX"/indoor_scans_3nn/comp_5/comp_5."$r"."$a".grid -show_prog_bar
# 		./out/index3.out   "$DIR_DB"/indoor_scans/comp_5/           "$DIR_INDEX"/indoor_scans_3nn/comp_5/comp_5."$r"."$a".grid -show_prog_bar
# 	done
# done


# for r in 040 080 120 160 200
# do
# 	for a in 015 030 045 060 075
# 	do
# 		./out_tmp/grid_3nn.out "$DIR_DB"/indoor_scans/comp_1/ "$r" "$a" "$DIR_INDEX"/indoor_scans_3nn/comp_1/comp_1."$r"."$a".grid -show_prog_bar
# 		./out_tmp/index6.out   "$DIR_DB"/indoor_scans/comp_1/           "$DIR_INDEX"/indoor_scans_3nn/comp_1/comp_1."$r"."$a".grid -show_prog_bar
# 		./out_tmp/index3_3nn.out "$DIR_DB"/indoor_scans/comp_1/           "$DIR_INDEX"/indoor_scans_3nn_sorted/comp_1/comp_1."$r"."$a".grid -show_prog_bar
# 	done
# done