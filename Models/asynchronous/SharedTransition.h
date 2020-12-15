//
// Created by black on 14.12.2020.
//

#ifndef ATLFORMULACHECKER_SHAREDTRANSITION_H
#define ATLFORMULACHECKER_SHAREDTRANSITION_H

#include "LocalTransition.h"
#include <vector>

class SharedTransition: public LocalTransition {
private:
    std::vector<LocalTransition> transitionList;
public:
    SharedTransition(LocalTransition localTransition);

    void addTransition(LocalTransition localTransition);
};


#endif //ATLFORMULACHECKER_SHAREDTRANSITION_H
