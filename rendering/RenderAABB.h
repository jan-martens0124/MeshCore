//
// Created by Jonas on 8/11/2021.
//

#ifndef OPTIXMESHCORE_RENDERAABB_H
#define OPTIXMESHCORE_RENDERAABB_H

#include <QOpenGLShaderProgram>
#include "AbstractRenderModel.h"
#include "../core/AABB.h"
#include "../core/WorldSpaceMesh.h"

class RenderAABB: public AbstractRenderModel {
private:
    float unscaledSurfaceArea;
    float unscaledVolume;
    std::shared_ptr<QOpenGLShaderProgram> ambientShader;
public:

    RenderAABB(const AABB &aabb, const Transformation& transformation, const std::shared_ptr<QOpenGLShaderProgram>& shader);
    RenderAABB(const WorldSpaceMesh& worldSpaceMesh, const std::shared_ptr<QOpenGLShaderProgram>& shader);

    RenderModelDetailDialog *createRenderModelDetailDialog(QWidget* parent) override;

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) override;
};


#endif //OPTIXMESHCORE_RENDERAABB_H
