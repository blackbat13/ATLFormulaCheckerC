//
// Created by blackbat on 06.04.17.
//

#include "BridgeModelTestSuite.h"

int BridgeModelTestSuite::parseLine(char *line) {
    int i = strlen(line);
    const char *p = line;
    while (*p < '0' || *p > '9') {
        p++;
    }

    line[i - 3] = '\0';
    i = atoi(p);
    return i;
}

double BridgeModelTestSuite::getVirtualMemory() {
    FILE *file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL) {
        if (strncmp(line, "VmSize:", 7) == 0) {
            result = parseLine(line);
            break;
        }
    }

    fclose(file);
    return result;
}

double BridgeModelTestSuite::getPhysicalMemory() {
    FILE *file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            result = parseLine(line);
            break;
        }
    }

    fclose(file);
    return result;
}

BridgeModelTestSuite::BridgeModelTestSuite(int numberOfTests, int noCardsAvailable, int noEndCards) : numberOfTests(
        numberOfTests), noEndCards(noEndCards), noCardsAvailable(noCardsAvailable) {

}

void BridgeModelTestSuite::startTests() {
    for (int i = 0; i < this->numberOfTests; ++i) {
        struct timespec start, finish;
        double elapsed;

        clock_gettime(CLOCK_MONOTONIC, &start);

        BridgeModel bridgeModel = BridgeModel(this->noCardsAvailable, this->noEndCards, BridgeModel::State(HANDS_TYPE(), LEFTS_TYPE(2,0), 0, BOARD_TYPE(4,-1), 0, HISTORY_TYPE(), 0, -1));

        printf("Current Virtual Memory used: %lf MB", getVirtualMemory()/1024);
        cout << "Current Virtual Memory used: " << getVirtualMemory()/1024 << " MB" << endl;
        cout << "Current Physical Memory used: " << getPhysicalMemory()/1024 << " MB" << endl;
    }
}
