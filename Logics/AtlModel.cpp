#include "AtlModel.h"

AtlModel::AtlModel(unsigned short numberOfAgents, unsigned int numberOfStates) : numberOfAgents(numberOfAgents),
                                                                                 numberOfStates(numberOfStates) {
    this->imperfectInformation = std::vector<std::vector<std::set<unsigned int> > >(numberOfAgents);
    this->agentsActions = std::vector<std::vector<std::string> >(numberOfAgents);

    this->transitions = std::vector<std::set<Transition> >(1000);
    this->preStates = std::vector<std::set<unsigned int> >(1000);
    this->numberOfTransitions = 0;
    this->initialStatesCount = 0;
}

void AtlModel::addAction(unsigned short agentNumber, const std::string &action) {
    this->agentsActions[agentNumber].push_back(action);
}

void AtlModel::addTransition(unsigned int from, unsigned int to, const std::vector<std::string> &actions) {
    Transition transition = Transition(to, actions);
    while (from >= this->transitions.size()) {
        this->transitions.resize(this->transitions.size() + 1000);
    }

    while (to >= this->preStates.size()) {
        this->preStates.resize(this->preStates.size() + 1000);
    }

    this->transitions[from].insert(transition);
    this->preStates[to].insert(from);
}

void AtlModel::addEpistemicClass(unsigned short agentNumber, const std::set<unsigned int> &epistemicClass) {
    if (this->epistemicClassMembership.empty()) {
        this->epistemicClassMembership = std::vector<std::vector<unsigned int> >(this->numberOfAgents,
                                                                                 std::vector<unsigned int>(
                                                                                         this->numberOfStates,
                                                                                         -1));
        this->epistemicClassDisjoint = std::vector<DisjointUnion>(this->numberOfAgents,
                                                                  DisjointUnion(this->numberOfStates));
        this->canGoThere = std::vector<std::vector<std::map<std::string, std::set<unsigned int> > > >(
                this->numberOfAgents,
                std::vector<std::map<std::string, std::set<unsigned int> > >(this->numberOfStates));
    }

    this->imperfectInformation[agentNumber].push_back(epistemicClass);
    int epistemicClassNumber = (int) this->imperfectInformation[agentNumber].size() - 1;
    int firstState = *epistemicClass.begin();
    for (auto state: epistemicClass) {
        this->epistemicClassMembership[agentNumber][state] = epistemicClassNumber;
        this->epistemicClassDisjoint[agentNumber].unionn(firstState, state);
    }

    this->findWhereCanGo(epistemicClass, epistemicClassNumber, agentNumber);
}

void AtlModel::finishEpistemicClasses(unsigned short agentNumber) {
    this->epistemicClassMembership = std::vector<std::vector<unsigned int> >(this->numberOfAgents,
                                                                             std::vector<unsigned int>(
                                                                                     this->numberOfStates,
                                                                                     -1));
    this->epistemicClassDisjoint = std::vector<DisjointUnion>(this->numberOfAgents,
                                                              DisjointUnion(this->numberOfStates));
    this->canGoThere = std::vector<std::vector<std::map<std::string, std::set<unsigned int> > > >(
            this->numberOfAgents,
            std::vector<std::map<std::string, std::set<unsigned int> > >(this->numberOfStates));

    for (unsigned int i = 0; i < this->imperfectInformation[agentNumber].size(); ++i) {
        int firstState = *(this->imperfectInformation[agentNumber][i].begin());
        for (auto state: this->imperfectInformation[agentNumber][i]) {
            this->epistemicClassMembership[agentNumber][state] = i;
            this->epistemicClassDisjoint[agentNumber].unionn(firstState, state);
        }

        this->findWhereCanGo(this->imperfectInformation[agentNumber][i], i, agentNumber);
    }
}

void AtlModel::findWhereCanGo(const std::set<unsigned int> &epistemicClass, unsigned int epistemicClassNumber,
                              unsigned short agentNumber) {
    for (const auto &action: this->agentsActions[agentNumber]) {
        std::set<unsigned int> canGoTemp;
        for (auto state: epistemicClass) {
            std::set<unsigned int> canGoStateTemp;
            for (auto transition: this->transitions[state]) {
                if (transition.actions[agentNumber] == action) {
                    canGoStateTemp.insert(transition.nextState);
                }
            }

            canGoTemp.insert(canGoStateTemp.begin(), canGoStateTemp.end());
            if (canGoStateTemp.empty()) {
                canGoTemp.clear();
                break;
            }
        }

        this->canGoThere[agentNumber][epistemicClassNumber][action] = canGoTemp;
    }
}

