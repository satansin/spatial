#!/bin/bash


## del-Sim query for scene databases


### deterministic queries

#### comp_7 spec_query_01 noise_0 alg_10_11_12_13_14 MSE_0_1_2_3_4

	m_range=( ) # 0 1 2 3 4 )
	a_range=( ) # ( 10 11 12 13 14 )
	for m in "${m_range[@]}"; do
		for a in "${a_range[@]}"; do
			tmux new-session -d -s spec_0_01_0_"$a"_"$m" "./query_2_spec.sh 0 01 0 $a $m"
		done
	done

#### comp_7 rand_query_d_1000 qi_0..9 noise_0 alg_10_11_12_13_14 MSE_0_1_2_3_4

	i_range=( ) # 0 1 2 3 4 5 6 7 8 9 )
	m_range=( ) # "'0 1 2 3 4'" ) # 0 1 2 3 4 )
	a_range=( ) # "'13 14'" ) # ( 10 11 12 13 14 )
	for i in "${i_range[@]}"; do
		for m in "${m_range[@]}"; do
			for a in "${a_range[@]}"; do
				tmux new-session -d -s rand_0_0_"$i"_0_"$a"_"$m" "./query_2_rand.sh 0 0 $i 0 $a $m"
			done
		done
	done

#### comp_7 spec_query_02_06_08_10 noise_0 alg_10_11_12_13_14 MSE_0

	q_range=( ) # 02 06 08 10 )
	a_range=( ) # 10 11 12 13 14 )
	for q in "${q_range[@]}"; do
		for a in "${a_range[@]}"; do
			tmux new-session -d -s spec_0_"$q"_0_"$a"_0 "./query_2_spec.sh 0 $q 0 $a 0"
		done
	done

#### comp_7 spec_query_01 noise_0_1_2_3_4 alg_10_11_12_13_14 (query_rand_noise_index_0..9) MSE_5

	n_range=( ) # 0 1 2 3 4 )
	a_range=( ) # 10 11 12 13 14 )
	for n in "${n_range[@]}"; do
		for a in "${a_range[@]}"; do
			tmux new-session -d -s spec_0_01_"$n"_"$a"_5 "./query_2_spec.sh 0 01 $n $a 5"
		done
	done

	n_range=( ) # 1 2 3 4 )
	a_range=( ) # 10 11 12 13 14 )
	rn_range=( ) # 0 1 2 3 4 5 6 7 8 9 )
	for n in "${n_range[@]}"; do
		for a in "${a_range[@]}"; do
			for r in "${rn_range[@]}"; do
				tmux new-session -d -s spec_0_01_"$n"_"$a"_"$r"_5 "./query_2_spec_rn.sh 0 01 $n $a $r 5"
			done
		done
	done


### probabilistic queries

#### comp_7_6_5_1 spec_query_01 noise_1 alg_1_3_5_8_9 MSE_default

	c_range=( ) # 0 1 2 3 )
	a_range=( ) # 1 3 5 8 9 10 )
	for c in "${c_range[@]}"; do
		for a in "${a_range[@]}"; do
			tmux new-session -d -s spec_"$c"_01_1_"$a" "./query_2_spec.sh $c 01 1 $a"
		done
	done

#### comp_7_6_5_1 rand_d_1000 qi_0..9 noise_1 alg_1_3_5_8_9 MSE_default

	c_range=( ) # 0 1 2 3 )
	i_range=( ) # 0 1 2 3 4 5 6 7 8 9
	a_range=( ) # 1 3 5 8 9 10 )
	for c in "${c_range[@]}"; do
		for i in "${i_range[@]}"; do
			for a in "${a_range[@]}"; do
				# tmux new-session -d -s rand_"$c"_0_"$i"_1_"$a" "./query_2_rand.sh $c 0 $i 1 $a"
				./query_2_rand.sh $c 0 $i 1 $a
			done
		done
	done

#### ... TODO: spec var_q?

#### ... TODO: rand var_d?

#### comp_7 spec_query_01 noise_1 alg_15_16_17_18_19 MSE_4_5_6_7

	m_range=( ) # 4 5 6 7 )
	a_range=( ) # 15 16 17 18 19 20 ) TODO: change to 20, which has already executed?
	for m in "${m_range[@]}"; do
		for a in "${a_range[@]}"; do
			tmux new-session -d -s spec_0_01_1_"$a"_"$m" "./query_2_spec.sh 0 01 1 $a $m"
			# ./query_2_spec.sh 0 01 1 $a $m
		done
	done

