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
    if(!name.empty()){
        this->name[0] = std::toupper(name[0]);
    }
}

AbstractRenderModel::~AbstractRenderModel() {

    delete vertexArray;
    delete indexBuffer;
    delete vertexBuffer;

    if(this->detailDialog != nullptr){
        this->detailDialog->close();
        delete this->detailDialog;
    }
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
    this->name = newName;
    for (const auto &listener: this->listeners){
        listener->notifyNameChanged(this->name);
    }
}

QMenu* AbstractRenderModel::getContextMenu(){

    auto contextMenu = new QMenu(QString("Context menu"));

    QAction* visibleAction = contextMenu->addAction(QString("Visible"));
    QObject::connect(visibleAction, &QAction::triggered, [=](bool enabled){
        this->setVisible(enabled);
    });
    visibleAction->setCheckable(true);
    visibleAction->setChecked(this->isVisible());
    contextMenu->addAction(visibleAction);

    QAction* colorAction = contextMenu->addAction(QString("Change Color..."));
    QObject::connect(colorAction, &QAction::triggered, [=](){
        auto initialColor = this->getColor();
        auto resultColor = QColorDialog::getColor(QColor(255.f*initialColor.r, 255.f*initialColor.g, 255.f*initialColor.b, 255.f*initialColor.a), nullptr, QString(), QColorDialog::ShowAlphaChannel);
        if(resultColor.isValid()){
            this->setColor(Color(resultColor.red() / 255.f, resultColor.green() / 255.f, resultColor.blue() / 255.f, resultColor.alpha() / 255.f));
        }
    });

    contextMenu->addAction(colorAction);

    return contextMenu;
}
