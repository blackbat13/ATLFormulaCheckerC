#include "SeleneModel.h"

SeleneModel::State::State(short noVoters, short maxCoerced) {
    this->publicVotes = std::vector<short>(noVoters, -1);
    this->votesPublished = false;
    this->votingFinished = false;
    this->votingStarted = false;
    this->voteWait = 0;
    this->defenseTimer = 0;
    this->falseCopTrackVot = std::vector<short>(noVoters, -1);
    this->trackers = std::vector<short>(noVoters, -1);
    this->envVoteDemanded = std::vector<short>(noVoters, -1);
    this->votersOwnedTrackers = std::vector<short>(noVoters, -1);
    this->votersTrackersSet = std::vector<bool>(noVoters);
    this->votes = std::vector<short>(noVoters, -1);
    this->votersVotes = std::vector<short>(noVoters, -1);

    // Election Defense System
    this->falseTrackersSent = std::vector<bool>(noVoters);

    // Voters
    this->votersDemanded = std::vector<short>(noVoters, -1);
    this->helpRequestsSent = std::vector<bool>(noVoters);
    this->trueTrackersCopied = std::vector<bool>(noVoters);

    // Coercer
    this->coercedVoters = 0;
    this->maxCoerced = maxCoerced;
    this->coercerVotesDemanded = std::vector<short>(noVoters, -1);
}

SeleneModel::State::State() {
    this->publicVotes = std::vector<short>();
    this->votesPublished = false;
    this->votingFinished = false;
    this->votingStarted = false;
    this->voteWait = 0;
    this->defenseTimer = 0;
    this->falseCopTrackVot = std::vector<short>();
    this->trackers = std::vector<short>();
    this->envVoteDemanded = std::vector<short>();
    this->votersOwnedTrackers = std::vector<short>();
    this->votersTrackersSet = std::vector<bool>();
    this->votes = std::vector<short>();
    this->votersVotes = std::vector<short>();

    // Election Defense System
    this->falseTrackersSent = std::vector<bool>();

    // Voters
    this->votersDemanded = std::vector<short>();
    this->helpRequestsSent = std::vector<bool>();
    this->trueTrackersCopied = std::vector<bool>();

    // Coercer
    this->coercedVoters = 0;
    this->maxCoerced = 0;
    this->coercerVotesDemanded = std::vector<short>();
}

SeleneModel::State::State(const std::vector<short> &publicVotes, bool votesPublished, bool votingFinished,
                          bool votingStarted, const std::vector<short> &falseCopTrackVot,
                          const std::vector<short> &trackers, const std::vector<short> &envVoteDemanded,
                          const std::vector<short> &votersOwnedTrackers, const std::vector<bool> &votersTrackersSet,
                          const std::vector<short> &votes, const std::vector<bool> &falseTrackersSent,
                          const std::vector<short> &votersDemanded, const std::vector<bool> &helpRequestsSent,
                          const std::vector<bool> &trueTrackersCopied, short coercedVoters, short maxCoerced,
                          const std::vector<short> &coercerVotesDemanded) : publicVotes(publicVotes),
                                                                            votesPublished(votesPublished),
                                                                            votingFinished(votingFinished),
                                                                            votingStarted(votingStarted),
                                                                            falseCopTrackVot(falseCopTrackVot),
                                                                            trackers(trackers),
                                                                            envVoteDemanded(envVoteDemanded),
                                                                            votersOwnedTrackers(votersOwnedTrackers),
                                                                            votersTrackersSet(votersTrackersSet),
                                                                            votes(votes),
                                                                            falseTrackersSent(falseTrackersSent),
                                                                            votersDemanded(votersDemanded),
                                                                            helpRequestsSent(helpRequestsSent),
                                                                            trueTrackersCopied(trueTrackersCopied),
                                                                            coercedVoters(coercedVoters),
                                                                            maxCoerced(maxCoerced),
                                                                            coercerVotesDemanded(
                                                                                    coercerVotesDemanded) {}

SeleneModel::State::State(const SeleneModel::State &copy) {
    this->publicVotes = copy.publicVotes;
    this->votesPublished = copy.votesPublished;
    this->votingFinished = copy.votingFinished;
    this->votingStarted = copy.votingStarted;
    this->falseCopTrackVot = copy.falseCopTrackVot;
    this->trackers = copy.trackers;
    this->envVoteDemanded = copy.envVoteDemanded;
    this->votersOwnedTrackers = copy.votersOwnedTrackers;
    this->votersTrackersSet = copy.votersTrackersSet;
    this->votes = copy.votes;
    this->votersVotes = copy.votersVotes;
    this->voteWait = copy.voteWait;
    this->defenseTimer = copy.defenseTimer;

    // Election Defense System
    this->falseTrackersSent = copy.falseTrackersSent;

    // Voters
    this->votersDemanded = copy.votersDemanded;
    this->helpRequestsSent = copy.helpRequestsSent;
    this->trueTrackersCopied = copy.trueTrackersCopied;

    // Coercer
    this->coercedVoters = copy.coercedVoters;
    this->maxCoerced = copy.maxCoerced;
    this->coercerVotesDemanded = copy.coercerVotesDemanded;
}

