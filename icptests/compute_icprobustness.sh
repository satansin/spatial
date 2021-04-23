#!/bin/sh -x
#
# Szymon Rusinkiewicz
# Princeton University
#
# compute_icprobustness.sh
# Run icprobustness and make SVG charts for all pairs of overlapping bunny scans
#
# If inkscape is installed, will convert SVG to PDF
#

dir=icprobustness_results
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

	for method in `seq 0 7`
	do
		../icprobustness $mesh1 $mesh2 $method $ntrials
	done
done < ../data/robustness_tests.txt

for method in `seq 0 7`
do
	for maxiters in 20 100 500
	do
		../results2svg *_${method}_${maxiters}.txt icprobustness_${method}_${maxiters}.svg
		inkscape icprobustness_${method}_${maxiters}.svg -C --export-pdf=icprobustness_${method}_${maxiters}.pdf
	done
done

