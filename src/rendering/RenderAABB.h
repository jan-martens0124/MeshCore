//
// Created by Jonas on 8/11/2021.
//

#ifndef OPTIXMESHCORE_RENDERAABB_H
#define OPTIXMESHCORE_RENDERAABB_H

#include <qt5/QtGui/QOpenGLShaderProgram>
#include "AbstractRenderModel.h"
#include "meshcore/core/AABB.h"
#include "meshcore/core/WorldSpaceMesh.h"

class RenderAABB: public AbstractRenderModel {
private:
    AABB aabb;
public:

    RenderAABB(const AABB &aabb, const Transformation& transformation);
    RenderAABB(const WorldSpaceMesh& worldSpaceMesh);

    RenderModelDetailDialog *createRenderModelDetailDialog(QWidget* parent) override;

    void draw(const OpenGLWidget* openGLWidget, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) override;
};


#endif //OPTIXMESHCORE_RENDERAABB_H
