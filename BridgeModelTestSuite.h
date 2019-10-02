//
// Created by blackbat on 06.04.17.
//

#ifndef ATLFORMULACHECKER_BRIDGEMODELTESTSUITE_H
#define ATLFORMULACHECKER_BRIDGEMODELTESTSUITE_H

#include "Models/BridgeModel.h"
#include <cstdio>
#include <cstring>
#include <set>

class BridgeModelTestSuite {
private:
    long long virtualMemorySum;
    long long physicalMemorySum;
    double formulaTimeSum;
    double totalTimeSum;
    double modelGenerationTimeSum;
    int numberOfTests;
    int noCardsAvailable;
    int noEndCards;

    int parseLine(char* line);
    double getVirtualMemory();
    double getPhysicalMemory();
public:
    BridgeModelTestSuite(int numberOfTests, int noCardsAvailable, int noEndCards);
    void startTests();
    void printStatistics();
    void saveStatistics();
};


#endif //ATLFORMULACHECKER_BRIDGEMODELTESTSUITE_H
