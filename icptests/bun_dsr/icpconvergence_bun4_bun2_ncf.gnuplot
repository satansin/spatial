set term pdf lw 3 size 6.4,5.0 font "Helvetica,20"
set out "icpconvergence_bun4_bun2_ncf.pdf"
set style data linespoints
set log
set key bottom right reverse Left nobox
set xlabel "Error before ICP iteration" offset 0,0.3 font "Helvetica,24"
set ylabel "Error after ICP iteration" offset 1.3,0 font "Helvetica,24"
set format "%g"
plot \
 "bun4_bun2_5_1000.txt"   lc 2      pt 7 ps 0.6 title "Quadratic", \
 "bun4_bun2_1_1000.txt"   lc 1      pt 7 ps 0.6 title "Point-to-plane", \
 "bun4_bun2_3_1000.txt"   lc 7      pt 7 ps 0.6 title "Two-plane", \
 "bun4_bun2_4_1000.txt"   lc 8 dt 2 pt 7 ps 0.6 title "Symmetric-RN", \
 "bun4_bun2_2_1000.txt"   lc 4      pt 7 ps 0.6 title "Symmetric", \
 "bun4_bun2_4_1000.txt"  with lines lc 8 dt 2 notitle, \
 "bun4_bun2_9_30.txt"   lc 6      pt 7 ps 0.6 title "Point-to-plane-NN*", \
 "bun4_bun2_11_100.txt" lc 2 dt 2 pt 7 ps 0.6 title "Centroid-to-centroid-tri*", \
 "bun4_bun2_12_1000.txt" lc 5      pt 7 ps 0.6 title "Centroid-to-centroid-tri-symm*", \
 "bun4_bun2_15_50.txt" lc 8      pt 7 ps 0.6 title "Centroid-to-NN-tri*", \
 "bun4_bun2_16_50.txt" lc 1 dt 2 pt 7 ps 0.6 title "Centroid-to-NN-tri-symm*"
