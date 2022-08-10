//
// Created by Jonas on 8/11/2021.
//

#ifndef OPTIXMESHCORE_RENDERAABBTREE_H
#define OPTIXMESHCORE_RENDERAABBTREE_H


#include <QOpenGLShaderProgram>
#include "AbstractRenderModel.h"
#include "../meshcore/acceleration/AABBTree.h"
#include "../../core/OBBTree.h"
#include "RenderAABB.h"

class RenderAABBTree: public AbstractRenderModel {
private:
    unsigned int renderDepth = 0;
    std::shared_ptr<QOpenGLShaderProgram> shader;
    std::vector<std::shared_ptr<RenderAABBTree>> children;
    RenderAABB renderAABB;

public:
    template <unsigned int Degree>
    RenderAABBTree(const AABBTree<Degree>& aabbTree, const Transformation& transformationMatrix, const std::shared_ptr<QOpenGLShaderProgram>& shader);

    template <unsigned int Degree>
    RenderAABBTree(const OBBTree<Degree>& obbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader);

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) override;
    void setTransformation(const Transformation &transformation) override;
    void setRenderDepth(unsigned int newRenderDepth);
    unsigned int getDepth();
    void setColor(const Color &newColor) override;

private:
public:
    RenderModelDetailDialog *createRenderModelDetailDialog(QWidget* parent) override;

    QMenu *getContextMenu() override;

private:
    void drawRecursive(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode, unsigned int depth);
};

void RenderAABBTree::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) {
    if(this->isVisible()) this->drawRecursive(viewMatrix, projectionMatrix, lightMode, renderDepth);
}

void RenderAABBTree::drawRecursive(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode, unsigned int depth) {
    if(depth > 0){
        for (const auto &child : this->children){
            child->drawRecursive(viewMatrix, projectionMatrix, lightMode, depth-1);
        }
    }
    else if(depth == 0){
        renderAABB.draw(viewMatrix, projectionMatrix, lightMode);
    }
}

template <unsigned int Degree>
RenderAABBTree::RenderAABBTree(const AABBTree<Degree> &aabbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader): AbstractRenderModel(transformation, "aabbTree"),
        renderAABB(aabbTree.getBounds(), transformation, shader) {
        if(aabbTree.isSplit()){
            for (const auto &child : aabbTree.getChildren()){
                if(!child->isEmpty()) this->children.emplace_back(std::make_shared<RenderAABBTree>(*child, transformation, shader));
            }
        }
}

template <unsigned int Degree>
RenderAABBTree::RenderAABBTree(const OBBTree<Degree> &obbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader): AbstractRenderModel(Transformation::fromQuaternion(obbTree.getBounds().getRotation()), "obbTree"), renderAABB(obbTree.getBounds().getAabb(), Transformation::fromQuaternion(obbTree.getBounds().getRotation()), shader){
    if(obbTree.isSplit()){
        for (const auto &child : obbTree.getChildren()){
            if(!child->isEmpty()) this->children.emplace_back(std::make_shared<RenderAABBTree>(*child, transformation, shader));
        }
    }
}

void RenderAABBTree::setTransformation(const Transformation &transformation) {
    AbstractRenderModel::setTransformation(transformation);
    renderAABB.setTransformation(transformation);
    for (const auto &child : this->children){
        child->setTransformation(transformation); // TODO probably wrong for OBB, yes!
    }
}

void RenderAABBTree::setRenderDepth(unsigned int newRenderDepth) {
    RenderAABBTree::renderDepth = newRenderDepth;
    for (const auto &listener: this->listeners){
        listener->notify();
    }
}

unsigned int RenderAABBTree::getDepth() {
    unsigned int depth = 0;
    for (const auto &child : this->children){
        depth = std::max(depth, child->getDepth() + 1);
    }
    return depth;
}

void RenderAABBTree::setColor(const Color &newColor) {
    AbstractRenderModel::setColor(newColor);
    renderAABB.setColor(newColor);
    for (const auto &child : this->children){
        child->setColor(newColor);
    }
}

RenderModelDetailDialog *RenderAABBTree::createRenderModelDetailDialog(QWidget* parent) {

    auto dialog = AbstractRenderModel::createRenderModelDetailDialog(parent);

    auto listener = std::make_shared<SimpleRenderModelListener>();
    this->addListener(listener);

    auto* optionsLayout = new QGridLayout();

    auto* slider = new QSlider(Qt::Horizontal);
    slider->setMinimum(0);
    slider->setValue(this->renderDepth);
    slider->setMaximum(this->getDepth());
    optionsLayout->addWidget(new QLabel("Rendering Depth"), 0, 0);
    optionsLayout->addWidget(slider, 1, 0);
    QObject::connect(slider, &QSlider::valueChanged, [=](){
        this->setRenderDepth(slider->value());
    });
    listener->setOnChanged([=](){
        slider->setValue(renderDepth);
    });

    auto visibleCheckBox = new QCheckBox(QString("Visible"));
    visibleCheckBox->setChecked(this->isVisible());
    listener->setOnVisibleChanged([=](bool oldVisible, bool newVisible) {
        visibleCheckBox->setChecked(newVisible);
    });
    QObject::connect(visibleCheckBox, &QCheckBox::clicked, [&](bool enabled) {
        this->setVisible(enabled);
    });
    optionsLayout->addWidget(visibleCheckBox, 2, 0);

    auto* optionsWidget = new QWidget();
    optionsWidget->setLayout(optionsLayout);
    dialog->addTab(optionsWidget, QString("Options"));

    // TODO think about render animation on seperate thread

    return dialog;
}

QMenu *RenderAABBTree::getContextMenu() {
    return AbstractRenderModel::getContextMenu();

    // TODO, if necessary
}


#endif //OPTIXMESHCORE_RENDERAABBTREE_H
