//
// Created by Jonas on 10/03/2021.
//

#ifndef MESHCORE_RENDERLINE_H
#define MESHCORE_RENDERLINE_H


#include <QOpenGLBuffer>
#include "RenderModel.h"

class RenderLine: protected QOpenGLFunctions {
private:
    QOpenGLBuffer* indexBuffer;
    QOpenGLBuffer* vertexBuffer;
    QOpenGLVertexArrayObject* vertexArray;
public:
    [[nodiscard]] const Color &getColor() const;

private:
    Color color;
    glm::mat4 transformation;
public:
    void setTransformationMatrix(const glm::mat4 &transformationMatrix);
    void setColor(const Color& c);

    RenderLine();
    RenderLine(RenderLine&& other) noexcept;
    RenderLine& operator=(RenderLine&& other) noexcept;
    ~RenderLine();

    explicit RenderLine(Vertex vertex1, Vertex vertex2, glm::mat4 transformationMatrix);
    void draw(QOpenGLShaderProgram& shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
};


#endif //MESHCORE_RENDERLINE_H
