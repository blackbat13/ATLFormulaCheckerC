//
// Created by black on 14.12.2020.
//

#include "GlobalModel.h"

GlobalModel::GlobalModel(std::vector<LocalModel> localModels, std::vector<std::string> reduction,
                         std::vector<std::string> persistent, std::vector<std::string> coalition,
                         std::vector<std::string> goal): model(SimpleModel(0)) {
    this->localModels = localModels;
    this->reduction = reduction;
    this->persistent = persistent;
    this->coalition = coalition;
    this->goal = goal;
    this->states = std::vector<GlobalState>();
    this->dependent = std::vector<std::vector<std::vector<int> > >();
    this->agentsCount = 0;
    this->statesDict = std::map<GlobalState, int>();
    this->stack1 = std::vector<GlobalState>();
    this->stack2 = std::vector<int>();
    this->g = std::vector<GlobalState>();
    this->stack1Dict = std::map<GlobalState, int>();
    this->transitionsCount = 0;
    this->epistemicStatesDictionaries = std::vector<std::map<EpistemicState, std::set<unsigned int> > >();
}

void GlobalModel::generate(bool reduction) {
    this->agentsCount = this->localModels.size();
    this->epistemicStatesDictionaries = std::vector<std::map<EpistemicState, std::set<unsigned int> > >(
            this->agentsCount);
    this->model = SimpleModel(this->agentsCount);
    this->addToStack(GlobalState(this->agentsCount));
    this->addIndexToTransitions();
    this->computeDependentTransitions();
    this->computeSharedTransitions();
    if (reduction) {
        this->iterPor();
    } else {
        this->compute();
    }

    this->prepareEpistemicRelation();
}

void GlobalModel::prepareEpistemicRelation() {
    for (int i = 0; i < this->agentsCount; i++) {
        for (auto el : this->epistemicStatesDictionaries[i]) {
            this->model.addEpistemicClass(i, el.second);
        }
    }
}

void GlobalModel::addIndexToTransitions() {
    for (int agentId = 0; agentId < this->agentsCount; agentId++) {
        for (int i = 0; i < this->localModels[agentId].transitions.size(); i++) {
            for (int j = 0; j < this->localModels[agentId].transitions[i].size(); j++) {
                this->localModels[agentId].transitions[i][j]->setI(i);
                this->localModels[agentId].transitions[i][j]->setJ(j);
            }
        }
    }
}

void GlobalModel::computeSharedTransitions() {
    struct temp {
        int agentId;
        int i;
        int j;
        SharedTransition *sharedTransition;
    };
    std::vector<temp> replace;

    for (int agentId = 0; agentId < this->agentsCount; agentId++) {
        for (int i = 0; i < this->localModels[agentId].transitions.size(); i++) {
            for (int j = 0; j < this->localModels[agentId].transitions[i].size(); j++) {
                auto transition = this->localModels[agentId].transitions[i][j];
                if (!transition->isShared()) {
                    continue;
                }

                auto sharedTransition = this->createSharedTransitions(transition, agentId);
                std::sort(sharedTransition->transitionList.begin(), sharedTransition->transitionList.end());
                replace.push_back({agentId, i, j, sharedTransition});
            }
        }
    }

    for (auto rep : replace) {
        this->localModels[rep.agentId].transitions[rep.i][rep.j] = rep.sharedTransition;
    }
}

SharedTransition* GlobalModel::createSharedTransitions(LocalTransition* transition, int agentId) {
    auto sharedTransition = new SharedTransition(transition);
    for (int agentId2 = 0; agentId2 < this->agentsCount; agentId2++) {
        if (agentId == agentId2) {
            continue;
        }

        if (this->localModels[agentId2].hasAction(transition->getAction())) {
            for (auto transition2 : this->localModels[agentId2].getTransitions()) {
                if (transition2->getAction() == transition->getAction()) {
                    sharedTransition->addTransition(transition2);
                    break;
                }
            }
        }
    }

    return sharedTransition;
}

std::vector<LocalTransition*> GlobalModel::availableTransitionsInStateForAgent(GlobalState state, int agentId) {
    int agentStateId = state.localStates[agentId];
    auto allTransitions = this->localModels[agentId].privateTransitionsFromState(agentStateId);
    for (auto tr : this->localModels[agentId].sharedTransitionsFromState(agentStateId)) {
        allTransitions.push_back(tr);
    }

    std::vector<LocalTransition*> result;

    for (auto tr : allTransitions) {
        if (tr->checkConditions(state)) {
            result.push_back(tr);
        }
    }

    return result;
}

