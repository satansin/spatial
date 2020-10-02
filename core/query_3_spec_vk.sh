#!/bin/bash

source ../common/config/dir_loc.sh
source ./query_3_common.sh

rs_eid=( $1 ) # 0 1 2 3 4
qi_range=( $2 ) # 01 02 03 04 05
nl_range=( $3 ) # 0 1 2 3 4
pcr_adv_eid=( $4 )
k_eid=( $5 )


## object (sim) (spec)

CURR_DS="$DIR_OBJ_RESULT"/object_scans_spec
mkdir -p "$CURR_DS"

for rs in "${rs_eid[@]}"; do

	DB_F="$DIR_OBJ_DB"/"${rs_range[rs]}"_vk/
	CURR_DB="$CURR_DS"/"${rs_range[rs]}"_vk
	mkdir -p "$CURR_DB"

	for qi in "${qi_range[@]}"; do

		for nl in "${nl_range[@]}"; do

			Q_PLY="$DIR_OBJ_QUERY"/spec/q_"$qi"."$nl".ply
			CURR_NL="$CURR_DB"/q_"$qi"_"$nl"
			mkdir -p "$CURR_NL"

			for al_id in "${pcr_adv_eid[@]}"; do

				CURR_AL="$CURR_NL"/"${pcr_adv_alg[al_id]}"
				# rm -rf "$CURR_AL"
				mkdir -p "$CURR_AL"

				if [ $al_id -eq 21 ] || [ $al_id -eq 22 ]; then

					## top-k: donut / gt
					for k in "${k_eid[@]}"; do
						./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_OBJ_INDEX"/object_scans_"${pcr_adv_idx[al_id]}"/"${rs_range[rs]}"_vk/"${rs_range[rs]}"_vk."$r".grid "$Q_PLY" -small -k="$k" \
						-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r".k"$k".dat
					done

				elif [ $al_id -eq 23 ] || [ $al_id -eq 24 ]; then

					## top-k: super4pcs / superg4pcs
					for k in "${k_eid[@]}"; do
						./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}".k"$k".dat -num_parts=1 -k="$k"
					done

				elif [ $al_id -eq 25 ] || [ $al_id -eq 26 ]; then

					## top-k: donut / gt (prob)
					for k in "${k_eid[@]}"; do
						./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_OBJ_INDEX"/object_scans_"${pcr_adv_idx[al_id]}"/"${rs_range[rs]}"_vk/"${rs_range[rs]}"_vk."$r".grid "$Q_PLY" "$e" -small -k="$k" \
						-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$e".k"$k".dat
					done

				elif [ $al_id -eq 27 ]; then

					## top-k: 3nn (prob)
					for k in "${k_eid[@]}"; do
						./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_OBJ_INDEX"/object_scans_"${pcr_adv_idx[al_id]}"/"${rs_range[rs]}"_vk/"${rs_range[rs]}"_vk."$r"."$a".grid "$Q_PLY" "$e" -small -k="$k" \
						-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$a"."$e".k"$k".dat
					done

				elif [ $al_id -eq 28 ] || [ $al_id -eq 29 ]; then

					## top-k: super4pcs / superg4pcs (prob)
					for k in "${k_eid[@]}"; do
						./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" "$e" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$e".k"$k".dat -num_parts=1 -k="$k"
					done

				fi

			done

		done

	done

done

