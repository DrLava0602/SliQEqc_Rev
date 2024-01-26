#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <iostream>
#include <stdio.h> // FILE
#include <unordered_map>
#include <sys/time.h> //estimate time
#include <fstream> //fstream
#include <sstream> // int to string
#include <cstdlib> //atoi
#include <string> //string
#include <sstream>
#include <random>
#include <cmath>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include "../cudd/cudd/cudd.h"
#include "../cudd/cudd/cuddInt.h"
#include "../cudd/util/util.h"


#define PI 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899

class Simulator
{
public:
    // constructor and destructor
    Simulator(int nshots, int seed, int bitSize, bool reorder, bool alloc) :
    n(0), r(bitSize), w(4), k(0), inc(3), shift(0), error(0),
    normalize_factor(1), gatecount(0), NodeCount(0), isMeasure(0), shots(nshots), isReorder(reorder), isAlloc(alloc)
    , statevector("null"), gen(std::default_random_engine(seed)){
    }
    ~Simulator()  {
        clear();
    }

    /* gates */
    void Toffoli(int targ, std::vector<int> cont, std::vector<int> ncont, int cur_bit);
    void Fredkin(int swapA , int swapB, std::vector<int> cont, int cur_bit);
    void PauliX(int iqubit, int cur_bit);


    /* measurement */
    void measurement();
    void getStatevector();

    /* simulation */
    void init_simulator(int n);
    void sim_qasm_eqc(int cur_bit);
    void sim_qasm(std::string qasm);
    void print_results();
    bool check_equ();

    /* misc */
    void reorder();
    void decode_entries();
    void print_info(double runtime, size_t memPeak);
    std::string qasmfile;

private:
    DdManager *manager;
    DdNode ***All_Bdd;
    DdNode **Initial_BDD;
    DdNode **Single_Bdd;
    int n; // # of qubits
    int current_bit;
    int r; // resolution of integers
    int w; // # of integers
    int k; // k in algebraic representation
    int inc; // add inc BDDs when overflow occurs, used in alloc_BDD
    int shift; // # of right shifts
    int shots;
    bool isMeasure;
    bool isReorder;
    bool isAlloc;
    int nClbits;
    std::vector<std::vector<int>> measured_qubits_to_clbits; // empty if not measured
    std::string measure_outcome;
    double normalize_factor; // normalization factor used in measurement
    DdNode *bigBDD; // big BDD used if measurement
    std::default_random_engine gen; // random generator
    std::unordered_map<DdNode *, double> Node_Table; // key: node, value: summed prob
    std::unordered_map<std::string, int> state_count;
    std::string statevector;
    std::string run_output; // output string for Qiskit

    unsigned long gatecount;
    unsigned long NodeCount;
    double error;

    /* measurement */
    double measure_probability(DdNode *node, int kd2, int nVar, int nAnci_fourInt, int edge);
    void measure_one(int position, int kd2, double H_factor, int nVar, int nAnci_fourInt, std::string *outcome);

    /* misc */
    void init_state(int *constants);
    void alloc_BDD(DdNode ***Bdd, bool extend);
    void dropLSB(DdNode ***Bdd);
    int overflow3(DdNode *g, DdNode *h, DdNode *crin);
    int overflow2(DdNode *g, DdNode *crin);
    void nodecount();

    // Clean up Simulator
    void clear() {
        for(int i=0;i<n;i++) Cudd_RecursiveDeref(manager, Single_Bdd[i]);
        delete[] Single_Bdd;
        measured_qubits_to_clbits.clear();
        measure_outcome.clear();
        Node_Table.clear();
        state_count.clear();
        Cudd_Quit(manager);
    };
};

#endif