std::vector<std::vector<LocalTransition*> > GlobalModel::enabledTransitionsInState(GlobalState state) {
    std::vector<std::vector<LocalTransition*> > allTransitions;
    allTransitions.reserve(this->localModels.size());
    for (int agentId = 0; agentId < this->localModels.size(); agentId++) {
        allTransitions.push_back(this->availableTransitionsInStateForAgent(state, agentId));
    }

    std::vector<std::vector<LocalTransition*> > result;
    result.reserve(this->agentsCount);
    for (int agentId = 0; agentId < this->agentsCount; agentId++) {
        result.push_back(this->enabledTransitionsForAgent(agentId, allTransitions));
    }

    return result;
}

std::vector<LocalTransition*>
GlobalModel::enabledTransitionsForAgent(int agentId, std::vector<std::vector<LocalTransition*>> allTransitions) {
    std::vector<LocalTransition*> result;
    for (auto transition : allTransitions[agentId]) {
        if (!transition->isShared()) {
            result.push_back(transition);
            continue;
        }

        if (this->checkIfSharedTransitionIsEnabled(transition, agentId, allTransitions)) {
            result.push_back(transition);
        }
    }

    return result;
}

bool GlobalModel::checkIfSharedTransitionIsEnabled(LocalTransition* transition, int agentId,
                                                   std::vector<std::vector<LocalTransition*>> allTransitions) {
    bool isOk = true;
    for (int agentId2 = 0; agentId2 < this->localModels.size(); agentId2++) {
        if (agentId2 == agentId) {
            continue;
        }

        if (this->localModels[agentId2].hasAction(transition->getAction())) {
            isOk = false;
            for (auto transition2 : allTransitions[agentId2]) {
                if (transition2->isShared() && transition2->getAction() == transition->getAction()) {
                    isOk = true;
                    break;
                }
            }
        }

        if (!isOk) {
            return false;
        }
    }

    return true;
}

std::set<LocalTransition*> GlobalModel::enabledTransitionsInStateSingleItemSet(GlobalState state) {
    auto enabled = this->enabledTransitionsInState(state);
    std::set<LocalTransition*> result;
    for (int agentId = 0; agentId < this->agentsCount; agentId++) {
        for (auto transition : enabled[agentId]) {
            result.insert(transition);
            if (!transition->isShared()) {
                continue;
            }
            for (int agentId2 = agentId + 1; agentId2 < this->agentsCount; agentId2++) {
                int i = 0;
                for (auto transition2 : enabled[agentId2]) {
                    if (transition2->isShared() && transition2->getAction() == transition->getAction()) {
                        enabled[agentId2].erase(enabled[agentId2].begin() + i);
                        break;
                    }

                    i++;
                }
            }
        }
    }

    return result;
}

GlobalState GlobalModel::newStateAfterPrivateTransition(GlobalState state, LocalTransition* transition) {
    auto agentId = transition->getAgentId();
    auto newState = GlobalState::copyState(state, this->persistent);
    newState.setLocalState(agentId, this->localModels[agentId].getStateId(transition->getStateTo()));
    newState = this->copyPropsToState(newState, *transition);
    return newState;
}

std::pair<GlobalState, std::vector<int> > GlobalModel::newStateAfterSharedTransition(GlobalState state,
                                                                                     std::vector<std::pair<int, LocalTransition*> > actualTransition) {
    auto newState = GlobalState::copyState(state, this->persistent);
    std::vector<int> agents;
    for (auto actTran : actualTransition) {
        newState.setLocalState(actTran.first, this->localModels[actTran.first].getStateId(actTran.second->getStateTo()));
        newState = this->copyPropsToState(newState, *actTran.second);
        agents.push_back(actTran.first);
    }

    return std::make_pair(newState, agents);
}

GlobalState
GlobalModel::newStateAfterSharedTransitionList(GlobalState state, std::vector<LocalTransition*> transitions) {
    auto newState = GlobalState::copyState(state, this->persistent);
    for (auto transition : transitions) {
        newState.setLocalState(transition->getAgentId(),
                               this->localModels[transition->getAgentId()].getStateId(transition->getStateTo()));
        newState = this->copyPropsToState(newState, *transition);
    }

    return newState;
}

