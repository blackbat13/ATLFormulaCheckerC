//
// Created by Damian Kurpiewski
//

#include "BridgeModelLowerAbstr.h"

#define NotRandom false

BridgeModelLowerAbstr::State::State(const HANDS_TYPE &hands, const LEFTS_TYPE &lefts, char next,
                                    const BOARD_TYPE &board, char beginning, char clock,
                                    char suit) : hands(hands), lefts(lefts), next(next), board(board),
                                                 beginning(beginning),
                                                 clock(clock), suit(suit) {}

BridgeModelLowerAbstr::State::State() = default;

bool BridgeModelLowerAbstr::State::operator<(const BridgeModelLowerAbstr::State &rhs) const {
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
    if (clock < rhs.clock)
        return true;
    if (rhs.clock < clock)
        return false;
    return suit < rhs.suit;
}

bool BridgeModelLowerAbstr::State::operator>(const BridgeModelLowerAbstr::State &rhs) const {
    return rhs < *this;
}

bool BridgeModelLowerAbstr::State::operator<=(const BridgeModelLowerAbstr::State &rhs) const {
    return !(rhs < *this);
}

bool BridgeModelLowerAbstr::State::operator>=(const BridgeModelLowerAbstr::State &rhs) const {
    return !(*this < rhs);
}

bool BridgeModelLowerAbstr::State::operator==(const BridgeModelLowerAbstr::State &rhs) const {
    return hands == rhs.hands &&
           lefts == rhs.lefts &&
           next == rhs.next &&
           board == rhs.board &&
           beginning == rhs.beginning &&
           clock == rhs.clock &&
           suit == rhs.suit;
}

bool BridgeModelLowerAbstr::State::operator!=(const BridgeModelLowerAbstr::State &rhs) const {
    return !(rhs == *this);
}

BridgeModelLowerAbstr::BridgeModelLowerAbstr(int noCardsAvailable, int noEndCards,
                                             BridgeModelLowerAbstr::State firstState,
                                             std::string outputFileName, CARD_TYPE abstractionLevel) : noCardsAvailable(
        noCardsAvailable),
                                                                                                       noEndCards(
                                                                                                               noEndCards),
                                                                                                       firstState(
                                                                                                               std::move(
                                                                                                                       firstState)),

                                                                                                       abstractionLevel(
                                                                                                               abstractionLevel) {
    this->outputFile.open(outputFileName);
    std::random_device randomDevice;
    this->randomEngine = std::default_random_engine(randomDevice());
    this->generateCardsDictionary();

    if (this->firstState.hands.empty()) {
        this->firstState.hands = this->generateRandomHands(noCardsAvailable, noEndCards);
        this->printHands(this->firstState);
    }

    State abstractState = this->firstState;
    this->abstractState(abstractState);
    this->abstractStateContent[abstractState].insert(this->firstState);
//    this->abstractState(this->firstState);
    this->printHands(this->firstState);
    this->printHands(abstractState);

    this->createAtlModel();
    this->model.addAction(0, "wait");
    this->generateAvailableCards();
    this->outputFile << "Generating beginning states... ";
    this->generateBeginningStates();
    this->beginningStatesCount = this->states.size();
    this->model.setBeginningStatesCount(this->beginningStatesCount);
    this->outputFile << "Done" << std::endl;
    this->outputFile << "Generated " << this->beginningStatesCount << " beginning states" << std::endl;
    this->outputFile << "Generating rest of model... ";
    this->generateRestOfModel();
    this->model.setWinningStates(this->winningStates);
    this->outputFile << "Done" << std::endl;
    this->outputFile << "Generated model with " << this->stateNumber << " states" << std::endl;
    this->outputFile << "Preparing epistemic relation... ";
    this->model.setNumberOfStates(this->stateNumber + 1);
    this->prepareEpistemicRelation();
    this->outputFile << "Done" << std::endl;
    this->outputFile << "Generated " << this->model.imperfectInformation[0].size() << " epistemic classes" << std::endl;
    this->outputFile << "Clearing old data... ";
    this->clear();
    this->outputFile << "Done" << std::endl;
    this->outputFile.close();
}

