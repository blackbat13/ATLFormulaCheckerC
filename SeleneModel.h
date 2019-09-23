#ifndef ATLFORMULACHECKER_SELENEMODEL_H
#define ATLFORMULACHECKER_SELENEMODEL_H

#include "AtlModel.h"
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include <random>
#include <queue>
#include <tuple>
#include <cstdio>
#include "cppitertools/itertools.hpp"

class SeleneModel {
public:
    struct CoercerEpistemicState;

    struct State {
        // Environment
        std::vector<short> publicVotes;
        bool votesPublished;
        bool votingFinished;
        bool votingStarted;
        short voteWait;
        short defenseTimer;
        std::vector<short> falseCopTrackVot;
        std::vector<short> trackers;
        std::vector<short> envVoteDemanded;
        std::vector<short> votersOwnedTrackers;
        std::vector<bool> votersTrackersSet;
        std::vector<short> votes;
        std::vector<short> votersVotes;

        // Election Defense System
        std::vector<bool> falseTrackersSent;

        // Voters
        std::vector<short> votersDemanded;
        std::vector<bool> helpRequestsSent;
        std::vector<bool> trueTrackersCopied;

        // Coercer
        short coercedVoters;
        short maxCoerced;
        std::vector<short> coercerVotesDemanded;

        State(short noVoters, short maxCoerced);

        State();

        State(const std::vector<short> &publicVotes, bool votesPublished, bool votingFinished, bool votingStarted,
              const std::vector<short> &falseCopTrackVot, const std::vector<short> &trackers,
              const std::vector<short> &envVoteDemanded, const std::vector<short> &votersOwnedTrackers,
              const std::vector<bool> &votersTrackersSet, const std::vector<short> &votes,
              const std::vector<bool> &falseTrackersSent, const std::vector<short> &votersDemanded,
              const std::vector<bool> &helpRequestsSent, const std::vector<bool> &trueTrackersCopied,
              short coercedVoters, short maxCoerced, const std::vector<short> &coercerVotesDemanded);

        State(State const &copy);

        void print();

        void printVector(std::vector<short> v);

        void printVector(std::vector<bool> v);


        CoercerEpistemicState toCoercerState();

        bool operator<(const State &rhs) const;

        bool operator>(const State &rhs) const;

        bool operator<=(const State &rhs) const;

        bool operator>=(const State &rhs) const;

        bool operator==(const State &rhs) const;

        bool operator!=(const State &rhs) const;
    };

    struct CoercerEpistemicState {
        bool operator<(const CoercerEpistemicState &rhs) const;

        bool operator>(const CoercerEpistemicState &rhs) const;

        bool operator<=(const CoercerEpistemicState &rhs) const;

        bool operator>=(const CoercerEpistemicState &rhs) const;

        bool votesPublished;
        bool votingFinished;
        bool votingStarted;
        std::vector<short> falseCopTrackVot;
        std::vector<short> publicVotes;
        short coercedVoters;
        short maxCoerced;
        short voteWait;
        short defenseTimer;
        std::vector<short> coercerVotesDemanded;
    };

private:
    AtlModel model;
    short noVoters;
    short noBallots;
    short maxCoerced;
    int stateNumber;
    short maxWaitingForVotes;
    short maxWaitingForHelp;
    std::map<State, int> stateToNumber;
    std::map<CoercerEpistemicState, std::set<int> > coercerEpistemicClasses;

    void generateModel();

    int addState(State state);

    std::string toString(short a);

    std::vector<std::vector<short> > cartessianProduct(std::vector<std::vector<short> > &array);

    void addActions();

    void addEpistemicState(State state, int stateNumber);

    void printVector(std::vector<std::string> v);

    void prepareWinningStates();

    void prepareWinningStatesForPercent();

    void finishEpistemicRelation();

public:
    SeleneModel(short noVoters, short noBallots, short maxCoerced, short maxWaitingForVotes, short maxWaitingForHelp);

    void simulate();

    void clear();

    AtlModel &getModel();

    std::vector<State> states;
    std::set<int> formula1WinningStates;
    std::set<int> formula2WinningStates;
    std::set<int> formula3WinningStates;
    std::set<int> formula4WinningStates;
    std::set<int> formula4PercentageWinningStates;
    std::set<int> configurationStates;
};


#endif //ATLFORMULACHECKER_SELENEMODEL_H