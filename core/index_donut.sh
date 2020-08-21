#!/bin/bash

source ../common/config/dir_loc.sh


# mkdir -p "$DIR_INDEX"/indoor_scans_donut/


# mkdir -p "$DIR_INDEX"/indoor_scans_donut/comp_7/
# for r in 200 250 300
# do
# 	./out/grid_donut.out    "$DIR_DB"/indoor_scans/comp_7/ "$r" "$DIR_INDEX"/indoor_scans_donut/comp_7/comp_7."$r".grid -show_prog_bar -small
# 	./out/index3.out        "$DIR_DB"/indoor_scans/comp_7/      "$DIR_INDEX"/indoor_scans_donut/comp_7/comp_7."$r".grid -show_prog_bar
# 	./out/binary_helper.out "$DIR_DB"/indoor_scans/comp_7/      "$DIR_INDEX"/indoor_scans_donut/comp_7/comp_7."$r".grid
# done
# gdb -ex 'file ./out/binary_helper.out' -ex 'run /ssd/hliubs/3dor_exp/db/indoor_scans/comp_7/ /ssd/hliubs/3dor_exp/index/indoor_scans_donut/comp_7/comp_7.300.grid'


# mkdir -p "$DIR_INDEX"/indoor_scans_donut/comp_6/
# for r in 050 100 150 200 250 300 350 400 450 500
# do
# 	./out/grid_donut.out    "$DIR_DB"/indoor_scans/comp_6/ "$r" "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6."$r".grid -show_prog_bar -small
# 	./out/index3.out        "$DIR_DB"/indoor_scans/comp_6/      "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6."$r".grid -show_prog_bar
# 	./out/binary_helper.out "$DIR_DB"/indoor_scans/comp_6/      "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6."$r".grid
# done
# gdb -ex 'file ./out/grid_donut.out' -ex 'run /ssd/hliubs/3dor_exp/db/indoor_scans/comp_6/ 50 /ssd/hliubs/3dor_exp/index/indoor_scans_donut/comp_6/comp_6.050.grid'


# mkdir -p "$DIR_INDEX"/indoor_scans_donut/comp_5/
# for r in 200 250 300
# do
# 	./out/grid_donut.out    "$DIR_DB"/indoor_scans/comp_5/ "$r" "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5."$r".grid -show_prog_bar
# 	./out/index3.out        "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5."$r".grid -show_prog_bar
# 	./out/binary_helper.out "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5."$r".grid
# done


mkdir -p "$DIR_INDEX"/indoor_scans_donut/comp_1/
for r in 250 #100 #200 300
do
	./out/grid_donut.out    "$DIR_DB"/indoor_scans/comp_1/ "$r" "$DIR_INDEX"/indoor_scans_donut/comp_1/comp_1."$r".grid -show_prog_bar
	./out/index3.out        "$DIR_DB"/indoor_scans/comp_1/      "$DIR_INDEX"/indoor_scans_donut/comp_1/comp_1."$r".grid -show_prog_bar
	./out/binary_helper.out "$DIR_DB"/indoor_scans/comp_1/      "$DIR_INDEX"/indoor_scans_donut/comp_1/comp_1."$r".grid
done
# gdb -ex 'file ./out/index3.out' -ex 'run /ssd/hliubs/3dor_exp/db/indoor_scans/comp_1/ /ssd/hliubs/3dor_exp/index/indoor_scans_donut/comp_1/comp_1.300.grid'
