#!/bin/bash

source ../common/config/dir_loc.sh

OUT=out

rs_range=( 7 6 5 1 )
rs_eid=( $1 ) # 0 1 2 3
qi_range=( $2 ) # 01 02 06 08 10
nl_range=( $3 ) # 0 1 2 3 4
r_range=( 200 ) #050 100 150 200 250 300 350 400 450 500 )
a_range=( 015 ) #030 )
t_range=( 030 015 )
e_range=( 10 3.87 1.4 0.48 )
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
)
pcr_adv_idx=(
	gt gt
	donut donut
	3nn 3nn
	3lnn 3nn_sim
)
pcr_adv_eid=( $4 )


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

			Q_PLY="$DIR_QUERY"/indoor_scans_spec/comp_"$rs"/q_"$qi"."$nl".ply
			CURR_NL="$CURR_DB"/q_"$qi"_"$nl"
			mkdir -p "$CURR_NL"

			for al_id in "${pcr_adv_eid[@]}"; do

				CURR_AL="$CURR_NL"/"${pcr_adv_alg[al_id]}"
				# rm -rf "$CURR_AL"
				mkdir -p "$CURR_AL"

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
							./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r"."$a".grid "$Q_PLY" "$e" \
							-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$a"."$e".dat
						done
					done

				elif [ $al_id -eq 6 ]; then

					## 3lnn
					for r in "${r_range[@]}"; do
						for t in "${t_range[@]}"; do
							for a in "${a_range[@]}"; do
								./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r"."$t"."$a".grid "$Q_PLY" "$e" \
								-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$t"."$a"."$e".dat
							done
						done
					done

				elif [ $al_id -eq 7 ]; then

					## 3nn-sim
					for r in "${r_range[@]}"; do
						./"$OUT"/query_3nn_prob3.out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r".grid "$Q_PLY" "$e" \
						-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$e".dat -simple
					done

				elif [ $al_id -eq 8 ] || [ $al_id -eq 9 ]; then

					## super4pcs / superg4pcs
					for r in "${r_range[@]}"; do
						./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" "$e" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$e".dat
					done

				elif [ $al_id -eq 10 ]; then

					## goicp
					./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}".dat

				fi

			done

		done

	done

done

