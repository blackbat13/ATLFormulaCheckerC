//
// Created by black on 14.12.2020.
//

#ifndef ATLFORMULACHECKER_GLOBALMODEL_H
#define ATLFORMULACHECKER_GLOBALMODEL_H

#include "LocalModel.h"
#include "LocalTransition.h"
#include "SharedTransition.h"
#include "../../Tools/SetTools.h"
#include <vector>
#include <string>
#include <set>
#include <map>
#include <stack>

struct EpistemicState {
    bool operator==(const EpistemicState &rhs) const;

    bool operator!=(const EpistemicState &rhs) const;

    bool operator<(const EpistemicState &rhs) const;

    bool operator>(const EpistemicState &rhs) const;

    bool operator<=(const EpistemicState &rhs) const;

    bool operator>=(const EpistemicState &rhs) const;

    std::vector<int> localState;
    bool init;
    std::map<std::string, std::string> props;

    void print();
};

class GlobalModel {
private:

    SimpleModel model;
    std::vector<LocalModel> localModels;
    std::vector<std::string> reduction;
    std::vector<std::string> persistent;
    std::vector<std::string> coalition;
    std::vector<std::string> goal;
    std::vector<std::vector<std::vector<std::vector<int> > > > dependent;

    int agentsCount;
    std::map<GlobalState, int> statesDict;
    std::vector<GlobalState> stack1;
    std::vector<int> stack2;
    std::vector<GlobalState> g;
    std::map<GlobalState, int> stack1Dict;
    int transitionsCount;
    std::vector<std::map<EpistemicState, std::set<unsigned int> > > epistemicStatesDictionaries;

    void prepareEpistemicRelation();
    void addIndexToTransitions();
    void computeSharedTransitions();
    void computeDependentTransitions();
    SharedTransition* createSharedTransitions(LocalTransition* transition, int agentId);
    std::vector<LocalTransition*> availableTransitionsInStateForAgent(GlobalState state, int agentId);
    std::vector<std::vector<LocalTransition*> > enabledTransitionsInState(const GlobalState& state);
    std::vector<LocalTransition*> enabledTransitionsForAgent(int agentId, std::vector<std::vector<LocalTransition*> >  allTransitions);
    bool checkIfSharedTransitionIsEnabled(LocalTransition* transition, int agentId, std::vector<std::vector<LocalTransition*> > allTransitions);
    std::set<LocalTransitionTup> enabledTransitionsInStateSingleItemSet(GlobalState state);
    GlobalState newStateAfterPrivateTransition(GlobalState state, LocalTransition* transition);
    std::pair<GlobalState, std::vector<int> > newStateAfterSharedTransition(GlobalState state, const std::vector<std::pair<int, LocalTransition*> >& actualTransition);
    GlobalState newStateAfterSharedTransitionList(GlobalState state, const std::vector<LocalTransition*>& transitions);
    void computeNextForState(const GlobalState& state, int currentStateId);
    void computeNextForStateForAgent(const GlobalState& state, int currentStateId, int agentId, std::vector<std::string> &visited, std::vector<std::vector<LocalTransition*> >  allTransitions);
    static GlobalState copyPropsToState(GlobalState state, const LocalTransition& transition);
    int stateFind(const GlobalState& state);
    bool isInG(const GlobalState& state);
    int findStateOnStack1(const GlobalState& state);
    bool addToStack(const GlobalState& state);
    void popFromStack();
    void iterPor();
    std::set<LocalTransitionTup> ample(const GlobalState& state);
    bool checkForVisible(GlobalState state, const std::set<LocalTransitionTup>& x);
    bool checkForCycle(const GlobalState& state, const std::set<LocalTransitionTup>& x);
    bool checkForK(GlobalState state, std::set<LocalTransition*> x);
    std::set<LocalTransitionTup> enabledForX(const std::set<LocalTransitionTup>& x);
    std::set<LocalTransitionTup> dependentForX(const std::set<LocalTransitionTup>& x, std::set<LocalTransitionTup> dis, std::set<LocalTransitionTup> u);
    GlobalState successor(const GlobalState& state, LocalTransition* transition);
    int addState(GlobalState &state);
    EpistemicState getEpistemicState(GlobalState state, int agentId);
    void addToEpistemicDictionary(const EpistemicState& state, int newStateId, int agentId);
    void addTransition(int stateFrom, int stateTo, const std::string& action, const std::vector<int>& agents);
    std::vector<std::string> createListOfActions(const std::string& action, const std::vector<int>& agents) const;
    void compute();
    std::vector<std::set<std::string> > getActions();

public:
    std::vector<GlobalState> states;
    GlobalModel(std::vector<LocalModel> localModels, std::vector<std::string> reduction,
                std::vector<std::string> persistent, std::vector<std::string> coalition, std::vector<std::string> goal);

    void generate(bool reduction);
    int agentNameToId(const std::string& agentName);
    std::vector<int> agentNameCoalitionToIds(const std::vector<std::string>& agentNames);
    void print();
    void setCoalition(std::vector<std::string> coalition);
    std::set<unsigned int> getWinningStates(int formulaNo);
    std::pair<std::set<unsigned int>, double> verifyApproximation(bool perfectInf, int formulaNo);
    std::pair<bool, double> verifyParallel(int formulaNo);
    int getStatesCount() const;
    int getTransitionsCount() const;

    const SimpleModel &getModel() const;
};


#endif //ATLFORMULACHECKER_GLOBALMODEL_H
