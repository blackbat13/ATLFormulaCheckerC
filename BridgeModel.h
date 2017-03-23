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

typedef short CARD_TYPE;
typedef std::vector<short> HAND_TYPE;
typedef std::vector<HAND_TYPE > HANDS_TYPE;
typedef std::vector<char> LEFTS_TYPE;
typedef std::vector<CARD_TYPE> BOARD_TYPE;
typedef std::vector<CARD_TYPE> HISTORY_TYPE;

class BridgeModel {
public:
    struct State{
        HANDS_TYPE hands;
        LEFTS_TYPE lefts;
        char next;
        BOARD_TYPE board;
        char beginning;
        HISTORY_TYPE history;
        char clock;
        char suit;

        State();
        State(const HANDS_TYPE &hands, const LEFTS_TYPE &lefts, int next,
              const BOARD_TYPE &board, int beginning, const HISTORY_TYPE &history, int clock, int suit);

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
    int newSuit(State state, CARD_TYPE card);
    BOARD_TYPE newBoardAfterPlay(State state, CARD_TYPE card);
    HISTORY_TYPE newHistoryAfterPlay(State state, CARD_TYPE card);
    int countRemainingCards(State state);
    int getWinner(int beginning, BOARD_TYPE board);
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
