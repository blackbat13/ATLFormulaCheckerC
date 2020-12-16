//
// Created by black on 16.12.2020.
//

#include "SetTools.h"

std::set<LocalTransition> SetTools::difference(std::set<LocalTransition> a, std::set<LocalTransition> b) {
    std::set<LocalTransition> result;
    for(auto el : a) {
        if(b.find(el) == b.end()) {
            result.insert(el);
        }
    }

    return result;
}

std::set<LocalTransition> SetTools::setUnion(std::set<LocalTransition> a, std::set<LocalTransition> b) {
    std::set<LocalTransition> result;
    for(auto el : a) {
        result.insert(el);
    }

    for(auto el : b) {
        result.insert(el);
    }

    return result;
}
