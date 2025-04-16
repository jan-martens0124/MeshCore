//
// Created by Jonas on 24/08/2022.
//

#include <glm/gtc/type_ptr.hpp>
#include "RenderLine.h"
#include "Exception.h"
#include "meshcore/rendering/OpenGLWidget.h"

RenderLine::RenderLine(const Vertex &startPoint, const Vertex &endPoint, const Transformation &transformation): AbstractRenderModel(transformation, "AABB"){
    std::vector<glm::vec3> data;
    data.push_back(startPoint);
    data.push_back(endPoint);

    std::vector<unsigned int> indices;
    indices.push_back(0);
    indices.push_back(1);

    this->vertexBuffer->create();
    this->vertexBuffer->bind();
    this->vertexBuffer->allocate(&data.front(), (int) (data.size() * sizeof(glm::vec3)));

    this->vertexArray->create();
    this->vertexArray->bind();

    this->initializeOpenGLFunctions();
    GL_CALL(glEnableVertexAttribArray(0));

    this->indexBuffer->create();
    this->indexBuffer->bind();
    this->indexBuffer->allocate(&indices.front(), indices.size() * sizeof(unsigned int));

    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr));
}

void RenderLine::draw(const OpenGLWidget* openGLWidget, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) {
    if(this->isVisible()){

        // Bind required buffers and shaders
        this->initializeOpenGLFunctions();
        this->vertexArray->bind();
        this->indexBuffer->bind();

        auto& ambientShader = openGLWidget->getAmbientShader();
        ambientShader->bind();

        // Set MVP matrix uniform
        const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->getTransformationMatrix();
        ambientShader->setUniformValue("u_ModelViewProjectionMatrix", QMatrix4x4(glm::value_ptr(modelViewProjectionMatrix)).transposed());

        // Set color uniform
        QVector4D drawColor;
        const auto color = this->getMaterial().getDiffuseColor();
        drawColor = QVector4D(color.r, color.g, color.b, color.a);
        if(lightMode){
            if(glm::vec3(color) == glm::vec3(1,1,1)){
                drawColor = QVector4D(0, 0, 0, color.a);
            }
            else if(glm::vec3(color) == glm::vec3(0,0,0)){
                drawColor = QVector4D(1, 1, 1, color.a);
            }
        }
        ambientShader->setUniformValue("u_Color", drawColor);


        GL_CALL(glDrawElements(GL_LINES, this->indexBuffer->size()/sizeof(unsigned int), GL_UNSIGNED_INT, nullptr));
    }
}
