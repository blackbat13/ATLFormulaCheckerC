//
// Created by black on 14.12.2020.
//

#ifndef ATLFORMULACHECKER_LOCALMODEL_H
#define ATLFORMULACHECKER_LOCALMODEL_H

#include "../SimpleModel.h"
#include "LocalTransition.h"
#include "SharedTransition.h"
#include <string>
#include <map>
#include <vector>
#include <set>
#include <algorithm>

class LocalModel {
public:
    int agentId;
    std::string agentName;
    std::map<std::string, int> states;
    std::vector<std::vector<LocalTransition*> > transitions;
    std::set<std::string> actions;
    std::vector<std::vector<std::string> > protocol;
    std::vector<std::string> props;

    LocalModel(int agentId, std::string agentName, std::map<std::string, int> states, std::vector<std::vector<LocalTransition*> > transitions, std::vector<std::vector<std::string> > protocol, std::set<std::string> actions);

    std::vector<LocalTransition*> transitionsFromState(int stateId);

    std::vector<LocalTransition*> privateTransitionsFromState(int stateId);

    std::vector<LocalTransition*> sharedTransitionsFromState(int stateId);

    bool hasAction(std::string action);

    int getStateId(std::string stateName);

    std::vector<LocalTransition*> getTransitions();

    void print();

    void applyProtocol();
protected:
    void computeProps();
};


#endif //ATLFORMULACHECKER_LOCALMODEL_H
