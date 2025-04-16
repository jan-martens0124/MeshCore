//
// Created by Jonas on 15/05/2024.
//

#include "RenderPlane.h"
#include "Exception.h"
#include "meshcore/rendering/OpenGLWidget.h"
#include <glm/gtc/type_ptr.hpp>

void RenderPlane::draw(const OpenGLWidget *openGLWidget, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) {
    if(this->isVisible()){

        // Draw normal if enabled
        if(this->normalEnabled){
            this->renderNormal.draw(openGLWidget, viewMatrix, projectionMatrix, lightMode);
        }

        // Bind required buffers and shaders
        this->initializeOpenGLFunctions();
        this->vertexArray->bind();
        this->indexBuffer->bind();

        auto& shader = openGLWidget->getAmbientShader();
        shader->bind();

        // Toggle culling as required
        if(this->cullingEnabled){
            GL_CALL(glEnable(GL_CULL_FACE));
        }
        else{
            GL_CALL(glDisable(GL_CULL_FACE));
        }

        // Set MVP matrix uniform
        const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->getTransformationMatrix();
        shader->setUniformValue("u_ModelViewProjectionMatrix", QMatrix4x4(glm::value_ptr(modelViewProjectionMatrix)).transposed());

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
        shader->setUniformValue("u_Color", drawColor);

        GL_CALL(glDrawElements(GL_TRIANGLES, this->indexBuffer->size()/sizeof(unsigned int), GL_UNSIGNED_INT, nullptr));
    }
}

RenderPlane::RenderPlane(const Plane &plane, const Transformation &transformation): AbstractRenderModel(transformation, "Plane"),
                                                                                    renderNormal(Ray(glm::vec3(0,0,0), plane.getNormal())) {

    auto axis = glm::cross(plane.getNormal(), glm::vec3(0,0,1));
    float angle = -glm::acos(glm::dot(plane.getNormal(), glm::vec3(0,0,1)));

    if(glm::length(axis) < 1e-6){
        axis = glm::vec3(1,0,0);
    }

    Quaternion normalAlignmentRotation(axis, angle);

    this->renderNormal = RenderRay(Ray(normalAlignmentRotation.rotateVertex(glm::vec3(0,0,-plane.getD())), plane.getNormal()));
    this->renderNormal.setMaterial(PhongMaterial(Color::Red()));
    this->renderNormal.getTransformation().factorScale(10);

    std::vector<glm::vec3> data;
    data.push_back(normalAlignmentRotation.rotateVertex(glm::vec3(100,100,-plane.getD())));
    data.push_back(normalAlignmentRotation.rotateVertex(glm::vec3(100,-100,-plane.getD())));
    data.push_back(normalAlignmentRotation.rotateVertex(glm::vec3(-100,-100,-plane.getD())));
    data.push_back(normalAlignmentRotation.rotateVertex(glm::vec3(-100,100,-plane.getD())));

    std::vector<unsigned int> indices;

    // TODO culling option can replace this
//    indices.push_back(0);
//    indices.push_back(1);
//    indices.push_back(2);
//    indices.push_back(0);
//    indices.push_back(2);
//    indices.push_back(3);

    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(1);
    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(2);

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

QMenu *RenderPlane::getContextMenu() {
    auto* contextMenu = AbstractRenderModel::getContextMenu();

    contextMenu->addSeparator();

    QAction* cullingAction = contextMenu->addAction(QString("Culling"));
    QObject::connect(cullingAction, &QAction::triggered, [=](bool enabled){
        this->setCullingEnabled(enabled);
    });
    cullingAction->setCheckable(true);
    cullingAction->setChecked(this->isCullingEnabled());
    contextMenu->addAction(cullingAction);

    QAction* normalAction = contextMenu->addAction(QString("Normal"));
    QObject::connect(normalAction, &QAction::triggered, [=](bool enabled){
        this->normalEnabled = enabled;
    });
    normalAction->setCheckable(true);
    normalAction->setChecked(this->normalEnabled);
    contextMenu->addAction(normalAction);

    return contextMenu;
}

bool RenderPlane::isCullingEnabled() const {
    return cullingEnabled;
}

void RenderPlane::setCullingEnabled(bool cullingEnabled) {
    RenderPlane::cullingEnabled = cullingEnabled;
}

void RenderPlane::setTransformation(const Transformation &transformation) {
    AbstractRenderModel::setTransformation(transformation);
    this->renderNormal.setTransformation(transformation);
}


