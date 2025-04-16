//
// Created by Jonas on 29/03/2022.
//

#ifndef MESHCORE_SIMULATEDANNEALING_H
#define MESHCORE_SIMULATEDANNEALING_H

#include "AbstractLocalSearch.h"

template<class S>
class SimulatedAnnealing: public AbstractLocalSearch<S>{

private:
    // Geometric cooling scheme
    double currentTemperature;
    double coolingRate;
    double startTemperature;

    unsigned int maximumIterations;

public:
    SimulatedAnnealing(double startTemperature, double stopTemperature, unsigned int maximumIterations): startTemperature(startTemperature), maximumIterations(maximumIterations){
        this->coolingRate = std::pow(10, std::log10(stopTemperature/startTemperature)/maximumIterations);
    }

private:
    double performIteration(unsigned int currentIteration, std::shared_ptr<S> currentSolution, double currentScore) override {

        // Move towards new candidate
        auto move = this->moveFactory.sample(currentSolution);
        move->doMove(currentSolution);
        auto newScore = this->objectiveFunction.evaluate(currentSolution);

        // Update temperature
        this->currentTemperature *= this->coolingRate;

        // Accept move if it improves the score
        if(newScore < currentScore){
            currentScore = newScore;
        }
        else{
            // If not, accept move with probability
            double probability = std::exp((newScore - currentScore)/this->currentTemperature);
            if(probability > this->random.nextDouble()){
                currentScore = newScore;
            }
            else{
                // Reject move
                move->undoMove(currentSolution);
            }
        }
        return currentScore;
    }

    void initialize(std::shared_ptr<S> initialSolution, double initialScore) override {
        this->currentTemperature = this->startTemperature;
    }
};

#endif //MESHCORE_SIMULATEDANNEALING_H
