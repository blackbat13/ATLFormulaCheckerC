//
// Created by Damian Kurpiewski on 20.03.2017.
//

#include "BridgeModel.h"

BridgeModel::State::State(const HANDS_TYPE &hands, const LEFTS_TYPE &lefts, int next,
                          const BOARD_TYPE &board, int beginning, const HISTORY_TYPE &history, int clock,
                          int suit) : hands(hands), lefts(lefts), next(next), board(board), beginning(beginning),
                                      history(history), clock(clock), suit(suit) {}

BridgeModel::State::State(){}

bool BridgeModel::State::operator<(const BridgeModel::State &rhs) const {
    if (hands < rhs.hands)
        return true;
    if (rhs.hands < hands)
        return false;
    if (lefts < rhs.lefts)
        return true;
    if (rhs.lefts < lefts)
        return false;
    if (next < rhs.next)
        return true;
    if (rhs.next < next)
        return false;
    if (board < rhs.board)
        return true;
    if (rhs.board < board)
        return false;
    if (beginning < rhs.beginning)
        return true;
    if (rhs.beginning < beginning)
        return false;
    if (history < rhs.history)
        return true;
    if (rhs.history < history)
        return false;
    if (clock < rhs.clock)
        return true;
    if (rhs.clock < clock)
        return false;
    return suit < rhs.suit;
}

bool BridgeModel::State::operator>(const BridgeModel::State &rhs) const {
    return rhs < *this;
}

bool BridgeModel::State::operator<=(const BridgeModel::State &rhs) const {
    return !(rhs < *this);
}

bool BridgeModel::State::operator>=(const BridgeModel::State &rhs) const {
    return !(*this < rhs);
}

BridgeModel::BridgeModel(int noCardsAvailable, int noEndCards, BridgeModel::State firstState): noCardsAvailable(noCardsAvailable), noEndCards(noEndCards), firstState(firstState) {
    srand((unsigned int)time(NULL));
    this->generateCardsDictionary();

    if(this->firstState.hands.size() == 0) {
        this->firstState.hands = this->generateRandomHands(noCardsAvailable, noEndCards);
        this->printHands(this->firstState);
    }

    this->createAtlModel();
    this->model.addAction(0, "wait");
    this->generateAvailableCards();
    printf("Starting generating beginning states\n");
    this->generateBeginningStates();
    this->beginningStatesCount = this->states.size();
    printf("Generated %lu beginning states\n", this->beginningStatesCount);
    printf("Starting generating rest of model\n");
    this->generateRestOfModel();
    printf("Generated model with %lu states\n", this->states.size());
    printf("Starting preparing epistemic relation\n");
    this->prepareEpistemicRelation();
    printf("Prepared epistemic relation\n");
}

void BridgeModel::createAtlModel() {
    switch(this->noEndCards) {
        case 1:
            this->model = AtlModel(3, 100);
            break;
        case 2:
            this->model = AtlModel(3, 1000);
            break;
        case 3:
            this->model = AtlModel(3, 100000);
            break;
        case 4:
            this->model = AtlModel(3, 3000000);
            break;
        default:
            this->model = AtlModel(3, 10000000);
    }
}

void BridgeModel::generateAvailableCards() {
    int cardNumber = 14;
    for(int i = 0; i < this->noCardsAvailable; ++i) {
        for(int c = 4; c >= 1; --c) {
            this->cardsAvailable.push_back(cardNumber * 10 + c);
            this->model.addAction(0, this->cardsDictionary[cardNumber * 10 + c]);
        }

        --cardNumber;
    }
}

void BridgeModel::generateBeginningStates() {
    HAND_TYPE enemyHands = this->firstState.hands[1];
    for(auto card: this->firstState.hands[3]) {
        enemyHands.push_back(card);
    }

    for(auto&& player2: iter::combinations(enemyHands, this->noEndCards)) {
        HAND_TYPE player4 = enemyHands;
        for(auto card: player2){
            HAND_TYPE::iterator it = std::find(player4.begin(), player4.end(), card);
            if(it != player4.end()) {
                player4.erase(it);
            }
        }

        HANDS_TYPE newHands = this->firstState.hands;
        newHands[1] = HAND_TYPE();
        newHands[3] = HAND_TYPE();
        for(auto card: player2) {
            newHands[1].push_back(card);
        }

        for(auto card: player4) {
            newHands[3].push_back(card);
        }

        std::sort(newHands[1].begin(), newHands[1].end());
        std::sort(newHands[3].begin(), newHands[3].end());

        State state = State(newHands, this->firstState.lefts, this->firstState.next, this->firstState.board, this->firstState.beginning, this->firstState.history, this->firstState.clock, this->firstState.suit);
        this->addState(state);
    }

}

