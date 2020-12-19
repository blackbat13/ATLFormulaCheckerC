#include <iostream>
#include "Models/BridgeModel.h"
#include <cstdio>
#include "Models/asynchronous/GlobalModel.h"
#include "Models/asynchronous/parser/GlobalModelParser.h"

using namespace std;

int main() {
    //auto model = GlobalModelParser::parse("train_controller.txt");
    auto model = GlobalModelParser::parse("Selene_2_1_2.txt");

    model.generate(true);


    printf("States Count: %d\n", model.getStatesCount());
    printf("Transition Count: %d\n", model.getTransitionsCount());

    printf("Approximation Imperfect Information\n");
    auto result = model.verifyApproximation(true, 1);
    for(auto stateId : result.first) {
        printf("%d ", stateId);
    }

    printf("\n");

    printf("Approximation Perfect Information\n");
    result = model.verifyApproximation(false, 1);
    for(auto stateId : result.first) {
        printf("%d ", stateId);
    }

    printf("\n");

    printf("Recursive verification\n");
    auto result2 = model.verifyParallel(1);

    printf("Result: %d\n", result2.first);

//    model.print();

//    auto simpleModel = model.getModel();
//    simpleModel.simulate(0);

    return 0;
}