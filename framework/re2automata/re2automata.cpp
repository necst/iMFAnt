#include <vector>
#include <list>
#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <ostream>
#include <fstream>
#include <string.h>
#include <algorithm>
#include <cstdlib>
#include <chrono>
#include "../compiler/ast.hpp"
#include "automata.hpp"
#include <sstream> 
#include <bitset>


#define EPS 6
#define INF 102
#define SEP 44
#define MINUS 45
#define ANML 1
#define HEX 16
#define V 0
#define ALPHA_SIZE 256
#define REPS 10

using namespace std;


/**
 * Inserts transition in a FSA
 * @param matrix contains the FSA
 * @param value contains the character enabling the transition
 * @param row is the transition starting state
 * @param col is the transition arrival state
 * @param flag_mask is enabled in case the transition is initial and start-of-line token is enabled
 */

void insertCOO(singleCOO *matrix, char value, int row, int col, int flag_mask)
{
    int mask_sol = 1 << 9; 

    unsigned char c = value; 

    uint16_t v = (uint16_t) c; 

    if(flag_mask){
        v = v | mask_sol; 
    }

    if(V)
        cout<<"inserting "<<v<<" from "<<row<<" to "<<col<<endl;

    matrix->nonzeroValues.push_back(v);
    matrix->rowIdx.push_back(row);
    matrix->colIdx.push_back(col);

    if(value!=EPS)
        matrix->nextFree++;

    matrix->lastUsed = col;

};

/**
 * Given an AST and an initial state returns a FSA rooted in that initial state
 * @param root is the AST
 * @param startingPt is the FSA starting point
 * @return FSA rooted in startingPT
 */
singleCOO* thompson(AstNodePtr root, int startingPt){
    // printf("beginning of thompson\n");

    // printAST(root,0);

    singleCOO* a = new singleCOO;
    a->nextFree = startingPt;

    a->nextFree=startingPt+1;
    a->lastUsed = startingPt;


    addSubexp(a, startingPt, root, -1, -1, 0);



    addFinal(a);


    if(V)
        printAutomaton(a);

    return a;
}

/**
 * Given a FSA, checks which states are final and marks them as such in a proper structure
 * @param a is the FSA
 */
void addFinal(singleCOO *a){
    for(int i=0;i<(a->rowIdx.size());i++){
        int toCheck=-1;
        int isFinal=1;
        // check state a->colIdx[i]
        toCheck=a->colIdx[i];
        for(int j=0;j<a->rowIdx.size();j++){
            if(toCheck==a->rowIdx[j]) isFinal=0;
        }
        if(isFinal){
            // if no transition is rooted in that state, the state is final
            a->finalStates.push_back(toCheck);
        }
    }
}

/**
 * Given an AST, recursively adds each node as one or more transition in the FSA
 * @param a is the FSA
 * @param start is the initial state for the transition to be encoded from the current node
 * @param node is the AST node 
 * @param orFlag is 1 if the current node is part of an OR-ed expression
 * @param loopFlag is 1 the current node is part of a quantified expression
 * @param anchorFlag is 1 if the current node is subject to start-of-line anchoring
 */
void addSubexp(singleCOO* a, int start, AstNodePtr node, int orFlag, int loopFlag, int anchorFlag)
{
    if (node == NULL)
        return;
    if(node->eKind == ANCHOR_SOL_NODE){
        if(V)
            printf("-- ANCHOR\n");
        addSubexp(a, start, node->brother, orFlag, loopFlag, 1);
    }
    if (node->eKind == OPCP_EXP){   
        if(V)
            printf("-- OPCP\n");
        addSubexp(a, start, node->child, -1, loopFlag, anchorFlag);
        if(orFlag==-1)
            addSubexp(a, a->lastUsed, node->brother, -1, loopFlag, 0);
    }
    else if (node->eKind == AND_EXP){
        if(V)
            printf("-- AND %d \n", (uint16_t)node->reference[0]);
        if(loopFlag!=-1){
            a->loops[loopFlag].path->states.push_back(start);
            a->loops[loopFlag].path->states.push_back(a->nextFree);
            a->loops[loopFlag].path->chars.push_back((uint16_t)(*(node->reference)));
        }   

        unsigned char c = *(node->reference);


        insertCOO(a, *(node->reference), start, a->nextFree, anchorFlag);

        
        for(int i=1;i<node->referenceLength;i++){
            if(loopFlag!=-1){
                a->loops[loopFlag].path->states.push_back(a->nextFree);
                a->loops[loopFlag].path->chars.push_back((uint16_t)(*(node->reference+i)));
            }
            unsigned char c = *(node->reference+i);
            insertCOO(a, *(node->reference+i), a->lastUsed, a->nextFree, 0);
        }
        
        if(orFlag == -1)
            addSubexp(a, a->lastUsed, node->brother, orFlag, loopFlag, 0);

    }
    else if (node->eKind == OR_EXP){
        if(V)
            printf("-- OR\n");
        if(node->referenceLength > 0){
            // single characters alternations are treated as a range (character class)
            insertCOO(a, EPS, start, a->nextFree, anchorFlag);
            if(loopFlag!=-1){
                a->loops[loopFlag].path->states.push_back(start);
                a->loops[loopFlag].path->states.push_back(a->nextFree);
                a->loops[loopFlag].path->chars.push_back(EPS);
            }
            a->nextFree++;
            range r;
            r.end = a->lastUsed; 
            r.start = start;
            r.neg=0;
            // compute range ref's length
            int l = 0;
            int w = 0;
            for(int k=0;k<node->referenceLength;k++){
                r.ref.push_back((uint16_t)node->reference[k]);
            }
            r.len = node->referenceLength; 
            a->ranges.push_back(r);
            // add brother
            if(orFlag == -1)
                addSubexp(a, a->lastUsed, node->brother, orFlag, loopFlag, 0);   

        } else {
            int final = a->nextFree;
            int initial = start;
            a->nextFree++;
            // add children
            AstNodePtr tmp = node->child; 
            while(tmp!=NULL){
                addSubexp(a, initial, tmp, 1, loopFlag, anchorFlag);
                insertCOO(a, EPS, a->lastUsed, final, 0);
                if(loopFlag!=-1){
                    a->loops[loopFlag].path->states.push_back(final);
                    a->loops[loopFlag].path->chars.push_back(EPS);
                }
                tmp = tmp->brother; 
            }
            // add brother
            addSubexp(a, final, node->brother, -1, loopFlag, 0);  

        }
    }
    else if (node->eKind==NOT_OR_EXP){
        if(V)
            printf("-- NOT EXP\n");
        AstNodePtr tmp = node->child; 
        range r; 
        insertCOO(a, EPS, start, a->nextFree, anchorFlag);
        if(loopFlag!=-1){
            a->loops[loopFlag].path->states.push_back(start);
            a->loops[loopFlag].path->states.push_back(a->nextFree);
            a->loops[loopFlag].path->chars.push_back(EPS);
        }
        a->nextFree++;
        r.end = a->lastUsed; 
        r.start = start;
        int l = 0;
        while(tmp!=NULL){
            l+=tmp->referenceLength;
            tmp = tmp->brother; 
        }
        tmp = node->child;
        int writing = 0;
        while(tmp!=NULL){
            AstNodePtr tmp2 = tmp->brother;
            // list all characters/ranges
            if(tmp->eKind == OPCP_EXP)
                tmp = tmp->child;
            if(tmp->eKind!=RANGE_EXP){
                // pushing reference in range
                for(int k=0;k<tmp->referenceLength;k++){
                    r.ref.push_back((uint16_t)tmp->reference[k]);
                    writing++;
                }
            } else {
                for(int k=0;k<tmp->referenceLength;k+=2){
                    r.ref.push_back((uint16_t)tmp->reference[k]);
                    writing++;
                    r.ref.push_back(MINUS);
                    writing++;
                    r.ref.push_back((uint16_t)tmp->reference[k+1]);
                    writing++;
                }
            }
            
            tmp = tmp2; 
        }
        r.len = writing; 
        r.neg = 1; 
        a->ranges.push_back(r);
        // add brother
        if(orFlag==-1)
            addSubexp(a, a->lastUsed, node->brother, orFlag, loopFlag, 0);
    }
    else if (node->eKind == RANGE_EXP){
        if(V)
            printf("-- RANGE\n");      
        insertCOO(a, EPS, start, a->nextFree, anchorFlag);
        if(loopFlag!=-1){
            a->loops[loopFlag].path->states.push_back(start);
            a->loops[loopFlag].path->states.push_back(a->nextFree);
            a->loops[loopFlag].path->chars.push_back(EPS);
        }
        a->nextFree++;
        range r;
        r.end = a->lastUsed; 
        r.start = start;
        r.neg=0;
        // compute range ref's length
        int l = 0;
        int w = 0;
        uint16_t mask_rg = 1 << 8; 
        for(int k=0;k<node->referenceLength;k+=2){
            r.ref.push_back((uint16_t)node->reference[k]);
            r.ref.push_back(MINUS | mask_rg);
            r.ref.push_back((uint16_t)node->reference[k+1]);
            w+=3;
        }
        r.len = w; 
        a->ranges.push_back(r);
        // add brother
        if(orFlag == -1)
            addSubexp(a, a->lastUsed, node->brother, orFlag, loopFlag, 0);   
    }
    else if (node->eKind == QUANTIFIER_EXP){
        if(V)
            printf("-- QUANT\n");
        int currLoop = a->loops.size();
        loop l;
        l.init = start; 
        
        int lowerBound = -1, upperBound = -1;
        if (*(node->reference + node->referenceLength - 1) == INF)
        {
            // infinite loop
            unsigned char c = *(node->reference);
            int lLen = 0;
            while (c != SEP)
            {
                lLen++;
                c = *(node->reference + lLen);
            }
            std::string tmpL;
            for (int i = 0; i < lLen; i++)
            {
                tmpL.push_back(node->reference[i]);
            }
            lowerBound = stoi(tmpL);
            upperBound = -1;
        }
        else if(*(node->reference + node->referenceLength - 1) == SEP){
            unsigned char c = *(node->reference);
            int lLen = 0;
            while (c != SEP)
            {
                lLen++;
                c = *(node->reference + lLen);
            }
            std::string tmpL;
            for (int i = 0; i < lLen; i++)
            {
                tmpL.push_back(node->reference[i]);
            }
            lowerBound = stoi(tmpL);
            upperBound = -1;
        }
        else
        {
            int tmpEnd = a->nextFree;
            a->nextFree++;
            unsigned char c = *(node->reference);
            int uLen = 0, lLen = 0;

            while (c != SEP && lLen < node->referenceLength)
            {
                lLen++;
                c = *(node->reference + lLen);

            }

            uLen = node->referenceLength - lLen - 1;
            if(uLen<0){
                uLen=0;
            }

            std::string tmpL;
             
            for (int i = 0; i < lLen; i++)
            {
                tmpL.push_back(node->reference[i]);
            }
            lowerBound = stoi(tmpL);
            if (uLen!=0){
                std::string tmpU;
                for (int i = lLen + 1; i < node->referenceLength; i++)
                {
                    tmpU.push_back(node->reference[i]);
                }
                upperBound = stoi(tmpU);
            } else {
                upperBound = lowerBound;
            }
                
        }
        
        l.upperBound=upperBound;
        l.lowerBound=lowerBound;
        a->loops.push_back(l);
        a->loops[currLoop].path = new pathStates;
        addSubexp(a, start, node->child, orFlag, currLoop, anchorFlag);
        // management of nested loops
        if(loopFlag!=-1){
            // push all newly added states to the nesting loop as well
            for(int j=0;j<a->loops[currLoop].path->states.size();j++)
                a->loops[loopFlag].path->states.push_back(a->loops[currLoop].path->states[j]);
            for(int j=0;j<a->loops[currLoop].path->chars.size();j++)
                a->loops[loopFlag].path->chars.push_back((uint16_t) a->loops[currLoop].path->chars[j]);
        }

        a->loops[currLoop].end = a->lastUsed;
        // add brother
        if(orFlag == -1)
            addSubexp(a, a->lastUsed, node->brother, orFlag, -1, 0);
    } 
}

/**
 * Prints a single FSA
 * @param matrix is the FSA
 */
void printAutomaton(singleCOO *matrix)
{
    std::cout<<"AUTOMATA TRANSITIONS\n";

    cout<<"initial state is "<<dec<<matrix->initialState<<endl;

    for (int i = 0; i < matrix->nonzeroValues.size(); i++)
    {
        std::cout<<dec<< matrix->rowIdx[i]<<"--"<<hex<< matrix->nonzeroValues[i]<< "->"<<dec<< matrix->colIdx[i]<<endl;
    }

    std::cout<<"BOUNDED LOOPS:\n";
    if (matrix->loops.size() == 0)
        printf("none\n");
    else {
        for (int i = 0; i < matrix->loops.size(); i++)
        {
            std::cout<<"lb = "<<dec<<matrix->loops[i].lowerBound<<", ub = "<<dec<<matrix->loops[i].upperBound;
            std::cout<<"\n from "<< matrix->loops[i].init <<" to "<<matrix->loops[i].end<<endl;
            std::cout<<"involving states "; 
            for(int k=0;k<matrix->loops[i].path->states.size();k++)
                std::cout<<matrix->loops[i].path->states[k]<<" ";
            std::cout<<"\nand chars ";
            for(int k=0;k<matrix->loops[i].path->chars.size();k++)
                std::cout<<matrix->loops[i].path->chars[k]<<" ";
        }

    }

    std::cout<<"\nRANGE STRINGS\n";
    for(int i=0;i<matrix->ranges.size();i++){
        std::cout<<"RANGE chars from state "<<dec<<matrix->ranges[i].start<<" to state "<<dec<<matrix->ranges[i].end<<" with length "<<matrix->ranges[i].ref.size()<<":\nchars: ";

        for(int j=0;j<matrix->ranges[i].len;j++){
            cout<<hex<<matrix->ranges[i].ref[j]<<" ";
            // std::cout<< matrix->ranges[i].ref[j];
        }
        if(matrix->ranges[i].neg){
            std::cout<<"\nnegation: TRUE"<<endl;
        }else{
            std::cout<<"\nnegation: FALSE"<<endl ;
        }
    }
    std::cout<<"\nFINAL STATES:\n";
    for(int i=0;i<matrix->finalStates.size();i++)
        std::cout<<dec<<matrix->finalStates[i];
    std::cout<<endl;
};


/**
 * Prints a MFSA
 * @param matrix is the MFSA
 */
void printAutomaton_multi(mergedCOO* matrix)
{
    std::cout<<"initial states"<<endl;
    for(int i=0;i<matrix->initialStates.size();i++){
        std::cout<<matrix->initialStates[i]<<" belongs to "<<dec<<matrix->IbelongsTo[i]<<endl;
    }
    std::cout<<"\ntransitions:"<<endl;
    for(int i=0;i<matrix->nonzeroValues.size();i++){
        std::cout<<endl<<matrix->rowIdx[i]<<"--"<<hex<<matrix->nonzeroValues[i]<<"->"<<matrix->colIdx[i]<<endl;
        std::cout<<"belongs to: "<<dec<<endl; 
        for(int j=0;j<matrix->belongsTo[i].size();j++)
            std::cout<<matrix->belongsTo[i][j]<<" ";
    }
    std::cout<<"\nfinal states"<<endl;
    for(int i=0;i<matrix->finalStates.size();i++){
        std::cout<<matrix->finalStates[i]<<" belongs to ";
        for(int j=0;j<matrix->FbelongsTo[i].size();j++)
            std::cout<<dec<<matrix->FbelongsTo[i][j]<<" ";
        std::cout<<endl;
    }
    
    
    std::cout<<"\nrange expressions"<<endl;
    for(int i=0;i<matrix->ranges.size();i++){
        std::cout<<"from "<<dec<<matrix->ranges[i].start<<" to "<<dec <<matrix->ranges[i].end<<", neg = "<<matrix->ranges[i].neg<<" with reference: ";
        for(int j=0;j<matrix->ranges[i].len;j++)
            std::cout<<hex<<matrix->ranges[i].ref[j];
        std::cout<<" belongs to: ";
        for(int j=0;j<matrix->rangeBelongsTo[i].size();j++)
            std::cout<<dec<<matrix->rangeBelongsTo[i][j]<<" ";
        std::cout<<endl;
    }
    if(matrix->ranges.size()==0) std::cout<<"\nno range expressions\n";
    std::cout<<endl; 
};

