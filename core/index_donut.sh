#!/bin/bash

source ../common/config/dir_loc.sh


## Create folders for indoor_scans

mkdir -p "$DIR_INDEX"/indoor_scans_donut/

mkdir -p "$DIR_INDEX"/indoor_scans_donut/comp_7/
mkdir -p "$DIR_INDEX"/indoor_scans_donut/comp_6/
mkdir -p "$DIR_INDEX"/indoor_scans_donut/comp_5/
mkdir -p "$DIR_INDEX"/indoor_scans_donut/comp_1/

# mkdir -p "$DIR_INDEX"/indoor_scans_donut_sorted/

# mkdir -p "$DIR_INDEX"/indoor_scans_donut_sorted/comp_7/
# mkdir -p "$DIR_INDEX"/indoor_scans_donut_sorted/comp_6/
# mkdir -p "$DIR_INDEX"/indoor_scans_donut_sorted/comp_5/
# mkdir -p "$DIR_INDEX"/indoor_scans_donut_sorted/comp_1/


for r in 300
do
	./out_tmp/grid_donut.out "$DIR_DB"/indoor_scans/comp_1/ "$r" "$DIR_INDEX"/indoor_scans_donut/comp_1/comp_1."$r".grid -show_prog_bar
	# ./out/index3.out     "$DIR_DB"/indoor_scans/comp_1/      "$DIR_INDEX"/indoor_scans_donut/comp_1/comp_1."$r".grid -show_prog_bar
done

# for r in 200 250 300
# do
# 	./out/grid_donut.out "$DIR_DB"/indoor_scans/comp_7/ "$r" "$DIR_INDEX"/indoor_scans_donut/comp_7/comp_7."$r".grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_7/      "$DIR_INDEX"/indoor_scans_donut/comp_7/comp_7."$r".grid -show_prog_bar
# done

# for r in 050 #100 150 200 250 300 350 400 450 500
# do
# 	./out/grid_donut.out "$DIR_DB"/indoor_scans/comp_6/ "$r" "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6."$r".grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_6/      "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6."$r".grid -show_prog_bar
# done
# gdb -ex 'file ./out/grid_donut.out' -ex 'run /ssd/hliubs/3dor_exp/db/indoor_scans/comp_6/ 50 /ssd/hliubs/3dor_exp/index/indoor_scans_donut/comp_6/comp_6.050.grid'

# for r in 030 060 090 150 180 210 240 #120 300 400 500 600 700 800 900 1000
# do
# 	./out/grid_donut.out  "$DIR_DB"/indoor_scans/comp_5/ "$r" "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5."$r".grid -show_prog_bar
# 	./out/reg_anal.out    "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5."$r".grid
# 	./out/index6.out      "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5."$r".grid -show_prog_bar
# 	./out/index3.out      "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5."$r".grid -show_prog_bar

# 	# ./out/grid_donut.out  "$DIR_DB"/indoor_scans/comp_5/ "$r" "$DIR_INDEX"/indoor_scans_donut_sorted/comp_5/comp_5."$r".grid -show_prog_bar -sort_entry
# 	# ./out/index3.out      "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_donut_sorted/comp_5/comp_5."$r".grid -show_prog_bar
# done


# for r in 040 080 120 160 200
# do
# 	for a in 015 030 045 060 075
# 	do
# 		./out_tmp/grid_3nn.out "$DIR_DB"/indoor_scans/comp_1/ "$r" "$a" "$DIR_INDEX"/indoor_scans_3nn/comp_1/comp_1."$r"."$a".grid -show_prog_bar
# 		./out_tmp/index6.out   "$DIR_DB"/indoor_scans/comp_1/           "$DIR_INDEX"/indoor_scans_3nn/comp_1/comp_1."$r"."$a".grid -show_prog_bar

# 		./out_tmp/grid_3nn.out   "$DIR_DB"/indoor_scans/comp_1/ "$r" "$a" "$DIR_INDEX"/indoor_scans_3nn_sorted/comp_1/comp_1."$r"."$a".grid -show_prog_bar -sort_entry
# 		./out_tmp/index3_3nn.out "$DIR_DB"/indoor_scans/comp_1/           "$DIR_INDEX"/indoor_scans_3nn_sorted/comp_1/comp_1."$r"."$a".grid -show_prog_bar
# 	done
# done