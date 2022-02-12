//
// Created by Jonas on 7/10/2021.
//

#ifndef MESHCORE_VERTEXTRIANGLE_H
#define MESHCORE_VERTEXTRIANGLE_H

#include "Vertex.h"
#include "AABB.h"

class AABB;

template<> struct std::hash<VertexTriangle> {
    size_t operator()(const VertexTriangle& vertexTriangle) const;
};

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

    [[nodiscard]] Vertex getClosestPoint(const Vertex& vertex) const;

    bool operator==(const VertexTriangle& other) const;

    friend std::ostream& operator<<(std::ostream& o, const VertexTriangle& vertexTriangle);
    friend struct std::hash<VertexTriangle>;
};

#endif //MESHCORE_VERTEXTRIANGLE_H
