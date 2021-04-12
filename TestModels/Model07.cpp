#include "TreeModelGenerator.cpp"

ParallelModel *getModel(int n, int k) {
    auto generator = new TreeModelGenerator(n, n, k - 1, 2);
    auto model = generator->getModel();
    if (model == nullptr) {
        return nullptr;
    }
    auto lastStateId = generator->calculateLevelFirstStateId(k - 1) - 1;
    
    
    
    // Transitions
    
    model->states[lastStateId]->addTransition(1, lastStateId + 1);
    model->states[lastStateId]->addTransition(1, lastStateId + 2);
    
    
    
    // Winning states
    
    model->states[lastStateId + 2]->setAccept();
    
    
    
    return model;
}

bool getExpectedResult() {
    return false;
}
