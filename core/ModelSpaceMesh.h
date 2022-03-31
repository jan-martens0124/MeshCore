//
// Created by Jonas on 27/01/2021.
//

#ifndef MESHCORE_MODELSPACEMESH_H
#define MESHCORE_MODELSPACEMESH_H

#include <vector>
#include <optional>
#include "Vertex.h"
#include "IndexTriangle.h"
#include "IndexEdge.h"
#include "AABB.h"

class ModelSpaceMesh {
private:
    AABB bounds;
    std::vector<Vertex> vertices;
    std::vector<IndexTriangle> triangles;
    mutable std::optional<std::vector<IndexEdge>> edges;
public:
    ModelSpaceMesh() = default;
    ModelSpaceMesh(std::vector<Vertex> vertices, std::vector<IndexTriangle> triangles);
    ModelSpaceMesh(const ModelSpaceMesh& other) = default;
    ~ModelSpaceMesh() = default;

    [[nodiscard]] const std::vector<Vertex>& getVertices() const;
    [[nodiscard]] const std::vector<IndexTriangle>& getTriangles() const;
    [[nodiscard]] const std::vector<IndexEdge>& getEdges() const;
    [[nodiscard]] std::vector<IndexEdge> getSufficientIntersectionEdges() const;

    const AABB &getBounds() const;

    ModelSpaceMesh getConvexHull() const;
};


#endif //MESHCORE_MODELSPACEMESH_H
