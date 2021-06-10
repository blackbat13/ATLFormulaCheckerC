#include "../Models/ParallelModel.hpp"

#define M10_DBG true

// n - number of states in each layer other than the first one
// m - number of different actions outgoing from each node
// l - number of same actions outgoing from each node
// k - number of layers
ParallelModel *getModel(int n, int m, int l, int k) {
    int numberOfStates = 1 + (k - 1) * n;
    
    auto model = new ParallelModel(numberOfStates);
    
    // Transitions outgoing from the initial state
    for (int im = 0; im < m; ++im) {
        for (int il = 0; il < l; ++il) {
            int fromId = 0;
            int toId = 1 + ((l * im + il) % n);
            int action = im;
            model->states[fromId]->addTransition(action, toId);
            #if M10_DBG
                cout << "L[" << 0 << "->" << (0 + 1) << "] " << fromId << " --" << action << "-> " << toId << endl;
            #endif
        }
    }
    
    // Transitions outgoing from states other than the initial state
    for (int ik = 1; ik < k - 1; ++ik) {
        for (int in = 0; in < n; ++in) {
            for (int im = 0; im < m; ++im) {
                for (int il = 0; il < l; ++il) {
                    int fromId = 1 + (ik - 1) * n + in;
                    int toId = 1 + ik * n + ((in + l * im + il) % n);
                    int action = im;
                    model->states[fromId]->addTransition(action, toId);
                    #if M10_DBG
                        cout << "L[" << ik << "->" << (ik + 1) << "] " << fromId << " --" << action << "-> " << toId << endl;
                    #endif
                }
            }
        }
    }
    
    
    // Epistemic classes
    // model->unify(, );
    
    // Winning states
    model->states[numberOfStates - 1]->setAccept();
    #if M10_DBG
        cout << "Accept: " << (numberOfStates - 1) << endl;
    #endif
    
    return model;
}

bool getExpectedResult() {
    return false;
}
