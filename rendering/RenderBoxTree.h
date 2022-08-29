//
// Created by Jonas on 8/11/2021.
//

#ifndef OPTIXMESHCORE_RENDERBOXTREE_H
#define OPTIXMESHCORE_RENDERBOXTREE_H


#include <QOpenGLShaderProgram>
#include "AbstractRenderModel.h"
#include "../meshcore/acceleration/AABBTree.h"
#include "RenderAABB.h"
#include "../core/AABB.h"
#include "../core/OBB.h"
#include "../acceleration/AbstractBoundsTree.h"
#include "RenderBoundsTree.h"

class RenderBoxTree: public AbstractRenderModel {
private:
    unsigned int renderDepth = 0;
    std::shared_ptr<QOpenGLShaderProgram> shader;
    std::vector<std::shared_ptr<RenderBoxTree>> children;
    RenderAABB renderAABB;

public:
    template <unsigned int Degree>
    RenderBoxTree(const AABBTree<Degree>& aabbTree, const Transformation& transformationMatrix, const std::shared_ptr<QOpenGLShaderProgram>& shader);

    template <unsigned int Degree>
    RenderBoxTree(const AbstractBoundsTree<AABB, Degree>& obbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader);

    template <unsigned int Degree>
    RenderBoxTree(const AbstractBoundsTree<OBB, Degree>& obbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader);

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) override;
    void setTransformation(const Transformation &transformation) override;
    void setRenderDepth(unsigned int newRenderDepth);
    unsigned int getDepth();
    void setColor(const Color &newColor) override;

private:
public:
    RenderModelDetailDialog *createRenderModelDetailDialog(QWidget* parent) override;

    QMenu *getContextMenu() override;

    unsigned int getRenderDepth() const;

private:
    void drawRecursive(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode, unsigned int depth);
};

void RenderBoxTree::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) {
    if(this->isVisible()) this->drawRecursive(viewMatrix, projectionMatrix, lightMode, renderDepth);
}

void RenderBoxTree::drawRecursive(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode, unsigned int depth) {
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
RenderBoxTree::RenderBoxTree(const AABBTree<Degree> &aabbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader): AbstractRenderModel(transformation, "aabbTree"),
                                                                                                                                                           renderAABB(aabbTree.getBounds(), transformation, shader) {
        if(aabbTree.isSplit()){
            for (const auto &child : aabbTree.getChildren()){
                if(!child->isEmpty()) this->children.emplace_back(std::make_shared<RenderBoxTree>(*child, transformation, shader));
            }
        }
}

template <unsigned int Degree>
RenderBoxTree::RenderBoxTree(const AbstractBoundsTree<AABB, Degree> &aabbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader): AbstractRenderModel(transformation, "aabbTree"),
                                                                                                                                                           renderAABB(aabbTree.getBounds(), transformation, shader) {
    if(aabbTree.isSplit()){
        for (const auto &child : aabbTree.getChildren()){
            if(!child->isEmpty()) this->children.emplace_back(std::make_shared<RenderBoxTree>(*child, transformation, shader));
        }
    }
}

template <unsigned int Degree>
RenderBoxTree::RenderBoxTree(const AbstractBoundsTree<OBB, Degree> &obbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader): AbstractRenderModel(Transformation::fromQuaternion(obbTree.getBounds().getRotation()), "obbTree"), renderAABB(obbTree.getBounds().getAabb(), Transformation::fromQuaternion(obbTree.getBounds().getRotation()), shader){
    if(obbTree.isSplit()){
        for (const auto &child : obbTree.getChildren()){
            if(!child->isEmpty()) this->children.emplace_back(std::make_shared<RenderBoxTree>(*child, transformation, shader));
        }
    }
}

void RenderBoxTree::setTransformation(const Transformation &transformation) {
    AbstractRenderModel::setTransformation(transformation);
    renderAABB.setTransformation(transformation);
    for (const auto &child : this->children){
        child->setTransformation(transformation); // TODO probably wrong for OBB, yes!
    }
}

void RenderBoxTree::setRenderDepth(unsigned int newRenderDepth) {
    RenderBoxTree::renderDepth = newRenderDepth;
    for (const auto &listener: this->listeners){
        listener->notify();
    }
}

unsigned int RenderBoxTree::getRenderDepth() const {
    return renderDepth;
}

unsigned int RenderBoxTree::getDepth() {
    unsigned int depth = 0;
    for (const auto &child : this->children){
        depth = std::max(depth, child->getDepth() + 1);
    }
    return depth;
}

void RenderBoxTree::setColor(const Color &newColor) {
    AbstractRenderModel::setColor(newColor);
    renderAABB.setColor(newColor);
    for (const auto &child : this->children){
        child->setColor(newColor);
    }
}

RenderModelDetailDialog *RenderBoxTree::createRenderModelDetailDialog(QWidget* parent) {

    auto dialog = AbstractRenderModel::createRenderModelDetailDialog(parent);

    auto listener = std::make_shared<SimpleRenderModelListener>();
    this->addListener(listener);

    auto* optionsLayout = new QGridLayout();

    auto visibleCheckBox = new QCheckBox(QString("Visible"));
    visibleCheckBox->setChecked(this->isVisible());
    listener->setOnVisibleChanged([=](bool oldVisible, bool newVisible) {
        visibleCheckBox->setChecked(newVisible);
    });
    QObject::connect(visibleCheckBox, &QCheckBox::clicked, [&](bool enabled) {
        this->setVisible(enabled);
    });
    optionsLayout->addWidget(visibleCheckBox, 0, 0);

    // TODO this isn't properly implemented yet
    auto animateRenderDepth = new QCheckBox(QString("Animate render depth"));
    animateRenderDepth->setChecked(false);
    std::atomic<bool> animate = false;
    QObject::connect(animateRenderDepth, &QCheckBox::clicked, [&](bool enabled) {
        animate = enabled;
        // Start a thread that increases the render depth every second until animate is false
        if(enabled){
            std::thread([&]() {
                while(animate){
                    this->setRenderDepth((this->getRenderDepth()+1)%this->getDepth());
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
            }).detach();
        }
        // TODO an now we somehow have to clean this thread up :)
    });
    optionsLayout->addWidget(animateRenderDepth, 1, 0);

    auto* slider = new QSlider(Qt::Horizontal);
    slider->setMinimum(0);
    slider->setValue(this->renderDepth);
    slider->setMaximum(this->getDepth());
    optionsLayout->addWidget(new QLabel("Rendering Depth"), 2, 0);
    optionsLayout->addWidget(slider, 3, 0);
    QObject::connect(slider, &QSlider::valueChanged, [=](){
        this->setRenderDepth(slider->value());
    });
    listener->setOnChanged([=](){
        slider->setValue(renderDepth);
    });


    auto* optionsWidget = new QWidget();
    optionsWidget->setLayout(optionsLayout);
    dialog->addTab(optionsWidget, QString("Options"));

    // TODO think about render animation on seperate thread

    return dialog;
}

QMenu *RenderBoxTree::getContextMenu() {
    return AbstractRenderModel::getContextMenu();

    // TODO, if necessary
}


#endif //OPTIXMESHCORE_RENDERBOXTREE_H
