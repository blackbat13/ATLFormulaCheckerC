//
// Created by black on 14.12.2020.
//

#include "SharedTransition.h"

SharedTransition::SharedTransition(LocalTransition localTransition) : LocalTransition(localTransition.stateFrom,
                                                                                      localTransition.stateFrom,
                                                                                      localTransition.action, true,
                                                                                      localTransition.conditions,
                                                                                      localTransition.props) {
    this->id = localTransition.id;
    this->agentId = localTransition.agentId;
    this->transitionList = std::vector<LocalTransition>();
    this->transitionList.push_back(localTransition);
    this->i = localTransition.i;
    this->j = localTransition.j;
}

bool SharedTransition::operator==(const SharedTransition &rhs) const {
    return static_cast<const LocalTransition &>(*this) == static_cast<const LocalTransition &>(rhs) &&
           transitionList == rhs.transitionList;
}

bool SharedTransition::operator!=(const SharedTransition &rhs) const {
    return !(rhs == *this);
}

void SharedTransition::addTransition(LocalTransition localTransition) {
    this->transitionList.push_back(localTransition);
}

bool SharedTransition::operator<(const SharedTransition &rhs) const {
    if (static_cast<const LocalTransition &>(*this) < static_cast<const LocalTransition &>(rhs))
        return true;
    if (static_cast<const LocalTransition &>(rhs) < static_cast<const LocalTransition &>(*this))
        return false;
    return agentId < rhs.agentId;
}

bool SharedTransition::operator>(const SharedTransition &rhs) const {
    return rhs < *this;
}

bool SharedTransition::operator<=(const SharedTransition &rhs) const {
    return !(rhs < *this);
}

bool SharedTransition::operator>=(const SharedTransition &rhs) const {
    return !(*this < rhs);
}
