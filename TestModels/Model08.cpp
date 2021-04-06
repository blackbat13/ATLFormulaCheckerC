#include "TreeModelGenerator.cpp"

ParallelModel *getModel(int n, int k) {
    auto generator = new TreeModelGenerator(n, k - 1, 2);
    auto model = generator->getModel();
    auto lastStateId = generator->calculateLevelFirstStateId(k - 1) - 1;
    
    
    
    // Transitions
    
    model->states[lastStateId]->addTransition(1, lastStateId + 1);
    model->states[lastStateId]->addTransition(1, lastStateId + 2);
    
    
    
    // Winning states
    
    model->states[lastStateId + 1]->setAccept();
    model->states[lastStateId + 2]->setAccept();
    
    
    
    return model;
}

bool getExpectedResult() {
    return true;
}
