#!/bin/bash

for i in 4; do #{0..3}; do
	for j in {0..2}; do
		../matching ../data/bun_zipper_res3.ply ../data/bun_zipper_res2.ply $i $j
	done
done