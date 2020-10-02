#!/bin/bash

source ../common/config/dir_loc.sh
source ./query_3_common.sh

rs_eid=( $1 ) # 0 1 2 3 4
qi_range=( $2 ) # 0 1 2 3 4 5 6 7 8 9
nl_range=( $3 ) # 0 1 2 3 4
pcr_adv_eid=( $4 )
m_eid=( $5 )


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

				elif [ $al_id -eq 10 ] || [ $al_id -eq 30 ]; then

					## goicp
					./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}".dat

				elif [ $al_id -eq 11 ] || [ $al_id -eq 12 ]; then

					for m_id in "${m_eid[@]}"; do
						./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_OBJ_INDEX"/object_scans_"${pcr_adv_idx[al_id]}"/"${rs_range[rs]}"/"${rs_range[rs]}"."$r".grid "$Q_PLY" "${m_range[m_id]}" -small \
							-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."${m_range[m_id]}".dat
					done

				elif [ $al_id -eq 13 ] || [ $al_id -eq 14 ]; then

					## super4pcs / superg4pcs
					for m_id in "${m_eid[@]}"; do
						./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" "${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."${m_range[m_id]}".dat -num_parts=1
					done

				elif [ $al_id -eq 15 ] || [ $al_id -eq 16 ]; then

					## donut / gt
					for m_id in "${m_eid[@]}"; do
						./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_OBJ_INDEX"/object_scans_"${pcr_adv_idx[al_id]}"/"${rs_range[rs]}"/"${rs_range[rs]}"."$r".grid "$Q_PLY" "$e" -small \
						-delta="${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$e"."${m_range[m_id]}".dat
					done

				elif [ $al_id -eq 17 ]; then
				
					## 3nn
					for m_id in "${m_eid[@]}"; do
						./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_OBJ_INDEX"/object_scans_"${pcr_adv_idx[al_id]}"/"${rs_range[rs]}"/"${rs_range[rs]}"."$r"."$a".grid "$Q_PLY" "$e" -small \
						-delta="${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$a"."$e"."${m_range[m_id]}".dat
					done

				elif [ $al_id -eq 18 ] || [ $al_id -eq 19 ]; then

					## super4pcs / superg4pcs
					for m_id in "${m_eid[@]}"; do
						./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" "$e" -delta="${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$e"."${m_range[m_id]}".dat -num_parts=1
					done

				elif [ $al_id -eq 20 ]; then

					## goicp
					for m_id in "${m_eid[@]}"; do
						./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" -delta="${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."${m_range[m_id]}".dat
					done

				elif [ $al_id -eq 21 ] || [ $al_id -eq 22 ]; then

					## top-k: donut / gt
					./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_OBJ_INDEX"/object_scans_"${pcr_adv_idx[al_id]}"/"${rs_range[rs]}"/"${rs_range[rs]}"."$r".grid "$Q_PLY" -small \
					-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r".dat

				elif [ $al_id -eq 23 ] || [ $al_id -eq 24 ]; then

					## top-k: super4pcs / superg4pcs
					./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}".dat -num_parts=1

				elif [ $al_id -eq 25 ] || [ $al_id -eq 26 ]; then

					## top-k: donut / gt (prob)
					./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_OBJ_INDEX"/object_scans_"${pcr_adv_idx[al_id]}"/"${rs_range[rs]}"/"${rs_range[rs]}"."$r".grid "$Q_PLY" "$e" -small \
					-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$e".dat

				elif [ $al_id -eq 27 ]; then

					## top-k: 3nn (prob)
					./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_OBJ_INDEX"/object_scans_"${pcr_adv_idx[al_id]}"/"${rs_range[rs]}"/"${rs_range[rs]}"."$r"."$a".grid "$Q_PLY" "$e" -small \
					-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$a"."$e".dat

				elif [ $al_id -eq 28 ] || [ $al_id -eq 29 ]; then

					## top-k: super4pcs / superg4pcs (prob)
					./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" "$e" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$e".dat -num_parts=1

				fi

			done

		done

	done

done

