#include <iostream>
#include <vector>
#include <set>
#include "BridgeModel.h"

using namespace std;

int main() {
    int m, n;
    m = 3;
    n = 3;

    BridgeModel bridgeModel = BridgeModel(m, n, BridgeModel::State(HANDS_TYPE(), std::vector<int>(2,0), 0, std::vector<int>(4,-1), 0, std::vector<int>(), 0, -1));

    set<int> winningStates;
    for(int i = 0; i < bridgeModel.getStates().size(); ++i) {
        BridgeModel::State state = bridgeModel.getStates()[i];
        if(state.lefts[0] > m/2 && state.lefts[0] + state.lefts[1] == m) {
            winningStates.insert(i);
        }
    }

    set<int> result = bridgeModel.getModel().minimumFormulaOneAgentMultipleStatesDisjoint(0, winningStates);
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

    string a;
    cin >> a;
    return 0;
}