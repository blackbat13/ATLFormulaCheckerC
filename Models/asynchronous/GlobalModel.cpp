//
// Created by black on 14.12.2020.
//

#include "GlobalModel.h"

#include <utility>

GlobalModel::GlobalModel(std::vector<LocalModel> localModels, std::vector<std::string> reduction,
                         std::vector<std::string> persistent, std::vector<std::string> coalition,
                         std::vector<std::string> goal) : model(SimpleModel(0)) {
    this->localModels = std::move(localModels);
    this->reduction = std::move(reduction);
    this->persistent = std::move(persistent);
    this->coalition = std::move(coalition);
    this->goal = std::move(goal);
    this->dependent = std::vector<std::vector<std::vector<std::vector<int> > > >();
    this->states = std::vector<GlobalState>();
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
    this->computeSharedTransitions();
    this->computeDependentTransitions();
    if (reduction) {
        this->iterPor();
    } else {
        this->compute();
    }

    this->prepareEpistemicRelation();
    auto actions = this->getActions();
    for (int agentId = 0; agentId < this->localModels.size(); agentId++) {
        this->model.addActions(agentId, actions[agentId]);
    }
}

void GlobalModel::prepareEpistemicRelation() {
    for (int i = 0; i < this->agentsCount; i++) {
        for (const auto& el : this->epistemicStatesDictionaries[i]) {
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

SharedTransition *GlobalModel::createSharedTransitions(LocalTransition *transition, int agentId) {
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

std::vector<LocalTransition *> GlobalModel::availableTransitionsInStateForAgent(GlobalState state, int agentId) {
    int agentStateId = state.localStates[agentId];
    auto allTransitions = this->localModels[agentId].privateTransitionsFromState(agentStateId);
    for (auto tr : this->localModels[agentId].sharedTransitionsFromState(agentStateId)) {
        allTransitions.push_back(tr);
    }

    std::vector<LocalTransition *> result;

    for (auto tr : allTransitions) {
        if (tr->checkConditions(state)) {
            result.push_back(tr);
        }
    }

    return result;
}

std::vector<std::vector<LocalTransition *> > GlobalModel::enabledTransitionsInState(const GlobalState& state) {
    std::vector<std::vector<LocalTransition *> > allTransitions;
    allTransitions.reserve(this->localModels.size());
    for (int agentId = 0; agentId < this->localModels.size(); agentId++) {
        allTransitions.push_back(this->availableTransitionsInStateForAgent(state, agentId));
    }

    std::vector<std::vector<LocalTransition *> > result;
    result.reserve(this->agentsCount);
    for (int agentId = 0; agentId < this->agentsCount; agentId++) {
        result.push_back(this->enabledTransitionsForAgent(agentId, allTransitions));
    }

    return result;
}

std::vector<LocalTransition *>
GlobalModel::enabledTransitionsForAgent(int agentId, std::vector<std::vector<LocalTransition *>> allTransitions) {
    std::vector<LocalTransition *> result;
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

bool GlobalModel::checkIfSharedTransitionIsEnabled(LocalTransition *transition, int agentId,
                                                   std::vector<std::vector<LocalTransition *>> allTransitions) {
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

std::set<LocalTransitionTup> GlobalModel::enabledTransitionsInStateSingleItemSet(GlobalState state) {
    auto enabled = this->enabledTransitionsInState(std::move(state));
    std::set<LocalTransitionTup> result;
    for (int agentId = 0; agentId < this->agentsCount; agentId++) {
        for (auto transition : enabled[agentId]) {
            result.insert(transition->toTup());
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

GlobalState GlobalModel::newStateAfterPrivateTransition(GlobalState state, LocalTransition *transition) {
    auto agentId = transition->getAgentId();
    auto newState = GlobalState::copyState(std::move(state), this->persistent);
    newState.setLocalState(agentId, this->localModels[agentId].getStateId(transition->getStateTo()));
    newState = this->copyPropsToState(newState, *transition);
    return newState;
}

std::pair<GlobalState, std::vector<int> > GlobalModel::newStateAfterSharedTransition(GlobalState state,
                                                                                     const std::vector<std::pair<int, LocalTransition *> >& actualTransition) {
    auto newState = GlobalState::copyState(state, this->persistent);
    std::vector<int> agents;
    for (auto actTran : actualTransition) {
        newState.setLocalState(actTran.first,
                               this->localModels[actTran.first].getStateId(actTran.second->getStateTo()));
        newState = this->copyPropsToState(newState, *actTran.second);
        agents.push_back(actTran.first);
    }

    return std::make_pair(newState, agents);
}

GlobalState
GlobalModel::newStateAfterSharedTransitionList(GlobalState state, const std::vector<LocalTransition *>& transitions) {
    auto newState = GlobalState::copyState(state, this->persistent);
    for (auto transition : transitions) {
        newState.setLocalState(transition->getAgentId(),
                               this->localModels[transition->getAgentId()].getStateId(transition->getStateTo()));
        newState = this->copyPropsToState(newState, *transition);
    }

    return newState;
}

void GlobalModel::computeNextForState(const GlobalState& state, int currentStateId) {
    auto allTransitions = this->enabledTransitionsInState(state);
    std::vector<std::string> visited;
    for (int agentId = 0; agentId < this->localModels.size(); agentId++) {
        this->computeNextForStateForAgent(state, currentStateId, agentId, visited, allTransitions);
    }
}

void GlobalModel::computeNextForStateForAgent(const GlobalState& state, int currentStateId, int agentId,
                                              std::vector<std::string> &visited,
                                              std::vector<std::vector<LocalTransition *> > allTransitions) {
    for (auto transition : allTransitions[agentId]) {
        if (transition->isShared() &&
            std::find(visited.begin(), visited.end(), transition->getAction()) == visited.end()) {
            visited.push_back(transition->getAction());
            auto actualTransition = std::vector<std::pair<int, LocalTransition *> >();
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

GlobalState GlobalModel::copyPropsToState(GlobalState state, const LocalTransition& transition) {
    for (const auto& prop : transition.getProps()) {
        if (prop.second == "?") {
            continue;
        } else if (prop.second == "!") {
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

int GlobalModel::stateFind(const GlobalState& state) {
    if (this->statesDict.find(state) != this->statesDict.end()) {
        return this->statesDict[state];
    }

    return -1;
}

bool GlobalModel::isInG(const GlobalState& state) {
    for (const auto& st : this->g) {
        if (st == state) {
            return true;
        }
    }

    return false;
}

int GlobalModel::findStateOnStack1(const GlobalState& state) {
    if (this->stack1Dict.find(state) != this->stack1Dict.end()) {
        return this->stack1Dict[state];
    }

    return -1;
}

bool GlobalModel::addToStack(const GlobalState& state) {
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

void GlobalModel::computeDependentTransitions() {
    for (int agentId = 0; agentId < this->agentsCount; agentId++) {
        this->dependent.emplace_back();
        auto agentTransitions = this->localModels[agentId].getTransitions();

        for(int i = 0; i < this->localModels[agentId].transitions.size(); i++) {
            this->dependent[agentId].emplace_back();
            for(int j = 0; j < this->localModels[agentId].transitions[i].size(); j++) {
                auto transition = this->localModels[agentId].transitions[i][j];
                this->dependent[agentId][i].emplace_back();
                for(int agent2Id = 0; agent2Id < this->agentsCount; agent2Id++) {
                    if(this->localModels[agent2Id].hasAction(transition->getAction())) {
                        this->dependent[agentId][i][j].push_back(agent2Id);
                    }
                }
            }
        }
    }
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
            std::set<LocalTransitionTup> eG;
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
                    auto a = this->localModels[el.agentId].transitions[el.i][el.j];
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

std::set<LocalTransitionTup> GlobalModel::ample(const GlobalState& state) {
    auto v = this->enabledTransitionsInStateSingleItemSet(state);
    while (!v.empty()) {
        LocalTransitionTup alpha = *v.begin();
        std::set<LocalTransitionTup> x, u, dis;
        x.insert(alpha);
        u.insert(alpha);
        do {
            auto enabled = this->enabledForX(x);
            dis = SetTools::setUnion(dis, enabled);
            x = this->dependentForX(x, dis, u);
            u = SetTools::setUnion(u, x);
        }while (!x.empty() && (SetTools::difference(x, v)).empty());

        if (x.empty() && !checkForVisible(state, u)) {
            return u;
        }

        v = SetTools::difference(v, u);
    }

    return this->enabledTransitionsInStateSingleItemSet(state);
}

bool GlobalModel::checkForCycle(const GlobalState& state, const std::set<LocalTransitionTup>& x) {
    for (auto el : x) {
        auto transition = this->localModels[el.agentId].transitions[el.i][el.j];
        auto successorState = this->successor(state, transition);
        if (this->findStateOnStack1(successorState) != -1) {
            successorState.print();
            return true;
        }
    }

    return false;
}

bool GlobalModel::checkForK(GlobalState state, std::set<LocalTransition *> x) {
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

std::set<LocalTransitionTup> GlobalModel::enabledForX(const std::set<LocalTransitionTup>& x) {
    std::set<LocalTransitionTup> result;
    for (auto el : x) {
        auto transition = this->localModels[el.agentId].transitions[el.i][el.j];
        if (auto sharedTransition = dynamic_cast<SharedTransition *>(transition)) {
            for (auto transition2 : sharedTransition->transitionList) {
                for (auto tr : this->localModels[transition2->getAgentId()].getTransitions()) {
                    if (tr->getStateFrom() != transition2->getStateFrom()) {
                        result.insert(tr->toTup());
                    }
                }
            }
        } else {
            for (auto tr : this->localModels[transition->getAgentId()].getTransitions()) {
                if (tr->getStateFrom() != transition->getStateFrom()) {
                    result.insert(tr->toTup());
                }
            }
        }
    }

    return result;
}

std::set<LocalTransitionTup>
GlobalModel::dependentForX(const std::set<LocalTransitionTup>& x, std::set<LocalTransitionTup> dis,
                           std::set<LocalTransitionTup> u) {
    std::set<LocalTransitionTup> result;
    for (auto el : x) {
        for(auto agentId : this->dependent[el.agentId][el.i][el.j]) {
            auto agentTransitions = this->localModels[agentId].getTransitions();
            for(auto tr : agentTransitions) {
                if (dis.find(tr->toTup()) == dis.end() && u.find(tr->toTup()) == u.end()) {
                    result.insert(tr->toTup());
                }
            }
        }
    }

    return result;
}

GlobalState GlobalModel::successor(const GlobalState& state, LocalTransition *transition) {
    if (auto sharedTransition = dynamic_cast<SharedTransition *>(transition)) {
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

    if (sum == 0 && state.props.empty()) {
        return {state.localStates, true, std::map<std::string, std::string>()};
    }

    EpistemicState epistemicState = {state.localStates, false, std::map<std::string, std::string>()};
    for (const auto& prop : this->localModels[agentId].props) {
        if (state.props.find(prop) != state.props.end()) {
            epistemicState.props[prop] = state.props[prop];
        }
    }

    return epistemicState;
}

void GlobalModel::addToEpistemicDictionary(const EpistemicState& state, int newStateId, int agentId) {
    if (this->epistemicStatesDictionaries[agentId].find(state) == this->epistemicStatesDictionaries[agentId].end()) {
        this->epistemicStatesDictionaries[agentId][state] = std::set<unsigned int>();
    }

    this->epistemicStatesDictionaries[agentId][state].insert(newStateId);
}

void GlobalModel::addTransition(int stateFrom, int stateTo, const std::string& action, const std::vector<int>& agents) {
    this->transitionsCount++;
    this->model.addTransition(stateFrom, stateTo, this->createListOfActions(action, agents));
}

std::vector<std::string> GlobalModel::createListOfActions(const std::string& action, const std::vector<int>& agents) const {
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

int GlobalModel::agentNameToId(const std::string& agentName) {
    for (int agentId = 0; agentId < this->localModels.size(); agentId++) {
        if (this->localModels[agentId].agentName == agentName) {
            return agentId;
        }
    }

    return -1;
}

std::vector<int> GlobalModel::agentNameCoalitionToIds(const std::vector<std::string>& agentNames) {
    auto agentIds = std::vector<int>();
    for (const auto& agentName : agentNames) {
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
    this->coalition = std::move(coalition);
}

std::set<unsigned int> GlobalModel::getWinningStates(int formulaNo) {
    auto winningStates = std::set<unsigned int>();
    for (auto state : this->states) {
        bool ok = true;
        if (formulaNo == 0) {
            std::string propName = "pun1";
            if (!state.hasProp(propName) || state.getProp(propName) == "false") {
                ok = false;
            }
        } else if (formulaNo == 1) {
            std::string propName = "v_Voter1";
            if (!state.hasProp(propName) || state.getProp(propName) == "1") {
                ok = false;
            } else {
                for (auto stateId : this->model.epistemicClassForState(state.id,
                                                                       this->agentNameToId(this->coalition[0]))) {
                    if (this->states[stateId].hasProp(propName) && this->states[stateId].getProp(propName) == "1") {
                        ok = false;
                        break;
                    }
                }
            }
        } else if (formulaNo == 2) {
            std::string propName = "t_Voter1";
            if (!state.hasProp(propName)) {
                ok = false;
            } else {
                auto propVal = state.getProp(propName);
                for(auto stateId : this->model.epistemicClassForState(state.id, this->agentNameToId(this->coalition[0]))) {
                    if(this->states[stateId].getProp(propName) != propVal) {
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
    for (const auto& local : this->localModels) {
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

std::pair<bool, double> GlobalModel::verifyParallel(int formulaNo) {
    auto parallelModel = this->model.toParallelModel(this->agentNameToId(this->coalition[0]),
                                                     this->getWinningStates(formulaNo));
    return std::make_pair(parallelModel.recursiveDFS(0, -1, false, 0), 0);
}

const SimpleModel &GlobalModel::getModel() const {
    return model;
}

int GlobalModel::getStatesCount() const {
    return this->states.size();
}

int GlobalModel::getTransitionsCount() const {
    return this->transitionsCount;
}

bool GlobalModel::checkForVisible(GlobalState state, const std::set<LocalTransitionTup>& x) {
    for(auto el : x) {
        auto transition = this->localModels[el.agentId].transitions[el.i][el.j];
        if(el.agentId == this->agentNameToId(this->coalition[0])) {
            return true;
        }

        for(const auto& prop : this->reduction) {
            if(transition->hasProp(prop)) {
                return true;
            }
        }
    }

    return false;
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

void EpistemicState::print() {
    //printf("Local state: %d, Init: %d, Props: ", this->localState, this->init);
    for (const auto& el : this->props) {
        printf("%s:%s, ", el.first.c_str(), el.second.c_str());
    }

    printf("\n");
}