bool AtlModel::isReachableByAgentDisjoint(const std::string &action, unsigned int fromState, unsigned short agentNumber,
                                          unsigned int firstWinning,
                                          DisjointUnion winningStates) {
    bool actionOk = false;
    for (auto transition: this->transitions[fromState]) {
        if (transition.actions[agentNumber] == action) {
            actionOk = true;
            if (!winningStates.isSame(firstWinning, transition.nextState)) {
                return false;
            }
        }
    }

    return actionOk;
}

bool
AtlModel::isReachableByAgent(const std::string &action, unsigned int fromState, const bool *isWinningState,
                             unsigned short agentNumber) {
    bool actionOk = false;
    for (auto transition: this->transitions[fromState]) {
        if (transition.actions[agentNumber] == action) {
            actionOk = true;
            if (!isWinningState[transition.nextState]) {
                return false;
            }
        }
    }

    return actionOk;
}

bool
AtlModel::isReachableByAgentInSet(const std::string &action, unsigned int fromState,
                                  std::set<unsigned int> winningStates,
                                  unsigned short agentNumber) {
    bool actionOk = false;
    for (auto transition: this->transitions[fromState]) {
        if (transition.actions[agentNumber] == action) {
            actionOk = true;
            if (winningStates.find(transition.nextState) == winningStates.end()) {
                return false;
            }
        }
    }

    return actionOk;
}

std::pair<std::set<unsigned int>, bool>
AtlModel::basicFormulaOneAgentMultipleStatesDisjoint(unsigned short agentNumber,
                                                     const std::set<unsigned int> &currentStates,
                                                     unsigned int firstWinning,
                                                     DisjointUnion &winningStatesDisjoint,
                                                     std::vector<std::map<std::string, std::set<unsigned int> > > &customCanGoThere) {
    std::set<unsigned int> resultStates;
    std::vector<std::string> actions = this->agentsActions[agentNumber];
    std::set<unsigned int> preImage;
    bool modified = false;
    for (auto winningState: currentStates) {
        for (auto preState: this->preStates[winningState]) {
            preImage.insert(this->epistemicClassMembership[agentNumber][preState]);
        }
    }

    firstWinning = winningStatesDisjoint.find(firstWinning);
    for (auto stateEpistemicClassNumber: preImage) {
        int state = *this->imperfectInformation[agentNumber][stateEpistemicClassNumber].begin();
        state = winningStatesDisjoint.find(state);
        if (state == firstWinning) {
            continue;
        }

        auto sameStates = this->imperfectInformation[agentNumber][stateEpistemicClassNumber];
        for (const auto &action: actions) {
            auto statesCanGo = customCanGoThere[stateEpistemicClassNumber][action];
            if (statesCanGo.empty()) {
                continue;
            }

            bool isOk = true;
            std::set<unsigned int> newStatesCanGo;
            for (auto stateCan: statesCanGo) {
                auto newStateCan = winningStatesDisjoint.find(stateCan);
                if (firstWinning != newStateCan) {
                    isOk = false;
                }

                newStatesCanGo.insert(newStateCan);
            }

            customCanGoThere[stateEpistemicClassNumber][action] = newStatesCanGo;

            if (isOk) {
                resultStates.insert(sameStates.begin(), sameStates.end());

                winningStatesDisjoint.unionn(firstWinning, state);
                firstWinning = winningStatesDisjoint.find(firstWinning);
                modified = true;
                break;
            }
        }
    }

    return std::make_pair(resultStates, modified);
}

std::set<unsigned int>
AtlModel::basicFormulaOneAgentMultipleStatesPerfectInformation(unsigned short agentNumber,
                                                               const std::set<unsigned int> &currentStates,
                                                               std::vector<bool> &isWinningState) {
    std::set<unsigned int> resultStates;
    std::vector<std::string> actions = this->agentsActions[agentNumber];
    std::set<unsigned int> preImage;
    for (auto winningState: currentStates) {
        preImage.insert(this->preStates[winningState].begin(), this->preStates[winningState].end());
    }

    for (auto state: preImage) {

        std::set<std::string> allActions;
        std::set<std::string> badActions;
        std::set<std::string> actionsDifference;
        for (auto transition: this->transitions[state]) {
            allActions.insert(transition.actions[agentNumber]);
            if (!isWinningState[transition.nextState]) {
                badActions.insert(transition.actions[agentNumber]);
            }
        }

        std::set_difference(allActions.begin(), allActions.end(), badActions.begin(), badActions.end(),
                            std::inserter(actionsDifference, actionsDifference.begin()));
        if (!actionsDifference.empty()) {
            resultStates.insert(state);
        }
    }

    for (auto stateNumber: resultStates) {
        isWinningState[stateNumber] = true;
    }

    return resultStates;
}

