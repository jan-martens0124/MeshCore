//
// Created by Jonas on 15/03/2022.
//

#ifndef OPTIXMESHCORE_OBB_H
#define OPTIXMESHCORE_OBB_H

#include "AABB.h"
#include "Transformation.h"

/** OBB as an AABB in its own model space, defined by a transformation **/
class OBB {
private:
    AABB aabb;
    Transformation transformation; // potential memory bottleneck, 224 bytes TODO store 2 glm::mat4x3 matrices instead (normal and inverse) or test low memory rotation class

//    const glm::mat4x3 transformationMatrix;
//    const glm::mat4x3 inverseTransformationMatrix;

public:

    MC_FUNC_QUALIFIER OBB(): aabb(), transformation(){};

    MC_FUNC_QUALIFIER OBB(const AABB& aabb, const Transformation& transformation): aabb(aabb), transformation(transformation){}

    MC_FUNC_QUALIFIER [[nodiscard]] float getSurfaceArea() const {
        // Assume the transformation is an affine transformation
        return aabb.getSurfaceArea() * transformation.getScale();
    }

    MC_FUNC_QUALIFIER [[nodiscard]] float getVolume() const {
        // Assume the transformation is an affine transformation
        auto scale = transformation.getScale();
        return aabb.getVolume() * scale * scale * scale;
    }

    MC_FUNC_QUALIFIER [[nodiscard]] Vertex getCenter() const {
        return transformation.transformVertex(aabb.getCenter());
    }

    MC_FUNC_QUALIFIER [[nodiscard]] const AABB &getAabb() const {
        return aabb;
    }

    MC_FUNC_QUALIFIER [[nodiscard]] const Transformation &getTransformation() const {
        return transformation;
    }

    MC_FUNC_QUALIFIER [[nodiscard]] bool containsPoint(Vertex point) const {
        auto transformedPoint = this->transformation.inverseTransformVertex(point);
        return this->aabb.containsPoint(transformedPoint);
    }

    MC_FUNC_QUALIFIER [[nodiscard]] Vertex getClosestPoint(Vertex point) const {
        auto transformedPoint = this->transformation.inverseTransformVertex(point);
        return this->aabb.getClosestPoint(point);
    }

};

#endif //OPTIXMESHCORE_OBB_H
