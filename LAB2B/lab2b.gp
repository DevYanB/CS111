#! /usr/bin/gnuplot

set terminal png
set datafile separator ","

set title "Scalability-1: Synchronized Throughput"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput (ops / second)"
set logscale y 10
set output 'lab2b_1.png'
set key right top

plot \
     "< grep add-m lab2_add.csv" using ($2):(1000000000/($6)) \
     title 'add w/ mutex' with linespoints lc rgb 'green', \
     "< grep add-s lab2_add.csv" using ($2):(1000000000/($6)) \
     title 'add w/ spin' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,[[:digit:]]*,1000,1,' lab2b_list.csv" using \
    ($2):(1000000000/($7)) title 'list w/ mutex' with linespoints lc rgb 'orange', \
     "< grep 'list-none-s,[[:digit:]]*,1000,1,' lab2b_list.csv" using \
    ($2):(1000000000/($7)) title 'list w/ spin' with linespoints lc rgb 'violet'

set title "Per-operation mutex wait vs mutex synchronized list"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Average Time (ns)"
set logscale y 10
set output 'lab2b_2.png'
set key left top
plot \
     "< grep 'list-none-m,[[:digit:]]*,1000,1,' lab2b_list.csv" using ($2):($7) \
    title 'completion time per operation' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[[:digit:]]*,1000,1,' lab2b_list.csv" using ($2):($8) \
    title 'lock-wait time' with linespoints lc rgb 'green'

set title "Successful Iterations vs. Synchrnoized threads"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Successful Iterations"
set logscale y 10
set output 'lab2b_3.png'
plot \
     "< grep 'list-id-none' lab2b_list.csv" using ($2):($3) \
    title 'unprotected w/ yield=id' with points lc rgb 'red', \
     "< grep 'list-id-s' lab2b_list.csv" using ($2):($3) \
    title 'spin w/ yield=id' with points lc rgb 'green', \
     "< grep 'list-id-m' lab2b_list.csv" using ($2):($3) \
    title 'mutex w/ yield=id' with points lc rgb 'orange'

set title "Throughput vs. Mutex synchronized partitioned list threads"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:]
set ylabel "Throughput (operations per second)"
set logscale y 10
set output 'lab2b_4.png'
set key left top
plot \
     "< grep 'list-none-m,[[:digit:]]*,1000,1,' lab2b_list.csv" using \
    ($2):(1000000000/($7)) title '1 list' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[[:digit:]]*,1000,4,' lab2b_list.csv" using \
    ($2):(1000000000/($7)) title '4 lists' with linespoints lc rgb 'yellow', \
     "< grep 'list-none-m,[[:digit:]]*,1000,8,' lab2b_list.csv" using \
    ($2):(1000000000/($7)) title '8 lists' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,[[:digit:]]*,1000,16,' lab2b_list.csv" using \
    ($2):(1000000000/($7)) title '12 lists' with linespoints lc rgb 'blue'

set title "Scalability-5: Spin-Synchronized Throughput of Partitioned Lists"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:]
set ylabel "Throughput (operations per second)"
set logscale y 10
set output 'lab2b_5.png'
set key left top
plot \
     "< grep 'list-none-s,[[:digit:]]*,1000,1,' lab2b_list.csv" using \
    ($2):(1000000000/($7)) title '1 list' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[[:digit:]]*,1000,4,' lab2b_list.csv" using \
    ($2):(1000000000/($7)) title '4 lists' with linespoints lc rgb 'yellow', \
     "< grep 'list-none-s,[[:digit:]]*,1000,8,' lab2b_list.csv" using \
    ($2):(1000000000/($7)) title '8 lists' with linespoints lc rgb 'green', \
     "< grep 'list-none-s,[[:digit:]]*,1000,16,' lab2b_list.csv" using \
    ($2):(1000000000/($7)) title '16 lists' with linespoints lc rgb 'blue'