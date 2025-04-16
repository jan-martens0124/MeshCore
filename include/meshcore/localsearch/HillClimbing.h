//
// Created by Jonas on 29/03/2022.
//

#ifndef MESHCORE_HILLCLIMBING_H
#define MESHCORE_HILLCLIMBING_H

#include "AbstractLocalSearch.h"

template<class S>
class HillClimbing: public AbstractLocalSearch<S>{
private:
    bool acceptEqual = false;

public:
    HillClimbing() = default;
    explicit HillClimbing(bool acceptEqual): acceptEqual(acceptEqual){}
private:
    double performIteration(unsigned int currentIteration, std::shared_ptr<S> currentSolution, double currentScore) override {
        auto move = this->moveFactory.sample(currentSolution, this->random);
        move->doMove(currentSolution);
        auto newScore = this->objectiveFunction.evaluate(currentSolution);
        if(newScore < currentScore || (acceptEqual && newScore == currentScore)){
            return newScore;
        }
        else{
            move->undoMove(currentSolution);
            return currentScore;
        }
    }

    void initialize(std::shared_ptr<S> initialSolution, double initialScore) override {

    }
};

#endif //MESHCORE_HILLCLIMBING_H
