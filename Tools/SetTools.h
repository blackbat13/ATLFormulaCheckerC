//
// Created by black on 16.12.2020.
//

#ifndef ATLFORMULACHECKER_SETTOOLS_H
#define ATLFORMULACHECKER_SETTOOLS_H

#include <set>
#include "../Models/asynchronous/LocalTransition.h"

class SetTools {
public:
    static std::set<LocalTransitionTup> difference(std::set<LocalTransitionTup> a, std::set<LocalTransitionTup> b);
    static std::set<LocalTransitionTup> setUnion(const std::set<LocalTransitionTup>& a, std::set<LocalTransitionTup> b);
};


#endif //ATLFORMULACHECKER_SETTOOLS_H
