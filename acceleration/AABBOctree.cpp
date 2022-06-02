//
// Created by Jonas on 2/03/2021.
//

#include "AABBOctree.h"

AABBOctree::AABBOctree(const std::shared_ptr<ModelSpaceMesh>& modelSpaceMesh): AABBTree<8u>(modelSpaceMesh->getBounds(), 0){
    const auto& vertices = modelSpaceMesh->getVertices();
    for(const auto& triangle: modelSpaceMesh->getTriangles()){
        this->triangles.emplace_back(VertexTriangle(vertices[triangle.vertexIndex0], vertices[triangle.vertexIndex1], vertices[triangle.vertexIndex2]));
    }
    if(!this->triangles.empty()){
        this->empty = false;
        this->splitTopDown(8u,8u);
    }
}

void AABBOctree::splitTopDown(unsigned int maxDepth, unsigned int maxTrianglesPerNode) {

    if(this->triangles.size()>=maxTrianglesPerNode && this->depth < maxDepth){
        doSplit();
        for(const auto& child: children){
            child->splitTopDown(maxDepth, maxTrianglesPerNode);
        }
    }
}

void AABBOctree::doSplit() {

    glm::vec3 center = this->bounds.getCenter();
    glm::vec3 minimum = this->bounds.getMinimum();
    glm::vec3 maximum = this->bounds.getMaximum();

    children[0] = std::make_shared<AABBOctree>(AABBOctree(AABB(glm::vec3(minimum.x, minimum.y, minimum.z), glm::vec3(center.x, center.y, center.z)), depth + 1));
    children[1] = std::make_shared<AABBOctree>(AABBOctree(AABB(glm::vec3(center.x, minimum.y, minimum.z), glm::vec3(maximum.x, center.y, center.z)), depth + 1));
    children[2] = std::make_shared<AABBOctree>(AABBOctree(AABB(glm::vec3(minimum.x, center.y, minimum.z), glm::vec3(center.x, maximum.y, center.z)), depth + 1));
    children[3] = std::make_shared<AABBOctree>(AABBOctree(AABB(glm::vec3(center.x, center.y, minimum.z), glm::vec3(maximum.x, maximum.y, center.z)), depth + 1));
    children[4] = std::make_shared<AABBOctree>(AABBOctree(AABB(glm::vec3(minimum.x, minimum.y, center.z), glm::vec3(center.x, center.y, maximum.z)), depth + 1));
    children[5] = std::make_shared<AABBOctree>(AABBOctree(AABB(glm::vec3(center.x, minimum.y, center.z), glm::vec3(maximum.x, center.y, maximum.z)), depth + 1));
    children[6] = std::make_shared<AABBOctree>(AABBOctree(AABB(glm::vec3(minimum.x, center.y, center.z), glm::vec3(center.x, maximum.y, maximum.z)), depth + 1));
    children[7] = std::make_shared<AABBOctree>(AABBOctree(AABB(glm::vec3(center.x, center.y, center.z), glm::vec3(maximum.x, maximum.y, maximum.z)), depth + 1));

    for(const auto& vertexTriangle: this->triangles){
        for(auto& _child: children){
            auto child = dynamic_cast<AABBOctree*>(_child.get()); // TODO this is ugly, can we assume a child of an octree is always octree? And we should be able to access these members anyway
            if(Intersection::intersect(child->bounds, vertexTriangle)){
                child->triangles.emplace_back(vertexTriangle);
                child->empty = false;
            }

        }
    }
    triangles.clear();
    this->split = true;
}