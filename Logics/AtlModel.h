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
#include "../Models/ParallelModel.hpp"

class AtlModel {
private:

    /// Number of agents in the model.
    unsigned short numberOfAgents;

    /// Number of states in the model.
    unsigned int numberOfStates;

    /// Number of transitions in the model.
    unsigned int numberOfTransitions;

    /// Vector of transitions. For each state identifier store set of transitions going out from that state.
    std::vector<std::set<Transition> > transitions;

    std::vector<std::set<unsigned int> > preStates;

    /// Vector of agents actions. For each agent identifier store vector of actions available for that agent.
    std::vector<std::vector<std::string> > agentsActions;

    /// For each state store identifier of the epistemic class to which that state belongs.
    std::vector<std::vector<unsigned int> > epistemicClassMembership;

    std::vector<DisjointUnion> epistemicClassDisjoint;

    std::vector<std::vector<std::map<std::string, std::set<unsigned int> > > > canGoThere;

    std::set<unsigned int> winningStates;

    /// Number of initial states
    unsigned int initialStatesCount;
public:

    ParallelModel toParallelModel(int agentId);

    unsigned long getInitialStatesCount() const;

    void setInitialStatesCount(unsigned int initialStatesCount);

public:
    std::vector<std::vector<std::set<unsigned int> > > imperfectInformation;

    AtlModel(unsigned short numberOfAgents, unsigned int numberOfStates);

    AtlModel();

    /**
     * Add action for a given agent.
     *
     * @param agentNumber agent id
     * @param action action to add
     */
    void addAction(unsigned short agentNumber, const std::string& action);

    /**
     * Add transition to the model.
     *
     * @param from initial state identifier
     * @param to end state identifier
     * @param actions vecotr of agents actions
     */
    void addTransition(unsigned int from, unsigned int to, const std::vector<std::string>& actions);

    /**
     * Add epistemic class.
     *
     * @param agentNumber agent identifier
     * @param epistemicClass set of state identifiers in the given epistemic class
     */
    void addEpistemicClass(unsigned short agentNumber, const std::set<unsigned int>& epistemicClass);

    /**
     * Finish creation of the epistemic classes for given agent.
     * Should be called after adding all epistemic classes for given agent.
     *
     * @param agentNumber agent identifier
     */
    void finishEpistemicClasses(unsigned short agentNumber);

    /**
     * Precomputate relations between epistemic classes.
     *
     * @param epistemicClass
     * @param epistemicClassNumber
     * @param agentNumber agent identifier
     */
    void findWhereCanGo(const std::set<unsigned int>& epistemicClass, unsigned int epistemicClassNumber, unsigned short agentNumber);

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
    bool isReachableByAgentDisjoint(const std::string& action, unsigned int fromState, unsigned short agentNumber, unsigned int firstWinning,
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
    bool isReachableByAgent(const std::string& action, unsigned int fromState, const bool isWinningState[], unsigned short agentNumber);

    /**
     * Check if winning states are reachable for agent from the given state, when using given action.
     *
     * @param action
     * @param fromState
     * @param winningStates
     * @param agentNumber
     * @return true if at least one of the winning states is reachable by agent with given action, false otherwise
     */
    bool isReachableByAgentInSet(const std::string& action, unsigned int fromState, std::set<unsigned int> winningStates, unsigned short agentNumber);

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
    std::pair<std::set<unsigned int>, bool>
    basicFormulaOneAgentMultipleStatesDisjoint(unsigned short agentNumber, const std::set<unsigned int>& currentStates, unsigned int firstWinning,
                                               DisjointUnion &winningStatesDisjoint,
                                               std::vector<std::map<std::string, std::set<unsigned int> > > &customCanGoThere);

    /**
     * Compute one step of a fixpoint algorithm for singleton coalition under perfect information.
     *
     * @param agentNumber identifier of the agent in singleton coalition
     * @param currentStates set of identifiers of states in the current step
     * @param isWinningState
     * @return
     */
    std::set<unsigned int> basicFormulaOneAgentMultipleStatesPerfectInformation(unsigned short agentNumber, const std::set<unsigned int>& currentStates,
                                                                       std::vector<bool> &isWinningState);

    /**
     *
     * @param agentNumber
     * @param winningStates
     * @return
     */
    std::set<unsigned int> minimumFormulaOneAgentMultipleStatesDisjoint(unsigned short agentNumber, const std::set<unsigned int> &winningStates);

    /**
     *
     * @param agentNumber
     * @param winningStates
     * @return
     */
    std::set<unsigned int> minimumFormulaOneAgentMultipleStatesPerfectInformation(unsigned short agentNumber, const std::set<unsigned int>& winningStates);

    /**
     *
     * @param numberOfStates
     */
    void setNumberOfStates(unsigned int numberOfStates);

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
    const std::set<unsigned int> &getWinningStates() const;

    /**
     *
     * @param winningStates
     */
    void setWinningStates(const std::set<unsigned int> &winningStates);

    /**
     *
     * @param stateNumber
     * @return
     */
    std::set<Transition> getTransitions(unsigned int stateNumber);
};


#endif //ATLFORMULACHECKER_ATLMODEL_H