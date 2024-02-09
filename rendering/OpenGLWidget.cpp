//
// Created by Jonas on 18/05/2022.
//

#include <qfiledialog.h>
#include "OpenGLWidget.h"
#include "ShaderProgramSource.h"
#include "RenderMesh.h"
#include "RenderSphere.h"
#include "RenderWidget.h"
#include "Exception.h"
#include "../external/gifencoder/GifEncoder.h"
#include <glm/gtx/hash.hpp>
#include <QProgressDialog>

[[maybe_unused]] OpenGLWidget::OpenGLWidget(QWidget *parent): QOpenGLWidget(parent) {}

Q_DECLARE_METATYPE(Color);
Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(std::shared_ptr<WorldSpaceMesh>)
Q_DECLARE_METATYPE(RenderWidget*)
Q_DECLARE_METATYPE(AABB)
Q_DECLARE_METATYPE(Sphere)
Q_DECLARE_METATYPE(Transformation)
Q_DECLARE_METATYPE(glm::vec3)

void OpenGLWidget::initializeGL() {

    this->resetView();

    initializeOpenGLFunctions();

    qRegisterMetaType<Color>();
    qRegisterMetaType<std::string>();
    qRegisterMetaType<std::shared_ptr<WorldSpaceMesh>>();
    qRegisterMetaType<RenderWidget*>();
    qRegisterMetaType<AABB>();
    qRegisterMetaType<Transformation>();
    qRegisterMetaType<glm::vec3>();
    qRegisterMetaType<Sphere>();

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

    // Store the axis render line
    axisRenderLines.emplace_back(std::make_shared<RenderLine>(glm::vec3(0,0,0), glm::vec3(1e8,0,0), Transformation(), ambientShader));
    axisRenderLines.emplace_back(std::make_shared<RenderLine>(glm::vec3(0,0,0), glm::vec3(0,1e8,0), Transformation(), ambientShader));
    axisRenderLines.emplace_back(std::make_shared<RenderLine>(glm::vec3(0,0,0), glm::vec3(0,0,1e8), Transformation(), ambientShader));
    axisRenderLines[0]->setColor(Color(1,0,0,1));
    axisRenderLines[1]->setColor(Color(0,1,0,1));
    axisRenderLines[2]->setColor(Color(0,0,1,1));
}

void OpenGLWidget::resetView() {
    viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-INITIAL_VIEW_DISTANCE, 0.0f, 0.0f));

    // Default rotation: y-axis to the right and z-axis upwards
    viewMatrix = glm::mat4({0, 0, 1, 0,
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 0, 1
                            }) * viewMatrix;

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

    if(this->axisEnabled){
        for (const auto &axisRenderLine: this->axisRenderLines){
            axisRenderLine->draw(viewMatrix, projectionMatrix, lightMode);
        }
    }

    for(auto& renderModel: this->sortedRenderModels){
        renderModel->draw(viewMatrix, projectionMatrix, lightMode);
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
        for (const auto &modelEntry: groupEntry.second){
            if(auto renderMesh = std::dynamic_pointer_cast<RenderMesh>(modelEntry.second)){
                if(renderMesh->getColor().a<1.0){
                    renderMesh->setCullingEnabled(this->lightMode);
                }
            }
        }
    }
    this->parentWidget()->update();
    this->update();
}

