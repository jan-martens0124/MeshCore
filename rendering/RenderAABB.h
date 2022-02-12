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
    std::shared_ptr<QOpenGLShaderProgram> shader;
public:

    RenderAABB(const AABB &aabb, const glm::mat4& transformationMatrix, const std::shared_ptr<QOpenGLShaderProgram>& shader);
    RenderAABB(const WorldSpaceMesh& worldSpaceMesh, const std::shared_ptr<QOpenGLShaderProgram>& shader);

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) override;
};


#endif //OPTIXMESHCORE_RENDERAABB_H
