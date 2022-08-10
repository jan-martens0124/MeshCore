//
// Created by Jonas on 8/11/2021.
//

#include "AbstractRenderModel.h"
#include <iostream>

bool AbstractRenderModel::isVisible() const {
    return visible;
}

void AbstractRenderModel::setVisible(bool newVisible) {
    const auto oldValue = this->visible;
    this->visible = newVisible;
    for (const auto &listener: this->listeners){
        listener->notifyVisibleChanged(oldValue, this->visible);
    }
}

const Color &AbstractRenderModel::getColor() const {
    return color;
}

void AbstractRenderModel::setColor(const Color &newColor) {
    const auto oldValue = this->color;
    this->color = newColor;
    for (const auto &listener: this->listeners){
        listener->notifyColorChanged(oldValue, this->color);
    }
}

glm::mat4 AbstractRenderModel::getTransformationMatrix() const {
    return transformation.getMatrix();
}

AbstractRenderModel::AbstractRenderModel(const Transformation& transformation, const std::string& name):
        color(Color(1)),
        vertexBuffer(new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer)),
        indexBuffer(new QOpenGLBuffer(QOpenGLBuffer::Type::IndexBuffer)),
        vertexArray(new QOpenGLVertexArrayObject()),
        transformation(transformation)
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

    // We don't have to do this anymore, as the dialog will be removed by its parent (the RenderModelControlWidget)
//    if(this->detailDialog != nullptr){
//        this->detailDialog->close();
//        delete this->detailDialog;
//    }
}

AbstractRenderModel::AbstractRenderModel(AbstractRenderModel &&other) noexcept:
        color(other.color),
        transformation(other.transformation)
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
        this->transformation = other.transformation;
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
    const auto oldValue = this->name;
    this->name = newName;
    for (const auto &listener: this->listeners){
        listener->notifyNameChanged(oldValue, this->name);
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

RenderModelDetailDialog *AbstractRenderModel::getDetailsDialog(QWidget* parent){

    // Show existing dialog if already exists
    if(this->detailDialog==nullptr){
        this->detailDialog = this->createRenderModelDetailDialog(parent);
    }
    return this->detailDialog;
}

RenderModelDetailDialog *AbstractRenderModel::createRenderModelDetailDialog(QWidget* parent) {
    return new RenderModelDetailDialog(this, parent);
}

void AbstractRenderModel::setTransformation(const Transformation &transformation) {
    const auto oldValue = this->transformation;
    AbstractRenderModel::transformation = transformation;
    for (const auto &listener: this->listeners){
        listener->notifyTransformationChanged(oldValue, this->transformation);
    }
}

const Transformation &AbstractRenderModel::getTransformation() const {
    return this->transformation;
};

