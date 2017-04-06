#include <iostream>
#include <vector>
#include <set>
#include <ctime>
#include <iomanip>
#include "BridgeModel.h"
#include "sys/types.h"
#include "sys/sysinfo.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

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
    int noCardsAvailable;
    int noEndCards;

    int m, n;
    m = 4;
    n = 4;

    struct timespec start, finish;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);

    BridgeModel bridgeModel = BridgeModel(m, n, BridgeModel::State(HANDS_TYPE(), LEFTS_TYPE(2,0), 0, BOARD_TYPE(4,-1), 0, HISTORY_TYPE(), 0, -1));

    cout << "Current Virtual Memory used: " << getVirtualMemory()/1024 << " MB" << endl;
    cout << "Current Physical Memory used: " << getPhysicalMemory()/1024 << " MB" << endl;

    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;

    cout << "Generated model in " << setprecision(10) << elapsed << "s" << endl;

    set<int> winningStates;
    for(int i = 0; i < bridgeModel.getStates().size(); ++i) {
        BridgeModel::State state = bridgeModel.getStates()[i];
        if(state.lefts[0] > m/2 && state.lefts[0] + state.lefts[1] == m) {
            winningStates.insert(i);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    set<int> result = bridgeModel.getModel().minimumFormulaOneAgentMultipleStatesDisjoint(0, winningStates);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;

    cout << "Computed formula in " << setprecision(10) << elapsed << "s" << endl;

    cout << "Size: " << result.size() << endl;
    bool isOk = true;
    for(int i = 0; i < bridgeModel.getBeginningStatesCount(); ++i) {
        if(result.find(i) == result.end()) {
            isOk = false;
            break;
        }
    }

    if(isOk) {
        cout << "Formula result: true" << endl;
    } else {
        cout << "Formula result: false" << endl;
    }

//    string a;
//    cin >> a;
    return 0;
}