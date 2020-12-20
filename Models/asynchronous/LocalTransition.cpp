//
// Created by black on 14.12.2020.
//

#include "LocalTransition.h"

LocalTransition::LocalTransition(std::string stateFrom, std::string stateTo, std::string action, bool shared,
                                 std::vector<Condition> cond, std::map<std::string, std::string> props) {
    this->id = -1;
    this->agentId = -1;
    this->action = action;
    this->shared = shared;
    this->stateFrom = stateFrom;
    this->stateTo = stateTo;
    this->props = props;
    this->conditions = cond;
    this->i = -1;
    this->j = -1;
}

bool LocalTransition::checkConditions(GlobalState state) const {
    for(auto cond : this->conditions) {
        if((cond.condSym == "==" && ((state.props.find(cond.condVar) == state.props.end()) || (state.props[cond.condVar] != cond.condVal))) || (cond.condSym == "!=" && (state.props.find(cond.condVar) != state.props.end()) && state.props[cond.condVar] == cond.condVal)) {
            return false;
        }
    }

    return true;
}

void LocalTransition::print() {
    printf("%s: ", this->action.c_str());
    printf("(Shared: %d) ", this->shared);
    printf("%s -> %s", this->stateFrom.c_str(), this->stateTo.c_str());
    printf("[");
    for(auto el : this->props) {
        printf("%s:%s, ", el.first.c_str(), el.second.c_str());
    }

    printf("]; conditions: ");
    for(auto el : this->conditions) {
        printf("%s %s %s, ", el.condVar.c_str(), el.condSym.c_str(), el.condVal.c_str());
    }

    printf("\n");
}

int LocalTransition::getId() const {
    return id;
}

int LocalTransition::getAgentId() const {
    return agentId;
}

const std::string &LocalTransition::getAction() const {
    return action;
}

bool LocalTransition::isShared() const {
    return shared;
}

const std::string &LocalTransition::getStateFrom() const {
    return stateFrom;
}

const std::string &LocalTransition::getStateTo() const {
    return stateTo;
}

const std::map<std::string, std::string> &LocalTransition::getProps() const {
    return props;
}

const std::vector<Condition> &LocalTransition::getConditions() const {
    return conditions;
}

int LocalTransition::getI() const {
    return i;
}

int LocalTransition::getJ() const {
    return j;
}

void LocalTransition::setId(int id) {
    LocalTransition::id = id;
}

void LocalTransition::setAgentId(int agentId) {
    LocalTransition::agentId = agentId;
}

void LocalTransition::setAction(const std::string &action) {
    LocalTransition::action = action;
}

void LocalTransition::setI(int i) {
    LocalTransition::i = i;
}

void LocalTransition::setJ(int j) {
    LocalTransition::j = j;
}

bool LocalTransition::operator<(const LocalTransition &rhs) const {
    if (agentId < rhs.agentId)
        return true;
    if (rhs.agentId < agentId)
        return false;
    if (i < rhs.i)
        return true;
    if (rhs.i < i)
        return false;
    return j < rhs.j;
}

bool LocalTransition::operator>(const LocalTransition &rhs) const {
    return rhs < *this;
}

bool LocalTransition::operator<=(const LocalTransition &rhs) const {
    return !(rhs < *this);
}

bool LocalTransition::operator>=(const LocalTransition &rhs) const {
    return !(*this < rhs);
}

bool LocalTransition::operator==(const LocalTransition &rhs) const {
    return agentId == rhs.agentId &&
           i == rhs.i &&
           j == rhs.j;
}

bool LocalTransition::operator!=(const LocalTransition &rhs) const {
    return !(rhs == *this);
}

LocalTransitionTup LocalTransition::toTup() {
    return {this->agentId, this->i, this->j};
}

bool LocalTransition::hasProp(std::string key) {
    return this->props.find(key) != this->props.end();
}

bool LocalTransitionTup::operator<(const LocalTransitionTup &rhs) const {
    if (agentId < rhs.agentId)
        return true;
    if (rhs.agentId < agentId)
        return false;
    if (i < rhs.i)
        return true;
    if (rhs.i < i)
        return false;
    return j < rhs.j;
}

bool LocalTransitionTup::operator>(const LocalTransitionTup &rhs) const {
    return rhs < *this;
}

bool LocalTransitionTup::operator<=(const LocalTransitionTup &rhs) const {
    return !(rhs < *this);
}

bool LocalTransitionTup::operator>=(const LocalTransitionTup &rhs) const {
    return !(*this < rhs);
}

bool LocalTransitionTup::operator==(const LocalTransitionTup &rhs) const {
    return agentId == rhs.agentId &&
           i == rhs.i &&
           j == rhs.j;
}

bool LocalTransitionTup::operator!=(const LocalTransitionTup &rhs) const {
    return !(rhs == *this);
}
