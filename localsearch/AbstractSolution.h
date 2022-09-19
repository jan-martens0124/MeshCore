//
// Created by Jonas on 17/09/2022.
//

#ifndef OPTIXMESHCORE_ABSTRACTSOLUTION_H
#define OPTIXMESHCORE_ABSTRACTSOLUTION_H

#include <memory>

struct AbstractSolution{
    virtual ~AbstractSolution() = default;
    [[nodiscard]] virtual bool isFeasible() const = 0;
    [[nodiscard]] virtual std::shared_ptr<AbstractSolution> clone() const = 0;
};

#endif //OPTIXMESHCORE_ABSTRACTSOLUTION_H