void SeleneModel::State::print() {
    printf("Public Votes: ");
    printVector(this->publicVotes);

    printf("Votes Published: %d\n", this->votesPublished);
    printf("Voting Finished: %d\n", this->votingFinished);
    printf("Voting Started: %d\n", this->votingStarted);

    printf("Vote Wait: %d\n", this->voteWait);
    printf("Defense Timer: %d\n", this->defenseTimer);

    printf("False Copied Tracker for Voters: ");
    printVector(this->falseCopTrackVot);

    printf("Trackers: ");
    printVector(this->trackers);

    printf("Environment Votes Demanded: ");
    printVector(this->envVoteDemanded);

    printf("Voters Owned Trackers: ");
    printVector(this->votersOwnedTrackers);

    printf("Voters Trackers Set: ");
    printVector(this->votersTrackersSet);

    printf("Votes: ");
    printVector(this->votes);

    printf("Voters Votes: ");
    printVector(this->votersVotes);

    printf("False Tracker Sent: ");
    printVector(this->falseTrackersSent);

    printf("Voters Demanded Votes: ");
    printVector(this->votersDemanded);

    printf("Help Request Sent: ");
    printVector(this->helpRequestsSent);

    printf("True Trackers Copied: ");
    printVector(this->trueTrackersCopied);

    printf("Coercer Votes Demanded: ");
    printVector(this->coercerVotesDemanded);

    printf("Coerced Voters: %d\n", this->coercedVoters);

    printf("Max Coerced: %d\n", this->maxCoerced);
}

void SeleneModel::State::printVector(const std::vector<short>& v) {
    for (short i : v) {
        printf("%d ", i);
    }

    printf("\n");
}

void SeleneModel::State::printVector(const std::vector<bool>& v) {
    for (bool i : v) {
        printf("%d ", i);
    }

    printf("\n");
}

SeleneModel::CoercerEpistemicState SeleneModel::State::toCoercerState() {
    CoercerEpistemicState state;
    state.votesPublished = this->votesPublished;
    state.votingFinished = this->votingFinished;
    state.votingStarted = this->votingStarted;
    state.falseCopTrackVot = this->falseCopTrackVot;
    state.publicVotes = this->publicVotes;
    state.coercedVoters = this->coercedVoters;
    state.maxCoerced = this->maxCoerced;
    state.coercerVotesDemanded = this->coercerVotesDemanded;
    state.voteWait = this->voteWait;
    state.defenseTimer = this->defenseTimer;
    return state;
}

bool SeleneModel::State::operator<(const SeleneModel::State &rhs) const {
    if (publicVotes < rhs.publicVotes)
        return true;
    if (rhs.publicVotes < publicVotes)
        return false;
    if (votesPublished < rhs.votesPublished)
        return true;
    if (rhs.votesPublished < votesPublished)
        return false;
    if (votingFinished < rhs.votingFinished)
        return true;
    if (rhs.votingFinished < votingFinished)
        return false;
    if (votingStarted < rhs.votingStarted)
        return true;
    if (rhs.votingStarted < votingStarted)
        return false;
    if (voteWait < rhs.voteWait)
        return true;
    if (rhs.voteWait < voteWait)
        return false;
    if (defenseTimer < rhs.defenseTimer)
        return true;
    if (rhs.defenseTimer < defenseTimer)
        return false;
    if (falseCopTrackVot < rhs.falseCopTrackVot)
        return true;
    if (rhs.falseCopTrackVot < falseCopTrackVot)
        return false;
    if (trackers < rhs.trackers)
        return true;
    if (rhs.trackers < trackers)
        return false;
    if (envVoteDemanded < rhs.envVoteDemanded)
        return true;
    if (rhs.envVoteDemanded < envVoteDemanded)
        return false;
    if (votersOwnedTrackers < rhs.votersOwnedTrackers)
        return true;
    if (rhs.votersOwnedTrackers < votersOwnedTrackers)
        return false;
    if (votersTrackersSet < rhs.votersTrackersSet)
        return true;
    if (rhs.votersTrackersSet < votersTrackersSet)
        return false;
    if (votes < rhs.votes)
        return true;
    if (rhs.votes < votes)
        return false;
    if (falseTrackersSent < rhs.falseTrackersSent)
        return true;
    if (rhs.falseTrackersSent < falseTrackersSent)
        return false;
    if (votersDemanded < rhs.votersDemanded)
        return true;
    if (rhs.votersDemanded < votersDemanded)
        return false;
    if (helpRequestsSent < rhs.helpRequestsSent)
        return true;
    if (rhs.helpRequestsSent < helpRequestsSent)
        return false;
    if (trueTrackersCopied < rhs.trueTrackersCopied)
        return true;
    if (rhs.trueTrackersCopied < trueTrackersCopied)
        return false;
    if (coercedVoters < rhs.coercedVoters)
        return true;
    if (rhs.coercedVoters < coercedVoters)
        return false;
    if (maxCoerced < rhs.maxCoerced)
        return true;
    if (rhs.maxCoerced < maxCoerced)
        return false;
    if (votersVotes < rhs.votersVotes)
        return true;
    if (rhs.votersVotes < votersVotes)
        return false;
    return coercerVotesDemanded < rhs.coercerVotesDemanded;
}