void BridgeModelLowerAbstr::createAtlModel() {
    switch (this->noEndCards) {
        case 1:
            this->model = AtlModel(1, 100);
            break;
        case 2:
            this->model = AtlModel(1, 1000);
            break;
        case 3:
            this->model = AtlModel(1, 100000);
            break;
        case 4:
            this->model = AtlModel(1, 3000000);
            break;
        case 5:
            this->model = AtlModel(1, 108000000);
            break;
        default:
            this->model = AtlModel(1, 1000000000);
    }
}

void BridgeModelLowerAbstr::generateAvailableCards() {
    CARD_TYPE cardNumber = 14;
    for (short i = 0; i < this->noCardsAvailable; ++i) {
        for (short c = 4; c >= 1; --c) {
            this->cardsAvailable.push_back(cardNumber * (short) 10 + c);
            this->model.addAction(0, this->cardsDictionary[cardNumber * (short) 10 + c]);
        }

        --cardNumber;
    }
}

void BridgeModelLowerAbstr::generateBeginningStates() {
    HAND_TYPE enemyHands = this->firstState.hands[1];
    for (auto card: this->firstState.hands[3]) {
        enemyHands.push_back(card);
    }

    for (auto &&player2: iter::combinations(enemyHands, this->noEndCards)) {
        HAND_TYPE player4 = enemyHands;
        for (auto card: player2) {
            auto it = std::find(player4.begin(), player4.end(), card);
            if (it != player4.end()) {
                player4.erase(it);
            }
        }

        HANDS_TYPE newHands = this->firstState.hands;
        newHands[1] = HAND_TYPE();
        newHands[3] = HAND_TYPE();
        for (auto card: player2) {
            newHands[1].push_back(card);
        }

        for (auto card: player4) {
            newHands[3].push_back(card);
        }

        std::sort(newHands[1].begin(), newHands[1].end());
        std::sort(newHands[3].begin(), newHands[3].end());

        State state = State(newHands, this->firstState.lefts, this->firstState.next, this->firstState.board,
                            this->firstState.beginning, this->firstState.clock, this->firstState.suit);
        this->addState(state);
    }

}

void BridgeModelLowerAbstr::generateRestOfModel() {
    int currentStateNumber = -1;
    while (!this->states.empty()) {
        State state = this->states.front();
        this->states.pop();
        ++currentStateNumber;
        for (auto contentState : this->abstractStateContent[state]) {
//            this->printHands(contentState);
            if (contentState.next == contentState.beginning && contentState.clock == 0) {
                if (this->countRemainingCards(contentState) == 0) {
                    break;
                }

                for (int i = 0; i < contentState.hands[contentState.next].size(); ++i) {
                    CARD_TYPE card = contentState.hands[contentState.next][i];
                    if (card == -1) {
                        continue;
                    }

                    State newState = this->newStateAfterPlay(contentState, i);
                    int agentNumber = contentState.next;
                    if (agentNumber == 2) {
                        agentNumber = 0;
                    }

                    std::vector<std::string> actions(1, "wait");
                    if (agentNumber == 0) {
                        actions[agentNumber] = this->cardsDictionary[this->abstractCard(card)];
                    }

                    int newStateNumber = this->addState(newState);
                    this->model.addTransition(currentStateNumber, newStateNumber, actions);
                }
            } else if (contentState.clock == 4) {
//            char winner = this->getWinner(contentState.beginning, contentState.board);
                std::vector<char> winners = this->getWinners(contentState.beginning, contentState.board);
                for (auto winner : winners) {
                    LEFTS_TYPE newLefts = contentState.lefts;
                    ++newLefts[winner % 2];
                    char newNext = winner;
                    char newClock = 0;
                    char newBeginning = winner;
                    char newSuit = -1;
                    BOARD_TYPE newBoard(4, -1);
                    std::vector<std::string> actions(1, "wait");
                    State newState = State(contentState.hands, newLefts, newNext, newBoard, newBeginning, newClock,
                                           newSuit);

                    int newStateNumber = this->addState(newState);
                    this->model.addTransition(currentStateNumber, newStateNumber, actions);
                }
            } else {
                int color = contentState.board[contentState.beginning] % 10;
                bool haveColor = false;
                for (auto card: contentState.hands[contentState.next]) {
                    if (card % 10 == color) {
                        haveColor = true;
                        break;
                    }
                }

                for (int i = 0; i < contentState.hands[contentState.next].size(); ++i) {
                    CARD_TYPE card = contentState.hands[contentState.next][i];
                    if (!(!haveColor || card % 10 == color) || card == -1) {
                        continue;
                    }

                    State newState = this->newStateAfterPlay(contentState, i);
                    int agentNumber = contentState.next;
                    if (agentNumber == 2) {
                        agentNumber = 0;
                    }

                    std::vector<std::string> actions(1, "wait");
                    if (agentNumber == 0) {
                        actions[agentNumber] = this->cardsDictionary[this->abstractCard(card)];
                    }

                    int newStateNumber = this->addState(newState);
                    this->model.addTransition(currentStateNumber, newStateNumber, actions);
                }
            }
        }

        this->abstractStateContent[state].clear();
    }
}

