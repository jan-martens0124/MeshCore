//
// Created by Jonas on 17/03/2022.
//

#ifndef OPTIXMESHCORE_AABBFACTORY_H
#define OPTIXMESHCORE_AABBFACTORY_H

#include <unordered_set>
#include <memory>
#include "../core/AABB.h"
#include "../core/VertexTriangle.h"

class AABBFactory{

public:
    static AABB createAABB(const std::vector<Vertex>& vertices){
        return createAABB(vertices.begin(), vertices.end());
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

};
#endif //OPTIXMESHCORE_AABBFACTORY_H
