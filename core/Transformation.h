//
// Created by Jonas on 10/12/2020.
//

#ifndef MESHCORE_TRANSFORMATION_H
#define MESHCORE_TRANSFORMATION_H

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "Vertex.h"
#include "Core.h"
#include "Quaternion.h"

class Transformation {

private:
    float scale;
    float yaw;
    float pitch;
    float roll;
    glm::vec3 position;

private:
    mutable glm::mat4 matrix;
    mutable glm::mat4 inverseMatrix;
    mutable glm::mat4 rotationMatrix;
    mutable bool validMatrix = false;
    mutable bool validInverseMatrix = false;
    mutable bool validRotationMatrix = false;

public:
    MC_FUNC_QUALIFIER Transformation(): position(), scale(1.0f), yaw(0.0f), roll(0.0f), pitch(0.0f),
                      matrix(), inverseMatrix(), rotationMatrix() {}

    MC_FUNC_QUALIFIER bool operator==(const Transformation &other) const {
        return scale == other.scale &&
               yaw == other.yaw &&
               pitch == other.pitch &&
               roll == other.roll &&
               position == other.position;
    }
    MC_FUNC_QUALIFIER bool operator!=(const Transformation &other) const {
        return !(other == *this);
    }

private:
    MC_FUNC_QUALIFIER void invalidateMatrices() const {
        validMatrix = false;
        validInverseMatrix = false;
    }
    MC_FUNC_QUALIFIER void invalidateRotationMatrix() const {
        this->validRotationMatrix = false;
        this->invalidateMatrices();
    }

public:
    MC_FUNC_QUALIFIER glm::mat4 getRotationMatrix() const {
        if(!validRotationMatrix){
            rotationMatrix = glm::eulerAngleXYZ(roll, pitch, yaw);
        }
        return rotationMatrix;
    }

public:

    MC_FUNC_QUALIFIER glm::mat4 getMatrix() const {
        if(!validMatrix){
            glm::mat4 newModelTransformationMatrix(1.0f);
            newModelTransformationMatrix = glm::translate(newModelTransformationMatrix, position);
            newModelTransformationMatrix = glm::scale(newModelTransformationMatrix, glm::vec3(scale));
            newModelTransformationMatrix *= this->getRotationMatrix();
            this->matrix = newModelTransformationMatrix;
        }
        return matrix;
    }
    MC_FUNC_QUALIFIER glm::mat4 getInverseMatrix() const {
        if(!validInverseMatrix){
            inverseMatrix = glm::inverse(this->getMatrix());
        }
        return inverseMatrix;
    }

    MC_FUNC_QUALIFIER float getScale() const {
        return scale;
    }
    MC_FUNC_QUALIFIER void setScale(float newScale) {
        this->invalidateMatrices();
        this->scale = newScale;
    }
    MC_FUNC_QUALIFIER void deltaScale(float deltaScale) {
        this->invalidateMatrices();
        this->scale += deltaScale;
    }
    MC_FUNC_QUALIFIER void factorScale(float scaleFactor){
        this->invalidateMatrices();
        this->scale *= scaleFactor;
    }

    MC_FUNC_QUALIFIER void setRotation(float newYaw, float newPitch, float newRoll) {
        this->invalidateRotationMatrix();
        this->yaw = newYaw;
        this->pitch = newPitch;
        this->roll = newRoll;
    }
    MC_FUNC_QUALIFIER void deltaRotation(float deltaYaw, float deltaPitch, float deltaRoll) {
        this->invalidateRotationMatrix();
        this->yaw += deltaYaw;
        this->pitch += deltaPitch;
        this->roll += deltaRoll;
    }

    MC_FUNC_QUALIFIER float getYaw() const {
        return yaw;
    }
    MC_FUNC_QUALIFIER void setYaw(float newYaw) {
        this->invalidateRotationMatrix();
        this->yaw = newYaw;
    }
    MC_FUNC_QUALIFIER void deltaYaw(float deltaYaw) {
        this->invalidateRotationMatrix();
        this->yaw += deltaYaw;
    }

