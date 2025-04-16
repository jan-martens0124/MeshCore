//
// Created by Jonas on 15/03/2021.
//

#include "meshcore/acceleration/AABBVolumeHierarchy.h"
#include "meshcore/factories/AABBFactory.h"

AABBVolumeHierarchy::AABBVolumeHierarchy(const std::shared_ptr<ModelSpaceMesh>& modelSpaceMesh, unsigned int maxDepth, unsigned int maxTrianglesPerNode):
        AbstractBoundsTree<AABB, 2U, true>(modelSpaceMesh->getBounds(), 0){

    const auto& vertices = modelSpaceMesh->getVertices();

    for(const auto& triangle: modelSpaceMesh->getTriangles()){
        this->triangles.emplace_back(VertexTriangle(vertices[triangle.vertexIndex0], vertices[triangle.vertexIndex1], vertices[triangle.vertexIndex2]));
    }

    if(!this->triangles.empty()){
        this->empty = false;
        this->splitTopDown(maxDepth, maxTrianglesPerNode);
    }
}

void AABBVolumeHierarchy::splitTopDown(unsigned int maxDepth, unsigned int maxTrianglesPerNode) {
    if(this->triangles.size()>=maxTrianglesPerNode && this->depth < maxDepth){
        doSplit();
        for(const auto& child: children){
            if(child!=nullptr) child->splitTopDown(maxDepth, maxTrianglesPerNode);
        }
    }
}

void AABBVolumeHierarchy::doSplit() {

    float bestCost = std::numeric_limits<float>::max();
    std::shared_ptr<AABB> aabb1best;
    std::shared_ptr<AABB>  aabb2best;
    std::shared_ptr<std::vector<VertexTriangle>> vertexTriangles1best;
    std::shared_ptr<std::vector<VertexTriangle>> vertexTriangles2best;

    unsigned int buckets = 2;

    for(unsigned int i=1;i<=buckets;i++){
        for(unsigned int j=1;j<=buckets;j++){
            for(unsigned int k=1;k<=buckets;k++){

                if(i==0 && j==0 && k==0) continue;
                if(i==buckets && j==buckets && k==buckets) continue;

                Vertex relativesplit(i/float(buckets),j/float(buckets),k/float(buckets));
                glm::vec3 delta(this->bounds.getMaximum() - this->bounds.getMinimum());
                glm::vec3 dot = relativesplit * delta;
                Vertex splitPoint = this->bounds.getMinimum() + dot;
                std::shared_ptr<std::vector<VertexTriangle>> vertexTriangles1 = std::make_shared<std::vector<VertexTriangle>>();
                std::shared_ptr<std::vector<VertexTriangle>> vertexTriangles2 = std::make_shared<std::vector<VertexTriangle>>();
                for (const auto &triangle : triangles){
                    if(glm::all(glm::lessThanEqual(triangle.bounds.getCenter(), splitPoint))){
                        vertexTriangles1->emplace_back(triangle);
                    }
                    else{
                        vertexTriangles2->emplace_back(triangle);
                    }
                }

                if(vertexTriangles1->size() < 5) continue;
                if(vertexTriangles2->size() < 5) continue;

                std::shared_ptr<AABB> aabb1 = std::make_shared<AABB>(AABBFactory::createAABB(*vertexTriangles1));
                std::shared_ptr<AABB> aabb2 = std::make_shared<AABB>(AABBFactory::createAABB(*vertexTriangles2));

                float cost = aabb1->getSurfaceArea() * float(vertexTriangles1->size()) + aabb2->getSurfaceArea()*float(vertexTriangles2->size());

//                float cost = aabb1->getVolume() * float(vertexTriangles1->size()) + aabb2->getVolume() * float(vertexTriangles2->size());
//                float cost = aabb1->getVolume() + aabb2->getVolume();
//                float cost = aabb1->getSurfaceArea() + aabb2->getSurfaceArea();

                if(cost < bestCost){
                    bestCost = cost;
                    vertexTriangles1best = vertexTriangles1;
                    vertexTriangles2best = vertexTriangles2;
                    aabb1best = aabb1;
                    aabb2best = aabb2;
                }
            }
        }
    }

    if(aabb1best==nullptr) return; // Don't make the split if no good split is found

    children[0] = std::make_shared<AABBVolumeHierarchy>(AABBVolumeHierarchy(*aabb1best, depth + 1, *vertexTriangles1best));
    children[1] = std::make_shared<AABBVolumeHierarchy>(AABBVolumeHierarchy(*aabb2best, depth + 1, *vertexTriangles2best));

    this->triangles.clear();
    this->split = true;
}

AABBVolumeHierarchy::AABBVolumeHierarchy(const AABB &aabb, unsigned int depth, std::vector<VertexTriangle> triangles): AbstractBoundsTree(aabb, depth){
    this->triangles = std::move(triangles);
    this->empty = this->triangles.empty();
}

AABBVolumeHierarchy::AABBVolumeHierarchy(const std::shared_ptr<ModelSpaceMesh>& modelSpaceMesh):
    AABBVolumeHierarchy(modelSpaceMesh, 32, 16) {

}