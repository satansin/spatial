#!/bin/bash

# tmux new-session -d -s new_session './query_3_spec.sh 5 8'

# ./query_3_spec.sh 4 '02' 1 '1 5'
# ./query_3_rand.sh 4 '9' 1 '1'



# tmux new-session -d -s new_session_1 "./query_2_spec.sh 0 01 '0 1 2 3 4' '8 9'"
# tmux new-session -d -s spec_1_01_8 "./query_2_spec.sh 1 01 1 8"
# tmux new-session -d -s spec_1_02_9 "./query_2_spec.sh 1 02 1 9"
# tmux new-session -d -s spec_1_06_8 "./query_2_spec.sh 1 06 1 8"
# tmux new-session -d -s spec_1_06_9 "./query_2_spec.sh 1 06 1 9"
# tmux new-session -d -s spec_1_08_8 "./query_2_spec.sh 1 08 1 8"
# tmux new-session -d -s spec_1_08_9 "./query_2_spec.sh 1 08 1 9"
# tmux new-session -d -s spec_1_10_8 "./query_2_spec.sh 1 10 1 8"
# tmux new-session -d -s spec_1_10_9 "./query_2_spec.sh 1 10 1 9"

# for i in {0..9}; do
# 	# tmux new-session -d -s new_session_"$i" "./query_2_rand.sh 0 '0 1 2 3 4' $i '0 1 2 3 4' '8 9'"
# 	tmux new-session -d -s rand_"$i" "./query_2_rand.sh 1 0 $i 1 '8 9'"
# done

# ./query_2_spec.sh 0 '01 02 06 08 10' '0 2 3 4' '1 3 5'
# ./query_2_rand.sh 0 '0 1 2 3 4' '0 1 2 3 4 5 6 7 8 9' '0 2 3 4' '1 3 5'

# ./query_2_spec.sh 1 01 0 10

# for i in {0..4}; do
# 	tmux new-session -d -s goicp_01_"$i" "./query_2_spec.sh 1 01 $i 10"
# 	tmux new-session -d -s goicp_02_"$i" "./query_2_spec.sh 1 02 $i 10"
# done

for i in {0..9}; do
	tmux new-session -d -s goicp_0_0_"$i" "./query_2_rand.sh 0 1 $i '0 1 2 3 4' 10"
done
