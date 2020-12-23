#!/bin/bash

# # test run
# CURR_AL="$DIR_RESULT"/indoor_scans_spec/comp_5/q_01_1/recon_donut_prob3_more/
# mkdir -p "$CURR_AL"

source ../common/config/dir_loc.sh

for i in 1_6 # 5_5 5_4 5_3 5_2 5_1 5 1_9 1_8 1_7 1_6 1_5
do
	./out/query_donut_prob3.out \
	"$DIR_DB"/indoor_scans/comp_5/ \
	"$DIR_INDEX"/indoor_scans_donut_color/comp_5/comp_5.200.grid \
	"$DIR_QUERY"/indoor_scans_spec/comp_5_recon_"$i"/q_01.1.ply \
	"$DIR_QUERY"/indoor_scans_spec/comp_"$i"/q_01.1.ply 3.2 -delta=1 \
	# -stat="$CURR_AL"/query_donut_prob3."$i".200.3.2.dat
done


# source ../common/config/dir_loc.sh
# source ./query_2_common.sh

# rs=5
# qi_range=( $1 ) # 01 02 06 10 # 08 is excluded
# nl_range=( $2 ) # 0 1 2 3 4
# pcr_adv_eid=( $3 )
# r=200
# e=3.2
# delta=1
# recon_range=( $4 ) # 1_5 ... 1_9 5 5_1 ... 5_5

# ## indoor (sim) (spec) (different sampling rate)

# CURR_DS="$DIR_RESULT"/indoor_scans_spec
# mkdir -p "$CURR_DS"

# 	e="${e_range[rs_id]}"

# 	DB_F="$DIR_DB"/indoor_scans/comp_"$rs"/
# 	CURR_DB="$CURR_DS"/comp_"$rs"
# 	mkdir -p "$CURR_DB"

# 	for qi in "${qi_range[@]}"; do

# 		for nl in "${nl_range[@]}"; do

# 			for recon_i in "${recon_range[@]}";

# 			if [ "$recon_i" -eq "$rs" ]; then
# 				Q_PLY="$DIR_QUERY"/indoor_scans_spec/comp_"$rs"/q_"$qi"."$nl".ply
# 			else
# 				Q_PLY="$DIR_QUERY"/indoor_scans_spec/comp_"$rs"_recon_"$recon_i"/q_"$qi"."$nl".ply
# 			fi
			
# 			CURR_NL="$CURR_DB"/q_"$qi"_"$nl"
# 			mkdir -p "$CURR_NL"

# 			for al_id in "${pcr_adv_eid[@]}"; do

# 				CURR_AL="$CURR_NL"/recon_"${pcr_adv_alg[al_id]}"
# 				# rm -rf "$CURR_AL"
# 				mkdir -p "$CURR_AL"

# 				if (( $al_id < 4 )); then

# 					## donut / gt
# 					./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"_color/comp_"$rs"/comp_"$rs"."$r".grid "$Q_PLY" "$e" -delta="$delta" \
# 					-stat="$CURR_AL"/recon_"${pcr_adv_alg[al_id]}"."$recon_i"."$r"."$e".dat

# 				elif (( $al_id < 6 )); then TODO!!!!
				
# 					## 3nn
# 					for r in "${r_range[@]}"; do
# 						for a in "${a_range[@]}"; do
# 							./"$OUT"/query_"${pcr_adv_alg[al_id]}".out "$DB_F" "$DIR_INDEX"/indoor_scans_"${pcr_adv_idx[al_id]}"/comp_"$rs"/comp_"$rs"."$r"."$a".grid "$Q_PLY" "$e" \
# 							-stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$r"."$a"."$e".dat
# 						done
# 					done

# 				elif [ $al_id -eq 8 ] || [ $al_id -eq 9 ]; then

# 					## super4pcs / superg4pcs
# 					for r in "${r_range[@]}"; do
# 						./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" "$e" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}"."$e".dat
# 					done

# 				elif [ $al_id -eq 10 ] || [ $al_id -eq 30 ]; then

# 					## goicp
# 					./"$OUT"/"${pcr_adv_alg[al_id]}".out "$DB_F" "$Q_PLY" -stat="$CURR_AL"/"${pcr_adv_alg[al_id]}".dat

# 				fi

# 			done

# 		done

# 	done

# done

