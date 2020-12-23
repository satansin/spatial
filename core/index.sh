#!/bin/bash

source ../common/config/dir_loc.sh


# mkdir -p "$DIR_INDEX"/indoor_scans_gt/
# mkdir -p "$DIR_INDEX"/indoor_scans_donut/
# mkdir -p "$DIR_INDEX"/indoor_scans_3nn/
# mkdir -p "$DIR_INDEX"/indoor_scans_3nn_sim/
# mkdir -p "$DIR_INDEX"/indoor_scans_3lnn/

# mkdir -p "$DIR_INDEX"/indoor_scans_gt/comp_7/
# mkdir -p "$DIR_INDEX"/indoor_scans_donut/comp_7/
# mkdir -p "$DIR_INDEX"/indoor_scans_3nn/comp_7/
# for r in 200 250 300; do
# 	./out/grid_gt.out    "$DIR_DB"/indoor_scans/comp_7/ "$r" "$DIR_INDEX"/indoor_scans_gt/comp_7/comp_7."$r".grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_7/      "$DIR_INDEX"/indoor_scans_gt/comp_7/comp_7."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_DB"/indoor_scans/comp_7/ "$r" "$DIR_INDEX"/indoor_scans_donut/comp_7/comp_7."$r".grid -show_prog_bar -small
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_7/      "$DIR_INDEX"/indoor_scans_donut/comp_7/comp_7."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_DB"/indoor_scans/comp_7/ "$r" "$DIR_INDEX"/indoor_scans_3nn/comp_7/comp_7."$r".015.grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_7/      "$DIR_INDEX"/indoor_scans_3nn/comp_7/comp_7."$r".015.grid -show_prog_bar
# done
# gdb -ex 'file ./out/binary_helper.out' -ex 'run /ssd/hliubs/3dor_exp/db/indoor_scans/comp_7/ /ssd/hliubs/3dor_exp/index/indoor_scans_donut/comp_7/comp_7.300.grid'


# mkdir -p "$DIR_INDEX"/indoor_scans_gt/comp_6/
# mkdir -p "$DIR_INDEX"/indoor_scans_donut/comp_6/
# mkdir -p "$DIR_INDEX"/indoor_scans_3nn/comp_6/
# mkdir -p "$DIR_INDEX"/indoor_scans_3nn_sim/comp_6/
# mkdir -p "$DIR_INDEX"/indoor_scans_3lnn/comp_6/
# for r in 050 100 150 200 250 300 350 400 450 500; do
# 	./out/grid_gt.out    "$DIR_DB"/indoor_scans/comp_6/ "$r" "$DIR_INDEX"/indoor_scans_gt/comp_6/comp_6."$r".grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_6/      "$DIR_INDEX"/indoor_scans_gt/comp_6/comp_6."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_DB"/indoor_scans/comp_6/ "$r" "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6."$r".grid -show_prog_bar -small
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_6/      "$DIR_INDEX"/indoor_scans_donut/comp_6/comp_6."$r".grid -show_prog_bar

# 	for a in 015 030; do
# 		./out/grid_3nn.out  "$DIR_DB"/indoor_scans/comp_6/ "$r" "$DIR_INDEX"/indoor_scans_3nn/comp_6/comp_6."$r"."$a".grid -show_prog_bar -ang_min="$a"
# 		./out/index3.out    "$DIR_DB"/indoor_scans/comp_6/      "$DIR_INDEX"/indoor_scans_3nn/comp_6/comp_6."$r"."$a".grid -show_prog_bar
# 	done

# 	./out/grid_3nn.out   "$DIR_DB"/indoor_scans/comp_6/ "$r" "$DIR_INDEX"/indoor_scans_3nn_sim/comp_6/comp_6."$r".grid -show_prog_bar -simp
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_6/      "$DIR_INDEX"/indoor_scans_3nn_sim/comp_6/comp_6."$r".grid -show_prog_bar

