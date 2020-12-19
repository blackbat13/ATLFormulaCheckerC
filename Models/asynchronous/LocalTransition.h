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

    Condition(std::string condVar, std::string condVal, std::string condSym) : condVar(condVar), condVal(condVal),
                                                                               condSym(condSym) {}
};

class LocalTransition {
protected:
    int id;
    int agentId;
    std::string action;
    bool shared;
    std::string stateFrom;
    std::string stateTo;
    std::map<std::string, std::string> props;
    std::vector<Condition> conditions;
    int i;
    int j;

public:
    LocalTransition(std::string stateFrom, std::string stateTo, std::string action, bool shared,
                    std::vector<Condition> cond, std::map<std::string, std::string> props);

    bool checkConditions(GlobalState state) const;

    void print();


    int getId() const;

    int getAgentId() const;

    const std::string &getAction() const;

    virtual bool isShared() const;

    const std::string &getStateFrom() const;

    const std::string &getStateTo() const;

    const std::map<std::string, std::string> &getProps() const;

    const std::vector<Condition> &getConditions() const;

    int getI() const;

    int getJ() const;

    void setId(int id);

    void setAgentId(int agentId);

    void setAction(const std::string &action);

    void setI(int i);

    void setJ(int j);

    bool operator<(const LocalTransition &rhs) const;

    bool operator>(const LocalTransition &rhs) const;

    bool operator<=(const LocalTransition &rhs) const;

    bool operator>=(const LocalTransition &rhs) const;

    bool operator==(const LocalTransition &rhs) const;

    bool operator!=(const LocalTransition &rhs) const;
};


#endif //ATLFORMULACHECKER_LOCALTRANSITION_H
