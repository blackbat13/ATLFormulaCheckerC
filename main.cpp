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

int parseLine(char* line) {
    int i = strlen(line);
    const char* p = line;
    while (*p < '0' || *p > '9') {
        p++;
    }

    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

double getVirtualMemory() {
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while(fgets(line, 128, file) != NULL) {
        if(strncmp(line, "VmSize:", 7) == 0) {
            result = parseLine(line);
            break;
        }
    }

    fclose(file);
    return result;
}

double getPhysicalMemory() {
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while(fgets(line, 128, file) != NULL) {
        if(strncmp(line, "VmRSS:", 6) == 0) {
            result = parseLine(line);
            break;
        }
    }

    fclose(file);
    return result;
}


int main() {
    int noCardsAvailable = 3;
    int noEndCards = 3;
    int numberOfTests = 1;

    BridgeModelTestSuite testSuite = BridgeModelTestSuite(numberOfTests, noCardsAvailable, noEndCards);
    testSuite.startTests();
//    string a;
//    cin >> a;
    return 0;
}