/**
 * Fixes ranges (character classes expressions) connecting the same couple of states s1 and s2, 
 * merging them into a single character class
 * @param a is the FSA
 */
void fixRange(singleCOO *a){
    vector<int> to_discard;
    for(int s = 0; s<a->ranges.size(); s++){
        for(int t=0; t<s; t++){
            // check whether any two ranges connect the same couple of state start, end
            if(s!=t && (a->ranges[s].start==a->ranges[t].start)&&(a->ranges[s].end==a->ranges[t].end)&&(a->ranges[s].neg==a->ranges[t].neg)){
                for(int i=0; i<a->ranges[s].len; i++){
                    int char_pres = 0; 
                    for(int j=0;j<a->ranges[t].len;j++){
                        if(a->ranges[s].ref[i]==a->ranges[t].ref[j]){
                            char_pres = 1; 
                        }
                    }
                    if(!char_pres){
                        a->ranges[t].ref.push_back(a->ranges[s].ref[i]);
                        a->ranges[t].len++;
                    }    
                }
                to_discard.push_back(s);
            }
        }

    }
    if(to_discard.size()>0){
        for(int i=to_discard.size()-1; i>=0; i--){
            a->ranges.erase(a->ranges.begin()+to_discard[i]);
        }
    }
    
}

/**
 * Given a FSA and a starting state, optimizes the AST ad produces the FSA
 * @param ast is the AST
 * @param st is the starting point
 * @return the corresponding FSA
 */
singleCOO* addAutomaton(AstNodePtr ast, int st){

    // printAST(ast, 0);

    int numInstr;

	numInstr = instructionCounter(ast);

    // fix lazy operators 
    fixLazy(ast);
    // these functions all fix AST structure 

	fixAST(ast, 0, 0); 

	numInstr = addressMgmt(ast, 0);

	jumpCalculator(ast, numInstr, NOOP_EXP);

    // translation from ast to fsa
	singleCOO *m = thompson(ast, st);
    m->initialState=m->rowIdx[0];
    return m;
}

int merged(mergingSet *ms, int state){
    for(int i=0;i<ms->size(); i++){
        for(int j=0; j<ms->at(i).oldStates.size(); j++){
            if (ms->at(i).oldStates[j] == state)
                return 1;
        }
    }
    return 0;
}



/**
 * Look for common character classess between a FSA and a MFSA
 * @param m is the MFSA
 * @param a is the FSA
 * @param ms is a structure to be filled with the states indicating the transitions to be merged
 * @param prevFound keeps the count of currently found structures to be merged (either from common character classes or sub-paths)
 * @return an updated counter of structures to be merged
 */
int findCommonRange_multi(mergedCOO* m, singleCOO* a, mergingSet *ms, int prevFound){
    if(V){
        // cout<<"\n\nmulti"<<endl;
        // printAutomaton_multi(m);
        // cout<<"\n\nsingle"<<endl;
        // printAutomaton(a);
    }

      
    for(int i=0;i<m->ranges.size();i++){
        for(int j=0;j<a->ranges.size();j++){
            // compare all ranges in the FSA and MFSA
            if(a->ranges[j].neg==m->ranges[i].neg
            && a->ranges[j].len==m->ranges[i].len
            && !merged(ms, a->ranges[j].start)&& !merged(ms, a->ranges[j].end)){

                cout<<"\ncomparing ";
                for(auto i: a->ranges[j].ref)
                    cout<<i<<" ";
                cout<<" and ";
                for(auto i: m->ranges[i].ref)
                    cout<<i<<" ";

                int cnt = 0;
                int flag = 0;
                vector<uint16_t> s1; 
                vector<uint16_t> s2;
                for(int k=0;k<a->ranges[j].len;k++){
                    s1.push_back((uint16_t)a->ranges[j].ref[k]);
                }
                for(int k=0;k<m->ranges[i].len;k++){
                    s2.push_back((uint16_t)m->ranges[i].ref[k]);
                }

                // check whether the character classes contain the same characters

                bitset<ALPHA_SIZE> visited1;
                uint16_t mask_rg = 1 << 8;
                uint16_t mask_sol = 1 << 9;

                for(int k=0; k<a->ranges[j].len; k++){
                    if(a->ranges[j].ref[k] == (mask_rg | MINUS)){
                        for(int l=a->ranges[j].ref[k-1]+1; l<a->ranges[j].ref[k+1]; l++){
                            visited1.set(l);
                        }
                    } else {
                        visited1.set(a->ranges[j].ref[k]);
                    }
                }


                bitset<ALPHA_SIZE> visited2;

                for(int k=0; k<m->ranges[i].len; k++){
                    if(m->ranges[i].ref[k] == (mask_rg | MINUS)){
                        for(int l=m->ranges[i].ref[k-1]+1; l<m->ranges[i].ref[k+1]; l++){
                            visited2.set(l);
                        }
                    } else {
                        visited2.set(m->ranges[i].ref[k]);
                    }
                }

                    
                // for(int k=0; k<ALPHA_SIZE; k++){
                //     if(visited1[k] && visited2[k]){
                //         cnt++;
                //     }
                // }

                if ((visited1 & visited2).count() == m->ranges[i].len)
                    flag = 1;



                
                if(flag && !existsArc(a, a->ranges[j].start, ms)){
                    // if so, save starting and final states to merge the transition described by the character class
                    mergingTab mt;
                    cout<<"MERGED"<<endl;
                    cout<<"replacing "<<dec<<a->ranges[j].start<<" with "<<m->ranges[i].start<<endl;
                    cout<<"replacing "<<dec<<a->ranges[j].end<<" with "<<m->ranges[i].end<<endl;

                    mt.newStates.push_back(m->ranges[i].start);
                    mt.oldStates.push_back(a->ranges[j].start);
                    mt.newStates.push_back(m->ranges[i].end);
                    mt.oldStates.push_back(a->ranges[j].end);
                    ms->push_back(mt);
                    prevFound++;
                }
            }
        }
    }
    return prevFound;
}


/**
 * Checks whether a transition is initial for a MFSA
 * @param idx is the index of the transition in the MFSA
 * @param m is the MFSA
 * @return vector of FSAs for which the transition is initial
 */
vector<int> isInit_trans(mergedCOO *m, int idx){
    vector<int> r;
    for(int i=0; i<m->initialStates.size(); i++){
        if(m->rowIdx[idx] == m->initialStates[i]){
            int common = 0;
            for(int k=0; k<m->belongsTo[idx].size(); k++){
                if(m->IbelongsTo[i]==m->belongsTo[idx][k]){
                    r.push_back(m->IbelongsTo[i]);
                }
            }
        }
    }
    sort(r.begin(), r.end());
    r.erase(unique(r.begin(), r.end()), r.end());
    return r; 
}

/**
 * Checks whether a transition is final for a MFSA
 * @param idx is the index of the transition in the MFSA
 * @param m is the MFSA
 * @return vector of FSAs for which the transition is final
 */
vector<int> isFin_trans(mergedCOO *m, int idx){
    vector<int> r; 
    for(int i=0; i<m->finalStates.size(); i++){
        if(m->colIdx[idx] == m->finalStates[i]){
            int common = 0;
            for(int j=0; j<m->FbelongsTo[i].size(); j++){
                // check whether for any of the FSAs the transiton belongs to the transition is final, too
                for(int k=0; k<m->belongsTo[idx].size(); k++){
                    if(m->FbelongsTo[i][j]==m->belongsTo[idx][k]){
                        r.push_back(m->FbelongsTo[i][j]);
                    }
                }
            }
        }
    }
    sort(r.begin(), r.end());
    r.erase(unique(r.begin(), r.end()), r.end());
    return r; 
}


/**
 * Checks whether a state is final for a FSA
 * @param state is the state to check
 * @param m is the FSA
 * @return vector of positions of state in the set of final states (possibly multiple in case of OR branches)
 */
std::vector<int> isFin_s(singleCOO *m, int state){
    std::vector<int> v;
    for(int i=0;i<m->finalStates.size();i++)
        if(m->finalStates[i]==state)
            v.push_back(i);
    return v;
}

/**
 * Checks whether a transition is enabled by a character class in a MFSA
 * @param s is the transition starting state
 * @param e is the transition arriving state
 * @param m is the MFSA
 * @return 1 if the transition is enabled by a character class, 0 otherwise
 */
int notInRange_multi(mergedCOO* a, int s, int e){
    for(int i=0;i<a->ranges.size();i++){
        if((s == a->ranges[i].start)&&(e == a->ranges[i].end))
            return 0;
    }
    return 1;
}

/**
 * Checks whether a state in the old vector of the swap table was swapped already
 * @param t is the swap table
 * @param state is the state
 * @return 1 if the transition was swapped already, 0 otherwise
 */
int checkedAlready(swapTable* t, int state){
    if(t->oldComplete.size()==0) return 0;
    for(int i=0;i<t->oldComplete.size();i++){
        if (t->oldComplete[i]==state) return 1;
    }
    return 0;
}

/**
 * Checks whether a transition is enabled by a character class in a FSA
 * @param s is the transition starting state
 * @param e is the transition arriving state
 * @param m is the FSA
 * @return 1 if the transition is enabled by a character class, 0 otherwise
 */
int notInRange(singleCOO* a, int s, int e){
    for(int i=0;i<a->ranges.size();i++){
        if((s == a->ranges[i].start)&&(e == a->ranges[i].end))
            return 0;
    }
    return 1;
}

/**
 * Adds an identifier to every belonging vector in the MFSA, in case a FSA and the MFSA to be merged are identical
 * @param id is the identifier to add
 * @param m is the MFSA
 */    
void addTwin(mergedCOO *m, int id){
    for(int i=0;i<m->belongsTo.size();i++){
        m->belongsTo[i].push_back(id);
    }
    m->initialStates.push_back(m->rowIdx[0]);
    m->IbelongsTo.push_back(id);

    for(int i=0;i<m->finalStates.size();i++){
        m->FbelongsTo[i].push_back(id);
    }
    
    for(int i=0;i<m->loopBelongsTo.size();i++){
        m->loopBelongsTo[i].push_back(id);
    }
    for(int i=0;i<m->rangeBelongsTo.size();i++){
        m->rangeBelongsTo[i].push_back(id);
    }
    m->mergedSoFar++;
}

/**
 * Finds one or more common sub-paths between a MFSA and a FSA
 * @param m is the MFSA
 * @param a is the FSA
 * @param ms is the set to be populated with the states to swap in order to merge the sub-paths
 * @param tmpFound is a counter, updated every time a common sub-path is found
 * @return an updated counter of common sub-paths found
 */   
int findCommonSub_multi(mergedCOO* m, singleCOO* a, mergingSet *ms, int tmpFound){
    int found = tmpFound;
    // check whether a == m
    if(m->nonzeroValues.size()==a->nonzeroValues.size()){
        // check whether automata are in fact equal
        int r = -1;
        for(int k=0;k<m->nonzeroValues.size();k++){
            if(m->nonzeroValues[k]!=a->nonzeroValues[k])
                r = 0;
        }
        for(int k=0;k<m->rowIdx.size()-1;k++){
            if(m->rowIdx[k]-m->rowIdx[k+1]!=a->rowIdx[k]-a->rowIdx[k+1]
                || m->colIdx[k]-m->colIdx[k+1]!=a->colIdx[k]-a->colIdx[k+1])
                r = 0;
        }
        if(m->ranges.size()!=a->ranges.size()){
            r=0;
        } else {
            for(int l=0;l<m->ranges.size();l++){
                if(!compareRanges(m->ranges[l], a->ranges[l]))
                    {r = 0;
                }
            }
        }
        if(r==-1) return r;                    
    }
            
    // initial hypothesis: m!=NULL, i.e. it contains the first automaton to be merged with already
    // very similar to the non-multi version
    for(int i=0;i<a->nonzeroValues.size();i++){
        for(int j=0;j<m->nonzeroValues.size();j++){
            if(a->nonzeroValues[i]==m->nonzeroValues[j]){
                int eqChar = 0, notNull = 0;
                int path = 1;
                // find subsequence 
                while(i+eqChar<a->nonzeroValues.size()
                && j+eqChar<m->nonzeroValues.size()
                && notInRange_multi(m, m->rowIdx[j+eqChar], m->colIdx[j+eqChar])
                && notInRange(a, a->rowIdx[i+eqChar], a->colIdx[i+eqChar])
                && (computeMult(a, a->rowIdx[i+eqChar], a->colIdx[i+eqChar]).size()==1)
                && (computeMult_m(m, m->rowIdx[j+eqChar], m->colIdx[j+eqChar]).size()==1)
            
                && (a->nonzeroValues[i+eqChar]==m->nonzeroValues[j+eqChar])
                && checkStatesCompatibility_multi(m, a, i, j, eqChar)
                // && path
                ){
                    eqChar++;
                    // if(a->nonzeroValues[i]!=EPS) notNull++;
                    // if(eqChar>0){
                    //     if((m->colIdx[j+eqChar-1]!=m->rowIdx[j+eqChar])
                    //         ||(a->colIdx[i+eqChar-1]!=a->rowIdx[i+eqChar]))
                    //             path = 0;
                    // } else {
                    //     path = 1;
                    // }
                }

                // check diag compatibility (i.e. connected path)
                // std::cout<<"found common path from automaton "<<a->id<<endl;
                // for(int k=0;k<eqChar;k++){
                //     std::cout<<a->nonzeroValues[i+k];
                // }
                // std::cout<<endl<<"path is "<<path<<", eqChar is "<<eqChar<<", notNull is "<<notNull<<endl;
                
                if(eqChar>0){

                    // std::cout<<"merging:"<<endl;
                    // for(int k=0;k<eqChar;k++){
                    //     std::cout<<a->nonzeroValues[i+k];
                    // }
                    // std::cout<<endl;

                    mergingTab mt;

                    // if first state is compatible start merging from there

                    int cfrState = -1;
                    
                    // if (ms->size()>0){
                    //     cfrState = ms->at(ms->size()-1).oldStates[ms->at(ms->size()-1).oldStates.size()-1];
                    // }
                    // std::cout<<"merging automata "<<a->id<<endl;

                    if(!existsArc(a, a->rowIdx[i], ms)){
                        int flag=1;
                        // for(int j=0;j<ms->size();j++){
                        //     for(int k=0;k<ms->at(j).oldStates.size();k++){
                        //         if(a->rowIdx[i]==ms->at(j).oldStates[k]){
                        //             flag = 0;
                        //         }
                        //         if(a->colIdx[i]==ms->at(j).oldStates[k]){
                        //             flag = 0;
                        //         }
                        //     }
                        // }   
                        mt.newStates.push_back(m->rowIdx[j]);
                        mt.oldStates.push_back(a->rowIdx[i]);
                        mt.newStates.push_back(m->colIdx[j]);
                        mt.oldStates.push_back(a->colIdx[i]);
                        // std::cout<<"merged transition with label "<<a->nonzeroValues[i]<<endl;
                        // std::cout<<"no single arc between "<<cfrState<<" and "<<a->rowIdx[i]<<endl;   
                        // std::cout<<"merging1 state "<<a->rowIdx[i]<<" to "<<a->colIdx[i]<<" labeled "<<a->nonzeroValues[i];
                    } 
                    // else {
                        // std::cout<<"single arc found between "<<cfrState<<" and "<<a->rowIdx[i]<<endl;
                    // }
                     

                    for(int k=1;k<eqChar;k++){
                        // int flag=1;
                        // for(int j=0;j<ms->size();j++){
                        //     for(int m=0;m<ms->at(j).oldStates.size();m++){
                        //         if(a->rowIdx[i+k]==ms->at(j).oldStates[m]){
                        //             flag = 0;
                        //         }
                        //         if(a->colIdx[i+k]==ms->at(j).oldStates[m]){
                        //             flag = 0;
                        //         }
                        //     }
                        // }
                        mt.newStates.push_back(m->rowIdx[j+k]);
                        mt.oldStates.push_back(a->rowIdx[i+k]);
                        mt.newStates.push_back(m->colIdx[j+k]);
                        mt.oldStates.push_back(a->colIdx[i+k]);
                        // std::cout<<"merged transition with label "<<a->nonzeroValues[i+k]<<endl;

                        // std::cout<<"merging1 state "<<a->rowIdx[i+k]<<" to "<<a->colIdx[i+k]<<" labeled "<<a->nonzeroValues[i+k];
                    }


                    // otherwise start from the second char
                    // printf("end of function\n");
                    if(mt.oldStates.size()>0)
                        ms->push_back(mt);
                    found++;

                }
            }
        }
    }
    
    
    
    return found;
}

