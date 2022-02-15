//
// Created by Jonas on 10/12/2020.
//

#ifndef MESHCORE_TRANSFORMATION_H
#define MESHCORE_TRANSFORMATION_H

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <optional>
#include "Vertex.h"

class Transformation {

private:
    float scale;
    float yaw;
    float pitch;
    float roll;
    glm::vec3 position;

private:
    mutable std::optional<glm::mat4> rotationMatrix;
    mutable std::optional<glm::mat4> matrix;
    mutable std::optional<glm::mat4> inverse;

public:
    Transformation();
    Transformation(const Transformation& other) = default;
    bool operator==(const Transformation &other) const;
    bool operator!=(const Transformation &other) const;

private:
    void invalidateMatrices() const;
    void invalidateRotationMatrix() const;
    glm::mat4 getRotationMatrix() const;

public:

    Vertex transformVertex(const Vertex& vertex) const;
    Vertex inverseTransformVertex(const Vertex& vertex) const;

    glm::mat4 getMatrix() const;
    glm::mat4 getInverseMatrix() const;

    float getScale() const;
    void setScale(float newScale);
    void deltaScale(float deltaScale);
    void factorScale(float scaleFactor);

    float getYaw() const;
    float getPitch() const;
    float getRoll() const;

    void setRotation(float newYaw, float newPitch, float newRoll);
    void setYaw(float newYaw);
    void setPitch(float newPitch);
    void setRoll(float newRoll);

    void deltaRotation(float deltaYaw, float deltaPitch, float deltaRoll);
    void deltaYaw(float deltaYaw);
    void deltaPitch(float deltaPitch);
    void deltaRoll(float deltaRoll);

    const glm::vec3 &getPosition() const;
    void setPosition(const glm::vec3 &newPosition);
    void deltaPosition(const glm::vec3 &deltaPosition);
};

#endif //MESHCORE_TRANSFORMATION_H