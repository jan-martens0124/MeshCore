//
// Created by Jonas on 27/01/2021.
//

#ifndef MESHCORE_MODELSPACEMESH_H
#define MESHCORE_MODELSPACEMESH_H

#include <vector>
#include <memory>
#include <optional>
#include <string>
#include "Vertex.h"
#include "IndexTriangle.h"
#include "IndexEdge.h"
#include "AABB.h"

class ModelSpaceMesh {
private:
    std::string name;
    AABB bounds;
    std::vector<Vertex> vertices;
    std::vector<IndexTriangle> triangles;

    // Cached data
    mutable std::optional<std::vector<IndexEdge>> edges;
    mutable std::optional<bool> convex;
    mutable std::optional<float> volume;
    mutable std::optional<std::shared_ptr<ModelSpaceMesh>> convexHull;
//    mutable std::optional<GJKMesh> gjkMesh;
    // TODO add AABB bounds as well?
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

    std::optional<std::shared_ptr<ModelSpaceMesh>> getConvexHull() const;
    bool isConvex();
    float getVolume();

    const std::string &getName() const;
    void setName(const std::string &newName);
};


#endif //MESHCORE_MODELSPACEMESH_H
