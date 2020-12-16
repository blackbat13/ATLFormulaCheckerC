//
// Created by black on 16.12.2020.
//

#ifndef ATLFORMULACHECKER_SETTOOLS_H
#define ATLFORMULACHECKER_SETTOOLS_H

#include <set>
#include "../Models/asynchronous/LocalTransition.h"

class SetTools {
public:
    static std::set<LocalTransition> difference(std::set<LocalTransition> a, std::set<LocalTransition> b);
    static std::set<LocalTransition> setUnion(std::set<LocalTransition> a, std::set<LocalTransition> b);
};


#endif //ATLFORMULACHECKER_SETTOOLS_H
