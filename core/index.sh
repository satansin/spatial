#!/bin/bash

source ../common/config/dir_loc.sh



## Generate grids by grid-size and radius

# mkdir -p "$DIR_INDEX"/indoor_scans/recon_bedroom_1/
# mkdir -p "$DIR_INDEX"/indoor_scans/recon_bedroom_2/
# mkdir -p "$DIR_INDEX"/indoor_scans/recon_bedroom_3/
# mkdir -p "$DIR_INDEX"/indoor_scans/recon_bedroom_4/
# ./out/grid.out "$DIR_DB"/indoor_scans/recon_bedroom_1/ 16 80 "$DIR_INDEX"/indoor_scans/recon_bedroom_1/recon_bedroom_1.016.080.grid #-show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/recon_bedroom_2/ 24 120 "$DIR_INDEX"/indoor_scans/recon_bedroom_2/recon_bedroom_2.024.120.grid #-show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/recon_bedroom_3/ 30 150 "$DIR_INDEX"/indoor_scans/recon_bedroom_3/recon_bedroom_3.030.150.grid #-show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/recon_bedroom_4/ 40 200 "$DIR_INDEX"/indoor_scans/recon_bedroom_4/recon_bedroom_4.040.200.grid #-show_prog_bar

# mkdir -p "$DIR_INDEX"/indoor_scans/recon_boardroom_1/
# mkdir -p "$DIR_INDEX"/indoor_scans/recon_boardroom_2/
# mkdir -p "$DIR_INDEX"/indoor_scans/recon_boardroom_3/
# mkdir -p "$DIR_INDEX"/indoor_scans/recon_boardroom_4/
# ./out/grid.out "$DIR_DB"/indoor_scans/recon_boardroom_1/ 18 90 "$DIR_INDEX"/indoor_scans/recon_boardroom_1/recon_boardroom_1.018.090.grid #-show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/recon_boardroom_2/ 24 120 "$DIR_INDEX"/indoor_scans/recon_boardroom_2/recon_boardroom_2.024.120.grid #-show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/recon_boardroom_3/ 32 160 "$DIR_INDEX"/indoor_scans/recon_boardroom_3/recon_boardroom_3.032.160.grid #-show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/recon_boardroom_4/ 42 210 "$DIR_INDEX"/indoor_scans/recon_boardroom_4/recon_boardroom_4.042.210.grid #-show_prog_bar

# mkdir -p "$DIR_INDEX"/indoor_scans/recon_loft_1/
# mkdir -p "$DIR_INDEX"/indoor_scans/recon_loft_2/
# mkdir -p "$DIR_INDEX"/indoor_scans/recon_loft_3/
# mkdir -p "$DIR_INDEX"/indoor_scans/recon_loft_4/
# ./out/grid.out "$DIR_DB"/indoor_scans/recon_loft_1/ 18 90 "$DIR_INDEX"/indoor_scans/recon_loft_1/recon_loft_1.018.090.grid #-show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/recon_loft_2/ 24 120 "$DIR_INDEX"/indoor_scans/recon_loft_2/recon_loft_2.024.120.grid #-show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/recon_loft_3/ 32 160 "$DIR_INDEX"/indoor_scans/recon_loft_3/recon_loft_3.032.160.grid #-show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/recon_loft_4/ 42 210 "$DIR_INDEX"/indoor_scans/recon_loft_4/recon_loft_4.042.210.grid #-show_prog_bar

# mkdir -p "$DIR_INDEX"/indoor_scans/comp_1/
# mkdir -p "$DIR_INDEX"/indoor_scans/comp_2/
# mkdir -p "$DIR_INDEX"/indoor_scans/comp_3/
# mkdir -p "$DIR_INDEX"/indoor_scans/comp_4/

# ./out/grid.out "$DIR_DB"/indoor_scans/comp_1/   4  80 "$DIR_INDEX"/indoor_scans/comp_1/comp_1.004.080.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_1/   8  80 "$DIR_INDEX"/indoor_scans/comp_1/comp_1.008.080.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_1/  12  80 "$DIR_INDEX"/indoor_scans/comp_1/comp_1.012.080.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_1/  16  80 "$DIR_INDEX"/indoor_scans/comp_1/comp_1.016.080.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_1/  18  90 "$DIR_INDEX"/indoor_scans/comp_1/comp_1.018.090.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_1/  20  80 "$DIR_INDEX"/indoor_scans/comp_1/comp_1.020.080.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_1/  24  80 "$DIR_INDEX"/indoor_scans/comp_1/comp_1.024.080.grid -show_prog_bar

# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/   6  30 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.006.030.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/  12  60 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.012.060.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/  18  90 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.018.090.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/  24 120 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.024.120.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/  30 150 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.030.150.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/  36 180 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.036.180.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/  48 240 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.048.240.grid -show_prog_bar

# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/   6 120 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.006.120.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/  12 120 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.012.120.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/  18 120 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.018.120.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/  30 120 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.030.120.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/  36 120 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.036.120.grid -show_prog_bar

# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/  12  30 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.012.030.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/  12  90 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.012.090.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_2/  12 150 "$DIR_INDEX"/indoor_scans/comp_2/comp_2.012.150.grid -show_prog_bar

# ./out/grid.out "$DIR_DB"/indoor_scans/comp_3/   8  40 "$DIR_INDEX"/indoor_scans/comp_3/comp_3.008.040.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_3/  16  80 "$DIR_INDEX"/indoor_scans/comp_3/comp_3.016.080.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_3/  24 120 "$DIR_INDEX"/indoor_scans/comp_3/comp_3.024.120.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_3/  32 160 "$DIR_INDEX"/indoor_scans/comp_3/comp_3.032.160.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_3/  40 200 "$DIR_INDEX"/indoor_scans/comp_3/comp_3.040.200.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_3/  48 240 "$DIR_INDEX"/indoor_scans/comp_3/comp_3.048.240.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_3/  60 300 "$DIR_INDEX"/indoor_scans/comp_3/comp_3.060.300.grid -show_prog_bar

# ./out/grid.out "$DIR_DB"/indoor_scans/comp_4/ 42 210 "$DIR_INDEX"/indoor_scans/comp_4/comp_4.042.210.grid -show_prog_bar




# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_1_100/
# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_2_100/
# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_3_100/
# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_4_100/
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_1_100/ 26 130 "$DIR_INDEX"/obj_scans/obj_scans_1_100/obj_scans_1_100.026.130.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_2_100/ 36 180 "$DIR_INDEX"/obj_scans/obj_scans_2_100/obj_scans_2_100.036.180.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_3_100/ 48 240 "$DIR_INDEX"/obj_scans/obj_scans_3_100/obj_scans_3_100.048.240.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_4_100/ 64 320 "$DIR_INDEX"/obj_scans/obj_scans_4_100/obj_scans_4_100.064.320.grid -show_prog_bar

# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_3_200/
# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_3_300/
# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_3_400/
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_3_200/ 48 240 "$DIR_INDEX"/obj_scans/obj_scans_3_200/obj_scans_3_200.048.240.grid #-show_prog_bar
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_3_300/ 48 240 "$DIR_INDEX"/obj_scans/obj_scans_3_300/obj_scans_3_300.048.240.grid #-show_prog_bar
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_3_400/ 48 240 "$DIR_INDEX"/obj_scans/obj_scans_3_400/obj_scans_3_400.048.240.grid #-show_prog_bar

## Not yet executed
# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_1_200/
# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_2_200/
# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_4_200/
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_1_200/ 26 130 "$DIR_INDEX"/obj_scans/obj_scans_1_200/obj_scans_1_200.026.130.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_2_200/ 36 180 "$DIR_INDEX"/obj_scans/obj_scans_2_200/obj_scans_2_200.036.180.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_4_200/ 64 320 "$DIR_INDEX"/obj_scans/obj_scans_4_200/obj_scans_4_200.064.320.grid -show_prog_bar

# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_1_300/
# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_2_300/
# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_4_300/
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_1_300/ 26 130 "$DIR_INDEX"/obj_scans/obj_scans_1_300/obj_scans_1_300.026.130.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_2_300/ 36 180 "$DIR_INDEX"/obj_scans/obj_scans_2_300/obj_scans_2_300.036.180.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_4_300/ 64 320 "$DIR_INDEX"/obj_scans/obj_scans_4_300/obj_scans_4_300.064.320.grid -show_prog_bar

# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_1_400/
# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_2_400/
# mkdir -p "$DIR_INDEX"/obj_scans/obj_scans_4_400/
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_1_400/ 26 130 "$DIR_INDEX"/obj_scans/obj_scans_1_400/obj_scans_1_400.026.130.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_2_400/ 36 180 "$DIR_INDEX"/obj_scans/obj_scans_2_400/obj_scans_2_400.036.180.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/obj_scans/obj_scans_4_400/ 64 320 "$DIR_INDEX"/obj_scans/obj_scans_4_400/obj_scans_4_400.064.320.grid -show_prog_bar
## Not yet executed



## Build R*-tree over the generated grids

# ./out/index.out "$DIR_DB"/indoor_scans/recon_bedroom_1/ "$DIR_INDEX"/indoor_scans/recon_bedroom_1/recon_bedroom_1.016.080.grid
# ./out/index.out "$DIR_DB"/indoor_scans/recon_bedroom_2/ "$DIR_INDEX"/indoor_scans/recon_bedroom_2/recon_bedroom_2.024.120.grid
# ./out/index.out "$DIR_DB"/indoor_scans/recon_bedroom_3/ "$DIR_INDEX"/indoor_scans/recon_bedroom_3/recon_bedroom_3.030.150.grid
# ./out/index.out "$DIR_DB"/indoor_scans/recon_bedroom_4/ "$DIR_INDEX"/indoor_scans/recon_bedroom_4/recon_bedroom_4.040.200.grid

