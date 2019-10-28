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
    for (int testNumber = 1; testNumber <= this->numberOfTests; ++testNumber) {
        printf("----------TEST NUMBER %d----------\n", testNumber);
        struct timespec start, finish;
        double elapsed;

        clock_gettime(CLOCK_MONOTONIC, &start);

        BridgeModel bridgeModel = BridgeModel(this->noCardsAvailable, this->noEndCards, BridgeModel::State(HANDS_TYPE(), LEFTS_TYPE(2,0), 0, BOARD_TYPE(4,-1), 0, HISTORY_TYPE(), 0, -1));

        clock_gettime(CLOCK_MONOTONIC, &finish);
        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;
        printf("Generated model in %fs\n", elapsed);
        this->modelGenerationTimeSum += elapsed;
        this->totalTimeSum += elapsed;

        this->physicalMemorySum += getPhysicalMemory();
        this->virtualMemorySum += getVirtualMemory();
        printf("Current Virtual Memory used: %f MB\n", getVirtualMemory()/1024);
        printf("Current Physical Memory used: %f MB\n", getPhysicalMemory()/1024);


        std::set<unsigned int> winningStates;
        for(int i = 0; i < bridgeModel.getStates().size(); ++i) {
            BridgeModel::State state = bridgeModel.getStates()[i];
            if(state.lefts[0] > this->noCardsAvailable/2 && state.lefts[0] + state.lefts[1] == this->noCardsAvailable) {
                winningStates.insert(i);
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &start);
        std::set<unsigned int> result = bridgeModel.getModel().minimumFormulaOneAgentMultipleStatesDisjoint(0, winningStates);
        clock_gettime(CLOCK_MONOTONIC, &finish);
        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;

        printf("Computed formula in %fs\n", elapsed);
        this->formulaTimeSum += elapsed;
        this->totalTimeSum += elapsed;

        bool isOk = true;
        for(int i = 0; i < bridgeModel.getBeginningStatesCount(); ++i) {
            if(result.find(i) == result.end()) {
                isOk = false;
                break;
            }
        }

        if(isOk) {
            printf("Formula result: true\n");
        } else {
            printf("Formula result: false\n");
        }

        printf("----------TEST NUMBER %d----------\n", testNumber);
        printf("\n\n");
    }
}

void BridgeModelTestSuite::printStatistics() {
    printf("----------TEST STATISTICS----------\n");
    printf("Bridge Model (%d, %d)\n", this->noCardsAvailable, this->noEndCards);
    printf("Number of tests: %d\n", this->numberOfTests);
    printf("Virtual memory: %f KB\n", (double)this->virtualMemorySum / (double)this->numberOfTests);
    printf("Physical memory: %f KB\n", (double)this->physicalMemorySum / (double)this->numberOfTests);
    printf("Formula time: %fs\n", this->formulaTimeSum / (double)this->numberOfTests);
    printf("Generation time: %fs\n", this->modelGenerationTimeSum / (double)this->numberOfTests);
    printf("Total time: %fs\n", this->totalTimeSum / (double)this->numberOfTests);
    printf("----------TEST STATISTICS----------\n");
    printf("\n\n");
}

void BridgeModelTestSuite::saveStatistics() {
    std::string filename = "./results/BridgeModel_" + std::to_string(this->noCardsAvailable) + "_" + std::to_string(this->noEndCards) + "_test.txt";
    FILE* file = fopen(filename.c_str(), "rw");
    fprintf(file, "----------TEST STATISTICS----------\n");
    fprintf(file, "Bridge Model (%d, %d)\n", this->noCardsAvailable, this->noEndCards);
    fprintf(file, "Number of tests: %d\n", this->numberOfTests);
    fprintf(file, "Virtual memory: %f KB\n", (double)this->virtualMemorySum / (double)this->numberOfTests);
    fprintf(file, "Physical memory: %f KB\n", (double)this->physicalMemorySum / (double)this->numberOfTests);
    fprintf(file, "Formula time: %fs\n", this->formulaTimeSum / (double)this->numberOfTests);
    fprintf(file, "Generation time: %fs\n", this->modelGenerationTimeSum / (double)this->numberOfTests);
    fprintf(file, "Total time: %fs\n", this->totalTimeSum / (double)this->numberOfTests);
    fprintf(file, "----------TEST STATISTICS----------\n");
    fprintf(file, "\n\n");
    fclose(file);
}
