//
// Created by Jonas on 7/05/2024.
//

#ifndef EXTENDEDMESHCORE_PLANE_H
#define EXTENDEDMESHCORE_PLANE_H

#include "Vertex.h"
#include <glm/gtc/epsilon.hpp>

class Plane {
    glm::vec3 normal;
    float d;

public:
    Plane() = default;
    Plane(const glm::vec3 &normal, float d) : normal(normal), d(d) {}
    Plane(const glm::vec3 &normal, const Vertex& point): normal(glm::normalize(normal)), d(-glm::dot(normal, point)) {}
    Plane(const Plane &plane) = default;

    [[nodiscard]] Vertex closestPoint(const Vertex &point) const {
        assert(glm::length(normal) == 1.0f);
        return point - signedDistance(point) * normal;
    }

    [[nodiscard]] float signedDistance(const Vertex &point) const {
        assert(glm::epsilonEqual(glm::length(normal), 1.0f, 1e-6f));
        return glm::dot(point, normal) + d;
    }

    [[nodiscard]] float distance(const Vertex &point) const {
        return std::abs(signedDistance(point));
    }

    [[nodiscard]] const glm::vec3 &getNormal() const {
        return normal;
    }

    [[nodiscard]] float getD() const {
        return d;
    }
};

#endif //EXTENDEDMESHCORE_PLANE_H
