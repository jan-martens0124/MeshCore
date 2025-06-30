//
// Created by Jonas on 5/11/2021.
//

#ifndef MESHCORE_AABBRAY_CPP
#define MESHCORE_AABBRAY_CPP

#include "meshcore/geometric/Intersection.h"

namespace Intersection {

    bool intersect(const AABB &aabb, const Ray &ray) {
        return intersect(aabb, ray, 0.0f, std::numeric_limits<float>::infinity());
    }

    bool intersect(const AABB &aabb, const Ray &ray, float tMin, float tMax) {

        float tx1 = (aabb.getMinimum().x - ray.origin.x)*ray.inverseDirection.x;
        float tx2 = (aabb.getMaximum().x - ray.origin.x)*ray.inverseDirection.x;

        tMin = std::fmax(tMin,std::fmin(tx1, tx2));
        tMax = std::fmin(tMax,std::fmax(tx1, tx2));

        float ty1 = (aabb.getMinimum().y - ray.origin.y)*ray.inverseDirection.y;
        float ty2 = (aabb.getMaximum().y - ray.origin.y)*ray.inverseDirection.y;

        tMin = std::fmax(tMin, std::fmin(ty1, ty2));
        tMax = std::fmin(tMax, std::fmax(ty1, ty2));

        float tz1 = (aabb.getMinimum().z - ray.origin.z)*ray.inverseDirection.z;
        float tz2 = (aabb.getMaximum().z - ray.origin.z)*ray.inverseDirection.z;

        tMin = std::fmax(tMin, std::fmin(tz1, tz2));
        tMax = std::fmin(tMax, std::fmax(tz1, tz2));

        return tMax >= tMin;
    }
}

#endif //MESHCORE_AABBRAY_CPP
