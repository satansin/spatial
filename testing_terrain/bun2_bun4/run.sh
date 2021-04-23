#!/bin/bash

for i in {0..3}; do
	for j in {0..2}; do
		../matching ../data/bun_zipper_res2.ply ../data/bun_zipper_res4.ply $i $j
	done
done