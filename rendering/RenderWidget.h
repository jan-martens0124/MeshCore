//
// Created by Jonas on 29/03/2022.
//

#ifndef OPTIXMESHCORE_RENDERWIDGET_H
#define OPTIXMESHCORE_RENDERWIDGET_H

#include <QApplication>
#include <unordered_map>
#include <iostream>
#include "AbstractRenderModel.h"
#include "../core/WorldSpaceMesh.h"
#include "../tasks/AbstractTaskObserver.h"
#include "RenderBoundsTree.h"
#include "OpenGLWidget.h"
#include "../tasks/AbstractTask.h"
#include "../acceleration/AbstractBoundsTree.h"
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
    void captureAnimation();
    void captureSceneToFile(const std::string& fileName);

    void clear();
    void clearGroup(const std::string &group);

private:
    // Render generic objects that extend AbstractRenderModel
    // This would imply that programmers make their own render models, which can't happen outside the main thread, this is not a good practice
    void addOrUpdateRenderModel(const std::string& group, const std::string& id, std::shared_ptr<AbstractRenderModel> renderModel);

public:
    // Render objects with ids like meshes
    void renderWorldSpaceMesh(const std::string &group, const std::shared_ptr<WorldSpaceMesh> &worldSpaceMesh, const Color& color = Color(1.0f));

    template<class BoundsTree>
    void renderBoundsTree(const std::string &group, const std::string& name, const std::shared_ptr<BoundsTree>& boundsTree, const Transformation& transformation, const Color &color=Color::White()){
        QMetaObject::invokeMethod(qApp, [group, color, name, transformation, boundsTree, this](){
            auto renderModel = std::make_shared<RenderBoundsTree>(*boundsTree, transformation, this->getOpenGLWidget()->getAmbientShader(), this->getOpenGLWidget()->getDiffuseShader());
            renderModel->setName(name);
            renderModel->setColor(color);
            this->addOrUpdateRenderModel(group, name, renderModel); // TODO replace name with actual id
        });
    }

    void renderBox(const std::string &group, const std::string& name, const AABB &aabb, const Transformation& transformation=Transformation(), const Color& = Color::White());
    void renderSphere(const std::string &group, const std::string& name, const Sphere &sphere, const Color &color = Color::White());
    void renderTriangle(const std::string &group, const std::string& name, const VertexTriangle &triangle, const Color &color = Color::White());
    void renderLine(const std::string &group, const Vertex &vertexA, const Vertex &vertexB, const Color &color = Color::White());
    void addControlWidget(const std::string &group, const std::shared_ptr<AbstractRenderModel> &renderModel);

    void observeTask(AbstractTask* task, const std::function<void(RenderWidget* renderWidget, std::shared_ptr<const AbstractSolution> solution)>& onSolutionNotified);
    void startCurrentTask();
    void stopCurrentTask();

    void notifyStarted() override;
    void notifyFinished() override;
    void notifyProgress(float progress) override;
    void notifyStatus(const std::string &status) override;
    void notifySolution(const std::shared_ptr<const AbstractSolution>& solution) override;
private:
    AbstractTask* currentTask = nullptr;
    std::function<void(RenderWidget* renderWidget, const std::shared_ptr<const AbstractSolution>& solution)> onSolutionNotified  = {};
    std::thread timerThread{};
    std::atomic<boolean> taskRunning = false;
private slots:
    [[maybe_unused]] void updateProgressBarSlot(int progress);
    [[maybe_unused]] void setStartButtonEnabledSlot(bool enabled);
    [[maybe_unused]] void setStopButtonEnabledSlot(bool enabled);
    [[maybe_unused]] void setStatusLabelSlot(const QString& status);
    [[maybe_unused]] void setTimeLabelSlot(const QString& time);
};

#endif //OPTIXMESHCORE_RENDERWIDGET_H