bool SeleneModel::State::operator>(const SeleneModel::State &rhs) const {
    return rhs < *this;
}

bool SeleneModel::State::operator<=(const SeleneModel::State &rhs) const {
    return (*this == rhs) || (*this < rhs);
//    return !(rhs < *this);
}

bool SeleneModel::State::operator>=(const SeleneModel::State &rhs) const {
    return (*this == rhs) || (*this > rhs);
//    return !(*this < rhs);
}

bool SeleneModel::State::operator==(const SeleneModel::State &rhs) const {
    return publicVotes == rhs.publicVotes &&
           votesPublished == rhs.votesPublished &&
           votingFinished == rhs.votingFinished &&
           votingStarted == rhs.votingStarted &&
           voteWait == rhs.voteWait &&
           defenseTimer == rhs.defenseTimer &&
           falseCopTrackVot == rhs.falseCopTrackVot &&
           trackers == rhs.trackers &&
           envVoteDemanded == rhs.envVoteDemanded &&
           votersOwnedTrackers == rhs.votersOwnedTrackers &&
           votersTrackersSet == rhs.votersTrackersSet &&
           votes == rhs.votes &&
           votersVotes == rhs.votersVotes &&
           falseTrackersSent == rhs.falseTrackersSent &&
           votersDemanded == rhs.votersDemanded &&
           helpRequestsSent == rhs.helpRequestsSent &&
           trueTrackersCopied == rhs.trueTrackersCopied &&
           coercedVoters == rhs.coercedVoters &&
           maxCoerced == rhs.maxCoerced &&
           coercerVotesDemanded == rhs.coercerVotesDemanded;
}

bool SeleneModel::State::operator!=(const SeleneModel::State &rhs) const {
    return !(rhs == *this);
}

SeleneModel::SeleneModel(short noVoters, short noBallots, short maxCoerced, short maxWaitingForVotes,
                         short maxWaitingForHelp) : noVoters(noVoters), noBallots(noBallots),
                                                    maxCoerced(maxCoerced), maxWaitingForVotes(maxWaitingForVotes),
                                                    maxWaitingForHelp(maxWaitingForHelp) {
    this->model = AtlModel(2, 1000000);
    this->states = std::vector<State>();
    this->stateNumber = 0;
    this->addActions();
    this->generateModel();
    printf("Number of states: %zu\n", this->states.size());
    printf("Number of states: %u\n", this->stateToNumber.size());
    this->finishEpistemicRelation();
    this->prepareWinningStates();
    this->prepareWinningStatesForPercent();
}

