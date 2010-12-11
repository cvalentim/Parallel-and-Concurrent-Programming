# Gnuplot script for plotting experimental data
set autoscale
unset log
unset label
set xtic auto
set ytic auto
set title "Tempo x Tamanho entrada"
set xlabel "Numero de chaves"
set terminal latex 
set out 'graphics.tex'
plot "stat_organized1.dat" using 2:3 with linespoint title '1 processo', \
     "stat_organized4.dat" using 2:3 with linespoint title '4 processos', \
     "stat_organized8.dat" using 2:3 with linespoint title '8 processos'
set term pop
