//
// Created by Jonas on 1/12/2020.
//

#include "RenderModel.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>

#include "OpenGLRenderWidget.h"
#include "ShaderProgramSource.h"
#include <QOpenGLShaderProgram>
#include <utility>

RenderModel::RenderModel(const WorldSpaceMesh& worldSpaceMesh, std::shared_ptr<QOpenGLShaderProgram> ambientShader, std::shared_ptr<QOpenGLShaderProgram> diffuseShader):
        color(1,1,1,1),
        vertexBuffer(new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer)),
        indexBuffer(new QOpenGLBuffer(QOpenGLBuffer::Type::IndexBuffer)),
        vertexArray(new QOpenGLVertexArrayObject()),
        transformation(worldSpaceMesh.getModelTransformation().getMatrix()),
        ambientShader(std::move(ambientShader)),
        diffuseShader(std::move(diffuseShader))
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

bool RenderModel::isCullingEnabled() const {
    return cullingEnabled;
}

void RenderModel::setCullingEnabled(bool cullingEnabled) {
    RenderModel::cullingEnabled = cullingEnabled;
}

bool RenderModel::isWireframeEnabled() const {
    return wireframeEnabled;
}

void RenderModel::setWireframeEnabled(bool wireframeEnabled) {
    RenderModel::wireframeEnabled = wireframeEnabled;
}

void RenderModel::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool lightMode) {

    if(this->visible){
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
            const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->transformation;
            this->ambientShader->setUniformValue("u_ModelViewProjectionMatrix", QMatrix4x4(glm::value_ptr(modelViewProjectionMatrix)).transposed());
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
            this->ambientShader->setUniformValue("u_Color", drawColor);
        }
        else{

            GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

            const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->transformation;
            glm::vec3 viewSpaceLightDirection = glm::vec4(0, 0, 1, 1) * viewMatrix;
            const glm::vec3 modelLightDirection = glm::vec3(glm::vec4(viewSpaceLightDirection, 1.0f) * this->transformation);
            const float ambientLighting = 0.05f;

            this->diffuseShader->bind();
            this->diffuseShader->setUniformValue("u_Ambient", ambientLighting);
            this->diffuseShader->setUniformValue("u_LightDirection", QVector3D(modelLightDirection.x, modelLightDirection.y, modelLightDirection.z));
            this->diffuseShader->setUniformValue("u_ModelViewProjectionMatrix", QMatrix4x4(glm::value_ptr(modelViewProjectionMatrix)).transposed());
            this->diffuseShader->setUniformValue("u_Color", QVector4D(this->color.r, this->color.g, this->color.b, this->color.a));

        }

        GL_CALL(glDrawElements(GL_TRIANGLES, this->indexBuffer->size()/sizeof(unsigned int),  GL_UNSIGNED_INT, nullptr));
    }
}

RenderModel::~RenderModel() {
    delete indexBuffer;
    delete vertexBuffer;
    delete vertexArray;
}

RenderModel &RenderModel::operator=(RenderModel &&other) noexcept {
    if(this != &other){
        this->indexBuffer = other.indexBuffer;
        this->vertexArray = other.vertexArray;
        this->vertexBuffer = other.vertexBuffer;
        this->color = other.color;
        this->transformation = other.transformation;
        this->cullingEnabled = other.cullingEnabled;
        this->wireframeEnabled = other.wireframeEnabled;
        this->ambientShader = other.ambientShader;
        this->diffuseShader = other.diffuseShader;
        this->visible = other.visible;

        other.indexBuffer = nullptr;
        other.vertexArray = nullptr;
        other.vertexBuffer = nullptr;
    }
    return *this;
}

RenderModel::RenderModel(RenderModel &&other) noexcept:
    color(other.color),
    transformation(other.transformation)
{
    this->indexBuffer = other.indexBuffer;
    this->vertexArray = other.vertexArray;
    this->vertexBuffer = other.vertexBuffer;
    this->color = other.color;
    this->transformation = other.transformation;
    this->cullingEnabled = other.cullingEnabled;
    this->wireframeEnabled = other.wireframeEnabled;
    this->ambientShader = other.ambientShader;
    this->diffuseShader = other.diffuseShader;
    this->visible = other.visible;

    other.indexBuffer = nullptr;
    other.vertexArray = nullptr;
    other.vertexBuffer = nullptr;
}

void RenderModel::setTransformationMatrix(const glm::mat4 &transformationMatrix) {
    RenderModel::transformation = transformationMatrix;
}

void RenderModel::setColor(const Color &c){
    RenderModel::color = c;
}

const Color &RenderModel::getColor() const {
    return color;
}

bool RenderModel::isVisible() const {
    return visible;
}

void RenderModel::setVisible(bool visible) {
    RenderModel::visible = visible;
}
