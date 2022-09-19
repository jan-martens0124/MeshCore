//
// Created by Jonas on 26/08/2022.
//

#ifndef MESHCORE_ABSTRACTRENDERBOUNDSTREE_H
#define MESHCORE_ABSTRACTRENDERBOUNDSTREE_H

#include <QOpenGLShaderProgram>
#include "AbstractRenderModel.h"
#include "../meshcore/core/AABB.h"
#include "RenderOBB.h"
#include "RenderSphere.h"


class RenderBoundsTree: public AbstractRenderModel{
private:
    unsigned int renderDepth = 0;
    std::atomic<bool> animating = false; // For cycling through the render depth
    std::vector<std::shared_ptr<RenderBoundsTree>> children;
    std::shared_ptr<AbstractRenderModel> renderNodeModel;

public:
    template <unsigned int Degree>
    RenderBoundsTree(const AABBTree<Degree>& aabbTree, const Transformation& transformationMatrix, const std::shared_ptr<QOpenGLShaderProgram>& shader);

    template <unsigned int Degree>
    RenderBoundsTree(const AbstractBoundsTree<AABB, Degree>& obbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader);

    template <unsigned int Degree>
    RenderBoundsTree(const AbstractBoundsTree<OBB, Degree>& obbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader);

    template <unsigned int Degree>
    RenderBoundsTree(const AbstractBoundsTree<Sphere, Degree>& sphereTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& ambientShader, const std::shared_ptr<QOpenGLShaderProgram>& diffuseShader);

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) override;
    void setTransformation(const Transformation &transformation) override;
    void setRenderDepth(unsigned int newRenderDepth);
    unsigned int getDepth();
    void setColor(const Color &newColor) override;

    RenderModelDetailDialog *createRenderModelDetailDialog(QWidget* parent) override;

    QMenu *getContextMenu() override;

    unsigned int getRenderDepth() const;

private:
    void drawRecursive(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode, unsigned int depth);
};

void RenderBoundsTree::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) {
    if(this->isVisible()) this->drawRecursive(viewMatrix, projectionMatrix, lightMode, renderDepth);
}

void RenderBoundsTree::drawRecursive(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode, unsigned int depth) {
    if(depth > 0){
        for (const auto &child : this->children){
            child->drawRecursive(viewMatrix, projectionMatrix, lightMode, depth-1);
        }
    }
    else if(depth == 0){
        renderNodeModel->draw(viewMatrix, projectionMatrix, lightMode);
    }
}

template<unsigned int Degree>
RenderBoundsTree::RenderBoundsTree(const AABBTree<Degree> &aabbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader): AbstractRenderModel(transformation, "aabbTree"), renderNodeModel(std::make_shared<RenderAABB>(aabbTree.getBounds(), transformation, shader)) {
    if(aabbTree.isSplit()){
        for (const auto &child : aabbTree.getChildren()){
            if(!child->isEmpty()) this->children.emplace_back(std::make_shared<RenderBoundsTree>(*child, transformation, shader));
        }
    }
}

template<unsigned int Degree>
RenderBoundsTree::RenderBoundsTree(const AbstractBoundsTree<AABB, Degree> &aabbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader): AbstractRenderModel(transformation, "aabbTree"), renderNodeModel(std::make_shared<RenderAABB>(aabbTree.getBounds(), transformation, shader)) {
    if(aabbTree.isSplit()){
        for (const auto &child : aabbTree.getChildren()){
            if(!child->isEmpty()) this->children.emplace_back(std::make_shared<RenderBoundsTree>(*child, transformation, shader));
        }
    }
}

template <unsigned int Degree>
RenderBoundsTree::RenderBoundsTree(const AbstractBoundsTree<OBB, Degree> &obbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader): AbstractRenderModel(transformation, "obbTree"), renderNodeModel(std::make_shared<RenderOBB>(obbTree.getBounds(), transformation, shader)){
    if(obbTree.isSplit()){
        for (const auto &child : obbTree.getChildren()){
            if(!child->isEmpty()) this->children.emplace_back(std::make_shared<RenderBoundsTree>(*child, transformation, shader));
        }
    }
}


template<unsigned int Degree>
RenderBoundsTree::RenderBoundsTree(const AbstractBoundsTree<Sphere, Degree> &sphereTree, const Transformation &transformation, const std::shared_ptr<QOpenGLShaderProgram> &ambientShader, const std::shared_ptr<QOpenGLShaderProgram> &diffuseShader): AbstractRenderModel(transformation, "sphereTree"), renderNodeModel(std::make_shared<RenderSphere>(sphereTree.getBounds(), transformation, ambientShader, diffuseShader)){
    this->setColor(Color(1.0, 1.0, 1.0, 0.1));
    if(sphereTree.isSplit()){
        for (const auto &child : sphereTree.getChildren()){
            if(!child->isEmpty()) this->children.emplace_back(std::make_shared<RenderBoundsTree>(*child, transformation, ambientShader, diffuseShader));
        }
    }
}

void RenderBoundsTree::setTransformation(const Transformation &transformation) {
    AbstractRenderModel::setTransformation(transformation);
    renderNodeModel->setTransformation(transformation);
    for (const auto &child : this->children){
        child->setTransformation(transformation);
    }
}

void RenderBoundsTree::setRenderDepth(unsigned int newRenderDepth) {
    RenderBoundsTree::renderDepth = newRenderDepth;
    for (const auto &listener: this->listeners){
        listener->notify();
    }
}

unsigned int RenderBoundsTree::getRenderDepth() const {
    return renderDepth;
}

unsigned int RenderBoundsTree::getDepth() {
    unsigned int depth = 0;
    for (const auto &child : this->children){
        depth = std::max(depth, child->getDepth() + 1);
    }
    return depth;
}

void RenderBoundsTree::setColor(const Color &newColor) {
    AbstractRenderModel::setColor(newColor);
    renderNodeModel->setColor(newColor);
    for (const auto &child : this->children){
        child->setColor(newColor);
    }
}

RenderModelDetailDialog *RenderBoundsTree::createRenderModelDetailDialog(QWidget* parent) {

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

    auto animateRenderDepth = new QCheckBox(QString("Animate render depth"));
    animateRenderDepth->setChecked(false);
    std::atomic<bool> animate = false;
    QObject::connect(animateRenderDepth, &QCheckBox::clicked, [&](bool enabled) {
        animating = enabled;
        // Start a thread that increases the render depth every second until animate is false
        if(enabled){
            std::thread([&]() {
                while(animating){
                    this->setRenderDepth((this->getRenderDepth()+1)%this->getDepth());
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
            }).detach();
        }
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

QMenu *RenderBoundsTree::getContextMenu() {
    return AbstractRenderModel::getContextMenu();

    // TODO, if necessary
}



#endif //MESHCORE_ABSTRACTRENDERBOUNDSTREE_H
