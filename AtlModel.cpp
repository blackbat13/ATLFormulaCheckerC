//
// Created by Damian Kurpiewski on 18.03.2017.
//

#include "AtlModel.h"

AtlModel::Transition::Transition(int nextState, const std::vector<std::string> &actions) : nextState(nextState),
                                                                                           actions(actions) {}

AtlModel::Transition::Transition() {

}

bool AtlModel::Transition::operator<(const AtlModel::Transition &rhs) const {
    if (nextState < rhs.nextState)
        return true;
    if (rhs.nextState < nextState)
        return false;
    return actions < rhs.actions;
}

bool AtlModel::Transition::operator>(const AtlModel::Transition &rhs) const {
    return rhs < *this;
}

bool AtlModel::Transition::operator<=(const AtlModel::Transition &rhs) const {
    return !(rhs < *this);
}

bool AtlModel::Transition::operator>=(const AtlModel::Transition &rhs) const {
    return !(*this < rhs);
}

AtlModel::AtlModel(int numberOfAgents, int numberOfStates) : numberOfAgents(numberOfAgents),
                                                             numberOfStates(numberOfStates) {
    this->imperfectInformation = std::vector<std::vector<std::set<int> > >((unsigned long) numberOfAgents);
    this->agentsActions = std::vector<std::vector<std::string> >((unsigned long) numberOfAgents);
    this->epistemicClassMembership = std::vector<std::vector<int> >((unsigned long) numberOfAgents,
                                                                    std::vector<int>((unsigned long) numberOfStates,
                                                                                     -1));
    this->epistemicClassDisjoint = std::vector<DisjointUnion>((unsigned long) numberOfAgents,
                                                              DisjointUnion(numberOfStates));
    this->canGoThere = std::vector<std::vector<std::map<std::string, std::set<int> > > >((unsigned long) numberOfAgents, std::vector<std::map<std::string, std::set<int> > >((unsigned long) numberOfStates));
    this->transitions = std::vector<std::set<Transition> >((unsigned long) numberOfStates);
    this->reverseTransitions = std::vector<std::set<Transition> >((unsigned long) numberOfStates);
    this->preStates = std::vector<std::set<int> >((unsigned long) numberOfStates);
}

void AtlModel::addAction(int agentNumber, std::string action) {
    this->agentsActions[agentNumber].push_back(action);
}

void AtlModel::addTransition(int from, int to, std::vector<std::string> actions) {
    Transition transition = Transition(to, actions);
    Transition reverseTransition = Transition(from, actions);
    this->transitions[from].insert(transition);
    this->reverseTransitions[to].insert(reverseTransition);
    this->preStates[to].insert(from);
}

void AtlModel::addEpistemicClass(int agentNumber, std::set<int> epistemicClass) {
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
    for(int i = 0; i < this->imperfectInformation[agentNumber].size(); ++i) {
        int firstState = *(this->imperfectInformation[agentNumber][i].begin());
        for(auto state: this->imperfectInformation[agentNumber][i]) {
            this->epistemicClassMembership[agentNumber][state] = i;
            this->epistemicClassDisjoint[agentNumber].unionn(firstState, state);
        }

        this->findWhereCanGo( this->imperfectInformation[agentNumber][i], i, agentNumber);
    }
}

void AtlModel::findWhereCanGo(std::set<int> epistemicClass, int epistemicClassNumber, int agentNumber) {
    for(auto action: this->agentsActions[agentNumber]) {
        std::set<int> canGoTemp;
        for(auto state: epistemicClass) {
            std::set<int> canGoStateTemp;
            for(auto transition: this->transitions[state]) {
                if(transition.actions[agentNumber] == action) {
                    canGoStateTemp.insert(transition.nextState);
                }
            }

            canGoTemp.insert(canGoStateTemp.begin(), canGoStateTemp.end());
            if(canGoStateTemp.size() == 0) {
                canGoTemp.clear();
                break;
            }
        }

        this->canGoThere[agentNumber][epistemicClassNumber][action] = canGoTemp;
    }
}

bool AtlModel::isReachableByAgentDisjoint(std::string action, int fromState, int agentNumber, int firstWinning,
                                          DisjointUnion winningStates) {
    bool actionOk = false;
    for(auto transition: this->transitions[fromState]) {
        if(transition.actions[agentNumber] == action) {
            actionOk = true;
            if(!winningStates.isSame(firstWinning, transition.nextState)) {
                return false;
            }
        }
    }

    return actionOk;
}

bool AtlModel::isReachableByAgent(std::string action, int fromState, bool *isWinningState, int agentNumber) {
    bool actionOk = false;
    for(auto transition: this->transitions[fromState]) {
        if(transition.actions[agentNumber] == action) {
            actionOk = true;
            if(!isWinningState[transition.nextState]) {
                return false;
            }
        }
    }

    return actionOk;
}

bool
AtlModel::isReachableByAgentInSet(std::string action, int fromState, std::set<int> winningStates, int agentNumber) {
    bool actionOk = false;
    for(auto transition: this->transitions[fromState]) {
        if(transition.actions[agentNumber] == action) {
            actionOk = true;
            if(winningStates.find(transition.nextState) == winningStates.end()) {
                return false;
            }
        }
    }

    return actionOk;
}

