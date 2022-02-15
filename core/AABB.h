//
// Created by Jonas on 7/10/2021.
//

#ifndef MESHCORE_AABB_H
#define MESHCORE_AABB_H

#include "Core.h"
#include "Vertex.h"

/** Axis Aligned Bounding Box **/
class AABB {
private:
    Vertex minimum;
    Vertex maximum;

public:
    MC_FUNC_QUALIFIER AABB(): minimum(), maximum(){};

    MC_FUNC_QUALIFIER AABB(Vertex minimum, Vertex maximum): minimum(minimum), maximum(maximum) {}

    MC_FUNC_QUALIFIER Vertex getMinimum() const {
        return this->minimum;
    }

    MC_FUNC_QUALIFIER Vertex getMaximum() const {
        return this->maximum;
    }

    MC_FUNC_QUALIFIER Vertex getCenter() const {
        return (this->maximum + this->minimum)/2.0f;
    }

    MC_FUNC_QUALIFIER Vertex getHalf() const{
        return (this->maximum - this->minimum)/2.0f;
    }

    MC_FUNC_QUALIFIER float getSurfaceArea() const {
        auto delta = maximum - minimum;
        return 2 * (delta.x * delta.y + delta.x * delta.z + delta.y * delta.z);
    }

    MC_FUNC_QUALIFIER float getVolume() const {
        auto delta = maximum - minimum;
        return delta.x * delta.y * delta.z;
    }
};

#endif //MESHCORE_AABB_H
