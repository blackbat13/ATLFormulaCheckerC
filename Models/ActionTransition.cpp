//
// Created by black on 28.10.2019.
//

#include "ActionTransition.h"

#include <utility>

bool ActionTransition::operator<(const ActionTransition &rhs) const {
    if (actions < rhs.actions)
        return true;
    if (rhs.actions < actions)
        return false;
    return nextStates < rhs.nextStates;
}

bool ActionTransition::operator>(const ActionTransition &rhs) const {
    return rhs < *this;
}

bool ActionTransition::operator<=(const ActionTransition &rhs) const {
    return !(rhs < *this);
}

bool ActionTransition::operator>=(const ActionTransition &rhs) const {
    return !(*this < rhs);
}

ActionTransition::ActionTransition() = default;

ActionTransition::ActionTransition(std::vector<std::string> actions, std::vector<unsigned int> nextStates) {
    this->actions = std::move(actions);
    this->nextStates = std::move(nextStates);
}

ActionTransition::ActionTransition(std::vector<std::string> actions, unsigned int nextState) {
    this->actions = std::move(actions);
    this->nextStates = std::vector<unsigned int>();
    this->nextStates.push_back(nextState);
}