void SeleneModel::generateModel() {
    State firstState = State(this->noVoters, this->maxCoerced);
    this->addState(firstState);
    int currentStateNumber = -1;
    for (auto state : this->states) {
        ++currentStateNumber;
        if (!state.votingStarted) {
            std::vector<std::pair<short, short> > envActions;
            std::vector<std::pair<short, short> > coercerActions;
            coercerActions.emplace_back(-1, -1); // Wait action

            for (int i = 0; i < this->noVoters; ++i) {
                if (state.trackers[i] != -1) {
                    continue;
                }
                for (int j = 0; j < this->noVoters; ++j) {
                    if (state.votersTrackersSet[j]) {
                        continue;
                    }
                    envActions.emplace_back(i, j);
                }
            }

            if (state.coercedVoters < state.maxCoerced) {
                for (int i = 0; i < this->noVoters; ++i) {
                    if (state.coercerVotesDemanded[i] != -1) {
                        continue;
                    }

                    for (int j = 0; j < this->noBallots; ++j) {
                        coercerActions.emplace_back(i, j);
                    }
                }
            }


            if (envActions.empty()) { // Start Voting

                for (auto &coercerAction: coercerActions) {
                    State newState = state;
                    if (coercerAction.first != -1) { // Coerce voter
                        newState.coercedVoters++;
                        newState.coercerVotesDemanded[coercerAction.first] = coercerAction.second;
                        newState.votersDemanded[coercerAction.first] = coercerAction.second;
                        newState.votingStarted = true;
                        assert(newState.votersDemanded[coercerAction.first] !=
                               state.votersDemanded[coercerAction.first]);
                        std::vector<std::string> actions(1, "RequestVote" + this->toString(coercerAction.second) +
                                                            "FromVoter" + this->toString(coercerAction.first));
                        for (int i = 0; i < this->noVoters; ++i) {
                            actions.emplace_back("Wait");
                        }

                        actions.emplace_back("Wait"); //ElectionDefense
                        actions.emplace_back("StartVoting");

                        int newStateNumber = this->addState(newState);


                        assert(currentStateNumber != newStateNumber);
                        this->model.addTransition(currentStateNumber, newStateNumber, actions);
                    } else {
                        newState.votingStarted = true;
                        std::vector<std::string> actions(1, "Wait");
                        for (int i = 0; i < this->noVoters; ++i) {
                            actions.emplace_back("Wait");
                        }

                        actions.emplace_back("Wait"); //ElectionDefense
                        actions.emplace_back("StartVoting");

                        int newStateNumber = this->addState(newState);

                        assert(currentStateNumber != newStateNumber);
                        this->model.addTransition(currentStateNumber, newStateNumber, actions);
                    }
                }
            } else { // Setting Trackers
                for (auto &envAction: envActions) {
                    for (auto &coercerAction: coercerActions) {
                        State newState = state;
                        if (coercerAction.first != -1) { // Coerce voter
                            newState.coercedVoters++;
                            newState.coercerVotesDemanded[coercerAction.first] = coercerAction.second;
                            newState.votersDemanded[coercerAction.first] = coercerAction.second;
                            newState.trackers[envAction.first] = envAction.second;
                            newState.votersTrackersSet[envAction.second] = true;


                            std::vector<std::string> actions(1, "RequestVote" + this->toString(coercerAction.second) +
                                                                "FromVoter" + this->toString(coercerAction.first));
                            for (int i = 0; i < this->noVoters; ++i) {
                                actions.emplace_back("Wait");
                            }

                            actions.emplace_back("Wait"); //ElectionDefense

                            actions.push_back("SetTracker" + this->toString(envAction.first) + "To" +
                                              this->toString(envAction.second));

                            int newStateNumber = this->addState(newState);


                            assert(currentStateNumber != newStateNumber);
                            this->model.addTransition(currentStateNumber, newStateNumber, actions);
                        } else {
                            newState.trackers[envAction.first] = envAction.second;
                            newState.votersTrackersSet[envAction.second] = true;
                            std::vector<std::string> actions(1, "Wait");

                            for (int i = 0; i < this->noVoters; ++i) {
                                actions.emplace_back("Wait");
                            }

                            actions.emplace_back("Wait");
                            actions.push_back("SetTracker" + this->toString(envAction.first) + "To" +
                                              this->toString(envAction.second));

                            int newStateNumber = this->addState(State(newState));


                            assert(currentStateNumber != newStateNumber);
                            this->model.addTransition(currentStateNumber, newStateNumber, actions);
                        }
                    }
                }

            }
        } else if (!state.votesPublished) {
            std::vector<std::pair<short, short> > coercerActions;
            coercerActions.emplace_back(-1, -1); // Wait action
            if (state.coercedVoters < state.maxCoerced) {
                for (int i = 0; i < this->noVoters; ++i) {
                    if (state.coercerVotesDemanded[i] != -1) {
                        continue;
                    }

                    for (int j = 0; j < this->noBallots; ++j) {
                        coercerActions.emplace_back(i, j);
                    }
                }
            }

            std::vector<std::vector<short> > votingProductArray(this->noVoters);
            bool allVoted = true;
            for (int i = 0; i < this->noVoters; ++i) {
                votingProductArray[i].push_back(-1);
                if (state.votersVotes[i] != -1) {
                    continue;
                }

                allVoted = false;
                for (int j = 0; j < this->noBallots; ++j) {
                    votingProductArray[i].push_back(j);
                }
            }

            if (allVoted) { //Publish Votes
                for (auto &coercerAction: coercerActions) {
                    State newState = state;
                    newState.votesPublished = true;
                    newState.voteWait += 1;
                    for (int i = 0; i < this->noVoters; ++i) {
                        newState.publicVotes[i] = newState.votersVotes[newState.trackers[i]]; //According to trackers
                    }

                    if (coercerAction.first != -1) { // Coerce voter
                        newState.coercedVoters++;
                        newState.coercerVotesDemanded[coercerAction.first] = coercerAction.second;
                        newState.votersDemanded[coercerAction.first] = coercerAction.second;
                        std::vector<std::string> actions(1, "RequestVote" + this->toString(coercerAction.second) +
                                                            "FromVoter" + this->toString(coercerAction.first));
                        for (int i = 0; i < this->noVoters; ++i) {
                            actions.push_back("Wait");
                        }

                        actions.push_back("Wait"); //ElectionDefense
                        actions.push_back("PublishVotes");
                        int newStateNumber = this->addState(newState);

                        this->configurationStates.insert(newStateNumber);


                        assert(currentStateNumber != newStateNumber);
                        this->model.addTransition(currentStateNumber, newStateNumber, actions);
                    } else {
                        std::vector<std::string> actions(1, "Wait");
                        for (int i = 0; i < this->noVoters; ++i) {
                            actions.push_back("Wait");
                        }

                        actions.push_back("Wait"); //ElectionDefense
                        actions.push_back("PublishVotes");

                        int newStateNumber = this->addState(newState);


                        this->configurationStates.insert(newStateNumber);

                        assert(currentStateNumber != newStateNumber);
                        this->model.addTransition(currentStateNumber, newStateNumber, actions);
                    }
                }
            } else {
                std::vector<std::vector<short> > product = this->cartessianProduct(votingProductArray);
                for (auto &possibility: product) {
                    State votedState = state;
                    votedState.voteWait += 1;

                    bool skipState = false;

                    for (int i = 0; i < this->noVoters; ++i) {
                        if (possibility[votedState.trackers[i]] != -1) {
                            votedState.votes[i] = possibility[votedState.trackers[i]];
                            votedState.votersVotes[votedState.trackers[i]] = possibility[votedState.trackers[i]];
                        } else if (votedState.votersVotes[votedState.trackers[i]] == -1 &&
                                   possibility[votedState.trackers[i]] == -1 &&
                                   votedState.voteWait > this->maxWaitingForVotes) {
                            skipState = true;
                            break;
                        }
                    }

                    if (skipState) {
                        continue;
                    }

                    for (auto &coercerAction: coercerActions) {
                        State newState = votedState;

                        if (coercerAction.first != -1) { // Coerce voter
                            newState.coercedVoters++;
                            newState.coercerVotesDemanded[coercerAction.first] = coercerAction.second;
                            newState.votersDemanded[coercerAction.first] = coercerAction.second;
                            std::vector<std::string> actions(1, "RequestVote" + this->toString(coercerAction.second) +
                                                                "FromVoter" + this->toString(coercerAction.first));

                            for (int i = 0; i < this->noVoters; ++i) {
                                if (possibility[i] == -1) {
                                    actions.push_back("Wait");
                                } else {
                                    actions.push_back("Vote" + this->toString(possibility[i]));
                                }
                            }

                            actions.push_back("Wait"); //ElectionDefense
                            actions.push_back("Wait");

                            int newStateNumber = this->addState(newState);


                            assert(currentStateNumber != newStateNumber);
                            this->model.addTransition(currentStateNumber, newStateNumber, actions);
                        } else {
                            std::vector<std::string> actions(1, "Wait");
                            for (int i = 0; i < this->noVoters; ++i) {
                                if (possibility[i] == -1) {
                                    actions.push_back("Wait");
                                } else {
                                    actions.push_back("Vote" + this->toString(possibility[i]));
                                }
                            }

                            actions.push_back("Wait"); //ElectionDefense
                            actions.push_back("Wait");

                            int newStateNumber = this->addState(newState);


                            assert(currentStateNumber != newStateNumber);
                            this->model.addTransition(currentStateNumber, newStateNumber, actions);
                        }
                    }
                }
            }
        } else if (!state.votingFinished) {
            std::vector<std::pair<short, short> > coercerActions;
            coercerActions.push_back(std::make_pair(-1, -1)); // Wait action

            std::vector<std::pair<short, short> > defenseActions;
            defenseActions.push_back(std::make_pair(-1, -1)); // Wait action
            for (int i = 0; i < this->noVoters; ++i) {
                if (state.helpRequestsSent[i] && !state.falseTrackersSent[i]) {
                    for (int j = 0; j < this->noVoters; ++j) {
                        defenseActions.push_back(std::make_pair(i, j));
                    }
                }
            }

            std::vector<std::vector<short> > votingProductArray(this->noVoters);
            for (int i = 0; i < this->noVoters; ++i) {
                if (state.defenseTimer >= this->maxWaitingForHelp) {
                    votingProductArray[i].push_back(-4);
                    continue;
                }

                votingProductArray[i].push_back(-1); // Wait
                if (state.helpRequestsSent[i] == false) {
                    for (int j = 0; j < this->noBallots; ++j) {
                        votingProductArray[i].push_back(j); // Help I need vote for j
                    }
                }

                if (state.votersOwnedTrackers[i] == -1) {
                    votingProductArray[i].push_back(-2); // Fetch Good Tracker
                } else if (!state.trueTrackersCopied[i]) {
                    votingProductArray[i].push_back(-3); // Copy Real Tracker
                }

                if (state.falseCopTrackVot[i] > -1) {
                    votingProductArray[i].push_back(-4); // Finish
                }
            }

            std::vector<std::vector<short> > product = this->cartessianProduct(votingProductArray);
            if (state.defenseTimer >= this->maxWaitingForHelp) {
                assert(product.size() == 1);
            }

            for (auto &possibility: product) {
                for (auto &defenseAction: defenseActions) {
                    for (auto &coercerAction: coercerActions) {
                        State newState = state;
                        newState.defenseTimer += 1;

                        std::vector<std::string> actions;

                        actions = std::vector<std::string>(1, "Wait");


                        int finished = 0;
                        for (int i = 0; i < this->noVoters; ++i) {
                            if (possibility[i] == -1) { //Wait
                                actions.push_back("Wait");
                            } else if (possibility[i] == -2) { // Fetch Good Tracker
                                short goodTracker = -1;
                                for (int j = 0; j < this->noVoters; ++j) {
                                    if (state.trackers[j] == i) {
                                        goodTracker = j;
                                        break;
                                    }
                                }

                                actions.push_back("FetchGoodTracker");

                                newState.votersOwnedTrackers[i] = goodTracker;

                            } else if (possibility[i] == -3) { // Copy Real Tracker
                                actions.push_back("CopyRealTracker");
                                newState.trueTrackersCopied[i] = true;
                                newState.falseCopTrackVot[i] = state.votersOwnedTrackers[i];
                            } else if (possibility[i] == -4) { // Finish
                                actions.push_back("Finish");
                                ++finished;
                            } else { // Help I Need possibility[i]
                                actions.push_back("HelpINeed" + this->toString(possibility[i]));
                                newState.helpRequestsSent[i] = true;
                                newState.envVoteDemanded[i] = possibility[i];
                            }
                        }

                        if (finished == this->noVoters) {
                            newState.votingFinished = true;
                        }

                        if (defenseAction.first != -1) {
                            newState.falseTrackersSent[defenseAction.first] = true;
                            if (possibility[defenseAction.first] != -3) {
                                newState.falseCopTrackVot[defenseAction.first] = defenseAction.second;
                            }

                            actions.push_back("SetFalseTrackerForVoter" + this->toString(defenseAction.first) + "To" +
                                              this->toString(defenseAction.second));
                        } else {
                            actions.push_back("Wait");
                        }

                        actions.push_back("Wait");


                        int newStateNumber = this->addState(newState);


                        assert(currentStateNumber != newStateNumber);
                        this->model.addTransition(currentStateNumber, newStateNumber, actions);
                    }
                }
            }
        } else {

            std::vector<std::string> actions(1, "Wait");

            for (int i = 0; i < this->noVoters; ++i) {
                actions.push_back("Wait");
            }

            actions.push_back("Wait"); //ElectionDefense
            actions.push_back("Wait");

            this->model.addTransition(currentStateNumber, currentStateNumber, actions);
        }
    }
}

