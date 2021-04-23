#!/bin/bash

PREFIX=/ssd/hliubs/recon_test

# mkdir -p "$PREFIX"/acc_ply
# mkdir -p "$PREFIX"/acc_index

# cp "$PREFIX"/sim_ply_1000/0000001.ply "$PREFIX"/acc_ply/
# cp "$PREFIX"/sim_ply_1000/0000001.ply.rst.0 "$PREFIX"/acc_ply/

# ./grid_recon.out   "$PREFIX"/acc_ply/0000001.ply 200 "$PREFIX"/acc_index/0000001.grid -show_prog_bar
# ./index3_recon.out "$PREFIX"/acc_ply/0000001.ply     "$PREFIX"/acc_index/0000001.grid -show_prog_bar

for i in {11..100}; do
	let "i_next= $i + 1"
	if [ $i -lt 10 ]; then
		fn=000000"$i"
	elif [ $i -lt 100 ]; then
		fn=00000"$i"
	elif [ $i -lt 1000 ]; then
		fn=0000"$i"
	else
		fn=000"$i"
	fi
	if [ $i_next -lt 10 ]; then
		fn_next=000000"$i_next"
	elif [ $i_next -lt 100 ]; then
		fn_next=00000"$i_next"
	elif [ $i_next -lt 1000 ]; then
		fn_next=0000"$i_next"
	else
		fn_next=000"$i_next"
	fi

	./query_recon.out "$PREFIX"/acc_ply/"$fn".ply "$PREFIX"/acc_index/"$fn".grid "$PREFIX"/sim_ply_1000/"$fn_next".ply 0.05 1 0.05 "$PREFIX"/acc_ply/"$fn_next".ply 0.1

	./../datapre/out/pre_index.out "$PREFIX"/acc_ply/"$fn_next".ply

	./grid_recon.out   "$PREFIX"/acc_ply/"$fn_next".ply 200 "$PREFIX"/acc_index/"$fn_next".grid -show_prog_bar
	./index3_recon.out "$PREFIX"/acc_ply/"$fn_next".ply     "$PREFIX"/acc_index/"$fn_next".grid -show_prog_bar

	rm "$PREFIX"/acc_index/"$fn".grid*
done