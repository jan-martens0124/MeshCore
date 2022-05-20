//
// Created by Jonas on 18/05/2022.
//

#ifndef OPTIXMESHCORE_OPENGLWIDGET_H
#define OPTIXMESHCORE_OPENGLWIDGET_H

#include "RenderModel.h"
#include "RenderLine.h"
#include "AbstractRenderModel.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <glm/glm.hpp>
#include <unordered_map>
#include <QVBoxLayout>

#define INITIAL_VIEW_DISTANCE 50.0f
#define INITIAL_FOV 86.0f

class RenderWidget;

class OpenGLWidget: public QOpenGLWidget, protected QOpenGLFunctions {
Q_OBJECT
private:

    bool lightMode = false;
    bool usePerspective = true;
    int width{};
    int height{};
    QPoint lastMousePosition;

    glm::mat4 viewMatrix{};
    glm::mat4 projectionMatrix{};

    mutable std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<AbstractRenderModel>>> groupedRenderModelsMap;
//    std::vector<std::shared_ptr<RenderModel>> sortedRenderModels;

private:
    std::vector<RenderLine> renderLines;
    std::shared_ptr<QOpenGLShaderProgram> ambientShader;
    std::shared_ptr<QOpenGLShaderProgram> diffuseShader;

public:
    [[maybe_unused]] explicit OpenGLWidget(QWidget *parent = nullptr);

public:
    void resetView();
    void toggleWireframe();
    void toggleCullFace();
    void toggleBoundingBoxes();


    [[nodiscard]] bool isUsePerspective() const;
    [[nodiscard]] bool isLightMode() const;


    void setUsePerspective(bool newUsePerspective);
    void setLightMode(bool newLightMode);

    void captureScene();
    void captureSceneToFile(const QString& fileName);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void calculateProjectionMatrix();

    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void clear();
    void clearGroup(std::string &group);
    void renderWorldSpaceMeshSlot(const std::string &group, const std::shared_ptr<WorldSpaceMesh> &worldSpaceMesh, const Color &color, RenderWidget* renderWidget);



private:
    std::unordered_map<std::string, std::shared_ptr<AbstractRenderModel>>& getOrInsertRenderModelsMap(const std::string &group) const;
};


#endif //OPTIXMESHCORE_OPENGLWIDGET_H
