set term pdf lw 3 ps 0.6 size 6.4,5.0 font "Helvetica,20"
set out "icpconvergence_bun000_bun090.pdf"
set style data linespoints
set log
set key bottom right reverse Left nobox
set xlabel "Error before ICP iteration" offset 0,0.3 font "Helvetica,24"
set ylabel "Error after ICP iteration" offset 1.3,0 font "Helvetica,24"
set format "%g"
plot \
 "bun000_bun090_0.txt" lc 6 pt 7 title "Point-to-point", \
 "bun000_bun090_5.txt" lc 2 pt 7 title "Quadratic", \
 "bun000_bun090_1.txt" lc 1 pt 7 title "Point-to-plane", \
 "bun000_bun090_3.txt" lc 7 pt 7 title "Two-plane", \
 "bun000_bun090_4.txt" lc 8 dt 2 pt 7 title "Symmetric-RN", \
 "bun000_bun090_2.txt" lc 4 pt 7 title "Symmetric", \
 "bun000_bun090_4.txt" with lines lc 8 dt 2 notitle
