//
// Created by Jonas on 15/02/2022.
//

#ifndef OPTIXMESHCORE_HASH_H
#define OPTIXMESHCORE_HASH_H

#include "../core/VertexTriangle.h"

#if GLM_HAS_CXX11_STL
    #include <glm/gtx/hash.hpp>
    template<> struct std::hash<VertexTriangle> {
        size_t operator()(const VertexTriangle &vertexTriangle) const {
            size_t result = 0;
            auto hash = std::hash<Vertex>();
            glm::detail::hash_combine(result, hash(vertexTriangle.vertex0));
            glm::detail::hash_combine(result, hash(vertexTriangle.vertex1));
            glm::detail::hash_combine(result, hash(vertexTriangle.vertex2));
            return result;
        }
    };
#else
    template<> struct std::hash<VertexTriangle> {
        size_t operator()(const VertexTriangle &vertexTriangle) const {
        return std::hash<float>()(vertexTriangle.vertex0.x +  vertexTriangle.vertex0.y + vertexTriangle.vertex0.z +
                                  vertexTriangle.vertex1.x +  vertexTriangle.vertex1.y + vertexTriangle.vertex1.z +
                                  vertexTriangle.vertex2.x +  vertexTriangle.vertex2.y + vertexTriangle.vertex2.z);
        }
    };
#endif

#endif //OPTIXMESHCORE_HASH_H
