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
    int noCardsAvailable = 1;
    int noEndCards = 1;
    int numberOfTests = 20;

    BridgeModelTestSuite testSuite = BridgeModelTestSuite(numberOfTests, noCardsAvailable, noEndCards, 0);
    testSuite.startTests2();
    testSuite.printStatistics();
    testSuite.saveStatistics();
//    SeleneModel seleneModel = SeleneModel(2, 2, 2);

//    int k = 0;

//    string a;
//    cin >> a;
    return 0;
}