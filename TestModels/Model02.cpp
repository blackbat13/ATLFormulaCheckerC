#include "../Models/ParallelModel.hpp"

ParallelModel *getModel() {
    auto model = new ParallelModel(7);
    
    
    
    // Transitions
    
    model->states[0]->addTransition(1, 1);
    model->states[0]->addTransition(2, 2);
    
    model->states[1]->addTransition(1, 3);
    model->states[1]->addTransition(1, 4);
    model->states[1]->addTransition(2, 0);
    
    model->states[2]->addTransition(1, 5);
    model->states[2]->addTransition(2, 6);
    
    model->states[3]->addTransition(1, 3);
    
    model->states[4]->addTransition(1, 4);
    
    model->states[5]->addTransition(1, 5);
    
    model->states[6]->addTransition(1, 6);
    
    
    
    // Epistemic classes
    
    model->unify(1, 2);
    
    
    
    // Winning states
    
    model->states[3]->setAccept();
    model->states[6]->setAccept();
    
    
    
    return model;
}

bool getExpectedResult() {
    return true;
}
