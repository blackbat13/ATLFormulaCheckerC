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

// Return current virtual memory usage by process in KB
double BridgeModelTestSuite::getVirtualMemoryOsx() {
//    struct task_basic_info t_info;
//    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
//
//    if (KERN_SUCCESS != task_info(mach_task_self(),
//                                  TASK_BASIC_INFO, (task_info_t)&t_info,
//                                  &t_info_count))
//    {
//        return -1;
//    }
//
//    return t_info.virtual_size/1024;
    return 0;
}

// Return current physical memory usage by process in KB
double BridgeModelTestSuite::getPhysicalMemoryOsx() {
//    struct task_basic_info t_info;
//    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
//
//    if (KERN_SUCCESS != task_info(mach_task_self(),
//                                  TASK_BASIC_INFO, (task_info_t)&t_info,
//                                  &t_info_count))
//    {
//        return -1;
//    }
//
//    return t_info.resident_size/1024;
    return 0;
}

BridgeModelTestSuite::BridgeModelTestSuite(int numberOfTests, int noCardsAvailable, int noEndCards,
                                           int abstractionLevel) : numberOfTests(
        numberOfTests), noCardsAvailable(noCardsAvailable), noEndCards(noEndCards), abstractionLevel(abstractionLevel) {
    this->physicalMemorySum = 0;
    this->virtualMemorySum = 0;
    this->perfectFormulaTrueCount = 0;
    this->imperfectFormulaTrueCount = 0;
    this->formulaResultEqualCount = 0;
}

void BridgeModelTestSuite::startTests2() {
    for (int testNumber = 1; testNumber <= this->numberOfTests; ++testNumber) {
        struct timespec start, finish;
        double elapsed;
        std::ifstream file;
        std::string path = "../results/BridgeModel_" + std::to_string(this->noCardsAvailable) + "_" +
                           std::to_string(this->noEndCards) + "_" + this->abstractionLevel + "/";
        std::string filename = path + "Model_" + testNumber + ".txt";
        file.open(filename);
        if (!file.good()) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            BridgeModel bridgeModel = BridgeModel(this->noCardsAvailable, this->noEndCards,
                                                  BridgeModel::State(HANDS_TYPE(), LEFTS_TYPE(2, 0), 0,
                                                                     BOARD_TYPE(4, -1),
                                                                     0, 0, -1), this->abstractionLevel);
            clock_gettime(CLOCK_MONOTONIC, &finish);
            elapsed = (finish.tv_sec - start.tv_sec);
            elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;
            printf("Generated model in %fs\n", elapsed);
            this->modelGenerationTimeSum += elapsed;
            this->totalTimeSum += elapsed;
            this->physicalMemorySum += GET_PHYSICAL_MEMORY_FUNCTION();
            this->virtualMemorySum += GET_VIRTUAL_MEMORY_FUNCTION();
            printf("Current Virtual Memory used: %f MB\n", GET_VIRTUAL_MEMORY_FUNCTION() / 1024);
            printf("Current Physical Memory used: %f MB\n", GET_PHYSICAL_MEMORY_FUNCTION() / 1024);

            std::ofstream fileOut;
            fileOut.open(filename);
            bridgeModel.getModel().saveToFile(fileOut);
            fileOut.close();
            file.open(filename);
        }

        std::set<int> result;

        AtlModel atlModel = AtlModel(1, 1);
        atlModel.loadFromFile(file, false);
        clock_gettime(CLOCK_MONOTONIC, &start);
        result = atlModel.minimumFormulaOneAgentMultipleStatesPerfectInformation(0, atlModel.getWinningStates());
        clock_gettime(CLOCK_MONOTONIC, &finish);
        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;

        atlModel = AtlModel(1, 1);
        atlModel.loadFromFile(file, true);
        result = atlModel.minimumFormulaOneAgentMultipleStatesDisjoint(0, atlModel.getWinningStates());
    }
}

