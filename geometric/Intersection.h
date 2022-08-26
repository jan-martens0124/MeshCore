//
// Created by Jonas on 5/11/2021.
//

#ifndef OPTIXMESHCORE_INTERSECTION_H
#define OPTIXMESHCORE_INTERSECTION_H

#include "../core/Ray.h"
#include "../core/VertexTriangle.h"
#include "../core/WorldSpaceMesh.h"
#include "../core/OBB.h"
#include "../core/Sphere.h"

namespace Intersection{

    // Ray-Triangle
    bool intersect(const Ray& ray, const VertexTriangle& triangle);
    float intersectionDistance(const Ray& ray, const VertexTriangle& triangle);

    // Triangle-Triangle
    bool intersect(const VertexTriangle& triangleA, const VertexTriangle& triangleB);

    // AABB-Triangle
    int intersect(const AABB& aabb, const VertexTriangle& vertexTriangle);

    // AABB-Ray
    bool intersect(const AABB &aabb, const Ray &ray);

    // AABB-AABB
    bool intersect(const AABB& firstAABB, const AABB& secondAABB, const glm::mat4 &firstToSecondTransformationMatrix, const glm::mat4 &secondToFirstTransformationMatrix);

    // OBB-OBB
    bool intersect(const OBB& firstOBB, const OBB& secondOBB);

    // OBB-Triangle
    bool intersect(const OBB& obb, const VertexTriangle& vertexTriangle);

    // OBB-Ray
    bool intersect(const OBB& obb, const Ray& ray);

    // Sphere-AABB
    bool intersect(const Sphere& sphere, const AABB& aabb);

    // Sphere-OBB
    bool intersect(const Sphere& sphere, const OBB& obb);

    // Sphere-Triangle
    bool intersect(const Sphere& sphere, const VertexTriangle& vertexTriangle);

    // Sphere-Ray
    bool intersect(const Sphere& sphere, const Ray& ray);

    // Sphere-Sphere
    bool intersect(const Sphere& firstSphere, const Sphere& secondSphere);

    // Mesh-Mesh
    bool intersects(const WorldSpaceMesh& worldSpaceMeshA, const WorldSpaceMesh& worldSpaceMeshB);
    bool inside(const WorldSpaceMesh& worldSpaceMeshA, const WorldSpaceMesh& worldSpaceMeshB);
}

#endif //OPTIXMESHCORE_INTERSECTION_H