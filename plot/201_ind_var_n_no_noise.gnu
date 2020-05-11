#!/bin/gnuplot

set out ofile

set size 1.00, 0.50
set terminal postscript portrait enhanced mono "Helvetica" 22

set ylabel "Query time (ms)"
set xlabel "n"
set pointsize 2

set key left top

set xtics ("10K" 1, "100K" 2, "1M" 3, "10M" 4)
set yrange [0.00 : 220.00]
#set logscale y

plot dfile using 1:($2*1000.0)  title "query3_acc" with linespoints linetype 1 pointtype 1, \
	 dfile using 1:($9*1000.0)  title "query6_acc" with linespoints linetype 1 pointtype 2, \
	 dfile using 1:($16*1000.0) title "query3" with linespoints linetype 1 pointtype 6, \
	 dfile using 1:($23*1000.0) title "query6" with linespoints linetype 1 pointtype 4

#set terminal x11
set size 1,1