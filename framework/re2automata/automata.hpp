#include <iostream>
#include <iterator>
#include <list>
#include <vector>
#include "../compiler/ast.hpp"

#ifndef AUTOMATA_H_
#define AUTOMATA_H_

#define INPUTFILE "stdin"
#define OUTPUTFILE "stdout"


// vectors when indexed access is moslty used
// lists otherwise 
struct pathStates{
    // if upperbound is -1 --> inf
    std::vector<uint16_t> chars;
    std::vector<int>  states;
};

struct loop{
    // if upperbound is -1 --> inf
    int upperBound, lowerBound, init, end, nested;
    pathStates *path;
};

// non uso le string perchè sono troppo grandi (in media 3/4 char mi sono suff)
// probabilmente tutte le size cose non mi servono più

struct range{
    int start, end, len, neg;
    std::vector<uint16_t> ref;
};

struct singleCOO{
    std::vector<uint16_t>       nonzeroValues;
    std::vector<int>        rowIdx, colIdx, finalStates;
    int                     nextFree, initialState, lastUsed, 
                            id;
    std::vector<loop>       loops;
    std::vector<range>      ranges;
};


struct mergingTab{
    std::vector<int>        oldStates, newStates;
};

struct swapTable{
    std::vector<int>        oldComplete, newComplete;
};


struct mergedCOO{
    int                                 mergedSoFar, nextFree;
    std::vector<uint16_t>               nonzeroValues;
    std::vector<int>                    rowIdx, colIdx, initialStates, finalStates, IbelongsTo;
    std::vector<std::vector<int> >      loopBelongsTo, rangeBelongsTo, belongsTo, FbelongsTo;
    std::vector<range>                  ranges;   
};

typedef std::vector<singleCOO*> toBeMerged;

typedef std::vector<mergingTab> mergingSet;

float avg(std::vector<int> v);

singleCOO* thompson(AstNodePtr root, int startingPt);

void addFinal(singleCOO *a);

void printAutomatonVerbose(singleCOO *matrix);

void writeToFile(singleCOO *a, std::string output);

singleCOO* initializedCOO();

mergedCOO* merge_multi(toBeMerged *set);

void addSubexp(singleCOO *a, int start, AstNodePtr node, int orFlag, int loopFlag, int anchorFlag);

void printAutomaton(singleCOO *matrix);

void translateAST(OptStruct *opt, AstNodePtr root, char *output);

void parseFile(std::string input, std::string output, int n);

void printAutomaton_multi(mergedCOO* matrix);

void generateNew_multi(mergedCOO* to, singleCOO* from);

int findCommonSub_multi(mergedCOO* m, singleCOO* a, mergingSet *ms, int tmpFound);

void profiler(std::vector<AstNodePtr> *mrg);

int findCommonRange_multi(mergedCOO* m, singleCOO* a, mergingSet *ms, int tmpFound);

int findCommonLoop_multi(mergedCOO* m, singleCOO* a, mergingSet *ms, int tmpFound);

int checkLoop_multi(mergedCOO* m, singleCOO* a, int s1, int f1, int s2, int f2);

int notInLoop_multi(mergedCOO* a, int s, int e);

int compareLoops(loop l1, loop l2);

int checkStatesCompatibility_multi(mergedCOO* m, singleCOO* a, int idx1, int idx2, int eq);

int compareRanges(range r1, range r2);

void addNew(singleCOO* a, mergedCOO* m, int offset);

mergingTab* mergeStructures(mergingSet *m, int s);

void buildSwappingTable_multi(swapTable* t, mergingTab *ms, mergedCOO* m, singleCOO* a, int next);

void printAutomatonVerbose_multi(mergedCOO* a);

void swap(swapTable* t, singleCOO* a);

int returnIdx(int * array, int state);

int compareEdges(singleCOO *a, mergedCOO *m, int a1, int m1);

int involvedInMerging(mergingTab *m, int state);

void loopsAnalysis_multi(mergedCOO* a);

mergingTab* checkConflictingTables(mergingSet *ms);

int involvedInMergingSet(mergingSet *ms, int state);

int isConflict(int state, std::vector<int> doNot);

void printDot(mergedCOO* a, std::string fileName);

int alreadySwapped(swapTable *t, int state);

int computeMax(mergedCOO *m);

int existsArc(singleCOO* a, int state1, int state2);

void addTwin(mergedCOO m, int id);

singleCOO *addAutomaton(AstNodePtr ast, int startingPt);

int merge_automata(std::vector<AstNodePtr> *mrg, int batch, char *out, char *time_dir, int dur_fe);

int existsArc_multi(mergedCOO* a, int state1, int state2);

int isInvolvedInLoopPath_multi(mergedCOO *a, int state);

int isInvolvedInLoopPath(singleCOO *a, int state);

void removeEps(singleCOO *a);

int notInFinalNorInitial(singleCOO *a, int state);

int notInOR(singleCOO *a, int idx);

void printANML(mergedCOO* a, char *fileName, int offset);

void printANML_s(singleCOO* a, char *fileName);

int evaluateFinalMerging (mergedCOO* m, singleCOO* a, int state_a, int state_m);

std::vector<int> isFin_m(mergedCOO *m, int state);

std::vector<int> isFin_s(singleCOO *a, int state);

int countStates_s(singleCOO *a);

int countStates_m(mergedCOO *a);

int countTrans_s(singleCOO *a);

int countTrans_m(mergedCOO *a);

int isLoopFinInf(mergedCOO *m, int state);

int isLoopFinInf_s(singleCOO *m, int state);

void loopExpansion_s(singleCOO *m);

int isLoopInitNull(mergedCOO *m, int state);

int correspondingRange(mergedCOO *m, int start, int end);

int correspondingRange_s(singleCOO *m, int start, int end);

// void removeEps(mergedCOO *a);

std::vector<int> nextEps(singleCOO *m, int state);

void cleaning(mergedCOO *m, std::vector<int> toEraseF, std::vector<int> toEraseS);

void allowedPaths(mergedCOO *m, int state, int id, std::vector<int> &currP, char *fileName, std::vector<std::vector<int> > *paths);

void lookForPaths(mergedCOO *m, char *fileName);

void simpleSwap(singleCOO *m, int o, int n);

void simpleSwap_m(mergedCOO *m, int o, int n);

int correspondingRange_s(singleCOO *m, int start, int end);

std::vector<int> computeMult(singleCOO *m, int from, int to);

std::vector<int> computeMult_m(mergedCOO *m, int from, int to);

void loopExpansion(mergedCOO *m);

void loopExpansion_s(singleCOO *m);

std::vector<int> ambiguityAnalysis(mergedCOO *m, std::vector<int> path);

int ambiguousAddition(std::vector<int> v, int s);

int arcIdx(mergedCOO *m, int from, int to);

void or2range(mergedCOO *m);

void or2range_s(singleCOO *m);

int isLoopInitNull_s(singleCOO *m, int state);

int computeMax_s(singleCOO *m);

int arcIdx_s(singleCOO *m, int from, int to);

void or2range_s(singleCOO *m);

void backpropagateOR(mergedCOO *m);

int belongsToLoop(mergedCOO *m, int loop, std::vector<int> id);

int belongsToRange(mergedCOO *m, int range, std::vector<int> id);

int belongsToArc(mergedCOO *m, int arc, std::vector<int> id);

void removeDup(singleCOO *m);

int isLoop(mergedCOO *m, int from, int to);

int isLoop_s(mergedCOO *m, int from, int to);

void printStd(mergedCOO *a, char *out);

void states_renumbering(singleCOO *s);

#endif
