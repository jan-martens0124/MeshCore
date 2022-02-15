//
// Created by Jonas on 9/11/2020.
//

#include "Ray.h"

Ray::Ray(Vertex origin, Vertex direction):
origin(origin),
direction(direction),
inverseDirection(1.0f/direction) {}
 
Ray Ray::getTransformed(const Transformation &transformation) const {
    return this->getTransformed(transformation.getMatrix());
}

Ray Ray::getTransformed(const glm::mat4 &transformationMatrix) const {
    Vertex transformedOrigin = transformationMatrix * glm::vec4(this->origin, 1);
    glm::vec3 transformedDirection = transformationMatrix * glm::vec4(this->origin, 0);
    return Ray(transformedOrigin, transformedDirection);
}