void GlobalModel::computeNextForState(GlobalState state, int currentStateId) {
    auto allTransitions = this->enabledTransitionsInState(state);
    std::vector<std::string> visited;
    for (int agentId = 0; agentId < this->localModels.size(); agentId++) {
        this->computeNextForStateForAgent(state, currentStateId, agentId, visited, allTransitions);
    }
}

void GlobalModel::computeNextForStateForAgent(GlobalState state, int currentStateId, int agentId,
                                              std::vector<std::string> &visited,
                                              std::vector<std::vector<LocalTransition*> > allTransitions) {
    for (auto transition : allTransitions[agentId]) {
        if (transition->isShared() && std::find(visited.begin(), visited.end(), transition->getAction()) == visited.end()) {
            visited.push_back(transition->getAction());
            auto actualTransition = std::vector<std::pair<int, LocalTransition*> >();
            actualTransition.emplace_back(agentId, transition);
            for (int nAId = agentId + 1; nAId < this->localModels.size(); nAId++) {
                for (auto nTr : allTransitions[nAId]) {
                    if (nTr->isShared() && nTr->getAction() == transition->getAction()) {
                        actualTransition.emplace_back(nAId, nTr);
                        break;
                    }
                }
            }

            auto res = this->newStateAfterSharedTransition(state, actualTransition);
            auto newState = res.first;
            auto agents = res.second;
            auto newStateId = this->addState(newState);
            this->addTransition(currentStateId, newStateId, transition->getAction(), agents);
        } else if (!transition->isShared()) {
            auto newState = this->newStateAfterPrivateTransition(state, transition);
            auto newStateId = this->addState(newState);
            auto agents = std::vector<int>();
            agents.push_back(agentId);
            this->addTransition(currentStateId, newStateId, transition->getAction(), agents);
        }
    }
}

GlobalState GlobalModel::copyPropsToState(GlobalState state, LocalTransition transition) {
    for (auto prop : transition.getProps()) {
        if (prop.second == "?") {
            continue;
        } else if(prop.second == "!") {
            if (state.props.find(prop.second) != state.props.end()) {
                state.setProp(prop.first, state.props[prop.second]);
            }
        } else if (prop.second == "true") {
            state.setProp(prop.first, prop.second);
        } else if (prop.second == "false") {
            state.removeProp(prop.first);
        } else if (state.props.find(prop.second) != state.props.end()) {
            state.setProp(prop.first, state.props[prop.second]);
        } else {
            state.setProp(prop.first, prop.second);
        }
    }

    return state;
}

int GlobalModel::stateFind(GlobalState state) {
    if (this->statesDict.find(state) != this->statesDict.end()) {
        return this->statesDict[state];
    }

    return -1;
}

void GlobalModel::computeDependentTransitions() {
    for (int agentId = 0; agentId < this->agentsCount; agentId++) {
        this->dependent.emplace_back();
        auto agentTransitions = this->localModels[agentId].getTransitions();
        for (int i = 0; i < agentTransitions.size(); i++) {
            std::vector<int> tmp;
            tmp.push_back(agentId);
            this->dependent[agentId].push_back(tmp);
            for (int agent2Id = 0; agent2Id < this->agentsCount; agent2Id++) {
                if (agent2Id == agentId) {
                    continue;
                }

                if (this->localModels[agent2Id].hasAction(agentTransitions[i]->getAction())) {
                    this->dependent[agentId][i].push_back(agent2Id);
                }
            }
        }
    }
}

bool GlobalModel::isInG(GlobalState state) {
    for (auto st : this->g) {
        if (st.equal(state)) {
            return true;
        }
    }

    return false;
}

int GlobalModel::findStateOnStack1(GlobalState state) {
    if (this->stack1Dict.find(state) != this->stack1Dict.end()) {
        return this->stack1Dict[state];
    }

    return -1;
}

bool GlobalModel::addToStack(GlobalState state) {
    if (this->stack1Dict.find(state) != this->stack1Dict.end()) {
        return false;
    }

    this->stack1.push_back(state);
    this->stack1Dict[state] = this->stack1.size() - 1;
    return true;
}

