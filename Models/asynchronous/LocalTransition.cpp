//
// Created by black on 14.12.2020.
//

#include "LocalTransition.h"

bool LocalTransition::operator<(const LocalTransition &rhs) const {
    return id < rhs.id;
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

bool LocalTransition::checkConditions(GlobalState state) {
    for(auto cond : this->conditions) {
        if((cond.condSym == "==" && ((state.props.find(cond.condVar) == state.props.end()) || (state.props[cond.condVar] != cond.condVal))) || (cond.condSym == "!=" && (state.props.find(cond.condVar) != state.props.end()) && state.props[cond.condVar] == cond.condVal)) {
            return false;
        }
    }

    return true;
}

bool LocalTransition::operator==(const LocalTransition &rhs) const {
    return agentId == rhs.agentId &&
           action == rhs.action;
}

bool LocalTransition::operator!=(const LocalTransition &rhs) const {
    return !(rhs == *this);
}

void LocalTransition::print() {
    printf("%s: ", this->action.c_str());
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
