//
// Created by black on 14.12.2020.
//

#ifndef ATLFORMULACHECKER_GLOBALSTATE_H
#define ATLFORMULACHECKER_GLOBALSTATE_H

#include "../State.h"
#include <map>
#include <vector>
#include <algorithm>

class GlobalState: public State {
public:
    int id;
    std::vector<int> localStates;
    std::map<std::string, std::string> props;

    GlobalState(std::vector<int> localStates, std::map<std::string, std::string> props, int id = -1);

    explicit GlobalState(int agentCount);

    static GlobalState copyState(GlobalState state, std::vector<std::string> persistent);

    void setLocalState(int index, int value);

    void setProp(std::string key, std::string value);

    void removeProp(std::string key);

    bool hasProp(std::string key);

    std::string getProp(std::string key);

    bool equal(GlobalState state);

    std::string toString();

    void print() override;

    bool operator==(const GlobalState &rhs) const;

    bool operator!=(const GlobalState &rhs) const;

    bool operator<(const GlobalState &rhs) const;

    bool operator>(const GlobalState &rhs) const;

    bool operator<=(const GlobalState &rhs) const;

    bool operator>=(const GlobalState &rhs) const;
};


#endif //ATLFORMULACHECKER_GLOBALSTATE_H