void GlobalModel::popFromStack() {
    this->stack1Dict.erase(this->stack1[this->stack1.size() - 1]);
    this->stack1.pop_back();
}

void GlobalModel::iterPor() {
    std::stack<int> dfsStack;
    dfsStack.push(1);
    while (!dfsStack.empty()) {
        auto dfs = dfsStack.top();
        dfsStack.pop();
        if (dfs == 1) {
            auto g = this->stack1[this->stack1.size() - 1];
            auto reexplore = false;
            auto i = this->findStateOnStack1(g);
            if (i != -1 && i != this->stack1.size() - 1) {
                int depth;
                if (this->stack2.empty()) {
                    depth = 0;
                } else {
                    depth = this->stack2[this->stack2.size() - 1];
                }

                if (i > depth) {
                    reexplore = true;
                } else {
                    this->popFromStack();
                    continue;
                }
            }

            if (!reexplore && this->isInG(g)) {
                this->popFromStack();
                continue;
            }

            this->g.push_back(g);
            auto gStateId = this->addState(g);
            std::set<LocalTransition*> eG;
            auto enG = this->enabledTransitionsInStateSingleItemSet(g);
            if (!enG.empty()) {
                if (!reexplore) {
                    eG = this->ample(g);
                }

                if (eG.empty()) {
                    eG = enG;
                }

                if (eG == enG) {
                    this->stack2.push_back(this->stack1.size());
                }

                dfsStack.push(-1);
                for (auto el : eG) {
                    auto a = el;
                    auto gP = this->successor(g, a);
                    auto gPStateId = this->addState(gP);

                    std::vector<int> agentList;
                    if (a->isShared()) {
                        for (int agentId = 0; agentId < this->localModels.size(); agentId++) {
                            auto local = this->localModels[agentId];
                            if (local.hasAction(a->getAction())) {
                                agentList.push_back(agentId);
                            }
                        }
                    } else {
                        agentList.push_back(a->getAgentId());
                    }

                    this->addTransition(gStateId, gPStateId, a->getAction(), agentList);
                    if (this->addToStack(gP)) {
                        dfsStack.push(1);
                    }
                }
            }
        } else if (dfs == -1) {
            int depth;
            if (this->stack2.empty()) {
                depth = 0;
            } else {
                depth = this->stack2[this->stack2.size() - 1];
            }

            if (depth == this->stack1.size()) {
                this->stack2.pop_back();
            }

            this->popFromStack();
        }
    }
}

std::set<LocalTransition*> GlobalModel::ample(GlobalState state) {
    auto v = this->enabledTransitionsInStateSingleItemSet(state);
    while (!v.empty()) {
        LocalTransition* alpha = *v.begin();
        std::set<LocalTransition*> x, u, dis;
        x.insert(alpha);
        u.insert(alpha);
        while (!x.empty()) {
            auto dif = SetTools::difference(x, v);
            if (!dif.empty()) {
                break;
            }

            auto enabled = this->enabledForX(x);
            auto uni = SetTools::setUnion(dis, enabled);
            dis = uni;

            x = this->dependentForX(x, dis, u);

            uni.clear();
            uni = SetTools::setUnion(u, x);
            u = uni;
        }

        if (x.empty() && !this->checkForCycle(state, u)){// && !this->checkForK(state, u)) {
            return u;
        }

        auto dif = SetTools::difference(v, u);
        v = dif;
    }

    return std::set<LocalTransition*>();
}

bool GlobalModel::checkForCycle(GlobalState state, std::set<LocalTransition*> x) {
    for (auto transition : x) {
        auto successorState = this->successor(state, transition);
        if (this->findStateOnStack1(successorState) != -1) {
            return true;
        }
    }

    return false;
}

bool GlobalModel::checkForK(GlobalState state, std::set<LocalTransition*> x) {
    for (auto transition : x) {
        auto successorState = this->successor(state, transition);
        for (auto agentId : this->agentNameCoalitionToIds(this->coalition)) {
            if (state.localStates[agentId] != successorState.localStates[agentId]) {
                return true;
            }

            for (auto prop : this->localModels[agentId].props) {
                if (state.props.find(prop) != state.props.end() &&
                    successorState.props.find(prop) == successorState.props.end()) {
                    return true;
                }

                if (state.props.find(prop) == state.props.end() &&
                    successorState.props.find(prop) != successorState.props.end()) {
                    return true;
                }

                if (state.props.find(prop) == state.props.end()) {
                    continue;
                }

                if (state.props[prop] != successorState.props[prop]) {
                    return true;
                }
            }
        }
    }

    return false;
}

