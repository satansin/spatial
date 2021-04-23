set term pdf lw 3 size 6.4,5.0 font "Helvetica,20"
set out "full_conv_bun2_bun3.pdf"
set style data linespoints
set log y
set key top right reverse Left nobox
set xlabel "ICP iteration" offset 0,0.3 font "Helvetica,24"
set ylabel "Error" offset 1.3,0 font "Helvetica,24"
set format "%g"
plot \
 "err_0_0.txt" lc 2      pt 7 ps 0 title "V2V-P2P", \
 "err_0_1.txt" lc 1      pt 7 ps 0 title "V2V-P2L", \
 "err_0_2.txt" lc 8 dt 2 pt 7 ps 0 title "V2V-Symm", \
 "err_1_0.txt" lc 7      pt 7 ps 0 title "V2N-P2P", \
 "err_1_1.txt" lc 4      pt 7 ps 0 title "V2N-P2L", \
 "err_1_2.txt" lc 2 dt 2 pt 7 ps 0 title "V2N-Symm", \
 "err_2_0.txt" lc 6      pt 7 ps 0 title "C2C-P2P", \
 "err_2_1.txt" lc 7 dt 2 pt 7 ps 0 title "C2C-P2L", \
 "err_2_2.txt" lc 1 dt 2 pt 7 ps 0 title "C2C-Symm", \
 "err_3_0.txt" lc 8      pt 7 ps 0 title "C2N-P2P", \
 "err_3_1.txt" lc 5      pt 7 ps 0 title "C2N-P2L", \
 "err_3_2.txt" lc 4 dt 2 pt 7 ps 0 title "C2N-Symm", \
 "err_4_0.txt" lc 2 dt 3 pt 7 ps 0 title "N2N-P2P", \
 "err_4_1.txt" lc 1 dt 3 pt 7 ps 0 title "N2N-P2L", \
 "err_4_2.txt" lc 8 dt 3 pt 7 ps 0 title "N2N-Symm"
