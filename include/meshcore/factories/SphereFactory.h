//
// Created by Jonas on 4/10/2022.
//

#ifndef MESHCORE_SPHEREFACTORY_H
#define MESHCORE_SPHEREFACTORY_H

#include "../core/VertexTriangle.h"
#include "../core/Sphere.h"
#include "external/miniball/Miniball.hpp"
#include <vector>
#include <stack>

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

    static Sphere createMinimumBoundingSphere3(Vertex vertex1, Vertex vertex2, Vertex vertex3){

        // Based on https://www.flipcode.com/archives/Smallest_Enclosing_Spheres.shtml
        // Referenced in "Real-Time Collision Detection" by Christer Ericson

        auto a = vertex2 - vertex1;
        auto b = vertex3 - vertex1;

        auto Denominator = 2.0f * (glm::dot(glm::cross(a,b),glm::cross(a,b)));

        auto o = (glm::dot(b,b) * glm::cross(glm::cross(a,b),a) +
                    (glm::dot(a,a) * glm::cross(b,glm::cross(a, b)))) / Denominator;

        auto radius = glm::length(o);
        auto center = vertex1 + o;

        assert(glm::length(center - vertex1) <= radius);
        assert(glm::length(center - vertex2) <= radius);
        assert(glm::length(center - vertex3) <= radius);

        return {center, radius};
    }

    static Sphere createMinimumBoundingSphere4(Vertex vertex1, Vertex vertex2, Vertex vertex3, Vertex vertex4){

        // Based on https://www.flipcode.com/archives/Smallest_Enclosing_Spheres.shtml
        // Referenced in "Real-Time Collision Detection" by Christer Ericson

        auto a = vertex2 - vertex1;
        auto b = vertex3 - vertex1;
        auto c = vertex4 - vertex1;

        float Denominator = 2.0f * glm::determinant(glm::mat3(a,b,c));

        auto o = (glm::dot(c,c) * glm::cross(a,b) +
                  glm::dot(b,b) * glm::cross(c,a) +
                  glm::dot(a,a) * glm::cross(b,c)) / Denominator;

        auto radius = glm::length(o);
        auto center = vertex1 + o;

        assert(glm::length(center - vertex1) <= radius);
        assert(glm::length(center - vertex2) <= radius);
        assert(glm::length(center - vertex3) <= radius);
        assert(glm::length(center - vertex4) <= radius);

        return {center, radius};
    }

private:
    static Sphere createMinimumBoundingSphereGaertner(const std::vector<Vertex>& vertices) {

        typedef float mytype;             // coordinate type
        const int       d = 3;            // dimension
        std::list<std::vector<mytype> > lp;
        for (const auto &vertex: vertices){
            std::vector<mytype> p(d);
            p[0] = vertex.x;
            p[1] = vertex.y;
            p[2] = vertex.z;
            lp.push_back(p);
        }

        // define the types of iterators through the points and their coordinates
        // ----------------------------------------------------------------------
        typedef std::list<std::vector<mytype>>::const_iterator PointIterator;
        typedef std::vector<mytype>::const_iterator CoordIterator;

        // create an instance of Miniball
        // ------------------------------
        typedef Miniball::Miniball<Miniball::CoordAccessor<PointIterator, CoordIterator>> MB;
        MB mb (d, lp.begin(), lp.end());
        Vertex center = {mb.center()[0], mb.center()[1], mb.center()[2]};
        float radius = glm::sqrt(mb.squared_radius());
        return {center, radius * (1+1e-4f)};
    }

private:
    static Sphere createMinimumBoundingSphereOld(Vertex* vertices, int numberOfVertices, int numberOfSupportVertices) {

        // Based on https://www.flipcode.com/archives/Smallest_Enclosing_Spheres.shtml
        // Referenced in "Real-Time Collision Detection" by Christer Ericson

        if(numberOfVertices == 0 || numberOfSupportVertices == 4) {
            switch(numberOfSupportVertices) {
                case 0:
                    return Sphere{};
                case 1:
                    return createMinimumBoundingSphere1(vertices[0]);
                case 2:
                    return createMinimumBoundingSphere2(vertices[0], vertices[1]);
                case 3:
                    return createMinimumBoundingSphere3(vertices[0], vertices[1], vertices[2]);
                case 4:
                    return createMinimumBoundingSphere4(vertices[0], vertices[1], vertices[2], vertices[3]);
                default:
                    throw std::runtime_error("Invalid number of vertices or support vertices whilst creating minimum bounding sphere");
            }
        }

        Sphere minimumBoundingSphere = createMinimumBoundingSphereOld(vertices, numberOfVertices - 1, numberOfSupportVertices);

        if(!minimumBoundingSphere.containsPoint(vertices[numberOfVertices - 1]))   // Signed distance to sphere
        {
            for(int i = numberOfVertices - 2; i >= 0; i--)
            {
                Vertex T = vertices[i];
                vertices[i] = vertices[i + 1];
                vertices[i + 1] = T;
            }

            minimumBoundingSphere = createMinimumBoundingSphereOld(vertices + 1, numberOfVertices - 1, numberOfSupportVertices + 1);
        }

        return minimumBoundingSphere;
    }
};


#endif //MESHCORE_SPHEREFACTORY_H
