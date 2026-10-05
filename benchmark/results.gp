# Plot FastSpinLock vs StdMutex behavior
# X access is number of cores
# Y access is locks/second (graph 1)
# Y access is usrTime+sysTime (graph 1)

# Cores vs locks per second
set title "Cores vs locks/second"
set xlabel "Cores"
set ylabel "Locks / second"
set xrange [0:30]
set format y "%f"

plot "myMutex.out" using 3:9 with lp title "FastSpinLock", \
     "stdMutex.out" using 3:9 with lp title "Std::mutex"

outfile="locksPerSecond"
load "export.gp"

set title "Cores vs locks/second"
set xlabel "Cores"
set ylabel "user+sys time"

plot "myMutex.out" using 3:($5+$6) with lp title "FastSpinLock", \
     "stdMutex.out" using 3:($5+$6) with lp title "Std::mutex"

outfile="user+sysTime"
load "export.gp"
