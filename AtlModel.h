//
// Created by Damian Kurpiewski on 18.03.2017.
//

#ifndef ATLFORMULACHECKER_ATLMODEL_H
#define ATLFORMULACHECKER_ATLMODEL_H

#include <vector>
#include <iostream>
#include <set>
#include <map>
#include <cstdio>
#include <iterator>
#include "DisjointUnion.h"

class AtlModel {
    struct Transition {
        Transition();

        int nextState;
        std::vector<std::string> actions;

        Transition(int nextState, const std::vector<std::string> &actions);

        bool operator<(const Transition &rhs) const;

        bool operator>(const Transition &rhs) const;

        bool operator<=(const Transition &rhs) const;

        bool operator>=(const Transition &rhs) const;
    };

    int numberOfAgents;
    int numberOfStates;
    std::vector<std::set<Transition> > transitions;
    std::vector<std::set<Transition> > reverseTransitions;
    std::vector<std::set<int> > preStates;
    std::vector<std::vector<std::set<int> > > imperfectInformation;
    std::vector<std::vector<std::string> > agentsActions;
    // states
    std::vector<std::vector<int> > epistemicClassMembership;
    std::vector<DisjointUnion> epistemicClassDisjoint;
    std::vector<std::vector<std::map<std::string, std::set<int> > > > canGoThere;
public:
    AtlModel(int numberOfAgents, int numberOfStates);

    AtlModel();

    void addAction(int agentNumber, std::string action);
//    void enlargeTransitions(int size);
    void addTransition(int from, int to, std::vector<std::string> actions);
//    bool isSameState(int agentNumber, int a, int b);
    void addEpistemicClass(int agentNumber, std::set<int> epistemicClass);
    void findWhereCanGo(std::set<int> epistemicClass, int epistemicClassNumber, int agentNumber);
//    std::vector<int> basicFormula(int agentNumber, int winningState);
    bool isReachableByAgentDisjoint(std::string action, int fromState, int agentNumber, int firstWinning, DisjointUnion winningStates);
    bool isReachableByAgent(std::string action, int fromState, bool isWinningState[], int agentNumber);
    bool isReachableByAgentInSet(std::string action, int fromState, std::set<int> winningStates, int agentNumber);
    std::pair<std::set<int>, bool> basicFormulaOneAgentMultipleStatesDisjoint(int agentNumber, std::set<int> currentStates, int firstWinning, DisjointUnion& winningStatesDisjoint, std::vector<std::map<std::string, std::set<int> > >& customCanGoThere);
    std::set<int> basicFormulaOneAgentMultipleStatesPerfectInformation(int agentNumber, std::set<int> currentStates, bool isWinningState[]);
    std::set<int> minimumFormulaOneAgentMultipleStatesDisjoint(int agentNumber, std::set<int> winningStates);
    std::set<int> minimumFormulaOneAgentMultipleStatesPerfectInformation(int agentNumber, std::set<int> winningStates);
};


#endif //ATLFORMULACHECKER_ATLMODEL_H
