#include "AtlModel.h"

AtlModel::AtlModel(unsigned short numberOfAgents, unsigned int numberOfStates) : numberOfAgents(numberOfAgents),
                                                                                 numberOfStates(numberOfStates) {
    this->imperfectInformation = std::vector<std::vector<std::set<unsigned int> > >(numberOfAgents);
    this->agentsActions = std::vector<std::vector<std::string> >(numberOfAgents);

    this->transitions = std::vector<std::set<Transition> >(numberOfStates + 1);
    this->preStates = std::vector<std::set<unsigned int> >(numberOfStates + 1);
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
    this->imperfectInformation[agentNumber].push_back(epistemicClass);
}

void AtlModel::finishEpistemicClasses(unsigned short agentNumber) {
    if(epistemicClassMembership.empty()) {
        this->epistemicClassMembership = std::vector<std::vector<unsigned int> >(this->numberOfAgents,
                                                                                 std::vector<unsigned int>(
                                                                                         this->numberOfStates,
                                                                                         -1));
        this->epistemicClassDisjoint = std::vector<DisjointUnion>(this->numberOfAgents,
                                                                  DisjointUnion(this->numberOfStates));
        this->canGoThere = std::vector<std::vector<std::map<std::string, std::set<unsigned int> > > >(
                this->numberOfAgents);
    }

    for (unsigned int i = 0; i < this->imperfectInformation[agentNumber].size(); ++i) {
        int firstState = *(this->imperfectInformation[agentNumber][i].begin());
        for (auto state: this->imperfectInformation[agentNumber][i]) {
            this->epistemicClassMembership[agentNumber][state] = i;
            this->epistemicClassDisjoint[agentNumber].unionn(firstState, state);
        }
    }

    for(int stateId = 0; stateId < this->numberOfStates; stateId++) {
        if(this->epistemicClassMembership[agentNumber][stateId] != -1) {
            continue;
        }

        std::set<unsigned int> epistemicClass;
        epistemicClass.insert(stateId);
        this->imperfectInformation[agentNumber].push_back(epistemicClass);
        this->epistemicClassMembership[agentNumber][stateId] = (int) this->imperfectInformation[agentNumber].size() - 1;
    }

    this->canGoThere[agentNumber] = std::vector<std::map<std::string, std::set<unsigned int> > >(this->imperfectInformation[agentNumber].size());

    for (unsigned int i = 0; i < this->imperfectInformation[agentNumber].size(); ++i) {
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
            bool inEpistemic = true;
            std::set<unsigned int> newStatesCanGo;
            for (auto stateCan: statesCanGo) {
                auto newStateCan = winningStatesDisjoint.find(stateCan);
                if (firstWinning != newStateCan) {
                    isOk = false;
                    if(newStateCan != state) { // if not same epistemic class
                        inEpistemic=false;
                    }
                }

                newStatesCanGo.insert(newStateCan);
            }

            customCanGoThere[stateEpistemicClassNumber][action] = newStatesCanGo;

            if(!isOk && inEpistemic) {  // bladzenie
                auto origStatesCanGo = this->canGoThere[agentNumber][stateEpistemicClassNumber][action];
                std::map<unsigned int, bool> goodState;
                int counter = 0;
                bool change = true;
                while(change) {
                    change=false;
                    for(auto stateCan : origStatesCanGo) {
                        if(goodState[stateCan]) {
                            continue;
                        }
                        else if(winningStatesDisjoint.find(stateCan) == firstWinning) {
                            goodState[stateCan] = true;
                            counter++;
                            change = true;
                        } else {
                            bool good = true;
                            for(auto tran : this->transitions[stateCan]) {
                                if(tran.actions[agentNumber] == action) {
                                    if(!goodState[tran.nextState]) {
                                        good = false;
                                        break;
                                    }
                                }
                            }

                            if(good) {
                                counter++;
                                change=true;
                                goodState[stateCan] = true;
                            }
                        }
                    }
                }

                if(counter == origStatesCanGo.size()) {
                    isOk=true;
                }
            }

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

    return resultStates;
}

AtlModel::AtlModel() = default;

void AtlModel::setNumberOfStates(unsigned int numberOfStates) {
    this->numberOfStates = numberOfStates;
    this->transitions.resize(numberOfStates);
    this->preStates.resize(numberOfStates);
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