std::pair<std::set<int>, bool>
AtlModel::basicFormulaOneAgentMultipleStatesDisjoint(int agentNumber, std::set<int> currentStates, int firstWinning,
                                                     DisjointUnion& winningStatesDisjoint,
                                                     std::vector<std::map<std::string, std::set<int> > >& customCanGoThere) {
    std::set<int> resultStates;
    std::vector<std::string> actions = this->agentsActions[agentNumber];
    std::set<int> preImage;
    bool modified = false;
    for(auto winningState: currentStates) {
        for(auto preState: this->preStates[winningState]) {
            preImage.insert(this->epistemicClassMembership[agentNumber][preState]);
        }
    }

    firstWinning = winningStatesDisjoint.find(firstWinning);
    for(auto stateEpistemicClassNumber: preImage) {
        int state = *this->imperfectInformation[agentNumber][stateEpistemicClassNumber].begin();
        state = winningStatesDisjoint.find(state);
        if(state == firstWinning) {
            continue;
        }

        std::set<int> sameStates = this->imperfectInformation[agentNumber][stateEpistemicClassNumber];
        for(auto action: actions) {
            std::set<int> statesCanGo = customCanGoThere[stateEpistemicClassNumber][action];
            if(statesCanGo.size() == 0) {
                continue;
            }

            bool isOk = true;
            std::set<int> newStatesCanGo;
            for(auto stateCan: statesCanGo) {
                int newStateCan = winningStatesDisjoint.find(stateCan);
                if(firstWinning != newStateCan) {
                    isOk = false;
                }

                newStatesCanGo.insert(newStateCan);
            }

            customCanGoThere[stateEpistemicClassNumber][action] = newStatesCanGo;

            if(isOk) {
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
AtlModel::basicFormulaOneAgentMultipleStatesPerfectInformation(int agentNumber, std::set<int> currentStates,
                                                               bool *isWinningState) {
    std::set<int> resultStates;
    std::vector<std::string> actions = this->agentsActions[agentNumber];
    std::set<int> preImage;
    for(auto winningState: currentStates) {
        preImage.insert(this->preStates[winningState].begin(), this->preStates[winningState].end());
    }

    for(auto state: preImage) {
        std::set<std::string> allActions;
        std::set<std::string> badActions;
        std::set<std::string> actionsDifference;
        for(auto transition: this->transitions[state]) {
            allActions.insert(transition.actions[agentNumber]);
            if(!isWinningState[transition.nextState]) {
                badActions.insert(transition.actions[agentNumber]);
            }
        }

        std::set_difference(allActions.begin(), allActions.end(), badActions.begin(), badActions.end(), std::inserter(actionsDifference, actionsDifference.begin()));
        if(actionsDifference.size() > 0) {
            resultStates.insert(state);
        }
    }

    for(auto stateNumber: resultStates) {
        isWinningState[stateNumber] = true;
    }

    return resultStates;
}

std::set<int>
AtlModel::minimumFormulaOneAgentMultipleStatesDisjoint(int agentNumber, std::set<int> winningStates) {
    if(winningStates.size() == 0) {
        return std::set<int>();
    }

    std::set<int> resultStates;
    resultStates.insert(winningStates.begin(), winningStates.end());
    int numberOfIterations = 0;
    std::set<int> currentStates = winningStates;
    DisjointUnion winningStatesDisjoint = DisjointUnion(this->epistemicClassDisjoint[agentNumber].getSubsets());
    int firstWinning = winningStatesDisjoint.find(*winningStates.begin());
    std::set<int> epistemicClassNumbers;
    for(auto stateNumber: winningStates) {
        int epistemicClassNumber = this->epistemicClassMembership[agentNumber][stateNumber];
        epistemicClassNumbers.insert(epistemicClassNumber);
    }

    for(auto epistemicClassNumber: epistemicClassNumbers) {
        std::set<int> epistemicStates = this->imperfectInformation[agentNumber][epistemicClassNumber];
        bool isOk = true;
        for(auto epistemicState: epistemicStates) {
            if(winningStates.find(epistemicState) == winningStates.end()) {
                isOk = false;
                break;
            }
        }

        if(isOk) {
            winningStatesDisjoint.unionn(firstWinning, *epistemicStates.begin());
        }
    }

    std::vector<std::map<std::string, std::set<int> > > customCanGoThere = this->canGoThere[agentNumber];
    while(true) {
        std::pair<std::set<int>, bool> formulaResult = this->basicFormulaOneAgentMultipleStatesDisjoint(agentNumber, currentStates, firstWinning, winningStatesDisjoint, customCanGoThere);
        currentStates = formulaResult.first;
        bool modified = formulaResult.second;
        resultStates.insert(currentStates.begin(), currentStates.end());
        if(!modified) {
            break;
        }

        ++numberOfIterations;
    }

    printf("Number of iterations: %d\n", numberOfIterations);
    return resultStates;
}

std::set<int>
AtlModel::minimumFormulaOneAgentMultipleStatesPerfectInformation(int agentNumber, std::set<int> winningStates) {
    std::set<int> resultStates;
    resultStates.insert(winningStates.begin(), winningStates.end());
    unsigned long resultStatesLength = resultStates.size();
    int numberOfIterations = 0;
    std::set<int> currentStates = winningStates;
    bool isWinningState[this->numberOfStates];
    for(int i = 0; i < this->numberOfStates; ++i) {
        isWinningState[i] = false;
    }

    for(auto stateNumber: winningStates) {
        isWinningState[stateNumber] = true;
    }

    while(true) {
        currentStates = this->basicFormulaOneAgentMultipleStatesPerfectInformation(agentNumber, currentStates, isWinningState);
        resultStates.insert(currentStates.begin(), currentStates.end());
        if(resultStates.size() == resultStatesLength) {
            break;
        }

        resultStatesLength = resultStates.size();
        ++numberOfIterations;
    }

    printf("Number of iterations: %d\n", numberOfIterations);
    return resultStates;
}

AtlModel::AtlModel() {}




