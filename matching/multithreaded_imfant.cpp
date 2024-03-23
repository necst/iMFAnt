#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <omp.h>
#include "iMFAnt.hpp"


using namespace std; 


int main(int argc, char *argv[]){

    if(argc != 5){
        cout<<"wrong number of arguments"<<endl; 
        return 0;
    }

    int reps = REPS;


    ofstream output_file;

    int num_of_automata = stoi(argv[3]);


    output_file.open(argv[4], std::ios::app);

    ifstream input_string_file(argv[1], std::ios::binary);

    std::stringstream buffer;
    buffer << input_string_file.rdbuf();

    // int max_threads = omp_get_max_threads();

    // cout<<"max threads: "<<max_threads<<endl;

    // to set the number of threads
    // export OMP_NUM_THREADS=8

    string input_str = buffer.str();
    
    
    vector<mfsa *> *mfsas = new vector<mfsa *>; 
    vector<int> *maximi = new vector<int>; 

    // for every anml file in the directory
    for (int j = 0; j < num_of_automata; j++){
        // cout<<dirEntry.path()<<endl;
        string s = argv[2];
        // cout<<s+"automaton"+to_string(j)+".anml"<<endl;
        mergedCOO* mfsa_pre =  new mergedCOO();
        string file_name = s+"automaton"+to_string(j)+".anml";
        cout<<file_name<<endl;
        load_nfa_from_file(file_name, mfsa_pre);
        // printAutomaton_multi(mfsa_pre);/
        if(mfsa_pre->rowIdx.size()>0){
            maximi->push_back(max(mfsa_pre));
            mfsa *nfa = new mfsa();
            mergedCOO2MiNFAnt(mfsa_pre, nfa);
            delete mfsa_pre;
            mfsas->push_back(nfa);
        } else {
            delete mfsa_pre;
        }
    }

    vector<int> *matches = new vector<int>; 
    for(int i=0; i<mfsas->size(); i++){
        matches->push_back(0);
    }

    for (int i=0; i<REPS; i++){
        vector<matching_path *> mps = vector<matching_path *>();
        for(int l=0; l<mfsas->size(); l++){
            // cout<<maximi[i]<<endl;
            matching_path *mp = new matching_path;
            mp->sv = new vector<int>;
            mp->active_re = new vector<vector<int> *>;
            for(int j=0;j<maximi->at(l)+1;j++){
                mp->sv->push_back(0);
                vector<int> *v= new vector<int>;
                mp->active_re->push_back(v);
            }
            mps.push_back(mp);
        }
        cout<<"structure created"<<endl;
        cout<<"repetition "<<i<<endl;
        std::string input_str  = buffer.str();
        int iam;
        auto start = std::chrono::high_resolution_clock::now();
        //omp_set_dynamic(0);
        //  #pragma omp parallel num_threads(100)
        {
            #pragma omp parallel for
            // #pragma omp parallel for num_threads(10)
            // #pragma opm for shared(mfsas, mps, buffer, input_str, matches) num_threads(1)
            for(int j=0; j<mfsas->size(); j++){
                cout<<"automata "<<j<<endl;
                // matches->at(j) = infant(mps->at(j), mfsas->at(j), 0, buffer.str());
                
                /*//check if correct number of threads
                iam = omp_get_thread_num();
                cout << "GNOOOOO " << iam << endl;*/
                matches->at(j) = infant(mps.at(j), mfsas->at(j), input_str);
                
            }
        }
        // #pragma omp barrier
        auto end = std::chrono::high_resolution_clock::now();

         for(int i=0; i<mfsas->size(); i++){
            mps.at(i)->sv->clear();
            delete mps.at(i)->sv;
             for(int j=0;j<mps.at(i)->active_re->size();j++){
                mps.at(i)->active_re->at(j)->clear();
                delete mps.at(i)->active_re->at(j);
            }
            mps.at(i)->active_re->clear();
            delete mps.at(i)->active_re;
            delete mps.at(i);
        }
        
        mps.clear();
        //delete mps;

        int mm=0; 
        for(int j=0; j<matches->size(); j++){
            output_file<<"RE "<<j<<": "<<matches->at(j)<<endl;
            mm+=matches->at(j);
        }
        
        output_file<<std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<<","<<mm<<endl;

    }
    
    for(int i=0; i<mfsas->size(); i++){
        delete mfsas->at(i);
    }    

}
