#include "SimpleModel.h"
#include <utility>

SimpleModel::SimpleModel(unsigned int noAgents) : noAgents(noAgents) {
    this->noStates = this->noTransitions = this->firstStateId = 0;
    this->epistemicClasses = std::vector<std::vector<std::set<unsigned int> > >(noAgents);
    this->epistemicClassMembership = std::vector<std::vector<unsigned int> >(noAgents);
    this->agentsActions = std::vector<std::vector<std::string> >(noAgents);
}

void SimpleModel::addTransition(unsigned int fromStateId, unsigned int toStateId, std::vector<std::string> actions) {
    this->resizeToState(std::max(fromStateId, toStateId));
    bool exist = false;
    for(auto & actionTransition : this->actionGraph[fromStateId]) {
        if(actionTransition.actions == actions) {
            actionTransition.nextStates.push_back(toStateId);
            exist = true;
            break;
        }
    }
    if(!exist) {
        this->actionGraph[fromStateId].push_back(ActionTransition(actions, toStateId));
    }
    this->graph[fromStateId].push_back(Transition(toStateId, std::move(actions)));
    this->noTransitions++;
}

void SimpleModel::resizeToState(unsigned int stateId) {
    if (stateId < this->graph.size()) {
        return;
    }

    this->graph.resize(stateId + 1);
    this->actionGraph.resize(stateId + 1);
    this->noStates = std::max(this->noStates, stateId + 1);
}

void SimpleModel::addEpistemicClass(unsigned short agentId, const std::set<unsigned int> &epistemicClass) {
    this->epistemicClasses[agentId].push_back(epistemicClass);
    if (this->epistemicClassMembership[agentId].size() <= this->noStates) {
        this->epistemicClassMembership[agentId].resize(this->noStates + 1);
    }

    unsigned int epistemicClassId = this->epistemicClasses[agentId].size() - 1;

    for (auto stateId : epistemicClass) {
        this->epistemicClassMembership[agentId][stateId] = epistemicClassId;
    }
}

AtlModel SimpleModel::toAtlImperfect() {
    AtlModel atlModel = AtlModel(this->noAgents, this->noStates);
    for (unsigned short agentId = 0; agentId < this->noAgents; agentId++) {
        for (const auto &action : this->agentsActions[agentId]) {
            atlModel.addAction(agentId, action);
        }
    }

    for (unsigned int stateId = 0; stateId < this->noStates; stateId++) {
        for (const auto &transition : this->graph[stateId]) {
            atlModel.addTransition(stateId, transition.nextState, transition.actions);
        }
    }

    for (unsigned short agentId = 0; agentId < this->noAgents; agentId++) {
        for (const auto &epistemicClass : this->epistemicClasses[agentId]) {
            atlModel.addEpistemicClass(agentId, epistemicClass);
        }

        atlModel.finishEpistemicClasses(agentId);
    }

    return atlModel;
}

void SimpleModel::addActions(unsigned short agentId, std::vector<std::string> actions) {
    this->agentsActions[agentId] = std::move(actions);
}

void SimpleModel::addState(const State& state) {
    this->states.push_back(state);
}

std::set<unsigned int> SimpleModel::epistemicClassForState(int stateId, int agentId) {
    if(this->epistemicClassMembership[agentId][stateId] == -1) {
        auto result = std::set<unsigned int>();
        result.insert(stateId);
        return result;
    }

    return this->epistemicClasses[agentId][this->epistemicClassMembership[agentId][stateId]];
}
