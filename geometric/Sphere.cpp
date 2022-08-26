//
// Created by Jonas on 25/08/2022.
//

#include "Intersection.h"

namespace Intersection {

    bool intersect(const Sphere& sphere, const AABB& aabb){
        auto distanceToCenterSquared = aabb.getDistanceSquaredTo(sphere.center);
        return distanceToCenterSquared <= sphere.radius * sphere.radius;
    }

    bool intersect(const Sphere& sphere, const OBB& obb){
        auto distanceToCenterSquared = obb.getDistanceSquaredTo(sphere.center);
        return distanceToCenterSquared <= sphere.radius * sphere.radius;
    }

    bool intersect(const Sphere& sphere, const VertexTriangle& vertexTriangle){
        auto closestPointToCenter = vertexTriangle.getClosestPoint(sphere.center);
        auto delta = closestPointToCenter - sphere.center;
        return glm::dot(delta, delta) <= sphere.radius * sphere.radius;
    }

    bool intersect(const Sphere& sphere, const Ray& ray){

        // As described in "Real-Time Collision Detection" by Christer Ericson
        auto m = ray.origin - sphere.center;
        auto c = glm::dot(m, m) - sphere.radius * sphere.radius;

        // If there is definitely at least one real root, there must be an intersection
        if (c <= 0.0f) return true;
        auto b = glm::dot(m, ray.direction);

        // Early exit if ray origin outside sphere and ray pointing away from sphere
        if (b > 0.0f) return false;
        float disc = b*b - c;

        // A negative discriminant corresponds to ray missing sphere
        if (disc < 0.0f) return false;

        // Now ray must hit sphere
        return true;
    }

    bool intersect(const Sphere& firstSphere, const Sphere& secondSphere){
        auto delta = firstSphere.center - secondSphere.center;
        auto distanceSquared = glm::dot(delta,delta);
        auto radiusSum = firstSphere.radius + secondSphere.radius;
        return distanceSquared <= radiusSum*radiusSum;
    }
}