/**
 * Checks whether an arc exists connecting a couple of states in a FSA
 * @param state1 is the starting state
 * @param state2 is the arriving state
 * @param a is the FSA
 * @return 1 if one such arc exists, 0 otherwise
 */  
int existsArc(singleCOO* a, int state2, mergingSet *ms){
    for(int j=0;j<ms->size();j++){
        for(int l=0;l<a->nonzeroValues.size();l++){
            if(a->rowIdx[l]==ms->at(j).oldStates[ms->at(j).oldStates.size()-1]&& a->colIdx[l]==state2){
                return 1;
            }
        }
    }
    // std::cout<<"compatible"<<endl;
    return 0;
}

/**
 * Checks whether an arc exists connecting a couple of states in a MFSA
 * @param state1 is the starting state
 * @param state2 is the arriving state
 * @param a is the MFSA
 * @return 1 if one such arc exists, 0 otherwise
 */ 
int existsArc_multi(mergedCOO* a, int state1, int state2){
    if(state1 == -1) return 0;
    for(int j=0;j<a->nonzeroValues.size();j++){
        if(a->rowIdx[j]==state1&& a->colIdx[j]==state2){
            return 1;
            // std::cout<<"not compatible"<<endl;
        }
    }
    // std::cout<<"compatible"<<endl;
    return 0;
}

/**
 * Checks whether a set of transitions in both a MFSA and a FSA describe an isomorphic path
 * @param idx1 is the index of the first transition to compare in the FSA
 * @param idx2 is the index of the first transition to compare in the MFSA
 * @param eq is the number of subsequent transitions to compare
 * @param a is the FSA
 * @param m is the MFSA
 * @return 1 if the paths are identical, 0 otherwise
 */ 
int checkStatesCompatibility_multi(mergedCOO* m, singleCOO* a, int idx1, int idx2, int eq){
    // condition on diagonal: sequential order of involved states matches
    for(int i=0;i<eq-1;i++){
        if(a->colIdx[idx1+i]!=a->rowIdx[idx1+i+1]) return 0;
        if(m->colIdx[idx2+i]!=m->rowIdx[idx2+i+1]) return 0;
    }
    
    return 1; 
}


/**
 * Checks whether a state was already swapped, upon building a transition table
 * @param state is the swapping table 
 * @param t is the swap table
 * @return 1 if state was swapped already, 0 otherwise
 */ 
int alreadySwapped(swapTable *t, int state){
    for(int i=0;i<t->newComplete.size();i++)
        {// std::cout<<"state "<<state<<" vs. "<<t->newComplete[i]<<endl;
            if(t->newComplete[i]==state)
                return 1;}
    return 0;
}

/**
 * Builds a swap table given a set of states involved in sub-paths to merge
 * @param t is the swap table to build
 * @param ms is a set containing the states involved in each sub-path to merge
 * @param m is the MFSA, necessary to indicate the common label for states to be merged
 * @param a is the FSA, necessary to indicate the labels to be replaced with either common or new ones
 * @param next indicates the first available label in the MFSA, to relabel non-merged states 
 * @return 1 if the paths are identical, 0 otherwise
 */ 
void buildSwappingTable_multi(swapTable* t, mergingSet *ms, mergedCOO* m, singleCOO* a, int next){
    // first put together all sub-paths in the merging set
    mergingTab *mtb = new mergingTab; 
    // remove useless elements
    int idx=0;
    // cout<<"swa swa swapping"<<endl;
    for(int i=0;i<ms->size();i++){
        for(int j=0;j<ms->at(i).newStates.size();j++){
            mtb->newStates.push_back(ms->at(i).newStates[j]);
            mtb->oldStates.push_back(ms->at(i).oldStates[j]);

            // cout<<ms->at(i).oldStates[j]<<" with "<<ms->at(i).newStates[j]<<endl;
        }
    }
    for(int i=0;i<a->rowIdx.size();i++){
        if(!checkedAlready(t, a->rowIdx[i])){
            t->oldComplete.push_back(a->rowIdx[i]);
            // cout<<"replacing bis "<<a->rowIdx[i];
            if(involvedInMerging(mtb, a->rowIdx[i])!=-1){
                if(!alreadySwapped(t, mtb->newStates[involvedInMerging(mtb, a->rowIdx[i])])){
                    t->newComplete.push_back(mtb->newStates[involvedInMerging(mtb, a->rowIdx[i])]);
                    // cout<<" 1 with "<<mtb->newStates[involvedInMerging(mtb, a->rowIdx[i])]<<endl;
                } else{
                    t->newComplete.push_back(next);
                    // cout<<" 2 with "<<next<<endl;
                    next++;
                }
            } else {
                t->newComplete.push_back(next);
                // cout<<" 3 with "<<next<<endl;

                next++;
            }
        }
    }
    for(int i=0;i<a->colIdx.size();i++){
        if(!checkedAlready(t, a->colIdx[i])){
            t->oldComplete.push_back(a->colIdx[i]);
            if(involvedInMerging(mtb, a->colIdx[i])!=-1){

                // IL PROBLEMA E QUA
                
                if(!alreadySwapped(t, mtb->newStates[involvedInMerging(mtb, a->colIdx[i])])){
                    t->newComplete.push_back(mtb->newStates[involvedInMerging(mtb, a->colIdx[i])]);
                } else {
                    t->newComplete.push_back(next);
                    next++;
                }
            } else {
               t->newComplete.push_back(next);
                next++;
            }
        }
    }

    // cout<<"\nREPLACING:\n";
    // for(int i=0; i<t->newComplete.size(); i++){
    //     cout<<"old "<<dec<<t->oldComplete[i]<<" new "<<dec<<t->newComplete[i]<<endl;
    // }
    delete mtb;    
}


/**
 * Checks whether a state is part of a sub-path to be merged
 * @param state is the state to check
 * @param m is the merging table containing all sub-paths to be merged
 * @return the index of state in the array containing old labels in case of merging, -1 otherwise 
 */ 
int involvedInMerging(mergingTab *m, int state){
    for (int i = 0; i < m->newStates.size(); i++)
    {
        if (state == m->oldStates[i]){
            // printf("state %d is involved", state);
            return i;
        }
    }
    return -1;
}

/**
 * Returns the index of an element in an array
 * @param state is the element to look for
 * @param array is the array
 * @return the position i of the element in the array, if present, -1 otherwise
 */ 
int returnIdx(std::vector<int> array, int state){
    for(int i=0;i<array.size();i++){
        if(array[i]==state) {
            // printf("returning idx %d\n", i);
            return i;
            }
    }
    return -1;
}

/**
 * swaps states label in a FSA according to a swap table
 * @param a is the FSA
 * @param t is the swap table
 */ 
void swap(swapTable* t, singleCOO* a){
    // initial and final states  
    for(int i=0;i<a->finalStates.size();i++){
        a->finalStates[i] = t->newComplete[returnIdx(t->oldComplete, a->finalStates[i])];
    }
    a->initialState = t->newComplete[returnIdx(t->oldComplete, a->initialState)];
    // normal transitions
    for(int i=0;i<a->nonzeroValues.size();i++){
        // printf("swapping %d with %d ", a->rowIdx[i], t->newComplete[returnIdx(t->oldComplete, a->size, a->rowIdx[i])]);
        a->rowIdx[i] = t->newComplete[returnIdx(t->oldComplete, a->rowIdx[i])]; 
        // printf("swapping %d with %d ", a->colIdx[i], t->newComplete[returnIdx(t->oldComplete, a->size, a->colIdx[i])]);
        a->colIdx[i] = t->newComplete[returnIdx(t->oldComplete, a->colIdx[i])];
    }
    // loops
    for(int i=0;i<a->loops.size();i++){
    // printf("end of loops\n");
        (a->loops[i]).init = t->newComplete[returnIdx(t->oldComplete, a->loops[i].init)];
        (a->loops[i]).end = t->newComplete[returnIdx(t->oldComplete, a->loops[i].end)];
        for(int j=0;j<a->loops[i].path->states.size();j++){
            a->loops[i].path->states[j] = t->newComplete[returnIdx(t->oldComplete, a->loops[i].path->states[j])];
        }
    }
    // ranges
    for(int i=0;i<a->ranges.size();i++){
        a->ranges[i].start = t->newComplete[returnIdx(t->oldComplete, a->ranges[i].start)];
        a->ranges[i].end = t->newComplete[returnIdx(t->oldComplete, a->ranges[i].end)];
    }    
}

/**
 * given a FSA and a MFSA, updates the MFSA with the incoming FSA information, managing both merged sub-paths and non-merged
 * @param to is the MFSA
 * @param from is the FSA
 */ 
void generateNew_multi(mergedCOO* to, singleCOO* from){
    if(to->mergedSoFar==0){
        // 1st merge: to not initialized yet
        // copy all the fields directly
        // std::cout<<"merging automaton "<<from->id<<endl;
        for(int i=0;i<from->nonzeroValues.size();i++){
            to->rowIdx.push_back(from->rowIdx[i]);
            to->colIdx.push_back(from->colIdx[i]);
            to->nonzeroValues.push_back(from->nonzeroValues[i]);
            std::vector<int> v; 
            v.push_back(from->id);
            to->belongsTo.push_back(v);
        }
        // std::cout<<"init state = "<<from->initialState<<endl;
        to->initialStates.push_back(from->initialState);
        for(int i=0;i<from->finalStates.size();i++){
            to->finalStates.push_back(from->finalStates[i]);
            std::vector<int> v;
            v.push_back(from->id);
            to->FbelongsTo.push_back(v);
        }
        to->IbelongsTo.push_back(from->id);


        for(int i=0;i<from->ranges.size();i++){
            range r;
            r.end=from->ranges[i].end;
            r.start=from->ranges[i].start;
            r.len=from->ranges[i].len;
            r.neg=from->ranges[i].neg;
            // cout<<"--- len is "<<r.len<<endl;
            for(int j=0;j<from->ranges[i].len;j++){
                r.ref.push_back(from->ranges[i].ref[j]);
            }
            to->ranges.push_back(r);
            std::vector<int> v; 
            v.push_back(from->id);
            to->rangeBelongsTo.push_back(v);
        }
        to->nextFree=computeMax_s(from);
        // std::cout<<"after first generate multi"<<endl;
        to->mergedSoFar=1;
        // printAutomaton_multi(to);

    } else {
        // worst case scenario: there are no common arcs 
        // to is already initialized, realloc to fit new edges
        // common states are already present in the merged automaton
        // simply add new ones
        int addedSoFar=to->nonzeroValues.size(); // old size, not updated yet
        for(int i=0;i<from->nonzeroValues.size();i++){
            // if(from->nonzeroValues[i]>256) 
            //     cout<<"huge "<<from->nonzeroValues[i]<<endl;
            int present = -1;
            // printf("to : %d %d %c\n", set[1]->colIdx[i], set[1]->rowIdx[i], set[1]->nonzeroValues[i]);
            for(int j=0;j<addedSoFar;j++){
                // printf("%d %d %c\n", mrg->colIdx[j], mrg->rowIdx[j], mrg->nonzeroValues[j]);
                // if the transition is common
                if((to->colIdx[j]==from->colIdx[i])&&(to->rowIdx[j]==from->rowIdx[i])&&(to->nonzeroValues[j]==from->nonzeroValues[i])){
                    present = j;
                }
            }
            if(present!=-1){
                // present already in the merged automaton matrix
                to->belongsTo[present].push_back(from->id);
            } else {
                // otherwise
                std::vector<int> v;
                v.push_back(from->id);
                to->belongsTo.push_back(v);
                to->rowIdx.push_back(from->rowIdx[i]);
                to->colIdx.push_back(from->colIdx[i]);
                to->nonzeroValues.push_back(from->nonzeroValues[i]);
                addedSoFar++;
            } 
        }


        
        addedSoFar=to->ranges.size();
        for(int i=0;i<from->ranges.size();i++){
            int present = -1;
            // printf("to compare: %d %d %c\n", set[1]->colIdx[i], set[1]->rowIdx[i], set[1]->nonzeroValues[i]);
            for(int j=0;j<to->ranges.size();j++){
                // printf("%d %d %c\n", mrg->colIdx[j], mrg->rowIdx[j], mrg->nonzeroValues[j]);
                // if the transition is common
                if(compareRanges(to->ranges[j], from->ranges[i]))
                    present = j;
                // std::cout<<"looking for common ranges"<<endl;
            }
            if(present!=-1){
                // std::cout<<"range present "<<endl;
                // present already in the merged automaton matrix
                to->rangeBelongsTo[present].push_back(from->id);;
            }else{
                // otherwise
                std::vector<int> v;
                v.push_back(from->id);
                to->rangeBelongsTo.push_back(v);
                to->ranges.push_back(from->ranges[i]);
                addedSoFar++;
            } 
        }
        int present = -1;
        for(int i=0;i<from->finalStates.size();i++){
            int present = -1;
            // printf("to compare: %d %d %c\n", set[1]->colIdx[i], set[1]->rowIdx[i], set[1]->nonzeroValues[i]);
            for(int j=0;j<to->finalStates.size();j++){
                // printf("%d %d %c\n", mrg->colIdx[j], mrg->rowIdx[j], mrg->nonzeroValues[j]);
                // if the transition is common
                if(from->finalStates[i]==to->finalStates[j])
                    present = j;
                // std::cout<<"looking for common ranges"<<endl;
            }
            if (present!=-1) {
                // std::cout<<"range present "<<endl;
                // present already in the merged automaton matrix
                to->FbelongsTo[present].push_back(from->id);;
            } else {
                // otherwise
                std::vector<int> v;
                v.push_back(from->id);
                to->FbelongsTo.push_back(v);
                to->finalStates.push_back(from->finalStates[i]);
                addedSoFar++;
            } 
        }
        to->initialStates.push_back(from->initialState);
        to->IbelongsTo.push_back(from->id);
        to->nextFree = to->nonzeroValues.size()+2;
        to->mergedSoFar++;
    }
};

/**
 * merges a set of n FSAs into a single MFSA 
 * @param set is the set of FSAs to merge
 * @return a MFSA resulting from the merging
 */ 
