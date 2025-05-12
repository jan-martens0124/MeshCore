//
// Created by Jonas on 15/02/2022.
//

#ifndef MESHCORE_HASH_H
#define MESHCORE_HASH_H

#include "meshcore/core/VertexTriangle.h"
#include "meshcore/core/Sphere.h"
#include "meshcore/core/Plane.h"
#include "meshcore/core/Ray.h"

#if GLM_HAS_CXX11_STL
    #include <glm/gtx/hash.hpp>
    template<> struct std::hash<VertexTriangle> {
        size_t operator()(const VertexTriangle &vertexTriangle) const {
            size_t result = 0;
            auto hash = std::hash<Vertex>();
            glm::detail::hash_combine(result, hash(vertexTriangle.vertices[0]));
            glm::detail::hash_combine(result, hash(vertexTriangle.vertices[1]));
            glm::detail::hash_combine(result, hash(vertexTriangle.vertices[2]));
            return result;
        }
    };

    template<> struct std::hash<AABB> {
        size_t operator()(const AABB &aabb) const {
            size_t result = 0;
            auto hash = std::hash<Vertex>();
            glm::detail::hash_combine(result, hash(aabb.getMinimum()));
            glm::detail::hash_combine(result, hash(aabb.getMaximum()));
            return result;
        }
    };

    template<> struct std::hash<Sphere> {
        size_t operator()(const Sphere &sphere) const {
            size_t result = 0;
            auto vertexHash = std::hash<Vertex>();
            auto floatHash = std::hash<float>();
            glm::detail::hash_combine(result, vertexHash(sphere.getCenter()));
            glm::detail::hash_combine(result, floatHash(sphere.getRadius()));
            return result;
        }
    };

    template<> struct std::hash<Plane> {
        size_t operator()(const Plane &plane) const {
            size_t result = 0;
            auto vectorHash = std::hash<glm::vec3>();
            auto floatHash = std::hash<float>();
            glm::detail::hash_combine(result, vectorHash(plane.getNormal()));
            glm::detail::hash_combine(result, floatHash(plane.getD()));
            return result;
        }
    };

    template<> struct std::hash<Ray> {
        size_t operator()(const Ray &ray) const {
            size_t result = 0;
            auto vertexHash = std::hash<Vertex>();
            auto vectorHash = std::hash<glm::vec3>();
            glm::detail::hash_combine(result, vertexHash(ray.getOrigin()));
            glm::detail::hash_combine(result, vectorHash(ray.getDirection()));
            return result;
        }
    };
#else
    #include <functional>


    template<> struct std::hash<Vertex> {
        size_t operator()(const Vertex &vertex) const {
            return std::hash<float>()(vertex.x +  vertex.y + vertex.z);
        }
    };

    template<> struct std::hash<VertexTriangle> {
        size_t operator()(const VertexTriangle &vertexTriangle) const {
            return std::hash<float>()(vertexTriangle.vertices[0].x +  vertexTriangle.vertices[0].y + vertexTriangle.vertices[0].z +
                                      vertexTriangle.vertices[1].x +  vertexTriangle.vertices[1].y + vertexTriangle.vertices[1].z +
                                      vertexTriangle.vertices[2].x +  vertexTriangle.vertices[2].y + vertexTriangle.vertices[2].z);
        }
    };

    template<> struct std::hash<AABB> {
        size_t operator()(const AABB &aabb) const {
            const auto& min = aabb.getMinimum();
            const auto& max = aabb.getMaximum();
            return std::hash<float>()(min.x + min.y + min.z + max.x + max.y + max.z);
        }
    };

    template<> struct std::hash<Sphere> {
        size_t operator()(const Sphere &sphere) const {
            return std::hash<float>()(sphere.getCenter().x + sphere.getCenter().y + sphere.getCenter().z + sphere.getRadius());
        }
    };

    template<> struct std::hash<Plane> {
        size_t operator()(const Plane &plane) const {
            return std::hash<float>()(plane.getNormal().x + plane.getNormal().y + plane.getNormal().z + plane.getD());
        }
    };

    template<> struct std::hash<Ray> {
        size_t operator()(const Ray &ray) const {
            return std::hash<float>()(ray.getOrigin().x + ray.getOrigin().y + ray.getOrigin().z + ray.getDirection().x + ray.getDirection().y + ray.getDirection().z);
        }
    };
#endif

#endif //MESHCORE_HASH_H
