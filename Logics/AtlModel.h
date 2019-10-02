#ifndef ATLFORMULACHECKER_ATLMODEL_H
#define ATLFORMULACHECKER_ATLMODEL_H

#include <vector>
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <cstdio>
#include <iterator>
#include <algorithm>
#include "../Tools/DisjointUnion.h"
#include "../Models/Transition.h"

class AtlModel {
private:

    /// Number of agents in the model.
    int numberOfAgents;

    /// Number of states in the model.
    int numberOfStates;

    /// Number of transitions in the model.
    int numberOfTransitions;

    /// Vector of transitions. For each state identifier store set of transitions going out from that state.
    std::vector<std::set<Transition> > transitions;

    std::vector<std::set<int> > preStates;

    /// Vector of agents actions. For each agent identifier store vector of actions available for that agent.
    std::vector<std::vector<std::string> > agentsActions;

    /// For each state store identifier of the epistemic class to which that state belongs.
    std::vector<std::vector<int> > epistemicClassMembership;

    std::vector<DisjointUnion> epistemicClassDisjoint;

    std::vector<std::vector<std::map<std::string, std::set<int> > > > canGoThere;

    std::set<int> winningStates;

    /// Number of initial states
    unsigned long initialStatesCount;
public:

    unsigned long getInitialStatesCount() const;

    void setInitialStatesCount(unsigned long initialStatesCount);

public:
    std::vector<std::vector<std::set<int> > > imperfectInformation;

    AtlModel(int numberOfAgents, int numberOfStates);

    AtlModel();

    /**
     * Add action for a given agent.
     *
     * @param agentNumber agent id
     * @param action action to add
     */
    void addAction(int agentNumber, const std::string& action);

    /**
     * Add transition to the model.
     *
     * @param from initial state identifier
     * @param to end state identifier
     * @param actions vecotr of agents actions
     */
    void addTransition(int from, int to, const std::vector<std::string>& actions);

    /**
     * Add epistemic class.
     *
     * @param agentNumber agent identifier
     * @param epistemicClass set of state identifiers in the given epistemic class
     */
    void addEpistemicClass(int agentNumber, const std::set<int>& epistemicClass);

    /**
     * Finish creation of the epistemic classes for given agent.
     * Should be called after adding all epistemic classes for given agent.
     *
     * @param agentNumber agent identifier
     */
    void finishEpistemicClasses(int agentNumber);

    /**
     * Precomputate relations between epistemic classes.
     *
     * @param epistemicClass
     * @param epistemicClassNumber
     * @param agentNumber agent identifier
     */
    void findWhereCanGo(const std::set<int>& epistemicClass, unsigned int epistemicClassNumber, int agentNumber);

    /**
     * Check if winning states are reachable for agent from the given state, when using given action.
     *
     * @param action
     * @param fromState
     * @param agentNumber
     * @param firstWinning
     * @param winningStates
     * @return true if at least one of the winning states is reachable by agent with given action, false otherwise
     */
    bool isReachableByAgentDisjoint(const std::string& action, int fromState, int agentNumber, int firstWinning,
                                    DisjointUnion winningStates);

    /**
     * Check if winning states are reachable for agent from the given state, when using given action.
     *
     * @param action
     * @param fromState
     * @param isWinningState
     * @param agentNumber
     * @return true if at least one of the winning states is reachable by agent with given action, false otherwise
     */
    bool isReachableByAgent(const std::string& action, int fromState, const bool isWinningState[], int agentNumber);

    /**
     * Check if winning states are reachable for agent from the given state, when using given action.
     *
     * @param action
     * @param fromState
     * @param winningStates
     * @param agentNumber
     * @return true if at least one of the winning states is reachable by agent with given action, false otherwise
     */
    bool isReachableByAgentInSet(const std::string& action, int fromState, std::set<int> winningStates, int agentNumber);

    /**
     * Compute one step of a fixpoint algorithm for singleton coalition under imperfect information
     * using disjoint-union structure.
     *
     * @param agentNumber identifier of the agent in singleton coalition
     * @param currentStates set of identifiers of states in the current computation step
     * @param firstWinning
     * @param winningStatesDisjoint
     * @param customCanGoThere
     * @return
     */
    std::pair<std::set<int>, bool>
    basicFormulaOneAgentMultipleStatesDisjoint(int agentNumber, const std::set<int>& currentStates, int firstWinning,
                                               DisjointUnion &winningStatesDisjoint,
                                               std::vector<std::map<std::string, std::set<int> > > &customCanGoThere);

    /**
     * Compute one step of a fixpoint algorithm for singleton coalition under perfect information.
     *
     * @param agentNumber identifier of the agent in singleton coalition
     * @param currentStates set of identifiers of states in the current step
     * @param isWinningState
     * @return
     */
    std::set<int> basicFormulaOneAgentMultipleStatesPerfectInformation(int agentNumber, const std::set<int>& currentStates,
                                                                       std::vector<bool> &isWinningState);

    /**
     *
     * @param agentNumber
     * @param winningStates
     * @return
     */
    std::set<int> minimumFormulaOneAgentMultipleStatesDisjoint(int agentNumber, const std::set<int> &winningStates);

    /**
     *
     * @param agentNumber
     * @param winningStates
     * @return
     */
    std::set<int> minimumFormulaOneAgentMultipleStatesPerfectInformation(int agentNumber, const std::set<int>& winningStates);

    /**
     *
     * @param numberOfStates
     */
    void setNumberOfStates(int numberOfStates);

    /**
     *
     * @param file
     */
    void saveToFile(std::ofstream &file);

    /**
     *
     * @param file
     * @param imperfect
     */
    void loadFromFile(std::ifstream &file, bool imperfect = true);

    /**
     * Removes all the transitions from the model.
     */
    void clearTransitions();

    /**
     *
     * @return
     */
    const std::set<int> &getWinningStates() const;

    /**
     *
     * @param winningStates
     */
    void setWinningStates(const std::set<int> &winningStates);

    /**
     *
     * @param stateNumber
     * @return
     */
    std::set<Transition> getTransitions(int stateNumber);
};


#endif //ATLFORMULACHECKER_ATLMODEL_H