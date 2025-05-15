//
// Created by Jonas on 15/05/2025.
//

#include "meshcore/factories/SphereFactory.h"

#include <stdexcept>

#include "../external/miniball/Miniball.hpp"

Sphere SphereFactory::createMinimumBoundingSphere3(Vertex vertex1, Vertex vertex2, Vertex vertex3) {

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

Sphere SphereFactory::createMinimumBoundingSphere4(Vertex vertex1, Vertex vertex2, Vertex vertex3, Vertex vertex4) {

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

Sphere SphereFactory::createMinimumBoundingSphereGaertner(const std::vector<Vertex> &vertices) {

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

Sphere SphereFactory::createMinimumBoundingSphereOld(Vertex *vertices, int numberOfVertices,
    int numberOfSupportVertices) {

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