void OpenGLWidget::captureAnimationSlot() {

    this->makeCurrent();
    auto initialViewMatrix = this->viewMatrix;
    QString fileName = QFileDialog::getSaveFileName(this, QString("Save animation"), "example", QString("GIF (*.gif)"));

    // Check if the user has selected a file
    if (fileName.isEmpty()) {
        return;
    }

    // Calculate how many steps we need to take to get a full rotation, in radians
    const float fullRotation = 2 * glm::pi<float>();
    int delay = 5;
    int steps = 200;
    const float rotationSpeed = fullRotation / float(steps);
    const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);

    // Show processing bar
    QProgressDialog progressDialog("Exporting GIF animation...", "Abort", 0, steps, this);
    progressDialog.setWindowModality(Qt::ApplicationModal);
    progressDialog.show();

    // Allocate a pixel buffer
    std::vector<uint8_t> pixels(width * height * 4, 0);

    // Initialise the gif encoder
    int quality = 10;
    bool useGlobalColorMap = false;
    int loop = 0;
    int preAllocSize = width * height * 3;
    GifEncoder gifEncoder;
    if (!gifEncoder.open(fileName.toStdString(), width, height, quality, useGlobalColorMap, loop, preAllocSize)) {
        fprintf(stderr, "Error opening the gif file\n");
    }

    for(int i=0;i<steps;i++){

        // Check if aborted
        if(progressDialog.wasCanceled()){
            if (!gifEncoder.close()) {
                fprintf(stderr, "Error closing the gif file\n");
            }
            viewMatrix = initialViewMatrix;
            GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
            this->update();
            QFile::remove(fileName);
            progressDialog.close();
            return;
        }

        progressDialog.setValue(i);

        this->viewMatrix = glm::rotate(viewMatrix, - rotationSpeed, glm::vec3(glm::inverse(viewMatrix) * glm::vec4(cameraUp,0.0f)));
        QOpenGLFunctions::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        this->update();
        auto capture = this->grabFramebuffer();
        for (int x = 0; x < width; ++x){
            for (int y = 0; y < height; ++y){
                auto rgb = capture.pixel(x, y);
                pixels[4 * (y * width + x) + 0] = (rgb >> 16) & 0xFF;
                pixels[4 * (y * width + x) + 1] = (rgb >> 8) & 0xFF;
                pixels[4 * (y * width + x) + 2] = (rgb >> 0) & 0xFF;
                pixels[4 * (y * width + x) + 3] = 255;
            }
        }
        gifEncoder.push(GifEncoder::PIXEL_FORMAT_RGBA, pixels.data(), width, height, delay);
    }
    progressDialog.setValue(steps);

    // Close the file handle and free memory.
    if (!gifEncoder.close()) {
        fprintf(stderr, "Error closing gif file\n");
    }
    viewMatrix = initialViewMatrix;
    QOpenGLFunctions::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    this->update();
    progressDialog.close();
}

void OpenGLWidget::captureSceneSlot() {

    this->makeCurrent();

    // We grab the frame before the user inputs the file name
    QOpenGLFunctions::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    this->update();
    auto capture = this->grabFramebuffer();

    QString fileName = QFileDialog::getSaveFileName(this, QString("Save screenshot"), "example", QString("Image Files (*.png)"));

    // Check if the user has selected a file
    if (fileName.isEmpty()) {
        return;
    }

    // Eliminate the alpha channel
    QImage rgbImage(capture.size(), QImage::Format_ARGB32);
    for (int x = 0; x < width; ++x){
        for (int y = 0; y < height; ++y){
            auto rgb = capture.pixel(x, y);
            rgbImage.setPixelColor(x,y, QColor(rgb));
        }
    }
    rgbImage.save(fileName);
}

void OpenGLWidget::captureSceneToFileSlot(const QString& fileName) {
    this->makeCurrent();
    QOpenGLFunctions::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    this->update();
    auto capture = this->grabFramebuffer();
    std::cout<< fileName.toStdString() << std::endl;

    // Eliminate the alpha channel
    QImage rgbImage(capture.size(), QImage::Format_ARGB32);
    for (int x = 0; x < width; ++x){
        for (int y = 0; y < height; ++y){
            auto rgb = capture.pixel(x, y);
            rgbImage.setPixelColor(x,y, QColor(rgb));
        }
    }
    rgbImage.save(fileName);
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

bool OpenGLWidget::isAxisEnabled() const {
    return axisEnabled;
}

void OpenGLWidget::setAxisEnabled(bool enabled) {
    OpenGLWidget::axisEnabled = enabled;
    this->update();
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

        this->makeCurrent();

        // No entry present yet, create new render Model
        auto renderMesh = std::make_shared<RenderMesh>(*worldSpaceMesh, this->ambientShader, this->diffuseShader);

        // Insert it in the renderModelsMap
        modelIterator = renderModelsMap.insert({worldSpaceMesh->getId(), renderMesh}).first;

        // Add required listeners
        this->addRenderModelListeners(group, renderMesh);

        // Add control widget to the renderWidget
        renderWidget->addControlWidget(group, renderMesh);

        this->updateSortedRenderModels();
    }

    // Set the color
    modelIterator->second->setColor(color);

    // Update the transformation
    modelIterator->second->setTransformation(worldSpaceMesh->getModelTransformation());

    this->update();
}