int BridgeModelLowerAbstr::addState(BridgeModelLowerAbstr::State &state) {
    State abstractState = state;
    this->abstractState(abstractState);
    this->abstractStateContent[abstractState].insert(state);
    int newStateNumber = this->getStateNumber(abstractState);
    State epistemicState = this->getEpistemicState(abstractState);
    this->addToEpistemicDictionary(epistemicState, newStateNumber);
    return newStateNumber;
}

int BridgeModelLowerAbstr::getStateNumber(BridgeModelLowerAbstr::State &state) {
    int newStateNumber = this->nextLevelStates[state];

    if (newStateNumber == 0) {
//        this->printHands(state);
        newStateNumber = this->stateNumber;
        this->states.push(state);

        // If current level is finished (beginning new one) then clear current level
        if (!this->nextLevelStates.empty() && this->nextLevelStates.begin()->first.clock != state.clock) {
            this->nextLevelStates.clear();
        }

        this->nextLevelStates[state] = newStateNumber;
        if (this->isWinningState(state)) {
            this->winningStates.insert(newStateNumber);
        }

        this->stateNumber++;
    }

    return newStateNumber;
}

void BridgeModelLowerAbstr::addToEpistemicDictionary(BridgeModelLowerAbstr::State &state, int newStateNumber) {
    if (this->epistemicStateClassNumber[state] == 0) {
        std::set<int> newEpistemicClass;
        newEpistemicClass.insert(newStateNumber);
        this->model.imperfectInformation[0].push_back(newEpistemicClass);
        this->epistemicStateClassNumber[state] = (int) this->model.imperfectInformation[0].size();
    } else {
        this->model.imperfectInformation[0][this->epistemicStateClassNumber[state] - 1].insert(newStateNumber);
    }
}

BridgeModelLowerAbstr::State BridgeModelLowerAbstr::getEpistemicState(BridgeModelLowerAbstr::State &state) {
    HANDS_TYPE epistemicHands = state.hands;
    epistemicHands[1] = this->keepValuesInList(epistemicHands[1], -1);
    epistemicHands[3] = this->keepValuesInList(epistemicHands[3], -1);
    State epistemicState = State(epistemicHands, state.lefts, state.next, state.board, state.beginning, state.clock,
                                 state.suit);
    return epistemicState;
}

void BridgeModelLowerAbstr::prepareEpistemicRelation() {
    this->model.finishEpistemicClasses(0);
//    this->model.clearTransitions();
}

