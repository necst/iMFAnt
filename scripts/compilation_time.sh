#!/usr/bin/bash

cd ..

mkdir -p mfsa

mkdir -p raw_results

mkdir -p plots

cd framework/compiler/

bison -d compiler.yy -v

flex -o compiler.yy.cc compiler.lex

g++ -o compiler compiler.yy.cc compiler.tab.cc ast.cpp ../re2automata/re2automata.cpp -DSTATES=0 -DSTACK_TIME=1 -w -std=c++11 -g -O3

python3 merging.py -r 30 -b 1 2 5 10 20 50 100 0

cd ../../plotting_scripts/

python3 plotter_comp_time.py
