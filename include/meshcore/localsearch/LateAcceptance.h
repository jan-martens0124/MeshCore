//
// Created by Jonas on 29/03/2022.
//

#ifndef MESHCORE_LATEACCEPTANCE_H
#define MESHCORE_LATEACCEPTANCE_H

#include "AbstractLocalSearch.h"
#include "array"

template<class S>
class LateAcceptance: public AbstractLocalSearch<S>{
private:
    const unsigned int L;
    unsigned int acceptanceIndex=0;
    std::vector<double> acceptanceValues;

public:
    explicit LateAcceptance(MoveFactory<S>& moveFactory, const ObjectiveFunction<S>& objectiveFunction, unsigned int maximumIterations, const Random& random, unsigned int L): AbstractLocalSearch<S>(moveFactory, objectiveFunction, maximumIterations, random), L(L), acceptanceValues(L, 0.0){}

private:
    double performIteration(unsigned int currentIteration, std::shared_ptr<S> currentSolution, double currentScore) override {

        // Move towards new candidate
        auto move = this->moveFactory.sample(currentSolution, this->random);
        if(move==nullptr){
            return currentScore;
        }
        move->doMove(currentSolution);
        auto newScore = this->objectiveFunction.evaluate(currentSolution);
        auto targetScore = acceptanceValues.at(acceptanceIndex);

        auto currentAcceptanceIndex = acceptanceIndex;
        acceptanceIndex++;
        if(acceptanceIndex==L){
            acceptanceIndex = 0;
        }

        // Accept or reject
        if(newScore < targetScore){
            acceptanceValues.at(currentAcceptanceIndex) = newScore;
            return newScore;
        }
        else if(newScore < currentScore){
            return newScore;
        }
        else{
            move->undoMove(currentSolution);
            return currentScore;
        }
    }

    void initialize(std::shared_ptr<S> initialSolution, double initialScore) override {
        for(int i = 0; i < L; i++){
            this->acceptanceValues.at(i) = initialScore;
        }
        this->acceptanceIndex = 0;
    }
};

#endif //MESHCORE_LATEACCEPTANCE_H