mergedCOO* merge_multi(toBeMerged *set){
    std::vector<int> res;
    mergedCOO* mrg = new mergedCOO;
    mrg->mergedSoFar=0;
    // add first automaton
    generateNew_multi(mrg, set->at(0));
    if(V)
        printAutomaton(set->at(0));

    for(int i=1;i<set->size();i++){


        if(V)
            std::cout<<"i = "<<i<<endl;
        mergingSet *ms = new mergingSet;
        // find common ranges 

        
        int foundSub = findCommonRange_multi(mrg, set->at(i), ms, 0);

        // cout<<endl<<dec<<i<<" after merging ranges: "<<endl;
        // printAutomaton_multi(mrg);

        // cout<<"found "<<foundSub<<" common ranges"<<endl;
        // find common substrings
        foundSub = findCommonSub_multi(mrg, set->at(i), ms, foundSub);

        // cout<<"found "<<foundSub<<" common transitions"<<endl;


        if(foundSub == 0){
            int offset = computeMax(mrg);
            addNew(set->at(i), mrg, offset);
        } else if(foundSub==-1){
            addTwin(mrg, set->at(i)->id);
        } else {
            swapTable* t = new swapTable;

            mrg->nextFree=computeMax(mrg);
            
            buildSwappingTable_multi(t, ms, mrg, set->at(i), mrg->nextFree);
            
            swap(t, set->at(i));

            // cout<<"\nafter swap: "<<endl;
            // printAutomaton(set->at(i));
     
            generateNew_multi(mrg, set->at(i));

            mrg->nextFree=computeMax(mrg);


            delete t;
        }      
        if(V){
            printAutomaton_multi(mrg);
            std::cout<<"added automaton "<<i<<endl;
            std::cout<<"optimizing"<<endl;
        }



        delete ms;
        
    }

    // cout<<"final"<<endl;
    // printAutomaton_multi(mrg);


    return mrg;

}

/**
 * expands a FSA loops to correctly represent quantified expression
 * @param m is the FSA
 */ 
