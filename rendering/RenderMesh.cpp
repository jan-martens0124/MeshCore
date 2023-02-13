//
// Created by Jonas on 18/05/2022.
//

#include "RenderMesh.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>

#include "ShaderProgramSource.h"
#include <QOpenGLShaderProgram>
#include <utility>
#include <QLabel>
#include <QFrame>
#include <QCheckBox>
#include <QGridLayout>
#include "Exception.h"

RenderMesh::RenderMesh(const WorldSpaceMesh& worldSpaceMesh, const std::shared_ptr<QOpenGLShaderProgram>& ambientShader, const std::shared_ptr<QOpenGLShaderProgram>& diffuseShader):
        AbstractRenderModel(worldSpaceMesh.getModelTransformation(), worldSpaceMesh.getModelSpaceMesh()->getName()),
        ambientShader(ambientShader),
        diffuseShader(diffuseShader),
        boundingBox(worldSpaceMesh, ambientShader)
{

    const std::vector<Vertex>& vertices = worldSpaceMesh.getModelSpaceMesh()->getVertices();
    const std::vector<IndexTriangle>& triangles = worldSpaceMesh.getModelSpaceMesh()->getTriangles();

    this->numberOfVertices = vertices.size();
    this->numberOfTriangles = triangles.size();
    this->unscaledSurfaceArea = worldSpaceMesh.getModelSpaceMesh()->getSurfaceArea();
    this->unscaledVolume = worldSpaceMesh.getModelSpaceMesh()->getVolume();

    std::vector<unsigned int> indices;
    std::vector<float> data;
    for(unsigned int i=0; i<triangles.size(); i++) {

        IndexTriangle t = triangles[i];
        std::vector<Vertex> triangleVertices;
        triangleVertices.emplace_back(vertices[t.vertexIndex0]);
        triangleVertices.emplace_back(vertices[t.vertexIndex1]);
        triangleVertices.emplace_back(vertices[t.vertexIndex2]);
        glm::vec3 normal = glm::triangleNormal(triangleVertices[0], triangleVertices[1], triangleVertices[2]);

        // We push each model space vertex once for each triangle because the normal is different for each triangle
        // (Adding up the normals in the shader doesn't provide visually satisfying results
        for (unsigned int j = 0; j < 3; j++) {
            data.emplace_back(triangleVertices[j].x);
            data.emplace_back(triangleVertices[j].y);
            data.emplace_back(triangleVertices[j].z);
            data.emplace_back(normal.x);
            data.emplace_back(normal.y);
            data.emplace_back(normal.z);
            indices.emplace_back(i * 3 + j);
        }
    }

    this->vertexBuffer->create();
    this->vertexBuffer->bind();
    this->vertexBuffer->allocate(&data.front(), data.size() * sizeof(float));

    this->vertexArray->create();
    this->vertexArray->bind();

    this->initializeOpenGLFunctions();

    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glEnableVertexAttribArray(1));

    this->indexBuffer->create();
    this->indexBuffer->bind();
    this->indexBuffer->allocate(&indices.front(), indices.size() * sizeof(unsigned int));

    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr));
    GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat))));
}

bool RenderMesh::isCullingEnabled() const {
    return cullingEnabled;
}

void RenderMesh::setCullingEnabled(bool newCullingEnabled) {
    RenderMesh::cullingEnabled = newCullingEnabled;
    for (const auto &listener: this->listeners){
        listener->notify();
    }
}

bool RenderMesh::isWireframeEnabled() const {
    return wireframeEnabled;
}

void RenderMesh::setWireframeEnabled(bool newWireframeEnabled) {
    RenderMesh::wireframeEnabled = newWireframeEnabled;
    for (const auto &listener: this->listeners){
        listener->notify();
    }
}

