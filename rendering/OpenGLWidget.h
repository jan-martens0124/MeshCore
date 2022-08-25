//
// Created by Jonas on 18/05/2022.
//

#ifndef OPTIXMESHCORE_OPENGLWIDGET_H
#define OPTIXMESHCORE_OPENGLWIDGET_H

#include "AbstractRenderModel.h"
#include "../core/OBB.h"
#include "../core/WorldSpaceMesh.h"
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

class RenderWidget;

class OpenGLWidget: public QOpenGLWidget, protected QOpenGLFunctions {
Q_OBJECT

private:

    bool lightMode = false;
    bool usePerspective = true;
    bool axisEnabled = false;

    int width{};
    int height{};
    QPoint lastMousePosition;

    glm::mat4 viewMatrix{};
    glm::mat4 projectionMatrix{};

    mutable std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<AbstractRenderModel>>> groupedRenderModelsMap;
    std::vector<std::shared_ptr<AbstractRenderModel>> sortedRenderModels;
    std::vector<std::shared_ptr<RenderLine>> axisRenderLines;

private:
    std::shared_ptr<QOpenGLShaderProgram> ambientShader;
    std::shared_ptr<QOpenGLShaderProgram> diffuseShader;
public:
    const std::shared_ptr<QOpenGLShaderProgram> &getAmbientShader() const;
    const std::shared_ptr<QOpenGLShaderProgram> &getDiffuseShader() const;

public:
    [[maybe_unused]] explicit OpenGLWidget(QWidget *parent = nullptr);

public:
    void resetView();

    [[nodiscard]] bool isUsePerspective() const;
    [[nodiscard]] bool isLightMode() const;
    [[nodiscard]] bool isAxisEnabled() const;
    void setUsePerspective(bool newUsePerspective);
    void setLightMode(bool newLightMode);
    void setAxisEnabled(bool enabled);


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
    void clearGroup(const std::string &group);
    void renderWorldSpaceMeshSlot(const std::string &group, const std::shared_ptr<WorldSpaceMesh> &worldSpaceMesh, const Color &color, RenderWidget* renderWidget);
    void renderBoxSlot(const std::string &group, const AABB &aabb, const Transformation& transformation, RenderWidget* renderWidget);
    void addOrUpdateRenderModelSlot(const std::string& group, const std::string& id, std::shared_ptr<AbstractRenderModel> sharedPtr, RenderWidget* renderWidget);
    void captureSceneSlot();
    void captureSceneToFileSlot(const QString& fileName);

private:
    std::unordered_map<std::string, std::shared_ptr<AbstractRenderModel>>& getOrInsertRenderModelsMap(const std::string &group) const;
    void updateSortedRenderModels();
};


#endif //OPTIXMESHCORE_OPENGLWIDGET_H
