//
// Created by Jonas on 30/11/2020.
//

#include "OpenGLRenderWidget.h"
#include "ShaderProgramSource.h"
#include <QOpenGLShaderProgram>
#include <QPushButton>
#include <QtWidgets>

[[maybe_unused]] OpenGLRenderWidget::OpenGLRenderWidget(QWidget *parent): QOpenGLWidget(parent) {}

void OpenGLRenderWidget::initializeGL() {

    this->resetView();

    initializeOpenGLFunctions();

    GL_CALL(glClearColor(0,0,0,1));

    GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glDepthFunc(GL_LESS));
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL_CALL(glEnable(GL_MULTISAMPLE));

    GL_CALL(glPixelStorei(GL_PACK_ALIGNMENT, 1));

    // Options (should be available in GUI, per model)
    GL_CALL(glEnable(GL_CULL_FACE));
    GL_CALL(glCullFace(GL_BACK));

    ShaderProgramSource diffuseShaderProgramSource = ShaderProgramSource::parseShader("../../meshcore/rendering/shaders/Diffuse.shader");
//    ShaderProgramSource diffuseShaderProgramSource = ShaderProgramSource::parseShader("shaders/Diffuse.shader");
    diffuseShader = std::make_shared<QOpenGLShaderProgram>();
    diffuseShader->addShaderFromSourceCode(QOpenGLShader::Vertex, diffuseShaderProgramSource.VertexSource);
    diffuseShader->addShaderFromSourceCode(QOpenGLShader::Fragment, diffuseShaderProgramSource.FragmentSource);
    diffuseShader->bindAttributeLocation("vertex", 0);
    diffuseShader->bindAttributeLocation("normal", 1);
    diffuseShader->link();

    ShaderProgramSource basicShaderProgramSource = ShaderProgramSource::parseShader("../../meshcore/rendering/shaders/Ambient.shader");
//    ShaderProgramSource basicShaderProgramSource = ShaderProgramSource::parseShader("shaders/Ambient.shader");
    ambientShader = std::make_shared<QOpenGLShaderProgram>();
    ambientShader->addShaderFromSourceCode(QOpenGLShader::Vertex, basicShaderProgramSource.VertexSource);
    ambientShader->addShaderFromSourceCode(QOpenGLShader::Fragment, basicShaderProgramSource.FragmentSource);
    ambientShader->bindAttributeLocation("vertex", 0);
    ambientShader->bindAttributeLocation("normal", 1);
    ambientShader->link();
}

void OpenGLRenderWidget::resetView() {
    viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f, - INITIAL_VIEW_DISTANCE));
    this->update();
}

void OpenGLRenderWidget::resizeGL(int w, int h) {
    this->width = w;
    this->height = h;

    this->calculateProjectionMatrix();
}

void OpenGLRenderWidget::calculateProjectionMatrix(){
    if(this->usePerspective){
        this->projectionMatrix = glm::perspective(glm::radians(INITIAL_FOV), float(width)/float(height), 0.001f, 10000.0f);
    }
    else{
        float halfWidth = float(width)/20.0f;
        float halfHeight = float(height)/20.0f;
        this->projectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1000.0f, 1000.0f);
    }
}

void OpenGLRenderWidget::paintGL() {

    for (const auto &renderModel : this->renderModels){
        renderModel->draw(viewMatrix, projectionMatrix, lightMode);
    }

    for(auto& line: this->renderLines){
        line.draw(*ambientShader, viewMatrix, projectionMatrix);
    }

    for(auto& renderModel: this->sortedRenderModels){
        renderModel->draw(viewMatrix, projectionMatrix, lightMode);
    }
}

void OpenGLRenderWidget::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->x() - lastMousePosition.x();
    int dy = event->y() - lastMousePosition.y();
    lastMousePosition = event->pos();
    const float rotationSpeed = 0.01f;

    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);

    this->viewMatrix = glm::rotate(viewMatrix, rotationSpeed * float(dx), glm::vec3(glm::inverse(viewMatrix) * glm::vec4(cameraUp,0.0f)));
    this->viewMatrix = glm::rotate(viewMatrix, rotationSpeed * float(dy), glm::vec3(glm::inverse(viewMatrix) * glm::vec4(cameraRight, 0.0f)));
    this->update();
}

void OpenGLRenderWidget::wheelEvent(QWheelEvent *event) {
    auto factor = float(event->angleDelta().y()) / 1200.0f;
    auto distance = glm::length(glm::vec3(viewMatrix[3]));
//    viewMatrix = glm::translate(viewMatrix, glm::vec3(glm::inverse(viewMatrix) * glm::vec4(glm::vec3(0.0f, 0.0f, factor * distance), 0.0f)));

    viewMatrix = glm::scale(viewMatrix, glm::vec3(1 + factor));

    this->update();
}

void OpenGLRenderWidget::mousePressEvent(QMouseEvent *event) {
     lastMousePosition = event->pos();
     this->update();
     this->setFocus();
}

void OpenGLRenderWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
    {
        QWidget* window = this->parentWidget();
        while(!window->isWindow()){
            window = window->parentWidget();
        }

        if(!window->isMaximized()){
            window->showMaximized();
        }
        else{
            window->showNormal();
        }
    }
}

void OpenGLRenderWidget::keyPressEvent(QKeyEvent* event){
    const auto key = event->key();
    const auto distance = glm::length(glm::vec3(viewMatrix[3]));
    if(key == Qt::Key_Plus){
        float zoomFactor = 0.1f;
        viewMatrix = glm::scale(viewMatrix, glm::vec3(1 + zoomFactor));
        this->update();
    }
    if (key == Qt::Key_Minus) {
        float zoomFactor = 0.1f;
        viewMatrix = glm::scale(viewMatrix, glm::vec3(1 - zoomFactor));
        this->update();
    }
    if (key == Qt::Key_Left) {
        const float rotationSpeed = 0.05f;
        const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
        this->viewMatrix = glm::rotate(viewMatrix, - rotationSpeed, glm::vec3(glm::inverse(viewMatrix) * glm::vec4(cameraUp,0.0f)));
        this->update();
    }
    if (key == Qt::Key_Right) {
        const float rotationSpeed = 0.05f;
        const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
        this->viewMatrix = glm::rotate(viewMatrix, rotationSpeed, glm::vec3(glm::inverse(viewMatrix) * glm::vec4(cameraUp,0.0f)));
        this->update();
    }
    if (key == Qt::Key_Up) {
        const float rotationSpeed = 0.05f;
        const glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
        this->viewMatrix = glm::rotate(viewMatrix, - rotationSpeed, glm::vec3(glm::inverse(viewMatrix) * glm::vec4(cameraRight,0.0f)));
        this->update();
    }
    if (key == Qt::Key_Down) {
        const float rotationSpeed = 0.05f;
        const glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
        this->viewMatrix = glm::rotate(viewMatrix, rotationSpeed, glm::vec3(glm::inverse(viewMatrix) * glm::vec4(cameraRight,0.0f)));
        this->update();
    }
    if (key == Qt::Key_A) {
        const float rotationSpeed = 0.05f;
        const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
        this->viewMatrix = glm::rotate(viewMatrix, rotationSpeed, glm::vec3(glm::inverse(viewMatrix) * glm::vec4(cameraFront,0.0f)));
        this->update();
    }
    if (key == Qt::Key_E) {
        const float rotationSpeed = 0.05f;
        const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
        this->viewMatrix = glm::rotate(viewMatrix, - rotationSpeed, glm::vec3(glm::inverse(viewMatrix) * glm::vec4(cameraFront,0.0f)));
        this->update();
    }
    if (key == Qt::Key_Z) {

        const float translationSpeed = 0.025f * distance;
        const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
        this->viewMatrix = glm::translate(viewMatrix, - translationSpeed * glm::vec3(glm::inverse(viewMatrix) * glm::vec4(cameraUp,0.0f)));
        this->update();
    }
    if (key == Qt::Key_S) {
        const float translationSpeed = 0.025f * distance;
        const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
        this->viewMatrix = glm::translate(viewMatrix, translationSpeed * glm::vec3(glm::inverse(viewMatrix) * glm::vec4(cameraUp,0.0f)));
        this->update();
    }
    if (key == Qt::Key_Q) {
        const float translationSpeed = 0.025f * distance;
        const glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
        this->viewMatrix = glm::translate(viewMatrix, translationSpeed * glm::vec3(glm::inverse(viewMatrix) * glm::vec4(cameraRight,0.0f)));
        this->update();
    }
    if (key == Qt::Key_D) {
        const float translationSpeed = 0.025f * distance;
        const glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
        this->viewMatrix = glm::translate(viewMatrix, - translationSpeed * glm::vec3(glm::inverse(viewMatrix) * glm::vec4(cameraRight,0.0f)));
        this->update();
    }
}

[[maybe_unused]] void OpenGLRenderWidget::addOrUpdateWorldSpaceMeshSlot(const WorldSpaceMesh& worldSpaceMesh, const Color& color){
    const std::string& id = worldSpaceMesh.getId();
    auto iterator = renderModelsMap.find(id);
    if(iterator != renderModelsMap.end()){
        iterator->second->setTransformationMatrix(worldSpaceMesh.getModelTransformation().getMatrix());
        this->update();
    }
    else{
        this->makeCurrent();
        auto model = std::make_shared<RenderModel>(RenderModel(worldSpaceMesh, ambientShader, diffuseShader));
        model->setColor(color);
        if(color.a<1.0){
            model->setCullingEnabled(false); // Don't cull transparent models by default
        }
        renderModelsMap[worldSpaceMesh.getId()] = model;
        sortedRenderModels.emplace_back(model);
        struct {
            bool operator()(const std::shared_ptr<RenderModel>& r1, const std::shared_ptr<RenderModel>& r2) const { return r1->getColor().a > r2->getColor().a; }
        } comparator;
        std::sort(sortedRenderModels.begin(), sortedRenderModels.end(), comparator);
        this->update();
    }
}

