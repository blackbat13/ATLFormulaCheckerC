//
// Created by black on 02.10.2019.
//

#ifndef ATLFORMULACHECKER_MODELGENERATOR_H
#define ATLFORMULACHECKER_MODELGENERATOR_H

#include "../Logics/AtlModel.h"
#include "State.h"
#include "SimpleModel.h"
#include <map>

class ModelGenerator {
protected:
    unsigned short noAgents;
    unsigned int stateId;
    std::map<State, unsigned int> statesMap;
    std::vector<std::map<State, std::set<unsigned int> > > epistemicStatesMap;
    SimpleModel model = SimpleModel(0);

    explicit ModelGenerator(int noAgents);

    unsigned int addState(State state);

    unsigned int getStateId(const State &state);

    virtual State getEpistemicState(State state, unsigned short agentId) = 0;

    void addToEpistemicMap(const State &epistemicState, unsigned int newStateId, unsigned short agentId);

    virtual std::vector<std::string> getPropsForState(State state) = 0;

    void prepareEpistemicRelation();

    virtual void generateInitialStates() = 0;

    virtual void generateModel() = 0;

    virtual std::vector<std::vector<std::string> > getActions() = 0;

    virtual std::vector<std::string> getPropsList() = 0;

    virtual std::set<unsigned int> getWinningStates(std::string prop) = 0;
public:
    void generate();
};


#endif //ATLFORMULACHECKER_MODELGENERATOR_H
