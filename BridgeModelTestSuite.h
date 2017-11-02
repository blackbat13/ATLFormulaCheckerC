//
// Created by blackbat on 06.04.17.
//

#ifndef ATLFORMULACHECKER_BRIDGEMODELTESTSUITE_H
#define ATLFORMULACHECKER_BRIDGEMODELTESTSUITE_H

#include "BridgeModel.h"
#include <cstdio>
#include <cstring>
#include <set>
#include <ctime>
#include <fstream>

#ifdef _WIN32
#define OS "Windows"
#elif __APPLE__
#include<mach/mach.h>
#define OS "MACOS"
#define GET_VIRTUAL_MEMORY_FUNCTION getVirtualMemoryOsx
#define GET_PHYSICAL_MEMORY_FUNCTION getPhysicalMemoryOsx
#elif __linux__
#define OS "Linux"
#define GET_VIRTUAL_MEMORY_FUNCTION getVirtualMemory
#define GET_PHYSICAL_MEMORY_FUNCTION getPhysicalMemory
#endif

class BridgeModelTestSuite {
private:
    long long virtualMemorySum;
    long long physicalMemorySum;
    double formulaTimeSum;
    double totalTimeSum;
    double modelGenerationTimeSum;
    double imperfectFormulaTrueCount;
    double perfectFormulaTrueCount;
    double formulaResultEqualCount;
    int numberOfTests;
    int noCardsAvailable;
    int noEndCards;
    int abstractionLevel;
    std::string path;
    FILE *resultFile;

    int parseLine(char* line);
    double getVirtualMemory();
    double getPhysicalMemory();
    double getVirtualMemoryOsx();
    double getPhysicalMemoryOsx();
    std::string getCurrentDateTime();
public:
    BridgeModelTestSuite(int numberOfTests, int noCardsAvailable, int noEndCards, int abstractionLevel = 0);
    void startTests();

    void startTests2();
    void printStatistics();
    void saveStatistics();
};


#endif //ATLFORMULACHECKER_BRIDGEMODELTESTSUITE_H
