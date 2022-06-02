//
// Created by Jonas on 15/03/2021.
//

#ifndef MESHCORE_AABBVOLUMEHIERARCHY_H
#define MESHCORE_AABBVOLUMEHIERARCHY_H

#include "AABBTree.h"
#include "../core/ModelSpaceMesh.h"

class AABBVolumeHierarchy final: public AABBTree<2u> {

private:
    void splitTopDown(unsigned int maxDepth, unsigned int maxTrianglesPerNode) override;
    void doSplit();
public:
    explicit AABBVolumeHierarchy(const std::shared_ptr<ModelSpaceMesh>& modelSpaceMesh);
    AABBVolumeHierarchy(const std::shared_ptr<ModelSpaceMesh>& modelSpaceMesh, unsigned int maxDepth, unsigned int maxTrianglesPerNode);

private:
    AABBVolumeHierarchy(const AABB &aabb, unsigned int depth, unsigned int maxDepth, unsigned int maxTrianglesPerNode): AABBTree(aabb, depth){};
public:
    [[nodiscard]] unsigned int getNumberOfRayIntersections(const Ray &ray) const override;

private:
    AABBVolumeHierarchy(const AABB &aabb, unsigned int depth, std::vector<VertexTriangle> triangles);
};


#endif //MESHCORE_AABBVOLUMEHIERARCHY_H