std::set<LocalTransition*> GlobalModel::enabledForX(std::set<LocalTransition*> x) {
    std::set<LocalTransition*> result;
    for (auto transition : x) {
        if (auto sharedTransition = dynamic_cast<SharedTransition*>(transition)) {
            for (auto transition2 : sharedTransition->transitionList) {
                for (auto tr : this->localModels[transition2->getAgentId()].getTransitions()) {
                    if (tr->getStateFrom() != transition2->getStateFrom()) {
                        result.insert(tr);
                    }
                }
            }
        } else {
            for (auto tr : this->localModels[transition->getAgentId()].getTransitions()) {
                if (tr->getStateFrom() != transition->getStateFrom()) {
                    result.insert(tr);
                }
            }
        }
    }

    return result;
}

std::set<LocalTransition*>
GlobalModel::dependentForX(std::set<LocalTransition*> x, std::set<LocalTransition*> dis, std::set<LocalTransition*> u) {
    std::set<LocalTransition*> result;
    for (auto transition : x) {
        if (auto sharedTransition = dynamic_cast<SharedTransition*>(transition)) {
            for (auto transition2 : sharedTransition->transitionList) {
                for (auto tr : this->localModels[transition2->getAgentId()].getTransitions()) {
                    if (dis.find(tr) == dis.end() && u.find(tr) == u.end()) {
                        result.insert(tr);
                    }
                }
            }
        } else {
            for (auto tr : this->localModels[transition->getAgentId()].getTransitions()) {
                if (dis.find(tr) == dis.end() && u.find(tr) == u.end()) {
                    result.insert(tr);
                }
            }
        }
    }

    return result;
}

GlobalState GlobalModel::successor(GlobalState state, LocalTransition* transition) {
    if (auto sharedTransition = dynamic_cast<SharedTransition*>(transition)) {
        return this->newStateAfterSharedTransitionList(state, sharedTransition->transitionList);
    } else {
        return this->newStateAfterPrivateTransition(state, transition);
    }
}

int GlobalModel::addState(GlobalState &state) {
    auto stateId = this->stateFind(state);
    if (stateId == -1) {
        stateId = this->states.size();
        state.id = stateId;
        this->states.push_back(state);
        this->statesDict[state] = stateId;
        this->model.addState(state);
        for (int i = 0; i < this->localModels.size(); i++) {
            auto epistemicState = this->getEpistemicState(state, i);
            this->addToEpistemicDictionary(epistemicState, stateId, i);
        }
    }

    state.id = stateId;
    return stateId;
}

EpistemicState GlobalModel::getEpistemicState(GlobalState state, int agentId) {
    int sum = 0;
    for (auto localState : state.localStates) {
        sum += localState;
    }

    if (sum == 0) {
        return {state.localStates[agentId], true, std::map<std::string, std::string>()};
    }

    EpistemicState epistemicState = {state.localStates[agentId], false, std::map<std::string, std::string>()};
    for (auto prop : this->localModels[agentId].props) {
        if (state.props.find(prop) != state.props.end()) {
            epistemicState.props[prop] = state.props[prop];
        }
    }

    return epistemicState;
}

void GlobalModel::addToEpistemicDictionary(EpistemicState state, int newStateId, int agentId) {
    if (this->epistemicStatesDictionaries[agentId].find(state) == this->epistemicStatesDictionaries[agentId].end()) {
        this->epistemicStatesDictionaries[agentId][state] = std::set<unsigned int>();
    }

    this->epistemicStatesDictionaries[agentId][state].insert(newStateId);
}

void GlobalModel::addTransition(int stateFrom, int stateTo, std::string action, std::vector<int> agents) {
    this->transitionsCount++;
    this->model.addTransition(stateFrom, stateTo, this->createListOfActions(action, agents));
}

std::vector<std::string> GlobalModel::createListOfActions(std::string action, std::vector<int> agents) {
    std::vector<std::string> actions = std::vector<std::string>(this->agentsCount, "");
    for (auto agentId : agents) {
        actions[agentId] = action;
    }

    return actions;
}

