#include "../Models/ParallelModel.hpp"

ParallelModel *getModel() {
    auto model = new ParallelModel(6);
    
    
    
    // Transitions
    
    model->states[0]->addTransition(1, 1);
    
    model->states[1]->addTransition(1, 2);
    model->states[1]->addTransition(2, 4);
    
    model->states[2]->addTransition(1, 3);
    model->states[2]->addTransition(2, 4);
    
    model->states[3]->addTransition(1, 5);
    model->states[3]->addTransition(2, 4);
    
    
    
    // Epistemic classes
    
    model->unify(1, 2);
    model->unify(2, 3);
    
    
    
    // Winning states
    
    model->states[5]->setAccept();
    
    
    
    return model;
}

bool getExpectedResult() {
    return true;
}
