//
// Created by Jonas on 18/07/2022.
//

#include <glm/gtc/type_ptr.hpp>
#include "RenderSphere.h"
#include "Exception.h"
#include "../core/IndexTriangle.h"
#include <QFrame>
#include <QCheckBox>
#include <QGridLayout>
#include <glm/gtx/normal.hpp>

RenderSphere::RenderSphere(const Sphere &sphere,
                           const Transformation& transformation,
                           const std::shared_ptr<QOpenGLShaderProgram> &ambientShader,
                           const std::shared_ptr<QOpenGLShaderProgram> &diffuseShader):

                           AbstractRenderModel(transformation, "Sphere"),
                           ambientShader(ambientShader),
                           diffuseShader(diffuseShader) {

    std::vector<Vertex> vertices;
    std::vector<IndexTriangle> indexTriangles;

    this->unscaledRadius = sphere.radius;
    this->unscaledVolume = sphere.getVolume();
    this->unscaledSurfaceArea = sphere.getSurfaceArea();

    // TODO the icosahedron approach might require fewer triangles for good looking spheres
    const auto sectorCount = 64;
    const auto stackCount = 32;
//    const auto sectorCount = 128;
//    const auto stackCount = 128;

    // Calculate vertices on the surface of the sphere
    for(auto currentStack=0u; currentStack <= stackCount; ++currentStack){
        const auto stackAngle = -glm::pi<float>() * ((float)currentStack / (float)stackCount-0.5f);
        for(auto currentSector=0u; currentSector <= sectorCount; ++currentSector){
            const auto sectorAngle= glm::two_pi<float>() * (float)currentSector / (float)sectorCount;
            const auto x= sphere.center.x + sphere.radius*glm::cos(stackAngle)*glm::cos(sectorAngle);
            const auto y= sphere.center.y + sphere.radius*glm::cos(stackAngle)*glm::sin(sectorAngle);
            const auto z= sphere.center.z + sphere.radius*glm::sin(stackAngle);
            vertices.emplace_back(x, y, z);
        }
    }

    // Generate indexTriangles with CCW indices according to http://www.songho.ca/opengl/gl_sphere.html
    // k1--k1+1
    // |  / |
    // | /  |
    // k2--k2+1
    unsigned int k1, k2;
    for(unsigned int currentStack = 0; currentStack < stackCount; ++currentStack)
    {
        k1 = currentStack * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for(unsigned int currentSector = 0; currentSector < sectorCount; ++currentSector, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(currentStack != 0) {
                indexTriangles.emplace_back(IndexTriangle{k1, k2, k1 + 1});
            }

            // k1+1 => k2 => k2+1
            if(currentStack != (stackCount - 1)) {
                indexTriangles.emplace_back(IndexTriangle{k1+1, k2, k2 + 1});
            }
        }
    }

    // Convert this to data in the buffers
    std::vector<unsigned int> indices;
    std::vector<float> data;
    for(unsigned int i=0; i<indexTriangles.size(); i++) {

        IndexTriangle t = indexTriangles[i];
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

    this->setColor(Color(1));
}


void RenderSphere::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) {


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

            GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

            const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->getTransformationMatrix();
            glm::vec3 viewSpaceLightDirection = glm::vec4(0, 0, 1, 1) * viewMatrix;
            const glm::vec3 modelLightDirection = glm::vec3(glm::vec4(viewSpaceLightDirection, 1.0f) *
                                                            this->getTransformationMatrix());
            const float ambientLighting = 0.05f;
            const auto color = this->getColor();
            this->diffuseShader->bind();
            this->diffuseShader->setUniformValue("u_Ambient", ambientLighting);
            this->diffuseShader->setUniformValue("u_LightDirection", QVector3D(modelLightDirection.x, modelLightDirection.y, modelLightDirection.z));
            this->diffuseShader->setUniformValue("u_ModelViewProjectionMatrix", QMatrix4x4(glm::value_ptr(modelViewProjectionMatrix)).transposed());
            this->diffuseShader->setUniformValue("u_Color", QVector4D(color.r, color.g, color.b, color.a));

        }

        GL_CALL(glDrawElements(GL_TRIANGLES, this->indexBuffer->size()/sizeof(unsigned int),  GL_UNSIGNED_INT, nullptr));
    }
}


RenderModelDetailDialog *RenderSphere::createRenderModelDetailDialog(QWidget *parent) {

    auto dialog = AbstractRenderModel::createRenderModelDetailDialog(parent);

    auto* optionsLayout = new QGridLayout();

    auto listener = std::make_shared<SimpleRenderModelListener>();
    this->addListener(listener);

    auto visibleCheckBox = new QCheckBox(QString("Visible"));
    visibleCheckBox->setChecked(this->isVisible());
    listener->setOnVisibleChanged([=](bool oldVisible, bool newVisible) {
        visibleCheckBox->setChecked(newVisible);
    });
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
    listener->setOnChanged([=]() {
        wireframeCheckBox->setChecked(this->isWireframeEnabled());
        cullingCheckBox->setChecked(this->isCullingEnabled());
    });
    QObject::connect(cullingCheckBox, &QCheckBox::clicked, [&](bool enabled) {
        this->setCullingEnabled(enabled);
    });
    optionsLayout->addWidget(cullingCheckBox, 2, 0);

    auto* optionsWidget = new QWidget();
    optionsWidget->setLayout(optionsLayout);
    dialog->addTab(optionsWidget, QString("Options"));

    return dialog;
}

QMenu *RenderSphere::getContextMenu() {
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

    return contextMenu;
}

bool RenderSphere::isCullingEnabled() const {
    return cullingEnabled;
}

void RenderSphere::setCullingEnabled(bool newCullingEnabled) {
    RenderSphere::cullingEnabled = newCullingEnabled;
    for (const auto &listener: this->listeners){
        listener->notify();
    }
}

bool RenderSphere::isWireframeEnabled() const {
    return wireframeEnabled;
}

void RenderSphere::setWireframeEnabled(bool newWireframeEnabled) {
    RenderSphere::wireframeEnabled = newWireframeEnabled;
    for (const auto &listener: this->listeners){
        listener->notify();
    }
}
