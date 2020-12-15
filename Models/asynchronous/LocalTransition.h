//
// Created by black on 14.12.2020.
//

#ifndef ATLFORMULACHECKER_LOCALTRANSITION_H
#define ATLFORMULACHECKER_LOCALTRANSITION_H

#include <string>
#include <map>
#include <vector>
#include "GlobalState.h"

class LocalTransition {
    int id;
    int agentId;
    std::string action;
    bool shared;
    std::string stateFrom;
    std::string stateTo;
    std::map<std::string,std::string> props;
    std::vector<std::string> cond;
    int i;
    int j;

public:
    LocalTransition(std::string stateFrom, std::string stateTo, std::string action, bool shared, std::vector<std::string>  cond, std::map<std::string, std::string> dict);

    bool checkConditions(GlobalState state);

    void print();
};


#endif //ATLFORMULACHECKER_LOCALTRANSITION_H
