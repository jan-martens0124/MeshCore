//
// Created by Jonas on 8/11/2021.
//

#ifndef OPTIXMESHCORE_RENDERAABBTREE_H
#define OPTIXMESHCORE_RENDERAABBTREE_H


#include <QOpenGLShaderProgram>
#include "AbstractRenderModel.h"
#include "../../core/AABBTree.h"
#include "RenderAABB.h"

class RenderAABBTree: public AbstractRenderModel {
private:
    unsigned int renderDepth = 0;
    std::shared_ptr<QOpenGLShaderProgram> shader;
    std::vector<std::shared_ptr<RenderAABBTree>> children;
    RenderAABB renderAABB;

public:
    template <unsigned int Degree>
    RenderAABBTree(const AABBTree<Degree>& aabbTree, const glm::mat4& transformationMatrix, const std::shared_ptr<QOpenGLShaderProgram>& shader);

    template <unsigned int Degree>
    RenderAABBTree(const OBBTree<Degree>& obbTree, const glm::mat4& transformationMatrix, const std::shared_ptr<QOpenGLShaderProgram>& shader);

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) override;
    void setTransformationMatrix(const glm::mat4 &transformation) override;
    void setRenderDepth(unsigned int newRenderDepth);
    unsigned int getDepth();

private:
    void drawRecursive(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode, unsigned int depth);
};

void RenderAABBTree::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) {
    this->drawRecursive(viewMatrix, projectionMatrix, lightMode, renderDepth);
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
RenderAABBTree::RenderAABBTree(const AABBTree<Degree> &aabbTree, const glm::mat4& transformationMatrix, const std::shared_ptr<QOpenGLShaderProgram>& shader): AbstractRenderModel(transformationMatrix),
        renderAABB(aabbTree.getBounds(), transformationMatrix, shader) {
        if(aabbTree.isSplit()){
            for (const auto &child : aabbTree.getChildren()){
                if(!child->isEmpty()) this->children.emplace_back(std::make_shared<RenderAABBTree>(*child, transformationMatrix, shader));
            }
        }
}

template <unsigned int Degree>
RenderAABBTree::RenderAABBTree(const OBBTree<Degree> &obbTree, const glm::mat4& transformationMatrix, const std::shared_ptr<QOpenGLShaderProgram>& shader) {
    this->transformationMatrix = transformationMatrix * obbTree.getBounds().getTransformation();
    this->renderAABB = obbTree.getBounds();
    if(obbTree.isSplit()){
        for (const auto &child : obbTree.getChildren()){
            if(!child->isEmpty()) this->children.emplace_back(std::make_shared<RenderAABBTree>(*child, transformationMatrix, shader));
        }
    }
}

void RenderAABBTree::setTransformationMatrix(const glm::mat4 &transformation) {
    AbstractRenderModel::setTransformationMatrix(transformation);
    renderAABB.setTransformationMatrix(transformation);
    for (const auto &child : this->children){
        child->setTransformationMatrix(transformation);
    }
}

void RenderAABBTree::setRenderDepth(unsigned int newRenderDepth) {
    RenderAABBTree::renderDepth = newRenderDepth;
}

unsigned int RenderAABBTree::getDepth() {
    unsigned int depth = 0;
    for (const auto &child : this->children){
        depth = std::max(depth, child->getDepth() + 1);
    }
    return depth;
}


#endif //OPTIXMESHCORE_RENDERAABBTREE_H
