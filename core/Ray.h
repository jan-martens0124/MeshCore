//
// Created by Jonas on 9/11/2020.
//

#ifndef OPTIX_SAMPLES_RAY_H
#define OPTIX_SAMPLES_RAY_H

#include <glm/glm.hpp>
#include "Vertex.h"
#include "Transformation.h"

class AABB;

class Ray {
public:
    const Vertex origin;
    const glm::vec3 direction;
    const glm::vec3 inverseDirection;
public:
    Ray(Vertex origin, glm::vec3 direction);

    [[nodiscard]] Ray getTransformed(const Transformation &transformation) const;
    [[nodiscard]] Ray getTransformed(const glm::mat4 &transformationMatrix) const;
};


#endif //OPTIX_SAMPLES_RAY_H