# ./out/index.out "$DIR_DB"/indoor_scans/recon_boardroom_1/ "$DIR_INDEX"/indoor_scans/recon_boardroom_1/recon_boardroom_1.018.090.grid
# ./out/index.out "$DIR_DB"/indoor_scans/recon_boardroom_2/ "$DIR_INDEX"/indoor_scans/recon_boardroom_2/recon_boardroom_2.024.120.grid
# ./out/index.out "$DIR_DB"/indoor_scans/recon_boardroom_3/ "$DIR_INDEX"/indoor_scans/recon_boardroom_3/recon_boardroom_3.032.160.grid
# ./out/index.out "$DIR_DB"/indoor_scans/recon_boardroom_4/ "$DIR_INDEX"/indoor_scans/recon_boardroom_4/recon_boardroom_4.042.210.grid

# ./out/index.out "$DIR_DB"/indoor_scans/recon_loft_1/ "$DIR_INDEX"/indoor_scans/recon_loft_1/recon_loft_1.018.090.grid
# ./out/index.out "$DIR_DB"/indoor_scans/recon_loft_2/ "$DIR_INDEX"/indoor_scans/recon_loft_2/recon_loft_2.024.120.grid
# ./out/index.out "$DIR_DB"/indoor_scans/recon_loft_3/ "$DIR_INDEX"/indoor_scans/recon_loft_3/recon_loft_3.032.160.grid
# ./out/index.out "$DIR_DB"/indoor_scans/recon_loft_4/ "$DIR_INDEX"/indoor_scans/recon_loft_4/recon_loft_4.042.210.grid

# ./out/index.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.004.080.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.008.080.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.012.080.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.016.080.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.018.090.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.020.080.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.024.080.grid

# ./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.006.030.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.012.060.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.018.090.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.024.120.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.030.150.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.036.180.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.048.240.grid

# ./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.006.120.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.012.120.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.018.120.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.030.120.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.036.120.grid

./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.012.030.grid
./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.012.090.grid
./out/index.out "$DIR_DB"/indoor_scans/comp_2/ "$DIR_INDEX"/indoor_scans/comp_2/comp_2.012.150.grid

# ./out/index.out "$DIR_DB"/indoor_scans/comp_3/ "$DIR_INDEX"/indoor_scans/comp_3/comp_3.008.040.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_3/ "$DIR_INDEX"/indoor_scans/comp_3/comp_3.016.080.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_3/ "$DIR_INDEX"/indoor_scans/comp_3/comp_3.024.120.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_3/ "$DIR_INDEX"/indoor_scans/comp_3/comp_3.032.160.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_3/ "$DIR_INDEX"/indoor_scans/comp_3/comp_3.040.200.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_3/ "$DIR_INDEX"/indoor_scans/comp_3/comp_3.048.240.grid
# ./out/index.out "$DIR_DB"/indoor_scans/comp_3/ "$DIR_INDEX"/indoor_scans/comp_3/comp_3.060.300.grid

# ./out/index.out "$DIR_DB"/indoor_scans/comp_4/ "$DIR_INDEX"/indoor_scans/comp_4/comp_4.042.210.grid

# ./out/index.out "$DIR_DB"/obj_scans/obj_scans_1_100/ "$DIR_INDEX"/obj_scans/obj_scans_1_100/obj_scans_1_100.026.130.grid
# ./out/index.out "$DIR_DB"/obj_scans/obj_scans_2_100/ "$DIR_INDEX"/obj_scans/obj_scans_2_100/obj_scans_2_100.036.180.grid
# ./out/index.out "$DIR_DB"/obj_scans/obj_scans_3_100/ "$DIR_INDEX"/obj_scans/obj_scans_3_100/obj_scans_3_100.048.240.grid
# ./out/index.out "$DIR_DB"/obj_scans/obj_scans_4_100/ "$DIR_INDEX"/obj_scans/obj_scans_4_100/obj_scans_4_100.064.320.grid

# ./out/index.out "$DIR_DB"/obj_scans/obj_scans_3_200/ "$DIR_INDEX"/obj_scans/obj_scans_3_200/obj_scans_3_200.048.240.grid
# ./out/index.out "$DIR_DB"/obj_scans/obj_scans_3_300/ "$DIR_INDEX"/obj_scans/obj_scans_3_300/obj_scans_3_300.048.240.grid
# ./out/index.out "$DIR_DB"/obj_scans/obj_scans_3_400/ "$DIR_INDEX"/obj_scans/obj_scans_3_400/obj_scans_3_400.048.240.grid
