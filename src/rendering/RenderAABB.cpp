//
// Created by Jonas on 8/11/2021.
//

#include <glm/gtc/type_ptr.hpp>
#include <QGridLayout>
#include <QLabel>
#include "RenderAABB.h"
#include "Exception.h"
#include "meshcore/utility/io.h"
#include "meshcore/rendering/OpenGLWidget.h"

void RenderAABB::draw(const OpenGLWidget* openGLWidget, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) {

    if (this->isVisible()) {

        // Bind required buffers and shaders
        this->initializeOpenGLFunctions();
        this->vertexArray->bind();
        this->indexBuffer->bind();

        auto& ambientShader = openGLWidget->getAmbientShader();

        ambientShader->bind();

        // Set MVP matrix uniform
        const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->getTransformationMatrix();
        ambientShader->setUniformValue("u_ModelViewProjectionMatrix",
                                             QMatrix4x4(glm::value_ptr(modelViewProjectionMatrix)).transposed());

        // Set color uniform
        QVector4D drawColor;
        const auto color = this->getMaterial().getDiffuseColor();
        drawColor = QVector4D(color.r, color.g, color.b, color.a);
        if (lightMode) {
            if (glm::vec3(color) == glm::vec3(1, 1, 1)) {
                drawColor = QVector4D(0, 0, 0, color.a);
            } else if (glm::vec3(color) == glm::vec3(0, 0, 0)) {
                drawColor = QVector4D(1, 1, 1, color.a);
            }
        }
        ambientShader->setUniformValue("u_Color", drawColor);


        GL_CALL(glDrawElements(GL_LINES, this->indexBuffer->size() / sizeof(unsigned int), GL_UNSIGNED_INT, nullptr));
    }
}

RenderAABB::RenderAABB(const AABB &aabb, const Transformation& transformation):
        AbstractRenderModel(transformation, "AABB"),
        aabb(aabb){

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

    this->setMaterial(PhongMaterial(Color::White()));
}

RenderAABB::RenderAABB(const WorldSpaceMesh &worldSpaceMesh):
RenderAABB(worldSpaceMesh.getModelSpaceMesh()->getBounds(), worldSpaceMesh.getModelTransformation()) {}

RenderModelDetailDialog *RenderAABB::createRenderModelDetailDialog(QWidget* parent) {
    auto dialog = AbstractRenderModel::createRenderModelDetailDialog(parent);

    auto* detailsLayout = new QGridLayout();

    auto min = aabb.getMinimum();
    auto max = aabb.getMaximum();
    detailsLayout->addWidget(new QLabel(QString::fromStdString("Minimum: (" + std::to_string(min.x) + "," + std::to_string(min.y) + "," + std::to_string(min.z) + ")")), 0, 0);
    detailsLayout->addWidget(new QLabel(QString::fromStdString("Maximum: (" + std::to_string(max.x) + "," + std::to_string(max.y) + "," + std::to_string(max.z) + ")")), 1, 0);

    detailsLayout->addWidget(new QLabel(QString::fromStdString("Unscaled surface area: " + std::to_string(aabb.getSurfaceArea()))), 2, 0);
    detailsLayout->addWidget(new QLabel(QString::fromStdString("Unscaled volume: " + std::to_string(aabb.getVolume()))), 3, 0);

    const auto scale = this->getTransformation().getScale();
    detailsLayout->addWidget(new QLabel(QString::fromStdString("Surface area: " + std::to_string(aabb.getSurfaceArea() * scale * scale))), 4, 0);
    detailsLayout->addWidget(new QLabel(QString::fromStdString("Volume: " + std::to_string(aabb.getVolume() * scale * scale * scale))), 5, 0);

    auto* detailsWidget = new QWidget();
    detailsWidget->setLayout(detailsLayout);
    dialog->addTab(detailsWidget, QString("Details"));

    return dialog;
}