    MC_FUNC_QUALIFIER float getPitch() const {
        return pitch;
    }
    MC_FUNC_QUALIFIER void setPitch(float newPitch) {
        this->invalidateRotationMatrix();
        this->pitch = newPitch;
    }
    MC_FUNC_QUALIFIER void deltaPitch(float deltaPitch) {
        this->invalidateRotationMatrix();
        this->pitch += deltaPitch;
    }

    MC_FUNC_QUALIFIER float getRoll() const {
        return roll;
    }
    MC_FUNC_QUALIFIER void setRoll(float newRoll) {
        this->invalidateRotationMatrix();
        this->roll = newRoll;
    }
    MC_FUNC_QUALIFIER void deltaRoll(float deltaRoll) {
        this->invalidateRotationMatrix();
        this->roll += deltaRoll;
    }

    MC_FUNC_QUALIFIER const glm::vec3& getPosition() const {
        return position;
    }
    MC_FUNC_QUALIFIER void setPosition(const glm::vec3 &newPosition) {
        this->invalidateMatrices();
        this->position = newPosition;
    }
    MC_FUNC_QUALIFIER void deltaPosition(const glm::vec3 &deltaPosition) {
        this->invalidateMatrices();
        this->position += deltaPosition;
    }

    MC_FUNC_QUALIFIER Vertex transformVertex(const Vertex &vertex) const {
        return this->getMatrix() * glm::vec4(vertex, 1);
    }
    MC_FUNC_QUALIFIER Vertex inverseTransformVertex(const Vertex& vertex) const {
        return this->getInverseMatrix() * glm::vec4(vertex, 1);
    }

    MC_FUNC_QUALIFIER Vertex transformVector(const Vertex &vertex) const {
        return this->getMatrix() * glm::vec4(vertex, 0);
    }
    MC_FUNC_QUALIFIER Vertex inverseTransformVector(const Vertex& vertex) const {
        return this->getInverseMatrix() * glm::vec4(vertex, 0);
    }

    MC_FUNC_QUALIFIER static Transformation fromRotationMatrix(const glm::mat3& rotationMatrix){

        Transformation returnObject;
        {
            float pitch = glm::asin(glm::clamp(rotationMatrix[2][0], -1.0f, 1.0f)); // pitch?

            float roll;
            float yaw;

            if(glm::abs(rotationMatrix[2][0]) < (1-1e-8)){
                roll = std::atan2(-rotationMatrix[2][1], rotationMatrix[2][2]);
                yaw = std::atan2(-rotationMatrix[1][0], rotationMatrix[0][0]);
            }
            else{
                yaw = 0.0f;
                roll = std::atan2(rotationMatrix[1][2], rotationMatrix[1][1]);
            }

            returnObject.setRotation(yaw, pitch, roll);
        }

#if !NDEBUG

        // Assert the matrices are equal, with at least a limited level of accuracy
        auto testRotationMatrix = glm::mat3(returnObject.getRotationMatrix());

        assert(glm::all(glm::epsilonEqual(testRotationMatrix[0],rotationMatrix[0], 0.0001f)));
        assert(glm::all(glm::epsilonEqual(testRotationMatrix[1],rotationMatrix[1], 0.0001f)));
        assert(glm::all(glm::epsilonEqual(testRotationMatrix[2],rotationMatrix[2], 0.0001f)));
#endif

        return returnObject;
    }

    MC_FUNC_QUALIFIER static Transformation fromEulerAngles(float pitch, float yaw, float roll) {
        Transformation returnObject;
        returnObject.setRotation(yaw, pitch, roll);
        return returnObject;
    }

    MC_FUNC_QUALIFIER static Transformation fromQuaternion(const Quaternion& quaternion){
        return Transformation::fromRotationMatrix(quaternion.getMatrix());
    }
};

#endif //MESHCORE_TRANSFORMATION_H