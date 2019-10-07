//
// Created by black on 02.10.2019.
//

#include "ModelGenerator.h"

ModelGenerator::ModelGenerator(int noAgents) {
    this->noAgents = noAgents;
    this->model = SimpleModel(noAgents);
    this->stateId = 0;
    this->statesMap = std::map<State, unsigned int>();
    this->epistemicStatesMap = std::vector<std::map<State, std::set<unsigned int> > >(noAgents);
}

unsigned int ModelGenerator::addState(State state) {
    unsigned int newStateId = this->getStateId(state);
    state.setProps(this->getPropsForState(state));
    for (unsigned short agentId = 0; agentId < this->noAgents; agentId++) {
        auto epistemicState = this->getEpistemicState(state, agentId);
        this->addToEpistemicMap(epistemicState, newStateId, agentId);
    }

    return newStateId;
}

unsigned int ModelGenerator::getStateId(const State &state) {
    unsigned int newStateId;
    if (this->statesMap[state] == 0) {
        this->statesMap[state] = this->stateId + 1;
        newStateId = this->stateId;
        this->model.addState(state);
        this->stateId++;
    } else {
        newStateId = this->statesMap[state] - 1;
    }

    return newStateId;
}

void ModelGenerator::addToEpistemicMap(const State& epistemicState, unsigned int newStateId, unsigned short agentId) {
    this->epistemicStatesMap[agentId][epistemicState].insert(newStateId);
}

void ModelGenerator::prepareEpistemicRelation() {
    for(unsigned short agentId = 0; agentId < this->noAgents; agentId++) {
        for(const auto& item : this->epistemicStatesMap[agentId]) {
            this->model.addEpistemicClass(agentId, item.second);
        }
    }
}

void ModelGenerator::generate() {
    this->generateInitialStates();
    this->generateModel();
    this->prepareEpistemicRelation();
}

