#include "AtlModel.h"

AtlModel::AtlModel(int numberOfAgents, int numberOfStates) : numberOfAgents(numberOfAgents),
                                                             numberOfStates(numberOfStates) {
    this->imperfectInformation = std::vector<std::vector<std::set<int> > >((unsigned long) numberOfAgents);
    this->agentsActions = std::vector<std::vector<std::string> >((unsigned long) numberOfAgents);

    this->transitions = std::vector<std::set<Transition> >((unsigned long) 1000);
    this->preStates = std::vector<std::set<int> >((unsigned long) 1000);
    this->numberOfTransitions = 0;
    this->initialStatesCount = 0;
}

void AtlModel::addAction(int agentNumber, const std::string &action) {
    this->agentsActions[agentNumber].push_back(action);
}

void AtlModel::addTransition(int from, int to, const std::vector<std::string> &actions) {
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

void AtlModel::addEpistemicClass(int agentNumber, const std::set<int> &epistemicClass) {
    if (this->epistemicClassMembership.empty()) {
        this->epistemicClassMembership = std::vector<std::vector<int> >((unsigned long) this->numberOfAgents,
                                                                        std::vector<int>(
                                                                                (unsigned long) this->numberOfStates,
                                                                                -1));
        this->epistemicClassDisjoint = std::vector<DisjointUnion>((unsigned long) this->numberOfAgents,
                                                                  DisjointUnion(this->numberOfStates));
        this->canGoThere = std::vector<std::vector<std::map<std::string, std::set<int> > > >(
                (unsigned long) this->numberOfAgents,
                std::vector<std::map<std::string, std::set<int> > >((unsigned long) this->numberOfStates));
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

void AtlModel::finishEpistemicClasses(int agentNumber) {
    this->epistemicClassMembership = std::vector<std::vector<int> >((unsigned long) this->numberOfAgents,
                                                                    std::vector<int>(
                                                                            (unsigned long) this->numberOfStates,
                                                                            -1));
    this->epistemicClassDisjoint = std::vector<DisjointUnion>((unsigned long) this->numberOfAgents,
                                                              DisjointUnion(this->numberOfStates));
    this->canGoThere = std::vector<std::vector<std::map<std::string, std::set<int> > > >(
            (unsigned long) this->numberOfAgents,
            std::vector<std::map<std::string, std::set<int> > >((unsigned long) this->numberOfStates));

    for (unsigned int i = 0; i < this->imperfectInformation[agentNumber].size(); ++i) {
        int firstState = *(this->imperfectInformation[agentNumber][i].begin());
        for (auto state: this->imperfectInformation[agentNumber][i]) {
            this->epistemicClassMembership[agentNumber][state] = i;
            this->epistemicClassDisjoint[agentNumber].unionn(firstState, state);
        }

        this->findWhereCanGo(this->imperfectInformation[agentNumber][i], i, agentNumber);
    }
}

void AtlModel::findWhereCanGo(const std::set<int> &epistemicClass, unsigned int epistemicClassNumber, int agentNumber) {
    for (const auto &action: this->agentsActions[agentNumber]) {
        std::set<int> canGoTemp;
        for (auto state: epistemicClass) {
            std::set<int> canGoStateTemp;
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

bool AtlModel::isReachableByAgentDisjoint(const std::string &action, int fromState, int agentNumber, int firstWinning,
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
AtlModel::isReachableByAgent(const std::string &action, int fromState, const bool *isWinningState, int agentNumber) {
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
AtlModel::isReachableByAgentInSet(const std::string &action, int fromState, std::set<int> winningStates,
                                  int agentNumber) {
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

std::pair<std::set<int>, bool>
AtlModel::basicFormulaOneAgentMultipleStatesDisjoint(int agentNumber, const std::set<int> &currentStates,
                                                     int firstWinning,
                                                     DisjointUnion &winningStatesDisjoint,
                                                     std::vector<std::map<std::string, std::set<int> > > &customCanGoThere) {
    std::set<int> resultStates;
    std::vector<std::string> actions = this->agentsActions[agentNumber];
    std::set<int> preImage;
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

        std::set<int> sameStates = this->imperfectInformation[agentNumber][stateEpistemicClassNumber];
        for (const auto &action: actions) {
            std::set<int> statesCanGo = customCanGoThere[stateEpistemicClassNumber][action];
            if (statesCanGo.empty()) {
                continue;
            }

            bool isOk = true;
            std::set<int> newStatesCanGo;
            for (auto stateCan: statesCanGo) {
                int newStateCan = winningStatesDisjoint.find(stateCan);
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

std::set<int>
AtlModel::basicFormulaOneAgentMultipleStatesPerfectInformation(int agentNumber, const std::set<int> &currentStates,
                                                               std::vector<bool> &isWinningState) {
    std::set<int> resultStates;
    std::vector<std::string> actions = this->agentsActions[agentNumber];
    std::set<int> preImage;
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

std::set<int>
AtlModel::minimumFormulaOneAgentMultipleStatesDisjoint(int agentNumber, const std::set<int> &winningStates) {
    if (winningStates.empty()) {
        return std::set<int>();
    }

    std::set<int> resultStates;
    resultStates.insert(winningStates.begin(), winningStates.end());
    int numberOfIterations = 0;
    std::set<int> currentStates = winningStates;
    DisjointUnion winningStatesDisjoint = this->epistemicClassDisjoint[agentNumber];
    int firstWinning = winningStatesDisjoint.find(*winningStates.begin());
    std::set<int> epistemicClassNumbers;
    for (auto stateNumber: winningStates) {
        int epistemicClassNumber = this->epistemicClassMembership[agentNumber][stateNumber];
        epistemicClassNumbers.insert(epistemicClassNumber);
    }

    for (auto epistemicClassNumber: epistemicClassNumbers) {
        std::set<int> epistemicStates = this->imperfectInformation[agentNumber][epistemicClassNumber];
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

    std::vector<std::map<std::string, std::set<int> > > customCanGoThere = this->canGoThere[agentNumber];
    while (true) {
        std::pair<std::set<int>, bool> formulaResult = this->basicFormulaOneAgentMultipleStatesDisjoint(agentNumber,
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

std::set<int>
AtlModel::minimumFormulaOneAgentMultipleStatesPerfectInformation(int agentNumber, const std::set<int> &winningStates) {
    std::set<int> resultStates;
    resultStates.insert(winningStates.begin(), winningStates.end());
    unsigned long resultStatesLength = resultStates.size();
    int numberOfIterations = 0;
    std::set<int> currentStates = winningStates;
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

void AtlModel::setNumberOfStates(int numberOfStates) {
    this->numberOfStates = numberOfStates;
    this->transitions.resize((unsigned long) numberOfStates);
    this->preStates.resize((unsigned long) numberOfStates);
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
    int numberOfStates, numberOfTransitions, numberOfAgents;
    int numberOfEpistemicClasses, numberOFWinningStates;
    file >> numberOfStates >> numberOfTransitions >> numberOfAgents >> this->initialStatesCount;
    this->numberOfTransitions = numberOfTransitions;
    this->numberOfStates = numberOfStates;
    this->numberOfAgents = numberOfAgents;

    this->agentsActions = std::vector<std::vector<std::string> >((unsigned long) numberOfAgents);
    this->transitions = std::vector<std::set<Transition> >((unsigned long) numberOfStates);
    this->preStates = std::vector<std::set<int> >((unsigned long) numberOfStates);
    this->agentsActions = std::vector<std::vector<std::string> >((unsigned long) numberOfAgents);
    this->winningStates = std::set<int>();

    for (int i = 0; i < numberOfTransitions; ++i) {
        int state, nextState;
        std::vector<std::string> actions(1);
        file >> state >> nextState >> actions[0];
        this->addTransition(state, nextState, actions);
    }

    for (int i = 0; i < numberOfAgents; ++i) {
        int numberOfActions;
        file >> numberOfActions;
        this->agentsActions[i].resize((unsigned long) numberOfActions);
        for (int j = 0; j < numberOfActions; ++j) {
            file >> this->agentsActions[i][j];
        }
    }

    file >> numberOFWinningStates;
    for (int i = 0; i < numberOFWinningStates; ++i) {
        int winningState;
        file >> winningState;
        this->winningStates.insert(winningState);
    }

    if (!imperfect) {
        return;
    }

    this->imperfectInformation = std::vector<std::vector<std::set<int> > >((unsigned long) numberOfAgents);
    this->epistemicClassMembership = std::vector<std::vector<int> >((unsigned long) numberOfAgents,
                                                                    std::vector<int>((unsigned long) numberOfStates));

    for (int i = 0; i < numberOfAgents; ++i) {
        file >> numberOfEpistemicClasses;
        this->imperfectInformation[i] = std::vector<std::set<int> >((unsigned long) numberOfEpistemicClasses);
        for (int j = 0; j < numberOfStates; ++j) {
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

const std::set<int> &AtlModel::getWinningStates() const {
    return winningStates;
}

void AtlModel::setWinningStates(const std::set<int> &winningStates) {
    AtlModel::winningStates = winningStates;
}

unsigned long AtlModel::getInitialStatesCount() const {
    return initialStatesCount;
}

void AtlModel::setInitialStatesCount(unsigned long initialStatesCount) {
    AtlModel::initialStatesCount = initialStatesCount;
}

std::set<AtlModel::Transition> AtlModel::getTransitions(int stateNumber) {
    return this->transitions[stateNumber];
}



