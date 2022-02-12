//
// Created by Jonas on 8/11/2021.
//

#include <glm/gtc/type_ptr.hpp>
#include "RenderAABB.h"
#include "Exception.h"

void RenderAABB::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) {

    if(this->visible) {

        // Bind required buffers and shaders
        this->initializeOpenGLFunctions();
        this->vertexArray->bind();
        this->indexBuffer->bind();
        this->shader->bind();

        // Set MVP matrix uniform
        const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->transformationMatrix;
        this->shader->setUniformValue("u_ModelViewProjectionMatrix", QMatrix4x4(glm::value_ptr(modelViewProjectionMatrix)).transposed());

        // Set color uniform
        QVector4D drawColor;
        drawColor = QVector4D(this->color.r, this->color.g, this->color.b, this->color.a);
        if(lightMode){
            if(glm::vec3(this->color) == glm::vec3(1,1,1)){
                drawColor = QVector4D(0, 0, 0, this->color.a);
            }
            else if(glm::vec3(this->color) == glm::vec3(0,0,0)){
                drawColor = QVector4D(1, 1, 1, this->color.a);
            }
        }
        this->shader->setUniformValue("u_Color", drawColor);


        GL_CALL(glDrawElements(GL_LINES, this->indexBuffer->size()/sizeof(unsigned int), GL_UNSIGNED_INT, nullptr));
    }
}

RenderAABB::RenderAABB(const AABB &aabb, const glm::mat4& transformationMatrix, const std::shared_ptr<QOpenGLShaderProgram>& shader):
    AbstractRenderModel(transformationMatrix),
    shader(shader){
    std::vector<unsigned int> indices;
    std::vector<Vertex> vertices;
    std::vector<float> vertexData;

    Vertex min = aabb.getMinimum();
    Vertex max = aabb.getMaximum();

    vertices.emplace_back(min.x, min.y, min.z);
    vertices.emplace_back(min.x, min.y, max.z);
    vertices.emplace_back(min.x, max.y, min.z);
    vertices.emplace_back(min.x, max.y, max.z);
    vertices.emplace_back(max.x, min.y, min.z);
    vertices.emplace_back(max.x, min.y, max.z);
    vertices.emplace_back(max.x, max.y, min.z);
    vertices.emplace_back(max.x, max.y, max.z);

    for (const auto &vertex : vertices){
        vertexData.emplace_back(vertex.x);
        vertexData.emplace_back(vertex.y);
        vertexData.emplace_back(vertex.z);
    }

    indices.emplace_back(0);
    indices.emplace_back(1);

    indices.emplace_back(0);
    indices.emplace_back(2);

    indices.emplace_back(0);
    indices.emplace_back(4);

    indices.emplace_back(1);
    indices.emplace_back(3);

    indices.emplace_back(1);
    indices.emplace_back(5);

    indices.emplace_back(2);
    indices.emplace_back(3);

    indices.emplace_back(2);
    indices.emplace_back(6);

    indices.emplace_back(3);
    indices.emplace_back(7);

    indices.emplace_back(4);
    indices.emplace_back(5);

    indices.emplace_back(4);
    indices.emplace_back(6);

    indices.emplace_back(5);
    indices.emplace_back(7);

    indices.emplace_back(6);
    indices.emplace_back(7);

    this->vertexBuffer->create();
    this->vertexBuffer->bind();
    this->vertexBuffer->allocate(&vertexData.front(), vertexData.size() * sizeof(float));

    this->vertexArray->create();
    this->vertexArray->bind();

    this->initializeOpenGLFunctions();

    GL_CALL(glEnableVertexAttribArray(0));

    this->indexBuffer->create();
    this->indexBuffer->bind();
    this->indexBuffer->allocate(&indices.front(), indices.size() * sizeof(unsigned int));

    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr));

    this->color = Color(1);
}

RenderAABB::RenderAABB(const WorldSpaceMesh &worldSpaceMesh, const std::shared_ptr<QOpenGLShaderProgram>& shader):
RenderAABB(AABB(worldSpaceMesh.getModelSpaceMesh()->getVertices()), worldSpaceMesh.getModelTransformation().getMatrix(), shader) {}


