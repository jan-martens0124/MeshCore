//
// Created by Jonas on 9/02/2023.
//

#ifndef MESHCORE_DISTANCE_H
#define MESHCORE_DISTANCE_H

#include "meshcore/core/Sphere.h"
#include "meshcore/core/AABB.h"
#include "meshcore/core/OBB.h"
#include "meshcore/core/VertexTriangle.h"
#include "meshcore/core/WorldSpaceMesh.h"

namespace Distance {

    float distance(const Sphere& firstSphere, const Sphere& secondSphere);

    float distance(const Sphere& sphere, const AABB& aabb);
    float distanceSquared(const Sphere& sphere, const AABB& aabb);

    float distance(const Sphere& sphere, const OBB& obb);

    float distance(const Sphere& sphere, const VertexTriangle& vertexTriangle);

    float distance(const VertexTriangle& triangleA, const VertexTriangle& triangleB);
    float distanceSquared(const VertexTriangle& triangleA, const VertexTriangle& triangleB, glm::vec3* triangleAClosestPoint, glm::vec3* triangleBClosestPoint);

    float distance(const AABB& firstAABB, const AABB& secondAABB);

    float distanceSquared(const AABB& firstAABB, const AABB& secondAABB);

    float distance(const VertexTriangle& triangle, const AABB& aabb);

    float distance(const WorldSpaceMesh& worldSpaceMeshA, const WorldSpaceMesh& worldSpaceMeshB);

    float distance(const WorldSpaceMesh& worldSpaceMeshA, const WorldSpaceMesh& worldSpaceMeshB, Vertex* closestVertexA, Vertex* closestVertexB);

}

#endif //MESHCORE_DISTANCE_H