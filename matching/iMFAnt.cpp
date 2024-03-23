#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <tuple>
#include <chrono>
#include <unistd.h>
#include <cstdlib>
#include "iMFAnt.hpp"
#include <bitset>

#define MB 1024000
#define V 0

using namespace std;
using namespace std::chrono;

/**
 * checks whether a transition is initial
 * @param m is the MFSA
 * @param idx is the index of the transition to check
 * @return vector of idxs encoding the FSAs for which the transition is initial
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
 * checks whether a transition is final
 * @param m is the MFSA
 * @param idx is the index of the transition to check
 * @return vector of idxs encoding the FSAs for which the transition is final
 */
vector<int> isFin_trans(mergedCOO *m, int idx){
    vector<int> r; 
    for(int i=0; i<m->finalStates.size(); i++){
        if(m->colIdx[idx] == m->finalStates[i]){
            // cout<<"analysis of final state "<<m->colIdx[idx]<<endl;

            int common = 0;
            for(int j=0; j<m->FbelongsTo[i].size(); j++){
                for(int k=0; k<m->belongsTo[idx].size(); k++){
                    if(m->FbelongsTo[i][j]==m->belongsTo[idx][k]){
                        // cout<<"found common "<<m->FbelongsTo[i][j]<<endl;
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
 * converts vector of uint16_t into another vector, taking into account elements' possible semantic value
 * @param ref is the input vector
 * @return vector of elements according to input semantic value
 */
vector<uint16_t> ref_to_list(vector<uint16_t> ref){
    
    vector<uint16_t> r; 
    uint16_t mask = 1 <<8; 

    for(int i=0;i<ref.size();i++){
        if((ref[i] &mask) == mask ){
            // all the letters 
            for(int j=ref[i-1]+1;j<ref[i+1];j++){
                r.push_back((uint16_t)j);
            }
        }
        else   
            r.push_back(ref[i]);
    }

    return r;
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
 * returns highest-valued used label in a MFSA
 * @param m is the MFSA
 * @return the highest-valued used label
 */
int max(mergedCOO *m){
    int max = 0;
    for (int i=0;i<m->nonzeroValues.size();i++){
        if(m->rowIdx[i]>max)
            max = m->rowIdx[i];
        if(m->colIdx[i]>max)
            max = m->colIdx[i];
    }
    return max;
}

/**
 * Prints a MFSA
 * @param matrix is the MFSA
 */
void printAutomaton_multi(mergedCOO* matrix)
{
    std::cout<<"initial states"<<endl;
    for(int i=0;i<matrix->initialStates.size();i++){
        std::cout<<matrix->initialStates[i]<<" belongs to "<<matrix->IbelongsTo[i]<<endl;
    }
    std::cout<<"\ntransitions:"<<endl;
    for(int i=0;i<matrix->nonzeroValues.size();i++){
        std::cout<<endl<<matrix->rowIdx[i]<<"--"<<(char)matrix->nonzeroValues[i]<<"->"<<matrix->colIdx[i]<<endl;
        std::cout<<"belongs to: "<<endl; 
        for(int j=0;j<matrix->belongsTo[i].size();j++)
            std::cout<<matrix->belongsTo[i][j]<<" ";
    }
    std::cout<<"\nfinal states"<<endl;
    for(int i=0;i<matrix->finalStates.size();i++){
        std::cout<<matrix->finalStates[i]<<" belongs to ";
        for(int j=0;j<matrix->FbelongsTo[i].size();j++)
            std::cout<<matrix->FbelongsTo[i][j]<<" ";
        std::cout<<endl;
    }
   
   
    std::cout<<"\nrange expressions"<<endl;
    for(int i=0;i<matrix->ranges.size();i++){
        std::cout<<"from "<<matrix->ranges[i].start<<" to " <<matrix->ranges[i].end<<", neg = "<<matrix->ranges[i].neg<<" with reference: ";
        for(int j=0;j<matrix->ranges[i].len;j++)
            std::cout<<matrix->ranges[i].ref[j];
        std::cout<<" belongs to: ";
        for(int j=0;j<matrix->rangeBelongsTo[i].size();j++)
            std::cout<<matrix->rangeBelongsTo[i][j]<<" ";
        std::cout<<endl;
    }
    if(matrix->ranges.size()==0) std::cout<<"\nno range expressions\n";
    std::cout<<endl; 
};

/**
 * Translates a string of hex characters into a vector of uint16_t elements
 * @param hex is the string
 * @return a vector containing uint16_t encoding of hex characters
 */
vector<uint16_t> hex_string_to_char_vec(string hex){
    stringstream ss(hex);
    char delim = '\\';
    // alphabet
    vector<uint16_t> r;
    string sub;
    // cout<<range_ref<<endl;
    while (getline (ss, sub, delim)) {
        // cout<<"range sub "<<sub<<endl;
        if(sub.size()>1){
            uint16_t c = (uint16_t)strtol(sub.substr(1,sub.size()-1).c_str(), NULL, 16);
            // cout<<"parsing in range "<<c<<endl;
            
            r.push_back(c);
        }
    }
    return r;
}

/**
 * Translates a string of hex characters into a vector of int elements
 * @param s is the string
 * @return a vector containing int encoding of hex characters
 */
vector<int> string_to_int_vec(string s){
    stringstream ss(s);
    char delim = ',';
    // alphabet
    vector<int> r;
    string sub;
    // cout<<range_ref<<endl;
    while (getline (ss, sub, delim)) {
        // cout<<"range sub "<<sub<<endl;
        if(sub.size()>0){
            r.push_back(stoi(sub));
        }
    }
    return r;
}

/**
 * Tokenizes a string into a vector of sub-strings separated by ' '
 * @param line is the string
 * @return a vector containing the sub-strings
 */
vector<string> line_to_strings(string line){
    stringstream ss(line);
    char delim = ' ';
    // alphabet
    vector<string> r;
    string sub;
    // cout<<range_ref<<endl;
    while (getline (ss, sub, delim)) {
        // cout<<"range sub "<<sub<<endl;
        if(sub.size()>0){
            r.push_back(sub);
        }
    }
    return r;
}

/**
 * Loads a MFSA from ANML file
 * @param filename is the name of the ANML file
 * @param m is the MFSA structure to save the loaded data
 */
void load_nfa_from_file(string &filename,  mergedCOO *m ) {

    // sample:
    //  <state-transition-element id="37" symbol-set="[\x4f]" start="14">
    // 	<activate-on-match element="38"/> <available-re="2,10,14"/>
    // 	<report-on-match reportcode="10,2"/>
    //  </state-transition-element>

    std::ifstream ifs(filename);
    std::string line;
    std::string s1 = "<state-transition-element";
    std::string s2 = "<state-transition-range-element";

    int idx=0;
    while (getline(ifs, line, '\n')){
        if(strstr(line.c_str(),s2.c_str())){
            // cout<<"parsing "<<line<<endl;

            // range sample
            range new_range;
            // <state-transition-element id="0_30" symbol-set="[^\x49\x52\x52]" start="-1">
            string sub; 
            vector<string> result = line_to_strings(line);
            // state 
            int state = stoi(result[1].substr(4, result[1].size()-5));
            // cout<<"\n\nstate "<<state<<endl;
            m->rowIdx.push_back(state);
            new_range.start = state;

            // cout<<"from "<<state<<endl;

            m->nonzeroValues.push_back(EPS);
            // nnz + hex to dec conversion
            char n = result[2][13];
            // for(int k=0;k<result[2].size();k++){
            //     cout<<result[2][k]<<" ";
            // }
            string range_ref;
            // cout<<"len "<<result[2].size()<<endl;
            if(n=='^'){
                // negated range
                new_range.neg = 1;
                range_ref = result[2].substr(14, result[2].size()-16);
            } else {
                // non-negated range
                new_range.neg = 0;
                range_ref = result[2].substr(13, result[2].size()-15);
                // cout<<"building new range with ref "<<range_ref<<endl;
            }
            // for(int k=0;k<range_ref.size();k++){
            //     cout<<range_ref[k]<<" ";
            // }
            // cout<<"aval len "<<aval.size()<<endl;
            vector<uint16_t> r = hex_string_to_char_vec(range_ref);
            // for (int rr = 0; rr < r.size(); rr++){
            //     cout<<rr<<" range "<<r[rr]<<endl;
            //     std::bitset<16> y(r[rr]);
            //     std::cout << y << '\n';
            // }
            uint16_t mask_sol = 1 << 9;
            // cout<<r[0]<<endl;
            if((r[0] & mask_sol) == mask_sol){ // anchor state
                // first anchor
                for (int rr = 0; rr < r.size(); rr++){
                    r[rr] -= mask_sol;
                }
                m->anchored_sol_states.push_back(state);
                // cout<<"anchoring "<<state<<endl;
            }
            // a possible additional mask is removed afterwards during the analysis of the character set       
            new_range.len = r.size();
            new_range.ref = r;
            if(result[3][7]!='-'){ // initial state 
                // push init states 
            //   cout<<"initial string "<<result[3].substr(7, result[3].size()-9)<<endl;
                vector<int> init_states = string_to_int_vec(result[3].substr(7, result[3].size()-9));
                for(int j=0;j<init_states.size();j++){
                  // cout"state "<<state<<" initial for "<<init_states[j]<<endl;
                    m->initialStates.push_back(state);
                    m->IbelongsTo.push_back(init_states[j]);
                }
            }
        	// <activate-on-match element="1"/> <available-re="0,1"/>

            getline(ifs, line, '\n');
            result = line_to_strings(line);
            
            stringstream ss_tmp(result[1]);
            vector<std::string> result_tmp;
            // destination 
            int dest = stoi(result[1].substr(9, result[1].size()-12));
            // cout<<"dest "<<dest<<endl;
            m->colIdx.push_back(dest);
            new_range.end = dest;
            m->ranges.push_back(new_range);
            // cout<<" string  to convert: "<<result[2].substr(15, result[2].size()-18)<<endl;
            vector<int> aval = string_to_int_vec(result[2].substr(15, result[2].size()-18));
            // for(int a=0;a<aval.size();a++){
                // cout<<"availabilities "<<aval[a]<<endl;
            // }
            m->rangeBelongsTo.push_back(aval);  
            m->belongsTo.push_back(aval);
            // possibly final 
            getline(ifs, line, '\n');
            string se3 = "report-on-match";
            // <state-transition-element id="37" symbol-set="[\x4f]" start="14">
            // 	<activate-on-match element="38"/> <available-re="2,10,14"/>
            // 	<report-on-match reportcode="10,2"/>
            //  </state-transition-element>
            if(strstr(line.c_str(),se3.c_str())){
              // cout"final line "<<line<<endl;
              // cout"final string "<<line.substr(30, line.size()-34)<<endl;
                // final state detected
                // <report-on-match reportcode="0,0"/>
                // cout<<line<<endl;
                m->finalStates.push_back(dest);
                vector<int> final_codes = string_to_int_vec(line.substr(30, line.size()-33));
              // coutdest<<" final codes are: ";
                // for(int i=0;i<final_codes.size();i++){
                //     cout<<final_codes[i]<<" ";
                // }
                // cout<<endl;
                m->FbelongsTo.push_back(final_codes);
            }
        } else if (strstr(line.c_str(),s1.c_str())){
            // cout<<"parsing "<<line<<endl;
            // std transition
            // <state-transition-element id="0_30"  symbol-set="[\x49]"  start="-1">
            vector<string> result = line_to_strings(line); 
            // state 
            int state = stoi(result[1].substr(4, result[1].size()-5));
            // cout<<"\n\nstate "<<state<<endl;
            m->rowIdx.push_back(state);
            // nnz + hex to dec conversion
            uint16_t chr = (uint16_t)strtol((result[2].substr(15, result[2].size()-16)).c_str(), NULL, 16);


            uint16_t mask_sol = 1 << 9; 
            // cout<<"mask is "<<mask<<endl;
            int flag_anchor=0;
            if((chr&mask_sol)==mask_sol){
                // cout<<"from "<<chr<<" to ";
                chr -= mask_sol;
                // cout<<chr<<endl;
                flag_anchor=1;
            }
            m->nonzeroValues.push_back(chr);
            // init flag
            if(result[3][7]!='-'){
                // push init states 
                // cout<<"initial line "<<result[3]<<endl;
                vector<int> init_states = string_to_int_vec(result[3].substr(7, result[3].size()-9));
                for(int j=0;j<init_states.size();j++){
                    // cout<<"initial state"<<state<<"for "<<init_states[j]<<endl;
                    m->initialStates.push_back(state);
                    m->IbelongsTo.push_back(init_states[j]);
                    if(flag_anchor){
                        m->anchored_sol_states.push_back(state);
                        // cout<<"anchoring "<<state<<endl;
                    }

                }
            }
        	// <activate-on-match element="5"/> <available-re="0,1"/>

            // cout<<"old line is "<<line<<endl;
            getline(ifs, line, '\n');
            // cout<<"new line is "<<line<<endl;


            result = line_to_strings(line);
            

            // cout<<"SECOND string to convert: "<<result[2].substr(15, result[2].size()-18)<<endl;

            vector<int> aval = string_to_int_vec(result[2].substr(15, result[2].size()-18));
            
            // destination 
            int dest = stoi(result[1].substr(9, result[1].size()-10));
            // cout<<"dest "<<dest<<endl;
            m->colIdx.push_back(dest);
            
            m->belongsTo.push_back(aval);  
            // possibly final 
            getline(ifs, line, '\n');
            string se3 = "report-on-match";
            if(strstr(line.c_str(),se3.c_str())){
                // final state detected
                // <report-on-match reportcode="0,0"/>
                m->finalStates.push_back(dest);
                // cout<<line<<endl;

                // cout<<"final state "<<dest<<endl;
                string codes = line.substr(30, line.size()-33);
                vector<int> r = string_to_int_vec(codes);
                // cout<<"for:";
                // for(int i=0;i<r.size();i++){
                //     cout<<r[i]<<" ";
                // }
                // cout<<endl;
                m->FbelongsTo.push_back(r);
            }
        }

    }
    // printAutomaton_multi(m);
	// return m;
}


/**
 * Checks whether a character encoded in uint16_t belongs in a character class
 * @param c is the character
 * @param range_ref is the character class to check
 * @return 1 if c belongs in range_ref, 0 otherwise
 */
int inRange(uint16_t c, vector<uint16_t> range_ref){

    for(int i=0;i<range_ref.size();i++){
        if(c==range_ref[i]){
            return 1;
        }
    }
    return 0;
}


/**
 * Translate a MFSA into a iMFAnt-compliant format
 * @param m is the initial MFSA
 * @param nfa is the MFSA in iMFAnt-compliant format
 */
void mergedCOO2MiNFAnt(mergedCOO *m, mfsa *nfa){

    // printAutomaton_multi(m);

    nfa->alphabet = new vector<char_vector>();

    for(int i=0;i<ALPHA_SIZE;i++){
        char_vector cv;
        cv.enabling_character=(unsigned char)i;
        cv.pointers_to_transition = new vector<transition>();
        nfa->alphabet->push_back(cv);
    }
    // ranges processing 
    // remove eps arcs depending on ranges 
    uint16_t mask_sol = 1 << 9;
    for(int r=m->nonzeroValues.size()-1;r>=0;r--){
        if((m->nonzeroValues[r]==EPS || m->nonzeroValues[r]==(EPS|mask_sol)) && correspondingRange(m, m->rowIdx[r], m->colIdx[r])!=-1){
            m->nonzeroValues.erase(m->nonzeroValues.begin()+r);
            m->rowIdx.erase(m->rowIdx.begin()+r);
            m->colIdx.erase(m->colIdx.begin()+r);
            m->belongsTo.erase(m->belongsTo.begin()+r);
        }
    }
    for(int r=0;r<m->ranges.size();r++){
        vector<uint16_t> actual_enabled = ref_to_list(m->ranges[r].ref);
        if(m->ranges[r].neg){
            // push a new transition for every char not involved in the range 
            for(int i=0;i<ALPHA_SIZE;i++){
                if(!inRange((unsigned char)i, actual_enabled)){
                    m->rowIdx.push_back(m->ranges[r].start);
                    m->colIdx.push_back(m->ranges[r].end);
                    m->nonzeroValues.push_back((unsigned char)i);
                    m->belongsTo.push_back(m->rangeBelongsTo[r]);
                    // cout<<"pushing char "<<(int)i<<", ";
                }
            }
        } else {
            // push a new transition for every char involved in the range
            for(int i=0;i<ALPHA_SIZE;i++){
                if(inRange((unsigned char)i, actual_enabled)){
                    m->rowIdx.push_back(m->ranges[r].start);
                    m->colIdx.push_back(m->ranges[r].end);
                    m->nonzeroValues.push_back((unsigned char)i);
                    m->belongsTo.push_back(m->rangeBelongsTo[r]);
                    // cout<<"pushing char "<<(int)i<<", ";
                }
            }
        }
    }
    for(int i=0;i<m->nonzeroValues.size();i++){
        // check whether entry exists
        // check existence of ranges 
        // ranges are treated separately, the alphabet enabling a transition belonging to a range are already there
        if(nfa->alphabet->at((uint16_t) m->nonzeroValues[i]).pointers_to_transition->size()==0){
            // prepare new entry 
            transition t;
            t.available=m->belongsTo[i];
            t.src = m->rowIdx[i];
            t.dst = m->colIdx[i];
            vector<int> v = isInit_trans(m, i);
            if(v.size()>0){
                t.isInit = v;
            }
            vector<int> w = isFin_trans(m, i);
            if(w.size()>0){
                t.isFin = w;
            }
            
            // char_vector tup;
            // tup.enabling_character=m->nonzeroValues[i];
            // tup.pointers_to_transition->push_back(t);
            nfa->alphabet->at((int)m->nonzeroValues[i]).enabling_character=m->nonzeroValues[i];
            nfa->alphabet->at((int)m->nonzeroValues[i]).pointers_to_transition->push_back(t);
            // cout<<"new entry for "<<m->nonzeroValues[i]<<endl;
        } else {
            transition t;
            t.available= m->belongsTo[i];
            t.src = m->rowIdx[i];
            t.dst = m->colIdx[i];
            vector<int> v = isInit_trans(m, i);
            if(v.size()>0){
                t.isInit = v;
            }
            vector<int> w = isFin_trans(m, i);
            if(w.size()>0){
                t.isFin = w;
            }
            nfa->alphabet->at((int)m->nonzeroValues[i]).pointers_to_transition->push_back(t);
        }

   }
    nfa->anchor_states = m->anchored_sol_states;
}


/**
 * Prints MFSA in iMFAnt-compliant format
 * @param nfa is the MFSA
 */
void print_nfa(mfsa *nfa){
    cout<<"PRINTING NFA"<<endl;
    for(int i=0;i<ALPHA_SIZE;i++){
        if(nfa->alphabet->at(i).pointers_to_transition->size()>0){
            cout<<"character "<<hex<<i<<" enables the following transitions:"<<endl;
            for(int j=0;j<nfa->alphabet->at(i).pointers_to_transition->size();j++){
                cout<<"from "<<dec<<(int)nfa->alphabet->at(i).pointers_to_transition->at(j).src<<" to "<<dec<<(int)nfa->alphabet->at(i).pointers_to_transition->at(j).dst<<endl;
                cout<<"final for: ";
                for(int k=0;k<nfa->alphabet->at(i).pointers_to_transition->at(j).isFin.size();k++)
                    cout<<nfa->alphabet->at(i).pointers_to_transition->at(j).isFin[k]<<" ";
                cout<<endl;
                cout<<"initial for: ";
                for(int k=0;k<nfa->alphabet->at(i).pointers_to_transition->at(j).isInit.size();k++)
                    cout<<nfa->alphabet->at(i).pointers_to_transition->at(j).isInit[k]<<" ";
                cout<<endl;
                cout<<"active for: ";
                for(int k=0;k<nfa->alphabet->at(i).pointers_to_transition->at(j).available.size();k++)
                    cout<<nfa->alphabet->at(i).pointers_to_transition->at(j).available[k]<<" ";
                cout<<endl;
            }
        }
    }

    cout<<"ancored sol states: ";
    for(int i=0;i<nfa->anchor_states.size();i++){
        cout<<nfa->anchor_states[i]<<" ";
    }
    cout<<endl;
}


/**
 * Prints current matching path
 * @param mp is the matching path
 */
void print_mp(matching_path *mp){
    cout<<"---CURRENT MATCHING PATH---"<<endl; 
    for(int i=0;i<mp->sv->size();i++){
        cout<<i<<": "<<mp->sv->at(i)<<" [";
        for(int j=0; j<mp->active_re->at(i)->size(); j++){
            cout<<mp->active_re->at(i)->at(j)<<",";
        }
        cout<<"] | ";
    }
    cout<<endl;
}


/**
 * Given a set of active FSAs for a transition and a set of FSAs final for a states, returns matched FSAs
 * @param finals contains the FSAs ids for which a state is final
 * @param active contains the FSAs currently active
 * @return vector of matched FSAs, i.e., simultaneously final and active
 */
vector<int> check_match(vector<int> finals, vector<int> *active){

    // cout<<"CURRENTLY ACTIVE"<<endl; 
    // for(int i=0;i<active.size();i++){
    //     cout<<active[i]<<" ";
    // }
    // cout<<endl;
    vector<int> match;
    if(finals.size()>0){
        // if both final and active for some match, there exists a match
        vector<int> found; 
        for(int f=0; f<finals.size(); f++){
            for (int ac=0; ac<active->size(); ac++){
                if(active->at(ac)==finals[f]){  
                    match.push_back(ac);
                }
            }
        }
    }
    // if(match.size()>0){
    //     cout<<"+++ MATCH FOUND FOR ";
    //      for(int m = match.size()-1; m>=0; m--){
    //          cout<<"matched: "<<active->at(m)<<" ";
    //      }   
    //      cout<<" +++"<<endl; 
    // }
    sort(match.begin(), match.end());
    match.erase(unique(match.begin(), match.end()), match.end());
    return match;
}


/**
 * Activates a set of moves on a matching path
 * @param to_activate contains the transitions to activate and the FSAs they are available for
 * @param m contains the current matching path
 * @param initials contains the current matching path
 */
void activate(vector<multi_move> *to_activate, matching_path *mp){

    // if a state is currently active but not in the to_activate list, it must be cleared

    for(int m = 0; m<mp->sv->size(); m++){
        if(mp->sv->at(m)){
            int f = 0; 
            for(int a=0; a<to_activate->size(); a++){
                if(m == to_activate->at(a).from){
                    f=1;
                }
            }
            if(!f){
                mp->sv->at(m) = 0;
                mp->active_re->at(m)->clear();
            }
        }
    }


    for(int m=0; m<to_activate->size(); m++){
        mp->sv->at(to_activate->at(m).from) = 0;
        mp->active_re->at(to_activate->at(m).from)->clear();
    }

    for (int  m = 0; m < to_activate->size(); m++){
        // cout<<"activating from "<<to_activate->at(m).from<<" to "<<to_activate->at(m).to<<endl;
        for(int a = 0; a<to_activate->at(m).active->size(); a++){
            mp->active_re->at(to_activate->at(m).to)->push_back(to_activate->at(m).active->at(a));

        }
        sort(mp->active_re->at(to_activate->at(m).to)->begin(), mp->active_re->at(to_activate->at(m).to)->end());
        mp->active_re->at(to_activate->at(m).to)->erase(unique(mp->active_re->at(to_activate->at(m).to)->begin(), mp->active_re->at(to_activate->at(m).to)->end()), mp->active_re->at(to_activate->at(m).to)->end());
        // cout<<"de-activating "<<to_activate->at(m).from<<endl;
        mp->sv->at(to_activate->at(m).to) = 1;

        // print_mp(mp);


    }

}

/**
 * Checks whether a transition is valid
 * @param currently_active contains the ids of currently active FSAs
 * @param available contains the ids of available FSAs on a transition
 * @return 1 if the transition is valid, 0 otherwise
 */
int valid_transition(vector<int> *currently_active, vector<int> available){
    // at least one id is both active and available
    if(V){
        cout<<"currently active: ";
        for(int j=0; j< currently_active->size(); j++)
            cout<<currently_active->at(j)<<" ";
        cout<<"\navailable: ";
        for(int j=0;j<available.size(); j++)
            cout << available[j]<<" ";
        cout<<endl;
    }
    for(int i=0; i<currently_active->size(); i++){
        for(int j=0; j<available.size(); j++){
            if(currently_active->at(i)==available[j]){
                return 1;
            }
        }
    }
    return 0;
}

void print_transition(transition t){
    cout<<"from "<<dec<<t.src<<" to "<<dec<<t.dst<<endl;
    cout<<"initial for: ";
    for(auto i: t.isInit)
        cout<<dec<<i<<" ";
    cout<<"\nfinal for: ";
    for(auto j: t.isFin)
        cout<<dec<<j<<" "; 
    cout<<"\navailable for: ";
    for(auto j: t.available)
        cout<<dec<<j<<" "; 
}

/**
 * iMFAnt matching algorithm
 * @param mp clear matching path
 * @param nfa MFSA to match
 * @param input input stream to match
 * @return number of matches
 */
int infant(matching_path *mp, mfsa *nfa, string& input){

    // print_nfa(nfa);


    if(input.size()==0)
        return 0;

    int match = 0;


    int size = MB;

    if(input.size()<MB){
        size = input.size();
    }
    
    for (int ic = 0;ic<size;ic++){
        
        vector<int> backup; 
        for(int i=0; i<mp->sv->size(); i++){
            backup.push_back(mp->sv->at(i));
            // cout<<mp->sv->at(i)<<" ";
        }
        if(V)
            print_mp(mp);
        unsigned char c = input[ic];

        // cout<<"--- matching "<<hex<<(int)c<<" ---"<<endl;
        
        int f = 0;
        if(nfa->alphabet->at((uint16_t)c).pointers_to_transition->size()>0){
            vector<int> initials; 
            vector<multi_move> *mm = new vector<multi_move>();
            // check all transitions
            for(int tr = 0; tr<nfa->alphabet->at((uint16_t)c).pointers_to_transition->size(); tr++){
                // print_transition(nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr));
                // for each transition enabled by character c, check whether the departure state is currently 
                // active or, if ic==0, whether it is initial 
                // if the departure state is currently enabled
                if(mp->sv->at(nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).src)== 1 && valid_transition(mp->active_re->at(nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).src),nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).available)){ // il nor updated mi impedisce di attraversare certi percorsi?
                    // cout << "activating "<< mp->sv->at(nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).dst)<<endl;
                    multi_move m; 
                    m.to = nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).dst;
                    m.from = nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).src;
                    // push ids rooted in the current src to enable their
                    m.active = new vector<int>();
                    for(int a = 0; a <nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).isInit.size(); a++){
                        m.active->push_back(nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).isInit[a]);
                    }
                    // among the ones already active, keep the available ones only
                    for(int a=0; a<mp->active_re->at(nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).src)->size(); a++){
                        for(int v = 0; v<nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).available.size(); v++){
                            if(mp->active_re->at(nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).src)->at(a)==nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).available[v]){
                                m.active->push_back(mp->active_re->at(nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).src)->at(a));
                                }
                        }
                    }
                    // remove duplicates
                    sort(m.active->begin(), m.active->end());
                    m.active->erase(unique(m.active->begin(), m.active->end()), m.active->end());
                    // check matches
                    if(m.active->size()>0) {
                        vector<int> mf = check_match(nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).isFin, m.active);
                        if(mf.size()>0){
                            // cout<<" at "<<ic<<endl;
                            for(int ma = mf.size()-1; ma>=0; ma--){
                                m.active->erase(m.active->begin()+mf[ma]);
                            }
                        }   
                        mm->push_back(m);
                        match+=mf.size();
                        f++;
                    }
                } else if (nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).isInit.size()>0){ // not active, but can be activated (initial state)
                    int isAnchor = 0; 
                    int proceed = 1; 
                    // cout<<"attempting to activate "<<nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).dst<<endl;
                    // check if state is anchor 
                    for(int an = 0; an<nfa->anchor_states.size(); an++){
                        if (nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).src == nfa->anchor_states[an])
                            isAnchor = 1;
                    }
                    if(isAnchor && ((ic>0 && input[ic-1]!='\n') || (ic==0))){
                        proceed=0;
                    }
                    if (proceed){
                        multi_move m;
                        m.to = nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).dst;
                        m.from = nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).src;
                        m.active = new vector<int>();
                        initials.push_back(nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).dst);
                        // check matches 
                        vector<int> tmp_match;
                        for(int a=0; a<nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).isInit.size(); a++){
                            int l=0;
                            for(int b=0;b<nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).isFin.size(); b++){
                                if(nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).isInit[a]==nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).isFin[b]){
                                    tmp_match.push_back(nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).isInit[a]);
                                    l++;
                                }
                            }
                            if(!l){
                                m.active->push_back(nfa->alphabet->at((uint16_t)c).pointers_to_transition->at(tr).isInit[a]);
                            }
                        }
                        sort(tmp_match.begin(), tmp_match.end());
                        tmp_match.erase(unique(tmp_match.begin(), tmp_match.end()), tmp_match.end());
                        match+=tmp_match.size();
                        // if(tmp_match.size()){
                        //     cout<<"+++ initial match +++ "<<ic<<endl;
                        // }
                        if(m.active->size()>0){
                            mm->push_back(m);
                        }
                        f++;
                    }
                    
                }
            }
            if(!f){
                // cout<<"no match for "<<hex<<c<<endl;
                for(int n=0;n<mp->active_re->size();n++)
                    mp->active_re->at(n)->clear();
                for(int i=0;i<mp->sv->size();i++)
                    mp->sv->at(i)=0;
            } else {
                // activate moves
                activate(mm, mp); 
            }
            for (int i = 0; i < mm->size(); i++)
            {
                delete mm->at(i).active;
            }
            delete mm;
        } else {
            for(int n=0;n<mp->active_re->size();n++)
                mp->active_re->at(n)->clear();
            for(int i=0;i<mp->sv->size();i++)
                mp->sv->at(i)=0;
        }
        // print_mp(mp);
    }
    return match; 
}