void loopExpansion_s(singleCOO *m){
    int next = computeMax_s(m); // for extra safety
    for(int i=m->loops.size()-1; i>=0;i--){
        // std::cout<<"loop from "<<m->loops[i].init<<" to "<<m->loops[i].end<<endl;
        int offset = next;
        // first remove replicas
        for(int j=m->loops[i].path->states.size()-2; j>=0;j--){
            // std::cout<<"state "<<m->loops[i].path->states[j]<<endl;
            if(m->loops[i].path->states[j]==m->loops[i].path->states[j+1]){
                m->loops[i].path->states.erase(m->loops[i].path->states.begin()+j);
            }
        }
        for(int j=0;j<m->loops[i].path->states.size()-1;j++){
            for(int k=0;k<m->loops[i].path->states.size()-1;k++){
                if(m->loops[i].path->states[j]==m->loops[i].path->states[k] && j!=k){
                    int len = k-j;
                    int ok=1;
                    for(int l=0;l<len;l++){
                        if(m->loops[i].path->states[j+l]!=m->loops[i].path->states[k+l]){
                            ok=0;
                        }
                    }
                    if(ok){
                        for(int l=0;l<len;l++){
                            m->loops[i].path->states.erase(m->loops[i].path->states.begin()+j);
                        }
                        k-=len;
                    }
                }
            }

        }
        int init_flag = 0; 
        if(m->loops[i].init == m->initialState)
            init_flag = 1;
            // std::cout<<"b "<<m->loops[i].lowerBound<<endl;
        // std::cout<<"after removing replicas size is "<<m->loops[i].path->states.size()<<endl;
        if((m->loops[i].lowerBound == 0 && m->loops[i].upperBound == 1)
        || (m->loops[i].lowerBound == 0 && m->loops[i].upperBound == -1)){
            // std::cout<<"case 1"<<endl;
            int from = m->loops[i].init;
            int to = m->loops[i].end;
            int statSize = m->nonzeroValues.size();
            // bypass
            for (int j=0; j<statSize; j++) {
                if (m->rowIdx[j] == to) {
                    m->rowIdx.push_back(from);
                    m->colIdx.push_back(m->colIdx[j]);
                    m->nonzeroValues.push_back(m->nonzeroValues[j]);
                    // duplicate range, if necessary
                    int tmpR = correspondingRange_s(m, to, m->colIdx[j]);
                    if(tmpR!=-1){
                        range newrange = m->ranges[tmpR];
                        newrange.start = from;
                        m->ranges.push_back(newrange);
                    }
                }
            }
            if(m->loops[i].upperBound==-1){
                // replicate last transition of the loop to the initial state to close the loop
                if(m->loops[i].path->states.size()>2){
                    for(int k=0;k<m->loops[i].path->states.size()-1;k++){
                        if(m->loops[i].path->states[k]==m->loops[i].init ){
                            int tmpIdx = arcIdx_s(m, m->loops[i].init, m->loops[i].path->states[k+1]);
                            m->rowIdx.push_back(m->loops[i].end);
                            m->colIdx.push_back(m->loops[i].path->states[k+1]);
                            m->nonzeroValues.push_back(m->nonzeroValues[tmpIdx]);
                            // std::cout<<"replicating from "<<m->loops[i].path->states[k]<<" to "<<from<<" char "<<m->nonzeroValues[tmpIdx]<<endl;
                            int tmpR = correspondingRange_s(m, m->loops[i].init, m->loops[i].path->states[k+1]);
                            if(tmpR!=-1){
                                range newrange = m->ranges[tmpR];
                                newrange.start = m->loops[i].end;
                                newrange.end = m->loops[i].path->states[k+1];
                                m->ranges.push_back(newrange);
                            }
                        }
                    }
                } else {
                    // backpropagate 
                    int tmpIdx = arcIdx_s(m, m->loops[i].init, m->loops[i].end);
                    m->rowIdx.push_back(m->loops[i].end);
                    m->colIdx.push_back(m->loops[i].end);
                    m->nonzeroValues.push_back(m->nonzeroValues[tmpIdx]);
                    // std::cout<<"replicating from "<<m->loops[i].path->states[k]<<" to "<<from<<" char "<<m->nonzeroValues[tmpIdx]<<endl;
                    int tmpR = correspondingRange_s(m, m->loops[i].init, m->loops[i].end);
                    if(tmpR!=-1){
                        range newrange = m->ranges[tmpR];
                        newrange.start = m->loops[i].end;
                        newrange.end = m->loops[i].end;
                        m->ranges.push_back(newrange);
                    }
                }
            }
            // std::cout<<"size of trans is "<<m->nonzeroValues.size()<<endl;
            // printAutomaton_multi(m);
            if(isFin_s(m, to).size()>0){
                // std::cout<<from<<" is final"<<endl;
                m->finalStates.push_back(from);
            }
            m->loops.erase(m->loops.begin()+i);
        } else if (m->loops[i].lowerBound == 0 && m->loops[i].upperBound > 1) {
            // std::cout<<"case 2  "<<endl;
            int from = m->loops[i].init;
            int to = m->loops[i].end;
            int statSize = m->nonzeroValues.size();
            // bypass
            for (int j=0; j<statSize; j++) {
                if (m->rowIdx[j] == to) {
                    m->rowIdx.push_back(from);
                    m->colIdx.push_back(m->colIdx[j]);
                    m->nonzeroValues.push_back(m->nonzeroValues[j]);
                    int tmpR = correspondingRange_s(m, to, m->colIdx[j]);
                    std::cout<<"bypassing from "<<from;
                    std::cout<<" to "<<m->colIdx[j];
                    std::cout<<" with "<<m->nonzeroValues[j]<<endl;
                    if(tmpR!=-1){
                        range newrange = m->ranges[tmpR];
                        newrange.start=from;
                        m->ranges.push_back(newrange);
                    }
                }
            }
            int finalFlag = 0;
            if(isFin_s(m, to).size()>0){
                finalFlag++;
                m->finalStates.push_back(from);
            }
            // replication and bypass for each replica
            m->nextFree = computeMax_s(m); // extra safety
            int detachedState = m->loops[i].end;
            int replicaStarts = m->nextFree;   
            int offset = replicaStarts - m->loops[i].init;   
            int toSwitch = -1;
            // add replicas
            int init = replicaStarts;
            for(int j=0;j<m->loops[i].upperBound-1;j++){
                // cout<<"------------------------replicating ------------------------"<<endl;
                if(j>0){
                    // bypass for replicas
                    for (int j=0; j<statSize; j++) {
                        if (m->rowIdx[j] == to) {
                            // std::cout<<"bypassing from "<<m->colIdx[m->colIdx.size()-1];
                            // std::cout<<" to "<<m->colIdx[j];
                            // std::cout<<" with "<<m->nonzeroValues[j]<<endl;
                            int tmpR = correspondingRange_s(m, to, m->colIdx[j]);
                            if(tmpR!=-1){
                                range newrange = m->ranges[tmpR];
                                newrange.start = m->colIdx[m->colIdx.size()-1];
                                newrange.end = m->colIdx[j];
                                m->ranges.push_back(newrange);
                            }
                            // cout<<"pushed transition from "<<m->colIdx[m->colIdx.size()-1]<<" to "<<m->colIdx[j]<<endl;

                            m->rowIdx.push_back(m->colIdx[m->colIdx.size()-1]);
                            m->colIdx.push_back(m->colIdx[j]);
                            m->nonzeroValues.push_back(m->nonzeroValues[j]);
                        }
                    }
                    if(finalFlag){
                        m->finalStates.push_back(m->colIdx[m->colIdx.size()-1]);
                    }
                }
                for(int k=0;k<m->loops[i].path->states.size()-1;k++){
                    int tmp = arcIdx_s(m, m->loops[i].path->states[k], m->loops[i].path->states[k+1]);
                    // std::cout<<"arc "<<m->loops[i].path->states[k]<<" to "<<m->loops[i].path->states[k+1]<<" is "<<tmp<<endl;
                    if(tmp!=-1){
                        if(m->loops[i].path->states[k+1]==to){
                            toSwitch = m->loops[i].path->states[k+1]+offset;
                            // std::cout<<"to switch "<<toSwitch<<endl;
                        }
                        if(m->loops[i].path->states[k]==from){
                            from = m->loops[i].path->states[k]+offset;
                            // std::cout<<"to switch "<<toSwitch<<endl;
                        }
                        // std::cout<<"replica "<<m->nonzeroValues[tmp]<<endl;
                        if(k==0 || m->loops[i].path->states[k]==m->loops[i].path->states[0]){
                            m->rowIdx.push_back(init);
                        } else {
                            m->rowIdx.push_back(m->loops[i].path->states[k]+offset);
                        }
                        m->colIdx.push_back(m->loops[i].path->states[k+1]+offset);
                        // std::cout<<"pushed from "<<m->loops[i].path->states[k]+offset<<" to "<<m->loops[i].path->states[k+1]+offset<<endl;
                        // std::cout<<"pushed to "<<m->loops[i].path->states[k]+offset<<endl;
                        m->nonzeroValues.push_back(m->nonzeroValues[tmp]);
                        int tmpR = correspondingRange_s(m, m->loops[i].path->states[k], m->loops[i].path->states[k+1]);
                        // std::cout<<"corresponding range from "<<m->loops[i].path->states[k]<<" to "<<m->loops[i].path->states[k+1]<<" is "<<tmpR<<endl;
                        if(tmpR!=-1) {
                            range newrange = m->ranges[tmpR];
                            newrange.start = m->rowIdx[m->rowIdx.size()-1];
                            newrange.end = m->colIdx[m->colIdx.size()-1];
                            m->ranges.push_back(newrange);
                            // std::cout<<"pushed range "<<newrange.start<<" to "<<newrange.end<<endl;
                        }
                    }
                }
                offset += toSwitch - init +m->loops[i].path->states.size() ; 
                // std::cout<<"offset "<<offset<<endl;
                init=toSwitch;
                // add bypass 
            }
            // close repetitions
            // std::cout<<"to switch "<<toSwitch<<endl;
            for(int j=0;j<m->rowIdx.size();j++){
                if(m->rowIdx[j] == toSwitch){
                    m->rowIdx[j] = to;
                }
                if(m->colIdx[j] == toSwitch){
                    m->colIdx[j] = to;
                }
            }
            for(int j=0;j<m->ranges.size();j++){
                if(m->ranges[j].start == toSwitch){
                    m->ranges[j].start = to;
                }
                if(m->ranges[j].end == toSwitch){
                    m->ranges[j].end = to;
                }
            }
            // dsetach arcs within the loop arriving to state to
            for(int j=0;j<m->loops[i].path->states.size()-1;j++){
                // std::cout<<"state "<<m->loops[i].path->states [j]<<endl;
                int tmpIdx = arcIdx_s(m, m->loops[i].path->states[j], m->loops[i].path->states[j+1]);
                // std::cout<<"tmpIdx "<<tmpIdx<<endl;
                if(tmpIdx!=-1 && m->colIdx[tmpIdx] == to){
                    int toSwitch = m->colIdx[tmpIdx];
                    m->colIdx[tmpIdx] = replicaStarts; 
                    // std::cout<<"switched "<<toSwitch<<" to "<<replicaStarts<<endl;
                }

                int tmpR = correspondingRange_s(m, m->loops[i].path->states[j], m->loops[i].path->states[j+1]);
                if(tmpR!=-1 && m->ranges[tmpR].end == to){
                    int toSwitch = m->colIdx[tmpIdx];
                    m->ranges[tmpR].end = replicaStarts; 
                }
                           
            }
            for(int k=0;k<m->ranges.size();k++){
                if(m->ranges[k].start == toSwitch){
                    m->ranges[k].start = replicaStarts;
                }
                if(m->ranges[k].end == toSwitch){
                    m->ranges[k].end = replicaStarts;
                }
            }
            for (int j=0; j<statSize; j++) {
                // this is the first bypass (after the first rep)
                if (m->rowIdx[j] == to) {
                    // std::cout<<"bypassing from "<<replicaStarts;
                    // std::cout<<" to "<<m->colIdx[j];
                    // std::cout<<" with "<<m->nonzeroValues[j]<<endl;
                    m->rowIdx.push_back(replicaStarts);
                    m->colIdx.push_back(m->colIdx[j]);
                    m->nonzeroValues.push_back(m->nonzeroValues[j]);
                    int tmpR = correspondingRange_s(m, to, m->colIdx[j]);
                    if(tmpR!=-1){
                        range newrange = m->ranges[tmpR];
                        newrange.start = replicaStarts;
                        newrange.end = m->colIdx[j];
                        m->ranges.push_back(newrange);
                    }
                }
            }
            if(finalFlag){
                m->finalStates.push_back(replicaStarts);
            }    
            m->loops.erase(m->loops.begin()+i);
        } else if ((m->loops[i].lowerBound == 1 && m->loops[i].upperBound == 1)
        || (m->loops[i].lowerBound == 1 && m->loops[i].upperBound == -1)) {
            // no replicas (greedy algo), this case is useless
            // std::cout<<"case 3"<<endl;
            if(m->loops[i].upperBound!=-1){
                m->loops.erase(m->loops.begin()+i);
            } else{
                if(m->loops[i].path->states.size()>2){
                    // replicate last transition of the loop to the initial state to close the loop
                    for(int k=0;k<m->loops[i].path->states.size()-1;k++){
                        if(m->loops[i].path->states[k]==m->loops[i].init){
                            // printAutomaton_multi(m);
                            int tmpIdx = arcIdx_s(m, m->loops[i].init, m->loops[i].path->states[k+1]);
                            m->rowIdx.push_back(m->loops[i].end);
                            m->colIdx.push_back(m->loops[i].path->states[k+1]);
                            m->nonzeroValues.push_back(m->nonzeroValues[tmpIdx]);
                            int tmpR = correspondingRange_s(m, m->loops[i].init, m->loops[i].path->states[k+1]);
                            if(tmpR!=-1){
                                range newrange = m->ranges[tmpR];
                                newrange.start = m->loops[i].end;
                                newrange.end = m->loops[i].path->states[k+1];
                                m->ranges.push_back(newrange);
                            }
                        }
                    }
                } else {
                    // backpropagate 
                    int tmpIdx = arcIdx_s(m, m->loops[i].init, m->loops[i].end);
                    m->rowIdx.push_back(m->loops[i].end);
                    m->colIdx.push_back(m->loops[i].end);
                    m->nonzeroValues.push_back(m->nonzeroValues[tmpIdx]);
                    // std::cout<<"replicating from "<<m->loops[i].path->states[k]<<" to "<<from<<" char "<<m->nonzeroValues[tmpIdx]<<endl;
                    int tmpR = correspondingRange_s(m, m->loops[i].init, m->loops[i].end);
                    if(tmpR!=-1){
                        range newrange = m->ranges[tmpR];
                        newrange.start = m->loops[i].end;
                        newrange.end = m->loops[i].end;
                        m->ranges.push_back(newrange);
                    }
                }
                m->loops.erase(m->loops.begin()+i);
            }
        } else if ((m->loops[i].lowerBound == m->loops[i].upperBound && m->loops[i].upperBound > 1)) {
            // same as previous case, no bypasses
            // std::cout<<"case 4  "<<endl;
            int from = m->loops[i].init;
            int to = m->loops[i].end;
            int statSize = m->nonzeroValues.size();
            // bypass
            int finalFlag = 0;
            if(isFin_s(m, to).size()>0){
                finalFlag++;
                // m->finalStates.push_back(from);
            }
            // replication and bypass for each replica
            m->nextFree = computeMax_s(m); // extra safety
            int detachedState = m->loops[i].end;
            int replicaStarts = m->nextFree;   
            int offset = replicaStarts - m->loops[i].init;   
            int toSwitch = -1;
            // add replicas
            int init = replicaStarts;
            for(int j=0;j<m->loops[i].upperBound-1;j++){
                for(int k=0;k<m->loops[i].path->states.size()-1;k++){
                    int tmp = arcIdx_s(m, m->loops[i].path->states[k], m->loops[i].path->states[k+1]);
                    // std::cout<<"arc "<<m->loops[i].path->states[k]<<" to "<<m->loops[i].path->states[k+1]<<" is "<<tmp<<endl;
                    if(tmp!=-1){
                        if(m->loops[i].path->states[k+1]==to){
                            toSwitch = m->loops[i].path->states[k+1]+offset;
                            // std::cout<<"to switch "<<toSwitch<<endl;
                        }
                        if(m->loops[i].path->states[k]==from){
                            from = m->loops[i].path->states[k]+offset;
                            // std::cout<<"to switch "<<toSwitch<<endl;
                        }
                        // std::cout<<"replica "<<m->nonzeroValues[tmp]<<endl;
                        if(k==0 || m->loops[i].path->states[k]==m->loops[i].path->states[0]){
                            m->rowIdx.push_back(init);
                        } else {
                            m->rowIdx.push_back(m->loops[i].path->states[k]+offset);
                        }
                        m->colIdx.push_back(m->loops[i].path->states[k+1]+offset);
                        // std::cout<<"pushed from "<<m->loops[i].path->states[k]+offset<<" to "<<m->loops[i].path->states[k+1]+offset<<endl;
                        // std::cout<<"pushed to "<<m->loops[i].path->states[k]+offset<<endl;
                        m->nonzeroValues.push_back(m->nonzeroValues[tmp]);
                        int tmpR = correspondingRange_s(m, m->loops[i].path->states[k], m->loops[i].path->states[k+1]);
                        // std::cout<<"corresponding range from "<<m->loops[i].path->states[k]<<" to "<<m->loops[i].path->states[k+1]<<" is "<<tmpR<<endl;
                        if(tmpR!=-1) {
                            range newrange;
                            newrange.start = m->rowIdx[m->rowIdx.size()-1];
                            newrange.end = m->colIdx[m->colIdx.size()-1];
                            newrange.len =  m->ranges[tmpR].len;
                            newrange.ref = m->ranges[tmpR].ref;
                            newrange.neg = m->ranges[tmpR].neg;
                            m->ranges.push_back(newrange);
                            // std::cout<<"pushed range "<<newrange.start<<" to "<<newrange.end<<endl;
                        }
                    }
                }
                offset += toSwitch - init + m->loops[i].path->states.size() ; 
                // std::cout<<"offset "<<offset<<endl;
                init=toSwitch;
                // add bypass 
            }
            // close repetitions
            // std::cout<<"to switch "<<toSwitch<<endl;
            for(int j=0;j<m->rowIdx.size();j++){
                if(m->rowIdx[j] == toSwitch){
                    m->rowIdx[j] = to;
                }
                if(m->colIdx[j] == toSwitch){
                    m->colIdx[j] = to;
                }
            }
            // TESTARE MEGLIO APPARTENENZA RANGE
            for(int j=0;j<m->ranges.size();j++){
                if(m->ranges[j].start == toSwitch){
                    m->ranges[j].start = to;
                }
                if(m->ranges[j].end == toSwitch){
                    m->ranges[j].end = to;
                }
            }
            // dsetach arcs within the loop arriving to state to
            for(int j=0;j<m->loops[i].path->states.size()-1;j++){
                // std::cout<<"state "<<m->loops[i].path->states [j]<<endl;
                int tmpIdx = arcIdx_s(m, m->loops[i].path->states[j], m->loops[i].path->states[j+1]);
                // std::cout<<"tmpIdx "<<tmpIdx<<endl;
                if(tmpIdx!=-1 && m->colIdx[tmpIdx] == to){
                    int toSwitch = m->colIdx[tmpIdx];
                    m->colIdx[tmpIdx] = replicaStarts; 
                    // std::cout<<"switched "<<toSwitch<<" to "<<replicaStarts<<endl;
                }
                int tmpR = correspondingRange_s(m, m->loops[i].path->states[j], m->loops[i].path->states[j+1]);
                if(tmpR!=-1 && m->ranges[tmpR].end == to){
                    int toSwitch = m->colIdx[tmpIdx];
                    m->ranges[tmpR].end = replicaStarts; 
                }                          
            }
            for(int k=0;k<m->ranges.size();k++){
                if(m->ranges[k].start == toSwitch){
                    m->ranges[k].start = replicaStarts;
                }
                if(m->ranges[k].end == toSwitch){
                    m->ranges[k].end = replicaStarts;
                }
            }   
            m->loops.erase(m->loops.begin()+i);
        } else if((m->loops[i].lowerBound == 1 && m->loops[i].upperBound > 1)){
            // repeat once, then bypass
            // std::cout<<"case 4  "<<endl;
            int from = m->loops[i].init;
            int to = m->loops[i].end;
            int statSize = m->nonzeroValues.size();
            // bypass
            int finalFlag = 0;
            m->nextFree = computeMax_s(m); // extra safety
            int detachedState = m->loops[i].end;
            int replicaStarts = m->nextFree;   
            int offset = replicaStarts - m->loops[i].init;   
            int toSwitch = -1;
            if(isFin_s(m, to).size()>0){
                finalFlag++;
                // m->finalStates.push_back(replicaStarts);
            }
            // replication and bypass for each replica
            // add replicas
            int init = replicaStarts;
            for(int j=0;j<m->loops[i].upperBound-1;j++){
                if(j>0){
                    // bypass for replicas
                    for (int j=0; j<statSize; j++) {
                        if (m->rowIdx[j] == to) {
                            // std::cout<<"bypassing from "<<m->colIdx[m->colIdx.size()-1];
                            // std::cout<<" to "<<m->colIdx[j];
                            // std::cout<<" with "<<m->nonzeroValues[j]<<endl;
                            int tmpR = correspondingRange_s(m, to, m->colIdx[j]);
                            if(tmpR!=-1){
                                range newrange = m->ranges[tmpR];
                                newrange.start = m->colIdx[m->colIdx.size()-1];
                                newrange.end = m->colIdx[j];
                                m->ranges.push_back(newrange);
                            }
                            m->rowIdx.push_back(m->colIdx[m->colIdx.size()-1]);
                            m->colIdx.push_back(m->colIdx[j]);
                            m->nonzeroValues.push_back(m->nonzeroValues[j]);
                        }
                    }
                    if(finalFlag){
                        m->finalStates.push_back(m->colIdx[m->colIdx.size()-1]);
                    }
                }
                for(int k=0;k<m->loops[i].path->states.size()-1;k++){
                    int tmp = arcIdx_s(m, m->loops[i].path->states[k], m->loops[i].path->states[k+1]);
                    // std::cout<<"arc "<<m->loops[i].path->states[k]<<" to "<<m->loops[i].path->states[k+1]<<" is "<<tmp<<endl;
                    if(tmp!=-1){
                        if(m->loops[i].path->states[k+1]==to){
                            toSwitch = m->loops[i].path->states[k+1]+offset;
                            // std::cout<<"to switch "<<toSwitch<<endl;
                        }
                        if(m->loops[i].path->states[k]==from){
                            from = m->loops[i].path->states[k]+offset;
                            // std::cout<<"to switch "<<toSwitch<<endl;
                        }
                        // std::cout<<"replica "<<m->nonzeroValues[tmp]<<endl;
                        if(k==0 || m->loops[i].path->states[k]==m->loops[i].path->states[0]){
                            m->rowIdx.push_back(init);
                        } else {
                            m->rowIdx.push_back(m->loops[i].path->states[k]+offset);
                        }
                        m->colIdx.push_back(m->loops[i].path->states[k+1]+offset);
                        // std::cout<<"pushed from "<<m->loops[i].path->states[k]+offset<<" to "<<m->loops[i].path->states[k+1]+offset<<endl;
                        // std::cout<<"pushed to "<<m->loops[i].path->states[k]+offset<<endl;
                        m->nonzeroValues.push_back(m->nonzeroValues[tmp]);
                        int tmpR = correspondingRange_s(m, m->loops[i].path->states[k], m->loops[i].path->states[k+1]);
                        // std::cout<<"corresponding range from "<<m->loops[i].path->states[k]<<" to "<<m->loops[i].path->states[k+1]<<" is "<<tmpR<<endl;
                        if(tmpR!=-1) {
                            range newrange = m->ranges[tmpR];
                            newrange.start = m->rowIdx[m->rowIdx.size()-1];
                            newrange.end = m->colIdx[m->colIdx.size()-1];
                            m->ranges.push_back(newrange);
                            // std::cout<<"pushed range "<<newrange.start<<" to "<<newrange.end<<endl;
                        }
                    }
                }
                offset += toSwitch - init +m->loops[i].path->states.size() ; 
                // std::cout<<"offset "<<offset<<endl;
                init=toSwitch;
                // add bypass 
            }
            // close repetitions
            // std::cout<<"to switch "<<toSwitch<<endl;
            for(int j=0;j<m->rowIdx.size();j++){
                if(m->rowIdx[j] == toSwitch){
                    m->rowIdx[j] = to;
                }
                if(m->colIdx[j] == toSwitch){
                    m->colIdx[j] = to;
                }

            }
            for(int j=0;j<m->ranges.size();j++){
                if(m->ranges[j].start == toSwitch){
                    m->ranges[j].start = to;
                }
                if(m->ranges[j].end == toSwitch){
                    m->ranges[j].end = to;
                }
            }
            // dsetach arcs within the loop arriving to state to
            for(int j=0;j<m->loops[i].path->states.size()-1;j++){
                // std::cout<<"state "<<m->loops[i].path->states [j]<<endl;
                int tmpIdx = arcIdx_s(m, m->loops[i].path->states[j], m->loops[i].path->states[j+1]);
                // std::cout<<"tmpIdx "<<tmpIdx<<endl;
                if(tmpIdx!=-1 && m->colIdx[tmpIdx] == to){
                    int toSwitch = m->colIdx[tmpIdx];
                    m->colIdx[tmpIdx] = replicaStarts; 
                    // std::cout<<"switched "<<toSwitch<<" to "<<replicaStarts<<endl;
                }

                int tmpR = correspondingRange_s(m, m->loops[i].path->states[j], m->loops[i].path->states[j+1]);
                if(tmpR!=-1 && m->ranges[tmpR].end == to){
                    int toSwitch = m->colIdx[tmpIdx];
                    m->ranges[tmpR].end = replicaStarts; 
                }
                           
            }
            for(int k=0;k<m->ranges.size();k++){
                if(m->ranges[k].start == toSwitch){
                    m->ranges[k].start = replicaStarts;
                }
                if(m->ranges[k].end == toSwitch){
                    m->ranges[k].end = replicaStarts;
                }
            }
            for (int j=0; j<statSize; j++) {
                // this is the first bypass (after the first rep)
                if (m->rowIdx[j] == to) {
                    // std::cout<<"bypassing from "<<replicaStarts;
                    // std::cout<<" to "<<m->colIdx[j];
                    // std::cout<<" with "<<m->nonzeroValues[j]<<endl;
                    m->rowIdx.push_back(replicaStarts);
                    m->colIdx.push_back(m->colIdx[j]);
                    m->nonzeroValues.push_back(m->nonzeroValues[j]);
                    int tmpR = correspondingRange_s(m, to, m->colIdx[j]);
                    if(tmpR!=-1){
                        range newrange = m->ranges[tmpR];
                        newrange.start = replicaStarts;
                        newrange.end = m->colIdx[j];
                        m->ranges.push_back(newrange);
                    }
                }
            }
            if(finalFlag){
                m->finalStates.push_back(replicaStarts);
            }    
            // if(m->loops[i].upperBound!=-1){
            m->loops.erase(m->loops.begin()+i);
            // }
        } else if (m->loops[i].lowerBound > 1 && m->loops[i].upperBound > 1) {
        //    std::cout<<"case 4  "<<endl;
            int from = m->loops[i].init;
            int to = m->loops[i].end;
            int statSize = m->nonzeroValues.size();
            // bypass
            int finalFlag = 0;
            if(isFin_s(m, to).size()>0){
                finalFlag++;
            }
            // replication and bypass for each replica
            m->nextFree = computeMax_s(m); // extra safety
            int detachedState = m->loops[i].end;
            int replicaStarts = m->nextFree;   
            int offset = replicaStarts - m->loops[i].init;   
            int toSwitch = -1;
            // add replicas
            int init = replicaStarts;
            for(int k=0;k<m->finalStates.size();k++){
                if(m->finalStates[k] == replicaStarts){
                    // std::cout<<"pulling from fstates "<<replicaStarts<<endl;
                    m->finalStates.erase(m->finalStates.begin()+k);
                    k--;
                }
            }
            for(int j=1;j<m->loops[i].upperBound;j++){
                if(j>m->loops[i].lowerBound-1){
                    // bypass for replicas
                    for (int k=0; k<statSize; k++) {
                        if (m->rowIdx[k] == to) {
                            // std::cout<<"bypassing from "<<m->colIdx[m->colIdx.size()-1];
                            // std::cout<<" to "<<m->colIdx[j];
                            // std::cout<<" with "<<m->nonzeroValues[j]<<endl;
                            int tmpR = correspondingRange_s(m, to, m->colIdx[k]);
                            if(tmpR!=-1){
                                range newrange = m->ranges[tmpR];
                                newrange.start = m->colIdx[m->colIdx.size()-1];
                                newrange.end = m->colIdx[k];
                                m->ranges.push_back(newrange);
                            }
                            m->rowIdx.push_back(m->colIdx[m->colIdx.size()-1]);
                            m->colIdx.push_back(m->colIdx[k]);
                            m->nonzeroValues.push_back(m->nonzeroValues[k]);
                        }
                    }
                    if(finalFlag){
                        m->finalStates.push_back(m->colIdx[m->colIdx.size()-1]);
                    }
                }
                for(int k=0;k<m->loops[i].path->states.size()-1;k++){
                    int tmp = arcIdx_s(m, m->loops[i].path->states[k], m->loops[i].path->states[k+1]);
                    // std::cout<<"arc "<<m->loops[i].path->states[k]<<" to "<<m->loops[i].path->states[k+1]<<" is "<<tmp<<endl;
                    if(tmp!=-1){
                        if(m->loops[i].path->states[k+1]==to){
                            toSwitch = m->loops[i].path->states[k+1]+offset;
                            // std::cout<<"to switch "<<toSwitch<<endl;
                        }
                        if(m->loops[i].path->states[k]==from){
                            from = m->loops[i].path->states[k]+offset;
                            // std::cout<<"to switch "<<toSwitch<<endl;
                        }
                        // std::cout<<"replica "<<m->nonzeroValues[tmp]<<endl;
                        if(k==0 || m->loops[i].path->states[k]==m->loops[i].path->states[0]){
                            m->rowIdx.push_back(init);
                        } else {
                            m->rowIdx.push_back(m->loops[i].path->states[k]+offset);
                        }
                        m->colIdx.push_back(m->loops[i].path->states[k+1]+offset);
                        // std::cout<<"pushed from "<<m->loops[i].path->states[k]+offset<<" to "<<m->loops[i].path->states[k+1]+offset<<endl;
                        // std::cout<<"pushed to "<<m->loops[i].path->states[k]+offset<<endl;
                        m->nonzeroValues.push_back(m->nonzeroValues[tmp]);
                        int tmpR = correspondingRange_s(m, m->loops[i].path->states[k], m->loops[i].path->states[k+1]);
                        // std::cout<<"corresponding range from "<<m->loops[i].path->states[k]<<" to "<<m->loops[i].path->states[k+1]<<" is "<<tmpR<<endl;
                        if(tmpR!=-1) {
                            range newrange = m->ranges[tmpR];
                            newrange.start = m->rowIdx[m->rowIdx.size()-1];
                            newrange.end = m->colIdx[m->colIdx.size()-1];
                            m->ranges.push_back(newrange);
                            // std::cout<<"pushed range "<<newrange.start<<" to "<<newrange.end<<endl;
                        }
                    }
                }
                offset += toSwitch - init + m->loops[i].path->states.size() ; 
                // std::cout<<"offset "<<offset<<endl;
                init=toSwitch;
                // add bypass 
            }
            // close repetitions
            // std::cout<<"to switch "<<toSwitch<<endl;
            for(int j=0;j<m->rowIdx.size();j++){
                if(m->rowIdx[j] == toSwitch){
                    m->rowIdx[j] = to;
                }
                if(m->colIdx[j] == toSwitch){
                    m->colIdx[j] = to;
                }

            }
            for(int j=0;j<m->ranges.size();j++){
                if(m->ranges[j].start == toSwitch){
                    m->ranges[j].start = to;
                }
                if(m->ranges[j].end == toSwitch){
                    m->ranges[j].end = to;
                }
            }
            // dsetach arcs within the loop arriving to state to
            for(int j=0;j<m->loops[i].path->states.size()-1;j++){
                // std::cout<<"state "<<m->loops[i].path->states [j]<<endl;
                int tmpIdx = arcIdx_s(m, m->loops[i].path->states[j], m->loops[i].path->states[j+1]);
                // std::cout<<"tmpIdx "<<tmpIdx<<endl;
                if(tmpIdx!=-1 && m->colIdx[tmpIdx] == to){
                    int toSwitch = m->colIdx[tmpIdx];
                    m->colIdx[tmpIdx] = replicaStarts; 
                    // std::cout<<"switched "<<toSwitch<<" to "<<replicaStarts<<endl;
                }

                int tmpR = correspondingRange_s(m, m->loops[i].path->states[j], m->loops[i].path->states[j+1]);
                if(tmpR!=-1 && m->ranges[tmpR].end == to){
                    int toSwitch = m->colIdx[tmpIdx];
                    m->ranges[tmpR].end = replicaStarts; 
                }
                           
            }
            for(int k=0;k<m->ranges.size();k++){
                if(m->ranges[k].start == toSwitch){
                    m->ranges[k].start = replicaStarts;
                }
                if(m->ranges[k].end == toSwitch){
                    m->ranges[k].end = replicaStarts;
                }
            }
            
            // if(finalFlag){
            //     m->finalStates.push_back(replicaStarts);
            // }    
            m->loops.erase(m->loops.begin()+i);
        } else if(m->loops[i].lowerBound>1 && m->loops[i].upperBound==-1){
            // same as previous case, no bypasses
            // std::cout<<"case 6  "<<endl;
            int from = m->loops[i].init;
            int to = m->loops[i].end;
            int statSize = m->nonzeroValues.size();
            int finalFlag = 0;
            if(isFin_s(m, to).size()>0){
                finalFlag++;
                // m->finalStates.push_back(from);
            }
            // replication and bypass for each replica
            m->nextFree = computeMax_s(m); // extra safety
            int detachedState = m->loops[i].end;
            int replicaStarts = m->nextFree;   
            int offset = replicaStarts - m->loops[i].init;   
            int toSwitch = -1;
            int lastRepStartingPt = -1;
            // add replicas
            int init = replicaStarts;
            for(int j=0;j<m->loops[i].lowerBound-1;j++){
                for(int k=0;k<m->loops[i].path->states.size()-1;k++){
                    int tmp = arcIdx_s(m, m->loops[i].path->states[k], m->loops[i].path->states[k+1]);
                    // std::cout<<"arc "<<m->loops[i].path->states[k]<<" to "<<m->loops[i].path->states[k+1]<<" is "<<tmp<<endl;
                    if(tmp!=-1){
                        if(m->loops[i].path->states[k+1]==to){
                            toSwitch = m->loops[i].path->states[k+1]+offset;
                            // std::cout<<"to switch "<<toSwitch<<endl;
                        }
                        if(m->loops[i].path->states[k]==from){
                            from = m->loops[i].path->states[k]+offset;
                            // std::cout<<"to switch "<<toSwitch<<endl;
                        }
                        // std::cout<<"replica "<<m->nonzeroValues[tmp]<<endl;
                        if(k==0 || m->loops[i].path->states[k]==m->loops[i].path->states[0]){
                            m->rowIdx.push_back(init);
                        } else {
                            m->rowIdx.push_back(m->loops[i].path->states[k]+offset);
                        }
                        m->colIdx.push_back(m->loops[i].path->states[k+1]+offset);
                        if(k==1)
                            lastRepStartingPt = m->rowIdx[m->rowIdx.size()-1];

                        // std::cout<<"pushed from "<<m->loops[i].path->states[k]+offset<<" to "<<m->loops[i].path->states[k+1]+offset<<endl;
                        // std::cout<<"pushed to "<<m->loops[i].path->states[k]+offset<<endl;
                        m->nonzeroValues.push_back(m->nonzeroValues[tmp]);
                        int tmpR = correspondingRange_s(m, m->loops[i].path->states[k], m->loops[i].path->states[k+1]);
                        // std::cout<<"corresponding range from "<<m->loops[i].path->states[k]<<" to "<<m->loops[i].path->states[k+1]<<" is "<<tmpR<<endl;
                        if(tmpR!=-1) {
                            range newrange = m->ranges[tmpR];
                            newrange.start = m->rowIdx[m->rowIdx.size()-1];
                            newrange.end = m->colIdx[m->colIdx.size()-1];
                            m->ranges.push_back(newrange);
                            // std::cout<<"pushed range "<<newrange.start<<" to "<<newrange.end<<endl;
                        }
                    }
                }
                init=toSwitch;
                offset += toSwitch - init + m->loops[i].path->states.size() ; 
                // std::cout<<"offset "<<offset<<endl;
                // add bypass 
            }
            // close repetitions
            if(m->loops[i].path->states.size()>2){
                for(int k=0;k<m->loops[i].path->states.size()-1;k++){
                    if(m->loops[i].path->states[k]==m->loops[i].init ){
                        int tmpIdx = arcIdx_s(m, m->loops[i].init, m->loops[i].path->states[k+1]);
                        m->rowIdx.push_back(m->loops[i].end);
                        m->colIdx.push_back(lastRepStartingPt);
                        m->nonzeroValues.push_back(m->nonzeroValues[tmpIdx]);
                        // std::cout<<"replicating from "<<m->loops[i].path->states[k]<<" to "<<from<<" char "<<m->nonzeroValues[tmpIdx]<<endl;
                        int tmpR = correspondingRange_s(m, m->loops[i].init, m->loops[i].path->states[k+1]);
                        if(tmpR!=-1){
                            range newrange = m->ranges[tmpR];
                            newrange.start = m->loops[i].end;
                            newrange.end = m->loops[i].path->states[k+1];
                            m->ranges.push_back(newrange);
                        }
                    }
                }
            } else {
                // backpropagate 
                int tmpIdx = arcIdx_s(m, m->loops[i].init, m->loops[i].end);
                m->rowIdx.push_back(m->loops[i].end);
                m->colIdx.push_back(m->loops[i].end);
                m->nonzeroValues.push_back(m->nonzeroValues[tmpIdx]);
                // std::cout<<"replicating from "<<m->loops[i].path->states[k]<<" to "<<from<<" char "<<m->nonzeroValues[tmpIdx]<<endl;
                int tmpR = correspondingRange_s(m, m->loops[i].init, m->loops[i].end);
                if(tmpR!=-1){
                    range newrange = m->ranges[tmpR];
                    newrange.start = m->loops[i].end;
                    newrange.end = m->loops[i].end;
                    m->ranges.push_back(newrange);
                }
            }
            // std::cout<<"to switch "<<toSwitch<<endl;
            for(int j=0;j<m->rowIdx.size();j++){
                if(m->rowIdx[j] == toSwitch){
                    m->rowIdx[j] = to;
                }
                if(m->colIdx[j] == toSwitch){
                    m->colIdx[j] = to;
                }

            }
            // TESTARE MEGLIO APPARTENENZA RANGE
            for(int j=0;j<m->ranges.size();j++){
                if(m->ranges[j].start == toSwitch){
                    m->ranges[j].start = to;
                }
                if(m->ranges[j].end == toSwitch){
                    m->ranges[j].end = to;
                }
            }
            // dsetach arcs within the loop arriving to state to
            for(int j=0;j<m->loops[i].path->states.size()-1;j++){
                // std::cout<<"state "<<m->loops[i].path->states [j]<<endl;
                int tmpIdx = arcIdx_s(m, m->loops[i].path->states[j], m->loops[i].path->states[j+1]);
                // std::cout<<"tmpIdx "<<tmpIdx<<endl;
                if(tmpIdx!=-1 && m->colIdx[tmpIdx] == to){
                    int toSwitch = m->colIdx[tmpIdx];
                    m->colIdx[tmpIdx] = replicaStarts; 
                    // std::cout<<"switched "<<toSwitch<<" to "<<replicaStarts<<endl;
                }

                int tmpR = correspondingRange_s(m, m->loops[i].path->states[j], m->loops[i].path->states[j+1]);
                if(tmpR!=-1 && m->ranges[tmpR].end == to){
                    int toSwitch = m->colIdx[tmpIdx];
                    m->ranges[tmpR].end = replicaStarts; 
                }
                           
            }
            for(int k=0;k<m->ranges.size();k++){
                if(m->ranges[k].start == toSwitch){
                    m->ranges[k].start = replicaStarts;
                }
                if(m->ranges[k].end == toSwitch){
                    m->ranges[k].end = replicaStarts;
                }
            }   
            // enclose loop
            
            m->loops.erase(m->loops.begin()+i);
        }
    }

    for(int i=0;i<m->rowIdx.size();i++){
        int p=0;
        for(int j=0;j<m->colIdx.size();j++){
            if(i!=j && m->rowIdx[i]==m->colIdx[j])
                p++;
        }
        if(!p)
            m->initialState = m->rowIdx[i];
        
    }

    

}