BridgeModelLowerAbstr::State
BridgeModelLowerAbstr::newStateAfterPlay(BridgeModelLowerAbstr::State &state, char cardIndex) {
    CARD_TYPE card = state.hands[state.next][cardIndex];
    BOARD_TYPE newBoard = this->newBoardAfterPlay(state, card);

    char newNext = (state.next + (char) 1) % (char) 4;
    char newClock = state.clock + (char) 1;
    HANDS_TYPE newHands = state.hands;
    newHands[state.next][cardIndex] = -1;
    char newSuit = this->newSuit(state, card);

    State newState = State(newHands, state.lefts, newNext, newBoard, state.beginning, newClock, newSuit);

    return newState;
}

char BridgeModelLowerAbstr::newSuit(BridgeModelLowerAbstr::State &state, CARD_TYPE &card) {
    if (state.next == state.beginning) {
        return (char) (card % 10);
    }

    return state.suit;
}

BOARD_TYPE BridgeModelLowerAbstr::newBoardAfterPlay(BridgeModelLowerAbstr::State &state, CARD_TYPE card) {
    BOARD_TYPE newBoard = state.board;
    newBoard[state.next] = card;
    return newBoard;
}

int BridgeModelLowerAbstr::countRemainingCards(BridgeModelLowerAbstr::State &state) {
    int remainingCardsCount = 0;
    for (auto card: state.hands[state.next]) {
        if (card != -1) {
            ++remainingCardsCount;
        }
    }

    return remainingCardsCount;
}

char BridgeModelLowerAbstr::getWinner(char &beginning, BOARD_TYPE &board) {
    std::vector<CARD_TYPE> cards;
    cards.reserve(4);
    for (int i = 0; i < 4; ++i) {
        cards.push_back(board[(beginning + i) % 4]);
    }

    char winner = beginning;
    CARD_TYPE winningCard = cards[0];
    auto color = (char) (cards[0] % 10);
    for (char i = 1; i < 4; ++i) {
        if (cards[i] % 10 == color && cards[i] > winningCard) {
            winningCard = cards[i];
            winner = (char) ((beginning + i) % 4);
        }
    }

    return winner;
}

std::vector<char> BridgeModelLowerAbstr::getWinners(char &beginning, BOARD_TYPE &board) {
    std::vector<CARD_TYPE> cards;
    cards.reserve(4);
    for (int i = 0; i < 4; ++i) {
        cards.push_back(board[(beginning + i) % 4]);
    }

    char winner = beginning;
    CARD_TYPE winningCard = cards[0];
    auto color = (char) (cards[0] % 10);
    for (char i = 1; i < 4; ++i) {
        if (cards[i] % 10 == color && cards[i] > winningCard) {
            winningCard = cards[i];
            winner = (char) ((beginning + i) % 4);
        }
    }

    std::vector<char> winners;

    for (char i = 0; i < 4; ++i) {
        if (cards[i] == winningCard) {
            winners.push_back((char) ((beginning + i) % 4));
        }
    }

    return winners;
}

HAND_TYPE BridgeModelLowerAbstr::keepValuesInList(HAND_TYPE &list, CARD_TYPE value) {
    unsigned long countValues = 0;
    for (auto element: list) {
        if (element == value) {
            ++countValues;
        }
    }

    return HAND_TYPE(countValues, value);
}

std::vector<std::string> BridgeModelLowerAbstr::generateReadableCardsArray() {
    std::string cardNames[] = {"Ace", "King", "Queen", "Jack", "ten", "nine", "eight", "seven", "six", "five", "four",
                               "three",
                               "two"};
    std::string cardColors[] = {"Spade", "Heart", "Diamond", "Club"};
    std::vector<std::string> cards;
    for (const auto &name: cardNames) {
        for (const auto &color: cardColors) {
            cards.push_back(name + color);
        }
    }

    return cards;
}

