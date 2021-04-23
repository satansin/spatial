set term pdf lw 3 size 6.4,5.0 font "Helvetica,20"
set out "icpconvergence_bun2_bun3_ncf.pdf"
set style data linespoints
set log
set key bottom right reverse Left nobox
set xlabel "Error before ICP iteration" offset 0,0.3 font "Helvetica,24"
set ylabel "Error after ICP iteration" offset 1.3,0 font "Helvetica,24"
set format "%g"
plot \
 "bun2_bun3_5_1000.txt"   lc 2      pt 7 ps 0.6 title "Quadratic", \
 "bun2_bun3_1_1000.txt"   lc 1      pt 7 ps 0.6 title "Point-to-plane", \
 "bun2_bun3_3_1000.txt"   lc 7      pt 7 ps 0.6 title "Two-plane", \
 "bun2_bun3_4_1000.txt"   lc 8 dt 2 pt 7 ps 0.6 title "Symmetric-RN", \
 "bun2_bun3_2_1000.txt"   lc 4      pt 7 ps 0.6 title "Symmetric", \
 "bun2_bun3_4_1000.txt"  with lines lc 8 dt 2 notitle, \
 "bun2_bun3_9_30.txt"   lc 6      pt 7 ps 0.6 title "Point-to-plane-NN*", \
 "bun2_bun3_11_100.txt" lc 2 dt 2 pt 7 ps 0.6 title "Centroid-to-centroid-tri*", \
 "bun2_bun3_12_1000.txt" lc 5      pt 7 ps 0.6 title "Centroid-to-centroid-tri-symm*", \
 "bun2_bun3_15_50.txt" lc 8      pt 7 ps 0.6 title "Centroid-to-NN-tri*", \
 "bun2_bun3_16_50.txt" lc 1 dt 2 pt 7 ps 0.6 title "Centroid-to-NN-tri-symm*"
