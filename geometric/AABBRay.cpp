//
// Created by Jonas on 5/11/2021.
//

#ifndef OPTIXMESHCORE_AABBRAY_CPP
#define OPTIXMESHCORE_AABBRAY_CPP

#include "Intersection.h"

namespace Intersection {

    bool intersect(const AABB &aabb, const Ray &ray) {
        float tmin = -std::numeric_limits<float>::infinity();
        float tmax = std::numeric_limits<float>::infinity();

        float tx1 = (aabb.getMinimum().x - ray.origin.x)*ray.inverseDirection.x;
        float tx2 = (aabb.getMaximum().x - ray.origin.x)*ray.inverseDirection.x;

        tmin = std::fmax(tmin,std::fmin(tx1, tx2));
        tmax = std::fmin(tmax,std::fmax(tx1, tx2));

        float ty1 = (aabb.getMinimum().y - ray.origin.y)*ray.inverseDirection.y;
        float ty2 = (aabb.getMaximum().y - ray.origin.y)*ray.inverseDirection.y;

        tmin = std::fmax(tmin, std::fmin(ty1, ty2));
        tmax = std::fmin(tmax, std::fmax(ty1, ty2));

        float tz1 = (aabb.getMinimum().z - ray.origin.z)*ray.inverseDirection.z;
        float tz2 = (aabb.getMaximum().z - ray.origin.z)*ray.inverseDirection.z;

        tmin = std::fmax(tmin, std::fmin(tz1, tz2));
        tmax = std::fmin(tmax, std::fmax(tz1, tz2));

        return (tmax >= std::fmax(tmin,0.0));
    }
}

#endif //OPTIXMESHCORE_AABBRAY_CPP
