//
// Created by Jonas on 4/10/2022.
//

#ifndef MESHCORE_SPHEREFACTORY_H
#define MESHCORE_SPHEREFACTORY_H

#include "../core/VertexTriangle.h"
#include "../core/Sphere.h"
#include <vector>
#include <stack>
#include <iostream>

class SphereFactory{
public:
    static Sphere createMinimumBoundingSphere(const std::vector<Vertex>& vertices){
        return createMinimumBoundingSphereGaertner(vertices);
    }

    static Sphere createMinimumBoundingSphere(const std::vector<VertexTriangle>& triangles){

        std::vector<Vertex> vertices;
        vertices.reserve(triangles.size() * 3);
        for(auto& triangle : triangles){
            vertices.push_back(triangle.vertices[0]);
            vertices.push_back(triangle.vertices[1]);
            vertices.push_back(triangle.vertices[2]);
        }

        auto result = createMinimumBoundingSphereGaertner(vertices);

#if !NDEBUG
        for (const auto &point: vertices){
            if(!result.containsPoint(point)){
                auto deltaCenter = point - result.getCenter();
                auto dot = glm::dot(deltaCenter, deltaCenter);
                auto radiusSquared = result.getRadiusSquared();
                std::cout << "Point outside bounding sphere!" << std::endl;
            }

            assert(result.containsPoint(point));
        }
#endif
        return result;
    }

private:

    static Sphere createMinimumBoundingSphere1(Vertex vertex1){
        return {vertex1, 0.0f};
    }

    static Sphere createMinimumBoundingSphere2(Vertex vertex1, Vertex vertex2){
        auto center = (vertex1 + vertex2) / 2.0f;
        auto radius = glm::distance(vertex1, vertex2) / 2.0f;

        assert(glm::distance(center, vertex1) <= radius);
        assert(glm::distance(center, vertex2) <= radius);

        return {center, radius};
    }

    static Sphere createMinimumBoundingSphere3(Vertex vertex1, Vertex vertex2, Vertex vertex3);

    static Sphere createMinimumBoundingSphere4(Vertex vertex1, Vertex vertex2, Vertex vertex3, Vertex vertex4);

private:
    static Sphere createMinimumBoundingSphereGaertner(const std::vector<Vertex>& vertices);

private:
    static Sphere createMinimumBoundingSphereOld(Vertex* vertices, int numberOfVertices, int numberOfSupportVertices);
};


#endif //MESHCORE_SPHEREFACTORY_H