void BridgeModelLowerAbstr::generateCardsDictionary() {
    std::vector<std::string> cards = this->generateReadableCardsArray();
    int cardNameNumber = 14;
    int i = 0;
    while (cardNameNumber > 1) {
        int cardColorNumber = 4;
        while (cardColorNumber > 0) {
            this->cardsDictionary[cardNameNumber * 10 + cardColorNumber] = cards[i];
            ++i;
            --cardColorNumber;
        }

        --cardNameNumber;
    }
}

std::vector<std::vector<std::string> > BridgeModelLowerAbstr::handsToReadableHands(HANDS_TYPE &hands) {
    std::vector<std::vector<std::string> > readableHands;
    for (auto hand: hands) {
        std::vector<std::string> readableHand;
        readableHand.reserve(hand.size());
        for (auto cardNumber: hand) {
            readableHand.push_back(this->cardsDictionary[cardNumber]);
        }

        readableHands.push_back(readableHand);
    }

    return readableHands;
}

HANDS_TYPE BridgeModelLowerAbstr::generateRandomHands(int &noCardsAvailable, int &noCardsInHand) {
    std::vector<CARD_TYPE> array;
    std::vector<bool> used((unsigned long) noCardsAvailable * 4, false);
    std::vector<CARD_TYPE> cardNumbers;
    for (short i = 14; i > 0; --i) {
        for (short j = 4; j > 0; --j) {
            cardNumbers.push_back(i * (short) 10 + j);
        }
    }

    if (NotRandom) {
        long number = 0;
        for (int i = 0; i < noCardsInHand * 4; ++i) {
            array.push_back(cardNumbers[number]);
            ++number;
        }
    } else {
        std::uniform_int_distribution<int> uniformIntDistribution(0, noCardsAvailable * 4 - 1);
        for (int i = 0; i < noCardsInHand * 4; ++i) {
            long number;
            do {
                number = uniformIntDistribution(this->randomEngine);
            } while (used[number]);

            array.push_back(cardNumbers[number]);
            used[number] = true;
        }
    }

    HANDS_TYPE hands;
    int j = 0;
    for (int i = 0; i < 4; ++i) {
        HAND_TYPE hand;
        for (int k = 0; k < noCardsInHand; ++k) {
            hand.push_back(array[j]);
            ++j;
        }

        std::sort(hand.begin(), hand.end());
        hands.push_back(hand);
    }

    return hands;
}

AtlModel &BridgeModelLowerAbstr::getModel() {
    return model;
}

void BridgeModelLowerAbstr::printHands(BridgeModelLowerAbstr::State &state) {
    std::vector<std::vector<std::string> > hands = this->handsToReadableHands(state.hands);
    for (int i = 0; i < 4; ++i) {
        for (const auto &card: hands[i]) {
            this->outputFile << card << " ";
        }
    }

    this->outputFile << std::endl;
}

unsigned long BridgeModelLowerAbstr::getBeginningStatesCount() const {
    return beginningStatesCount;
}

void BridgeModelLowerAbstr::clear() {
    this->cardsAvailable.clear();
    this->epistemicStateClassNumber.clear();
    this->nextLevelStates.clear();
}

bool BridgeModelLowerAbstr::isWinningState(State &state) {
    return (state.lefts[0] > this->noEndCards / 2 && state.lefts[0] + state.lefts[1] == this->noEndCards);
}

const std::set<int> &BridgeModelLowerAbstr::getWinningStates() const {
    return winningStates;
}

void BridgeModelLowerAbstr::abstractState(BridgeModelLowerAbstr::State &state) {
    for (short i = 0; i < state.hands.size(); ++i) {
        for (short j = 0; j < state.hands[i].size(); ++j) {
            if (state.hands[i][j] < this->abstractionLevel) {
                state.hands[i][j] = (CARD_TYPE) (this->abstractionLevel + state.hands[i][j] % 10);
            }
        }
    }
}

CARD_TYPE BridgeModelLowerAbstr::abstractCard(CARD_TYPE card) {
    if (card < this->abstractionLevel) {
        return (CARD_TYPE) (this->abstractionLevel + card % 10);
    } else {
        return card;
    }
}



