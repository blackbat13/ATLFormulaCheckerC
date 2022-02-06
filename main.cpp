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
    cout << thread::hardware_concurrency() << endl;
    struct timeval tb, te;
    auto model = SimpleModel(argv[1]);

    int t = 5;
    if (argc >= 3) {
        t = atoi(argv[2]);
    }

    bool imperfect = true;
    if (argc >= 4 && strcmp(argv[3], "-p") == 0) {
        imperfect = false;
    }

    int formulasCount = model.getFormulasCount();

    auto parallel = model.toParallelModel(imperfect);
    cout << "Parallel states size: " << parallel->states.size() << endl;
    cout << "#" << parallel->threadsVector.size() << endl;

    for (int formulaIndex = 0; formulaIndex < formulasCount; formulaIndex++) {
        bool result;
        unsigned long long czas = 0;

        cout << "Formula index: " << formulaIndex << endl;

        model.setParallelAccept(parallel, formulaIndex);

        gettimeofday(&tb, NULL);
        result = parallel->forkRecursiveDFS(0, 0);
        gettimeofday(&te, NULL);


        czas = 1000000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec);

        cout << "standard DFS: " << czas << " usec, ";

        if (result) cout << "True" << endl;
        else cout << "False" << endl;

        gettimeofday(&tb, NULL);
        result = parallel->forkRecursiveDFS(0, t);
        gettimeofday(&te, NULL);


        czas = 1000000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec);

        cout << "parallel DFS: " << czas << " usec, ";

        if (result) cout << "True" << endl;
        else cout << "False" << endl;

        gettimeofday(&tb, NULL);
        auto resultap = model.verifyApproximationImperfect(formulaIndex);
        gettimeofday(&te, NULL);

        cout << "Imperfect verification time: " << 1000000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec)
             << " usec" << endl;

        if (*resultap.begin() == 0) {
            cout << "Imperfect verification result: TRUE" << endl;
        } else {
            cout << "Imperfect verification result: FALSE" << endl;
        }

        gettimeofday(&tb, NULL);
        resultap = model.verifyApproximationPerfect(formulaIndex);
        gettimeofday(&te, NULL);

        cout << "Perfect verification time: " << 1000000 * (te.tv_sec - tb.tv_sec) + (te.tv_usec - tb.tv_usec)
             << " usec" << endl;

        if (*resultap.begin() == 0) {
            cout << "Perfect verification result: TRUE" << endl;
        } else {
            cout << "Perfect verification result: FALSE" << endl;
        }
    }

    delete(parallel);
    return 0;
}