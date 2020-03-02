#!/bin/bash

source ../common/config/dir_loc.sh



## Generate grids by grid-size and radius - indoor_scans

# mkdir -p "$DIR_INDEX"/indoor_scans/comp_1/

# ./out/grid.out "$DIR_DB"/indoor_scans/comp_1/   4  80 "$DIR_INDEX"/indoor_scans/comp_1/comp_1.004.080.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_1/   8  80 "$DIR_INDEX"/indoor_scans/comp_1/comp_1.008.080.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_1/  12  80 "$DIR_INDEX"/indoor_scans/comp_1/comp_1.012.080.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_1/  16  80 "$DIR_INDEX"/indoor_scans/comp_1/comp_1.016.080.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_1/  18  90 "$DIR_INDEX"/indoor_scans/comp_1/comp_1.018.090.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_1/  20  80 "$DIR_INDEX"/indoor_scans/comp_1/comp_1.020.080.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_1/  24  80 "$DIR_INDEX"/indoor_scans/comp_1/comp_1.024.080.grid -show_prog_bar

# mkdir -p "$DIR_INDEX"/indoor_scans/comp_5/
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_5/   12 120 "$DIR_INDEX"/indoor_scans/comp_5/comp_5.012.120.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_5/   24 120 "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.120.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_5/   36 120 "$DIR_INDEX"/indoor_scans/comp_5/comp_5.036.120.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_5/   48 120 "$DIR_INDEX"/indoor_scans/comp_5/comp_5.048.120.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_5/   60 120 "$DIR_INDEX"/indoor_scans/comp_5/comp_5.060.120.grid -show_prog_bar

# ./out/grid.out "$DIR_DB"/indoor_scans/comp_5/   24  30 "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.030.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_5/   24  60 "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.060.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_5/   24  90 "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.090.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_5/   24 150 "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.150.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_5/   24 180 "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.180.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_5/   24 240 "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.240.grid -show_prog_bar
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_5/   24 300 "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.300.grid -show_prog_bar

# mkdir -p "$DIR_INDEX"/indoor_scans/comp_6/
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_6/  40 200 "$DIR_INDEX"/indoor_scans/comp_6/comp_6.040.200.grid -show_prog_bar

# mkdir -p "$DIR_INDEX"/indoor_scans/comp_7/
# ./out/grid.out "$DIR_DB"/indoor_scans/comp_7/  60 300 "$DIR_INDEX"/indoor_scans/comp_7/comp_7.060.300.grid -show_prog_bar

## Build R*-tree over the generated grids - indoor_scans

# ./out/index3.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.004.080.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.008.080.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.012.080.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.016.080.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.018.090.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.020.080.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.024.080.grid

# ./out/index3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans/comp_5/comp_5.012.120.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.120.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans/comp_5/comp_5.036.120.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans/comp_5/comp_5.048.120.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans/comp_5/comp_5.060.120.grid

# ./out/index3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.030.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.060.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.090.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.150.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.180.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.240.grid
# ./out/index3.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.300.grid

# ./out/index3.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_INDEX"/indoor_scans/comp_6/comp_6.040.200.grid

# ./out/index3.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_INDEX"/indoor_scans/comp_7/comp_7.060.300.grid


# ./out/index6.out "$DIR_DB"/indoor_scans/comp_1/ "$DIR_INDEX"/indoor_scans/comp_1/comp_1.016.080.grid

# ./out/index6.out "$DIR_DB"/indoor_scans/comp_5/ "$DIR_INDEX"/indoor_scans/comp_5/comp_5.024.120.grid

# ./out/index6.out "$DIR_DB"/indoor_scans/comp_6/ "$DIR_INDEX"/indoor_scans/comp_6/comp_6.040.200.grid

# ./out/index6.out "$DIR_DB"/indoor_scans/comp_7/ "$DIR_INDEX"/indoor_scans/comp_7/comp_7.060.300.grid












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



# ./out/index.out "$DIR_DB"/obj_scans/obj_scans_1_100/ "$DIR_INDEX"/obj_scans/obj_scans_1_100/obj_scans_1_100.026.130.grid
# ./out/index.out "$DIR_DB"/obj_scans/obj_scans_2_100/ "$DIR_INDEX"/obj_scans/obj_scans_2_100/obj_scans_2_100.036.180.grid
# ./out/index.out "$DIR_DB"/obj_scans/obj_scans_3_100/ "$DIR_INDEX"/obj_scans/obj_scans_3_100/obj_scans_3_100.048.240.grid
# ./out/index.out "$DIR_DB"/obj_scans/obj_scans_4_100/ "$DIR_INDEX"/obj_scans/obj_scans_4_100/obj_scans_4_100.064.320.grid

# ./out/index.out "$DIR_DB"/obj_scans/obj_scans_3_200/ "$DIR_INDEX"/obj_scans/obj_scans_3_200/obj_scans_3_200.048.240.grid
# ./out/index.out "$DIR_DB"/obj_scans/obj_scans_3_300/ "$DIR_INDEX"/obj_scans/obj_scans_3_300/obj_scans_3_300.048.240.grid
# ./out/index.out "$DIR_DB"/obj_scans/obj_scans_3_400/ "$DIR_INDEX"/obj_scans/obj_scans_3_400/obj_scans_3_400.048.240.grid
