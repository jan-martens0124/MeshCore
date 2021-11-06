//
// Created by Jonas on 9/11/2020.
//

#include "Ray.h"
#include "../../core/AABBTree.h"

Ray::Ray(Vertex origin, Vertex direction):
origin(origin),
direction(direction),
inverseDirection(1.0f/direction) {}

std::ostream &operator<<(std::ostream &os, const Ray &ray) {
    return os << "Ray(Origin" << ray.origin << ", Direction" << ray.direction << ")";
}

Ray Ray::getTransformed(const Transformation &transformation) const {
    Vertex transformedOrigin = transformation.getMatrix() * glm::vec4(this->origin, 1);
    Vertex transformedDirection = transformation.getMatrix() * glm::vec4(this->origin, 0);
    return Ray(transformedOrigin, transformedDirection);
}