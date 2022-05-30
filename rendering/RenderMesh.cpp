//
// Created by Jonas on 18/05/2022.
//

#include "RenderMesh.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>

#include "OpenGLRenderWidget.h"
#include "ShaderProgramSource.h"
#include <QOpenGLShaderProgram>
#include <utility>

RenderMesh::RenderMesh(const WorldSpaceMesh& worldSpaceMesh, const std::shared_ptr<QOpenGLShaderProgram>& ambientShader, const std::shared_ptr<QOpenGLShaderProgram>& diffuseShader):
        AbstractRenderModel(worldSpaceMesh.getModelTransformation().getMatrix(), worldSpaceMesh.getModelSpaceMesh()->getName()),
        ambientShader(ambientShader),
        diffuseShader(diffuseShader),
        boundingBox(worldSpaceMesh, ambientShader)
{

    const std::vector<Vertex> vertices = worldSpaceMesh.getModelSpaceMesh()->getVertices();
    const std::vector<IndexTriangle> triangles = worldSpaceMesh.getModelSpaceMesh()->getTriangles();

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
}

bool RenderMesh::isWireframeEnabled() const {
    return wireframeEnabled;
}

void RenderMesh::setWireframeEnabled(bool newWireframeEnabled) {
    RenderMesh::wireframeEnabled = newWireframeEnabled;
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
            const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->getTransformation();
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

            GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

            const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->getTransformation();
            glm::vec3 viewSpaceLightDirection = glm::vec4(0, 0, 1, 1) * viewMatrix;
            const glm::vec3 modelLightDirection = glm::vec3(glm::vec4(viewSpaceLightDirection, 1.0f) * this->getTransformation());
            const float ambientLighting = 0.05f;
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

RenderMesh::~RenderMesh() {
    delete indexBuffer;
    delete vertexBuffer;
    delete vertexArray;
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
}

void RenderMesh::showContextMenu(const QPoint &position) {
    QMenu contextMenu(QString("Context menu"));

    QAction* visibleAction = contextMenu.addAction(QString("Visible"));
    QObject::connect(visibleAction, &QAction::triggered, [=](bool enabled){
        this->setVisible(enabled);
    });
    visibleAction->setCheckable(true);
    visibleAction->setChecked(this->isVisible());
    contextMenu.addAction(visibleAction);

    QAction* wireframeAction = contextMenu.addAction(QString("Wireframe"));
    QObject::connect(wireframeAction, &QAction::triggered, [=](bool enabled){
        this->setWireframeEnabled(enabled);
    });
    wireframeAction->setCheckable(true);
    wireframeAction->setChecked(this->isWireframeEnabled());
    contextMenu.addAction(wireframeAction);

    QAction* cullingAction = contextMenu.addAction(QString("Culling"));
    QObject::connect(cullingAction, &QAction::triggered, [=](bool enabled){
        this->setCullingEnabled(enabled);
    });
    cullingAction->setCheckable(true);
    cullingAction->setChecked(this->isCullingEnabled());
    contextMenu.addAction(cullingAction);

    QAction* boundingBoxAction = contextMenu.addAction(QString("Bounding box"));
    QObject::connect(boundingBoxAction, &QAction::triggered, [=](bool enabled){
        this->setBoundingBoxEnabled(enabled);
    });
    boundingBoxAction->setCheckable(true);
    boundingBoxAction->setChecked(this->isBoundingBoxEnabled());
    contextMenu.addAction(boundingBoxAction);

    QAction* colorAction = contextMenu.addAction(QString("Change Color..."));
    QObject::connect(colorAction, &QAction::triggered, [=](){
        auto initialColor = this->getColor();
        auto resultColor = QColorDialog::getColor(QColor(255.f*initialColor.r, 255.f*initialColor.g, 255.f*initialColor.b, 255.f*initialColor.a), nullptr, QString(), QColorDialog::ShowAlphaChannel);
        if(resultColor.isValid()){
            this->setColor(Color(resultColor.red() / 255.f, resultColor.green() / 255.f, resultColor.blue() / 255.f, resultColor.alpha() / 255.f));
        }
    });
    contextMenu.addAction(colorAction);

    contextMenu.exec(position);
}

void RenderMesh::setColor(const Color &newColor) {
    AbstractRenderModel::setColor(newColor);
    this->boundingBox.setColor(Color(newColor.r, newColor.g, newColor.b, 1.0f));
}

void RenderMesh::setTransformationMatrix(const glm::mat4 &newTransformationMatrix) {
    AbstractRenderModel::setTransformationMatrix(newTransformationMatrix);
    this->boundingBox.setTransformationMatrix(newTransformationMatrix);
}
