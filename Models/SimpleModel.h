#ifndef ATLFORMULACHECKER_SIMPLEMODEL_H
#define ATLFORMULACHECKER_SIMPLEMODEL_H

#include <vector>
#include <set>
#include <fstream>
#include "Transition.h"
#include "State.h"
#include "ActionTransition.h"
#include "../Logics/AtlModel.h"
#include "./asynchronous/GlobalState.h"
#include "ParallelModel.hpp"

class SimpleModel {
protected:
    std::vector<std::vector<Transition> > graph;
    std::vector<std::vector<ActionTransition> > actionGraph;
    unsigned int noStates;
    unsigned short noAgents;
    unsigned int noTransitions;
    std::vector<std::vector<std::set<unsigned int> > > epistemicClasses;
    std::vector<std::vector<unsigned int> > epistemicClassMembership;
    std::vector<GlobalState> states;
    unsigned int firstStateId;
    std::vector<std::set<std::string> > agentsActions;
    std::vector<std::vector<unsigned int> > winningStates;
    int agentId;
public:
    explicit SimpleModel(unsigned int noAgents);
    explicit SimpleModel(std::string filename);
    void addTransition(unsigned int fromStateId, unsigned int toStateId, std::vector<std::string> actions);
    void resizeToState(unsigned int stateId);
//    void addEpistemicRelation(unsigned int stateId1, unsigned int stateId2, unsigned short agentId);
    void addEpistemicClass(unsigned short agentId, const std::set<unsigned int>& epistemicClass);
    void addActions(unsigned short agentId, std::set<std::string> actions);
    AtlModel toAtlImperfect();
    AtlModel toAtlPerfect();
    void addState(GlobalState state);
    std::set<unsigned int> epistemicClassForState(int stateId, int agentId);
    void simulate(int agentId);
    void simulatePrintCurrentState(int currentState);
    void simulatePrintEpistemicStates(int currentState, int agentId);
    void simulatePrintTransitions(int currentState);
    ParallelModel* toParallelModel(bool imperfect);
    void setParallelAccept(ParallelModel *parallelModel, int formulaIndex);
    int getAgentId();
    void printStats();
    std::set<unsigned int> verifyApproximationImperfect(int formulaIndex = 0);
    std::set<unsigned int> verifyApproximationPerfect(int formulaIndex = 0);
    int getFormulasCount();
};


#endif //ATLFORMULACHECKER_SIMPLEMODEL_H
