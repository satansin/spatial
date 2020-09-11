#!/bin/bash

source ../common/config/dir_loc.sh

OUT=out

rs_range=( 1_sel_100i 2_sel_001k 3_sel_010k 4_sel_100k 5_sel_001m )
rs_eid=( $1 ) # 0 1 2 3 4
qi_range=( $2 ) # 0 1 2 3 4 5 6 7 8 9
nl_range=( $3 ) # 0 1 2 3 4
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
)
pcr_adv_idx=(
	gt gt
	donut donut
	3nn 3nn
	3lnn 3nn_sim
)
pcr_adv_eid=( $4 )


## object (sim) (rand)

CURR_DS="$DIR_OBJ_RESULT"/object_scans_rand
mkdir -p "$CURR_DS"

for rs in "${rs_eid[@]}"; do

	DB_F="$DIR_OBJ_DB"/"${rs_range[rs]}"/
	CURR_DB="$CURR_DS"/"${rs_range[rs]}"
	mkdir -p "$CURR_DB"

	for qi in "${qi_range[@]}"; do

		for nl in "${nl_range[@]}"; do

			Q_PLY="$DIR_OBJ_QUERY"/rand/rand_"$((rs+1))"/q_"$qi"."$nl".ply
			CURR_NL="$CURR_DB"/q_"$qi"_"$nl"
			mkdir -p "$CURR_NL"

			for al_id in "${pcr_adv_eid[@]}"; do

				CURR_AL="$CURR_NL"/"${pcr_adv_alg[al_id]}"
				# rm -rf "$CURR_AL"
				mkdir -p "$CURR_AL"

				if [ $al_id -eq 0 ] || [ $al_id -eq 1 ] || [ $al_id -eq 2 ] || [ $al_id -eq 3 ]; then

					## donut / gt
					./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_OBJ_INDEX"/object_scans_"${pcr_adv_idx[al_id]}"/"${rs_range[rs]}"/"${rs_range[rs]}"."$r".grid "$Q_PLY" "$e" -small \
					-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$e".dat

				elif [ $al_id -eq 4 ] || [ $al_id -eq 5 ]; then
				
					## 3nn
					./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_OBJ_INDEX"/object_scans_"${pcr_adv_idx[al_id]}"/"${rs_range[rs]}"/"${rs_range[rs]}"."$r"."$a".grid "$Q_PLY" "$e" -small \
					-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$a"."$e".dat

				elif [ $al_id -eq 8 ] || [ $al_id -eq 9 ]; then

					## super4pcs / superg4pcs
					./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" "$e" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$e".dat -num_parts=1

				elif [ $al_id -eq 10 ]; then

					## goicp
					./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}".dat

				fi

			done

		done

	done

done

