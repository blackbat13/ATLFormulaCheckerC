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

    std::vector<std::vector<std::vector<int> > > dependent;
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
    SharedTransition* createSharedTransitions(LocalTransition* transition, int agentId);
    std::vector<LocalTransition*> availableTransitionsInStateForAgent(GlobalState state, int agentId);
    std::vector<std::vector<LocalTransition*> > enabledTransitionsInState(GlobalState state);
    std::vector<LocalTransition*> enabledTransitionsForAgent(int agentId, std::vector<std::vector<LocalTransition*> >  allTransitions);
    bool checkIfSharedTransitionIsEnabled(LocalTransition* transition, int agentId, std::vector<std::vector<LocalTransition*> > allTransitions);
    std::set<LocalTransition*> enabledTransitionsInStateSingleItemSet(GlobalState state);
    GlobalState newStateAfterPrivateTransition(GlobalState state, LocalTransition* transition);
    std::pair<GlobalState, std::vector<int> > newStateAfterSharedTransition(GlobalState state, std::vector<std::pair<int, LocalTransition*> > actualTransition);
    GlobalState newStateAfterSharedTransitionList(GlobalState state, std::vector<LocalTransition*> transitions);
    void computeNextForState(GlobalState state, int currentStateId);
    void computeNextForStateForAgent(GlobalState state, int currentStateId, int agentId, std::vector<std::string> &visited, std::vector<std::vector<LocalTransition*> >  allTransitions);
    GlobalState copyPropsToState(GlobalState state, LocalTransition transition);
    int stateFind(GlobalState state);
    void computeDependentTransitions();
    bool isInG(GlobalState state);
    int findStateOnStack1(GlobalState state);
    bool addToStack(GlobalState state);
    void popFromStack();
    void iterPor();
    std::set<LocalTransition*> ample(GlobalState state);
    bool checkForCycle(GlobalState state, std::set<LocalTransition*> x);
    bool checkForK(GlobalState state, std::set<LocalTransition*> x);
    std::set<LocalTransition*> enabledForX(std::set<LocalTransition*> x);
    std::set<LocalTransition*> dependentForX(std::set<LocalTransition*> x, std::set<LocalTransition*> dis, std::set<LocalTransition*> u);
    GlobalState successor(GlobalState state, LocalTransition* transition);
    int addState(GlobalState &state);
    EpistemicState getEpistemicState(GlobalState state, int agentId);
    void addToEpistemicDictionary(EpistemicState state, int newStateId, int agentId);
    void addTransition(int stateFrom, int stateTo, std::string action, std::vector<int> agents);
    std::vector<std::string> createListOfActions(std::string action, std::vector<int> agents);
    void compute();
    std::vector<std::set<std::string> > getActions();

public:
    std::vector<GlobalState> states;
    GlobalModel(std::vector<LocalModel> localModels, std::vector<std::string> reduction,
                std::vector<std::string> persistent, std::vector<std::string> coalition, std::vector<std::string> goal);

    void generate(bool reduction);
    int agentNameToId(std::string agentName);
    std::vector<int> agentNameCoalitionToIds(std::vector<std::string> agentNames);
    void print();
    void setCoalition(std::vector<std::string> coalition);
    std::set<unsigned int> getWinningStates(int formulaNo);
    std::pair<std::set<unsigned int>, double> verifyApproximation(bool perfectInf, int formulaNo);
    std::pair<bool, double> verifyParallel(int formulaNo);
    int getStatesCount();
    int getTransitionsCount();

    const SimpleModel &getModel() const;
};


#endif //ATLFORMULACHECKER_GLOBALMODEL_H
