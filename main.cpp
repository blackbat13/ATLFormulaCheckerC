#include <iostream>
#include "Models/BridgeModel.h"
#include "Models/asynchronous/GlobalModel.h"
#include "Models/asynchronous/parser/GlobalModelParser.h"
#include <sys/time.h>
#include <fstream>
#include <cstring>
#include <thread>

#define MODEL_ID 10
#define RAW_PERF_OUT true

#if MODEL_ID == 1
    #include "TestModels/Model01.cpp"
#elif MODEL_ID == 2
    #include "TestModels/Model02.cpp"
#elif MODEL_ID == 3
    #include "TestModels/Model03.cpp"
#elif MODEL_ID == 4
    #include "TestModels/Model04.cpp"
#elif MODEL_ID == 5
    #include "TestModels/Model05.cpp"
#elif MODEL_ID == 6
    #include "TestModels/Model06.cpp"
#elif MODEL_ID == 7
    #include "TestModels/Model07.cpp"
#elif MODEL_ID == 8
    #include "TestModels/Model08.cpp"
#elif MODEL_ID == 9
    #include "TestModels/Model09.cpp"
#elif MODEL_ID == 10
    #include "TestModels/Model10.cpp"
#endif

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
    
    #if MODEL_ID <= 6
        // For models 1-6
        auto parallel = getModel();
    #elif MODEL_ID <= 8
        // For models 7, 8
        int n = strtol(argv[1], NULL, 10);
        int k = strtol(argv[2], NULL, 10);
        auto parallel = getModel(n, k);
    #elif MODEL_ID <= 9
        // For models 8, 9
        int n = strtol(argv[1], NULL, 10);
        int m = strtol(argv[2], NULL, 10);
        int k = strtol(argv[3], NULL, 10);
        auto parallel = getModel(n, m, k);
    #elif MODEL_ID <= 10
        // For model 10
        int n = strtol(argv[1], NULL, 10);
        int m = strtol(argv[2], NULL, 10);
        int l = strtol(argv[3], NULL, 10);
        int k = strtol(argv[4], NULL, 10);
        auto parallel = getModel(n, m, l, k);
    #endif

    auto expectedResult = getExpectedResult();


    // Verification
    gettimeofday(&tb, NULL);
    // bool result = parallel->parallelRecursiveDFS(0, -1, ParallelModel::standard, 0);
    // bool result = parallel->recursiveDFS(0, -1, ParallelModel::standard, 0);
    // bool result = parallel->parallelRecursiveDFS(0, -1, ParallelModel::standard, 0);
    bool result = parallel->forkRecursiveDFS(0, t);
    gettimeofday(&te, NULL);

    #if RAW_PERF_OUT
        for (int i = 1; i < argc; ++i) {
            cout << argv[i] << " ";
        }
        cout << (1000000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec)) << endl;
    #else
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
    #endif

    return 0;
}
