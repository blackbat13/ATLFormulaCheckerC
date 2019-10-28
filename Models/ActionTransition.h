//
// Created by black on 28.10.2019.
//

#ifndef ATLFORMULACHECKER_ACTIONTRANSITION_H
#define ATLFORMULACHECKER_ACTIONTRANSITION_H

#include <vector>
#include <string>

class ActionTransition {
public:
    ActionTransition();

    std::vector<std::string> actions;

    std::vector<unsigned int> nextStates;

    ActionTransition(std::vector<std::string> actions, std::vector<unsigned int> nextStates);

    ActionTransition(std::vector<std::string> actions, unsigned int nextState);

    bool operator<(const ActionTransition &rhs) const;

    bool operator>(const ActionTransition &rhs) const;

    bool operator<=(const ActionTransition &rhs) const;

    bool operator>=(const ActionTransition &rhs) const;
};


#endif //ATLFORMULACHECKER_ACTIONTRANSITION_H