std::set<unsigned int>
AtlModel::minimumFormulaOneAgentMultipleStatesDisjoint(unsigned short agentNumber,
                                                       const std::set<unsigned int> &winningStates) {
    if (winningStates.empty()) {
        return std::set<unsigned int>();
    }

    std::set<unsigned int> resultStates;
    resultStates.insert(winningStates.begin(), winningStates.end());
    int numberOfIterations = 0;
    auto currentStates = winningStates;
    DisjointUnion winningStatesDisjoint = this->epistemicClassDisjoint[agentNumber];
    auto firstWinning = winningStatesDisjoint.find(*winningStates.begin());
    std::set<int> epistemicClassNumbers;
    for (auto stateNumber: winningStates) {
        int epistemicClassNumber = this->epistemicClassMembership[agentNumber][stateNumber];
        epistemicClassNumbers.insert(epistemicClassNumber);
    }

    for (auto epistemicClassNumber: epistemicClassNumbers) {
        auto epistemicStates = this->imperfectInformation[agentNumber][epistemicClassNumber];
        bool isOk = true;
        for (auto epistemicState: epistemicStates) {
            if (winningStates.find(epistemicState) == winningStates.end()) {
                isOk = false;
                break;
            }
        }

        if (isOk) {
            winningStatesDisjoint.unionn(firstWinning, *epistemicStates.begin());
        }
    }

    epistemicClassNumbers.clear();

    auto customCanGoThere = this->canGoThere[agentNumber];
    while (true) {
        auto formulaResult = this->basicFormulaOneAgentMultipleStatesDisjoint(agentNumber,
                                                                              currentStates,
                                                                              firstWinning,
                                                                              winningStatesDisjoint,
                                                                              customCanGoThere);
        currentStates = formulaResult.first;
        bool modified = formulaResult.second;
        if (!modified) {
            break;
        }

        resultStates.insert(currentStates.begin(), currentStates.end());
        ++numberOfIterations;
    }

    printf("Number of iterations: %d\n", numberOfIterations);
    return resultStates;
}

std::set<unsigned int>
AtlModel::minimumFormulaOneAgentMultipleStatesPerfectInformation(unsigned short agentNumber,
                                                                 const std::set<unsigned int> &winningStates) {
    std::set<unsigned int> resultStates;
    resultStates.insert(winningStates.begin(), winningStates.end());
    unsigned long resultStatesLength = resultStates.size();
    int numberOfIterations = 0;
    auto currentStates = winningStates;
    std::vector<bool> isWinningState = std::vector<bool>(this->numberOfStates, false);

    for (auto stateNumber: winningStates) {
        isWinningState[stateNumber] = true;
    }

    while (true) {

        currentStates = this->basicFormulaOneAgentMultipleStatesPerfectInformation(agentNumber, currentStates,
                                                                                   isWinningState);
        resultStates.insert(currentStates.begin(), currentStates.end());
        if (resultStates.size() == resultStatesLength) {
            break;
        }

        resultStatesLength = resultStates.size();
        ++numberOfIterations;
    }

    printf("Number of iterations: %d\n", numberOfIterations);
    return resultStates;
}

AtlModel::AtlModel() = default;

void AtlModel::setNumberOfStates(unsigned int numberOfStates) {
    this->numberOfStates = numberOfStates;
    this->transitions.resize(numberOfStates);
    this->preStates.resize(numberOfStates);
}

void AtlModel::saveToFile(std::ofstream &file) {
    file << this->numberOfStates << std::endl;
    this->numberOfTransitions = 0;
    for (auto &transition : this->transitions) {
        this->numberOfTransitions += transition.size();
    }

    file << this->numberOfTransitions << std::endl;
    file << this->numberOfAgents << std::endl;
    file << this->initialStatesCount << std::endl;
    for (unsigned int i = 0; i < this->transitions.size(); ++i) {
        for (auto t : this->transitions[i]) {
            file << i << " " << t.nextState << " " << t.actions[0] << std::endl;
        }
    }

    for (int i = 0; i < this->numberOfAgents; ++i) {
        file << this->agentsActions[i].size() << std::endl;
        for (const auto &action : this->agentsActions[i]) {
            file << action << std::endl;
        }
    }

    file << this->winningStates.size() << std::endl;
    for (auto state : this->winningStates) {
        file << state << std::endl;
    }

    for (int i = 0; i < this->numberOfAgents; ++i) {
        file << this->imperfectInformation[i].size() << std::endl;
        for (int j : this->epistemicClassMembership[i]) {
            file << j << std::endl;
        }
    }
}

