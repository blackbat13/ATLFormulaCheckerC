//
// Created by black on 14.12.2020.
//

#include "SharedTransition.h"

SharedTransition::SharedTransition(LocalTransition* localTransition) : LocalTransition(localTransition->getStateFrom(),
                                                                                      localTransition->getStateTo(),
                                                                                      localTransition->getAction(), true,
                                                                                      localTransition->getConditions(),
                                                                                      localTransition->getProps()) {
    this->id = localTransition->getI();
    this->agentId = localTransition->getAgentId();
    this->transitionList = std::vector<LocalTransition*>();
    this->transitionList.push_back(localTransition);
    this->i = localTransition->getI();
    this->j = localTransition->getJ();
    this->protName = localTransition->getProtName();
}

bool SharedTransition::operator==(const SharedTransition &rhs) const {
    return static_cast<const LocalTransition &>(*this) == static_cast<const LocalTransition &>(rhs) &&
           transitionList == rhs.transitionList;
}

bool SharedTransition::operator!=(const SharedTransition &rhs) const {
    return !(rhs == *this);
}

void SharedTransition::addTransition(LocalTransition* localTransition) {
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

bool SharedTransition::isShared() const {
    return true;
}
