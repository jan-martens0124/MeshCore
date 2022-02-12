//
// Created by Jonas on 9/11/2020.
//

#ifndef OPTIX_SAMPLES_RAY_H
#define OPTIX_SAMPLES_RAY_H

#include <glm/glm.hpp>
#include <iostream>
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
    friend std::ostream & operator<<(std::ostream & os, const Ray& ray);

    [[nodiscard]] Ray getTransformed(const Transformation &transformation) const;
    [[nodiscard]] Ray getTransformed(const glm::mat4 &transformationMatrix) const;
};
std::ostream & operator<<(std::ostream & os, const Ray& ray);

#endif //OPTIX_SAMPLES_RAY_H
