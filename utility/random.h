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

    int nextInteger(int lowerBoundInclusive=0, int upperBoundInclusive=std::numeric_limits<int>::max()) const {
        assert(lowerBoundInclusive <= upperBoundInclusive && "The upper bound should be at least as great as the lower bound");
        return boost::random::uniform_int_distribution<int>(lowerBoundInclusive, upperBoundInclusive)(randomEngine);
    }

    unsigned int nextUnsignedInteger(unsigned int lowerBoundInclusive=0u, unsigned int upperBoundInclusive=std::numeric_limits<unsigned int>::max()) const {
        assert(lowerBoundInclusive <= upperBoundInclusive && "The upper bound should be at least as great as the lower bound");
        return boost::random::uniform_int_distribution<unsigned int>(lowerBoundInclusive, upperBoundInclusive)(randomEngine);
    }

    double nextDouble(double lowerBoundInclusive=0.0, double upperBoundExclusive=1.0) const {
        assert(lowerBoundInclusive < upperBoundExclusive && "The upper bound is exclusive and therefore must be greater than the lower bound");
        return boost::random::uniform_real_distribution<double>(lowerBoundInclusive, upperBoundExclusive)(randomEngine);
    }

    float nextFloat(float lowerBoundInclusive=0.0f, float upperBoundExclusive=1.0f) const{
        assert(lowerBoundInclusive < upperBoundExclusive && "The upper bound is exclusive and therefore must be greater than the lower bound");
        return boost::random::uniform_real_distribution<float>(lowerBoundInclusive, upperBoundExclusive)(randomEngine);
    }
};

#endif //OPTIXMESHCORE_RANDOM_H
