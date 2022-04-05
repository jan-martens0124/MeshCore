//
// Created by Jonas on 29/03/2022.
//

#ifndef OPTIXMESHCORE_RANDOM_H
#define OPTIXMESHCORE_RANDOM_H

#include <boost/random.hpp>

class Random{
private:
    mutable boost::random::mt19937 randomEngine;
    const boost::random::uniform_real_distribution<double> uniformDoubleDistribution{};
    const boost::random::uniform_real_distribution<float> uniformFloatDistribution{};
public:
    explicit Random(int seed=0): randomEngine(seed){}

    double nextDouble() const {
        return uniformDoubleDistribution(randomEngine);
    }

    float nextFloat() const {
        return uniformFloatDistribution(randomEngine);
    }

    int nextInteger(int lowerBoundInclusive=0, int upperBoundInclusive=std::numeric_limits<int>::max()) const {
        assert(lowerBoundInclusive <= upperBoundInclusive);
        return boost::random::uniform_int_distribution<int>(lowerBoundInclusive, upperBoundInclusive)(randomEngine);
    }

    unsigned int nextUnsignedInteger(unsigned int lowerBoundInclusive=0, unsigned int upperBoundInclusive=std::numeric_limits<unsigned int>::max()) const {
        assert(lowerBoundInclusive <= upperBoundInclusive);
        return boost::random::uniform_int_distribution<unsigned int>(lowerBoundInclusive, upperBoundInclusive)(randomEngine);
    }
};

#endif //OPTIXMESHCORE_RANDOM_H