void OpenGLWidget::clear() {
    this->groupedRenderModelsMap.clear();
    this->updateSortedRenderModels();
    this->update();
}

void OpenGLWidget::clearGroup(const std::string &group) {
    this->groupedRenderModelsMap.erase(group);
    this->updateSortedRenderModels();
    this->update();
}

void OpenGLWidget::setGroupVisible(const std::string &group, bool visible) {
    for (const auto &modelEntry: this->groupedRenderModelsMap[group]){
        modelEntry.second->setVisible(visible);
    }
    this->update();
}

void OpenGLWidget::updateSortedRenderModels(){

    // Add all models to the vector
    this->sortedRenderModels.clear();
    for(auto& groupEntry: this->groupedRenderModelsMap){
        for (const auto &renderModelEntry: groupEntry.second){
            this->sortedRenderModels.emplace_back(renderModelEntry.second);
        }
    }

    // Sort them by transparency
    std::sort(this->sortedRenderModels.begin(), this->sortedRenderModels.end(), [](const std::shared_ptr<AbstractRenderModel>& a, const std::shared_ptr<AbstractRenderModel>& b){
        return a->getColor().a > b->getColor().a;
    });
}

void OpenGLWidget::renderBoxSlot(const std::string &group, const std::string &name, const AABB &aabb, const Transformation& transformation, const Color& color, RenderWidget *renderWidget) {

    // Find the group
    auto& renderModelsMap = this->getOrInsertRenderModelsMap(group);

    // Find the model in this group
    auto hash = std::hash<AABB>{}(aabb);
    glm::detail::hash_combine(hash, std::hash<glm::mat4>{}(transformation.getMatrix()));
    auto renderId = std::to_string(std::hash<AABB>{}(aabb));
    auto modelIterator = renderModelsMap.find(renderId);

    // Create a new model if not found
    if(modelIterator == renderModelsMap.end()){

        this->makeCurrent();

        // No entry present yet, create new render Model
        auto renderAABB = std::make_shared<RenderAABB>(aabb, transformation, this->ambientShader);

        // Insert it in the renderModelsMap
        modelIterator = renderModelsMap.insert({renderId, renderAABB}).first;

        // Add required listeners
        this->addRenderModelListeners(group, renderAABB);

        // Add control widget to the renderWidget
        renderWidget->addControlWidget(group, renderAABB);

        // Set the color
        modelIterator->second->setColor(Color(1,1,1,1));
        modelIterator->second->setName(name);

        this->updateSortedRenderModels();
    }

    // Update the color
    modelIterator->second->setColor(color);

    // Update the transformation
    modelIterator->second->setTransformation(transformation);

    this->update();

}

void OpenGLWidget::renderSphereSlot(const std::string &group, const std::string& name, const Sphere &sphere, const Color& color, RenderWidget *renderWidget) {

    // Find the group
    auto& renderModelsMap = this->getOrInsertRenderModelsMap(group);

    // Find the model in this group
    auto renderId = std::to_string(std::hash<Sphere>{}(sphere));
    auto modelIterator = renderModelsMap.find(renderId);

    // Create a new model if not found
    if(modelIterator == renderModelsMap.end()){

        this->makeCurrent();

        // No entry present yet, create new render Model
        auto renderSphere = std::make_shared<RenderSphere>(sphere, Transformation(), this->ambientShader, this->diffuseShader);

        // Insert it in the renderModelsMap
        modelIterator = renderModelsMap.insert({renderId, renderSphere}).first;

        // Add required listeners
        this->addRenderModelListeners(group, renderSphere);

        // Add control widget to the renderWidget
        renderWidget->addControlWidget(group, renderSphere);

        // Set the color
        modelIterator->second->setColor(color);
        modelIterator->second->setName(name);

        this->updateSortedRenderModels();
    }

    // Update the transformation
    modelIterator->second->setTransformation(Transformation());

    this->update();

}

void OpenGLWidget::renderTriangleSlot(const std::string &group, const std::string &name, const VertexTriangle &triangle,
                                      const Color &color, RenderWidget *renderWidget) {

    // Find the group
    auto& renderModelsMap = this->getOrInsertRenderModelsMap(group);

    // Find the model in this group
    auto renderId = std::to_string(std::hash<VertexTriangle>{}(triangle));
    auto modelIterator = renderModelsMap.find(renderId);

    // Create a new model if not found
    if(modelIterator == renderModelsMap.end()){

        this->makeCurrent();

        // No entry present yet, create new render Model
//        auto renderTriangle = std::make_shared<RenderTriangle>(triangle, Transformation(), this->ambientShader, this->diffuseShader);
        // TODO create a renderTriangle or handle it as a mesh
    }

    // Update the transformation
    modelIterator->second->setTransformation(Transformation());

    this->update();
}

