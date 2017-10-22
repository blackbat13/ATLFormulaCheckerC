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
#include "SeleneModel.h"

using namespace std;

int main() {
    int noCardsAvailable = 13;
    int noEndCards = 5;
    int numberOfTests = 1;

    BridgeModelTestSuite testSuite = BridgeModelTestSuite(numberOfTests, noCardsAvailable, noEndCards, 100);
    testSuite.startTests();
    testSuite.printStatistics();
    testSuite.saveStatistics();
//    SeleneModel seleneModel = SeleneModel(2, 2, 2);

//    int k = 0;

//    string a;
//    cin >> a;
    return 0;
}