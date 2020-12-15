//
// Created by black on 14.12.2020.
//

#ifndef ATLFORMULACHECKER_LOCALMODEL_H
#define ATLFORMULACHECKER_LOCALMODEL_H

#include "../SimpleModel.h"
#include "LocalTransition.h"
#include <string>
#include <map>
#include <vector>
#include <set>

class LocalModel {
private:
    int agentId;
    std::string agentName;
    std::map<std::string, int> states;
    std::vector<std::vector<LocalTransition> > transitions;
    std::set<std::string> actions;
    std::map<std::string, std::vector<std::vector<LocalTransition> > > protocols;
    std::vector<std::string> props;
    SimpleModel model;
public:
    LocalModel(int agentId, std::string agentName, std::map<std::string, int> states, std::vector<std::vector<LocalTransition> > transitions, std::map<std::string, std::vector<std::vector<std::string> > > protocols, std::set<std::string> actions);

    void generate();

    std::vector<LocalTransition> transitionsFromState(int stateId);

    std::vector<LocalTransition> privateTransitionsFromState(int stateId);

    std::vector<LocalTransition> sharedTransitionsFromState(int stateId);

    bool hasAction(std::string action);

    int getStateId(std::string stateName);

    std::vector<LocalTransition> getTransitions();

    void print();
protected:
    void computeProps();
};


#endif //ATLFORMULACHECKER_LOCALMODEL_H
