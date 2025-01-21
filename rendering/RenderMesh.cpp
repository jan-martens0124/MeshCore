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
#include "OpenGLWidget.h"

RenderMesh::RenderMesh(const WorldSpaceMesh& worldSpaceMesh):
        AbstractRenderModel(worldSpaceMesh.getModelTransformation(), worldSpaceMesh.getModelSpaceMesh()->getName()),
        boundingBox(worldSpaceMesh)
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

    // Store the axis render line
    auto& aabb = worldSpaceMesh.getModelSpaceMesh()->getBounds();
    axisRenderLines.emplace_back(std::make_shared<RenderLine>(glm::vec3(0,0,0), glm::vec3(glm::max(0.0f, aabb.getMaximum().x),0,0), Transformation()));
    axisRenderLines.emplace_back(std::make_shared<RenderLine>(glm::vec3(0,0,0), glm::vec3(0,glm::max(0.0f, aabb.getMaximum().y),0), Transformation()));
    axisRenderLines.emplace_back(std::make_shared<RenderLine>(glm::vec3(0,0,0), glm::vec3(0,0,glm::max(0.0f, aabb.getMaximum().z)), Transformation()));
    axisRenderLines[0]->setMaterial(PhongMaterial(Color::Red()));
    axisRenderLines[1]->setMaterial(PhongMaterial(Color::Green()));
    axisRenderLines[2]->setMaterial(PhongMaterial(Color::Blue()));

    for (const auto &renderLine: axisRenderLines){
        renderLine->setTransformation(worldSpaceMesh.getModelTransformation());
    }

    // Store the normals
    for (const auto &triangle: triangles){
        auto vertexTriangle = VertexTriangle(vertices[triangle.vertexIndex0], vertices[triangle.vertexIndex1], vertices[triangle.vertexIndex2]);
        auto center = vertexTriangle.getCentroid();
        auto normal = vertexTriangle.normal;
        normal = glm::normalize(normal) * glm::sqrt(glm::length(normal)); // Normals scale with surface, but we want them to scale with the scaling factor of the mod
        this->normalRays.emplace_back(center, normal);
    }
}

