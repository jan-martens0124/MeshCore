//
// Created by Jonas on 5/11/2021.
//

#ifndef MESHCORE_INTERSECTION_H
#define MESHCORE_INTERSECTION_H

#include "meshcore/core/Ray.h"
#include "meshcore/core/VertexTriangle.h"
#include "meshcore/core/WorldSpaceMesh.h"
#include "meshcore/core/OBB.h"
#include "meshcore/core/Sphere.h"
#include "meshcore/core/Line.h"
#include "meshcore/core/Plane.h"

struct AABBTriangleData; // Forward declaration for TriangleAABBData

namespace Intersection{

    // Ray-Triangle
    bool intersect(const Ray& ray, const VertexTriangle& triangle);
    float intersectionDistance(const Ray& ray, const VertexTriangle& triangle);

    // Triangle-Triangle
    bool intersect(const VertexTriangle& triangleA, const VertexTriangle& triangleB);

    // AABB-Triangle
    int intersect(const AABB& aabb, const VertexTriangle& vertexTriangle);
    bool intersect(const AABB& aabb, const VertexTriangle& vertexTriangle, const AABBTriangleData &triangleAABBData);

    // AABB-Ray
    bool intersect(const AABB &aabb, const Ray &ray);
    bool intersect(const AABB &aabb, const Ray &ray, float tMin, float tMax);

    // AABB-AABB
    bool intersect(const AABB& firstAABB, const AABB& secondAABB);
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
    float calculateOverlappingVolume(const Sphere& firstSphere, const Sphere& secondSphere);

    // Mesh-Mesh
    bool debugIntersects(const WorldSpaceMesh& worldSpaceMeshA, const WorldSpaceMesh& worldSpaceMeshB);
    bool intersect(const WorldSpaceMesh& worldSpaceMeshA, const WorldSpaceMesh& worldSpaceMeshB);
    bool inside(const WorldSpaceMesh& worldSpaceMeshA, const WorldSpaceMesh& worldSpaceMeshB);

    // Plane
    std::optional<Line> intersect(const Plane& planeA, const Plane& planeB);
    std::optional<glm::vec3> intersect(const Plane& plane, const Line& line);
    std::optional<glm::vec3> intersect(const Plane& planeA, const Plane& planeB, const Plane& planeC);
}

#endif //MESHCORE_INTERSECTION_H