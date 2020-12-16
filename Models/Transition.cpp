//
// Created by black on 02.10.2019.
//

#include "Transition.h"
#include <utility>

Transition::Transition(unsigned int nextState, std::vector<std::string> actions) : nextState(nextState),
                                                                                   actions(std::move(actions)) {}

Transition::Transition() = default;

bool Transition::operator<(const Transition &rhs) const {
    if (nextState < rhs.nextState)
        return true;
    if (rhs.nextState < nextState)
        return false;
    return actions < rhs.actions;
}

bool Transition::operator>(const Transition &rhs) const {
    return rhs < *this;
}

bool Transition::operator<=(const Transition &rhs) const {
    return !(rhs < *this);
}

bool Transition::operator>=(const Transition &rhs) const {
    return !(*this < rhs);
}

void Transition::print() {
    printf("Next state: %d, actions: ", this->nextState);
    for(auto act : this->actions) {
        printf("%s, ", act.c_str());
    }
}
