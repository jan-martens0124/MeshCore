//
// Created by Jonas on 17/03/2022.
//

#ifndef MESHCORE_AABBFACTORY_H
#define MESHCORE_AABBFACTORY_H

#include <unordered_set>
#include <memory>
#include "../core/AABB.h"
#include "../core/VertexTriangle.h"

class AABBFactory{

public:
//
//    static AABB createAABB(const ModelSpaceMesh& modelSpaceMesh){
//        return createAABB(modelSpaceMesh.getVertices());
//    }
//
//    static AABB createAABB(const WorldSpaceMesh& worldSpaceMesh){
//        // This is called in preprocessing when the overhead of computing the convex hull probably doesn't matter, or this is called many times and the decrease in vertices is worth it
//        auto& convexHullVertices = worldSpaceMesh.getModelSpaceMesh()->getConvexHull()->getVertices();
//        return createAABB(convexHullVertices.begin(), convexHullVertices.end(), worldSpaceMesh.getModelTransformation());
//    }

    static AABB createAABB(const std::vector<Vertex>& vertices){
        return createAABB(vertices.begin(), vertices.end());
    }

    static AABB createAABB(const std::vector<glm::vec3>& vertices, const Transformation& transformation){
        return createAABB(vertices.begin(), vertices.end(), transformation);
    }

    static AABB createAABB(const std::vector<VertexTriangle>& triangles){
        auto minimum = triangles.begin()->bounds.getMinimum();
        auto maximum = triangles.begin()->bounds.getMaximum();
        auto second = triangles.begin(); second++;
        for(auto iter = second;iter!=triangles.end(); ++iter){
            minimum = glm::min(minimum, iter->bounds.getMinimum());
            maximum = glm::max(maximum, iter->bounds.getMaximum());
        }
        return {minimum, maximum};
    }

private:
    template<class Iter>
    static AABB createAABB(const Iter& first, const Iter& last){
        auto minimum = *first;
        auto maximum = minimum;
        auto second = first; second++;
        for(auto iter = second;iter!=last; ++iter){
            minimum = glm::min(minimum, *iter);
            maximum = glm::max(maximum, *iter);
        }
        return {minimum, maximum};
    }

    template<class Iter>
    static AABB createAABB(const Iter& first, const Iter& last, const Transformation& transformation){
        auto minimum = transformation.transformVertex(*first);
        auto maximum = minimum;
        auto second = first; second++;
        for(auto iter = second;iter!=last; ++iter){
            auto transformedVertex = transformation.transformVertex(*iter);
            minimum = glm::min(minimum, transformation.transformVertex(*iter));
            maximum = glm::max(maximum, transformation.transformVertex(*iter));
        }
        return {minimum, maximum};
    }

};
#endif //MESHCORE_AABBFACTORY_H
