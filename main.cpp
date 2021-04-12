#include <iostream>
#include "Models/BridgeModel.h"
#include "Models/asynchronous/GlobalModel.h"
#include "Models/asynchronous/parser/GlobalModelParser.h"
#include <sys/time.h>
#include <fstream>
#include <cstring>
#include <thread>

// #include "TestModels/Model01.cpp"
// #include "TestModels/Model02.cpp"
// #include "TestModels/Model03.cpp"
// #include "TestModels/Model04.cpp"
// #include "TestModels/Model05.cpp"
// #include "TestModels/Model06.cpp"
// #include "TestModels/Model07.cpp"
// #include "TestModels/Model08.cpp"
#include "TestModels/Model09.cpp"

using namespace std;

int main(int argc, char **argv) {
    struct timeval tb, te;

    // auto parallel = new ParallelModel(3);   // parameter - number of states
    // cout << "#" << parallel->threadsVector.size() << endl;
    // cout << "Parallel states size: " << parallel->states.size() << endl;

    // // Define the model

    // // Transitions
    // parallel->states[0]->addTransition(1, 1);   // addTransition(action, nextState)
    // parallel->states[0]->addTransition(2, 2);   // Actions coming from one state should be sorted (by action number)

    // // Epistemic classes
    // parallel->unify(1, 2);  // unify states in epistemic classes

    // // Winning states
    // parallel->setAccept();   // mark winning states
    
    
    // For models 1-6
    // auto parallel = getModel();
    
    // For models 7 and 8
    // int n = strtol(argv[1], NULL, 10);
    // int k = strtol(argv[2], NULL, 10);
    // auto parallel = getModel(n, k);
    
    // For model 9
    int n = strtol(argv[1], NULL, 10);
    int m = strtol(argv[2], NULL, 10);
    int k = strtol(argv[3], NULL, 10);
    auto parallel = getModel(n, m, k);
    
    auto expectedResult = getExpectedResult();


    // Verification
    gettimeofday(&tb, NULL);
    bool result = parallel->parallelRecursiveDFS(0, -1, ParallelModel::standard, 0);
    // bool result = parallel->recursiveDFS(0, -1, ParallelModel::standard, 0);
    gettimeofday(&te, NULL);
    // cout << n << " " << m << " " << k << " " << (1000000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec)) << endl;
    cout << "Verification time: " << 1000000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec) << " usec" << endl;
    
    if (result == expectedResult) {
        cout << "Result OK" << endl;
    }
    else {
        cout << "Result ERR: expected " << (expectedResult ? "True" : "False") << ", got " << (result ? "True" : "False") << endl;
    }

    if (result) {
        cout << "True" << endl;
        parallel->printStrategy(0, cout);
    }
    else {
        cout << "False" << endl;
    }
    return 0;
}
