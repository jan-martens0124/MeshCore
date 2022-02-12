//
// Created by Jonas on 10/03/2021.
//

#include <glm/gtc/type_ptr.hpp>
#include "RenderLine.h"
#include "OpenGLRenderWidget.h"

RenderLine::~RenderLine() {
    delete indexBuffer;
    delete vertexBuffer;
    delete vertexArray;
}

RenderLine::RenderLine():
        color(1,1,1,1),
        transformation(),
        vertexBuffer(nullptr),
        indexBuffer(nullptr),
        vertexArray(nullptr)
{
}

RenderLine &RenderLine::operator=(RenderLine &&other) noexcept {
    if(this != &other){
        this->indexBuffer = other.indexBuffer;
        this->vertexArray = other.vertexArray;
        this->vertexBuffer = other.vertexBuffer;
        this->color = other.color;
        this->transformation = other.transformation;

        other.indexBuffer = nullptr;
        other.vertexArray = nullptr;
        other.vertexBuffer = nullptr;
    }
    return *this;
}

RenderLine::RenderLine(RenderLine &&other) noexcept:
        color(other.color),
        transformation(other.transformation)
{
    this->indexBuffer = other.indexBuffer;
    this->vertexArray = other.vertexArray;
    this->vertexBuffer = other.vertexBuffer;

    other.indexBuffer = nullptr;
    other.vertexArray = nullptr;
    other.vertexBuffer = nullptr;
}

void RenderLine::setTransformationMatrix(const glm::mat4 &transformationMatrix) {
    RenderLine::transformation = transformationMatrix;
}

void RenderLine::setColor(const Color &c){
    RenderLine::color = c;
}

void RenderLine::draw(QOpenGLShaderProgram &shader, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {


    this->initializeOpenGLFunctions();
    this->vertexArray->bind();
    this->indexBuffer->bind();
    shader.bind();


    glLineWidth(50);

    glm::mat4 projectionViewMatrix = projectionMatrix * viewMatrix;
    const glm::mat4 modelViewProjectionMatrix = projectionViewMatrix * this->transformation;

    shader.setUniformValue("u_ModelViewProjectionMatrix", QMatrix4x4(glm::value_ptr(modelViewProjectionMatrix)).transposed());
    shader.setUniformValue("u_Color", QVector4D(this->color.r, this->color.g, this->color.b, this->color.a));
    glDrawElements(GL_LINES, this->indexBuffer->size()/sizeof(unsigned int),  GL_UNSIGNED_INT, nullptr);

}

RenderLine::RenderLine(Vertex vertex1, Vertex vertex2, glm::mat4 transformationMatrix):
        color(1,1,1,1),
        vertexBuffer(new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer)),
        indexBuffer(new QOpenGLBuffer(QOpenGLBuffer::Type::IndexBuffer)),
        vertexArray(new QOpenGLVertexArrayObject()),
        transformation(transformationMatrix)
        {

    std::vector<unsigned int> indices;
    std::vector<float> data;


    data.emplace_back(vertex1.x);
    data.emplace_back(vertex1.y);
    data.emplace_back(vertex1.z);
    data.emplace_back(vertex2.x);
    data.emplace_back(vertex2.y);
    data.emplace_back(vertex2.z);
    indices.emplace_back(0);
    indices.emplace_back(1);

    this->vertexBuffer->create();
    this->vertexBuffer->bind();
    this->vertexBuffer->allocate(&data.front(), data.size() * sizeof(float));

    this->vertexArray->create();
    this->vertexArray->bind();
    this->initializeOpenGLFunctions();

    glEnableVertexAttribArray(0);

    this->indexBuffer->create();
    this->indexBuffer->bind();
    this->indexBuffer->allocate(&indices.front(), indices.size() * sizeof(unsigned int));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          nullptr);
}

const Color &RenderLine::getColor() const {
    return color;
}
