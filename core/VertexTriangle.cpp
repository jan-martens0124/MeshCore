//
// Created by Jonas on 7/10/2021.
//

#include "VertexTriangle.h"

VertexTriangle::VertexTriangle(Vertex vertex0, Vertex vertex1, Vertex vertex2):
    vertex0(vertex0),
    vertex1(vertex1),
    vertex2(vertex2),
    edge0(vertex1-vertex0),
    edge1(vertex2-vertex1),
    edge2(vertex0-vertex2),
    normal(glm::cross(edge0, edge1)),
    bounds(*this){
}

VertexTriangle VertexTriangle::getTransformed(const glm::mat4& transformationMatrix) const{
    Vertex rVertex0 = transformationMatrix * glm::vec4(this->vertex0, 1);
    Vertex rVertex1 = transformationMatrix * glm::vec4(this->vertex1, 1);
    Vertex rVertex2 = transformationMatrix * glm::vec4(this->vertex2, 1);
    return VertexTriangle(rVertex0, rVertex1, rVertex2);
}

VertexTriangle VertexTriangle::getTransformed(const Transformation& transformation) const {
    return this->getTransformed(transformation.getMatrix());
}