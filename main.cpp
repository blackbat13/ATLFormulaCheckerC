#include <iostream>
#include "Models/BridgeModel.h"
#include "Models/asynchronous/GlobalModel.h"
#include "Models/asynchronous/parser/GlobalModelParser.h"
#include <sys/time.h>
#include <fstream>
#include <cstring>
#include <thread>

using namespace std;

int main(int argc, char **argv) {
    struct timeval tb,te;

    auto parallel = new ParallelModel(3);   // parameter - number of states
    cout << "#" << parallel->threadsVector.size() << endl;
    cout << "Parallel states size: " << parallel->states.size() << endl;

    // Define the model

    // Transitions
    parallel->states[0]->addTransition(1,1);   // addTransition(action, nextState)
    parallel->states[0]->addTransition(2,2);   // Actions coming from one state should be sorted (by action number)

    // Epistemic classes
    parallel->unify(1,2);  // unify states in epistemic classes

    // Winning states
    parallel->setAccept();   // mark winning states


    // Verification
    gettimeofday(&tb, NULL);
    bool result = parallel->parallelRecursiveDFS(0, -1, ParallelModel::standard, 0);
    gettimeofday(&te, NULL);

    cout << "Verification time: " << 1000000*(te.tv_sec-tb.tv_sec)+(te.tv_usec - tb.tv_usec) << " usec" << endl;

    if(result) cout << "True" << endl;
    else cout << "False" << endl;
    return 0;
}