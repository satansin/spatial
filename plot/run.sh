#!/bin/bash

source ../common/config/dir_loc.sh

# gnuplot -e "ofile='$DIR_STAT/01_tuning_w_r/tuning_w_new.eps'" -e "dfile='$DIR_STAT/01_tuning_w_r/tuning_w_new.dat'" 101_tuning_w.gnu
gnuplot -e "ofile='$DIR_STAT/01_tuning_w_r/tuning_r_new.eps'" -e "dfile='$DIR_STAT/01_tuning_w_r/tuning_r_new.dat'" 102_tuning_r.gnu