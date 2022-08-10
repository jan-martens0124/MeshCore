//
// Created by Jonas on 29/03/2022.
//

#ifndef OPTIXMESHCORE_RENDERWIDGET_H
#define OPTIXMESHCORE_RENDERWIDGET_H

#include <unordered_map>
#include <iostream>
#include "AbstractRenderModel.h"
#include "../core/WorldSpaceMesh.h"
#include "../acceleration/AABBTree.h"
#include "../tasks/AbstractTaskObserver.h"
#include "OpenGLWidget.h"
#include "../tasks/AbstractTask.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class RenderWidget; }
QT_END_NAMESPACE

class RenderWidget: public QWidget, public AbstractTaskObserver {
Q_OBJECT

public:
    explicit RenderWidget(QWidget *parent = nullptr);
    ~RenderWidget() override;
    [[nodiscard]] OpenGLWidget* getOpenGLWidget() const;

private:
    Ui::RenderWidget *ui;
    std::unordered_map<std::string, QVBoxLayout *> groupLayouts;
    QVBoxLayout * getOrAddGroupLayout(const std::string &group);

public:

    void captureScene();
    void captureSceneToFile(const std::string& fileName);

    void clear();
    void clearGroup(const std::string &group);



    // Render generic objects that extend AbstractRenderModel
    void addOrUpdateRenderModel(const std::string& group, const std::string& id, std::shared_ptr<AbstractRenderModel> renderModel);

    // Render objects with ids like meshes
    void renderWorldSpaceMesh(const std::string &group, const std::shared_ptr<WorldSpaceMesh> &worldSpaceMesh, const Color &color);

    // Render primitives
    // TODO render new object each time, hash object as id? // Not ideal, open question
    void renderBox(const std::string &group, const AABB &aabb, const Transformation& transformation);
//    void renderSphere(const std::string &group, const AABB &aabb, const glm::mat4& transformationMatrix)
    void addControlWidget(const std::string &group, const std::shared_ptr<AbstractRenderModel> &renderModel);

    void observeTask(AbstractTask* task);
    void startCurrentTask();
    void stopCurrentTask();

    void notifyStarted() override;
    void notifyFinished() override;
    void notifyProgress(float progress) override;
    void notifyStatus(const std::string &status) override;
    void notifySolution(const AbstractMeshSolution &solution) override;
private:
    AbstractTask* currentTask = nullptr;
    static Color defaultColors[];
private slots:
    void updateProgressBarSlot(int progress);
    void setStartButtonEnabledSlot(bool enabled);
    void setStopButtonEnabledSlot(bool enabled);
    void setStatusLabelSlot(const QString& status);
};


#endif //OPTIXMESHCORE_RENDERWIDGET_H