int SeleneModel::addState(const SeleneModel::State& state) {
    if (this->stateToNumber[state] == 0) {
        this->stateToNumber[state] = this->stateNumber;

        this->states.push_back(state);
        assert(this->states.size() - 1 == this->stateNumber);
        this->addEpistemicState(state, this->stateNumber);
        return this->stateNumber++;
    } else {
        return this->stateToNumber[state];
    }
}

std::string SeleneModel::toString(short a) {
    std::string str;
    if (a == 0) {
        return "0";
    }

    while (a > 0) {
        str = char((a % 10) + '0') + str;
        a /= 10;
    }

    return str;
}

std::vector<std::vector<short> > SeleneModel::cartessianProduct(std::vector<std::vector<short> > &array) {
    std::vector<std::vector<short> > result;
    std::vector<int> it(array.size());
    while (true) {
        std::vector<short> product(array.size());
        for (unsigned int i = 0; i < array.size(); ++i) {
            product[i] = array[i][it[i]];
        }

        result.push_back(product);
        int i = 0;
        it[i]++;
        while (i < array.size() && it[i] >= array[i].size()) {
            it[i] = 0;
            i++;
            if (i >= array.size()) {
                break;
            }

            it[i]++;
        }

        if (i >= array.size()) {
            break;
        }
    }

    return result;
}