void RenderMesh::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool lightMode) {

    if(this->isVisible()){

        this->initializeOpenGLFunctions();
        this->vertexArray->bind();
        this->indexBuffer->bind();

        if(this->cullingEnabled){
            GL_CALL(glEnable(GL_CULL_FACE));
        }
        else{
            GL_CALL(glDisable(GL_CULL_FACE));
        }

        if(this->wireframeEnabled){

            GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

            this->ambientShader->bind();
            const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->getTransformationMatrix();
            this->ambientShader->setUniformValue("u_ModelViewProjectionMatrix", QMatrix4x4(glm::value_ptr(modelViewProjectionMatrix)).transposed());
            QVector4D drawColor;
            const auto color = this->getColor();
            drawColor = QVector4D(color.r, color.g, color.b, color.a);
            if(lightMode){
                if(glm::vec3(color) == glm::vec3(1,1,1)){
                    drawColor = QVector4D(0, 0, 0, color.a);
                }
                else if(glm::vec3(color) == glm::vec3(0,0,0)){
                    drawColor = QVector4D(1, 1, 1, color.a);
                }
            }
            this->ambientShader->setUniformValue("u_Color", drawColor);
        }
        else{
            // TODO what if we want to draw the surface and the wireframe at the same time?
            //  How to determine the right color=> separate: default color, combined: color for surface and black/white wireframe
            GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

            const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->getTransformationMatrix();
            glm::vec3 viewSpaceLightDirection = glm::vec4(0, 0, 1, 1) * viewMatrix;
            const glm::vec3 modelLightDirection = glm::vec3(glm::vec4(viewSpaceLightDirection, 1.0f) *
                                                                    this->getTransformationMatrix());
            const float ambientLighting = 0.05f; // TODO make this a configurable member of OpenGLWidget
            const auto color = this->getColor();
            this->diffuseShader->bind();
            this->diffuseShader->setUniformValue("u_Ambient", ambientLighting);
            this->diffuseShader->setUniformValue("u_LightDirection", QVector3D(modelLightDirection.x, modelLightDirection.y, modelLightDirection.z));
            this->diffuseShader->setUniformValue("u_ModelViewProjectionMatrix", QMatrix4x4(glm::value_ptr(modelViewProjectionMatrix)).transposed());
            this->diffuseShader->setUniformValue("u_Color", QVector4D(color.r, color.g, color.b, color.a));

        }

        GL_CALL(glDrawElements(GL_TRIANGLES, this->indexBuffer->size()/sizeof(unsigned int),  GL_UNSIGNED_INT, nullptr));

        if(this->boundingBoxEnabled) boundingBox.draw(viewMatrix, projectionMatrix, lightMode);
    }
}

RenderMesh &RenderMesh::operator=(RenderMesh &&other) noexcept {
    if(this != &other){
        this->indexBuffer = other.indexBuffer;
        this->vertexArray = other.vertexArray;
        this->vertexBuffer = other.vertexBuffer;
        this->cullingEnabled = other.cullingEnabled;
        this->wireframeEnabled = other.wireframeEnabled;
        this->ambientShader = other.ambientShader;
        this->diffuseShader = other.diffuseShader;
        this->boundingBoxEnabled = other.boundingBoxEnabled;
        this->boundingBox = std::move(other.boundingBox);
        this->numberOfTriangles = other.numberOfTriangles;
        this->numberOfVertices = other.numberOfVertices;
        this->unscaledSurfaceArea = other.unscaledSurfaceArea;
        this->unscaledVolume = other.unscaledVolume;

        other.indexBuffer = nullptr;
        other.vertexArray = nullptr;
        other.vertexBuffer = nullptr;
    }
    return *this;
}

bool RenderMesh::isBoundingBoxEnabled() const {
    return boundingBoxEnabled;
}

void RenderMesh::setBoundingBoxEnabled(bool newBoundingBoxEnabled) {
    RenderMesh::boundingBoxEnabled = newBoundingBoxEnabled;
    for (const auto &listener: this->listeners){
        listener->notify();
    }
}

QMenu* RenderMesh::getContextMenu() {

    auto* contextMenu = AbstractRenderModel::getContextMenu();

    contextMenu->addSeparator();

    QAction* wireframeAction = contextMenu->addAction(QString("Wireframe"));
    QObject::connect(wireframeAction, &QAction::triggered, [=](bool enabled){
        this->setWireframeEnabled(enabled);
    });
    wireframeAction->setCheckable(true);
    wireframeAction->setChecked(this->isWireframeEnabled());
    contextMenu->addAction(wireframeAction);

    QAction* cullingAction = contextMenu->addAction(QString("Culling"));
    QObject::connect(cullingAction, &QAction::triggered, [=](bool enabled){
        this->setCullingEnabled(enabled);
    });
    cullingAction->setCheckable(true);
    cullingAction->setChecked(this->isCullingEnabled());
    contextMenu->addAction(cullingAction);

    QAction* boundingBoxAction = contextMenu->addAction(QString("Bounding box"));
    QObject::connect(boundingBoxAction, &QAction::triggered, [=](bool enabled){
        this->setBoundingBoxEnabled(enabled);
    });
    boundingBoxAction->setCheckable(true);
    boundingBoxAction->setChecked(this->isBoundingBoxEnabled());
    contextMenu->addAction(boundingBoxAction);
    return contextMenu;

    // TODO add option to save the transformed mesh (int its current position) to a file
}

