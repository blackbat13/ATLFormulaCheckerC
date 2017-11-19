#include <iostream>
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

int main() {
    int noCardsAvailable = 13;
    int noEndCards = 4;
    int numberOfTests = 20;

//    BridgeModelTestSuite testSuite = BridgeModelTestSuite(numberOfTests, noCardsAvailable, noEndCards, 0);
//    testSuite.startTests2();
//    testSuite.printStatistics();
//    testSuite.saveStatistics();

    BridgeModelAbstrTestSuite testSuite = BridgeModelAbstrTestSuite(numberOfTests, noCardsAvailable, noEndCards, 90);
    testSuite.startTests();
    testSuite.printStatistics();
    testSuite.saveStatistics();

//    SeleneModel seleneModel = SeleneModel(2, 2, 2);

//    int k = 0;

//    string a;
//    cin >> a;
    return 0;
}