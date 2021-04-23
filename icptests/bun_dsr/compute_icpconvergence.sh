#!/bin/sh -x


dir=bun_dsr
#ntrials=1000
#ntrials=50
ntrials_list=( 1000 30 1000 50 1000 1000 1000 1000 1000 30 100 1000 1000 50 50 50 )
method_list=( 0 8 10 14 1 2 3 4 5 9 11 12 13 15 16 17 )

# model=bun3
# data=bun2
# model=bun2
# data=bun3

model=bun4
data=bun2

for i in `seq 0 15`
do
	../icpconvergence ${model}.ply ${data}.ply ${method_list[i]} ${ntrials_list[i]}
done

cat > icpconvergence_${model}_${data}_cf.gnuplot << END
set term pdf lw 3 size 6.4,5.0 font "Helvetica,20"
set out "icpconvergence_${model}_${data}_cf.pdf"
set style data linespoints
set log
set key bottom right reverse Left nobox
set xlabel "Error before ICP iteration" offset 0,0.3 font "Helvetica,24"
set ylabel "Error after ICP iteration" offset 1.3,0 font "Helvetica,24"
set format "%g"
plot \
 "${model}_${data}_0_${ntrials_list[0]}.txt"  lc 6 pt 7 ps 0.6 title "Point-to-point", \
 "${model}_${data}_8_${ntrials_list[1]}.txt"  lc 2 pt 7 ps 0.6 title "Point-to-NN", \
 "${model}_${data}_10_${ntrials_list[2]}.txt" lc 1 pt 7 ps 0.6 title "Centroid-to-centroid*", \
 "${model}_${data}_14_${ntrials_list[3]}.txt" lc 7 pt 7 ps 0.6 title "Centroid-to-NN*"
END

gnuplot icpconvergence_${model}_${data}_cf.gnuplot


cat > icpconvergence_${model}_${data}_ncf.gnuplot << END
set term pdf lw 3 size 6.4,5.0 font "Helvetica,20"
set out "icpconvergence_${model}_${data}_ncf.pdf"
set style data linespoints
set log
set key bottom right reverse Left nobox
set xlabel "Error before ICP iteration" offset 0,0.3 font "Helvetica,24"
set ylabel "Error after ICP iteration" offset 1.3,0 font "Helvetica,24"
set format "%g"
plot \\
 "${model}_${data}_5_${ntrials_list[8]}.txt"   lc 2      pt 7 ps 0.6 title "Quadratic", \\
 "${model}_${data}_1_${ntrials_list[4]}.txt"   lc 1      pt 7 ps 0.6 title "Point-to-plane", \\
 "${model}_${data}_3_${ntrials_list[6]}.txt"   lc 7      pt 7 ps 0.6 title "Two-plane", \\
 "${model}_${data}_4_${ntrials_list[7]}.txt"   lc 8 dt 2 pt 7 ps 0.6 title "Symmetric-RN", \\
 "${model}_${data}_2_${ntrials_list[5]}.txt"   lc 4      pt 7 ps 0.6 title "Symmetric", \\
 "${model}_${data}_4_${ntrials_list[7]}.txt"  with lines lc 8 dt 2 notitle, \\
 "${model}_${data}_9_${ntrials_list[9]}.txt"   lc 6      pt 7 ps 0.6 title "Point-to-plane-NN*", \\
 "${model}_${data}_11_${ntrials_list[10]}.txt" lc 2 dt 2 pt 7 ps 0.6 title "Centroid-to-centroid-tri*", \\
 "${model}_${data}_12_${ntrials_list[11]}.txt" lc 5      pt 7 ps 0.6 title "Centroid-to-centroid-tri-symm*", \\
 "${model}_${data}_15_${ntrials_list[13]}.txt" lc 8      pt 7 ps 0.6 title "Centroid-to-NN-tri*", \\
 "${model}_${data}_16_${ntrials_list[14]}.txt" lc 1 dt 2 pt 7 ps 0.6 title "Centroid-to-NN-tri-symm*"
END

gnuplot icpconvergence_${model}_${data}_ncf.gnuplot
