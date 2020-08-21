#!/bin/bash

source ../common/config/dir_loc.sh

OUT=out

rs_range=( 7 6 5 1 ) # do not change
rs_eid=( 1 ) # matched with rs_range and e_range: (0-7, 1-6, 2-5, 3-1)
e_range=( 10 3.87 1.4 0.48 ) # do not change

d_range=( 1000 1500 2000 2500 3000 )
qi_range=( 0 1 2 3 4 5 6 7 8 9 )
nl_range=( 0 1 2 3 4 ) #0 1 2 3 4 5 )
r_range=( 200 ) #050 100 150 200 250 300 350 400 450 500 )
a_range=( 015 ) #030 )
t_range=( 030 015 )

pcr_adv_alg=(
	prob3 prob3_cpq
	donut_prob3 donut_prob3_cpq
	3nn_prob3 3nn_prob3_cpq
	3lnn_prob3 3nn_sim_prob3
)
pcr_adv_idx=(
	gt gt
	donut donut
	3nn 3nn
	3lnn 3nn_sim
)
pcr_adv_eid=( 1 3 5 ) #0 1 2 3 4 5 6 7 )


## indoor (sim) (rand)

CURR_DS="$DIR_RESULT"/indoor_scans_rand
mkdir -p "$CURR_DS"

for rs_id in "${rs_eid[@]}"; do

	rs=${rs_range[rs_id]}
	e=${e_range[rs_id]}

	if (( $rs > 5 )); then
		OPT="-small"
	else
		OPT=""
	fi

	DB_F="$DIR_DB"/indoor_scans/comp_"$rs"/
	CURR_DB="$CURR_DS"/comp_"$rs"
	mkdir -p "$CURR_DB"

	# for d in "${d_range[@]}"; do
	for d in 2000 2500 3000; do

		CURR_D="$CURR_DB"/d_"$d"
		mkdir -p "$CURR_D"

		for qi in "${qi_range[@]}"; do

			for nl in "${nl_range[@]}"; do

				if (( $nl < 1 )); then
					OPT_DELTA="-delta=1"
				else
					OPT_DELTA=""
				fi

				Q_PLY="$DIR_QUERY"/indoor_scans_rand/comp_"$rs"/d_"$d"/q_"$qi"."$nl".ply
				CURR_NL="$CURR_D"/q_"$qi"_"$nl"
				mkdir -p "$CURR_NL"

				## 1.1 pcr_adv

				CURR_FW="$CURR_NL"/pcr_adv
				mkdir -p "$CURR_FW"

				for al_id in "${pcr_adv_eid[@]}"; do

					CURR_AL="$CURR_FW"/"${pcr_adv_alg[al_id]}"
					# rm -rf "$CURR_AL"
					mkdir -p "$CURR_AL"

					if (( $al_id < 4 )); then

						## for donut and gt
						for r in "${r_range[@]}"; do
							./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r".grid "$Q_PLY" "$e" "$OPT" "$OPT_DELTA" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$e".dat
						done

					elif (( $al_id < 6 )); then
					
						## for 3nn
						for r in "${r_range[@]}"; do
							for a in "${a_range[@]}"; do
								./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r"."$a".grid "$Q_PLY" "$e" "$OPT_DELTA" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$a"."$e".dat
							done
						done

					elif (( $al_id < 7 )); then

						## for 3lnn
						for r in "${r_range[@]}"; do
							for t in "${t_range[@]}"; do
								for a in "${a_range[@]}"; do
									./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r"."$t"."$a".grid "$Q_PLY" "$e" "$OPT_DELTA" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$t"."$a"."$e".dat
								done
							done
						done

					else

						## for 3nn-sim
						for r in "${r_range[@]}"; do
							./"$OUT"/query_3nn_prob3.out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r".grid "$Q_PLY" "$e" "$OPT_DELTA" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$e".dat -simple
						done

					fi

				done

			done

		done

	done

done

