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

using namespace std;

int main() {
    int noCardsAvailable = 2;
    int noEndCards = 2;
    int numberOfTests = 5;

    BridgeModelTestSuite testSuite = BridgeModelTestSuite(numberOfTests, noCardsAvailable, noEndCards);
    testSuite.startTests();
    testSuite.printStatistics();
//    testSuite.saveStatistics();
//    string a;
//    cin >> a;
    return 0;
}