/**
 * optimizes single characters alternation into a single character class enabling a transition
 * @param m is the FSA
 */ 
void or2range_s(singleCOO *m){
    
    // printAutomaton(m);
    for(int i=0;i<m->nonzeroValues.size();i++){
        std::vector<int> mult = computeMult(m, m->rowIdx[i], m->colIdx[i]);
        // std::cout<<"mult is "<<mult.size()<<" from "<<m->rowIdx[i]<<" to "<<m->colIdx[i]<<endl;
        if(mult.size()>1){
            std::vector<uint16_t> ref; 
            std::vector<int> tmp;
            int from = m->rowIdx[i];
            int to = m->colIdx[i];
            range r;
            r.len=0;
            for(int j=mult.size()-1;j>=0;j--){
                // std::cout<<"char is "<<m->nonzeroValues[mult[j]]<<endl;
                ref.push_back(m->nonzeroValues[mult[j]]);
                // cout<<"PUSHING CHAR "<<m->nonzeroValues[mult[j]]<<endl;
                // delete from COO vectors
                m->rowIdx.erase(m->rowIdx.begin()+mult[j]);
                m->colIdx.erase(m->colIdx.begin()+mult[j]);
                // std::cout<<"erasing arc with label "<<m->nonzeroValues[mult[j]]<<endl;
                m->nonzeroValues.erase(m->nonzeroValues.begin()+mult[j]);
                r.len++;
            }
            i--;
            // add eps arc
            m->rowIdx.push_back(from);
            m->colIdx.push_back(to);
            m->nonzeroValues.push_back(EPS);
            r.start = from;
            r.end = to;
            r.neg = 0;
            r.ref = ref;
            // push character class 
            m->ranges.push_back(r);
        }
    }


    // delete redundant transitions, possible merging with other range structs 
    

    
    vector<int> to_del;

    uint16_t mask_sol = 1 << 9;
    // in case there is multiplicity between an actual arc and an existing range 

    for(int i=0;i<m->nonzeroValues.size();i++){
        if(m->nonzeroValues[i]!=EPS && m->nonzeroValues[i]!=(EPS | mask_sol)){
            for(int j=0; j<m->ranges.size(); j++){
                if (m->rowIdx[i]==m->ranges[j].start && 
                    m->colIdx[i]==m->ranges[j].end){
                        // push char to range
                        m->ranges[j].ref.push_back(m->nonzeroValues[i]);
                        m->ranges[j].len++;
                        // delete arc
                        to_del.push_back(i);
                        // cout<<"deleting arc from "<<m->rowIdx[i]<<" to "<<m->colIdx[i]<<endl;
                }
            }
        }
    }

    sort(to_del.begin(), to_del.end());
    to_del.erase(unique(to_del.begin(), to_del.end()), to_del.end());


    for(int i=to_del.size()-1; i>=0;i--){
        m->rowIdx.erase(m->rowIdx.begin()+to_del[i]);
        m->colIdx.erase(m->colIdx.begin()+to_del[i]);
        m->nonzeroValues.erase(m->nonzeroValues.begin()+to_del[i]);
    }

    vector<int> to_del2;
    vector<int> to_del_from;
    vector<int> to_del_to;


    // multiplicity among ranges
    // cout<<"+++++ (1) print tmp before before merging "<<endl;
    // printAutomaton(m);
    // cout<<"+++++ end tmp"<<endl;

    for(int i=m->ranges.size()-1; i>=0; i--){
        for(int j=i-1; j>=0; j--){
            if(i!=j && m->ranges[i].start == m->ranges[j].start && 
                m->ranges[i].end == m->ranges[j].end &&
                m->ranges[i].neg == m->ranges[j].neg){
                for(int k=0;k<m->ranges[i].len;k++){
                    m->ranges[j].ref.push_back(m->ranges[i].ref[k]);
                }
                m->ranges[j].len += m->ranges[i].len;
                to_del2.push_back(i);
                to_del_from.push_back(m->ranges[i].start);
                to_del_to.push_back(m->ranges[i].end);
            }
        }
    }



    // cout<<"to delete: ";
    // for(int i=0;i<to_del2.size();i++){
    //     cout<<to_del2[i]<<" ";
    // }
    // cout<<endl;

    sort(to_del2.begin(), to_del2.end());
    to_del2.erase(unique(to_del2.begin(), to_del2.end()), to_del2.end());

    for(int i=to_del2.size()-1; i>=0;i--){
        m->ranges.erase(m->ranges.begin()+to_del2[i]);
    }

    sort(to_del_from.begin(), to_del_from.end());
    to_del_from.erase(unique(to_del_from.begin(), to_del_from.end()), to_del_from.end());

    sort(to_del_to.begin(), to_del_to.end());
    to_del_to.erase(unique(to_del_to.begin(), to_del_to.end()), to_del_to.end());
    // remove corresponding arc too

    // cout<<"arcs to delete: ";
    // for(int i=0;i<to_del_from.size();i++){
    //     cout<<to_del_from[i]<<" to "<<to_del_to[i]<<endl;
    // }

    for(int j=to_del_from.size()-1; j>=0; j--){
        int cnt = 0;
        for(int i=m->nonzeroValues.size()-1; i>=0; i--){
            if(!cnt && m->rowIdx[i]==to_del_from[j] && m->colIdx[i]==to_del_to[j]){
                m->rowIdx.erase(m->rowIdx.begin()+i);
                m->colIdx.erase(m->colIdx.begin()+i);
                m->nonzeroValues.erase(m->nonzeroValues.begin()+i);
                cnt++;
            }
        }
    }
    

    // cout<<"after sorting to delete: ";
    // for(int i=0;i<to_del2.size();i++){
    //     cout<<to_del2[i]<<" ";
    // }
    // cout<<endl;

    // cout<<"+++++ (2) print tmp before before merging "<<endl;
    // printAutomaton(m);
    // cout<<"+++++ end tmp"<<endl;
   
}