#### comp_7 spec_query_01 noise_0_2_3_4 alg_1_3_5_9_10 (query_rand_noise_index_0..9) MSE_default

	n_range=( ) # 0 2 3 4 )
	a_range=( ) # 1 3 5 8 9 10 )
	for n in "${n_range[@]}"; do
		for a in "${a_range[@]}"; do
			# tmux new-session -d -s spec_0_01_"$n"_"$a" "./query_2_spec.sh 0 01 $n $a"
			./query_2_spec.sh 0 01 $n $a
		done
	done

	n_range=( ) # 1 2 3 4 )
	a_range=( ) # 1 3 5 8 9 10 )
	rn_range=( ) # 0 1 2 3 4 5 6 7 8 9 )
	for n in "${n_range[@]}"; do
		for a in "${a_range[@]}"; do
			for r in "${rn_range[@]}"; do
				tmux new-session -d -s spec_0_01_"$n"_"$a"_"$r" "./query_2_spec_rn.sh 0 01 $n $a $r"
				# ./query_2_spec_rn.sh 0 01 $n $a $r
			done
		done
	done

#### comp_7 rand_d_1000 qi_0..9 noise_0_2_3_4 alg_1_3_5_9_10 MSE_default

	i_range=( ) # 0 1 2 3 4 5 6 7 8 9 )
	n_range=( ) # "'0 1 2 3 4'" )
	a_range=( ) # 1 3 5 8 9 10 )
	for i in "${i_range[@]}"; do
		for n in "${n_range[@]}"; do
			for a in "${a_range[@]}"; do
				tmux new-session -d -s rand_0_0_"$i"_"$n"_"$a" "./query_2_rand.sh 0 0 $i $n $a"
				# ./query_2_rand.sh 0 0 $i $n $a
			done
		done
	done


## del-Sim query for object databases


### deterministic queries

#### 2_sel_001k spec_q_02 noise_0 alg_11_12_13_14_20 MSE_0_1_2_3_4

	m_range=( ) # 0 1 2 3 4 )
	a_range=( ) # ( 11 12 13 14 20 )
	for m in "${m_range[@]}"; do
		for a in "${a_range[@]}"; do
			tmux new-session -d -s spec_1_02_0_"$a"_"$m" "./query_3_spec.sh 1 02 0 $a $m"
		done
	done

#### 2_sel_001k rand noise_0 alg_11_12_13_14_20 MSE_0_1_2_3_4

	i_range=( ) # 0 1 2 3 4 5 6 7 8 9 )
	m_range=( ) # "'0 1 2 3 4'" ) # 0 1 2 3 4 )
	a_range=( ) # ( 11 12 13 14 20 )
	for i in "${i_range[@]}"; do
		for m in "${m_range[@]}"; do
			for a in "${a_range[@]}"; do
				tmux new-session -d -s rand_1_"$i"_0_"$a"_"$m" "./query_3_rand.sh 1 $i 0 $a $m"
			done
		done
	done

#### 2_sel_001k(_rm) spec_q_01_02_03_04_05 noise_0 alg_11_12_13_14_20 (rm_0..4) MSE_0
	
	q_range=( ) # 01 03 04 05 ) # 02 has been covered
	a_range=( ) # 11 12 13 14 20 )
	for q in "${q_range[@]}"; do
		for a in "${a_range[@]}"; do
			tmux new-session -d -s spec_1_"$q"_0_"$a"_0 "./query_3_spec.sh 1 $q 0 $a 0"
			# ./query_3_spec.sh 1 $q 1 $a 0
		done
	done
	
	q_range=( ) # 01 02 03 04 05 )
	a_range=( ) # 10 11 12 13 14 )
	rm_range=( ) # "'0 1 2 3 4'" )
	for q in "${q_range[@]}"; do
		for a in "${a_range[@]}"; do
			for r in "${rm_range[@]}"; do
				tmux new-session -d -s spec_1_"$q"_0_"$a"_"$r"_0 "./query_3_spec_rm.sh 1 $q 0 $a $r 0"
				# ./query_3_spec_rm.sh 1 $q 0 $a $r 0
			done
		done
	done

