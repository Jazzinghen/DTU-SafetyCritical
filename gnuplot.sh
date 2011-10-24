f1(x) = (8711.27 * x)
f2(x) = 86832.1155 + (135.72 * x)
f3(x) = 1283458.546 + (135.72 * x)


plot[c=0:10000] f1(c) title "On-The-Fly", f2(c) title "Lookup Table", f3(c) title "Lookup Table + Generation"

set title "Correction times"
set xlabel "Codewords"
set ylabel "Time (in nanoseconds)"
set key left top
set terminal png small size 800,600 enhance
set output 'EncodingDecodingTimes.png'

replot
