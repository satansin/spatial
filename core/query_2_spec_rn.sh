#!/bin/bash

source ../common/config/dir_loc.sh
source ./query_2_common.sh

rs_eid=( $1 ) # 0 1 2 3
qi_range=( $2 ) # 01 02 06 08 10
nl_range=( $3 ) # 0 1 2 3 4
pcr_adv_eid=( $4 )
q_rn_range=( $5 )
m_eid=( $6 )


## indoor (sim) (spec)

CURR_DS="$DIR_RESULT"/indoor_scans_spec
mkdir -p "$CURR_DS"

for rs_id in "${rs_eid[@]}"; do

	rs="${rs_range[rs_id]}"
	e="${e_range[rs_id]}"

	if (( $rs > 5 )); then
		OPT="-small"
	else
		OPT=""
	fi

	DB_F="$DIR_DB"/indoor_scans/comp_"$rs"/
	CURR_DB="$CURR_DS"/comp_"$rs"
	mkdir -p "$CURR_DB"

	for qi in "${qi_range[@]}"; do

		for nl in "${nl_range[@]}"; do

			for rn in "${q_rn_range[@]}"; do

				Q_PLY="$DIR_QUERY"/indoor_scans_spec/comp_"$rs"/q_01_rn/q_"$qi"."$nl".ply."$rn"
				CURR_NL="$CURR_DB"/q_"$qi"_"$nl"_"$rn"
				mkdir -p "$CURR_NL"

				for al_id in "${pcr_adv_eid[@]}"; do

					CURR_AL="$CURR_NL"/"${pcr_adv_alg[al_id]}"
					# rm -rf "$CURR_AL"
					mkdir -p "$CURR_AL"

					## TODO
					# exec_al "$al_id" r_range "${t_range[@]}" "${m_eid[@]}" "${m_range[@]}" "$DB_F" "$DIR_INDEX" "$Q_PLY" "$CURR_AL" "$rs" "$e" "$OPT"

					if (( $al_id < 4 )); then

						## donut / gt
						for r in "${r_range[@]}"; do
							./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r".grid "$Q_PLY" "$e" "$OPT" \
							-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$e".dat
						done

					elif (( $al_id < 6 )); then
					
						## 3nn
						for r in "${r_range[@]}"; do
							for a in "${a_range[@]}"; do
								./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r"."$a".grid "$Q_PLY" "$e" "$OPT" \
								-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$a"."$e".dat
							done
						done

					elif [ $al_id -eq 6 ]; then

						## 3lnn
						for r in "${r_range[@]}"; do
							for t in "${t_range[@]}"; do
								for a in "${a_range[@]}"; do
									./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r"."$t"."$a".grid "$Q_PLY" "$e" "$OPT" \
									-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$t"."$a"."$e".dat
								done
							done
						done

					elif [ $al_id -eq 7 ]; then

						## 3nn-sim
						for r in "${r_range[@]}"; do
							./"$OUT"/query_3nn_prob3.out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r".grid "$Q_PLY" "$e" "$OPT" \
							-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$e".dat -simple
						done

					elif [ $al_id -eq 8 ] || [ $al_id -eq 9 ]; then

						## super4pcs / superg4pcs
						for r in "${r_range[@]}"; do
							./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" "$e" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$e".dat
						done

					elif [ $al_id -eq 10 ] || [ $al_id -eq 30 ]; then

						## goicp
						./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}".dat

					elif [ $al_id -eq 11 ] || [ $al_id -eq 12 ]; then

						for r in "${r_range[@]}"; do
							for m_id in "${m_eid[@]}"; do
								./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r".grid "$Q_PLY" "${m_range[m_id]}" "$OPT" \
								-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."${m_range[m_id]}".dat
							done
						done

					elif [ $al_id -eq 13 ] || [ $al_id -eq 14 ]; then

						## super4pcs / superg4pcs
						for r in "${r_range[@]}"; do
							for m_id in "${m_eid[@]}"; do
								./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" "${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."${m_range[m_id]}".dat
							done
						done

					elif [ $al_id -eq 15 ] || [ $al_id -eq 16 ]; then

						## donut / gt
						for r in "${r_range[@]}"; do
							for m_id in "${m_eid[@]}"; do
								./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r".grid "$Q_PLY" "$e" "$OPT" \
								-delta="${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$e"."${m_range[m_id]}".dat
							done
						done

					elif [ $al_id -eq 17 ]; then
					
						## 3nn
						for r in "${r_range[@]}"; do
							for a in "${a_range[@]}"; do
								for m_id in "${m_eid[@]}"; do
									./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r"."$a".grid "$Q_PLY" "$e" "$OPT" \
									-delta="${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$a"."$e"."${m_range[m_id]}".dat
								done
							done
						done

					elif [ $al_id -eq 18 ] || [ $al_id -eq 19 ]; then

						## super4pcs / superg4pcs
						for r in "${r_range[@]}"; do
							for m_id in "${m_eid[@]}"; do
								./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" "$e" -delta="${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$e"."${m_range[m_id]}".dat
							done
						done

					elif [ $al_id -eq 20 ]; then

						## goicp with delta
						for m_id in "${m_eid[@]}"; do
							./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" -delta="${m_range[m_id]}" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."${m_range[m_id]}".dat
						done

					elif [ $al_id -eq 21 ] || [ $al_id -eq 22 ]; then

						## top-k: donut / gt (det)
						for r in "${r_range[@]}"; do
							./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r".grid "$Q_PLY" "$OPT" \
							-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r".dat
						done

					elif [ $al_id -eq 23 ] || [ $al_id -eq 24 ]; then

						## top-k: super4pcs / superg4pcs (det)
						./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}".dat

					elif [ $al_id -eq 25 ] || [ $al_id -eq 26 ]; then

						## top-k: donut / gt (prob)
						for r in "${r_range[@]}"; do
							./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r".grid "$Q_PLY" "$e" "$OPT" \
							-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$e".dat
						done

					elif [ $al_id -eq 27 ]; then

						## top-k: 3nn (prob)
						for r in "${r_range[@]}"; do
							for a in "${a_range[@]}"; do
								./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r"."$a".grid "$Q_PLY" "$e" "$OPT" \
								-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$a"."$e".dat
							done
						done

					elif [ $al_id -eq 28 ] || [ $al_id -eq 29 ]; then

						## top-k: super4pcs / superg4pcs (prob)
						./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" "$e" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$e".dat

					fi

				done

			done

		done

	done

done