AtlModel &SeleneModel::getModel() {
    return this->model;
}

void SeleneModel::addActions() {
    this->model.addAction(0, "Wait");
    this->model.addAction(1, "Wait");
    for (short i = 0; i < this->noVoters; ++i) {
        for (short j = 0; j < this->noBallots; ++j) {
            std::string action = "RequestVote" + this->toString(j);
            action += "FromVoter" + this->toString(i);
            printf("%s\n", action.c_str());
            this->model.addAction(0, action);
        }
    }

    for (short i = 0; i < this->noBallots; ++i) {
        this->model.addAction(1, "Vote" + this->toString(i));
        this->model.addAction(1, "HelpINeed" + this->toString(i));
    }

    this->model.addAction(1, "FetchGoodTracker");
    this->model.addAction(1, "CopyRealTracker");
    this->model.addAction(1, "Finish");
}

void SeleneModel::addEpistemicState(SeleneModel::State state, int stateNumber) {
    CoercerEpistemicState coercerEpistemicState = state.toCoercerState();
    this->coercerEpistemicClasses[coercerEpistemicState].insert(stateNumber);
}

void SeleneModel::simulate() {
    int currentState = 0;
    int choice = 0;
    while (true) {
        this->states[currentState].print();
        int i = 0;

        std::vector<Transition> transitions;
        for (const auto& t : this->model.getTransitions(currentState)) {
            printf("%d: NextState: %d ", i, t.nextState);
            this->printVector(t.actions);
            transitions.push_back(t);
            ++i;
        }

        printf("Your choice: ");
        scanf("%d", &choice);
        if (choice == -1) {
            return;
        }

        currentState = transitions[choice].nextState;
    }

}

