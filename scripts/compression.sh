#!/usr/bin/bash

cd ../

mkdir -p mfsa

mkdir -p raw_results

mkdir -p plots

cd framework/compiler/

bison -d compiler.yy -v

flex -o compiler.yy.cc compiler.lex

g++ -o compiler compiler.yy.cc compiler.tab.cc ast.cpp ../re2automata/re2automata.cpp -DSTATES=1 -DSTACK_TIME=0 -w -std=c++11 -g -O3

python3 merging.py -r 1 -b 1 2 5 10 20 50 100 0

cd ../../plotting_scripts/

python3 states_comp.py -c 1 2 5 10 20 50 100 0

cd ../framework/compiler/

bison -d compiler.yy -v

flex -o compiler.yy.cc compiler.lex

g++ -o compiler compiler.yy.cc compiler.tab.cc ast.cpp ../re2automata/re2automata.cpp -DSTATES=0 -DSTACK_TIME=0 -w -std=c++11 -g -O3

python3 merging.py -r 1 -b 1 2 5 10 20 50 100 0

cd ../../plotting_scripts/

python3 transitions_comp.py -c 1 2 5 10 20 50 100 0

python3 plotter_compression.py

