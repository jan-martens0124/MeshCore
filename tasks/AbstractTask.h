//
// Created by Jonas on 14/12/2020.
//

#ifndef MESHCORE_ABSTRACTTASK_H
#define MESHCORE_ABSTRACTTASK_H


#include <thread>
#include <atomic>
#include "AbstractTaskObserver.h"
#include "../../solutions/AbstractMeshSolution.h"
#include <boost/random.hpp>

class AbstractTask {
private:
    unsigned int seed;
    std::thread* thread;
    boost::random::mt19937 randomEngine;
    std::vector<AbstractTaskObserver *> taskObservers;

protected:
    std::atomic<bool> stopCalled;

public:
    AbstractTask();

    void setSeed(unsigned int seed);
    virtual void run();
    void start();
    void stop();
    void join();

    void registerObserver(AbstractTaskObserver* observer);
    void unregisterObserver(AbstractTaskObserver* observer);
    void notifyObserversUpdate() const;
    void notifyObserversProgress(float progress) const;
    void notifyObserversFinished() const;
    void notifyObserversStarted() const;
    void notifyObserversStatus(const std::string& status) const;
    void notifyObserversSolution(const AbstractMeshSolution& solution) const;

protected:
    float getRandomFloat(float maxValue);
    unsigned int getRandomUint(unsigned int maxValueExclusive);
};

#endif //MESHCORE_ABSTRACTTASK_H
