//
// Created by Jonas on 7/10/2021.
//

#include "AABB.h"
#include "VertexTriangle.h"

AABB::AABB(glm::vec3 minimum, glm::vec3 maximum): minimum(minimum), maximum(maximum) {}

glm::vec3 AABB::getMinimum() const {
    return this->minimum;
}

glm::vec3 AABB::getMaximum() const {
    return this->maximum;
}

glm::vec3 AABB::getCenter() const {
    return (this->maximum + this->minimum)/2.0f;
}

glm::vec3 AABB::getHalf() const{
    return (this->maximum - this->minimum)/2.0f;
}

AABB::AABB(const std::vector<Vertex> &vertices): minimum(0), maximum(0){
    assert(!vertices.empty());

    this->minimum = vertices[0];
    this->maximum = vertices[0];
    for(auto iterator = vertices.begin()++; iterator < vertices.end(); iterator++){
        this->minimum = glm::min(this->minimum, *iterator);
        this->maximum = glm::max(this->maximum, *iterator);
    }
}

AABB::AABB(const std::vector<VertexTriangle> &vertexTriangles) {
    assert(!vertexTriangles.empty());
    this->minimum = vertexTriangles[0].bounds.getMinimum();
    this->maximum = vertexTriangles[0].bounds.getMaximum();
    for(auto iterator = vertexTriangles.begin()++; iterator < vertexTriangles.end(); iterator++){
        this->minimum = glm::min(this->minimum, iterator->bounds.getMinimum());
        this->maximum = glm::max(this->maximum, iterator->bounds.getMaximum());
    }

}

AABB::AABB(const VertexTriangle &vertexTriangle):
    minimum(glm::min(vertexTriangle.vertex0, glm::min(vertexTriangle.vertex1, vertexTriangle.vertex2))),
    maximum(glm::max(vertexTriangle.vertex0, glm::max(vertexTriangle.vertex1, vertexTriangle.vertex2))) {}

float AABB::getSurfaceArea() const {
    auto delta = maximum - minimum;
    return 2 * (delta.x * delta.y + delta.x * delta.z + delta.y * delta.z);
}

float AABB::getVolume() const {
    auto delta = maximum - minimum;
    return delta.x * delta.y * delta.z;
}