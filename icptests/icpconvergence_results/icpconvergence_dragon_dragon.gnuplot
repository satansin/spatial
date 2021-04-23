set term pdf lw 3 ps 0.6 size 6.4,5.0 font "Helvetica,20"
set out "icpconvergence_dragon_dragon.pdf"
set style data linespoints
set log
set key bottom right reverse Left nobox
set xlabel "Error before ICP iteration" offset 0,0.3 font "Helvetica,24"
set ylabel "Error after ICP iteration" offset 1.3,0 font "Helvetica,24"
set format "%g"
plot \
 "dragon_dragon_0.txt" lc 6 pt 7 title "Point-to-point", \
 "dragon_dragon_5.txt" lc 2 pt 7 title "Quadratic", \
 "dragon_dragon_1.txt" lc 1 pt 7 title "Point-to-plane", \
 "dragon_dragon_3.txt" lc 7 pt 7 title "Two-plane", \
 "dragon_dragon_4.txt" lc 8 dt 2 pt 7 title "Symmetric-RN", \
 "dragon_dragon_2.txt" lc 4 pt 7 title "Symmetric", \
 "dragon_dragon_4.txt" with lines lc 8 dt 2 notitle