void GlobalModel::compute() {
    auto state = GlobalState(this->localModels.size());
    this->states.push_back(state);
    this->model.addState(state);
    int i = 0;
    while (i < this->states.size()) {
        state = this->states[i];
        auto currentStateId = i;
        i++;
        this->computeNextForState(state, currentStateId);
    }
}

int GlobalModel::agentNameToId(std::string agentName) {
    for (int agentId = 0; agentId < this->localModels.size(); agentId++) {
        if (this->localModels[agentId].agentName == agentName) {
            return agentId;
        }
    }

    return -1;
}

std::vector<int> GlobalModel::agentNameCoalitionToIds(std::vector<std::string> agentNames) {
    auto agentIds = std::vector<int>();
    for (auto agentName : agentNames) {
        agentIds.push_back(this->agentNameToId(agentName));
    }

    return agentIds;
}

void GlobalModel::print() {
    for (auto model : this->localModels) {
        model.print();
    }
}

void GlobalModel::setCoalition(std::vector<std::string> coalition) {
    this->coalition = coalition;
}

std::set<unsigned int> GlobalModel::getWinningStates(int formulaNo) {
    auto winningStates = std::set<unsigned int>();
    for (auto state : this->states) {
        bool ok = true;
        if (formulaNo == 1) {
            for (auto prop : this->goal) {
                if (state.props.find(prop) == state.props.end() || state.props[prop] == "false") {
                    ok = false;
                    break;
                }
            }
        } else if (formulaNo == 2) {
            if (state.props.find("v_Voter1") == state.props.end() || state.props["v_Voter1"] == "1") {
                ok = false;
            } else {
                for (auto stateId : this->model.epistemicClassForState(state.id,
                                                                       this->agentNameToId(this->coalition[0]))) {
                    if (this->states[stateId].props.find("v_Voter1") != this->states[stateId].props.end() &&
                        this->states[stateId].props["v_Voter1"] == "1") {
                        ok = false;
                        break;
                    }
                }
            }
        }

        if (ok) {
            winningStates.insert(state.id);
        }
    }

    return winningStates;
}

std::vector<std::set<std::string> > GlobalModel::getActions() {
    auto actions = std::vector<std::set<std::string> >();
    for (auto local : this->localModels) {
        actions.push_back(local.actions);
        actions[actions.size() - 1].insert("");
    }

    return actions;
}

std::pair<std::set<unsigned int>, double> GlobalModel::verifyApproximation(bool perfectInf, int formulaNo) {
    auto atlModel = this->model.toAtlImperfect();
    if (perfectInf) {
        auto result = atlModel.minimumFormulaOneAgentMultipleStatesPerfectInformation(
                this->agentNameToId(this->coalition[0]), this->getWinningStates(formulaNo));
        return std::make_pair(result, 0);
    } else {
        auto result = atlModel.minimumFormulaOneAgentMultipleStatesDisjoint(this->agentNameToId(this->coalition[0]),
                                                                            this->getWinningStates(formulaNo));
        return std::make_pair(result, 0);
    }
}

const SimpleModel &GlobalModel::getModel() const {
    return model;
}

int GlobalModel::getStatesCount() {
    return this->states.size();
}

int GlobalModel::getTransitionsCount() {
    return this->transitionsCount;
}

bool EpistemicState::operator==(const EpistemicState &rhs) const {
    return localState == rhs.localState &&
           init == rhs.init &&
           props == rhs.props;
}

bool EpistemicState::operator!=(const EpistemicState &rhs) const {
    return !(rhs == *this);
}

bool EpistemicState::operator<(const EpistemicState &rhs) const {
    if (localState < rhs.localState)
        return true;
    if (rhs.localState < localState)
        return false;
    if (init < rhs.init)
        return true;
    if (rhs.init < init)
        return false;
    return props < rhs.props;
}

bool EpistemicState::operator>(const EpistemicState &rhs) const {
    return rhs < *this;
}

bool EpistemicState::operator<=(const EpistemicState &rhs) const {
    return !(rhs < *this);
}

bool EpistemicState::operator>=(const EpistemicState &rhs) const {
    return !(*this < rhs);
}
