//
// Created by Jonas on 8/11/2021.
//

#ifndef OPTIXMESHCORE_RENDERAABBTREE_H
#define OPTIXMESHCORE_RENDERAABBTREE_H


#include <QOpenGLShaderProgram>
#include "AbstractRenderModel.h"
#include "../../core/AABBTree.h"
#include "RenderAABB.h"

template <unsigned int Degree>
class RenderAABBTree: public AbstractRenderModel {
private:
    unsigned int renderDepth = 0;
    std::shared_ptr<QOpenGLShaderProgram> shader;
    std::array<std::shared_ptr<RenderAABBTree<Degree>>, Degree> children;
    RenderAABB renderAABB;

public:
    RenderAABBTree(const AABBTree<Degree>& aabbTree, const glm::mat4& transformationMatrix, const std::shared_ptr<QOpenGLShaderProgram>& shader);
    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) override;
    void setTransformationMatrix(const glm::mat4 &transformation) override;
    void setRenderDepth(unsigned int newRenderDepth);
    unsigned int getDepth();

private:
    void drawRecursive(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode, unsigned int depth);
};

template<unsigned int Degree>
void RenderAABBTree<Degree>::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) {
    this->drawRecursive(viewMatrix, projectionMatrix, lightMode, renderDepth);
}

template<unsigned int Degree>
void RenderAABBTree<Degree>::drawRecursive(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode, unsigned int depth) {
    if(depth > 0){
        for (const auto &child : this->children){
            if(child!=nullptr) child->drawRecursive(viewMatrix, projectionMatrix, lightMode, depth-1);
        }
    }
    else if(depth == 0){
        renderAABB.draw(viewMatrix, projectionMatrix, lightMode);
    }
}

template<unsigned int Degree>
RenderAABBTree<Degree>::RenderAABBTree(const AABBTree<Degree> &aabbTree, const glm::mat4& transformationMatrix, const std::shared_ptr<QOpenGLShaderProgram>& shader): AbstractRenderModel(transformationMatrix),
        renderAABB(aabbTree.getBounds(), transformationMatrix, shader) {
        if(aabbTree.isSplit()){
            for(int childIndex=0; childIndex<Degree; childIndex++){
                auto child = aabbTree.getChildren()[childIndex];
                if(!child->isEmpty()) children[childIndex] = std::make_shared<RenderAABBTree<Degree>>(*child, transformationMatrix, shader);
            }
        }
}

template<unsigned int Degree>
void RenderAABBTree<Degree>::setTransformationMatrix(const glm::mat4 &transformation) {
    AbstractRenderModel::setTransformationMatrix(transformation);
    renderAABB.setTransformationMatrix(transformation);
    for (const auto &child : this->children){
        if(child!=nullptr) child->setTransformationMatrix(transformation);
    }
}

template<unsigned int Degree>
void RenderAABBTree<Degree>::setRenderDepth(unsigned int newRenderDepth) {
    RenderAABBTree::renderDepth = newRenderDepth;
}

template<unsigned int Degree>
unsigned int RenderAABBTree<Degree>::getDepth() {
    unsigned int depth = 0;
    for (const auto &child : this->children){
        if(child!=nullptr) depth = std::max(depth, child->getDepth() + 1);
    }
    return depth;
}


#endif //OPTIXMESHCORE_RENDERAABBTREE_H