void BridgeModelTestSuite::startTests() {
    for (int testNumber = 1; testNumber <= this->numberOfTests; ++testNumber) {
        printf("----------TEST NUMBER %d----------\n", testNumber);
        struct timespec start, finish;
        double elapsed;

        clock_gettime(CLOCK_MONOTONIC, &start);

        BridgeModel bridgeModel = BridgeModel(this->noCardsAvailable, this->noEndCards,
                                              BridgeModel::State(HANDS_TYPE(), LEFTS_TYPE(2, 0), 0, BOARD_TYPE(4, -1),
                                                                 0, 0, -1), this->abstractionLevel);

        clock_gettime(CLOCK_MONOTONIC, &finish);
        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;
        printf("Generated model in %fs\n", elapsed);
        this->modelGenerationTimeSum += elapsed;
        this->totalTimeSum += elapsed;

        this->physicalMemorySum += GET_PHYSICAL_MEMORY_FUNCTION();
        this->virtualMemorySum += GET_VIRTUAL_MEMORY_FUNCTION();
        printf("Current Virtual Memory used: %f MB\n", GET_VIRTUAL_MEMORY_FUNCTION()/1024);
        printf("Current Physical Memory used: %f MB\n", GET_PHYSICAL_MEMORY_FUNCTION()/1024);

        clock_gettime(CLOCK_MONOTONIC, &start);
        std::set<int> result = bridgeModel.getModel().minimumFormulaOneAgentMultipleStatesDisjoint(0,
                                                                                                   bridgeModel.getWinningStates());
        clock_gettime(CLOCK_MONOTONIC, &finish);
        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;

        printf("Computed formula under imperfect information in %fs\n", elapsed);
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
            printf("Formula result (imperfect information): true\n");
            this->imperfectFormulaTrueCount++;
        } else {
            printf("Formula result (imperfect information: false\n");
        }

        clock_gettime(CLOCK_MONOTONIC, &start);
        result = bridgeModel.getModel().minimumFormulaOneAgentMultipleStatesPerfectInformation(0, bridgeModel.getWinningStates());
        clock_gettime(CLOCK_MONOTONIC, &finish);
        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_sec - start.tv_sec) / 1000000000.0;

        printf("Computed formula under perfect information in %fs\n", elapsed);
        this->formulaTimeSum += elapsed;
        this->totalTimeSum += elapsed;

        bool isOk2 = true;
        for(int i = 0; i < bridgeModel.getBeginningStatesCount(); ++i) {
            if(result.find(i) == result.end()) {
                isOk2 = false;
                break;
            }
        }

        if(isOk2) {
            printf("Formula result (perfect information): true\n");
            this->perfectFormulaTrueCount++;
        } else {
            printf("Formula result (perfect information): false\n");
        }

        if(isOk == isOk2) {
            this->formulaResultEqualCount++;
        }

        printf("----------TEST NUMBER %d----------\n", testNumber);
        printf("\n\n");
    }
}

void BridgeModelTestSuite::printStatistics() {
    printf("----------TEST STATISTICS----------\n");
    printf("Bridge Model (%d, %d)\n", this->noCardsAvailable, this->noEndCards);
    printf("Number of tests: %d\n", this->numberOfTests);
    printf("Virtual memory: %f MB\n", ((double)this->virtualMemorySum / (double)this->numberOfTests) / 1024);
    printf("Physical memory: %f MB\n", ((double)this->physicalMemorySum / (double)this->numberOfTests) / 1024);
    printf("Formula time: %fs\n", this->formulaTimeSum / (double)this->numberOfTests);
    printf("Generation time: %fs\n", this->modelGenerationTimeSum / (double)this->numberOfTests);
    printf("Total time: %fs\n", this->totalTimeSum / (double)this->numberOfTests);
    printf("Imperfect formula true percentage: %.2f%%\n", 100*this->imperfectFormulaTrueCount / (double)this->numberOfTests);
    printf("Perfect formula true percentage: %.2f%%\n", 100*this->perfectFormulaTrueCount / (double)this->numberOfTests);
    printf("Formula equality percentage: %.2f%%\n", 100*this->formulaResultEqualCount / (double)this->numberOfTests);
    printf("----------TEST STATISTICS----------\n");
    printf("\n\n");
}

void BridgeModelTestSuite::saveStatistics() {
    std::string filename = "../results/BridgeModel_" + std::to_string(this->noCardsAvailable) + "_" + std::to_string(this->noEndCards) + "_test_" + getCurrentDateTime() + "_" + OS + ".txt";
    FILE* file = fopen(filename.c_str(), "w");
    fprintf(file, "----------TEST STATISTICS----------\n");
    fprintf(file, "Bridge Model (%d, %d)\n", this->noCardsAvailable, this->noEndCards);
    fprintf(file, "Number of tests: %d\n", this->numberOfTests);
    fprintf(file, "Virtual memory: %f MB\n", ((double)this->virtualMemorySum / (double)this->numberOfTests) / 1024);
    fprintf(file, "Physical memory: %f MB\n", ((double)this->physicalMemorySum / (double)this->numberOfTests) / 1024);
    fprintf(file, "Formula time: %fs\n", this->formulaTimeSum / (double)this->numberOfTests);
    fprintf(file, "Generation time: %fs\n", this->modelGenerationTimeSum / (double)this->numberOfTests);
    fprintf(file, "Total time: %fs\n", this->totalTimeSum / (double)this->numberOfTests);
    fprintf(file, "Imperfect formula true percentage: %.2f%%\n", 100*this->imperfectFormulaTrueCount / (double)this->numberOfTests);
    fprintf(file, "Perfect formula true percentage: %.2f%%\n", 100*this->perfectFormulaTrueCount / (double)this->numberOfTests);
    fprintf(file, "Formula equality percentage: %.2f%%\n", 100*this->formulaResultEqualCount / (double)this->numberOfTests);
    fprintf(file, "----------TEST STATISTICS----------\n");
    fprintf(file, "\n\n");
    fclose(file);
}

std::string BridgeModelTestSuite::getCurrentDateTime() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y_%H:%M:%S",timeinfo);
    std::string str(buffer);
    return str;
}
