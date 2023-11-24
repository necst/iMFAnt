#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <chrono>

#ifndef INFANT_H_
#define INFANT_H_

#define ALPHA_SIZE 256
#define EPS 6


using namespace std;


struct pathStates{
    // if upperbound is -1 --> inf
    std::vector<uint16_t> chars;
    std::vector<int>  states;
};

// non uso le string perchè sono troppo grandi (in media 3/4 char mi sono suff)
// probabilmente tutte le size cose non mi servono più

struct range{
    int start, end, len, neg;
    std::vector<uint16_t> ref;
};

struct mergedCOO{
    int                                 mergedSoFar, nextFree;
    std::vector<uint16_t>               nonzeroValues;
    std::vector<int>                    rowIdx, colIdx, initialStates, finalStates, IbelongsTo;
    std::vector<std::vector<int> >      loopBelongsTo, rangeBelongsTo, belongsTo, FbelongsTo;
    std::vector<range>                  ranges;
    std::vector<int>                    anchored_sol_states;
};

struct transition{
    // transition from src to dst
    int src, dst;
    // REs for which the state is final/initial
    vector<int> isInit, isFin;
    // REs for which the transition is available
    vector<int> available;
};

struct multi_move{
    int from; 
    int to; 
    vector<int> *active; 
};

struct matching_path{
    vector<int> *sv; 
    vector<vector<int> *> *active_re;
};

struct char_vector {
    // char enabling the transitions in the vector
    uint16_t enabling_character;
    vector<transition> *pointers_to_transition;
};

struct mfsa {
    vector<char_vector> *alphabet;
    vector<int> anchor_states;
};

vector<int> isInit_trans(mergedCOO *m, int idx);

vector<int> isFin_trans(mergedCOO *m, int idx);

vector<uint16_t> ref_to_list(vector<uint16_t> ref);

int correspondingRange(mergedCOO *m, int start, int end); 

void printAutomaton_multi(mergedCOO* matrix);

vector<uint16_t> hex_string_to_char_vec(string hex);

vector<int> string_to_int_vec(string s); 

vector<string> line_to_strings(string line); 

void load_nfa_from_file(string &filename,  mergedCOO *m ); 

int inRange(uint16_t c, vector<uint16_t> range_ref);

void mergedCOO2MiNFAnt(mergedCOO *m, mfsa *nfa); 

void print_nfa(mfsa *nfa); 

void print_mp(matching_path *mp); 

vector<int> check_match(vector<int> finals, vector<int> *active);

void activate(vector<multi_move> *to_activate, matching_path *mp); 

int valid_transition(vector<int> *currently_active, vector<int> available); 

int infant(matching_path *mp, mfsa *nfa, string& input);

int max(mergedCOO *m);

#endif