#include <iostream>
#include "Models/BridgeModel.h"
#include "Models/asynchronous/GlobalModel.h"
#include "Models/asynchronous/parser/GlobalModelParser.h"
#include <sys/time.h>
#include <fstream>

using namespace std;

int main(int argc, char **argv) {
    if(argc < 6) {
        cout << "Usage: ./stv tellers voters candidates reductions formula" << endl;
        cout << "tellers - number of tellers" << endl;
        cout << "voters - number of voters" << endl;
        cout << "candidates - number of candidates" << endl;
        cout << "reductions - 1 for reduced model, 0 otherwise" << endl;
        cout << "formula - formula id" << endl;
        cout << "Example: ./stv 2 1 2 1 1" << endl;
        cout << "Program will now exit" << endl;
        return 0;
    }

    struct timeval tb, te;
    int tellers, voters, candidates, formula;
    bool reductions;

    tellers = atoi(argv[1]);
    voters = atoi(argv[2]);
    candidates = atoi(argv[3]);
    formula = atoi(argv[4]);
    reductions = argv[5][0] == '1';

    ofstream results("results.txt", fstream::app);

    results << "Configuration: (T: " << tellers << ", V: " << voters << ", C: " << candidates << ", R: " << reductions << ")" << endl;
    results << "Formula: " << formula << endl;

    auto model = GlobalModelParser::parse("Selene_" + to_string(tellers) + "_"+to_string(voters)+"_"+to_string(candidates)+".txt");

    gettimeofday(&tb, NULL);
    model.generate(reductions);
    gettimeofday(&te, NULL);

    results << "Model generation time: " << 1000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec) / 1000
         << " milliseconds" << endl;
    results << "States Count: " << model.getStatesCount() << endl;
    results << "Transition Count: " << model.getTransitionsCount() << endl;

    gettimeofday(&tb, NULL);
    auto result = model.verifyApproximation(true, formula);
    gettimeofday(&te, NULL);

    results << "Imperfect verification time: " << 1000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec) / 1000
         << " milliseconds" << endl;

    if(*result.first.begin() == 0){
        results << "Imperfect verification result: TRUE" << endl;
    } else {
        results << "Imperfect verification result: FALSE" << endl;
    }

    gettimeofday(&tb, NULL);
    result = model.verifyApproximation(false, formula);
    gettimeofday(&te, NULL);

    results << "Perfect verification time: " << 1000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec) / 1000
         << " milliseconds" << endl;

    if(*result.first.begin() == 0){
        results << "Perfect verification result: TRUE" << endl;
    } else {
        results << "Perfect verification result: FALSE" << endl;
    }

    gettimeofday(&tb, NULL);
    auto result2 = model.verifyParallel(formula);
    gettimeofday(&te, NULL);

    results << "Recursive DominoDFS time: " << 1000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec) / 1000
         << " milliseconds" << endl;

    if (result2.first) {
        results << "Recursive DominoDFS result: TRUE" << endl;
    } else {
        results << "Recursive DominoDFS result: FALSE" << endl;
    }

    results << "Model END" << endl << endl;
    results.close();

    return 0;
}