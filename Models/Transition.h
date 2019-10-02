//
// Created by black on 02.10.2019.
//

#ifndef ATLFORMULACHECKER_TRANSITION_H
#define ATLFORMULACHECKER_TRANSITION_H

#include <vector>
#include <string>

class Transition {
public:
    Transition();

    /// Identifier of the state.
    int nextState{};

    /// Vector of agents actions, where action_i corresponds to agent_i.
    std::vector<std::string> actions;

    Transition(int nextState, std::vector<std::string> actions);

    bool operator<(const Transition &rhs) const;

    bool operator>(const Transition &rhs) const;

    bool operator<=(const Transition &rhs) const;

    bool operator>=(const Transition &rhs) const;
};


#endif //ATLFORMULACHECKER_TRANSITION_H
