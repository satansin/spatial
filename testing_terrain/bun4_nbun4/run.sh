#!/bin/bash

for i in {0..4}; do
	for j in {0..2}; do
		../matching ../data/bun_zipper_res4.ply noisy_output.ply $i $j
	done
done