# 	for t in 030 015; do
# 		./out/grid_3lnn.out "$DIR_DB"/indoor_scans/comp_6/ "(($r-$t))" "$DIR_INDEX"/indoor_scans_3nn/comp_6/comp_6."$r".015."$t".grid -show_prog_bar -ann_thn="$t" # r is actually ann_mid
# 		./out/index3.out    "$DIR_DB"/indoor_scans/comp_6/             "$DIR_INDEX"/indoor_scans_3nn/comp_6/comp_6."$r".015."$t".grid -show_prog_bar
# 	done
# done
# gdb -ex 'file ./out/grid_donut.out' -ex 'run /ssd/hliubs/3dor_exp/db/indoor_scans/comp_6/ 50 /ssd/hliubs/3dor_exp/index/indoor_scans_donut/comp_6/comp_6.050.grid'


# mkdir -p "$DIR_INDEX"/indoor_scans_gt/comp_5/
# mkdir -p "$DIR_INDEX"/indoor_scans_donut/comp_5/
# mkdir -p "$DIR_INDEX"/indoor_scans_3nn/comp_5/
# for r in 200 #250 300 #030 060 090 120 150 180 210 240 300 400 500 600 700 800 900 1000
# do
# 	./out/grid_gt.out    "$DIR_DB"/indoor_scans/comp_5/ "$r" "$DIR_INDEX"/indoor_scans_gt/comp_5/comp_5."$r".grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_gt/comp_5/comp_5."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_DB"/indoor_scans/comp_5/ "$r" "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5."$r".grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_donut/comp_5/comp_5."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_DB"/indoor_scans/comp_5/ "$r" "$DIR_INDEX"/indoor_scans_3nn/comp_5/comp_5."$r".015.grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_3nn/comp_5/comp_5."$r".015.grid -show_prog_bar
# done

# mkdir -p "$DIR_INDEX"/indoor_scans_donut_color/
# mkdir -p "$DIR_INDEX"/indoor_scans_donut_color/comp_5/
# for r in 200; do
# 	./out/grid_donut.out "$DIR_DB"/indoor_scans/comp_5/ "$r" "$DIR_INDEX"/indoor_scans_donut_color/comp_5/comp_5."$r".grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_donut_color/comp_5/comp_5."$r".grid -show_prog_bar
# done

# mkdir -p "$DIR_INDEX"/indoor_scans_gt_color/
# mkdir -p "$DIR_INDEX"/indoor_scans_gt_color/comp_5/
# for r in 200; do
# 	./out/grid_donut.out "$DIR_DB"/indoor_scans/comp_5/ "$r" "$DIR_INDEX"/indoor_scans_gt_color/comp_5/comp_5."$r".grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_gt_color/comp_5/comp_5."$r".grid -show_prog_bar
# done

# mkdir -p "$DIR_INDEX"/indoor_scans_3nn_color/
# mkdir -p "$DIR_INDEX"/indoor_scans_3nn_color/comp_5/
# for r in 200; do
# 	./out/grid_donut.out "$DIR_DB"/indoor_scans/comp_5/ "$r" "$DIR_INDEX"/indoor_scans_3nn_color/comp_5/comp_5."$r".015.grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_3nn_color/comp_5/comp_5."$r".015.grid -show_prog_bar
# done


# mkdir -p "$DIR_INDEX"/indoor_scans_gt/comp_5_5/
# mkdir -p "$DIR_INDEX"/indoor_scans_donut/comp_5_5/
# mkdir -p "$DIR_INDEX"/indoor_scans_3nn/comp_5_5/
# for r in 200; do
# 	./out/grid_gt.out    "$DIR_DB"/indoor_scans/comp_5/ "$r" "$DIR_INDEX"/indoor_scans_gt/comp_5/comp_5."$r".grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_gt/comp_5/comp_5."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_DB"/indoor_scans/comp_5_5/ "$r" "$DIR_INDEX"/indoor_scans_donut/comp_5_5/comp_5_5."$r".grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_5_5/      "$DIR_INDEX"/indoor_scans_donut/comp_5_5/comp_5_5."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_DB"/indoor_scans/comp_5/ "$r" "$DIR_INDEX"/indoor_scans_3nn/comp_5/comp_5."$r".015.grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_5/      "$DIR_INDEX"/indoor_scans_3nn/comp_5/comp_5."$r".015.grid -show_prog_bar
# done


