#include <iostream>
#include <vector>
#include <set>
#include <ctime>
#include <iomanip>
#include "Models/BridgeModel.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
//#include "BridgeModelTestSuite.h"
#include "Models/asynchronous/GlobalModel.h"
#include "Models/asynchronous/parser/GlobalModelParser.h"

using namespace std;

//void runTests() {
//    int noCardsAvailable = 2;
//    int noEndCards = 2;
//    int numberOfTests = 1;
//
//    BridgeModelTestSuite testSuite = BridgeModelTestSuite(numberOfTests, noCardsAvailable, noEndCards);
//    testSuite.startTests();
//    testSuite.printStatistics();
//    testSuite.saveStatistics();
//}
//
//ParallelModel createModel() {
//    int n = 2;
//    int noCardsAvailable = n;
//    int noEndCards = n;
//    BridgeModel bridgeModel = BridgeModel(noCardsAvailable, noEndCards, BridgeModel::State(HANDS_TYPE(), LEFTS_TYPE(2,0), 0, BOARD_TYPE(4,-1), 0, HISTORY_TYPE(), 0, -1));
//    return bridgeModel.getModel().toParallelModel(0);
//}

int main() {
//    runTests();
//    auto model = createModel();
//
//    for(auto state : model.states) {
//        cout << "State Id: " << state.state_id << endl;
//        cout << "Epistemic Class Id: " << state.equivalence_vector[0] << endl;
//        for(int i = 0; i < state.action_list.size(); i++) {
//            cout << "Transition:" << endl;
//            cout << "Action: " << state.action_list[i][0] << endl;
//            cout << "State: " << state.state_list[i] << endl;
//        }
//
//        cout << endl;
//    }
//
//    cout << endl << endl;
//    for(auto epClass: model.abstraction_classes) {
//        cout << "Epistemic Class Id: " << epClass.class_id << endl;
//        cout << "States:" << endl;
//        for (auto stateId : epClass.members) {
//            cout << stateId << endl;
//        }
//    }
//    string a;
//    cin >> a;


    auto model = GlobalModelParser::parse("train_controller.txt");

    model.generate(false);

    model.print();

    auto simpleModel = model.getModel();
    simpleModel.simulate(0);

    return 0;
}