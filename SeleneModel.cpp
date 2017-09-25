//
// Created by Damian Kurpiewski
//

#include "SeleneModel.h"

SeleneModel::State::State(short noVoters, short maxCoerced) {
    this->publicVotes = std::vector<short>(noVoters, -1);
    this->votesPublished = false;
    this->votingFinished = false;
    this->votingStarted = false;
    this->falseCopTrackVot = std::vector<short>(noVoters, -1);
    this->trackers = std::vector<short>(noVoters, -1);
    this->envVoteDemanded = std::vector<short>(noVoters, -1);
    this->votersOwnedTrackers = std::vector<short>(noVoters, -1);
    this->votersTrackersSet = std::vector<bool>(noVoters);
    this->votes = std::vector<short>(noVoters, -1);

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
    this->falseCopTrackVot = std::vector<short>();
    this->trackers = std::vector<short>();
    this->envVoteDemanded = std::vector<short>();
    this->votersOwnedTrackers = std::vector<short>();
    this->votersTrackersSet = std::vector<bool>();
    this->votes = std::vector<short>();

    // Election Defense System
    this->falseTrackersSent = std::vector<bool>();

    // Voters
    this->votersDemanded = std::vector<short>();
    this->helpRequestsSent = std::vector<bool>();
    this->trueTrackersCopied = std::vector<bool>();

    // Coercer
    this->coercedVoters = 0;
    this->maxCoerced = maxCoerced;
    this->coercerVotesDemanded = std::vector<short>();
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
    return coercerVotesDemanded < rhs.coercerVotesDemanded;
}

bool SeleneModel::State::operator>(const SeleneModel::State &rhs) const {
    return rhs < *this;
}

bool SeleneModel::State::operator<=(const SeleneModel::State &rhs) const {
    return !(rhs < *this);
}

