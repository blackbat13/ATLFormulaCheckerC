//
// Created by Damian Kurpiewski
//

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
    struct State{
        // Environment
        std::vector<short> publicVotes;
        bool votesPublished;
        bool votingFinished;
        bool votingStarted;
        std::vector<short> falseCopTrackVot;
        std::vector<short> trackers;
        std::vector<short> envVoteDemanded;
        std::vector<short> votersOwnedTrackers;
        std::vector<bool> votersTrackersSet;
        std::vector<short> votes;

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

        State(State const& copy);

        void print();

        void printVector(std::vector<short> v);

        void printVector(std::vector<bool> v);

        bool operator<(const State &rhs) const;

        bool operator>(const State &rhs) const;

        bool operator<=(const State &rhs) const;

        bool operator>=(const State &rhs) const;
    };
private:
    AtlModel model;
    short noVoters;
    short noBallots;
    short maxCoerced;
    int stateNumber;
    std::map<State, int> stateToNumber;
    std::vector<State> states;
    void generateModel();
    int addState(State state);
    std::string toString(short a);
    std::vector<std::vector<short> > cartessianProduct(std::vector<std::vector<short> > &array);

public:
    SeleneModel(short noVoters, short noBallots, short maxCoerced);
    AtlModel &getModel();
};


#endif //ATLFORMULACHECKER_SELENEMODEL_H
