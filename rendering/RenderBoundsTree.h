//
// Created by Jonas on 26/08/2022.
//

#ifndef MESHCORE_ABSTRACTRENDERBOUNDSTREE_H
#define MESHCORE_ABSTRACTRENDERBOUNDSTREE_H

#include <QOpenGLShaderProgram>
#include "AbstractRenderModel.h"
#include "../acceleration/AbstractBoundsTree.h"
#include "RenderAABB.h"
#include "RenderOBB.h"
#include "RenderSphere.h"
#include "../acceleration/AABBTree.h"
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include <QGridLayout>
#include <thread>

class RenderBoundsTree: public AbstractRenderModel{
private:
    unsigned int renderDepth = 0;
    std::atomic<bool> animating = false; // For cycling through the render depth
    std::vector<std::shared_ptr<RenderBoundsTree>> children;
    std::shared_ptr<AbstractRenderModel> renderNodeModel;

public:
    template <unsigned int Degree>
    RenderBoundsTree(const AABBTree<Degree>& aabbTree, const Transformation& transformationMatrix, const std::shared_ptr<QOpenGLShaderProgram>& shader);

    template <unsigned int Degree, bool UniqueAssignment>
    RenderBoundsTree(const AbstractBoundsTree<AABB, Degree, UniqueAssignment>& aabbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& ambientShader, const std::shared_ptr<QOpenGLShaderProgram>& diffuseShader);

    template <unsigned int Degree, bool UniqueAssignment>
    RenderBoundsTree(const AbstractBoundsTree<OBB, Degree, UniqueAssignment>& obbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& ambientShader, const std::shared_ptr<QOpenGLShaderProgram>& diffuseShader);

    template <unsigned int Degree, bool UniqueAssignment>
    RenderBoundsTree(const AbstractBoundsTree<Sphere, Degree, UniqueAssignment>& sphereTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& ambientShader, const std::shared_ptr<QOpenGLShaderProgram>& diffuseShader);

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) override;
    void setTransformation(const Transformation &transformation) override;
    void setRenderDepth(unsigned int newRenderDepth);
    unsigned int getMaximumDepth();
    void setColor(const Color &newColor) override;

    RenderModelDetailDialog *createRenderModelDetailDialog(QWidget* parent) override;

    QMenu *getContextMenu() override;

    unsigned int getRenderDepth() const;

private:
    void drawRecursive(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode, unsigned int depth);
};

// Only define templated functions in this header file:

template<unsigned int Degree>
RenderBoundsTree::RenderBoundsTree(const AABBTree<Degree> &aabbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader): AbstractRenderModel(transformation, "aabbTree"), renderNodeModel(std::make_shared<RenderAABB>(aabbTree.getBounds(), transformation, shader)) {
    if(aabbTree.isSplit()){
        for (const auto &child : aabbTree.getChildren()){
            if(child && !child->isEmpty()) this->children.emplace_back(std::make_shared<RenderBoundsTree>(*child, transformation, shader));
        }
    }
}

template <unsigned int Degree, bool UniqueAssignment>
RenderBoundsTree::RenderBoundsTree(const AbstractBoundsTree<AABB, Degree, UniqueAssignment> &aabbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& ambientShader, const std::shared_ptr<QOpenGLShaderProgram>& diffuseShader): AbstractRenderModel(transformation, "aabbTree"), renderNodeModel(std::make_shared<RenderAABB>(aabbTree.getBounds(), transformation, ambientShader)) {
    if(aabbTree.isSplit()){
        for (const auto &child : aabbTree.getChildren()){
            if(child && !child->isEmpty()) this->children.emplace_back(std::make_shared<RenderBoundsTree>(*child, transformation, ambientShader, diffuseShader));
        }
    }
}

template <unsigned int Degree, bool UniqueAssignment>
RenderBoundsTree::RenderBoundsTree(const AbstractBoundsTree<OBB, Degree, UniqueAssignment> &obbTree, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& ambientShader, const std::shared_ptr<QOpenGLShaderProgram>& diffuseShader): AbstractRenderModel(transformation, "obbTree"), renderNodeModel(std::make_shared<RenderOBB>(obbTree.getBounds(), transformation, ambientShader)){
    if(obbTree.isSplit()){
        for (const auto &child : obbTree.getChildren()){
            if(child && !child->isEmpty()) this->children.emplace_back(std::make_shared<RenderBoundsTree>(*child, transformation, ambientShader, diffuseShader));
        }
    }
}

template <unsigned int Degree, bool UniqueAssignment>
RenderBoundsTree::RenderBoundsTree(const AbstractBoundsTree<Sphere, Degree, UniqueAssignment> &sphereTree, const Transformation &transformation, const std::shared_ptr<QOpenGLShaderProgram> &ambientShader, const std::shared_ptr<QOpenGLShaderProgram> &diffuseShader): AbstractRenderModel(transformation, "sphereTree"), renderNodeModel(std::make_shared<RenderSphere>(sphereTree.getBounds(), transformation, ambientShader, diffuseShader)){
    this->setColor(Color(1.0, 1.0, 1.0, 0.1));
    if(sphereTree.isSplit()){
        for (const auto &child : sphereTree.getChildren()){
            if(child && !child->isEmpty()) this->children.emplace_back(std::make_shared<RenderBoundsTree>(*child, transformation, ambientShader, diffuseShader));
        }
    }
}

#endif //MESHCORE_ABSTRACTRENDERBOUNDSTREE_H