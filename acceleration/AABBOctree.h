//
// Created by Jonas on 2/03/2021.
//

#ifndef MESHCORE_AABBOCTREE_H
#define MESHCORE_AABBOCTREE_H

#include "AABBTree.h"
#include "../core/ModelSpaceMesh.h"

class AABBOctree final : public AABBTree<8u> {
    void doSplit();
    void splitTopDown(unsigned int maxDepth, unsigned int maxTrianglesPerNode) override;

public:
    explicit AABBOctree(const std::shared_ptr<ModelSpaceMesh>& modelSpaceMesh);
    AABBOctree(const AABB &aabb, unsigned int depth): AABBTree(aabb, depth){};

};


#endif //MESHCORE_AABBOCTREE_H
