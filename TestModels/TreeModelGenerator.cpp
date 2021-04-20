#include "../Models/ParallelModel.hpp"

class TreeModelGenerator {
protected:
    int n;
    int m;
    int k;
    int extraStatesCount;
    ParallelModel *model = nullptr;
    
public:
    TreeModelGenerator(int n, int m, int k, int extraStatesCount) : n(n), m(m), k(k), extraStatesCount(extraStatesCount) {
    }
    
    int calculateStatesCount() {
        return (std::pow(this->n, this->k) - 1) / (this->n - 1);
    }
    
    int calculateLevelStatesCount(int levelId) {
        return std::pow(this->n, levelId);
    }
    
    int calculateLevelFirstStateId(int levelId) {
        return (std::pow(this->n, levelId) - 1) / (this->n - 1);
    }
    
    ParallelModel *getModel() {
        if (this->model != nullptr) {
            return this->model;
        }
        int treeStatesCount = this->calculateStatesCount();
        if (treeStatesCount > 400000000) {
            return nullptr;
        }
        int totalStatesCount = treeStatesCount + this->extraStatesCount;
        cout << "Number of states: " << totalStatesCount << endl;
        this->model = new ParallelModel(totalStatesCount);
        for (int levelId = 1; levelId < k; ++levelId) {
            int statesInLevelCount = this->calculateLevelStatesCount(levelId);
            int levelFirstSateId = this->calculateLevelFirstStateId(levelId);
            int parentLevelFirstSateId = this->calculateLevelFirstStateId(levelId - 1);
            for (int levelStateId = 0; levelStateId < statesInLevelCount; ++levelStateId) {
                int stateId = levelFirstSateId + levelStateId;
                int parentStateId = parentLevelFirstSateId + levelStateId / this->n;
                int action = ((levelStateId % this->n) ) % this->m + 1;
                // cout << parentStateId << " ---" << action << "--> " << stateId << endl;
                this->model->states[parentStateId]->addTransition(action, stateId);
            }
        }
        return this->model;
    }
    
};
