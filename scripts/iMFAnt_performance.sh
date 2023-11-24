#!/usr/bin/bash

cd ..

mkdir -p mfsa

mkdir -p raw_results

mkdir -p output_matches

mkdir -p plots

cd framework/compiler/

bison -d compiler.yy -v

flex -o compiler.yy.cc compiler.lex

g++ -o compiler compiler.yy.cc compiler.tab.cc ast.cpp ../re2automata/re2automata.cpp -DSTATES=1 -DSTACK_TIME=0 -w -std=c++11 -g -O3

python3 merging.py -r 1 -b 1 2 5 10 20 50 100 0

cd ../../matching/

make clean all

python3 imfant.py

cd ../plotting_scripts/

python3 plotter_iMFAnt_single.py

rm tmp.txt

python3 plotter_iMFAnt_multi.py

rm tmp.txt
