#set term pdf lw 3 ps 0.6 size 6.4,5.0 font "Helvetica,20"
set term pdf lw 3 size 6.4,5.0 font "Helvetica,20"
set out "icpconvergence_bun_zipper_res2_bun_zipper_res2_cf.pdf"
set style data linespoints
set log
set key bottom right reverse Left nobox
set xlabel "Error before ICP iteration" offset 0,0.3 font "Helvetica,24"
set ylabel "Error after ICP iteration" offset 1.3,0 font "Helvetica,24"
set format "%g"
plot \
 "bun_zipper_res2_bun_zipper_res2_0_1000.txt" lc 6 pt 7 ps 0.6 title "Point-to-point", \
 "bun_zipper_res2_bun_zipper_res2_8_100.txt" lc 2 pt 7 ps 0.6 title "Point-to-NN", \
 "bun_zipper_res2_bun_zipper_res2_10_1000.txt" lc 1 pt 7 ps 0.6 title "Centroid-to-centroid*", \
 "bun_zipper_res2_bun_zipper_res2_14_50.txt" lc 7 pt 7 ps 0.6 title "Centroid-to-NN*"