void SeleneModel::printVector(const std::vector<std::string>& v) {
    for (const std::string& i : v) {
        printf("%s ", i.c_str());
    }

    printf("\n");
}

void SeleneModel::prepareWinningStates() {
    this->formula1WinningStates = std::set<int>();
    this->formula2WinningStates = std::set<int>();
    for (unsigned int i = 0; i < this->states.size(); ++i) {
        assert(!this->model.getTransitions(i).empty());

        auto state = this->states[i];

        bool allNotVoted1 = true;
        if (state.votingFinished) {
            for (int v = 0; v < this->noVoters; ++v) {
                if (state.votes[v] == 0) {
                    allNotVoted1 = false;
                    break;
                }
            }

            if (!allNotVoted1) {
                this->formula1WinningStates.insert(i);
                this->formula2WinningStates.insert(i);
                continue;
            }

            auto coercerState = state.toCoercerState();
            bool isAllOk = true;
            for (auto stNumber: this->coercerEpistemicClasses[coercerState]) {
                auto state2 = this->states[stNumber];
                bool isOk = false;
                for (int v = 0; v < this->noVoters; ++v) {
                    if (state2.votersVotes[v] != 0) {
                        isOk = true;
                        break;
                    }
                }

                if (!isOk) {
                    isAllOk = false;
                    break;
                }
            }

            if (isAllOk) {
                this->formula1WinningStates.insert(i);
            }

            for (int v = 0; v < this->noVoters; ++v) {
                bool isOk = true;
                for (auto stNumber: this->coercerEpistemicClasses[coercerState]) {
                    auto state2 = this->states[stNumber];
                    if (state2.votersVotes[v] == 0) {
                        isOk = false;
                        break;
                    }
                }
                if (isOk) {
                    this->formula2WinningStates.insert(i);
                    break;
                }
            }
        }
    }

    this->formula3WinningStates = std::set<int>();
    this->formula4WinningStates = std::set<int>();
    for (unsigned int i = 0; i < this->states.size(); ++i) {
        auto state = this->states[i];
        bool someNotVoted1 = false;
        if (state.votingFinished) {
            for (int v = 0; v < this->noVoters; ++v) {
                if (state.votes[v] != 0) {
                    someNotVoted1 = true;
                    break;
                }
            }

            if (!someNotVoted1) {
                this->formula3WinningStates.insert(i);
                this->formula4WinningStates.insert(i);
                continue;
            }

            auto coercerState = state.toCoercerState();
            bool isAllOk = true;
            for (auto stNumber: this->coercerEpistemicClasses[coercerState]) {
                auto state2 = this->states[stNumber];
                bool isOk = false;
                for (int v = 0; v < this->noVoters; ++v) {
                    if (state2.votersVotes[v] != 0) {
                        isOk = true;
                        break;
                    }
                }

                if (!isOk) {
                    isAllOk = false;
                    break;
                }
            }

            if (isAllOk) {
                this->formula3WinningStates.insert(i);
            }

            for (int v = 0; v < this->noVoters; ++v) {
                bool isOk = true;
                for (auto stNumber: this->coercerEpistemicClasses[coercerState]) {
                    auto state2 = this->states[stNumber];
                    if (state2.votersVotes[v] == 0) {
                        isOk = false;
                        break;
                    }
                }
                if (isOk) {
                    this->formula4WinningStates.insert(i);
                    break;
                }
            }
        }
    }
}

