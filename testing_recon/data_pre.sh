#!/bin/bash

PREFIX=/ssd/hliubs/recon_test

# ./../datapre/out/filter.out "$PREFIX"/sim_ply/ "$PREFIX"/sim_ply_1000/ 1000 -batch

for i in {1..4445}; do
	if [ $i -lt 10 ]; then
		./../datapre/out/pre_index.out "$PREFIX"/sim_ply_1000/000000"$i".ply
	elif [ $i -lt 100 ]; then
		./../datapre/out/pre_index.out "$PREFIX"/sim_ply_1000/00000"$i".ply
	elif [ $i -lt 1000 ]; then
		./../datapre/out/pre_index.out "$PREFIX"/sim_ply_1000/0000"$i".ply
	else
		./../datapre/out/pre_index.out "$PREFIX"/sim_ply_1000/000"$i".ply
	fi
done
