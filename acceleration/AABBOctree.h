//
// Created by Jonas on 2/03/2021.
//

#ifndef MESHCORE_AABBOCTREE_H
#define MESHCORE_AABBOCTREE_H

#include "../core/ModelSpaceMesh.h"
#include "AbstractBoundsTree.h"

class AABBOctree final : public AbstractBoundsTree<AABB, 8u> {
    void doSplit();
    void splitTopDown(unsigned int maxDepth, unsigned int maxTrianglesPerNode) override;

public:
    explicit AABBOctree(const std::shared_ptr<ModelSpaceMesh>& modelSpaceMesh);
    AABBOctree(const AABB &aabb, unsigned int depth): AbstractBoundsTree(aabb, depth){};

};


#endif //MESHCORE_AABBOCTREE_H
