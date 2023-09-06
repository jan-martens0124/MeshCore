//
// Created by Jonas on 18/05/2022.
//

#ifndef OPTIXMESHCORE_OPENGLWIDGET_H
#define OPTIXMESHCORE_OPENGLWIDGET_H

#include "AbstractRenderModel.h"
#include "../core/OBB.h"
#include "../core/Sphere.h"
#include "../core/WorldSpaceMesh.h"
#include "RenderLine.h"
#include "../core/VertexTriangle.h"
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
    std::unordered_map<std::string, std::vector<std::shared_ptr<AbstractRenderGroupListener>>> groupListeners;
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
    void addGroupListener(const std::string& group, const std::shared_ptr<AbstractRenderGroupListener>& listener);
    void removeGroupListener(const std::string& group, const std::shared_ptr<AbstractRenderGroupListener>& listener);

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
    void setGroupVisible(const std::string &group, bool visible);
    void renderWorldSpaceMeshSlot(const std::string &group, const std::shared_ptr<WorldSpaceMesh> &worldSpaceMesh, const Color &color, RenderWidget* renderWidget);
    void renderBoxSlot(const std::string &group, const std::string &name, const AABB &aabb, const Transformation& transformation, const Color& color, RenderWidget *renderWidget);
    void renderSphereSlot(const std::string &group, const std::string &name, const Sphere &sphere, const Color& color, RenderWidget* renderWidget);
    void renderTriangleSlot(const std::string &group, const std::string &name, const VertexTriangle &triangle, const Color& color, RenderWidget* renderWidget);
    void renderLineSlot(const std::string &group, const std::string &name, const glm::vec3 &start, const glm::vec3 &end, const Color& color, RenderWidget* renderWidget);

    void addOrUpdateRenderModelSlot(const std::string& group, const std::string& id, std::shared_ptr<AbstractRenderModel> sharedPtr, RenderWidget* renderWidget);
    void captureSceneSlot();
    void captureSceneToFileSlot(const QString& fileName);
    void captureAnimationSlot();

private:
    std::unordered_map<std::string, std::shared_ptr<AbstractRenderModel>>& getOrInsertRenderModelsMap(const std::string &group) const;
    void updateSortedRenderModels();
    void addRenderModelListeners(const std::string& group, const std::shared_ptr<AbstractRenderModel>& renderModel);
};


#endif //OPTIXMESHCORE_OPENGLWIDGET_H
