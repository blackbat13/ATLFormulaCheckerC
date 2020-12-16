//
// Created by black on 14.12.2020.
//

#ifndef ATLFORMULACHECKER_LOCALTRANSITION_H
#define ATLFORMULACHECKER_LOCALTRANSITION_H

#include <string>
#include <map>
#include <vector>
#include "GlobalState.h"

struct Condition {
    std::string condVar;
    std::string condVal;
    std::string condSym;
};

class LocalTransition {
public:
    int id;
    int agentId;
    std::string action;
    bool shared;
    std::string stateFrom;
    std::string stateTo;
    std::map<std::string,std::string> props;
    std::vector<Condition> conditions;
    int i;
    int j;


    LocalTransition(std::string stateFrom, std::string stateTo, std::string action, bool shared, std::vector<Condition> cond, std::map<std::string, std::string> props);

    bool checkConditions(GlobalState state);

    void print();

    bool operator<(const LocalTransition &rhs) const;

    bool operator>(const LocalTransition &rhs) const;

    bool operator<=(const LocalTransition &rhs) const;

    bool operator>=(const LocalTransition &rhs) const;

    bool operator==(const LocalTransition &rhs) const;

    bool operator!=(const LocalTransition &rhs) const;
};


#endif //ATLFORMULACHECKER_LOCALTRANSITION_H
