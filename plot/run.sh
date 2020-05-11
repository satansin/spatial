#!/bin/bash

source ../common/config/dir_loc.sh

# gnuplot -e "ofile='$DIR_STAT/01_tuning_w_r/tuning_w_new.eps'" -e "dfile='$DIR_STAT/01_tuning_w_r/tuning_w_new.dat'" 101_tuning_w.gnu
# gnuplot -e "ofile='$DIR_STAT/01_tuning_w_r/tuning_r_new.eps'" -e "dfile='$DIR_STAT/01_tuning_w_r/tuning_r_new.dat'" 102_tuning_r.gnu
# gnuplot -e "ofile='$DIR_STAT/02_indoors/01_varied_n/no_noise_pcr_adv.eps'" -e "dfile='$DIR_STAT/02_indoors/01_varied_n/no_noise_pcr_adv.dat'" 201_ind_var_n_no_noise.gnu
gnuplot -e "ofile='$DIR_STAT/02_indoors/01_varied_n/noise_1_pcr_adv.eps'" -e "dfile='$DIR_STAT/02_indoors/01_varied_n/noise_1_pcr_adv.dat'" 202_ind_var_n_noise_1.gnu