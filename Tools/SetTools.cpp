//
// Created by black on 16.12.2020.
//

#include "SetTools.h"

std::set<LocalTransitionTup> SetTools::difference(std::set<LocalTransitionTup> a, std::set<LocalTransitionTup> b) {
    std::set<LocalTransitionTup> result;
    for(auto el : a) {
        if(b.find(el) == b.end()) {
            result.insert(el);
        }
    }

    return result;
}

std::set<LocalTransitionTup> SetTools::setUnion(const std::set<LocalTransitionTup>& a, std::set<LocalTransitionTup> b) {
    std::set<LocalTransitionTup> result;
    for(auto el : a) {
        result.insert(el);
    }

    for(auto el : b) {
        result.insert(el);
    }

    return result;
}