/**
 * prints a MFSA in a MFSA-compliant ANML representation for the execution
 * @param m is the MFSA
 * @param output is the output file
 * @param offset is an offset to shift the states label by a constant factor 
 */
void printANML_infant(mergedCOO *m, char* output, int offset){
    if(V)
        printAutomaton_multi(m);
    FILE * fPtr = fopen(output, "w");
    uint16_t mask_sol = 1 << 9;
    uint16_t mask_rg = 1 << 8; 
    fprintf(fPtr, "<anml version=\"1.0\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n<automata-network id=\"merged\">\n");
    // states: for each state check initial, final, involvement in loop/range 
    for(int i=0;i<m->nonzeroValues.size();i++){
        if(m->nonzeroValues[i]!=EPS && m->nonzeroValues[i]!=(EPS | mask_sol)){
            fprintf(fPtr, "<state-transition-element id=\"%d\" symbol-set=\"[\\x", m->rowIdx[i]+offset);
            // std::cout<<"printing "<<m->nonzeroValues[i]<<endl;
            uint16_t pt = m->nonzeroValues[i];

            // if(pt > 256)
            //     cout<<"SONO STRONZO "<<pt;
            // printf("hex is %x int is %d char is %x\n", m->nonzeroValues[i], m->nonzeroValues[i], m->nonzeroValues[i]);
            if(pt<HEX)
                fprintf(fPtr, "0%x", pt);
            else
                fprintf(fPtr, "%x", pt);
            fprintf(fPtr, "]\"");
            //%x]\"", (), m->nonzeroValues[i]);
            // possibly initial 
            vector<int> in = isInit_trans(m, i);
            if(in.size()>0){
                fprintf(fPtr, " start=\"");
                for(int k=0;k<in.size();k++){
                    fprintf(fPtr, "%d", in[k]);
                    if(k!=in.size()-1)
                        fprintf(fPtr, ",");
                }
                fprintf(fPtr, "\">");
            }
            else
                fprintf(fPtr, " start=\"-1\">");
            // int flag = isLoopInitNull(m, m->colIdx[i]);
            int state=-1;
            // if(flag!=-1)
            //     state = m->loops[flag].end;
            // in case it is not final ignore
            // if(isFin_trans(m, i).size()!=0 && state!=-1)
            //     fprintf(fPtr, "\n\t<report-on-match reportcode=\"%d\"/>", m->loopBelongsTo[flag][0]);
            fprintf(fPtr, "\n\t<activate-on-match element=\"%d\"/>", m->colIdx[i]+offset);
            fprintf(fPtr, " <available-re=\"");
            for(int k=0;k<m->belongsTo[i].size();k++){
                fprintf(fPtr, "%d", m->belongsTo[i][k]);
                if(k!=m->belongsTo[i].size()-1)
                    fprintf(fPtr, ",");
            }
            fprintf(fPtr, "\"/>");
            vector<int> r = isFin_trans(m, i);
            if(r.size()>0){
                fprintf(fPtr, "\n\t<report-on-match reportcode=\"");
                for(int k=0;k<r.size();k++){
                    fprintf(fPtr, "%d,",r[k]);
                }            
                fprintf(fPtr, "\"/>");
            }
            fprintf(fPtr, "\n</state-transition-element>\n");
        } else if(correspondingRange(m, m->rowIdx[i], m->colIdx[i])!=-1){
            uint16_t curr_mask = 0;
            cout<<"\nprinting range from "<<dec<<m->rowIdx[i]<<" to "<<m->colIdx[i]<<endl;

            if(m->nonzeroValues[i] != EPS)
                curr_mask = mask_sol;

            fprintf(fPtr, "<state-transition-range-element id=\"%d\" symbol-set=\"[", (m->rowIdx[i]+offset));
            int idx = correspondingRange(m, m->rowIdx[i], m->colIdx[i]);
            if(m->ranges[idx].neg)
                fprintf(fPtr, "^");
            // might have two masks 
            for(int k=0;k<m->ranges[idx].len;k++){
                cout<<hex<<m->ranges[idx].ref[k]<<" ";
                uint16_t c = m->ranges[idx].ref[k]|curr_mask;
                // if((int) c!=MINUS){
                if ((int)c<HEX)
                    fprintf(fPtr, "\\x0%x", c);
                else
                    fprintf(fPtr, "\\x%x",  c);
                // std::bitset<16> y(c);
                // std::cout << y << '\n';
                // } else {
                //     fprintf(fPtr, "-");
                // }
                
            }
            
            fprintf(fPtr, "]\"");
            // possibly initial 
            vector<int> in = isInit_trans(m, i);
            if(in.size()>0){
                fprintf(fPtr, " start=\"");
                for(int k=0;k<in.size();k++){
                    fprintf(fPtr, "%d", in[k]);
                    if(k!=in.size()-1)
                        fprintf(fPtr, ",");
                }
                fprintf(fPtr, "\">");
            }
            else
                fprintf(fPtr, " start=\"-1\">");
            // in case it is not final ignore
            fprintf(fPtr, "\n\t<activate-on-match element=\"%d\"/>", m->colIdx[i]+offset);    
            fprintf(fPtr, " <available-re=\"");
            for(int k=0;k<m->belongsTo[i].size();k++){
                fprintf(fPtr, "%d", m->belongsTo[i][k]);
                if(k!=m->belongsTo[i].size()-1)
                    fprintf(fPtr, ",");
            }
            fprintf(fPtr, "\"/>");
            vector<int> r = isFin_trans(m, i);
            if(r.size()>0){
                fprintf(fPtr, "\n\t<report-on-match reportcode=\"");
                for(int k=0;k<r.size();k++){
                    fprintf(fPtr, "%d,", r[k]);
                }            
                fprintf(fPtr, "\"/>");
            }


            fprintf(fPtr, "\n</state-transition-range-element>\n");
        }
    }
    
    fprintf(fPtr, "</automata-network>");
    fprintf(fPtr, "\n</anml>");
    fclose(fPtr);
}

/**
 * checks whether a transition in a MFSA is enabled by a character class
 * @param m is the MFSA
 * @param start is the starting state
 * @param end is the arriving state
 * @return the index of the corresponding character class in the vector of character classes, if it exists, -1 otherwise
 */
int correspondingRange(mergedCOO *m, int start, int end){
    for(int i=0;i<m->ranges.size();i++){
        if(m->ranges[i].start==start && m->ranges[i].end==end)
            return i;
    }
    return -1;
}

/**
 * checks whether a transition in a FSA is enabled by a character class
 * @param m is the FSA
 * @param start is the starting state
 * @param end is the arriving state
 * @return the index of the corresponding character class in the vector of character classes, if it exists, -1 otherwise
 */
int correspondingRange_s(singleCOO *m, int start, int end){
    for(int i=0;i<m->ranges.size();i++){
        if(m->ranges[i].start==start && m->ranges[i].end==end)
            return i;
    }
    return -1;
}

/**
 * returns the label with highest value plus one in a MFSA
 * @param m is the MFSA
 * @return a non-used label in MFSAs states
 */
int computeMax(mergedCOO *m){
    int tmpMax = -1; 
    for(int i=0;i<m->rowIdx.size();i++){
        if(m->rowIdx[i]>tmpMax)
            tmpMax=m->rowIdx[i];
    }
    for(int i=0;i<m->colIdx.size();i++){
        if(m->colIdx[i]>tmpMax)
            tmpMax=m->colIdx[i];
    }
    return tmpMax+1;
}

/**
 * returns the label with highest value plus one in a FSA
 * @param m is the FSA
 * @return a non-used label in MFSAs states
 */
int computeMax_s(singleCOO *m){
    int tmpMax = -1; 
    for(int i=0;i<m->rowIdx.size();i++){
        if(m->rowIdx[i]>tmpMax)
            tmpMax=m->rowIdx[i];
    }
    for(int i=0;i<m->colIdx.size();i++){
        if(m->colIdx[i]>tmpMax)
            tmpMax=m->colIdx[i];
    }
    return tmpMax+1;
}

/**
 * Adds a disjoint FSA to a MFSA (no common sub-paths), renaming all its states with an offset to avoid overlapping
 * @param a is the FSA
 * @param m is the MFSA
 * @param offset is the offset ensuring no overlapping
 */  
void addNew(singleCOO* a, mergedCOO* m, int offset){
    // transitions
    // std::cout<<"NEW NEW"<<endl;
    // printAutomaton(a);
    for(int i=0;i<a->nonzeroValues.size();i++){
        std::vector<int> v;
        m->colIdx.push_back(a->colIdx[i]+offset);
        m->rowIdx.push_back(a->rowIdx[i]+offset);
        m->nonzeroValues.push_back(a->nonzeroValues[i]);
        v.push_back(a->id);
        m->belongsTo.push_back(v);
    }
    // initial and final stateaddNews
    m->initialStates.push_back(a->initialState+offset);
    m->IbelongsTo.push_back(a->id);
    for(int i=0;i<a->finalStates.size();i++){
        m->finalStates.push_back(a->finalStates[i]+offset);
        std::vector<int> v;
        v.push_back(a->id);
        m->FbelongsTo.push_back(v);
    }
    // loops
    
    for(int i=0;i<a->ranges.size();i++){
        std::vector<int> v;
        a->ranges[i].start+=offset;
        a->ranges[i].end+=offset;
        m->ranges.push_back(a->ranges[i]);
        v.push_back(a->id);
        m->rangeBelongsTo.push_back(v);
    }
    // worst case
    m->nextFree=computeMax(m);
    m->mergedSoFar++;
}


/**
 * Prints MFSA to dot file
 * @param a is the MFSA
 * @param output is the file
 */
void printDot(mergedCOO* a, std::string output){

    // printAutomaton_multi(a);

    std::ofstream fPtr;
    fPtr.open(output);

    fPtr<<"digraph finite_state_machine {\nfontname=\"Palatino Linotype\"\n";
    fPtr<<"node [fontname=\"Palatino Linotype\"]\nedge [fontname=\"Palatino Linotype\"]\n";

    fPtr<<"fontname=\"Palatino\""<<endl;
    fPtr<<"fake [style=invisible]"<<endl;
    fPtr<<"node [colorscheme=pastel15]"<<endl;
    fPtr<<"edge [colorscheme=pastel15]"<<endl;

    // initial states
    for(int i=0;i<a->initialStates.size();i++){
        fPtr<<a->initialStates[i]<<endl;
    }
    // nodes
    fPtr<<a->rowIdx[0]<<endl;
    for(int i=0;i<a->colIdx.size();i++){
        fPtr<<a->colIdx[i]<<endl;
    }

    // final states 
    for(int i=0;i<a->finalStates.size();i++){
        fPtr<<a->finalStates[i]<<endl;
    }

    for(int i=0;i<a->initialStates.size();i++){
        fPtr<<"Start"<<a->IbelongsTo[i]+1<<" [margin=0 width=0 shape=plaintext color="<<a->IbelongsTo[i]+1<<"]"<<endl;
    }

    for(int i=0;i<a->initialStates.size();i++){
        fPtr<<"End"<<i+1<<" [margin=0 width=0 shape=plaintext color="<<i+1<<"]"<<endl;
    }
    
    // initial and final transitions
    for(int i=0;i<a->initialStates.size();i++){
        fPtr<<"Start"<<a->IbelongsTo[i]+1<<" -> "<< a->initialStates[i]<<" [color = "<<a->IbelongsTo[i]+1<<"]"<<endl;
    }

    for(int i=0;i<a->finalStates.size();i++){
        for(int j=0;j<a->FbelongsTo[i].size();j++){
            fPtr<<a->finalStates[i]<<" -> "<<"End"<<a->FbelongsTo[i][j]+1<<" [color = "<<a->FbelongsTo[i][j]+1<<"]"<<endl;
        }
        fPtr<<endl;
    }

    // transitions    
    std::cout<<"size once in dot is "<<a->rowIdx.size()<<endl;

    for(int i=0;i<a->rowIdx.size();i++){
        // fPtr<<"cazzo "<<a->nonzeroValues[i]<<endl;
        if(a->nonzeroValues[i]!=EPS){
            // std::cout<<"1. transition from "<<a->rowIdx[i]<<" to "<<a->colIdx[i]<<" with value "<<a->nonzeroValues[i]<<endl;
            for(int k=0;k<a->belongsTo[i].size();k++)
                fPtr<<a->rowIdx[i]<<" -> "<<a->colIdx[i]<<" [label =\""<<a->nonzeroValues[i]<<"\", color = "<<a->belongsTo[i][k]+1<<"]"<<endl;
        } else {

            int flag = 1; 
            for(int j=0;j<a->ranges.size();j++){
                if(a->rowIdx[i]==a->ranges[j].start && a->colIdx[i]==a->ranges[j].end)
                    flag = 0;
            }
            if(flag){
                for(int k=0;k<a->belongsTo[i].size();k++)
                    fPtr<<a->rowIdx[i]<<" -> "<<a->colIdx[i]<<" [color = "<<a->belongsTo[i][k]+1<<"]"<<endl;
            }      
        }
    }
    // loop
    // ranges
    for(int i=0;i<a->ranges.size();i++){
        for(int k=0;k<a->rangeBelongsTo[i].size();k++){
            if(a->ranges[i].neg){
                fPtr<<a->ranges[i].start<<" -> "<<a->ranges[i].end<<" [label =\"!";
            } else {
                fPtr<<a->ranges[i].start<<" -> "<<a->ranges[i].end<<" [label =\"";
            }
            for(int j=0;j<a->ranges[i].len;j++){
                fPtr<<a->ranges[i].ref[j];
            }
            fPtr<<"\", color = "<<a->rangeBelongsTo[i][k]+1<<"]"<<endl;
        }
    }
    fPtr<<"}";
    fPtr.close();
}


/**
 * Compares two character classes are identival (check both states and chatacters involved)
 * @param r1 is the first character class
 * @param r2 is the second character class
 * @return 1 if the classes are identical, 0 otherwise
 */
int compareRanges(range r1, range r2){
    int ret = 1;
    // std::cout<<"start "<<r1.start<<", "<<r2.start<<endl;
    // std::cout<<"end "<<r1.end<<", "<<r2.end<<endl;
    // std::cout<<"len "<<r1.len<<", "<<r2.len<<endl;
    // std::cout<<"neg "<<r1.neg<<", "<<r2.neg<<endl;
    if(r1.start == r2.start
        && r1.end == r2.end 
        && r1.neg == r2.neg
        && r1.len == r2.len){
            for(int i=0;i<r1.len;i++){
                // std::cout<<"chars "<<r1.ref[i]<<", "<<r2.ref[i]<<endl;
                if(r1.ref[i]!=r2.ref[i])
                    ret = 0;
            }
        } else {
            ret =0;
        }
    return ret;
}

/**
 * compute average of a vector of integers
*/
float avg(vector<int> v){
    float sum = 0;
    for(int i=0;i<v.size();i++){
        sum+=v[i];
    }
    return sum/v.size();
}

/**
 * merges a set of ASTs 
 * @param mrg contains the ASTs to merge
 * @param batch is the merging factor
 * @param out is the dir to save output MFSAs
 * @param time_dir is the dir to save compilation time, if necessary
 * @param dur_fe is the duration of the frontend phase in milliseconds
 * @return 0 if the execution was concluded successfully
 */
