[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.10204679.svg)](https://doi.org/10.5281/zenodo.10204679)
# One Automaton To Rule Them All: Beyond Multiple Regular Expressions Execution

This repository contains the source code of the paper ``One Automaton To Rule Them All: Beyond Multiple Regular Expressions Execution'' where we deal with the problem of efficient Regular Expressions (REs) matching.
The key idea of this work is to exploit REs morphological similarities within the same dataset to enable memory reduction when storing the patterns and improving the dataset-matching throughput. 
This idea is expressed through:
1. the Multi-RE Finite State Automata (MFSA) model that extends the Finite State Automata (FSA) model to improve REs parallelization by leveraging similarities within a specific application ruleset
2. a multi-level compilation framework to manage REs merging and optimization to produce MFSA(s) automatically
3. an execution engine called iMFAnt, which is an extension of the state-of-the-art iNFAnt algorithm 

# Table of contents

1. [Repo Overview](#repocode)
2. [CGO'24 AE Instructions](#cgo24ae)
3. [Artifacts Setup](#art-setup)
4. [Running the Experiments](#run-exp)
5. [Charts Visualization](#viz)
6. [Customization Options](#custom-exp)
7. [Credits and Contributors](#credits)

## Repo Overview <a name="repocode"></a>
* **```benchmark_scripts```** and **```benchmark_results```** contain the scripts to evaluate datasets similarity and the numericalresult of such evaluation, respectively (Fig. 1).
* **```datasets```** contains the RE datasets
* **```framework```** contains the compilation framework
* **```input_streams```** contains the data streams to match
* **```matching```** contains the iMFAnt engine
* **```mfsa```** contains the automata in ANML format, resulting from the compilation framework
* **```output_matches```** contains the numeric results of MFSAs analysis (time, #matches)
* **```plots```** contains the plots resulting from the execution of the scripts in ```scripts``` folder
* **```plotting scripts```** contains the scripts for the plotting of the numerical results
* **```raw_results```** contains the numerical results corresponding to the plots 
* **```scripts```** contains the scripts to plot the compression, compilation time, execution time and throughput results (Figs. 7, 8, 9, 10)

To customize the experiments go to [the customization section](#custom-exp)


## Default Configurations <a name="default"></a>

Run the scripts in ```iMFAnt/scripts``` folder to reproduce the experimental results reported in the paper. 
These include the automatic generation of the plots with the default merging factors (0, 1, 2, 5, 10, 20, 50, 100)

# CGO'24 Artifact Evaluation Instructions <a name="cgo24ae"></a>

The following instructions are to reproduce the CGO'24 results.
The fast way is to go to [the artifacts setup](#art-setup)


## Artifacts Setup <a name="art-setup"></a>

An x86 CPU is mandatory for the Docker setup, **other CPUs are usable in general but not tested.**

Clone the repository:

```
git clone https://github.com/necst/iMFAnt.git
```
and navigate into the repo folder

```
cd iMFAnt
```

Please follow one of the two setups: 1) [native Ubuntu-based machine](#ubuntu-setup); 2) [any OS with Docker](#docker-setup) and then to the [Running the Experiments](#run-exp).

### 1) Native Ubuntu-based Machine <a name="ubuntu-setup"></a>

To execute CGO'24 AE are required 
1. `make cmake`
2. `flex`
3. `bison`
4. `g++` 
4. `openmp` (included in `g++` installation)
5. `python3.10` with `numpy pandas matplotlib`

A possible command to install everything you need:

```
sudo apt-get install -y locales curl wget tar sudo git apt-utils nano
sudo apt-get install -y g++ python3 python3-pip automake make cmake flex bison
sudo pip3 install numpy pandas matplotlib
```

navigate into the scripts folder
```
cd scripts/
```

### 2) Any OS with Docker <a name="docker-setup"></a>

It is supposed you have installed and configured Docker according to  [the docker instructions](https://docs.docker.com/engine/install).
An x86 CPU is mandatory.

Open a terminal and then run the script 

```
run_docker.sh
```

that will build and run the docker container.

navigate into the scripts folder
```
cd iMFAnt/scripts/
```

## Running the Experiments <a name="run-exp"></a>

All the experiments take a shorter time than expected if you open them and reduce the number of repetitions, changing the value of ```-r``` flag in the execution of `python3 merging.py -r XXX`.
More info on [experiment customization here](#custom-exp)

Each experiment will execute and collect the corresponding results and concur to the paper figure.

### Compression Results (Figure 7)
To reproduce Fig. 7 (ETA: 5 min)
```
./compression.sh
```
To visualize these results [go here](#viz)

### Compilation Results (Figure 8)
To reproduce Fig. 8 (ETA: 30 min with 30 reps)
```
./compilation_time.sh
```
To visualize these results [go here](#viz)

### iMFAnt Execution Results single-/multi-threaded(Figures 9 and 10)
To reproduce Figs. 9 and 10 (ETA: 12 h with 15 reps)
```
./iMFAnt_performance.sh
```
Figure 9 and 10 are expected to display different optimal merging factor according to testing machine characteristics.
Anyhow, the MFSAs are expected to improve the simple multi-thread scaling according to the trends in the paper.

To visualize these results [go here](#viz)

## Chart Visualization <a name="viz"></a>

The scripts run automatically the desired experiments and you will find the corresponding PDF plots in  `iMFAnt/plots` folder.

If you follow the Docker setup, `exit` the container and you will find there the results.

Then open the PDFs as you prefer.


# Customization Options <a name="custom-exp"></a>

* To merge a new dataset save the RE file and the input stream file in `dataset` and `input_streams` folders, directly. Add the corresponding directory into `framework/compiler/merging.py` file, in ```inputRE``` list and run it to perform the merging.
* To pattern-match a new dataset, add the corresponding `mfsa` and `input_streams` directories to `imfant.py` file and run it, ensuring that the merging factors are consistent with the ones employed during the merging.

### Automata Generation: 
* The following options are available for the compilation of REs in `framework/re2automata/re2automata.cpp`:

    | Variable      | Meaning                                                        |
    | ------------- | -------------------------------------------------------------- |
    | STATES        | if 1 eval. states compression, else eval. trans. compression   |
    | ANML          | if 1 output ANML file, else output DOT file                    |
    | STACK_TIME    | if 1 eval. compilation time                                    |
    | V             | if 1 enable verbose mode                                       |

  These parameters are active by default.

* Enter the compilation framework and compile the compiler: 
  ```cd framework/compiler```
  and build the compiler, entering the following: 

```
cd framework/compiler
bison -d compiler.yy -v
flex -o compiler.yy.cc compiler.lex
```
*  At compile-time, set the desired values of ```STATES``` and ```STACK_TIME``` according to the values in the table:

```
g++ -o compiler compiler.yy.cc compiler.tab.cc ast.cpp ../re2automata/re2automata.cpp -DSTATES=1 -DSTACK_TIME=0 -w -std=c++11 -g -O3
```
* Run 
```
python merging.py -r REPS -b [M1 ... Mn]
``` 
to generate the automata for the benchmark datasets. ```-r``` indicates the number of repetitions to evaluate the compilation time, and ```-b``` indicates the list of merging factors to reproduce the results in the paper. In practice, any value for M is valid. To merge the entire dataset, set M=0. Adjust the merging factors consistently in ```matching/imfant.py``` to run the pattern matching engine.
* The generated automata are in ```mfsa```
* The compression results are in ```raw_results```

### iMFAnt execution customization:

* Compile the matching algorithm: 
```
cd ../../matching/ && make clean all
```
* To match a single folder of MFSAs run: 
```
export OMP_NUM_THREADS=N; make && ./multithreaded_imfant STREAM_IN + MFSA_DIR NUM OUTPUT_FILE
```
where `N` is the number of threads, `STREAM_IN` is the input stream to be matched, `MFSA_DIR` is the directory of the folder containing the MFSAs to match, `NUM` is the number of MFSAs to match in that directory, and `OUTPUT_FILE` contains the results of the matching (matching time, #matches). 

### happy RE matching :)

# Credits and Contributors <a name="credits"></a> 

Contributors: Luisa Cicolini, Filippo Carloni, Marco Domenico Santambrogio, Conficconi Davide

If you find this repository useful, please use the following citation(s):

```
@inproceedings{cicolini2024one,
  title={One Automaton to Rule Them All: Beyond Multiple Regular Expressions Execution},
  author={Cicolini, Luisa and Carloni, Filippo and Santambrogio, Marco D and Conficconi, Davide},
  booktitle={2024 IEEE/ACM International Symposium on Code Generation and Optimization (CGO)},
  pages={193--206},
  year={2024},
  organization={IEEE}
}

```
