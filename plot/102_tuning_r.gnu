#!/bin/gnuplot

set out ofile

set size 1.00, 0.50
set terminal postscript portrait enhanced mono "Helvetica" 22

set ylabel "Query time (ms)"
set xlabel "w"
set pointsize 2

set key left top

set xtics 60.00
set yrange [0.00 : 100.00]
#set logscale y

plot dfile using 1:($2*1000.0)  title "L=0" with linespoints linetype 1 pointtype 1, \
	 dfile using 1:($9*1000.0)  title "L=1" with linespoints linetype 1 pointtype 2, \
	 dfile using 1:($16*1000.0) title "L=2" with linespoints linetype 1 pointtype 6, \
	 dfile using 1:($23*1000.0) title "L=3" with linespoints linetype 1 pointtype 4, \
	 dfile using 1:($30*1000.0) title "L=4" with linespoints linetype 1 pointtype 3

#set terminal x11
set size 1,1