//
// Created by Jonas on 26/08/2022.
//

#include "RenderOBB.h"
#include "Exception.h"
#include <glm/gtc/type_ptr.hpp>
#include <QGridLayout>
#include <QLabel>

RenderModelDetailDialog *RenderOBB::createRenderModelDetailDialog(QWidget *parent) {
    auto dialog = AbstractRenderModel::createRenderModelDetailDialog(parent);

    auto* detailsLayout = new QGridLayout();
    detailsLayout->addWidget(new QLabel(QString::fromStdString("Unscaled surface area: " + std::to_string(unscaledSurfaceArea))), 0, 0);
    detailsLayout->addWidget(new QLabel(QString::fromStdString("Unscaled volume: " + std::to_string(unscaledVolume))), 1, 0);

    const auto scale = this->getTransformation().getScale();
    detailsLayout->addWidget(new QLabel(QString::fromStdString("Surface area: " + std::to_string(unscaledSurfaceArea * scale * scale))), 2, 0);
    detailsLayout->addWidget(new QLabel(QString::fromStdString("Volume: " + std::to_string(unscaledVolume * scale * scale * scale))), 3, 0);

    auto* detailsWidget = new QWidget();
    detailsWidget->setLayout(detailsLayout);
    dialog->addTab(detailsWidget, QString("Details"));

    return dialog;
}

RenderOBB::RenderOBB(const OBB &obb, const Transformation &transformation,
                     const std::shared_ptr<QOpenGLShaderProgram> &shader):
        AbstractRenderModel(transformation, "OBB"),
        ambientShader(shader), obbRotation(obb.getRotation()) {


    std::vector<unsigned int> indices;
    std::vector<Vertex> vertices;
    std::vector<float> vertexData;

    this->unscaledVolume = obb.getVolume();
    this->unscaledSurfaceArea = obb.getSurfaceArea();

    Vertex min = obb.getAabb().getMinimum();
    Vertex max = obb.getAabb().getMaximum();

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

void RenderOBB::draw(const OpenGLWidget* openGLWidget, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) {

    if (this->isVisible()) {

        // Bind required buffers and shaders
        this->initializeOpenGLFunctions();
        this->vertexArray->bind();
        this->indexBuffer->bind();
        this->ambientShader->bind();

        // Set MVP matrix uniform
        const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->getTransformationMatrix() *
                this->obbRotation.computeMatrix();
        this->ambientShader->setUniformValue("u_ModelViewProjectionMatrix",
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
        this->ambientShader->setUniformValue("u_Color", drawColor);


        GL_CALL(glDrawElements(GL_LINES, this->indexBuffer->size() / sizeof(unsigned int), GL_UNSIGNED_INT, nullptr));
    }
}
