#include <iostream>
#include "Models/BridgeModel.h"
#include "Models/asynchronous/GlobalModel.h"
#include "Models/asynchronous/parser/GlobalModelParser.h"
#include <sys/time.h>
#include <fstream>
#include <cstring>
#include <thread>

using namespace std;

int main(int argc, char **argv) {
//    if (argc < 6) {
//        cout << "Usage: ./stv tellers voters candidates reductions formula" << endl;
//        cout << "tellers (>=1) - number of tellers" << endl;
//        cout << "voters (>=1) - number of voters" << endl;
//        cout << "candidates (>=2) - number of candidates" << endl;
//        cout << "reductions (0-1) - 1 for reduced model, 0 otherwise" << endl;
//        cout << "formula (0-4) - formula id" << endl;
//        cout << "\t0 - <<Coercer>> F pun1" << endl;
//        cout << "\t1 - <<Coercer>> F (end && !v1=1 && K_Coercer !v1=1)" << endl;
//        cout << "\t2 - <<Voter1>> F (end && K_Voter vreg1=1 && K_Voter vreg1!=1)" << endl;
//        cout << "\t3 - phi1" << endl;
//        cout << "\t4 - phi2" << endl;
//        cout << "Example: ./stv 2 1 2 1 0" << endl;
//        cout << "Program will now exit" << endl;
//        return 0;
//    }
//
//    struct timeval tb, te;
//    int tellers, voters, candidates, formula;
//    bool reductions;
//
//    tellers = atoi(argv[1]);
//    voters = atoi(argv[2]);
//    candidates = atoi(argv[3]);
//    reductions = argv[4][0] == '1';
//    formula = atoi(argv[5]);
//
//    ofstream results("results.txt", fstream::app);
//
//    results << "Configuration: (T: " << tellers << ", V: " << voters << ", C: " << candidates << ", R: " << reductions
//            << ")" << endl;
//
//    if(formula == 0) {
//        results << "Formula: <<Coercer>> F pun1" << endl;
//    } else if(formula == 1) {
//        results << "Formula: <<Coercer>> F (end && !v1=1 && K_Coercer !v1=1)" << endl;
//    } else if(formula == 2) {
//        results << "Formula: <<Voter1>> F (end && (K_Voter vreg1=1 || K_Voter vreg1!=1))" << endl;
//    } else if(formula == 3) {
//        results << "Formula: phi1" << endl;
//    } else if(formula == 4) {
//        results << "Formula: phi2" << endl;
//    }
//
//

    cout << thread::hardware_concurrency() << endl;
    struct timeval tb, te;
    auto model = SimpleModel(argv[1]);
//    auto res = model.verifyApproximationImperfect();
//
//    for(auto stateId : res) {
//        cout << stateId << " ";
//    }
//    cout << endl;
//
//
    int t = 5;
    if (argc >= 3) {
        t = atoi(argv[2]);
    }

    bool imperfect = true;
    if (argc >= 4 && strcmp(argv[3], "-p") == 0) {
        imperfect = false;
    }


//
//    model.printStats();
//    model.simulate(model.getAgentId());

    auto parallel = model.toParallelModel(imperfect);

    cout << "Parallel states size: " << parallel->states.size() << endl;

//    parallel->states[1]->setAccept();

    cout << "#" << parallel->threadsVector.size() << endl;
    bool result;

    unsigned long long czas = 0;
    int n = 1;
    for(int i = 0; i < n; i++) {

        gettimeofday(&tb, NULL);

        result = parallel->forkRecursiveDFS(0, t);

        // pobierz bieżący czas
        gettimeofday(&te, NULL);

        czas += 1000000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec);
    }

    czas /= n;


    // ile to trwało
    cout << "polechali: " << czas << " usec" << endl;

    // no i czy znaleziono
    if (result) cout << "jest" << endl;
    else cout << "nie ma" << endl;
//    model.simulate(model.getAgentId());
    return 0;

//    auto model2 = GlobalModelParser::parse("train_controller.txt");
//    model2.generate(false);
//    for(auto state : model2.states) {
//        state.print();
//    }
//
//    auto sModel = model2.getModel();
//    sModel.simulate(2);
//    cout << "Hello World!" << endl;
//    return 0;
//
//
//    auto model = GlobalModelParser::parse(
//            "Selene_" + to_string(tellers) + "_" + to_string(voters) + "_" + to_string(candidates) + "_" +
//            to_string(formula) + ".txt");
//
////    model.print();
//
//    gettimeofday(&tb, NULL);
//    model.generate(reductions);
//    gettimeofday(&te, NULL);
//
//
//
////    for(auto state : model.states) {
////        state.print();
////    }
//
//    results << "Model generation time: " << 1000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec) / 1000
//            << " milliseconds" << endl;
//    results << "States Count: " << model.getStatesCount() << endl;
//    results << "Transition Count: " << model.getTransitionsCount() << endl;
//
////    auto m = model.getModel();
////    m.simulate(model.agentNameToId("Coercer1"));
////
////    results.close();
////    return 0;
//
//    gettimeofday(&tb, NULL);
//    auto result = model.verifyApproximation(true, formula);
//    gettimeofday(&te, NULL);
//
//    results << "Imperfect verification time: " << 1000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec) / 1000
//            << " milliseconds" << endl;
//
//    if (*result.first.begin() == 0) {
//        results << "Imperfect verification result: TRUE" << endl;
//    } else {
//        results << "Imperfect verification result: FALSE" << endl;
//    }
//
//    gettimeofday(&tb, NULL);
//    result = model.verifyApproximation(false, formula);
//    gettimeofday(&te, NULL);
//
//    results << "Perfect verification time: " << 1000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec) / 1000
//            << " milliseconds" << endl;
//
//    if (*result.first.begin() == 0) {
//        results << "Perfect verification result: TRUE" << endl;
//    } else {
//        results << "Perfect verification result: FALSE" << endl;
//    }
//
//    gettimeofday(&tb, NULL);
//    auto result2 = model.verifyParallel(formula);
//    gettimeofday(&te, NULL);
//
//    results << "Recursive DominoDFS time: " << 1000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec) / 1000
//            << " milliseconds" << endl;
//
//    if (result2.first) {
//        results << "Recursive DominoDFS result: TRUE" << endl;
//    } else {
//        results << "Recursive DominoDFS result: FALSE" << endl;
//    }
//
//    results << "Model END" << endl << endl;
//    results.close();

    return 0;
}