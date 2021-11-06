//
// Created by Jonas on 7/10/2021.
//

#ifndef MESHCORE_VERTEXTRIANGLE_H
#define MESHCORE_VERTEXTRIANGLE_H

#include "Vertex.h"
#include "AABB.h"

class AABB;

class VertexTriangle{
public:
    const Vertex vertex0, vertex1, vertex2;
    const glm::vec3 edge0, edge1, edge2;
    const glm::vec3 normal;
    const AABB bounds;
public:
    VertexTriangle(Vertex vertex0, Vertex vertex1, Vertex vertex2);
    [[nodiscard]] VertexTriangle getTransformed(const Transformation& transformation) const;
    [[nodiscard]] VertexTriangle getTransformed(const glm::mat4& transformationMatrix) const;
};

#endif //MESHCORE_VERTEXTRIANGLE_H