# mkdir -p "$DIR_INDEX"/indoor_scans_gt/comp_1/
# mkdir -p "$DIR_INDEX"/indoor_scans_donut/comp_1/
# mkdir -p "$DIR_INDEX"/indoor_scans_3nn/comp_1/
# for r in 200 #250 #300; do
# 	./out/grid_gt.out    "$DIR_DB"/indoor_scans/comp_1/ "$r" "$DIR_INDEX"/indoor_scans_gt/comp_1/comp_1."$r".grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_1/      "$DIR_INDEX"/indoor_scans_gt/comp_1/comp_1."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_DB"/indoor_scans/comp_1/ "$r" "$DIR_INDEX"/indoor_scans_donut/comp_1/comp_1."$r".grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_1/      "$DIR_INDEX"/indoor_scans_donut/comp_1/comp_1."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_DB"/indoor_scans/comp_1/ "$r" "$DIR_INDEX"/indoor_scans_3nn/comp_1/comp_1."$r".015.grid -show_prog_bar
# 	./out/index3.out     "$DIR_DB"/indoor_scans/comp_1/      "$DIR_INDEX"/indoor_scans_3nn/comp_1/comp_1."$r".015.grid -show_prog_bar
# done
# gdb -ex 'file ./out/index3.out' -ex 'run /ssd/hliubs/3dor_exp/db/indoor_scans/comp_1/ /ssd/hliubs/3dor_exp/index/indoor_scans_donut/comp_1/comp_1.300.grid'






# mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn/

# mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt/1_sel_100i/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut/1_sel_100i/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn/1_sel_100i/
# for r in 1000
# do
# 	./out/grid_gt.out    "$DIR_OBJ_DB"/1_sel_100i/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt/1_sel_100i/1_sel_100i."$r".grid -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_gt/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_gt/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_OBJ_DB"/1_sel_100i/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut/1_sel_100i/1_sel_100i."$r".grid -small -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_donut/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_donut/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_OBJ_DB"/1_sel_100i/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn/1_sel_100i/1_sel_100i."$r".015.grid -num_parts=1 # a = 15 by default
# 	./out/index3.out     "$DIR_OBJ_DB"/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_3nn/1_sel_100i/1_sel_100i."$r".015.grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_3nn/1_sel_100i/1_sel_100i."$r".015.grid -show_prog_bar
# done

# mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt/2_sel_001k/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut/2_sel_001k/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn/2_sel_001k/
# for r in 200 400 600 800 1000 1200 1400 # TODO: 1400 some error
# do
# 	./out/grid_gt.out    "$DIR_OBJ_DB"/2_sel_001k/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt/2_sel_001k/2_sel_001k."$r".grid -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_gt/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_gt/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_OBJ_DB"/2_sel_001k/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut/2_sel_001k/2_sel_001k."$r".grid -small -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_donut/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_donut/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_OBJ_DB"/2_sel_001k/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn/2_sel_001k/2_sel_001k."$r".015.grid -num_parts=1 # a = 15 by default
# 	./out/index3.out     "$DIR_OBJ_DB"/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_3nn/2_sel_001k/2_sel_001k."$r".015.grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_3nn/2_sel_001k/2_sel_001k."$r".015.grid -show_prog_bar
# done

# mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt/2_sel_001k_rm/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut/2_sel_001k_rm/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn/2_sel_001k_rm/
# for r in 1000
# do
# 	./out/grid_gt.out    "$DIR_OBJ_DB"/2_sel_001k_rm/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt/2_sel_001k_rm/2_sel_001k_rm."$r".grid -num_parts=1
# 	./out/index6.out     "$DIR_OBJ_DB"/2_sel_001k_rm/      "$DIR_OBJ_INDEX"/object_scans_gt/2_sel_001k_rm/2_sel_001k_rm."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_OBJ_DB"/2_sel_001k_rm/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut/2_sel_001k_rm/2_sel_001k_rm."$r".grid -small -num_parts=1
# 	./out/index6.out     "$DIR_OBJ_DB"/2_sel_001k_rm/      "$DIR_OBJ_INDEX"/object_scans_donut/2_sel_001k_rm/2_sel_001k_rm."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_OBJ_DB"/2_sel_001k_rm/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn/2_sel_001k_rm/2_sel_001k_rm."$r".015.grid -num_parts=1 # a = 15 by default
# 	./out/index6.out     "$DIR_OBJ_DB"/2_sel_001k_rm/      "$DIR_OBJ_INDEX"/object_scans_3nn/2_sel_001k_rm/2_sel_001k_rm."$r".015.grid -show_prog_bar
# done

# mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt/2_sel_001k_vk/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut/2_sel_001k_vk/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn/2_sel_001k_vk/
# for r in 1000
# do
# 	./out/grid_gt.out    "$DIR_OBJ_DB"/2_sel_001k_vk/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt/2_sel_001k_vk/2_sel_001k_vk."$r".grid -num_parts=1
# 	./out/index6.out     "$DIR_OBJ_DB"/2_sel_001k_vk/      "$DIR_OBJ_INDEX"/object_scans_gt/2_sel_001k_vk/2_sel_001k_vk."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_OBJ_DB"/2_sel_001k_vk/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut/2_sel_001k_vk/2_sel_001k_vk."$r".grid -small -num_parts=1
# 	./out/index6.out     "$DIR_OBJ_DB"/2_sel_001k_vk/      "$DIR_OBJ_INDEX"/object_scans_donut/2_sel_001k_vk/2_sel_001k_vk."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_OBJ_DB"/2_sel_001k_vk/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn/2_sel_001k_vk/2_sel_001k_vk."$r".015.grid -num_parts=1 # a = 15 by default
# 	./out/index6.out     "$DIR_OBJ_DB"/2_sel_001k_vk/      "$DIR_OBJ_INDEX"/object_scans_3nn/2_sel_001k_vk/2_sel_001k_vk."$r".015.grid -show_prog_bar
# done

# mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt/3_sel_010k/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut/3_sel_010k/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn/3_sel_010k/
# for r in 1000
# do
# 	./out/grid_gt.out    "$DIR_OBJ_DB"/3_sel_010k/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt/3_sel_010k/3_sel_010k."$r".grid -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_gt/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_gt/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_OBJ_DB"/3_sel_010k/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut/3_sel_010k/3_sel_010k."$r".grid -small -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_donut/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_donut/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_OBJ_DB"/3_sel_010k/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn/3_sel_010k/3_sel_010k."$r".015.grid -num_parts=1 # a = 15 by default
# 	./out/index3.out     "$DIR_OBJ_DB"/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_3nn/3_sel_010k/3_sel_010k."$r".015.grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_3nn/3_sel_010k/3_sel_010k."$r".015.grid -show_prog_bar
# done

# mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt/4_sel_100k/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut/4_sel_100k/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn/4_sel_100k/
# for r in 1000
# do
# 	./out/grid_gt.out    "$DIR_OBJ_DB"/4_sel_100k/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt/4_sel_100k/4_sel_100k."$r".grid -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/4_sel_100k/      "$DIR_OBJ_INDEX"/object_scans_gt/4_sel_100k/4_sel_100k."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/4_sel_100k/      "$DIR_OBJ_INDEX"/object_scans_gt/4_sel_100k/4_sel_100k."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_OBJ_DB"/4_sel_100k/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut/4_sel_100k/4_sel_100k."$r".grid -small -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/4_sel_100k/      "$DIR_OBJ_INDEX"/object_scans_donut/4_sel_100k/4_sel_100k."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/4_sel_100k/      "$DIR_OBJ_INDEX"/object_scans_donut/4_sel_100k/4_sel_100k."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_OBJ_DB"/4_sel_100k/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn/4_sel_100k/4_sel_100k."$r".015.grid -num_parts=1 # a = 15 by default
# 	./out/index3.out     "$DIR_OBJ_DB"/4_sel_100k/      "$DIR_OBJ_INDEX"/object_scans_3nn/4_sel_100k/4_sel_100k."$r".015.grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/4_sel_100k/      "$DIR_OBJ_INDEX"/object_scans_3nn/4_sel_100k/4_sel_100k."$r".015.grid -show_prog_bar
# done

# mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt/5_sel_001m/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut/5_sel_001m/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn/5_sel_001m/
# for r in 1000
# do
# 	./out/grid_gt.out    "$DIR_OBJ_DB"/5_sel_001m/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt/5_sel_001m/5_sel_001m."$r".grid -num_parts=10
# 	./out/index3.out     "$DIR_OBJ_DB"/5_sel_001m/      "$DIR_OBJ_INDEX"/object_scans_gt/5_sel_001m/5_sel_001m."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/5_sel_001m/      "$DIR_OBJ_INDEX"/object_scans_gt/5_sel_001m/5_sel_001m."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_OBJ_DB"/5_sel_001m/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut/5_sel_001m/5_sel_001m."$r".grid -small -num_parts=10
# 	./out/index3.out     "$DIR_OBJ_DB"/5_sel_001m/      "$DIR_OBJ_INDEX"/object_scans_donut/5_sel_001m/5_sel_001m."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/5_sel_001m/      "$DIR_OBJ_INDEX"/object_scans_donut/5_sel_001m/5_sel_001m."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_OBJ_DB"/5_sel_001m/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn/5_sel_001m/5_sel_001m."$r".015.grid -num_parts=10 # a = 15 by default
# 	./out/index3.out     "$DIR_OBJ_DB"/5_sel_001m/      "$DIR_OBJ_INDEX"/object_scans_3nn/5_sel_001m/5_sel_001m."$r".015.grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/5_sel_001m/      "$DIR_OBJ_INDEX"/object_scans_3nn/5_sel_001m/5_sel_001m."$r".015.grid -show_prog_bar
# done


# mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt_dense/1_sel_100i/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut_dense/1_sel_100i/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn_dense/1_sel_100i/
# for r in 1000
# do
# 	./out/grid_gt.out    "$DIR_OBJ_DB"/z_dense/1_sel_100i/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt_dense/1_sel_100i/1_sel_100i."$r".grid -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/z_dense/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_gt_dense/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/z_dense/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_gt_dense/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_OBJ_DB"/z_dense/1_sel_100i/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut_dense/1_sel_100i/1_sel_100i."$r".grid -small -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/z_dense/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_donut_dense/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/z_dense/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_donut_dense/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_OBJ_DB"/z_dense/1_sel_100i/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn_dense/1_sel_100i/1_sel_100i."$r".grid -num_parts=1 # a = 15 by default
# 	./out/index3.out     "$DIR_OBJ_DB"/z_dense/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_3nn_dense/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/z_dense/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_3nn_dense/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar
# done

# mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt_dense/2_sel_001k/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut_dense/2_sel_001k/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn_dense/2_sel_001k/
# for r in 1000
# do
# 	./out/grid_gt.out    "$DIR_OBJ_DB"/z_dense/2_sel_001k/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt_dense/2_sel_001k/2_sel_001k."$r".grid -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_gt_dense/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_gt_dense/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_OBJ_DB"/z_dense/2_sel_001k/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut_dense/2_sel_001k/2_sel_001k."$r".grid -small -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_donut_dense/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_donut_dense/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_OBJ_DB"/z_dense/2_sel_001k/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn_dense/2_sel_001k/2_sel_001k."$r".grid -num_parts=1 # a = 15 by default
# 	./out/index3.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_3nn_dense/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_3nn_dense/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar
# done

# mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt_dense/2_sel_001k_rm/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut_dense/2_sel_001k_rm/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn_dense/2_sel_001k_rm/
# for r in 1000
# do
# 	./out/grid_gt.out    "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt_dense/2_sel_001k_rm/2_sel_001k_rm."$r".grid -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/      "$DIR_OBJ_INDEX"/object_scans_gt_dense/2_sel_001k_rm/2_sel_001k_rm."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/      "$DIR_OBJ_INDEX"/object_scans_gt_dense/2_sel_001k_rm/2_sel_001k_rm."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut_dense/2_sel_001k_rm/2_sel_001k_rm."$r".grid -small -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/      "$DIR_OBJ_INDEX"/object_scans_donut_dense/2_sel_001k_rm/2_sel_001k_rm."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/      "$DIR_OBJ_INDEX"/object_scans_donut_dense/2_sel_001k_rm/2_sel_001k_rm."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn_dense/2_sel_001k_rm/2_sel_001k_rm."$r".grid -num_parts=1 # a = 15 by default
# 	./out/index3.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/      "$DIR_OBJ_INDEX"/object_scans_3nn_dense/2_sel_001k_rm/2_sel_001k_rm."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/      "$DIR_OBJ_INDEX"/object_scans_3nn_dense/2_sel_001k_rm/2_sel_001k_rm."$r".grid -show_prog_bar
# done

# mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt_dense/3_sel_010k/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut_dense/3_sel_010k/
# mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn_dense/3_sel_010k/
# for r in 1000
# do
# 	./out/grid_gt.out    "$DIR_OBJ_DB"/z_dense/3_sel_010k/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt_dense/3_sel_010k/3_sel_010k."$r".grid -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/z_dense/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_gt_dense/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/z_dense/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_gt_dense/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar

# 	./out/grid_donut.out "$DIR_OBJ_DB"/z_dense/3_sel_010k/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut_dense/3_sel_010k/3_sel_010k."$r".grid -small -num_parts=1
# 	./out/index3.out     "$DIR_OBJ_DB"/z_dense/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_donut_dense/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/z_dense/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_donut_dense/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar

# 	./out/grid_3nn.out   "$DIR_OBJ_DB"/z_dense/3_sel_010k/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn_dense/3_sel_010k/3_sel_010k."$r".grid -num_parts=1 # a = 15 by default
# 	./out/index3.out     "$DIR_OBJ_DB"/z_dense/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_3nn_dense/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar
# 	./out/index6.out     "$DIR_OBJ_DB"/z_dense/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_3nn_dense/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar
# done


mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt_dense_color/1_sel_100i/
mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut_dense_color/1_sel_100i/
mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn_dense_color/1_sel_100i/
for r in 1000
do
	# ./out/grid_gt.out    "$DIR_OBJ_DB"/z_dense/1_sel_100i/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt_dense_color/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar -num_parts=1
	# ./out/index3.out     "$DIR_OBJ_DB"/z_dense/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_gt_dense_color/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar

	./out/grid_donut.out "$DIR_OBJ_DB"/z_dense/1_sel_100i/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut_dense_color/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar -num_parts=1
	./out/index3.out     "$DIR_OBJ_DB"/z_dense/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_donut_dense_color/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar

	# ./out/grid_3nn.out   "$DIR_OBJ_DB"/z_dense/1_sel_100i/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn_dense_color/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar -num_parts=1 # a = 15 by default
	# ./out/index3.out     "$DIR_OBJ_DB"/z_dense/1_sel_100i/      "$DIR_OBJ_INDEX"/object_scans_3nn_dense_color/1_sel_100i/1_sel_100i."$r".grid -show_prog_bar