bool SeleneModel::State::operator>=(const SeleneModel::State &rhs) const {
    return !(*this < rhs);
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

void SeleneModel::State::printVector(std::vector<short> v) {
    for (short i : v) {
        printf("%d ", i);
    }

    printf("\n");
}

void SeleneModel::State::printVector(std::vector<bool> v) {
    for (bool i : v) {
        printf("%d ", i);
    }

    printf("\n");
}

SeleneModel::SeleneModel(short noVoters, short noBallots, short maxCoerced) : noVoters(noVoters), noBallots(noBallots),
                                                                              maxCoerced(maxCoerced) {
    this->model = AtlModel(2, 1000000);
    this->states = std::vector<State>();
    this->stateNumber = 1;
    this->addActions();
    this->generateModel();
    printf("Number of states: %d\n", this->states.size());

//    for(State state: this->states) {
//        state.print();
//
//        printf("\n\n");
//    }
}

void SeleneModel::generateModel() {
    State firstState = State(this->noVoters, this->maxCoerced);
    this->addState(firstState);
    int currentStateNumber = 0;
    for (int state_i=0; state_i<this->states.size(); ++state_i) {
        ++currentStateNumber;
        State state = this->states[state_i];
        if (!state.votingStarted) {
            std::vector<std::pair<short, short> > envActions;
            std::vector<std::pair<short, short> > coercerActions;
            coercerActions.push_back(std::make_pair(-1, -1)); // Wait action

            for (int i = 0; i < this->noVoters; ++i) {
                if (state.trackers[i] != -1) {
                    continue;
                }
                for (int j = 0; j < this->noVoters; ++j) {
                    if (state.votersTrackersSet[j]) {
                        continue;
                    }
                    envActions.push_back(std::make_pair(i, j));
                }
            }

            if (state.coercedVoters < state.maxCoerced) {
                for (int i = 0; i < this->noVoters; ++i) {
                    if (state.coercerVotesDemanded[i] != -1) {
                        continue;
                    }

                    for (int j = 0; j < this->noBallots; ++j) {
                        coercerActions.push_back(std::make_pair(i, j));
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
                        std::vector<std::string> actions(1, "RequestVote" + this->toString(coercerAction.second) +
                                                            "FromVoter" + this->toString(coercerAction.first));
                        actions.push_back("Wait");
                        int newStateNumber = this->addState(newState);
                        this->model.addTransition(currentStateNumber, newStateNumber, actions);
                    } else {
                        newState.votingStarted = true;
                        std::vector<std::string> actions(2, "Wait");
                        int newStateNumber = this->addState(newState);
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
                            actions.push_back("Wait");
                            int newStateNumber = this->addState(newState);
                            this->model.addTransition(currentStateNumber, newStateNumber, actions);
                        } else {
                            newState.trackers[envAction.first] = envAction.second;
                            newState.votersTrackersSet[envAction.second] = true;
                            std::vector<std::string> actions(2, "Wait");
                            int newStateNumber = this->addState(State(newState));
                            this->model.addTransition(currentStateNumber, newStateNumber, actions);
                        }
                    }
                }

            }
        } else if (!state.votesPublished) {
            std::vector<std::pair<short, short> > coercerActions;
            coercerActions.push_back(std::make_pair(-1, -1)); // Wait action
            if (state.coercedVoters < state.maxCoerced) {
                for (int i = 0; i < this->noVoters; ++i) {
                    if (state.coercerVotesDemanded[i] != -1) {
                        continue;
                    }

                    for (int j = 0; j < this->noBallots; ++j) {
                        coercerActions.push_back(std::make_pair(i, j));
                    }
                }
            }

            std::vector<std::vector<short> > votingProductArray(this->noVoters);
            bool allVoted = true;
            for (int i = 0; i < this->noVoters; ++i) {
                votingProductArray[i].push_back(-1);
                if (state.votes[i] != -1) {
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
                    for (int i = 0; i < this->noVoters; ++i) {
                        newState.publicVotes[i] = newState.votes[i];
                    }

                    if (coercerAction.first != -1) { // Coerce voter
                        newState.coercedVoters++;
                        newState.coercerVotesDemanded[coercerAction.first] = coercerAction.second;
                        newState.votersDemanded[coercerAction.first] = coercerAction.second;
                        std::vector<std::string> actions(1, "RequestVote" + this->toString(coercerAction.second) +
                                                            "FromVoter" + this->toString(coercerAction.first));
                        actions.push_back("Wait");
                        int newStateNumber = this->addState(newState);
                        this->model.addTransition(currentStateNumber, newStateNumber, actions);
                    } else {
                        std::vector<std::string> actions(2, "Wait");
                        int newStateNumber = this->addState(newState);
                        this->model.addTransition(currentStateNumber, newStateNumber, actions);
                    }
                }
            } else {
                std::vector<std::vector<short> > product = this->cartessianProduct(votingProductArray);
                for (auto &possibility: product) {
                    State votedState = state;
                    for (int i = 0; i < this->noVoters; ++i) {
                        if (possibility[i] != -1) {
                            votedState.votes[i] = possibility[i];
                        }
                    }

                    for (auto &coercerAction: coercerActions) {
                        State newState = votedState;
                        if (coercerAction.first != -1) { // Coerce voter
                            newState.coercedVoters++;
                            newState.coercerVotesDemanded[coercerAction.first] = coercerAction.second;
                            newState.votersDemanded[coercerAction.first] = coercerAction.second;
                            std::vector<std::string> actions(1, "RequestVote" + this->toString(coercerAction.second) +
                                                                "FromVoter" + this->toString(coercerAction.first));
                            if(possibility[0] == -1) {
                                actions.push_back("Wait");
                            } else {
                                actions.push_back("Vote" + this->toString(possibility[0]));
                            }
                            int newStateNumber = this->addState(newState);
                            this->model.addTransition(currentStateNumber, newStateNumber, actions);
                        } else {
                            std::vector<std::string> actions(1, "Wait");
                            if(possibility[0] == -1) {
                                actions.push_back("Wait");
                            } else {
                                actions.push_back("Vote" + this->toString(possibility[0]));
                            }
                            int newStateNumber = this->addState(newState);
                            this->model.addTransition(currentStateNumber, newStateNumber, actions);
                        }
                    }
                }
            }
        } else if (!state.votingFinished) {
            std::vector<std::pair<short, short> > coercerActions;
            coercerActions.push_back(std::make_pair(-1, -1)); // Wait action
            if (state.coercedVoters < state.maxCoerced) {
                for (int i = 0; i < this->noVoters; ++i) {
                    if (state.coercerVotesDemanded[i] != -1) {
                        continue;
                    }

                    for (int j = 0; j < this->noBallots; ++j) {
                        coercerActions.push_back(std::make_pair(i, j));
                    }
                }
            }

            std::vector<std::pair<short, short> > defenseActions;
            defenseActions.push_back(std::make_pair(-1, -1)); // Wait action
            for (int i = 0; i < this->noVoters; ++i) {
                if (state.helpRequestsSent[i] && !state.falseTrackersSent[i]) {
                    for (int j = 0; j < this->noBallots; ++j) {
                        defenseActions.push_back(std::make_pair(i, j));
                    }
                }
            }

            std::vector<std::vector<short> > votingProductArray(this->noVoters);
            for (int i = 0; i < this->noVoters; ++i) {
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

                if (state.trueTrackersCopied[i] || state.helpRequestsSent[i]) {
                    votingProductArray[i].push_back(-4); // Finish
                }
            }

            std::vector<std::vector<short> > product = this->cartessianProduct(votingProductArray);
            for (auto &possibility: product) {
                for (auto &defenseAction: defenseActions) {
                    for (auto &coercerAction: coercerActions) {
                        State newState = state;
                        std::vector<std::string> actions;
                        if (coercerAction.first != -1) { // Coerce voter
                            newState.coercedVoters++;
                            newState.coercerVotesDemanded[coercerAction.first] = coercerAction.second;
                            newState.votersDemanded[coercerAction.first] = coercerAction.second;
                            actions = std::vector<std::string>(1, "RequestVote" + this->toString(coercerAction.second) +
                                                                  "FromVoter" + this->toString(coercerAction.first));

                        } else {
                            actions = std::vector<std::string>(1, "Wait");
                        }

                        if (defenseAction.first != -1) {
                            newState.falseTrackersSent[defenseAction.first] = true;
                            if (newState.falseCopTrackVot[defenseAction.first] == -1) {
                                newState.falseCopTrackVot[defenseAction.first] = defenseAction.second;
                            }
                        }

                        int finished = 0;
                        for (int i = 0; i < this->noVoters; ++i) {
                            if (possibility[i] == -1) { //Wait
                                if(i == 0) {
                                    actions.push_back("Wait");
                                }

                            } else if (possibility[i] == -2) { // Fetch Good Tracker
                                short goodTracker = -1;
                                for (int j = 0; j < this->noVoters; ++j) {
                                    if (state.trackers[j] == i) {
                                        goodTracker = j;
                                        break;
                                    }
                                }

                                if(i == 0) {
                                    actions.push_back("FetchGoodTracker");
                                }

                                newState.votersOwnedTrackers[i] = goodTracker;

                            } else if (possibility[i] == -3) { // Copy Real Tracker
                                if(i == 0) {
                                    actions.push_back("CopyRealTracker");
                                }
                                newState.trueTrackersCopied[i] = true;
                                newState.falseCopTrackVot[i] = state.votersOwnedTrackers[i];
                            } else if (possibility[i] == -4) { // Finish
                                if(i == 0) {
                                    actions.push_back("Finish");
                                }
                                ++finished;
                            } else { // Help I Need possibility[i]
                                if(i == 0) {
                                    actions.push_back("HelpINeed" + this->toString(possibility[0]));
                                }
                                newState.helpRequestsSent[i] = true;
                                newState.envVoteDemanded[i] = possibility[i];
                            }
                        }

                        if (finished == this->noVoters) {
                            newState.votingFinished = true;
                        }

                        int newStateNumber = this->addState(newState);
                        this->model.addTransition(currentStateNumber, newStateNumber, actions);
                    }
                }
            }
        } else {
            std::vector<std::pair<short, short> > coercerActions;
            coercerActions.push_back(std::make_pair(-1, -1)); // Wait action
            if (state.coercedVoters < state.maxCoerced) {
                for (int i = 0; i < this->noVoters; ++i) {
                    if (state.coercerVotesDemanded[i] != -1) {
                        continue;
                    }

                    for (int j = 0; j < this->noBallots; ++j) {
                        coercerActions.push_back(std::make_pair(i, j));
                    }
                }
            }

            for (auto &coercerAction: coercerActions) {
                State newState = state;
                if (coercerAction.first != -1) { // Coerce voter
                    newState.coercedVoters++;
                    newState.coercerVotesDemanded[coercerAction.first] = coercerAction.second;
                    newState.votersDemanded[coercerAction.first] = coercerAction.second;
                    std::vector<std::string> actions(1, "RequestVote" + this->toString(coercerAction.second) +
                                                        "FromVoter" + this->toString(coercerAction.first));
                    actions.push_back("Wait");
                    int newStateNumber = this->addState(newState);
                    this->model.addTransition(currentStateNumber, newStateNumber, actions);
                } else {
                    std::vector<std::string> actions(2, "Wait");
                    int newStateNumber = this->addState(newState);
                    this->model.addTransition(currentStateNumber, newStateNumber, actions);
                }
            }
        }
    }
}

