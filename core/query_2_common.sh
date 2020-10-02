#!/bin/bash

OUT=out

rs_range=( 7 6 5 1 )
e_range=( 10 3.87 1.4 0.48 ) # do not change
m_range=( 0.001 0.002 0.003 0.004 0.005 0.01 0.015 0.02 )
r_range=( 200 ) #050 100 150 200 250 300 350 400 450 500 )
a_range=( 015 ) #030 )
t_range=( 030 015 )
d_range=( 1000 1500 2000 2500 3000 )
# MSE=( 001 032 060 100 120 140 ) # matched with nl, thus do not change: "${MSE[nl]}"

pcr_adv_alg=(
	prob3           # 0
	prob3_cpq       # 1
	donut_prob3     # 2
	donut_prob3_cpq # 3
	3nn_prob3       # 4
	3nn_prob3_cpq   # 5
	3lnn_prob3      # 6
	3nn_sim_prob3   # 7
	super4pcs_prob  # 8
	superg4pcs_prob # 9
	goicp           # 10
	det3_cpq        # 11
	donut_det3_cpq  # 12
	super4pcs_det   # 13
	superg4pcs_det  # 14
	prob3_cpq       # 15: with delta spec to 0.001...
	donut_prob3_cpq # 16: with delta spec to 0.001...
	3nn_prob3_cpq   # 17: with delta spec to 0.001...
	super4pcs_prob  # 18: with delta spec to 0.001...
	superg4pcs_prob # 19: with delta spec to 0.001...
	goicp           # 20: with delta spec to 0.001...
	tk_gt3_cpq      # 21
	tk_donut3_cpq   # 22
	tk_super4pcs    # 23
	tk_superg4pcs   # 24
	tk_gt3_cpq_prob    # 25
	tk_donut3_cpq_prob # 26
	tk_3nn3_cpq_prob   # 27
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

# function exec_al() {
# 	al_id=$1
# 	local -n r_range=$2
# 	local -n a_range=$3
# 	local -n t_range=$4
# 	local -n m_eid=$5
# 	local -n m_range=$6

# 	DB_F=$7
# 	DIR_INDEX=$8
# 	Q_PLY=$9
# 	CURR_AL=$10
# 	rs=$11
# 	e=$12
# 	OPT=$13

# 	if (( $al_id < 4 )); then

# 		## donut / gt
# 		for r in "${r_range[@]}"; do
# 			./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r".grid "$Q_PLY" "$e" "$OPT" \
# 			-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$e".dat
# 		done

# 	elif (( $al_id < 6 )); then
	
# 		## 3nn
# 		for r in "${r_range[@]}"; do
# 			for a in "${a_range[@]}"; do
# 				./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r"."$a".grid "$Q_PLY" "$e" "$OPT" \
# 				-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$a"."$e".dat
# 			done
# 		done

# 	elif [ $al_id -eq 6 ]; then

# 		## 3lnn
# 		for r in "${r_range[@]}"; do
# 			for t in "${t_range[@]}"; do
# 				for a in "${a_range[@]}"; do
# 					./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r"."$t"."$a".grid "$Q_PLY" "$e" "$OPT" \
# 					-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$t"."$a"."$e".dat
# 				done
# 			done
# 		done

# 	elif [ $al_id -eq 7 ]; then

# 		## 3nn-sim
# 		for r in "${r_range[@]}"; do
# 			./"$OUT"/query_3nn_prob3.out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r".grid "$Q_PLY" "$e" "$OPT" \
# 			-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$e".dat -simple
# 		done

# 	elif [ $al_id -eq 8 ] || [ $al_id -eq 9 ]; then

# 		## super4pcs / superg4pcs
# 		for r in "${r_range[@]}"; do
# 			./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" "$e" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$e".dat
# 		done

# 	elif [ $al_id -eq 10 ]; then

# 		## goicp
# 		for m_id in "${m_eid[@]}"; do
# 			./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" -delta="${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."${m_range[m_id]}".dat
# 		done

# 	elif [ $al_id -eq 11 ] || [ $al_id -eq 12 ]; then

# 		for r in "${r_range[@]}"; do
# 			for m_id in "${m_eid[@]}"; do
# 				./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r".grid "$Q_PLY" "${m_range[m_id]}" "$OPT" \
# 				-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."${m_range[m_id]}".dat
# 			done
# 		done

# 	elif [ $al_id -eq 13 ] || [ $al_id -eq 14 ]; then

# 		## super4pcs / superg4pcs
# 		for r in "${r_range[@]}"; do
# 			for m_id in "${m_eid[@]}"; do
# 				./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" "${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."${m_range[m_id]}".dat
# 			done
# 		done

# 	elif [ $al_id -eq 15 ] || [ $al_id -eq 16 ]; then

# 		## donut / gt
# 		for r in "${r_range[@]}"; do
# 			for m_id in "${m_eid[@]}"; do
# 				./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r".grid "$Q_PLY" "$e" "$OPT" \
# 				-delta="${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$e"."${m_range[m_id]}".dat
# 			done
# 		done

# 	elif [ $al_id -eq 17 ]; then
	
# 		## 3nn
# 		for r in "${r_range[@]}"; do
# 			for a in "${a_range[@]}"; do
# 				for m_id in "${m_eid[@]}"; do
# 					./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r"."$a".grid "$Q_PLY" "$e" "$OPT" \
# 					-delta="${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$a"."$e"."${m_range[m_id]}".dat
# 				done
# 			done
# 		done

# 	elif [ $al_id -eq 18 ] || [ $al_id -eq 19 ]; then

# 		## super4pcs / superg4pcs
# 		for r in "${r_range[@]}"; do
# 			for m_id in "${m_eid[@]}"; do
# 				./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" "$e" -delta="${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$e"."${m_range[m_id]}".dat
# 			done
# 		done

# 	fi

# }