//
// Created by Jonas on 18/05/2022.
//

#include <qfiledialog.h>
#include "OpenGLWidget.h"
#include "ShaderProgramSource.h"
#include "RenderMesh.h"
#include "RenderWidget.h"

[[maybe_unused]] OpenGLWidget::OpenGLWidget(QWidget *parent): QOpenGLWidget(parent) {}

void OpenGLWidget::initializeGL() {

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

void OpenGLWidget::resetView() {
    viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f, - INITIAL_VIEW_DISTANCE));
    this->update();
}

void OpenGLWidget::resizeGL(int w, int h) {
    this->width = w;
    this->height = h;

    this->calculateProjectionMatrix();
}

void OpenGLWidget::calculateProjectionMatrix(){
    if(this->usePerspective){
        this->projectionMatrix = glm::perspective(glm::radians(INITIAL_FOV), float(width)/float(height), 0.001f, 10000.0f);
    }
    else{
        float halfWidth = float(width)/20.0f;
        float halfHeight = float(height)/20.0f;
        this->projectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1000.0f, 1000.0f);
    }
}

void OpenGLWidget::paintGL() {

    // TODO use Sorted renderModels
//    for(auto& renderModel: this->sortedRenderModels){
//        renderModel->draw(viewMatrix, projectionMatrix, lightMode);
//    }

    for(const auto& [group, groupMap]: this->groupedRenderModelsMap){
        for(const auto& [id, renderModel]: groupMap){
            renderModel->draw(viewMatrix, projectionMatrix, lightMode);
        }
    }

}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
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

void OpenGLWidget::wheelEvent(QWheelEvent *event) {
    auto factor = float(event->angleDelta().y()) / 1200.0f;
    auto distance = glm::length(glm::vec3(viewMatrix[3]));
//    viewMatrix = glm::translate(viewMatrix, glm::vec3(glm::inverse(viewMatrix) * glm::vec4(glm::vec3(0.0f, 0.0f, factor * distance), 0.0f)));

    viewMatrix = glm::scale(viewMatrix, glm::vec3(1 + factor));

    this->update();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event) {
    lastMousePosition = event->pos();
    this->update();
    this->setFocus();
}

void OpenGLWidget::mouseDoubleClickEvent(QMouseEvent *event) {
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

void OpenGLWidget::keyPressEvent(QKeyEvent* event){
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

void OpenGLWidget::toggleWireframe() {

    this->makeCurrent();
    GLint currentPolygonMode[2];
    GL_CALL(glGetIntegerv(GL_POLYGON_MODE, currentPolygonMode));
    bool wireframeDisabled = currentPolygonMode[0] == GL_FILL;
    for(auto& groupEntry: this->groupedRenderModelsMap){
        for (const auto &modelEntry: groupEntry.second){
//            TODO
//            if(auto render/mesh = std::dynamic_pointer_cast<RenderMesh>(modelEntry.second){
//                renderMesh->setWireframeEnabled(wireframeDisabled);
//            }
        }
    }
    this->update();
    this->parentWidget()->update();
}

void OpenGLWidget::toggleCullFace() {

    this->makeCurrent();
    GL_CALL(GLboolean enabled = glIsEnabled(GL_CULL_FACE));
//    TODO
//    for(auto& entry: this->renderModelsMap){
//        entry.second->setCullingEnabled(!enabled);
//    }
    this->update();
}

void OpenGLWidget::toggleBoundingBoxes() {
    // TODO
//    for(auto& entry: this->renderModelsMap){
//        entry.second->setBoundingBoxEnabled(!entry.second->isBoundingBoxEnabled());
//    }
    this->update();
}

void OpenGLWidget::setLightMode(bool newLightMode){
    this->lightMode = newLightMode;
    this->makeCurrent();
    if(this->lightMode){
        GL_CALL(glClearColor(1,1,1,1));
    }
    else{
        GL_CALL(glClearColor(0,0,0,1));
    }

    for (const auto &groupEntry : this->groupedRenderModelsMap){
//        TODO
//        if(i0tem.second->getColor().a<1.0){
//            item.second->setCullingEnabled(this->lightMode);
//        }
    }
    this->parentWidget()->update();
    this->update();
}

void OpenGLWidget::captureScene() {
    this->makeCurrent();
    auto capture = this->grabFramebuffer();
    QString fileName = QFileDialog::getSaveFileName(this, QString("Save screenshot"), "example", QString("Image Files (*.jpg)"));
    std::cout<< fileName.toStdString() << std::endl;
    capture.save(fileName);
}

void OpenGLWidget::captureSceneToFile(const QString& fileName) {
    this->makeCurrent();
    auto capture = this->grabFramebuffer();
    std::cout<< fileName.toStdString() << std::endl;
    capture.save(fileName, nullptr, 100);
}

bool OpenGLWidget::isUsePerspective() const {
    return usePerspective;
}

void OpenGLWidget::setUsePerspective(bool newUsePerspective) {
    OpenGLWidget::usePerspective = newUsePerspective;
    this->calculateProjectionMatrix();
}

bool OpenGLWidget::isLightMode() const {
    return lightMode;
}

std::unordered_map<std::string, std::shared_ptr<AbstractRenderModel>>& OpenGLWidget::getOrInsertRenderModelsMap(const std::string& group) const {

    // Find the group
    auto iterator = groupedRenderModelsMap.find(group);

    // Add new group if not found
    if(iterator == groupedRenderModelsMap.end()){
        iterator = groupedRenderModelsMap.insert({group, {}}).first;
    }
    return iterator->second;
}

void OpenGLWidget::renderWorldSpaceMeshSlot(const std::string &group, const std::shared_ptr<WorldSpaceMesh> &worldSpaceMesh, const Color &color, RenderWidget* renderWidget){

    // Find the group
    auto& renderModelsMap = this->getOrInsertRenderModelsMap(group);

    // Find the model in this group
    auto modelIterator = renderModelsMap.find(worldSpaceMesh->getId());
    if(modelIterator == renderModelsMap.end()){

        // No entry present yet, create new render Model
        auto renderMesh = std::make_shared<RenderMesh>(*worldSpaceMesh, this->ambientShader, this->diffuseShader);

        // Insert it in the renderModelsMap
        modelIterator = renderModelsMap.insert({worldSpaceMesh->getId(), renderMesh}).first;

        // Add listener to redraw when mesh is changed
        const auto listener = std::make_shared<RenderModelListener>();
        listener->setOnChanged([this, renderMesh](){
            this->update();
        });
        renderMesh->addListener(listener);

        // Add control widget to the renderWidget
        renderWidget->addControlWidget(group, renderMesh);

    }

    // Update the color
    modelIterator->second->setColor(color);

    // Update the transformation
    modelIterator->second->setTransformationMatrix(worldSpaceMesh->getModelTransformation().getMatrix());
}

void OpenGLWidget::clear() {
    this->groupedRenderModelsMap.clear();
    this->update();
}

void OpenGLWidget::clearGroup(std::string &group) {
    this->groupedRenderModelsMap.erase(group);
    this->update();
}
