//
// Created by black on 14.12.2020.
//

#include "GlobalState.h"

GlobalState::GlobalState(std::vector<int> localStates, std::map<std::string, std::string> props,
                         int id) {
    this->id = id;
    this->localStates = localStates;
    this->props = props;
}

GlobalState::GlobalState(int agentCount) {
    this->id = 0;
    this->localStates = std::vector<int>(agentCount, 0);
    this->props = std::map<std::string, std::string>();
}

GlobalState GlobalState::copyState(GlobalState state, std::vector<std::string> persistent) {
    std::map<std::string, std::string> newProps;
    for(auto prop : state.props) {
        if(std::find(persistent.begin(), persistent.end(), prop.first) != persistent.end()) {
            newProps[prop.first] = prop.second;
        }
    }

    return GlobalState(state.localStates, newProps, -1);
}

void GlobalState::setLocalState(int index, int value) {
    this->localStates[index] = value;
}

void GlobalState::setProp(std::string key, std::string value) {
    this->props[key] = value;
}

void GlobalState::removeProp(std::string key) {
    this->props.erase(key);
}

bool GlobalState::equal(GlobalState state) {
    return this->localStates == state.localStates && this->props == state.props;
}

std::string GlobalState::toString() {
    std::string result;
    for(auto el : this->localStates) {
        result += std::to_string(el) + ", ";
    }

    result += " ";

    for(auto el : this->props) {
        result += el.first + ":" + el.second + ", ";
    }

    return result;
}

void GlobalState::print() {
    printf("ID: %d, Local States: ", this->id);
    for(auto el : this->localStates) {
        printf("%d, ", el);
    }
    printf(" Props: ");
    for(auto el : this->props) {
        printf("%s:%s, ", el.first.c_str(), el.second.c_str());
    }

    printf("\n");
}

bool GlobalState::operator==(const GlobalState &rhs) const {
    return localStates == rhs.localStates &&
           props == rhs.props;
}

bool GlobalState::operator!=(const GlobalState &rhs) const {
    return !(rhs == *this);
}

bool GlobalState::operator<(const GlobalState &rhs) const {
    if (localStates < rhs.localStates)
        return true;
    if (rhs.localStates < localStates)
        return false;
    return props < rhs.props;
}

bool GlobalState::operator>(const GlobalState &rhs) const {
    return rhs < *this;
}

bool GlobalState::operator<=(const GlobalState &rhs) const {
    return !(rhs < *this);
}

bool GlobalState::operator>=(const GlobalState &rhs) const {
    return !(*this < rhs);
}