void RenderMesh::initializeNormals() {
    normalRenderRays.clear();
    normalRenderRays.reserve(normalRays.size());
    for (const auto &normalRay: this->normalRays){
        auto renderRay = std::make_shared<RenderRay>(normalRay, this->getTransformation());
        renderRay->setMaterial(PhongMaterial(Color::Red()));
        renderRay->setTransformation(this->getTransformation());
        this->normalRenderRays.emplace_back(renderRay);
    }
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

void RenderMesh::draw(const OpenGLWidget* openGLWidget, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool lightMode) {

    if(this->isVisible()){

        if(this->axisEnabled){
            for (const auto &axisRenderLine: this->axisRenderLines){
                axisRenderLine->draw(openGLWidget, viewMatrix, projectionMatrix, lightMode);
            }
        }

        if(this->boundingBoxEnabled) boundingBox.draw(openGLWidget, viewMatrix, projectionMatrix, lightMode);

        if(this->normalsEnabled) {
            if(this->normalRenderRays.empty()) this->initializeNormals();
            for (const auto &normalRenderRay: this->normalRenderRays) {
                normalRenderRay->draw(openGLWidget, viewMatrix, projectionMatrix, lightMode);
            }
        }

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

            auto& ambientShader = openGLWidget->getAmbientShader();

            ambientShader->bind();
            const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->getTransformationMatrix();
            ambientShader->setUniformValue("u_ModelViewProjectionMatrix", QMatrix4x4(glm::value_ptr(modelViewProjectionMatrix)).transposed());
            QVector4D drawColor;
            const auto color = this->getMaterial().getDiffuseColor();
            const auto wireframeColorFactor = 0.5f; // TODO correct for light mode, but what if dark? It should become ligther no?
            drawColor = QVector4D(wireframeColorFactor * color.r, wireframeColorFactor * color.g, wireframeColorFactor * color.b, color.a);
            if(lightMode){
                if(glm::vec3(color) == glm::vec3(1,1,1)){
                    drawColor = QVector4D(0, 0, 0, color.a);
                }
                else if(glm::vec3(color) == glm::vec3(0,0,0)){
                    drawColor = QVector4D(1, 1, 1, color.a);
                }
            }
            ambientShader->setUniformValue("u_Color", drawColor);

            GL_CALL(glDrawElements(GL_TRIANGLES, this->indexBuffer->size()/sizeof(unsigned int), GL_UNSIGNED_INT, nullptr));
        }
        if(this->surfaceEnabled){

            GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

            const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->getTransformationMatrix();
            glm::vec3 viewSpaceLightDirection = glm::vec4(0, 0, 1, 1) * viewMatrix;
            const glm::vec3 modelLightDirection = glm::vec3(glm::vec4(viewSpaceLightDirection, 1.0f) *
                                                                    this->getTransformationMatrix());
            glm::vec3 cameraPosition = glm::inverse(viewMatrix) * glm::vec4(0,0,0,1000);
            glm::vec3 modelSpaceCameraPosition = glm::vec3(glm::inverse(this->getTransformationMatrix()) * glm::vec4(cameraPosition, 1.0f));
            const float ambientLighting = 0.25f; // TODO make this a configurable member of OpenGLWidget
            const auto& material = this->getMaterial();

            auto diffuseColor = material.getDiffuseColor();
            auto specularColor = material.getSpecularColor();

            if(this->wireframeEnabled){
                // Make the colors more pastel
                auto pastelFactor = 0.2f;
                diffuseColor.r = (diffuseColor.r + pastelFactor) / (1+pastelFactor);
                diffuseColor.g = (diffuseColor.g + pastelFactor) / (1+pastelFactor);
                diffuseColor.b = (diffuseColor.b + pastelFactor) / (1+pastelFactor);
            }

            auto& phongShader = openGLWidget->getPhongShader();
            phongShader->bind();
            phongShader->setUniformValue("u_ModelViewProjectionMatrix", QMatrix4x4(glm::value_ptr(modelViewProjectionMatrix)).transposed());
            phongShader->setUniformValue("u_LightDirection", QVector3D(modelLightDirection.x, modelLightDirection.y, modelLightDirection.z));
            phongShader->setUniformValue("u_ViewPosition", QVector3D(modelSpaceCameraPosition.x, modelSpaceCameraPosition.y, modelSpaceCameraPosition.z));
            phongShader->setUniformValue("u_AmbientIntensity", ambientLighting);
            phongShader->setUniformValue("u_DiffuseIntensity", 1.0f-ambientLighting);
            phongShader->setUniformValue("u_SpecularIntensity", 0.5f);
            phongShader->setUniformValue("u_DiffuseColor", QVector4D(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a));
            phongShader->setUniformValue("u_SpecularColor", QVector4D(specularColor.r, specularColor.g, specularColor.b, specularColor.a));
            phongShader->setUniformValue("u_SpecularPower", 8.0f);

            GL_CALL(glDrawElements(GL_TRIANGLES, this->indexBuffer->size()/sizeof(unsigned int), GL_UNSIGNED_INT, nullptr));
        }
    }
}

RenderMesh &RenderMesh::operator=(RenderMesh &&other) noexcept {
    if(this != &other){
        this->indexBuffer = other.indexBuffer;
        this->vertexArray = other.vertexArray;
        this->vertexBuffer = other.vertexBuffer;
        this->cullingEnabled = other.cullingEnabled;
        this->wireframeEnabled = other.wireframeEnabled;
        this->boundingBoxEnabled = other.boundingBoxEnabled;
        this->axisEnabled = other.axisEnabled;
        this->normalsEnabled = other.normalsEnabled;
        this->surfaceEnabled = other.surfaceEnabled;
        this->boundingBox = std::move(other.boundingBox);
        this->numberOfTriangles = other.numberOfTriangles;
        this->numberOfVertices = other.numberOfVertices;
        this->unscaledSurfaceArea = other.unscaledSurfaceArea;
        this->unscaledVolume = other.unscaledVolume;
        this->axisRenderLines = std::move(other.axisRenderLines);
        this->normalRays = std::move(other.normalRays);
        this->normalRenderRays = std::move(other.normalRenderRays);

        other.indexBuffer = nullptr;
        other.vertexArray = nullptr;
        other.vertexBuffer = nullptr;
    }
    return *this;
}

bool RenderMesh::isAxisEnabled() const {
    return axisEnabled;
}

