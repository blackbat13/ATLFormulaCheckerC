//
// Created by black on 14.12.2020.
//

#ifndef ATLFORMULACHECKER_GLOBALSTATE_H
#define ATLFORMULACHECKER_GLOBALSTATE_H

#include <map>
#include <vector>

class GlobalState {
private:
    int id;
    std::vector<int> localStates;
    std::map<std::string, std::string> props;
    std::vector<int> counters;
public:
    GlobalState(std::vector<int> localStates, std::map<std::string, std::string> props, std::vector<int> counters, int id = -1);

    GlobalState(int agentCount);

    static GlobalState copyState(GlobalState state, std::vector<std::string> persistent);

    void setLocalState(int index, int value);

    void setProp(std::string key, std::string value);

    void removeProp(std::string key);

    bool equal(GlobalState state);

    std::string toString();

    void print();
};


#endif //ATLFORMULACHECKER_GLOBALSTATE_H
