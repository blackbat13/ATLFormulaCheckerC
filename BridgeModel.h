//
// Created by Damian Kurpiewski on 20.03.2017.
//

#ifndef ATLFORMULACHECKER_BRIDGEMODEL_H
#define ATLFORMULACHECKER_BRIDGEMODEL_H

#include "AtlModel.h"
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include <random>
#include "cppitertools/itertools.hpp"

#define HAND_TYPE std::vector<short>
#define HANDS_TYPE std::vector<HAND_TYPE >

class BridgeModel {
public:
    struct State{
        HANDS_TYPE hands;
        std::vector<int> lefts;
        char next;
        std::vector<int> board;
        int beginning;
        std::vector<int> history;
        char clock;
        char suit;

        State();
        State(const HANDS_TYPE &hands, const std::vector<int> &lefts, int next,
              const std::vector<int> &board, int beginning, const std::vector<int> &history, int clock, int suit);

        bool operator<(const State &rhs) const;

        bool operator>(const State &rhs) const;

        bool operator<=(const State &rhs) const;

        bool operator>=(const State &rhs) const;
    };

private:
public:
    unsigned long getBeginningStatesCount() const;

private:
    AtlModel model;
    std::map<State, int> statesDictionary;
    std::map<State, std::set<int> > epistemicStatesDictionary;
    int noCardsAvailable;
    int noEndCards;
    std::vector<int> cardsAvailable;
    int stateNumber = 0;
    State firstState;
    unsigned long beginningStatesCount;
    std::vector<State> states;
    std::map<int, std::string> cardsDictionary;


    void createAtlModel();
    void generateAvailableCards();
    void generateBeginningStates();
    void generateRestOfModel();
    int addState(State state);
    int getStateNumber(State state);
    void addToEpistemicDictionary(State state, int newStateNumber);
    State getEpistemicState(State state);
    void prepareEpistemicRelation();
    State newStateAfterPlay(State state, int cardIndex);
    int newSuit(State state, int card);
    std::vector<int> newBoardAfterPlay(State state, int card);
    std::vector<int> newHistoryAfterPlay(State state, int card);
    int countRemainingCards(State state);
    int getWinner(int beginning, std::vector<int> board);
    HAND_TYPE keepValuesInList(HAND_TYPE list, int value);
    std::vector<std::string> generateReadableCardsArray();
    void generateCardsDictionary();

public:
    BridgeModel(int noCardsAvailable, int noEndCards, State firstState);
    HANDS_TYPE generateRandomHands(int noCardsAvailable, int noCardsInHand);
    std::vector<std::vector<std::string> > handsToReadableHands(HANDS_TYPE hands);
    void printHands(State state);
    AtlModel &getModel();

    const std::vector<State> &getStates() const;
};


#endif //ATLFORMULACHECKER_BRIDGEMODEL_H
