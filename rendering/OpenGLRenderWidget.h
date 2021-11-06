//
// Created by Jonas on 30/11/2020.
//

#ifndef MESHCORE_OPENGLRENDERWIDGET_H
#define MESHCORE_OPENGLRENDERWIDGET_H

#include "RenderModel.h"
#include "RenderLine.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <glm/glm.hpp>
#include <unordered_map>
#include <QVBoxLayout>

#define INITIAL_VIEW_DISTANCE 50.0f
#define INITIAL_FOV 86.0f

Q_DECLARE_METATYPE(Color)
Q_DECLARE_METATYPE(WorldSpaceMesh)
Q_DECLARE_METATYPE(Vertex)

class OpenGLRenderWidget: public QOpenGLWidget, protected QOpenGLFunctions {
Q_OBJECT
private:

    bool lightMode{};
    int width{};
    int height{};
    QPoint lastMousePosition;

    glm::mat4 viewMatrix{};
    glm::mat4 projectionMatrix{};

    std::unordered_map<std::string, std::shared_ptr<RenderModel>> renderModelsMap;
    std::vector<std::shared_ptr<RenderModel>> sortedRenderModels;

private:
    std::vector<RenderLine> renderLines;
    std::shared_ptr<QOpenGLShaderProgram> ambientShader;
    std::shared_ptr<QOpenGLShaderProgram> diffuseShader;

public:
    [[maybe_unused]] explicit OpenGLRenderWidget(QWidget *parent = nullptr);

public:
    void resetView();
    void toggleWireframe();
    void toggleCullFace();
    void toggleLightMode();

public:
    friend class ApplicationWindow;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

public:
    void addRenderLine(Vertex vertexA, Vertex vertexB, const Color &color); // TODO remove for now?
    void addOrUpdateWorldSpaceMesh(const WorldSpaceMesh& worldSpaceMesh, const Color& color);
    std::shared_ptr<RenderModel> getRenderModel(const WorldSpaceMesh& worldSpaceMesh);
    void removeWorldSpaceMesh(const WorldSpaceMesh& worldSpaceMesh);
    void clearWorldSpaceMeshes();

private slots:
    void addOrUpdateWorldSpaceMeshSlot(const WorldSpaceMesh &worldSpaceMesh, const Color &color);
    void removeWorldSpaceMeshSlot(const WorldSpaceMesh &worldSpaceMesh);
    void clearWorldSpaceMeshesSlot();

};

#endif //MESHCORE_OPENGLRENDERWIDGET_H