int SeleneModel::addState(SeleneModel::State state) {
    if (this->stateToNumber[state] == 0) {
        this->stateToNumber[state] = this->stateNumber++;
        this->states.push_back(state);
        return this->stateNumber - 1;
    } else {
        return this->stateToNumber[state];
    }
}

std::string SeleneModel::toString(short a) {
    std::string str = "";
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
        for (int i = 0; i < array.size(); ++i) {
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
    for(int i = 0; i < this->noVoters; ++i) {
        for(int j = 0; j < this->noBallots; ++j) {
            std::string action = "RequestVote" + this->toString(j);
            action += "FromVoter" + this->toString(i);
            this->model.addAction(0, action);
        }
    }

    for(int i = 0; i < this->noBallots; ++i) {
        this->model.addAction(1, "Vote" + this->toString(i));
        this->model.addAction(1, "HelpINeed" + this->toString(i));
    }

    this->model.addAction(1, "FetchGoodTracker");
    this->model.addAction(1, "CopyRealTracker");
    this->model.addAction(1, "Finish");
}

//TODO Add epistemic classes

void SeleneModel::CoercerEpistemicState::operator=(const SeleneModel::State &rhs) {
    this->votesPublished = rhs.votesPublished;
    this->votingFinished = rhs.votingFinished;
    this->votingStarted = rhs.votingStarted;
    this->falseCopTrackVot = rhs.falseCopTrackVot;
    this->publicVotes =rhs.publicVotes;
    this->coercedVoters =rhs.coercedVoters;
    this->maxCoerced = rhs.maxCoerced;
    this->coercerVotesDemanded = rhs.coercerVotesDemanded;
}
