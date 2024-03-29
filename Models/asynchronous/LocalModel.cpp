//
// Created by black on 14.12.2020.
//

#include "LocalModel.h"

LocalModel::LocalModel(int agentId, std::string agentName, std::map<std::string, int> states,
                       std::vector<std::vector<LocalTransition*> > transitions,
                       std::vector<std::vector<std::string> >  protocol,
                       std::set<std::string> actions) {
    this->agentId = agentId;
    this->agentName = agentName;
    this->states = states;
    this->transitions = transitions;
    this->actions = actions;
    this->protocol = protocol;
    this->props = std::vector<std::string>();
    this->computeProps();
    this->applyProtocol();
}

void LocalModel::computeProps() {
    auto propsSet = std::set<std::string>();
    for(auto ls : this->transitions) {
        for(auto tr: ls) {
            for(auto el : tr->getProps()) {
                propsSet.insert(el.first);
            }
        }
    }


    this->props = std::vector<std::string>();

    for(auto el : propsSet) {
        this->props.push_back(el);
    }

    std::sort(this->props.begin(), this->props.end());
}

std::vector<LocalTransition*> LocalModel::transitionsFromState(int stateId) {
    return this->transitions[stateId];
}

std::vector<LocalTransition*> LocalModel::privateTransitionsFromState(int stateId) {
    std::vector<LocalTransition*> result;
    for(auto tr : this->transitions[stateId]) {
        if(!tr->isShared()) {
            result.push_back(tr);
        }
    }

    return result;
}

std::vector<LocalTransition*> LocalModel::sharedTransitionsFromState(int stateId) {
    std::vector<LocalTransition*> result;
    for(auto tr : this->transitions[stateId]) {
        if(tr->isShared()) {
            result.push_back(tr);
        }
    }

    return result;
}

bool LocalModel::hasAction(std::string action) {
    return this->actions.find(action) != this->actions.end();
}

int LocalModel::getStateId(std::string stateName) {
    return this->states[stateName];
}

std::vector<LocalTransition*> LocalModel::getTransitions() {
    std::vector<LocalTransition*> result;
    for(const auto& transitionList : this->transitions) {
        for(const auto& transition: transitionList) {
            result.push_back(transition);
        }
    }

    std::sort(result.begin(), result.end());
    return result;
}

void LocalModel::print() {
    printf("%s\n", this->agentName.c_str());
    for(int i = 0; i < this->transitions.size(); i++) {
        for(int j = 0; j < this->transitions[i].size(); j++) {
            auto transition = this->transitions[i][j];
            printf("%d %d: ", i, j);
            transition->print();
        }
    }
}

void LocalModel::applyProtocol() {
    int p = 0;
    for(auto lst : this->protocol) {
        p++;
        for(auto trName : lst) {
            for(int i = 0; i < this->transitions.size(); i++) {
                for (int j = 0; j < this->transitions[i].size(); j++) {
                    if(this->transitions[i][j]->getAction() == trName) {
                        this->transitions[i][j]->setProtName(this->agentName + "_prot_" + to_string(p));
                    }
                }
            }
        }
    }
}
