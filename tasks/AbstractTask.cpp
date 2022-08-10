//
// Created by Jonas on 14/12/2020.
//

#include "AbstractTask.h"

void run_static(AbstractTask* task){
    task->run();
}

void AbstractTask::start() {
    this->join();
    this->stopCalled = false;
    randomEngine.seed(this->seed);
    thread = new std::thread(run_static, this);
}

void AbstractTask::join() {
    if(thread != nullptr){
        thread->join();
        delete thread;
        thread = nullptr;
    }
}

AbstractTask::AbstractTask():
    stopCalled(false),
    thread(nullptr),
    seed(0u),
    randomEngine(this->seed)
{}

void AbstractTask::setSeed(unsigned int newSeed) {
    this->seed = newSeed;
}

float AbstractTask::getRandomFloat(float maxValue){
    auto nextFloat = boost::random::uniform_real_distribution<float>(0, maxValue);
    return nextFloat(this->randomEngine);
}

unsigned int AbstractTask::getRandomUint(unsigned int maxValueExclusive) {
    auto nextUint = boost::random::uniform_int_distribution<unsigned int>(0u, maxValueExclusive);
    return nextUint(this->randomEngine);
}

void AbstractTask::notifyObserversUpdate() const {
    for(AbstractTaskObserver* observer: taskObservers){
        observer->notify();
    }
}

void AbstractTask::unregisterObserver(AbstractTaskObserver* observer) {
    auto iterator = taskObservers.begin();
    while(iterator!=taskObservers.end()){
        if(*iterator==observer){
            taskObservers.erase(iterator);
            std::cout << "Unregistered observer" << std::endl;
            return;
        }
        iterator++;
    }
}

void AbstractTask::registerObserver(AbstractTaskObserver* observer) {
    taskObservers.emplace_back(observer);
}

void AbstractTask::notifyObserversSolution(const AbstractMeshSolution& solution) const{
    for(AbstractTaskObserver* observer: taskObservers){
        observer->notifySolution(solution);
    }
}

void AbstractTask::notifyObserversProgress(float progress) const {
    for(AbstractTaskObserver* observer: taskObservers){
        observer->notifyProgress(progress);
    }
}

void AbstractTask::notifyObserversFinished() const {
    for(AbstractTaskObserver* observer: taskObservers){
        observer->notifyFinished();
    }
}

void AbstractTask::notifyObserversStarted() const {
    for(AbstractTaskObserver* observer: taskObservers){
        observer->notifyStarted();
    }
}

void AbstractTask::notifyObserversStatus(const std::string& status) const {
    for(AbstractTaskObserver* observer: taskObservers){
        observer->notifyStatus(status);
    }
}

void AbstractTask::stop() {
    this->stopCalled = true;
}

void AbstractTask::run() {
    this->stopCalled = false;
    this->notifyObserversStarted();
}