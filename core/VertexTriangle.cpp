//
// Created by Jonas on 7/10/2021.
//

#include "VertexTriangle.h"
#include <glm/gtx/hash.hpp>

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

std::ostream& operator<<(std::ostream& o, const VertexTriangle& vertexTriangle) {
    return o << "VertexTriangle(" << vertexTriangle.vertex0 << ", " << vertexTriangle.vertex1 << ", " << vertexTriangle.vertex2 << ")";
}

bool VertexTriangle::operator==(const VertexTriangle &other) const {
    return this->vertex0 == other.vertex0 && this->vertex1 == other.vertex1 && this->vertex2 == other.vertex2;
}

Vertex VertexTriangle::getClosestPoint(const Vertex &point) const {

    // As described in "Real-Time Collision Detection" by Christer Ericson

    // Check if point lies in the vertex region outside A
    auto ab = edge0;
    auto ac = - edge2;
    auto ap = point - vertex0;
    auto d1 = glm::dot(ab, ap);
    auto d2 = glm::dot(ac, ap);

    if(d1 <= 0.0f && d2 <= 0.0f) return vertex0;

    // Check if point lies in the vertex region outside B
    auto bp = point - vertex1;
    auto d3 = glm::dot(ab, bp);
    auto d4 = glm::dot(ac, bp);

    if(d3 >= 0.0f && d4 <= d3) return vertex1;

    // Check if point lies in de edge region of AB, if so return the projection of point onto AB
    auto vc = d1*d4 - d3 *d2;
    if(vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f){
        auto v = d1 / (d1 - d3);
        return vertex0 + v * ab;
    }

    // Check if point lies in the vertex region outside C
    auto cp = point - vertex2;
    auto d5 = glm::dot(ab, cp);
    auto d6 = glm::dot(ac, cp);

    if(d6 >= 0.0f && d5 <= d6) return vertex2;

    // Check if point lies in the edge region of AC, if so return the projection of point onto AC
    auto vb = d5*d2 - d1*d6;
    if(vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f){
        auto w = d2 / (d2 - d6);
        return vertex0 + w * ac;
    }

    // Check if point lies in the edge region of BC, if so return the projection of point onto BC
    auto va = d3*d6 - d5*d4;
    if(va <= 0.0f && (d4-d3) >= 0.0f && (d5 - d6) >= 0.0f){
        auto w = (d4-d3) / ((d4-d3) + (d5-d6));
        return vertex1 + w * (vertex2 - vertex1);
    }

    // The point lies in the face region. Compute the closest point through barycentric coordinates
    auto denom = 1.0f / (va + va + vc);
    auto v = vb * denom;
    auto w = vc * denom;
    return vertex0 + ab*v + ac*w;
}

size_t std::hash<VertexTriangle>::operator()(const VertexTriangle &vertexTriangle) const {
    return std::hash<Vertex>()(vertexTriangle.vertex0 + vertexTriangle.vertex1 + vertexTriangle.vertex2);
//    return std::hash<float>()(vertexTriangle.vertex0.x +  vertexTriangle.vertex0.y + vertexTriangle.vertex0.z +
//                              vertexTriangle.vertex1.x +  vertexTriangle.vertex1.y + vertexTriangle.vertex1.z +
//                              vertexTriangle.vertex2.x +  vertexTriangle.vertex2.y + vertexTriangle.vertex2.z);
}