//
// Created by Jonas on 25/08/2022.
//

#include "Intersection.h"

namespace Intersection {

    bool intersect(const Sphere& sphere, const AABB& aabb){
        auto distanceToCenterSquared = aabb.getDistanceSquaredTo(sphere.getCenter());
        return distanceToCenterSquared <= sphere.getRadiusSquared();
    }

    bool intersect(const Sphere& sphere, const OBB& obb){
        auto distanceToCenterSquared = obb.getDistanceSquaredTo(sphere.getCenter());
        return distanceToCenterSquared <= sphere.getRadiusSquared();
    }

    bool intersect(const Sphere& sphere, const VertexTriangle& vertexTriangle){

        // A triangle intersects a sphere if the distance from the sphere center to the triangle is less than the sphere radius.
        auto closestPointToCenter = vertexTriangle.getClosestPoint(sphere.getCenter());
        auto delta = closestPointToCenter - sphere.getCenter();

        // Compare squared distances to avoid taking the square root to calculate the distance
        return glm::dot(delta, delta) <= sphere.getRadiusSquared();
    }

    bool intersect(const Sphere& sphere, const Ray& ray){

        // As described in "Real-Time Collision Detection" by Christer Ericson
        auto m = ray.origin - sphere.getCenter();
        auto c = glm::dot(m, m) - sphere.getRadius() * sphere.getRadius();

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

        // Two spheres intersect if the distance between their centers is less than the sum of their radii
        auto delta = firstSphere.getCenter() - secondSphere.getCenter();

        // Compare squared distances to avoid taking the square root
        auto distanceSquared = glm::dot(delta,delta);
        auto radiusSum = firstSphere.getRadius() + secondSphere.getRadius();
        return distanceSquared <= radiusSum*radiusSum;
    }

    float calculateOverlappingVolume(const Sphere& firstSphere, const Sphere& secondSphere){

        // Calculate distance between centers
        auto d = glm::length(firstSphere.getCenter() - secondSphere.getCenter());

        // Test if the first sphere is fully inside the second sphere
        if(d + firstSphere.getRadius() <= secondSphere.getRadius()){
            assert(firstSphere.getVolume() > 0.0f);
            return firstSphere.getVolume();
        }

        // Test if the second sphere is fully inside the first sphere
        if(d + secondSphere.getRadius() <= firstSphere.getRadius()){
            assert(secondSphere.getVolume() > 0.0f);
            return secondSphere.getVolume();
        }

        // Calculate how deep the spheres are overlapping
        auto intrusion = firstSphere.getRadius() + secondSphere.getRadius() - d;
        if(intrusion <= 0){
            return 0.0f;
        }

        assert(intrusion > 0.0f);
        assert(intrusion < firstSphere.getRadius() + secondSphere.getRadius());

        // Based on https://mathworld.wolfram.com/Sphere-SphereIntersection.html
        return glm::pi<float>() * intrusion * intrusion * (d*d + 2*d*firstSphere.getRadius() - 3*firstSphere.getRadiusSquared() + 2*d*secondSphere.getRadius() + 6*firstSphere.getRadius()*secondSphere.getRadius() - 3*secondSphere.getRadiusSquared()) / (12*d);
    }
}

namespace Distance {
    float distance(const Sphere& firstSphere, const Sphere& secondSphere){
        auto deltaCenter = firstSphere.getCenter() - secondSphere.getCenter();
        auto distanceCenterSquared = glm::dot(deltaCenter, deltaCenter);
        return glm::max(0.0f, glm::sqrt(distanceCenterSquared) - firstSphere.getRadius() - secondSphere.getRadius());
    }

    float distance(const Sphere& sphere, const AABB& aabb){
        auto closestPoint = aabb.getClosestPoint(sphere.getCenter());
        auto deltaCenter = sphere.getCenter() - closestPoint;
        auto distanceCenterSquared = glm::dot(deltaCenter, deltaCenter);
        return glm::max(0.0f, glm::sqrt(distanceCenterSquared) - sphere.getRadius());
    }

    float distanceSquared(const Sphere& sphere, const AABB& aabb){
        auto d = distance(sphere, aabb);
        return d * d;
    }

    float distance(const Sphere& sphere, const OBB& obb){
        auto closestPoint = obb.getClosestPoint(sphere.getCenter());
        auto deltaCenter = sphere.getCenter() - closestPoint;
        auto distanceCenterSquared = glm::dot(deltaCenter, deltaCenter);
        return glm::max(0.0f, glm::sqrt(distanceCenterSquared) - sphere.getRadius());
    }

    float distance(const Sphere& sphere, const VertexTriangle& vertexTriangle){
        auto closestPoint = vertexTriangle.getClosestPoint(sphere.getCenter());
        auto deltaCenter = sphere.getCenter() - closestPoint;
        auto distanceCenterSquared = glm::dot(deltaCenter, deltaCenter);
        return glm::max(0.0f, glm::sqrt(distanceCenterSquared) - sphere.getRadius());
    }
}
