//
// Created by Jonas on 1/07/2022.
//

#ifndef OPTIXMESHCORE_SPHERE_H
#define OPTIXMESHCORE_SPHERE_H

#include "Vertex.h"

class Sphere{
public:
    const Vertex center;
    const float radius;
public:
    MC_FUNC_QUALIFIER Sphere(Vertex center, float radius): center(center), radius(radius) {}

    MC_FUNC_QUALIFIER [[nodiscard]] Sphere getTransformed(const Transformation &transformation) const {
        auto transformedCenter = transformation.transformVertex(this->center);
        auto transformedRadius = this->radius * transformation.getScale();
        return {transformedCenter, transformedRadius};
    }

    MC_FUNC_QUALIFIER [[nodiscard]] Sphere getRotated(const Quaternion &quaternion) const {
        Vertex rotatedCenter = quaternion.rotateVertex(this->center);
        return {rotatedCenter, this->radius};
    }

    MC_FUNC_QUALIFIER [[nodiscard]] Sphere getTransformed(const glm::mat4 &transformationMatrix) const {
        auto transformedCenter = transformationMatrix * glm::vec4(this->center, 1);
        auto transformedRadius = glm::length(transformationMatrix[0]); // Extract x-component scaling from the matrix
        return {transformedCenter, transformedRadius};
    }

    MC_FUNC_QUALIFIER [[nodiscard]] float getVolume() const {
        return (4.0f / 3.0f) * glm::pi<float>() * radius * radius * radius;
    }

    MC_FUNC_QUALIFIER [[nodiscard]] float getSurfaceArea() const {
        return 4.0f * glm::pi<float>() * radius * radius;
    }

    MC_FUNC_QUALIFIER [[nodiscard]] Vertex getClosestPoint(Vertex point) const {
        return glm::normalize(point - this->center) * this->radius + this->center;
    }

    MC_FUNC_QUALIFIER [[nodiscard]] float getDistanceSquaredTo(Vertex point) const {
        auto deltaCenter = point - this->center;
        auto distance = glm::sqrt(glm::dot(deltaCenter, deltaCenter)) - this->radius;
        return distance*distance;
    }
};

#endif //OPTIXMESHCORE_SPHERE_H
