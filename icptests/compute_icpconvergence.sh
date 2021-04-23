#!/bin/sh -x
#
# Szymon Rusinkiewicz
# Princeton University
#
# compute_icpconvergence.sh
# Run icpconvergence for all methods on all pairs of meshes listed
# in data/convergence_tests.txt
#
# If gnuplot is installed, will make plots of the results
#

dir=icpconvergence_results
#ntrials=1000
ntrials=50

make
mkdir -p $dir
cd $dir

while read mesh1 mesh2
do
	if test ! -f $mesh1
	then
		ln -s ../data/$mesh1 .
	fi
	if test ! -f $mesh2
	then
		ln -s ../data/$mesh2 .
	fi

	for method in `seq 0 5`
	do
		../icpconvergence $mesh1 $mesh2 $method $ntrials
	done

	m1=`echo $mesh1 | sed 's/.ply//'`
	m2=`echo $mesh2 | sed 's/.ply//'`
	cat > icpconvergence_${m1}_${m2}.gnuplot << END
set term pdf lw 3 ps 0.6 size 6.4,5.0 font "Helvetica,20"
set out "icpconvergence_${m1}_${m2}.pdf"
set style data linespoints
set log
set key bottom right reverse Left nobox
set xlabel "Error before ICP iteration" offset 0,0.3 font "Helvetica,24"
set ylabel "Error after ICP iteration" offset 1.3,0 font "Helvetica,24"
set format "%g"
plot \\
 "${m1}_${m2}_0.txt" lc 6 pt 7 title "Point-to-point", \\
 "${m1}_${m2}_5.txt" lc 2 pt 7 title "Quadratic", \\
 "${m1}_${m2}_1.txt" lc 1 pt 7 title "Point-to-plane", \\
 "${m1}_${m2}_3.txt" lc 7 pt 7 title "Two-plane", \\
 "${m1}_${m2}_4.txt" lc 8 dt 2 pt 7 title "Symmetric-RN", \\
 "${m1}_${m2}_2.txt" lc 4 pt 7 title "Symmetric", \\
 "${m1}_${m2}_4.txt" with lines lc 8 dt 2 notitle
END
	gnuplot icpconvergence_${m1}_${m2}.gnuplot

done < ../data/convergence_tests.txt