void BridgeModel::generateRestOfModel() {
    int currentStateNumber = -1;
    for(int s = 0; s < this->states.size(); ++s) {
        State state = this->states[s];
        ++currentStateNumber;
        if(state.next == state.beginning && state.clock == 0) {
            if(this->countRemainingCards(state) == 0) {
                break;
            }

            for(int i = 0; i < state.hands[state.next].size(); ++i) {
                int card = state.hands[state.next][i];
                if(card == -1) {
                    continue;
                }

                State newState = this->newStateAfterPlay(state, i);
                int agentNumber = state.next;
                if(agentNumber == 2) {
                    agentNumber = 0;
                }

                std::vector<std::string> actions(4, "wait");
                actions[agentNumber] = this->cardsDictionary[card];
                int newStateNumber = this->addState(newState);
                this->model.addTransition(currentStateNumber, newStateNumber, actions);
            }
        } else if(state.clock == 4) {
            int winner = this->getWinner(state.beginning, state.board);
            LEFTS_TYPE newLefts = state.lefts;
            ++newLefts[winner % 2];
            int newNext = winner;
            int newClock = 0;
            int newBeginning = winner;
            int newSuit = -1;
            BOARD_TYPE newBoard(4, -1);
            std::vector<std::string> actions(4, "wait");
            State newState = State(state.hands, newLefts, newNext, newBoard, newBeginning, state.history, newClock, newSuit);

            int newStateNumber = this->addState(newState);
            this->model.addTransition(currentStateNumber, newStateNumber, actions);
        } else {
            int color = state.board[state.beginning] % 10;
            bool haveColor = false;
            for(auto card: state.hands[state.next]) {
                if(card % 10 == color) {
                    haveColor = true;
                    break;
                }
            }

            for(int i = 0; i < state.hands[state.next].size(); ++i) {
                int card = state.hands[state.next][i];
                if(!(!haveColor || card % 10 == color) || card == -1) {
                    continue;
                }

                State newState = this->newStateAfterPlay(state, i);
                int agentNumber = state.next;
                if(agentNumber == 2) {
                    agentNumber = 0;
                }

                std::vector<std::string> actions(4, "wait");
                actions[agentNumber] = this->cardsDictionary[card];
                int newStateNumber = this->addState(newState);
                this->model.addTransition(currentStateNumber, newStateNumber, actions);
            }
        }
    }
}

int BridgeModel::addState(BridgeModel::State state) {
    int newStateNumber = this->getStateNumber(state);
    State epistemicState = this->getEpistemicState(state);
    this->addToEpistemicDictionary(epistemicState, newStateNumber);
    return newStateNumber;
}

int BridgeModel::getStateNumber(BridgeModel::State state) {
    int newStateNumber;
    if(this->statesDictionary[state] == 0) {
        this->statesDictionary[state] = this->stateNumber;
        newStateNumber = this->stateNumber;
        this->states.push_back(state);
        this->stateNumber++;
    } else {
        newStateNumber = this->statesDictionary[state];
    }

    return newStateNumber;
}

void BridgeModel::addToEpistemicDictionary(BridgeModel::State state, int newStateNumber) {
    this->epistemicStatesDictionary[state].insert(newStateNumber);
}

BridgeModel::State BridgeModel::getEpistemicState(BridgeModel::State state) {
    HANDS_TYPE epistemicHands = state.hands;
    epistemicHands[1] = this->keepValuesInList(epistemicHands[1], -1);
    epistemicHands[3] = this->keepValuesInList(epistemicHands[3], -1);
    State epistemicState = State(epistemicHands, state.lefts, state.next, state.board, state.beginning, state.history, state.clock, state.suit);
    return epistemicState;
}

void BridgeModel::prepareEpistemicRelation() {
    for(auto item: this->epistemicStatesDictionary) {
        this->model.addEpistemicClass(0, item.second);
    }
}

BridgeModel::State BridgeModel::newStateAfterPlay(BridgeModel::State state, int cardIndex) {
    int card = state.hands[state.next][cardIndex];
    BOARD_TYPE newBoard = this->newBoardAfterPlay(state, card);
    HISTORY_TYPE newHistory = this->newHistoryAfterPlay(state, card);

    int newNext = (state.next + 1) % 4;
    int newClock = state.clock + 1;
    HANDS_TYPE newHands = state.hands;
    newHands[state.next][cardIndex] = -1;
    int newSuit = this->newSuit(state, card);

    State newState = State(newHands, state.lefts, newNext, newBoard, state.beginning, newHistory, newClock, newSuit);

    return newState;
}

int BridgeModel::newSuit(BridgeModel::State state, CARD_TYPE card) {
    if(state.next == state.beginning) {
        return card % 10;
    } else {
        return state.suit;
    }
}

