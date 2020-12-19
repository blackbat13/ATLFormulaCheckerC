#include <iostream>
#include "Models/BridgeModel.h"
#include "Models/asynchronous/GlobalModel.h"
#include "Models/asynchronous/parser/GlobalModelParser.h"
#include <sys/time.h>
#include <fstream>

using namespace std;

int main() {
    struct timeval tb, te;
    ofstream results("results.txt", fstream::app);
    auto model = GlobalModelParser::parse("Selene_2_2_2.txt");

    gettimeofday(&tb, NULL);
    model.generate(true);
    gettimeofday(&te, NULL);

    results << "Model generation time: " << 1000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec) / 1000
         << " milliseconds" << endl;
    results << "States Count: " << model.getStatesCount() << endl;
    results << "Transition Count: " << model.getTransitionsCount() << endl;

    gettimeofday(&tb, NULL);
    auto result = model.verifyApproximation(true, 1);
    gettimeofday(&te, NULL);

    results << "Imperfect verification time: " << 1000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec) / 1000
         << " milliseconds" << endl;

    if(*result.first.begin() == 0){
        results << "Imperfect verification result: TRUE" << endl;
    } else {
        results << "Imperfect verification result: FALSE" << endl;
    }

    gettimeofday(&tb, NULL);
    result = model.verifyApproximation(false, 1);
    gettimeofday(&te, NULL);

    results << "Perfect verification time: " << 1000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec) / 1000
         << " milliseconds" << endl;

    if(*result.first.begin() == 0){
        results << "Perfect verification result: TRUE" << endl;
    } else {
        results << "Perfect verification result: FALSE" << endl;
    }

    gettimeofday(&tb, NULL);
    auto result2 = model.verifyParallel(1);
    gettimeofday(&te, NULL);

    results << "Recursive DominoDFS time: " << 1000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec) / 1000
         << " milliseconds" << endl;

    if (result2.first) {
        results << "Recursive DominoDFS result: TRUE" << endl;
    } else {
        results << "Recursive DominoDFS result: FALSE" << endl;
    }

    results.close();

    return 0;
}