void OpenGLWidget::renderLineSlot(const std::string &group, const std::string &name, const Vertex &start,
                                  const Vertex &end, const Color &color, RenderWidget *renderWidget) {

    // Find the group
    auto& renderModelsMap = this->getOrInsertRenderModelsMap(group);

    // Find the model in this group
    auto renderId = std::to_string(std::hash<glm::vec3>{}(start)) + std::to_string(std::hash<glm::vec3>{}(end));
    auto modelIterator = renderModelsMap.find(renderId);

    // Create a new model if not found
    if(modelIterator == renderModelsMap.end()){

        this->makeCurrent();

        // No entry present yet, create new render Model
        auto renderLine = std::make_shared<RenderLine>(start, end, Transformation(), this->ambientShader);

        // Insert it in the renderModelsMap
        modelIterator = renderModelsMap.insert({renderId, renderLine}).first;

        // Add required listeners
        this->addRenderModelListeners(group, renderLine);

        // Add control widget to the renderWidget
        renderWidget->addControlWidget(group, renderLine);

        // Set the color
        modelIterator->second->setColor(color);
        modelIterator->second->setName(name);

        this->updateSortedRenderModels();
    }

    // Update the transformation
    modelIterator->second->setTransformation(Transformation());

    this->update();

}

const std::shared_ptr<QOpenGLShaderProgram> &OpenGLWidget::getAmbientShader() const {
    return ambientShader;
}

const std::shared_ptr<QOpenGLShaderProgram> &OpenGLWidget::getDiffuseShader() const {
    return diffuseShader;
}

void OpenGLWidget::addOrUpdateRenderModelSlot(const std::string& group, const std::string& id, std::shared_ptr<AbstractRenderModel> renderModel, RenderWidget* renderWidget) {
    // Find the group
    auto& renderModelsMap = this->getOrInsertRenderModelsMap(group);

    // Find the model in this group
    auto modelIterator = renderModelsMap.find(id);
    if(modelIterator == renderModelsMap.end()){

        this->makeCurrent();

        // No entry present yet, insert new renderModel it in the renderModelsMap
        modelIterator = renderModelsMap.insert({id, renderModel}).first;

        // Add required listeners
        this->addRenderModelListeners(group, renderModel);

        // Add control widget to the renderWidget
        renderWidget->addControlWidget(group, renderModel);

        this->updateSortedRenderModels();
    }

    modelIterator->second->setTransformation(renderModel->getTransformation());

    this->update();
}

void OpenGLWidget::addGroupListener(const std::string &group, const std::shared_ptr<AbstractRenderGroupListener> &listener) {
    this->groupListeners[group].emplace_back(listener);
}

void OpenGLWidget::removeGroupListener(const std::string &group,const std::shared_ptr<AbstractRenderGroupListener> &listener) {
    this->groupListeners[group].erase(std::remove(this->groupListeners[group].begin(), this->groupListeners[group].end(), listener), this->groupListeners[group].end());
}

void OpenGLWidget::addRenderModelListeners(const std::string &group, const std::shared_ptr<AbstractRenderModel> &renderModel) {

    // Add listener to redraw when model is changed
    const auto listener = std::make_shared<SimpleRenderModelListener>();
    listener->setOnColorChanged([this](const Color& oldColor, const Color& newColor){
        if(oldColor.a!=newColor.a){
            this->updateSortedRenderModels();
        }
    });

    // Check visibility of all models in this group when changed
    auto onVisibleChanged = [this, group](bool oldVisible, bool newVisible){
        // Test if all models in this group have equal visibility
        bool uniform = true;
        for (const auto &renderModelInGroup: this->groupedRenderModelsMap[group]){
            if(renderModelInGroup.second->isVisible() != newVisible){
                uniform = false;
            }
        }

        // Notify group listeners accordingly
        for (const auto &groupListener: this->groupListeners[group]){
            groupListener->notifyVisible(newVisible, uniform);
        }
    };
    onVisibleChanged(renderModel->isVisible(), renderModel->isVisible()); // Trigger this listener once to keep group listeners in sync
    listener->setOnVisibleChanged(onVisibleChanged);

    listener->setOnChanged([this](){
        this->update();
    });
    renderModel->addListener(listener);
}