int merge_automata(std::vector<AstNodePtr> *mrg, int batch, char *out, char *time_dir, int dur_fe){

    int states = STATES;

    int stack_time = STACK_TIME;

    if(time_dir==NULL && stack_time){
        cout<<"insert a directory to save timing execution files or disable STACK_TIME"<<endl;
        return 1;
    }

    int *idx = new int[1];
    int id = 0;
    int startingPt = 0;
    idx[0]=0;
    toBeMerged *tbm = new toBeMerged;
    float size =0;
    ofstream f;
    std::string s = out;
    f.open(s+".txt");
    int reps = 1;//REPS*batch;
    std::vector<mergedCOO *> *merged = new std::vector<mergedCOO *>;
    // std::cout<<"reps for batch "<<batch <<" is "<<reps<<endl;
    // std::cout<<"in total will generate "<<mrg->size()/batch*reps<<" automata"<<endl;
    int d1=0; 
    int d2=0; 
    int d3=0;


    for(int i=0;i<mrg->size();i++){
        // cout<<"mrg size is "<<mrg->size()<<endl;
        // printAST(mrg->at(i), 0);        
        // printAutomaton(addAutomaton(mrg->at(i)));
        auto start_m1 = std::chrono::high_resolution_clock::now();
        singleCOO *tmp = addAutomaton(mrg->at(i), startingPt);
        auto stop_m1 = std::chrono::high_resolution_clock::now();
        auto duration_m1 = std::chrono::duration_cast<std::chrono::microseconds>(stop_m1 - start_m1);
        d1+=duration_m1.count();
        tbm->push_back(tmp);
        // for(int l = 0; l<tbm->at(tbm->size()-1)->nonzeroValues.size(); l++){
        //     if(tbm->at(tbm->size()-1)->nonzeroValues[l]>256){
        //         cout <<tbm->size()-1<<" HUGE HUGE HUGE "<<tbm->at(tbm->size()-1)->nonzeroValues[l]<<endl;
        //     }
        // }

        tbm->at(tbm->size()-1)->id=i%batch;
        // single FSA optimization 
        // cout<<"before single optimization phase"<<endl;


        

        auto start_m2 = std::chrono::high_resolution_clock::now();



        removeEps(tbm->at(tbm->size()-1));
        
        or2range_s(tbm->at(tbm->size()-1));
        
        loopExpansion_s(tbm->at(tbm->size()-1));



        fixRange(tbm->at(tbm->size()-1));

        removeDup(tbm->at(tbm->size()-1));

        auto stop_m2 = std::chrono::high_resolution_clock::now();
        auto duration_m2 = std::chrono::duration_cast<std::chrono::microseconds>(stop_m2 - start_m2);



        d2+=duration_m2.count();

        if(states)
            size+=countStates_s(tbm->at(tbm->size()-1));//tbm->at(tbm->size()-1)->nonzeroValues.size();
        else 
            size+=countTrans_s(tbm->at(tbm->size()-1));
        if(tbm->size()==batch || i==mrg->size()-1){
            // average middle-end for single opt 
            // std::cout<<"initial size is "<<siz<e<<" idx "<<idx[0]<<endl;
            // generating the matrix
            auto start_m3 = std::chrono::high_resolution_clock::now();

            mergedCOO *tmpM = merge_multi(tbm);
            auto stop_m3 = std::chrono::high_resolution_clock::now();
            auto duration_m3 = std::chrono::duration_cast<std::chrono::microseconds>(stop_m3 - start_m3);
            merged->push_back(tmpM);
            // printAutomaton_multi(tmpM);
            int r=0;
            if(states){
                r = countStates_m(merged->at(merged->size()-1)); //mrg->nonzeroValues.size();
            }
            else{
                r = countTrans_m(merged->at(merged->size()-1));
            }
            // std::cout<<"initial size is "<<size<<endl;
            // std::cout<<"final size is "<<r<<endl;
            float perc = (size-r)/size;
            startingPt = 0;//computeMax(merged->at(merged->size()-1))+1;
            // std::cout<<"starting point: "<<startingPt<<endl;
            if (stack_time)  
                d3+=duration_m3.count();
            f<<r<<","<<size<<endl;
            size=0;
            tbm->clear();
        
        }
        id++;
    }
    f.close();
    // generate automata files
    if(stack_time){
        ofstream fll;
        fll.open(time_dir, std::ios_base::app);
        // FILE *fll = fopen(time_dir, std::ios_base::app);
        fll<<dur_fe<<","<<d1<<","<<d2<<","<<d3<<",";
        f.close();
    }

    int offset = 0;

    int k=0;

    int d4=0;

    for(int k=0;k<reps;k++){
        for(int i=0;i<merged->size();i++){
            // std::cout<<"idx "<<idx[0]<<endl;
            
            std::string output;

            if(ANML)
                output = out + to_string(k*merged->size()+i) + ".anml";
            else
                output = out + to_string(k*merged->size()+i) + ".dot";
                    
            char* outFin = new char[output.size() + 1];

            strcpy(outFin, output.c_str());

            std::string output2; 

            if(V)
                std::cout<<"generating output"<<endl;




            if(ANML){
                auto start_m4 = std::chrono::high_resolution_clock::now();
                printANML_infant(merged->at(i), outFin, offset);
                auto stop_m4 = std::chrono::high_resolution_clock::now();
                auto duration_m4 = std::chrono::duration_cast<std::chrono::microseconds>(stop_m4 - start_m4);
                d4+=duration_m4.count();
            }
            else
                printDot(merged->at(i), outFin);
        }
        offset+=computeMax(merged->at(merged->size()-1));
    }

    if(stack_time){
        // cout<<"last stack"<<endl;
        ofstream fll2;
        fll2.open(time_dir, std::ios_base::app);
        fll2 <<d4<<endl;
        fll2.close();
    }

    return 0;
}

/**
 * Counts the number of states in a FSA
 * @param a is the FSA
 * @return the number of states in the FSA
 */
int countStates_s(singleCOO *a){
    std::vector<int> checked; 
    for(int i=0;i<a->rowIdx.size();i++){
        int present = 0;
        for(int j=0;j<checked.size();j++){
            if (checked[j]==a->rowIdx[i])
                present = 1; 
        }
        if(!present)    
            checked.push_back(a->rowIdx[i]);
    }
    for(int i=0;i<a->colIdx.size();i++){
        int present = 0;
        for(int j=0;j<checked.size();j++){
            if (checked[j]==a->colIdx[i])
                present = 1; 
        }
        if(!present)    
           checked.push_back(a->colIdx[i]);
    }
    return checked.size();
}

/**
 * Counts the number of transitions in a FSA
 * @param a is the FSA
 * @return the number of transitions in the FSA
 */
int countTrans_s(singleCOO *a){
    return a->nonzeroValues.size();
}

/**
 * Counts the number of states in a MFSA
 * @param a is the MFSA
 * @return the number of states in the MFSA
 */
int countStates_m(mergedCOO *a){
    std::vector<int> checked; 
    for(int i=0;i<a->rowIdx.size();i++){
        int present = 0;
        for(int j=0;j<checked.size();j++){
            if (checked[j]==a->rowIdx[i])
                present = 1; 
        }
        if(!present)    
            checked.push_back(a->rowIdx[i]);
    }
    for(int i=0;i<a->colIdx.size();i++){
        int present = 0;
        for(int j=0;j<checked.size();j++){
            if (checked[j]==a->colIdx[i])
                present = 1; 
        }
        if(!present)    
           checked.push_back(a->colIdx[i]);
    }
    return checked.size();
}

/**
 * Counts the number of transitions in a MFSA
 * @param a is the MFSA
 * @return the number of transitions in the MFSA
 */
int countTrans_m(mergedCOO *a){
    return a->nonzeroValues.size();
}

/**
 * remove eps-arcs by propagation in FSAs
 * @param m is the FSA
 */
void removeEps(singleCOO *m){
    // printAutomaton(m);
    int statsize = m->finalStates.size();
    std::vector<int> toRemove;
    for(int i=0;i<m->nonzeroValues.size();i++){
        std::vector<int> s = nextEps(m, m->colIdx[i]);
        // for every emtpy arc rooted in the current transition's arrival state
        for(int j=0;j<s.size();j++){
            int flag = 0; 
            int notNec = 1;
            // propagate
            for(int k=0;k<statsize;k++){
                if(m->colIdx[s[j]]==m->finalStates[k]){
                    m->finalStates.push_back(m->rowIdx[s[j]]);
                    flag++;
                    toRemove.push_back(k);
                }
            }
            for(int k=0;k<m->loops.size();k++){
                if( m->rowIdx[s[j]]==m->loops[k].end){
                    notNec=0;
                }
            }
            if(!flag || notNec){
                int newS = m->colIdx[s[j]];
                int oldS = m->colIdx[i];
                simpleSwap(m, oldS, newS);
            } 
                
        }
        // remove empty arc
        for(int j=0;j<s.size();j++){
            m->nonzeroValues.erase(m->nonzeroValues.begin()+s[j]);
            m->rowIdx.erase(m->rowIdx.begin()+s[j]);
            m->colIdx.erase(m->colIdx.begin()+s[j]);
            i--;
        }
    }
    for(int i=0;i<toRemove.size();i++){
        m->finalStates.erase(m->finalStates.begin()+toRemove[i]);
    }
}

/**
 * re-labels a state, swapping an old label with a new one, in a FSA
 * @param m is the FSA
 * @param o is the old label
 * @param n is the new label
 */
void simpleSwap(singleCOO *m, int o, int n){
    for(int i=0;i<m->rowIdx.size();i++){
        if(m->rowIdx[i]==o)
            m->rowIdx[i]=n;
        if(m->colIdx[i]==o)
            m->colIdx[i]=n;
    }
    for(int i=0;i<m->loops.size();i++){
        if(m->loops[i].init==o)
            m->loops[i].init=n;
        if(m->loops[i].end==o)
            m->loops[i].end=n;
        for(int j=0;j<m->loops[i].path->states.size();j++){
            if(m->loops[i].path->states[j]==o)
                m->loops[i].path->states[j]=n;
        }
    }
    for(int i=0;i<m->ranges.size();i++){
        if(m->ranges[i].start==o)
            m->ranges[i].start=n;
        if(m->ranges[i].end==o)
            m->ranges[i].end=n;
    }
    for(int i=0;i<m->finalStates.size();i++){
        if(m->finalStates[i]==o)
            m->finalStates[i]=n;
    }
    
    if(m->initialState==o)
        m->initialState=n;
    // printAutomaton_multi(m);
}

/**
 * finds eps-arcs rooted in a state
 * @param m is the FSA
 * @param state is the state to investigate
 * @return a vector of indices containing eps-transitions rooted in state
 */
std::vector<int> nextEps(singleCOO *m, int state){
    // returns the indices of the e-moves "next transitions"
    std::vector<int> r;
    // std::cout<<"curr arcs are "<<m->nonzeroValues.size()<<endl;
    uint16_t mask_sol = 1 << 9;
    for (int i=0;i<m->nonzeroValues.size();i++){
        // if the state yields to an eps transition not involved in a range
        if(m->rowIdx[i]==state && (m->nonzeroValues[i]==EPS  || m->nonzeroValues[i]==(EPS | mask_sol) )&& correspondingRange_s(m, m->rowIdx[i], m->colIdx[i])==-1)
            r.push_back(i); 
    }
    return r;
}

/**
 * computes the multiplicity of a transition in a FSA
 * @param m is the FSA
 * @param from is the transition initial state
 * @param to is the transition final state
 * @return a vector of indices containing transitions connecting that couple of states
 */
std::vector<int> computeMult(singleCOO *m, int from, int to){
    // returns the multiplicity of the transition from->to
    std::vector<int> mult;
    uint16_t mask_sol = 1<<9;
    // std::cout<<"from "<<from<<" to "<<to<<endl;
    for(int i=0;i<m->nonzeroValues.size();i++){
        // std::cout<<"from "<<m->rowIdx[i]<<" to "<<m->colIdx[i]<<" via "<<m->nonzeroValues[i]<<endl;
        if(m->rowIdx[i]==from && m->colIdx[i]==to && m->nonzeroValues[i]!=EPS && m->nonzeroValues[i]!=(EPS|mask_sol)){
            // std::cout<<"found "<<m->nonzeroValues[i]<<endl;
            mult.push_back(i);
        }
    }
    return mult;
}

/**
 * computes the multiplicity of a transition in a MFSA
 * @param m is the MFSA
 * @param from is the transition initial state
 * @param to is the transition final state
 * @return a vector of indices containing transitions connecting that couple of states
 */
std::vector<int> computeMult_m(mergedCOO *m, int from, int to){
    // returns the multiplicity of the transition from->to
    std::vector<int> mult;
    // std::cout<<"from "<<from<<" to "<<to<<endl;
    for(int i=0;i<m->nonzeroValues.size();i++){
        // std::cout<<"from "<<m->rowIdx[i]<<" to "<<m->colIdx[i]<<" via "<<m->nonzeroValues[i]<<endl;
        if(m->rowIdx[i]==from && m->colIdx[i]==to){
            mult.push_back(i);
        }
    }
    return mult;
}

/**
 * remove possible duplicate in FSA final states
 * @param m is the FSA
 */
void removeDup(singleCOO *m){
    for(int i=0;i<m->finalStates.size();i++){
        for(int j=0;j<m->finalStates.size();j++){
            if(j!=i && m->finalStates[i]==m->finalStates[j]){
                m->finalStates.erase(m->finalStates.begin()+j);
                j--;
            }
        }
    }
}

/**
 * return the index of a FSA transition in COO format
 * @param m is the FSA
 * @param from is the starting state
 * @param to is the arriving state
 * @return the index of the transition in the COO-compliant adj. matrix, -1 otherwise
 */
int arcIdx_s(singleCOO *m, int from, int to){
    for(int i=0;i<m->nonzeroValues.size();i++){
        if(m->rowIdx[i]==from && m->colIdx[i]==to)
            return i;
    }
    return -1;
}


/**
 * profiles a set of ASTs characteristics (avg. #states, #trans, etc)
 * @param mrg is the vector of ASTs
 */
void profiler(std::vector<AstNodePtr> *mrg){
    vector<singleCOO *> *tbm = new vector<singleCOO *>;
    float avg_nts = 0;
    int nts=0;
    float avg_lcc = 0;
    int lcc=0;
    for(int i=0;i<mrg->size();i++){
        tbm->push_back(addAutomaton(mrg->at(i), 0));
        avg_nts+=tbm->at(tbm->size()-1)->nonzeroValues.size();
        nts++;
        for(int j=0;j<tbm->at(tbm->size()-1)->ranges.size();j++){
            avg_lcc+=tbm->at(tbm->size()-1)->ranges[j].len;
            lcc++;
        }
    }
    cout<<"average number of transitions is "<<avg_nts/nts<<endl;
    cout<<"average length of character classes is "<<avg_lcc/lcc<<endl;
}


/**
 * computes the first available label in a FSA
 * @param s is the FSA
 * @return the first available label
 */
int max_s(singleCOO *s){
    int curr_max = 0; 
    for(int i=0; i<s->rowIdx.size(); i++){
        if(s->rowIdx[i]>curr_max)
            curr_max = s->rowIdx[i];
        if(s->colIdx[i]>curr_max)
            curr_max = s->colIdx[i];
        
    }
    return curr_max;
}

/**
 * renames a state in a FSA
 * @param s is the FSA
 * @param new_name is the new label
 * @param old_name is the old label
 */
void rename(singleCOO *s, int new_name, int old_name){
    for(int i=0; i<s->nonzeroValues.size(); i++){
        if(s->rowIdx[i]==old_name)
            s->rowIdx[i]=new_name;
        if(s->colIdx[i]==old_name)
            s->colIdx[i]=new_name;
    }

    for(int j=0; j<s->ranges.size(); j++){
        if(s->ranges[j].start==old_name)
            s->ranges[j].start=new_name;
        if(s->ranges[j].end==old_name)
            s->ranges[j].end=new_name;
    }

    for(int j=0; j<s->finalStates.size(); j++){
        if(s->finalStates[j]==old_name)
            s->finalStates[j]=new_name;
    }

    if(s->initialState==old_name)
        s->initialState=new_name;
}
