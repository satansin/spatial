set term pdf lw 3 ps 0.6 size 6.4,5.0 font "Helvetica,20"
set out "icpconvergence_tum1_tum2.pdf"
set style data linespoints
set log
set key bottom right reverse Left nobox
set xlabel "Error before ICP iteration" offset 0,0.3 font "Helvetica,24"
set ylabel "Error after ICP iteration" offset 1.3,0 font "Helvetica,24"
set format "%g"
plot \
 "tum1_tum2_0.txt" lc 6 pt 7 title "Point-to-point", \
 "tum1_tum2_5.txt" lc 2 pt 7 title "Quadratic", \
 "tum1_tum2_1.txt" lc 1 pt 7 title "Point-to-plane", \
 "tum1_tum2_3.txt" lc 7 pt 7 title "Two-plane", \
 "tum1_tum2_4.txt" lc 8 dt 2 pt 7 title "Symmetric-RN", \
 "tum1_tum2_2.txt" lc 4 pt 7 title "Symmetric", \
 "tum1_tum2_4.txt" with lines lc 8 dt 2 notitle
