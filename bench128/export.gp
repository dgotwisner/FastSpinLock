set terminal push
set terminal png size 1280,1024
set output sprintf("%s.png", outfile)
replot
set output
set terminal pop