void RenderMesh::setColor(const Color &newColor) {
    AbstractRenderModel::setColor(newColor);
    this->boundingBox.setColor(Color(newColor.r, newColor.g, newColor.b, 1.0f));
}

RenderModelDetailDialog* RenderMesh::createRenderModelDetailDialog(QWidget* parent) {

    auto dialog = AbstractRenderModel::createRenderModelDetailDialog(parent);

    auto* detailsLayout = new QGridLayout();
    detailsLayout->addWidget(new QLabel(QString::fromStdString("Number of vertices: " + std::to_string(numberOfVertices))), 0, 0);
    detailsLayout->addWidget(new QLabel(QString::fromStdString("Number of triangles: " + std::to_string(numberOfTriangles))), 1, 0);
    detailsLayout->addWidget(new QLabel(QString::fromStdString("Unscaled surface area: " + std::to_string(unscaledSurfaceArea))), 2, 0);
    detailsLayout->addWidget(new QLabel(QString::fromStdString("Unscaled volume: " + std::to_string(unscaledVolume))), 3, 0);

    auto* detailsWidget = new QWidget();
    detailsWidget->setLayout(detailsLayout);
    dialog->addTab(detailsWidget, QString("Details"));

    auto* optionsLayout = new QGridLayout();

    auto visibleCheckBox = new QCheckBox(QString("Visible"));
    visibleCheckBox->setChecked(this->isVisible());
    QObject::connect(visibleCheckBox, &QCheckBox::clicked, [&](bool enabled) {
        this->setVisible(enabled);
    });
    optionsLayout->addWidget(visibleCheckBox, 0, 0);

    auto wireframeCheckBox = new QCheckBox(QString("Show Wireframe"));
    wireframeCheckBox->setChecked(this->isWireframeEnabled());
    QObject::connect(wireframeCheckBox, &QCheckBox::clicked, [&](bool enabled) {
        this->setWireframeEnabled(enabled);
    });
    optionsLayout->addWidget(wireframeCheckBox, 1, 0);

    auto cullingCheckBox = new QCheckBox(QString("Enable Culling"));
    cullingCheckBox->setChecked(this->isCullingEnabled());

    QObject::connect(cullingCheckBox, &QCheckBox::clicked, [&](bool enabled) {
        this->setCullingEnabled(enabled);
    });
    optionsLayout->addWidget(cullingCheckBox, 2, 0);

    auto boundingBoxCheckBox = new QCheckBox(QString("Show Bounding Box"));
    boundingBoxCheckBox->setChecked(this->isBoundingBoxEnabled());
    QObject::connect(boundingBoxCheckBox, &QCheckBox::clicked, [&](bool enabled) {
        this->setBoundingBoxEnabled(enabled);
    });
    optionsLayout->addWidget(boundingBoxCheckBox, 3, 0);

    auto listener = std::make_shared<SimpleRenderModelListener>();
    this->addListener(listener);
    listener->setOnVisibleChanged([=](bool oldVisible, bool newVisible) {
        visibleCheckBox->setChecked(newVisible);
    });
    listener->setOnChanged([=]() {
        wireframeCheckBox->setChecked(this->isWireframeEnabled());
        cullingCheckBox->setChecked(this->isCullingEnabled());
        boundingBoxCheckBox->setChecked(this->isBoundingBoxEnabled());
    });

    auto volumeWidget = new QLabel(QString::fromStdString("Volume: " + std::to_string(unscaledVolume * this->getTransformation().getScale() * this->getTransformation().getScale() * this->getTransformation().getScale())));
    detailsLayout->addWidget(volumeWidget, 4, 0);
    listener->setOnTransformationChanged([=](const Transformation& oldTransformation, const Transformation& newTransformation) {
        volumeWidget->setText(QString::fromStdString("Volume: " + std::to_string(unscaledVolume * newTransformation.getScale() * newTransformation.getScale() * newTransformation.getScale())));
    });



    auto* optionsWidget = new QWidget();
    optionsWidget->setLayout(optionsLayout);
    dialog->addTab(optionsWidget, QString("Options"));

    return dialog;
}

void RenderMesh::setTransformation(const Transformation &newTransformation) {
    AbstractRenderModel::setTransformation(newTransformation);
    this->boundingBox.setTransformation(newTransformation);
}