done

mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt_dense_color/2_sel_001k/
mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut_dense_color/2_sel_001k/
mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn_dense_color/2_sel_001k/
for r in 1000
do
	# ./out/grid_gt.out    "$DIR_OBJ_DB"/z_dense/2_sel_001k/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt_dense_color/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar -num_parts=1
	# ./out/index3.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_gt_dense_color/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar

	./out/grid_donut.out "$DIR_OBJ_DB"/z_dense/2_sel_001k/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut_dense_color/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar -num_parts=1
	./out/index3.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_donut_dense_color/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar

	# ./out/grid_3nn.out   "$DIR_OBJ_DB"/z_dense/2_sel_001k/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn_dense_color/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar -num_parts=1 # a = 15 by default
	# ./out/index3.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k/      "$DIR_OBJ_INDEX"/object_scans_3nn_dense_color/2_sel_001k/2_sel_001k."$r".grid -show_prog_bar
done

mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt_dense_color/2_sel_001k_rm/
mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut_dense_color/2_sel_001k_rm/
mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn_dense_color/2_sel_001k_rm/
for r in 1000
do
	# ./out/grid_gt.out    "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt_dense_color/2_sel_001k_rm/2_sel_001k_rm."$r".grid -show_prog_bar -num_parts=1
	# ./out/index3.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/      "$DIR_OBJ_INDEX"/object_scans_gt_dense_color/2_sel_001k_rm/2_sel_001k_rm."$r".grid -show_prog_bar

	./out/grid_donut.out "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut_dense_color/2_sel_001k_rm/2_sel_001k_rm."$r".grid -show_prog_bar -num_parts=1
	./out/index3.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/      "$DIR_OBJ_INDEX"/object_scans_donut_dense_color/2_sel_001k_rm/2_sel_001k_rm."$r".grid -show_prog_bar

	# ./out/grid_3nn.out   "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn_dense_color/2_sel_001k_rm/2_sel_001k_rm."$r".grid -show_prog_bar -num_parts=1 # a = 15 by default
	# ./out/index3.out     "$DIR_OBJ_DB"/z_dense/2_sel_001k_rm/      "$DIR_OBJ_INDEX"/object_scans_3nn_dense_color/2_sel_001k_rm/2_sel_001k_rm."$r".grid -show_prog_bar
done

mkdir -p "$DIR_OBJ_INDEX"/object_scans_gt_dense_color/3_sel_010k/
mkdir -p "$DIR_OBJ_INDEX"/object_scans_donut_dense_color/3_sel_010k/
mkdir -p "$DIR_OBJ_INDEX"/object_scans_3nn_dense_color/3_sel_010k/
for r in 1000
do
	# ./out/grid_gt.out    "$DIR_OBJ_DB"/z_dense/3_sel_010k/ "$r" "$DIR_OBJ_INDEX"/object_scans_gt_dense_color/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar -num_parts=1
	# ./out/index3.out     "$DIR_OBJ_DB"/z_dense/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_gt_dense_color/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar

	# not run yet
	./out/grid_donut.out "$DIR_OBJ_DB"/z_dense/3_sel_010k/ "$r" "$DIR_OBJ_INDEX"/object_scans_donut_dense_color/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar -num_parts=1
	./out/index3.out     "$DIR_OBJ_DB"/z_dense/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_donut_dense_color/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar

	./out/grid_3nn.out   "$DIR_OBJ_DB"/z_dense/3_sel_010k/ "$r" "$DIR_OBJ_INDEX"/object_scans_3nn_dense_color/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar -num_parts=1 # a = 15 by default
	./out/index3.out     "$DIR_OBJ_DB"/z_dense/3_sel_010k/      "$DIR_OBJ_INDEX"/object_scans_3nn_dense_color/3_sel_010k/3_sel_010k."$r".grid -show_prog_bar
done