int OpenGLWidget::getWidth() const {
    return width;
}

int OpenGLWidget::getHeight() const {
    return height;
}

std::vector<uint8_t> OpenGLWidget::capturePixelBufferSlot() {

    this->makeCurrent();
    std::vector<uint8_t> pixelBuffer(width * height * 4, 0);
    QOpenGLFunctions::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    this->update();
    auto capture = this->grabFramebuffer();
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            auto rgb = capture.pixel(x, y);
            pixelBuffer[4 * (y * width + x) + 0] = (rgb >> 16) & 0xFF;
            pixelBuffer[4 * (y * width + x) + 1] = (rgb >> 8) & 0xFF;
            pixelBuffer[4 * (y * width + x) + 2] = (rgb >> 0) & 0xFF;
            pixelBuffer[4 * (y * width + x) + 3] = 255;
        }

    }
    return pixelBuffer;
}

void OpenGLWidget::captureLinearAnimationSlot(const std::string& group, std::shared_ptr<WorldSpaceMesh>& object, const Transformation& initialTransformation, const Transformation& finalTransformation, const Color& initialColor, const Color& finalColor, const QString& fileName, int steps, int delay, RenderWidget* renderWidget){

    // Animation time (s) = delay * steps / 100
    this->makeCurrent();

    // Show processing bar
    QProgressDialog progressDialog("Exporting " + fileName + " animation...", "Abort", 0, steps, this);
    progressDialog.setWindowModality(Qt::ApplicationModal);
    progressDialog.show();

    // Allocate a pixel buffer
    std::vector<uint8_t> pixels(width * height * 4, 0);

    // Initialise the gif encoder
    int quality = 10;
    bool useGlobalColorMap = false;
    int loop = 0;
    int preAllocSize = width * height * 3;
    GifEncoder gifEncoder;
    if (!gifEncoder.open(fileName.toStdString(), width, height, quality, useGlobalColorMap, loop, preAllocSize)) {
        fprintf(stderr, "Error opening the gif file\n");
    }

    for(int i=0;i<steps;i++){

        // Check if aborted
        if(progressDialog.wasCanceled()){
            if (!gifEncoder.close()) {
                fprintf(stderr, "Error closing the gif file\n");
            }
            GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
            this->update();
            QFile::remove(fileName);
            progressDialog.close();
            return;
        }

        progressDialog.setValue(i);

        // Do the linear interpolation between transformations, color and viewMatrix
        float t = float(i)/(steps - 1); // Linear interpolation
        object->setModelTransformation(Transformation::interpolate(initialTransformation, finalTransformation, t));
        auto color = (1.0f - t) * initialColor + t * finalColor;
        // TODO interpolate viewMatrix (Should this be a Transformation object as well?)
//        openGLWidget.viewMatrix;this->viewMatrix = glm::rotate(viewMatrix, - rotationSpeed, glm::vec3(glm::inverse(viewMatrix) * glm::vec4(cameraUp,0.0f)));

        renderWorldSpaceMeshSlot(group, object, {color.r, color.g, color.b, color.a}, renderWidget); // Stupid that this conversion is required

        QOpenGLFunctions::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        this->update();
        auto capture = this->grabFramebuffer();
        for (int x = 0; x < width; ++x){
            for (int y = 0; y < height; ++y){
                auto rgb = capture.pixel(x, y);
                pixels[4 * (y * width + x) + 0] = (rgb >> 16) & 0xFF;
                pixels[4 * (y * width + x) + 1] = (rgb >> 8) & 0xFF;
                pixels[4 * (y * width + x) + 2] = (rgb >> 0) & 0xFF;
                pixels[4 * (y * width + x) + 3] = 255;
            }
        }
        gifEncoder.push(GifEncoder::PIXEL_FORMAT_RGBA, pixels.data(), width, height, delay);
    }
    progressDialog.setValue(steps);

    // Close the file handle and free memory.
    if (!gifEncoder.close()) {
        fprintf(stderr, "Error closing the gif file\n");
    }
    QOpenGLFunctions::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    this->update();
    progressDialog.close();
}