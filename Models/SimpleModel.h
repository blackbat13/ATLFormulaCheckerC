#ifndef ATLFORMULACHECKER_SIMPLEMODEL_H
#define ATLFORMULACHECKER_SIMPLEMODEL_H

#include <vector>
#include <set>
#include "Transition.h"
#include "State.h"
#include "../Logics/AtlModel.h"

class SimpleModel {
protected:
    std::vector<std::vector<Transition> > graph;
    unsigned int noStates;
    unsigned short noAgents;
    unsigned int noTransitions;
    std::vector<std::vector<std::set<unsigned int> > > epistemicClasses;
    std::vector<std::vector<unsigned int> > epistemicClassMembership;
    std::vector<State> states;
    unsigned int firstStateId;
    std::vector<std::vector<std::string> > agentsActions;
public:
    explicit SimpleModel(unsigned int noAgents);
    void addTransition(unsigned int fromStateId, unsigned int toStateId, std::vector<std::string> actions);
    void resizeToState(unsigned int stateId);
//    void addEpistemicRelation(unsigned int stateId1, unsigned int stateId2, unsigned short agentId);
    void addEpistemicClass(unsigned short agentId, const std::set<unsigned int>& epistemicClass);
    void addActions(unsigned short agentId, std::vector<std::string> actions);
    AtlModel toAtlImperfect();
};


#endif //ATLFORMULACHECKER_SIMPLEMODEL_H