void OpenGLRenderWidget::toggleWireframe() {

    this->makeCurrent();
    GLint currentPolygonMode[2];
    GL_CALL(glGetIntegerv(GL_POLYGON_MODE, currentPolygonMode));
    bool wireframeDisabled = currentPolygonMode[0] == GL_FILL;
    for(auto& entry: this->renderModelsMap){
        entry.second->setWireframeEnabled(wireframeDisabled);
    }
    this->update();
    this->parentWidget()->update();
}

void OpenGLRenderWidget::toggleCullFace() {

    this->makeCurrent();
    GL_CALL(GLboolean enabled = glIsEnabled(GL_CULL_FACE));
    for(auto& entry: this->renderModelsMap){
        entry.second->setCullingEnabled(!enabled);
    }
    this->update();
}

void OpenGLRenderWidget::toggleBoundingBoxes() {
    for(auto& entry: this->renderModelsMap){
        entry.second->setBoundingBoxEnabled(!entry.second->isBoundingBoxEnabled());
    }
    this->update();
}

void OpenGLRenderWidget::setLightMode(bool lightMode){
    this->lightMode = lightMode;
    this->makeCurrent();
    if(this->lightMode){
        GL_CALL(glClearColor(1,1,1,1));
    }
    else{
        GL_CALL(glClearColor(0,0,0,1));
    }

    for (const auto &item : this->renderModelsMap){
        if(item.second->getColor().a<1.0){
            item.second->setCullingEnabled(this->lightMode);
        }
    }
    this->parentWidget()->update();
    this->update();
}

[[maybe_unused]] void OpenGLRenderWidget::clearWorldSpaceMeshesSlot() {
    this->renderModelsMap.clear();
    this->sortedRenderModels.clear();
}


[[maybe_unused]] void OpenGLRenderWidget::removeWorldSpaceMeshSlot(const WorldSpaceMesh &worldSpaceMesh) {
    auto iterator = renderModelsMap.find(worldSpaceMesh.getId());
    if(iterator!=renderModelsMap.end()){
        this->sortedRenderModels.erase(std::remove(this->sortedRenderModels.begin(), this->sortedRenderModels.end(), iterator->second), this->sortedRenderModels.end());
        this->renderModelsMap.erase(iterator);
    }
}

void OpenGLRenderWidget::addRenderLine(Vertex vertexA, Vertex vertexB, const Color &color) {
    this->makeCurrent();
    auto line = RenderLine(vertexA, vertexB, glm::mat4(1.0f));
    line.setColor(color);
    this->renderLines.emplace_back(std::move(line));
    this->update();
}

[[maybe_unused]] void OpenGLRenderWidget::addOrUpdateWorldSpaceMesh(const WorldSpaceMesh &worldSpaceMesh, const Color &color) {
    // This way we always execute this function on the application thread
    QMetaObject::invokeMethod(this, "addOrUpdateWorldSpaceMeshSlot", Qt::AutoConnection, Q_ARG(WorldSpaceMesh, worldSpaceMesh), Q_ARG(Color, color));
}

[[maybe_unused]] void OpenGLRenderWidget::clearWorldSpaceMeshes() {
    // This way we always execute this function on the application thread
    QMetaObject::invokeMethod(this, "clearWorldSpaceMeshesSlot", Qt::AutoConnection);
}

[[maybe_unused]] void OpenGLRenderWidget::removeWorldSpaceMesh(const WorldSpaceMesh &worldSpaceMesh) {
    // This way we always execute this function on the application thread
    QMetaObject::invokeMethod(this, "removeWorldSpaceMeshSlot", Qt::AutoConnection, Q_ARG(WorldSpaceMesh, worldSpaceMesh));
}

std::shared_ptr<RenderModel> OpenGLRenderWidget::getRenderModel(const WorldSpaceMesh& worldSpaceMesh) {
    auto iterator = renderModelsMap.find(worldSpaceMesh.getId());
    if(iterator!=renderModelsMap.end()){
        return iterator->second;
    }
    else return nullptr;
}

std::vector<std::shared_ptr<AbstractRenderModel>> &OpenGLRenderWidget::getRenderModels() {
    return renderModels;
}

[[deprecated]]
const std::shared_ptr<QOpenGLShaderProgram> &OpenGLRenderWidget::getAmbientShader() const {
    return ambientShader;
}

void OpenGLRenderWidget::captureScene() {
    this->makeCurrent();
    auto capture = this->grabFramebuffer();;
    QString fileName = QFileDialog::getSaveFileName(this, QString("Save screenshot"), "example", QString("Image Files (*.png *.jpg)"));
    capture.save(fileName);
}

bool OpenGLRenderWidget::isUsePerspective() const {
    return usePerspective;
}

void OpenGLRenderWidget::setUsePerspective(bool usePerspective) {
    OpenGLRenderWidget::usePerspective = usePerspective;
    this->calculateProjectionMatrix();
}

bool OpenGLRenderWidget::isLightMode() const {
    return lightMode;
}

