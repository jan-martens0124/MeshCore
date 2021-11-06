//
// Created by Jonas on 10/12/2020.
//

#include <iostream>
#include "Transformation.h"
#include <glm/gtx/euler_angles.hpp>
#include "Ray.h"

Transformation::Transformation(): position(), scale(1.0f), yaw(0.0f), roll(0.0f), pitch(0.0f) {}

std::ostream& operator<<(std::ostream& o, const Transformation& transformation) {
    o << "\tScale: " << transformation.scale << "\n";
    o << "\tPosition: " << transformation.position << "\n";
    o << "\tYaw: " << transformation.yaw << "\n";
    o << "\tPitch: " << transformation.yaw << "\n";
    o << "\tRoll: " << transformation.yaw << "\n";
    glm::mat4 transform = transformation.getMatrix();
    o << "\n\t[" << transform.operator[](0) << "\n\t " << transform.operator[](1) << "\n\t " << transform.operator[](2) << "\n\t " << transform.operator[](3) << ']';
    return o;
}

glm::mat4& Transformation::getMatrix() const{
    if(!matrix.has_value()){
        glm::mat4 newModelTransformationMatrix(1.0f);
        newModelTransformationMatrix = glm::translate(newModelTransformationMatrix, position);
        newModelTransformationMatrix = glm::scale(newModelTransformationMatrix, glm::vec3(scale));
        newModelTransformationMatrix *= this->getRotationMatrix();
        this->matrix = newModelTransformationMatrix;
    }
    return matrix.value();
}

glm::mat4& Transformation::getInverseMatrix() const {
    if(!inverse.has_value()){
        inverse = glm::inverse(this->getMatrix());
    }
    return inverse.value();
}

void Transformation::invalidateMatrices() const {
    matrix = std::nullopt;
    inverse = std::nullopt;
}

glm::mat4 Transformation::getRotationMatrix() const {
    if(!rotationMatrix.has_value()){
        rotationMatrix = glm::eulerAngleXYZ(roll, pitch, yaw);
    }
    return rotationMatrix.value();
}

float Transformation::getScale() const {
    return scale;
}

void Transformation::setScale(float newScale) {
    this->invalidateMatrices();
    this->scale = newScale;
}

void Transformation::deltaScale(float deltaScale) {
    this->invalidateMatrices();
    this->scale += deltaScale;
}

void Transformation::factorScale(float scaleFactor){
    this->invalidateMatrices();
    this->scale *= scaleFactor;
}

void Transformation::setRotation(float newYaw, float newPitch, float newRoll) {
    this->invalidateRotationMatrix();
    this->yaw = newYaw;
    this->pitch = newPitch;
    this->roll = newRoll;
}


void Transformation::deltaRotation(float deltaYaw, float deltaPitch, float deltaRoll) {
    this->invalidateRotationMatrix();
    this->yaw += deltaYaw;
    this->pitch += deltaPitch;
    this->roll += deltaRoll;
}

float Transformation::getYaw() const {
    return yaw;
}

void Transformation::setYaw(float newYaw) {
    this->invalidateRotationMatrix();
    this->yaw = newYaw;
}

void Transformation::deltaYaw(float deltaYaw) {
    this->invalidateRotationMatrix();
    this->yaw += deltaYaw;
}

float Transformation::getPitch() const {
    return pitch;
}

void Transformation::setPitch(float newPitch) {
    this->invalidateRotationMatrix();
    this->pitch = newPitch;
}

void Transformation::deltaPitch(float deltaPitch) {
    this->invalidateRotationMatrix();
    this->pitch += deltaPitch;
}

float Transformation::getRoll() const {
    return roll;
}

void Transformation::setRoll(float newRoll) {
    this->invalidateRotationMatrix();
    this->roll = newRoll;
}

void Transformation::deltaRoll(float deltaRoll) {
    this->invalidateRotationMatrix();
    this->roll += deltaRoll;
}

const glm::vec3 &Transformation::getPosition() const {
    return position;
}

void Transformation::setPosition(const glm::vec3 &newPosition) {
    this->invalidateMatrices();
    this->position = newPosition;
}

void Transformation::deltaPosition(const glm::vec3 &deltaPosition) {
    this->invalidateMatrices();
    this->position += deltaPosition;
}

void Transformation::invalidateRotationMatrix() const {
    this->rotationMatrix = std::nullopt;
    this->invalidateMatrices();
}

bool Transformation::operator==(const Transformation &other) const {
    return scale == other.scale &&
           yaw == other.yaw &&
           pitch == other.pitch &&
           roll == other.roll &&
           position == other.position;
}

bool Transformation::operator!=(const Transformation &other) const {
    return !(other == *this);
}

Vertex Transformation::transformVertex(const Vertex &vertex) const {
    return this->getMatrix() * glm::vec4(vertex, 1);
}

Vertex Transformation::inverseTransformVertex(const Vertex& vertex) const {
    return this->getInverseMatrix() * glm::vec4(vertex, 1);
}