void RenderMesh::setAxisEnabled(bool newAxisEnabled) {
    RenderMesh::axisEnabled = newAxisEnabled;
    for (const auto &listener: this->listeners){
        listener->notify();
    }
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

    QAction* surfaceAction = contextMenu->addAction(QString("Surface"));
    QObject::connect(surfaceAction, &QAction::triggered, [=](bool enabled){
        this->setSurfaceEnabled(enabled);
    });
    surfaceAction->setCheckable(true);
    surfaceAction->setChecked(this->isSurfaceEnabled());
    contextMenu->addAction(surfaceAction);

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

    QAction* axisAction = contextMenu->addAction(QString("Axis"));
    QObject::connect(axisAction, &QAction::triggered, [=](bool enabled){
        this->setAxisEnabled(enabled);
    });
    axisAction->setCheckable(true);
    axisAction->setChecked(this->isAxisEnabled());
    contextMenu->addAction(axisAction);

    QAction* normalsAction = contextMenu->addAction(QString("Normals"));
    QObject::connect(normalsAction, &QAction::triggered, [=](bool enabled){
        this->setNormalsEnabled(enabled);
    });
    normalsAction->setCheckable(true);
    normalsAction->setChecked(this->isNormalsEnabled());
    contextMenu->addAction(normalsAction);

    return contextMenu;

    // TODO add option to save the transformed mesh (int its current position) to a file
}

void RenderMesh::setMaterial(const PhongMaterial& newMaterial) {
    AbstractRenderModel::setMaterial(newMaterial);
    this->boundingBox.setMaterial(newMaterial);
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

    auto surfaceCheckBox = new QCheckBox(QString("Show Surface"));
    surfaceCheckBox->setChecked(this->isSurfaceEnabled());
    QObject::connect(surfaceCheckBox, &QCheckBox::clicked, [&](bool enabled) {
        this->setSurfaceEnabled(enabled);
    });
    optionsLayout->addWidget(surfaceCheckBox, 1, 0);

    auto wireframeCheckBox = new QCheckBox(QString("Show Wireframe"));
    wireframeCheckBox->setChecked(this->isWireframeEnabled());
    QObject::connect(wireframeCheckBox, &QCheckBox::clicked, [&](bool enabled) {
        this->setWireframeEnabled(enabled);
    });
    optionsLayout->addWidget(wireframeCheckBox, 2, 0);

    auto cullingCheckBox = new QCheckBox(QString("Enable Culling"));
    cullingCheckBox->setChecked(this->isCullingEnabled());

    QObject::connect(cullingCheckBox, &QCheckBox::clicked, [&](bool enabled) {
        this->setCullingEnabled(enabled);
    });
    optionsLayout->addWidget(cullingCheckBox, 3, 0);

    auto boundingBoxCheckBox = new QCheckBox(QString("Show Bounding Box"));
    boundingBoxCheckBox->setChecked(this->isBoundingBoxEnabled());
    QObject::connect(boundingBoxCheckBox, &QCheckBox::clicked, [&](bool enabled) {
        this->setBoundingBoxEnabled(enabled);
    });
    optionsLayout->addWidget(boundingBoxCheckBox, 0, 1);

    auto axisCheckBox = new QCheckBox(QString("Show Axis"));
    axisCheckBox->setChecked(this->isAxisEnabled());
    QObject::connect(axisCheckBox, &QCheckBox::clicked, [&](bool enabled) {
        this->setAxisEnabled(enabled);
    });
    optionsLayout->addWidget(axisCheckBox, 1, 1);

    auto normalsCheckBox = new QCheckBox(QString("Show Normals"));
    normalsCheckBox->setChecked(this->isNormalsEnabled());
    QObject::connect(normalsCheckBox, &QCheckBox::clicked, [&](bool enabled) {
        this->setNormalsEnabled(enabled);
    });
    optionsLayout->addWidget(normalsCheckBox, 2, 1);

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
    for (const auto &axisRenderLine: this->axisRenderLines){
        axisRenderLine->setTransformation(newTransformation);
    }
    for (const auto &normalRenderRay: this->normalRenderRays){
        normalRenderRay->setTransformation(newTransformation);
    }
}

void RenderMesh::setSurfaceEnabled(bool newSurfaceEnabled) {
    RenderMesh::surfaceEnabled = newSurfaceEnabled;
    for (const auto &listener: this->listeners){
        listener->notify();
    }
}

bool RenderMesh::isSurfaceEnabled() const {
    return surfaceEnabled;
}

void RenderMesh::setNormalsEnabled(bool newNormalsEnabled) {
    RenderMesh::normalsEnabled = newNormalsEnabled;
    for (const auto &listener: this->listeners){
        listener->notify();
    }
}

bool RenderMesh::isNormalsEnabled() const {
    return normalsEnabled;
}
