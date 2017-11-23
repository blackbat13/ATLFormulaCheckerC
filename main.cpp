#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <ctime>
#include <iomanip>
#include "BridgeModel.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "BridgeModelTestSuite.h"
#include "BridgeModelAbstrTestSuite.h"
#include "SeleneModel.h"

#include "cppitertools/itertools.hpp"

using namespace std;

std::vector<std::vector<short> > cartessianProduct(std::vector<std::vector<short> > &array) {
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

int main() {
    int noCardsAvailable = 5;
    int noEndCards = 5;
    int numberOfTests = 20;

//    BridgeModelTestSuite testSuite = BridgeModelTestSuite(numberOfTests, noCardsAvailable, noEndCards, 0);
//    testSuite.startTests2();
//    testSuite.printStatistics();
//    testSuite.saveStatistics();

    // BridgeModelAbstrTestSuite testSuite = BridgeModelAbstrTestSuite(numberOfTests, noCardsAvailable, noEndCards, 90);
    // testSuite.startTests();
    // testSuite.printStatistics();
    // testSuite.saveStatistics();

    vector<vector<short> > testsSpec;
    vector<short> test;
    test.push_back(1);
    test.push_back(2);
    test.push_back(3);
    test.push_back(4);
    test.push_back(5);
    test.push_back(6);
    testsSpec.push_back(test);
    test.clear();

    test.push_back(1);
    test.push_back(2);
    test.push_back(3);
    test.push_back(4);
    test.push_back(5);
    test.push_back(6);
    testsSpec.push_back(test);
    test.clear();

    test.push_back(1);
    test.push_back(2);
    test.push_back(3);
    test.push_back(4); //if
    testsSpec.push_back(test);
    test.clear();

    //test.push_back(1);
    test.push_back(2);
    test.push_back(3);
    testsSpec.push_back(test);
    test.clear();


//    test.push_back(2);
    test.push_back(3);
    test.push_back(4);
    testsSpec.push_back(test);
    test.clear();





//    auto possibilities = cartessianProduct(testsSpec);
//    for (auto possibility : possibilities) {
//
//
//        struct timespec start, finish;
//        double elapsed;
//        short noVoters = possibility[4];
//        short noBallots = possibility[3];
//        short maxCoerced = possibility[2];
//        short maxWaitingForVotes = possibility[1];
//        short maxWaitingForHelp = possibility[0];
//
//        if (maxCoerced > noVoters) {
//            continue;
//        }
//
//        printf("Now testing %d %d %d %d %d\n", noVoters, noBallots, maxCoerced, maxWaitingForVotes, maxWaitingForHelp);
//
//        ofstream resultFile("../results/SeleneModel_" + to_string(noVoters) + "_" + to_string(noBallots) + "_" +
//                            to_string(maxCoerced) + "_" + to_string(maxWaitingForVotes) + "_" +
//                            to_string(maxWaitingForHelp) + "_result.txt");
//
//        clock_gettime(CLOCK_MONOTONIC, &start);
//        SeleneModel seleneModel = SeleneModel(noVoters, noBallots, maxCoerced, maxWaitingForVotes, maxWaitingForHelp);
//        clock_gettime(CLOCK_MONOTONIC, &finish);
//
//        elapsed = (finish.tv_sec - start.tv_sec);
//        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;
//        resultFile << "Generated model in " << elapsed << " s" << endl;
//        resultFile << "Number of states: " << seleneModel.states.size() << endl;
//
//        seleneModel.clear();
//
//        clock_gettime(CLOCK_MONOTONIC, &start);
//        auto result = seleneModel.getModel().minimumFormulaOneAgentMultipleStatesPerfectInformation(0,
//                                                                                                    seleneModel.formula1WinningStates);
//        clock_gettime(CLOCK_MONOTONIC, &finish);
//
//        elapsed = (finish.tv_sec - start.tv_sec);
//        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;
//        resultFile << "Formula 1 perfect time: " << elapsed << " s" << endl;
//
//        if (result.find(0) != result.end()) {
//            resultFile << "Formula 1 perfect result: true" << endl;
//        } else {
//            resultFile << "Formula 1 perfect result: false" << endl;
//        }
//
//        clock_gettime(CLOCK_MONOTONIC, &start);
//        result = seleneModel.getModel().minimumFormulaOneAgentMultipleStatesPerfectInformation(0,
//                                                                                               seleneModel.formula2WinningStates);
//        clock_gettime(CLOCK_MONOTONIC, &finish);
//
//        elapsed = (finish.tv_sec - start.tv_sec);
//        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;
//        resultFile << "Formula 2 perfect time: " << elapsed << " s" << endl;
//
//        if (result.find(0) != result.end()) {
//            resultFile << "Formula 2 perfect result: true" << endl;
//        } else {
//            resultFile << "Formula 2 perfect result: false" << endl;
//        }
//
//        clock_gettime(CLOCK_MONOTONIC, &start);
//        result = seleneModel.getModel().minimumFormulaOneAgentMultipleStatesPerfectInformation(0,
//                                                                                               seleneModel.formula3WinningStates);
//        clock_gettime(CLOCK_MONOTONIC, &finish);
//
//        elapsed = (finish.tv_sec - start.tv_sec);
//        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;
//        resultFile << "Formula 3 perfect time: " << elapsed << " s" << endl;
//
//        if (result.find(0) != result.end()) {
//            resultFile << "Formula 3 perfect result: true" << endl;
//        } else {
//            resultFile << "Formula 3 perfect result: false" << endl;
//        }
//
//        clock_gettime(CLOCK_MONOTONIC, &start);
//        result = seleneModel.getModel().minimumFormulaOneAgentMultipleStatesPerfectInformation(0,
//                                                                                               seleneModel.formula4WinningStates);
//        clock_gettime(CLOCK_MONOTONIC, &finish);
//
//        elapsed = (finish.tv_sec - start.tv_sec);
//        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;
//        resultFile << "Formula 4 perfect time: " << elapsed << " s" << endl;
//
//        if (result.find(0) != result.end()) {
//            resultFile << "Formula 4 perfect result: true" << endl;
//        } else {
//            resultFile << "Formula 4 perfect result: false" << endl;
//        }
//
//        clock_gettime(CLOCK_MONOTONIC, &start);
//        result = seleneModel.getModel().minimumFormulaOneAgentMultipleStatesDisjoint(0,
//                                                                                     seleneModel.formula1WinningStates);
//        clock_gettime(CLOCK_MONOTONIC, &finish);
//
//        elapsed = (finish.tv_sec - start.tv_sec);
//        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;
//        resultFile << "Formula 1 imperfect time: " << elapsed << " s" << endl;
//
//        if (result.find(0) != result.end()) {
//            resultFile << "Formula 1 imperfect result: true" << endl;
//        } else {
//            resultFile << "Formula 1 imperfect result: false" << endl;
//        }
//
//        clock_gettime(CLOCK_MONOTONIC, &start);
//        result = seleneModel.getModel().minimumFormulaOneAgentMultipleStatesDisjoint(0,
//                                                                                     seleneModel.formula2WinningStates);
//        clock_gettime(CLOCK_MONOTONIC, &finish);
//
//        elapsed = (finish.tv_sec - start.tv_sec);
//        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;
//        resultFile << "Formula 2 imperfect time: " << elapsed << " s" << endl;
//
//        if (result.find(0) != result.end()) {
//            resultFile << "Formula 2 imperfect result: true" << endl;
//        } else {
//            resultFile << "Formula 2 imperfect result: false" << endl;
//        }
//
//        clock_gettime(CLOCK_MONOTONIC, &start);
//        result = seleneModel.getModel().minimumFormulaOneAgentMultipleStatesDisjoint(0,
//                                                                                     seleneModel.formula3WinningStates);
//        clock_gettime(CLOCK_MONOTONIC, &finish);
//
//        elapsed = (finish.tv_sec - start.tv_sec);
//        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;
//        resultFile << "Formula 3 imperfect time: " << elapsed << " s" << endl;
//
//        if (result.find(0) != result.end()) {
//            resultFile << "Formula 3 imperfect result: true" << endl;
//        } else {
//            resultFile << "Formula 3 imperfect result: false" << endl;
//        }
//
//        clock_gettime(CLOCK_MONOTONIC, &start);
//        result = seleneModel.getModel().minimumFormulaOneAgentMultipleStatesDisjoint(0,
//                                                                                     seleneModel.formula4WinningStates);
//        clock_gettime(CLOCK_MONOTONIC, &finish);
//
//        elapsed = (finish.tv_sec - start.tv_sec);
//        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;
//        resultFile << "Formula 4 imperfect time: " << elapsed << " s" << endl;
//
//        if (result.find(0) != result.end()) {
//            resultFile << "Formula 4 imperfect result: true" << endl;
//        } else {
//            resultFile << "Formula 4 imperfect result: false" << endl;
//        }
//
//        resultFile.close();
//    }


    struct timespec start, finish;
    double elapsed;
    short noVoters = 2;
    short noBallots = 2;
    short maxCoerced = 2;
    short maxWaitingForVotes = 1;
    short maxWaitingForHelp = 1;

    ofstream resultFile("../results/SeleneModel_" + to_string(noVoters) + "_" + to_string(noBallots) + "_" +
                        to_string(maxCoerced) + "_" + to_string(maxWaitingForVotes) + "_" +
                        to_string(maxWaitingForHelp) + "_percentage_result.txt");

    clock_gettime(CLOCK_MONOTONIC, &start);
    SeleneModel seleneModel = SeleneModel(noVoters, noBallots, maxCoerced, maxWaitingForVotes, maxWaitingForHelp);
    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;
    resultFile << "Generated model in " << elapsed << " s" << endl;
    resultFile << "Number of states: " << seleneModel.states.size() << endl;

    seleneModel.clear();

    clock_gettime(CLOCK_MONOTONIC, &start);
    auto result = seleneModel.getModel().minimumFormulaOneAgentMultipleStatesDisjoint(0,
                                                                                      seleneModel.formula4WinningStates);
    clock_gettime(CLOCK_MONOTONIC, &finish);

    cout << "Number of result states: " << result.size() << endl;

    resultFile << setprecision(10);
    cout << setprecision(10);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;
    resultFile << "Formula 4 imperfect time: " << elapsed << " s" << endl;

    if (result.find(0) != result.end()) {
        resultFile << "Formula 4 imperfect result: true" << endl;
    } else {
        resultFile << "Formula 4 imperfect result: false" << endl;
    }

    int count = 0;
    for (auto s: seleneModel.configurationStates) {
        if (result.find(s) != result.end()) {
            ++count;
        }
    }


    resultFile << "Number of reached configuration states: " << count << endl;
    resultFile << "Number of all configuration states: " << seleneModel.configurationStates.size() << endl;
    resultFile << "Percentage of configuration states: "
               << (100.0 * (double) count / (double) seleneModel.configurationStates.size()) << "%" << endl;

    resultFile.close();

    return 0;
}