#### 2_sel_001k spec_q_01 noise_0_1_2_3 alg_11_12_13_14_20 MSE_6

	n_range=( ) # "'0 1 2 3'" )
	a_range=( ) # 11 12 13 14 20 )
	for n in "${n_range[@]}"; do
		for a in "${a_range[@]}"; do
			tmux new-session -d -s spec_1_01_"$n"_"$a"_6 "./query_3_spec.sh 1 01 $n $a 6"
		done
	done


### probabilistic queries

#### TODO...

#### 2_sel_001k(_rm) spec_q_01_02_03_04_05 noise_1 alg_1_3_5_8_9 (rm_0..4) MSE_default
	
	q_range=( ) # 02 03 04 05 )
	a_range=( ) # 1 3 5 8 9 )
	for q in "${q_range[@]}"; do
		for a in "${a_range[@]}"; do
			# tmux new-session -d -s spec_1_"$q"_1_"$a" "./query_3_spec.sh 1 $q 1 $a"
			./query_3_spec.sh 1 $q 1 $a
		done
	done
	
	q_range=( ) # "'01 02 03 04 05'" ) # 01 02 03 04 05 )
	a_range=( ) # 1 3 5 8 9 )
	rm_range=( ) # 0 1 2 3 4 )
	for q in "${q_range[@]}"; do
		for a in "${a_range[@]}"; do
			for r in "${rm_range[@]}"; do
				tmux new-session -d -s spec_1_"$q"_1_"$a"_"$r" "./query_3_spec_rm.sh 1 $q 1 $a $r"
				# ./query_3_spec_rm.sh 1 $q 1 $a $r
			done
		done
	done

#### 2_sel_001k spec_q_02 noise_1 alg_15_16_17_18_19 MSE_5_6_7_8_9

	m_range=( ) # 5 6 7 8 9 )
	a_range=( ) # 15 16 17 18 19 20 )
	for m in "${m_range[@]}"; do
		for a in "${a_range[@]}"; do
			tmux new-session -d -s spec_1_02_1_"$a"_"$m" "./query_3_spec.sh 1 02 1 $a $m"
			# ./query_3_spec.sh 1 02 1 $a $m
		done
	done


## top-k query


### scene databases

#### comp_7 spec_q_01_02_06_10 noise_0 alg_21_22_23_24_30 (det)

	s=0
	q_range=( ) # 02 06 10 ) # 01 02 06 10
	n=0
	a_range=( ) # 30 ) # 21 22 23 24 30 )
	for q in "${q_range[@]}"; do
		for a in "${a_range[@]}"; do
			tmux new-session -d -s spec_"$s"_"$q"_"$n"_"$a" "./query_2_spec.sh $s $q $n $a"
			# ./query_2_spec.sh $s $q $n $a
		done
	done

#### comp_7 spec_q_01 noise(_0)_1_2_3_4 alg_25_26_27_28_29 (query_rand_noise_index_0..9) (prob)

	s=0
	q=01
	n_range=( ) # 0 1 2 3 4 )
	a_range=( ) # 25 26 27 28 29 30 ) # n = 0, a = 30 already covered
	for n in "${n_range[@]}"; do
		for a in "${a_range[@]}"; do
			tmux new-session -d -s spec_"$s"_"$q"_"$n"_"$a" "./query_2_spec.sh $s $q $n $a"
			# ./query_2_spec.sh $s $q $n $a
		done
	done

	n_range=( ) # 1 2 3 4 ) !! pay attention: no 0 exists here
	a_range=( ) # 25 26 27 28 29 )
	rn_range=( ) # 0 1 2 3 4 5 6 7 8 9 )
	for n in "${n_range[@]}"; do
		for a in "${a_range[@]}"; do
			for r in "${rn_range[@]}"; do
				tmux new-session -d -s spec_0_01_"$n"_"$a"_"$r" "./query_2_spec_rn.sh 0 01 $n $a $r"
				# ./query_2_spec_rn.sh 0 01 $n $a $r
			done
		done
	done

