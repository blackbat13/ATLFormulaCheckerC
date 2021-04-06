#include "../Models/ParallelModel.hpp"

ParallelModel *getModel() {
    auto model = new ParallelModel(12);
    
    
    
    // Transitions
    
    model->states[0]->addTransition(1, 1);
    model->states[0]->addTransition(1, 2);
    
    model->states[1]->addTransition(1, 3);
    model->states[1]->addTransition(2, 4);
    
    model->states[2]->addTransition(1, 5);
    model->states[2]->addTransition(2, 6);
    
    model->states[3]->addTransition(1, 7);
    model->states[3]->addTransition(1, 8);
    
    model->states[4]->addTransition(1, 9);
    model->states[4]->addTransition(2, 10);
    
    model->states[5]->addTransition(1, 4);
    model->states[5]->addTransition(2, 11);
    
    model->states[6]->addTransition(1, 2);
    model->states[6]->addTransition(2, 11);
    
    
    
    // Epistemic classes
    
    model->unify(1, 2);
    model->unify(5, 6);
    
    
    
    // Winning states
    
    model->states[5]->setAccept();
    model->states[7]->setAccept();
    model->states[9]->setAccept();
    model->states[11]->setAccept();
    
    
    
    return model;
}

bool getExpectedResult() {
    return true;
}