void AtlModel::loadFromFile(std::ifstream &file, bool imperfect) {
    unsigned int numberOfStates, numberOfTransitions;
    unsigned short numberOfAgents;
    int numberOfEpistemicClasses, numberOFWinningStates;
    file >> numberOfStates >> numberOfTransitions >> numberOfAgents >> this->initialStatesCount;
    this->numberOfTransitions = numberOfTransitions;
    this->numberOfStates = numberOfStates;
    this->numberOfAgents = numberOfAgents;

    this->agentsActions = std::vector<std::vector<std::string> >(numberOfAgents);
    this->transitions = std::vector<std::set<Transition> >(numberOfStates);
    this->preStates = std::vector<std::set<unsigned int> >(numberOfStates);
    this->agentsActions = std::vector<std::vector<std::string> >(numberOfAgents);
    this->winningStates = std::set<unsigned int>();

    for (unsigned int i = 0; i < numberOfTransitions; ++i) {
        int state, nextState;
        std::vector<std::string> actions(1);
        file >> state >> nextState >> actions[0];
        this->addTransition(state, nextState, actions);
    }

    for (unsigned short i = 0; i < numberOfAgents; ++i) {
        int numberOfActions;
        file >> numberOfActions;
        this->agentsActions[i].resize((unsigned long) numberOfActions);
        for (int j = 0; j < numberOfActions; ++j) {
            file >> this->agentsActions[i][j];
        }
    }

    file >> numberOFWinningStates;
    for (unsigned int i = 0; i < numberOFWinningStates; ++i) {
        unsigned int winningState;
        file >> winningState;
        this->winningStates.insert(winningState);
    }

    if (!imperfect) {
        return;
    }

    this->imperfectInformation = std::vector<std::vector<std::set<unsigned int> > >(numberOfAgents);
    this->epistemicClassMembership = std::vector<std::vector<unsigned int> >(numberOfAgents,
                                                                             std::vector<unsigned int>(numberOfStates));

    for (int i = 0; i < numberOfAgents; ++i) {
        file >> numberOfEpistemicClasses;
        this->imperfectInformation[i] = std::vector<std::set<unsigned int> >(numberOfEpistemicClasses);
        for (unsigned int j = 0; j < numberOfStates; ++j) {
            file >> this->epistemicClassMembership[i][j];
            if (this->epistemicClassMembership[i][j] != -1) {
                this->imperfectInformation[i][this->epistemicClassMembership[i][j]].insert(j);
            }
        }

        this->finishEpistemicClasses(i);
    }

    this->transitions.clear();
}

void AtlModel::clearTransitions() {
    this->transitions.clear();
}

const std::set<unsigned int> &AtlModel::getWinningStates() const {
    return winningStates;
}

void AtlModel::setWinningStates(const std::set<unsigned int> &winningStates) {
    AtlModel::winningStates = winningStates;
}

unsigned long AtlModel::getInitialStatesCount() const {
    return initialStatesCount;
}

void AtlModel::setInitialStatesCount(unsigned int initialStatesCount) {
    AtlModel::initialStatesCount = initialStatesCount;
}

std::set<Transition> AtlModel::getTransitions(unsigned int stateNumber) {
    return this->transitions[stateNumber];
}

ParallelModel AtlModel::toParallelModel(int agentId) {
//    auto parallelModel = ParallelModel();
//    map<string, int> actionToInt;
//    int actionId = 1;
//    for(auto stateId = 0; stateId < this->numberOfStates; stateId++) {
//        auto pState = ParallelState();
//        pState.state_id = stateId;
//        pState.equivalence_vector.push_back(this->epistemicClassMembership[agentId][stateId]);
//        for(auto transition : this->transitions[stateId]) {
//            auto actionVector = action_vector();
//            string action = transition.actions[agentId];
//            if(actionToInt[action] == 0) {
//                actionToInt[action] = actionId;
//                actionId++;
//            }
//            actionVector.push_back(actionToInt[action]);
//            pState.action_list.push_back(actionVector);
//            pState.state_list.push_back(transition.nextState);
//            for(auto i = pState.action_list.size() - 1; i >= 1; i--) {
//                if(pState.action_list[i][0] < pState.action_list[i-1][0]) {
//                    swap(pState.action_list[i], pState.action_list[i-1]);
//                    swap(pState.state_list[i], pState.state_list[i-1]);
//                } else {
//                    break;
//                }
//            }
//        }
//
//        parallelModel.states.push_back(pState);
//    }
//
//    for(auto stateId : this->winningStates) {
//        parallelModel.states[stateId].winning = true;
//    }
//
//    auto maxEpClass = max_element(this->epistemicClassMembership[agentId].begin(), this->epistemicClassMembership[agentId].end()).operator*();
//    parallelModel.abstraction_classes = vector<equivalence_class>(maxEpClass + 1);
//    for(auto stateId = 0; stateId < this->numberOfStates; stateId++) {
//        auto epClassId = this->epistemicClassMembership[agentId][stateId];
//        parallelModel.abstraction_classes[epClassId].class_id = epClassId;
//        parallelModel.abstraction_classes[epClassId].members.push_back(stateId);
//    }
//    return parallelModel;
    return ParallelModel();
}



