//
// Created by Damian Kurpiewski on 13.11.2017.
//

#ifndef ATLFORMULACHECKER_BRIDGEMODELABSTRTESTSUITE_H
#define ATLFORMULACHECKER_BRIDGEMODELABSTRTESTSUITE_H

#include "BridgeModelLowerAbstr.h"
#include "BridgeModelUpperAbstr.h"
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

class BridgeModelAbstrTestSuite {
private:
    long long virtualMemorySum;
    long long physicalMemorySum;
    double formulaTimeSum;
    double perfectFormulaTimeSum;
    double imperfectFormulaTimeSum;
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

    int parseLine(char *line);

    double getVirtualMemory();

    double getPhysicalMemory();

    double getVirtualMemoryOsx();

    double getPhysicalMemoryOsx();

    std::string getCurrentDateTime();

public:
    BridgeModelAbstrTestSuite(int numberOfTests, int noCardsAvailable, int noEndCards, int abstractionLevel = 0);

    void startTests();

    void printStatistics();

    void saveStatistics();
};


#endif //ATLFORMULACHECKER_BRIDGEMODELABSTRTESTSUITE_H