BOARD_TYPE BridgeModel::newBoardAfterPlay(BridgeModel::State state, CARD_TYPE card) {
    BOARD_TYPE newBoard = state.board;
    newBoard[state.next] = card;
    return newBoard;
}

HISTORY_TYPE BridgeModel::newHistoryAfterPlay(BridgeModel::State state, CARD_TYPE card) {
    HISTORY_TYPE newHistory = state.history;
    newHistory.push_back(card);
    std::sort(newHistory.begin(), newHistory.end());
    return newHistory;
}

int BridgeModel::countRemainingCards(BridgeModel::State state) {
    int remainingCardsCount = 0;
    for(auto card: state.hands[state.next]) {
        if(card != -1) {
            ++remainingCardsCount;
        }
    }

    return remainingCardsCount;
}

int BridgeModel::getWinner(int beginning, BOARD_TYPE board) {
    std::vector<CARD_TYPE> cards;
    for(int i = 0; i < 4; ++i) {
        cards.push_back(board[(beginning+i) % 4]);
    }

    int winner = beginning;
    int winningCard = cards[0];
    int color = cards[0] % 10;
    for(int i = 1; i < 4; ++i) {
        if(cards[i] % 10 == color && cards[i] > winningCard) {
            winningCard = cards[i];
            winner = (beginning + i) % 4;
        }
    }

    return winner;
}

HAND_TYPE BridgeModel::keepValuesInList(HAND_TYPE list, int value) {
    int countValues = 0;
    for(auto element: list) {
        if(element == value) {
            ++countValues;
        }
    }

    return HAND_TYPE((unsigned long)countValues, value);
}

std::vector<std::string> BridgeModel::generateReadableCardsArray() {
    std::string cardNames[] = {"Ace", "King", "Queen", "Jack", "ten", "nine", "eight", "seven", "six", "five", "four", "three",
            "two"};
    std::string cardColors[] = {"Spade", "Heart", "Diamond", "Club"};
    std::vector<std::string> cards;
    for(auto name: cardNames) {
        for(auto color: cardColors) {
            cards.push_back(name + color);
        }
    }

    return cards;
}

void BridgeModel::generateCardsDictionary() {
    std::vector<std::string> cards = this->generateReadableCardsArray();
    int cardNameNumber = 14;
    int i = 0;
    while(cardNameNumber > 1){
        int cardColorNumber = 4;
        while(cardColorNumber > 0) {
            this->cardsDictionary[cardNameNumber * 10 + cardColorNumber] = cards[i];
            ++i;
            --cardColorNumber;
        }

        --cardNameNumber;
    }
}

std::vector<std::vector<std::string> > BridgeModel::handsToReadableHands(HANDS_TYPE hands) {
    std::vector<std::vector<std::string> > readableHands;
    for(auto hand: hands) {
        std::vector<std::string> readableHand;
        for(auto cardNumber: hand) {
            readableHand.push_back(this->cardsDictionary[cardNumber]);
        }

        readableHands.push_back(readableHand);
    }

    return readableHands;
}

HANDS_TYPE BridgeModel::generateRandomHands(int noCardsAvailable, int noCardsInHand) {
    std::vector<int> array;
    std::vector<bool> used((unsigned long)noCardsAvailable * 4, false);
    std::vector<CARD_TYPE> cardNumbers;
    for(int i = 14; i > 0; --i) {
        for(int j = 4; j > 0; --j) {
            cardNumbers.push_back(i * 10 + j);
        }
    }

    for(int i = 0; i < noCardsInHand * 4; ++i) {
        long number;
        do {
            number = rand() % (noCardsAvailable * 4);
        }while(used[number]);

        array.push_back(cardNumbers[number]);
        used[number] = true;
    }

    HANDS_TYPE hands;
    int j = 0;
    for(int i = 0; i < 4; ++i) {
        HAND_TYPE hand;
        for(int k = 0; k < noCardsInHand; ++k) {
            hand.push_back(array[j]);
            ++j;
        }

        std::sort(hand.begin(), hand.end());
        hands.push_back(hand);
    }

    return hands;
}

AtlModel &BridgeModel::getModel() {
    return model;
}

const std::vector<BridgeModel::State> &BridgeModel::getStates() const {
    return states;
}

void BridgeModel::printHands(BridgeModel::State state) {
    std::vector<std::vector<std::string> > hands = this->handsToReadableHands(state.hands);
    for(int i = 0; i < 4; ++i) {
        for(auto card: hands[i]){
            printf("%s ", card.c_str());
        }
    }

    printf("\n");
}

unsigned long BridgeModel::getBeginningStatesCount() const {
    return beginningStatesCount;
}




