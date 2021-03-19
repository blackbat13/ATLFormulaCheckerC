#include "SimpleModel.h"
#include <utility>

SimpleModel::SimpleModel(unsigned int noAgents) : noAgents(noAgents) {
    this->noStates = this->noTransitions = this->firstStateId = 0;
    this->epistemicClasses = std::vector<std::vector<std::set<unsigned int> > >(noAgents);
    this->epistemicClassMembership = std::vector<std::vector<unsigned int> >(noAgents);
    this->agentsActions = std::vector<std::set<std::string> >(noAgents);
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

void SimpleModel::addActions(unsigned short agentId, std::set<std::string> actions) {
    this->agentsActions[agentId] = std::move(actions);
}

void SimpleModel::addState(GlobalState state) {
    this->states.push_back(state);
}

std::set<unsigned int> SimpleModel::epistemicClassForState(int stateId, int agentId) {
    if(stateId >= this->epistemicClassMembership[agentId].size() || this->epistemicClassMembership[agentId][stateId] == -1) {
        auto result = std::set<unsigned int>();
        result.insert(stateId);
        return result;
    }

    return this->epistemicClasses[agentId][this->epistemicClassMembership[agentId][stateId]];
}

void SimpleModel::simulate(int agentId) {
    printf("----SIMULATION START-----\n");
    int current_state = 0;
    while(true) {
        printf("\n");
        printf("Current state id: %d\n", current_state);
//        this->simulatePrintCurrentState(current_state);
//        this->simulatePrintEpistemicStates(current_state, agentId);
        if(this->graph[current_state].empty()) {
            break;
        }

        this->simulatePrintTransitions(current_state);
        int choice;
        printf("Choose transition: ");
        scanf("%d", &choice);
        if(choice == -1) {
            break;
        }

        current_state = this->graph[current_state][choice].nextState;
    }

    printf("----SIMULATION END-----\n");
}

void SimpleModel::simulatePrintCurrentState(int currentState) {
    printf("Current state:\n");
    auto state =  (this->states[currentState]);
    state.print();
}

void SimpleModel::simulatePrintEpistemicStates(int currentState, int agentId) {
    printf("Epistemic states:\n");
    auto epist = this->epistemicClassForState(currentState, agentId);
    for(auto state : epist) {
        this->states[state].print();
    }
}

void SimpleModel::simulatePrintTransitions(int currentState) {
    printf("Transitions:\n");
    int i = 0;
    for(auto transition : this->graph[currentState]) {
        printf("%d: ", i);
        transition.print();
        printf("\n");
        i++;
    }
}

ParallelModel* SimpleModel::toParallelModel(bool imperfect) {
    auto parallelModel = new ParallelModel(this->noStates);
    std::map<std::string, int> actionsId;

    for(int stateId = 0; stateId < this->noStates; stateId++) {
        std::vector<std::pair<int, int> > tran;
        for(auto transition : this->graph[stateId]){
            tran.emplace_back(atoi(transition.actions[agentId].c_str()), transition.nextState);
        }

        std::sort(tran.begin(), tran.end());

        for(auto tr : tran) {
            parallelModel->states[stateId]->addTransition(tr.first, tr.second);
//            cout << stateId << " " << tr.first << " " << tr.second << endl;
        }
    }

    if(imperfect) {
        for (int stateId = 0; stateId < this->noStates; stateId++) {
            for (auto epistemicState : this->epistemicClassForState(stateId, this->agentId)) {
                if (epistemicState == stateId || epistemicState < stateId) {
                    continue;
                }


                parallelModel->unify(stateId, epistemicState);
            }
        }
    }

    for(auto stateId : this->winningStates) {
        parallelModel->states[stateId]->setAccept();
    }

    return parallelModel;
}

std::set<unsigned int> SimpleModel::verifyApproximationImperfect() {
    auto model = this->toAtlImperfect();
    std::set<unsigned int> winningSet;
    for(auto stateId: this->winningStates) {
        winningSet.insert(stateId);
    }

    auto result = model.minimumFormulaOneAgentMultipleStatesDisjoint(this->agentId, winningSet);
    return result;
}

SimpleModel::SimpleModel(std::string filename) {
    std::ifstream file;
    file.open (filename, std::fstream::in);
    file >> this->noStates;
    file >> this->noAgents;
    file >> this->noTransitions;

    this->graph.resize(this->noStates);
    this->actionGraph.resize(this->noStates);

    this->firstStateId = 0;
    this->epistemicClasses = std::vector<std::vector<std::set<unsigned int> > >(noAgents);
    this->epistemicClassMembership = std::vector<std::vector<unsigned int> >(noAgents);
    this->agentsActions = std::vector<std::set<std::string> >(noAgents);

    for(int i = 0; i < this->noTransitions; i++) {
        int from, to;
        file >> from >> to;
        std::vector<std::string> actions(this->noAgents);
        for(int j = 0; j < this->noAgents; j++) {
            file >> actions[j];
            this->agentsActions[j].insert(actions[j]);
        }
        this->graph[from].push_back(Transition(to, actions));
    }

    for(int i = 0; i < this->noAgents; i++) {
        int epistemicCount;
        file >> epistemicCount;
        for(int j = 0; j < epistemicCount; j++) {
            int count;
            std::set<unsigned int> epistemicClass;
            file >> count;
            for(int k = 0; k < count; k++) {
                unsigned int stateId;
                file >> stateId;
                epistemicClass.insert(stateId);
            }
            this->addEpistemicClass(i, epistemicClass);
        }
    }

    int winningStatesCount;
    file >> winningStatesCount;
    this->winningStates = std::vector<unsigned int>(winningStatesCount);
    for(int i = 0; i < winningStatesCount; i++) {
        file >> this->winningStates[i];
    }

    file >> this->agentId;

    file.close();
}

int SimpleModel::getAgentId() {
    return this->agentId;
}

void SimpleModel::printStats() {
    std::cout << "Number of states: " << this->noStates << std::endl;
    std::cout << "Number of agents: " << this->noAgents << std::endl;
    std::cout << "Number of transitions: " << this->noTransitions << std::endl;
    std::cout << "Winning states: " << std::endl;
    for(auto stateId : this->winningStates) {
        std::cout << stateId << " ";
    }

    std::cout << std::endl;
    std::cout << "Agent: " << this->agentId << std::endl;
    std::cout << std::endl;
}
