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
    int noCardsAvailable = 3;
    int noEndCards = 3;
    int numberOfTests = 1;

    BridgeModelTestSuite testSuite = BridgeModelTestSuite(numberOfTests, noCardsAvailable, noEndCards);
    testSuite.startTests();
    testSuite.printStatistics();
    testSuite.saveStatistics();
//    string a;
//    cin >> a;
    return 0;
}