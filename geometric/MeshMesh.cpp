//
// Created by Jonas on 5/11/2021.
//

#include "Intersection.h"
#include "../acceleration/CachingBoundsTreeFactory.h"
#include "../acceleration/AABBVolumeHierarchy.h"

namespace Intersection {

    bool intersect(const WorldSpaceMesh& worldSpaceMeshA, const WorldSpaceMesh& worldSpaceMeshB){

        const auto& modelSpaceMeshA = worldSpaceMeshA.getModelSpaceMesh();
        const auto& modelSpaceMeshB = worldSpaceMeshB.getModelSpaceMesh();
        const auto& modelSpaceTransformationA = worldSpaceMeshA.getModelTransformation();
        const auto& modelSpaceTransformationB = worldSpaceMeshB.getModelTransformation();

        if(modelSpaceMeshA->getTriangles().size() >= modelSpaceMeshB->getTriangles().size()){
            const auto modelBToModelASpaceTransformation = modelSpaceTransformationA.getInverseMatrix() * modelSpaceTransformationB.getMatrix();

            const auto &modelBVertices = modelSpaceMeshB->getVertices();
            const auto &treeA = CachingBoundsTreeFactory<AABBVolumeHierarchy>::getBoundsTree(modelSpaceMeshA);
            return std::any_of(modelSpaceMeshB->getTriangles().begin(), modelSpaceMeshB->getTriangles().end(), [&](const auto &triangle){
                VertexTriangle vertexTriangle(modelBVertices[triangle.vertexIndex0], modelBVertices[triangle.vertexIndex1], modelBVertices[triangle.vertexIndex2]);
                return treeA->intersectsTriangle(vertexTriangle.getTransformed(modelBToModelASpaceTransformation));
            });
        }
        else{
            const auto modelAToModelBSpaceTransformation = modelSpaceTransformationB.getInverseMatrix() * modelSpaceTransformationA.getMatrix();
            const auto &modelAVertices = modelSpaceMeshA->getVertices();
            const auto &treeB = CachingBoundsTreeFactory<AABBVolumeHierarchy>::getBoundsTree(modelSpaceMeshB);

            return std::any_of(modelSpaceMeshA->getTriangles().begin(), modelSpaceMeshA->getTriangles().end(), [&](const auto &triangle){
                VertexTriangle vertexTriangle(modelAVertices[triangle.vertexIndex0], modelAVertices[triangle.vertexIndex1], modelAVertices[triangle.vertexIndex2]);
                return treeB->intersectsTriangle(vertexTriangle.getTransformed(modelAToModelBSpaceTransformation));
            });
        }
    }

    bool debugIntersects(const WorldSpaceMesh& worldSpaceMeshA, const WorldSpaceMesh& worldSpaceMeshB){
#if NDEBUG
        std::cout << "[MESHCORE] Using a naive triangleTriangleIntersects implementation -- use for debugging only" << std::endl;
#endif

        auto numOuterLoop =  worldSpaceMeshA.getModelSpaceMesh()->getTriangles().size();
        bool meshIntersects = false;
//    omp_set_num_threads(std::min(128u, numOuterLoop));
#pragma omp parallel
        {
#pragma omp for
            for(auto i=0; i<numOuterLoop; i++){
//    for (IndexTriangle thisTriangle: worldSpaceMeshA.modelSpaceMesh->getTriangles()) {
                IndexTriangle thisTriangle = worldSpaceMeshA.getModelSpaceMesh()->getTriangles()[i];

                Transformation thisModelTransformation = worldSpaceMeshA.getModelTransformation();

                Vertex worldSpaceVertex0 = thisModelTransformation.transformVertex(worldSpaceMeshA.getModelSpaceMesh()->getVertices()[thisTriangle.vertexIndex0]);
                Vertex worldSpaceVertex1 = thisModelTransformation.transformVertex(worldSpaceMeshA.getModelSpaceMesh()->getVertices()[thisTriangle.vertexIndex1]);
                Vertex worldSpaceVertex2 = thisModelTransformation.transformVertex(worldSpaceMeshA.getModelSpaceMesh()->getVertices()[thisTriangle.vertexIndex2]);

                for (IndexTriangle otherTriangle: worldSpaceMeshB.getModelSpaceMesh()->getTriangles()) {

                    Transformation otherModelTransformation = worldSpaceMeshB.getModelTransformation();

                    Vertex otherWorldSpaceVertex0 = otherModelTransformation.transformVertex(worldSpaceMeshB.getModelSpaceMesh()->getVertices()[otherTriangle.vertexIndex0]);
                    Vertex otherWorldSpaceVertex1 = otherModelTransformation.transformVertex(worldSpaceMeshB.getModelSpaceMesh()->getVertices()[otherTriangle.vertexIndex1]);
                    Vertex otherWorldSpaceVertex2 = otherModelTransformation.transformVertex(worldSpaceMeshB.getModelSpaceMesh()->getVertices()[otherTriangle.vertexIndex2]);
                    bool intersects = Intersection::intersect(VertexTriangle(otherWorldSpaceVertex0, otherWorldSpaceVertex1, otherWorldSpaceVertex2), VertexTriangle(worldSpaceVertex0, worldSpaceVertex1, worldSpaceVertex2));
                    if(intersects){
                        meshIntersects |= 1;
                        break;
                    }
                }
            }
        }
        return meshIntersects;
    }

    bool inside(const WorldSpaceMesh& worldSpaceMeshA, const WorldSpaceMesh& worldSpaceMeshB){
#if NDEBUG
        std::cout << "[MESHCORE] Using a naive rayTriangleInside implementation -- use for debugging only" << std::endl;
#endif
        for(Vertex modelSpaceOrigin: worldSpaceMeshA.getModelSpaceMesh()->getVertices()){
            Vertex worldSpaceOrigin = worldSpaceMeshA.getModelTransformation().transformVertex(modelSpaceOrigin);
            Vertex otherModelSpaceOrigin = worldSpaceMeshB.getModelTransformation().inverseTransformVertex(worldSpaceOrigin);
            Ray otherModelSpaceRay(otherModelSpaceOrigin, glm::vec3(1, 0, 0)); // Direction doesn't matter

            int numberOfIntersections = 0;
            for(IndexTriangle triangle: worldSpaceMeshB.getModelSpaceMesh()->getTriangles()){
                Vertex otherModelSpaceVertex0 = worldSpaceMeshB.getModelSpaceMesh()->getVertices()[triangle.vertexIndex0];
                Vertex otherModelSpaceVertex1 = worldSpaceMeshB.getModelSpaceMesh()->getVertices()[triangle.vertexIndex1];
                Vertex otherModelSpaceVertex2 = worldSpaceMeshB.getModelSpaceMesh()->getVertices()[triangle.vertexIndex2];
                if(Intersection::intersect(otherModelSpaceRay, VertexTriangle(otherModelSpaceVertex0, otherModelSpaceVertex1, otherModelSpaceVertex2))){
                    numberOfIntersections++;
                }
            }
            if(numberOfIntersections%2 == 0){
                return false;
            }
        }
        return true;
    }
}
