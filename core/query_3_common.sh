#!/bin/bash

source ../common/config/dir_loc.sh

OUT=out

rs_range=( 1_sel_100i 2_sel_001k 3_sel_010k 4_sel_100k 5_sel_001m )
m_range=( 0.001 0.002 0.003 0.004 0.005 0.02 0.04 0.06 0.08 0.1 )
# r_range=( 1000 ) # fix to 1000
r=1000
# a_range=( 015 ) # fix to 15
a=015
# e_range=( 24 ) # fix to 24
e=24
# MSE=( 001 032 060 100 120 140 ) # matched with nl, thus do not change: "${MSE[nl]}"

pcr_adv_alg=(
	prob6           # 0
	prob6_cpq       # 1
	donut_prob6     # 2
	donut_prob6_cpq # 3
	3nn_prob6       # 4
	3nn_prob6_cpq   # 5
	3lnn_prob6      # 6
	3nn_sim_prob6   # 7
	super4pcs_prob  # 8
	superg4pcs_prob # 9
	goicp           # 10
	det6_cpq        # 11
	donut_det6_cpq  # 12
	super4pcs_det   # 13
	superg4pcs_det  # 14
	prob6_cpq       # 15: with delta spec to 0.001...
	donut_prob6_cpq # 16: with delta spec to 0.001...
	3nn_prob6_cpq   # 17: with delta spec to 0.001...
	super4pcs_prob  # 18: with delta spec to 0.001...
	superg4pcs_prob # 19: with delta spec to 0.001...
	goicp           # 20: with delta spec to 0.001...
	tk_gt6_cpq      # 21
	tk_donut6_cpq   # 22
	tk_super4pcs    # 23
	tk_superg4pcs   # 24
	tk_gt6_cpq_prob    # 25
	tk_donut6_cpq_prob # 26
	tk_3nn6_cpq_prob   # 27
	tk_super4pcs_prob  # 28
	tk_superg4pcs_prob # 29
	tk_goicp           # 30
)
pcr_adv_idx=(
	gt gt
	donut donut
	3nn 3nn
	3lnn 3nn_sim
	non non non
	gt donut
	non non
	gt donut 3nn
	non non non
	gt donut
	non non
	gt donut 3nn
	non non non
)