#### comp_7_6_5_1 spec_q_01 noise_1 alg_25_26_27_28_29 (prob)

	s_range=( ) # 1 2 3 ) # 0 has been covered
	q=01
	n=1
	a_range=( ) # 25 26 27 28 29 ) # 30 # 28 & 29 too long for super/g4pcs even for the second dataset
	for s in "${s_range[@]}"; do
		for a in "${a_range[@]}"; do
			# tmux new-session -d -s spec_"$s"_"$q"_"$n"_"$a" "./query_2_spec.sh $s $q $n $a"
			./query_2_spec.sh $s $q $n $a
		done
	done


### object databases

#### 2_sel_001k(_rm) spec_q_01_02_03_04_05 noise_0 alg_21_22_23_24_30 (rm_0..4) (det)
	
	s=1
	q_range=( ) # 01 02 03 04 05 )
	n=0
	a_range=( ) # 21 22 23 24 30 ) # ICP fail to generate results, thus using existing results
	for q in "${q_range[@]}"; do
		for a in "${a_range[@]}"; do
			# tmux new-session -d -s spec_"$s"_"$q"_"$n"_"$a" "./query_3_spec.sh $s $q $n $a"
			./query_3_spec.sh $s $q $n $a
		done
	done
	
	q_range=( ) # 01 02 03 04 05 )
	a_range=( ) # 21 22 23 24 )
	rm_range=( ) # 0 1 2 3 4 ) # "'0 1 2 3 4'" )
	for q in "${q_range[@]}"; do
		for a in "${a_range[@]}"; do
			for r in "${rm_range[@]}"; do
				# tmux new-session -d -s spec_1_"$q"_0_"$a"_"$r" "./query_3_spec_rm.sh 1 $q 0 $a $r"
				./query_3_spec_rm.sh 1 $q 0 $a $r
			done
		done
	done

#### 2_sel_001k rand noise_0_1_2_3_4 alg_25_26_27_28_29 (prob)

	s=1
	i_range=( ) # 0 1 2 3 4 5 6 7 8 9 )
	n_range=( ) # "'0 1 2 3 4'" )
	a_range=( ) # 25 26 27 28 29 30 ) # ICP fail to generate results, thus using existing results
	for i in "${i_range[@]}"; do
		for n in "${n_range[@]}"; do
			for a in "${a_range[@]}"; do
				tmux new-session -d -s rand_"$s"_"$i"_"$n"_"$a" "./query_3_rand.sh $s $i $n $a"
				# ./query_3_rand.sh $s $i $n $a
			done
		done
	done

#### 2_sel_001k spec_q_02 noise_0 alg_21_22_23_24 k_2..40 (det)

	s=1
	q=02
	n=0
	a_range=( ) # 21 22 23 24 ) # ICP fail to generate results, thus using existing results
	k_range=( ) # 2 4 6 8 10 20 30 40 )
	for a in "${a_range[@]}"; do
		for k in "${k_range[@]}"; do
			tmux new-session -d -s spec_"$s"_"$q"_"$n"_"$a"_"$k" "./query_3_spec_vk.sh $s $q $n $a $k"
			# ./query_3_spec_vk.sh $s $q $n $a $k
		done
	done

#### 2_sel_001k spec_q_02 noise_1 alg_25_26_27_28_29 k_2..10 (prob)

	s=1
	q=02
	n=1
	a_range=( ) # 25 26 27 28 29 ) # ICP fail to generate results, thus using existing results
	k_range=( ) # 2 4 6 8 10 )
	for a in "${a_range[@]}"; do
		for k in "${k_range[@]}"; do
			# tmux new-session -d -s spec_"$s"_"$q"_"$n"_"$a"_"$k" "./query_3_spec_vk.sh $s $q $n $a $k"
			./query_3_spec_vk.sh $s $q $n $a $k
		done
	done

#### db_1_2_3_4_5 spec_q_02 noise_1 alg_25_26_27_28_29 (prob)

	s_range=( ) # 0 1 2 3 4 )
	q=02
	n=1
	a_range=( ) # ICP fail to generate results, thus using existing results
	for s in "${s_range[@]}"; do
		for a in "${a_range[@]}"; do
			# tmux new-session -d -s spec_"$s"_"$q"_"$n"_"$a" "./query_3_spec.sh $s $q $n $a"
			./query_3_spec.sh $s $q $n $a
		done
	done


## different sampling rate queries


### probabilistic del-Sim query for scene databases

#### comp_5 remember to run non-cpq version