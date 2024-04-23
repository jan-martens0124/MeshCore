//
// Created by Jonas on 24/08/2022.
//

#ifndef OPTIXMESHCORE_RENDERLINE_H
#define OPTIXMESHCORE_RENDERLINE_H


#include <QOpenGLShaderProgram>
#include "AbstractRenderModel.h"

class RenderLine: public AbstractRenderModel  {
public:
    RenderLine(const Vertex &vertexA, const Vertex& vertexB, const Transformation& transformation);

    void draw(const OpenGLWidget* openGLWidget, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) override;
//    RenderModelDetailDialog *createRenderModelDetailDialog(QWidget* parent) override;
//    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) override;
};


#endif //OPTIXMESHCORE_RENDERLINE_H
