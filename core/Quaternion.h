//
// Created by Jonas on 20/05/2022.
//

#ifndef MESHCORE_QUATERNION_H
#define MESHCORE_QUATERNION_H

#include "Core.h"
#include <glm/gtc/quaternion.hpp>

class Quaternion: public glm::fquat{
public:

    MC_FUNC_QUALIFIER Quaternion(): qua(1,0,0,0){};

    MC_FUNC_QUALIFIER explicit Quaternion(const glm::fquat &quaternion): qua(quaternion){}

    MC_FUNC_QUALIFIER explicit Quaternion(float yaw, float pitch, float roll){
        auto sx = sin(roll/2);
        auto sy = sin(pitch/2);
        auto sz = sin(yaw/2);
        auto cx = cos(roll/2);
        auto cy = cos(pitch/2);
        auto cz = cos(yaw/2);
        this->w = cx*cy*cz - sx*sy*sz;
        this->x = sx*cy*cz + cx*sy*sz;
        this->y = cx*sy*cz - sx*cy*sz;
        this->z = cx*cy*sz + sx*sy*cz;
    }

    MC_FUNC_QUALIFIER explicit Quaternion(const glm::mat3 rotationMatrix): qua(glm::quat_cast(rotationMatrix)){}

    MC_FUNC_QUALIFIER explicit Quaternion(const glm::mat4 rotationMatrix): qua(glm::quat_cast(rotationMatrix)){}

    MC_FUNC_QUALIFIER [[nodiscard]] Vertex rotateVertex(const glm::vec3& vertex) const{
        return (*this)*vertex; // glm quaternion multiplication does what we need
    };

    MC_FUNC_QUALIFIER [[nodiscard]] Vertex inverseRotateVertex(const glm::vec3& vertex) const{
        return glm::inverse(*this)*vertex;
    };

    MC_FUNC_QUALIFIER [[nodiscard]] glm::mat4 getMatrix() const{
        return glm::mat4_cast(*this);
    }

    MC_FUNC_QUALIFIER [[nodiscard]] glm::mat4 getInverseMatrix() const{
        return glm::mat4_cast(glm::inverse(*this));
    }

    MC_FUNC_QUALIFIER [[nodiscard]] Quaternion getInverse() const{
        return Quaternion(glm::inverse(*this));
    }

    MC_FUNC_QUALIFIER Quaternion operator*(const Quaternion& other) const{
        return Quaternion(glm::fquat(*this) * other);
    }

};
#endif //MESHCORE_QUATERNION_H
