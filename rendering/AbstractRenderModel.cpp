//
// Created by Jonas on 8/11/2021.
//

#include "AbstractRenderModel.h"
#include <iostream>

bool AbstractRenderModel::isVisible() const {
    return visible;
}

void AbstractRenderModel::setVisible(bool newVisible) {
    this->visible = newVisible;
    for (const auto &listener: this->listeners){
        listener->notifyVisibleChanged(this->visible);
    }
}

const Color &AbstractRenderModel::getColor() const {
    return color;
}

void AbstractRenderModel::setColor(const Color &newColor) {
    this->color = newColor;
    for (const auto &listener: this->listeners){
        listener->notifyColorChanged(this->color);
    }
}

const glm::mat4 &AbstractRenderModel::getTransformation() const {
    return transformationMatrix;
}

void AbstractRenderModel::setTransformationMatrix(const glm::mat4 &newTransformationMatrix) {
    AbstractRenderModel::transformationMatrix = newTransformationMatrix;
}

AbstractRenderModel::AbstractRenderModel(const glm::mat4& transformation, const std::string& name):
        color(Color(1)),
        vertexBuffer(new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer)),
        indexBuffer(new QOpenGLBuffer(QOpenGLBuffer::Type::IndexBuffer)),
        vertexArray(new QOpenGLVertexArrayObject()),
        transformationMatrix(transformation)
{
    this->name = name;
    if(name.size() > 0){
        this->name[0] = std::toupper(name[0]);
    }


}

AbstractRenderModel::~AbstractRenderModel() {
    delete vertexArray;
    delete indexBuffer;
    delete vertexBuffer;
}

AbstractRenderModel::AbstractRenderModel(AbstractRenderModel &&other) noexcept:
        color(other.color),
        transformationMatrix(other.transformationMatrix)
{
    this->indexBuffer = other.indexBuffer;
    this->vertexArray = other.vertexArray;
    this->vertexBuffer = other.vertexBuffer;
    this->visible = other.visible;
    this->name = other.name;

    other.indexBuffer = nullptr;
    other.vertexArray = nullptr;
    other.vertexBuffer = nullptr;
}

AbstractRenderModel &AbstractRenderModel::operator=(AbstractRenderModel &&other) noexcept {
    if(this != &other){
        this->indexBuffer = other.indexBuffer;
        this->vertexArray = other.vertexArray;
        this->vertexBuffer = other.vertexBuffer;
        this->color = other.color;
        this->transformationMatrix = other.transformationMatrix;
        this->visible = other.visible;
        this->name = other.name;

        other.indexBuffer = nullptr;
        other.vertexArray = nullptr;
        other.vertexBuffer = nullptr;
    }
    return *this;
}

const std::string &AbstractRenderModel::getName() const {
    return name;
}

void AbstractRenderModel::setName(const std::string &newName) {
    AbstractRenderModel::name = newName;
}
