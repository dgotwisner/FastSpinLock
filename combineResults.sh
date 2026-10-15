#!/bin/bash

# Combine the 3 benchmark results files with alignment separaters, and run the alignCompareTable.gawk script
# on it.  The output should be pasted in the README.md file in between the <tbody> and the </tbody> block, replacing whatever
# is currently between those lines.

rm -f combinedResults.txt
echo "ALIGNMENT NoAlign" >> combinedResults.txt
cat benchmark/results.txt >> combinedResults.txt
echo "ALIGNMENT Align64" >> combinedResults.txt
cat bench64/results.txt >> combinedResults.txt
echo "ALIGNMENT Align128" >> combinedResults.txt
cat bench128/results.txt >> combinedResults.txt

rm -f readmeHtml.txt
awk -f alignCompareTable.gawk combinedResults.txt > readmeHtml.txt
awk -f alignCompareTable-alignment-results.gawk combinedResults.txt > docs/alignment-results.md