void SeleneModel::finishEpistemicRelation() {
    for (const auto& s: this->coercerEpistemicClasses) {
        this->model.addEpistemicClass(0, s.second);
    }
}

void SeleneModel::clear() {
    this->states.clear();
    this->stateToNumber.clear();
    this->coercerEpistemicClasses.clear();
}

void SeleneModel::prepareWinningStatesForPercent() {
    this->formula4PercentageWinningStates = std::set<int>();
    for (unsigned int i = 0; i < this->states.size(); ++i) {
        auto state = this->states[i];
        bool someNotVoted1 = false;
        if (state.votingFinished) {
            for (int v = 0; v < this->noVoters; ++v) {
                if (state.votes[v] != 0) {
                    someNotVoted1 = true;
                    break;
                }
            }

            if (!someNotVoted1) {
                continue;
            }

            auto coercerState = state.toCoercerState();

            for (int v = 0; v < this->noVoters; ++v) {
                bool isOk = true;
                for (auto stNumber: this->coercerEpistemicClasses[coercerState]) {
                    auto state2 = this->states[stNumber];
                    if (state2.votersVotes[v] == 0) {
                        isOk = false;
                        break;
                    }
                }
                if (isOk) {
                    this->formula4PercentageWinningStates.insert(i);
                    break;
                }
            }
        }
    }
}

bool SeleneModel::CoercerEpistemicState::operator<(const SeleneModel::CoercerEpistemicState &rhs) const {
    if (votesPublished < rhs.votesPublished)
        return true;
    if (rhs.votesPublished < votesPublished)
        return false;
    if (votingFinished < rhs.votingFinished)
        return true;
    if (rhs.votingFinished < votingFinished)
        return false;
    if (votingStarted < rhs.votingStarted)
        return true;
    if (rhs.votingStarted < votingStarted)
        return false;
    if (falseCopTrackVot < rhs.falseCopTrackVot)
        return true;
    if (rhs.falseCopTrackVot < falseCopTrackVot)
        return false;
    if (publicVotes < rhs.publicVotes)
        return true;
    if (rhs.publicVotes < publicVotes)
        return false;
    if (coercedVoters < rhs.coercedVoters)
        return true;
    if (rhs.coercedVoters < coercedVoters)
        return false;
    if (maxCoerced < rhs.maxCoerced)
        return true;
    if (rhs.maxCoerced < maxCoerced)
        return false;
    if (voteWait < rhs.voteWait)
        return true;
    if (rhs.voteWait < voteWait)
        return false;
    if (defenseTimer < rhs.defenseTimer)
        return true;
    if (rhs.defenseTimer < defenseTimer)
        return false;
    return coercerVotesDemanded < rhs.coercerVotesDemanded;
}

bool SeleneModel::CoercerEpistemicState::operator>(const SeleneModel::CoercerEpistemicState &rhs) const {
    return rhs < *this;
}

bool SeleneModel::CoercerEpistemicState::operator<=(const SeleneModel::CoercerEpistemicState &rhs) const {
    return !(rhs < *this);
}

bool SeleneModel::CoercerEpistemicState::operator>=(const SeleneModel::CoercerEpistemicState &rhs) const {
    return !(*this < rhs);
}