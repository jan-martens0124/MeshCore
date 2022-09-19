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
    void captureSceneToFile(const std::string& fileName);

    void clear();
    void clearGroup(const std::string &group);



    // Render generic objects that extend AbstractRenderModel
    // TODO this would imply that programmers make their own render models, which can't happen outside the main thread...
    void addOrUpdateRenderModel(const std::string& group, const std::string& id, std::shared_ptr<AbstractRenderModel> renderModel);

    // Render objects with ids like meshes
    void renderWorldSpaceMesh(const std::string &group, const std::shared_ptr<WorldSpaceMesh> &worldSpaceMesh, const Color& color = Color(1.0f));

//    template<class Bounds, unsigned int Degree>
//    void renderBoundsTree(const std::string &group, const std::shared_ptr<AbstractBoundsTree<OBB, 2>> &boundsTree, const Color &color){
//        QMetaObject::invokeMethod(this->getOpenGLWidget(), "renderBoundsTreeSlot",
//                                  Qt::AutoConnection,
//                                  Q_ARG(std::string, group),
//                                  Q_ARG(std::shared_ptr<AbstractBoundsTree<OBB, 2>>, boundsTree), // We should copy the actual worldSpaceMesh object here, otherwise the transformation could change before the render thread reads it
//                                  Q_ARG(Color, color),
//                                  Q_ARG(RenderWidget*, this));
//    }

    // TODO this separation should happen in OPENGLWidget HAHHA SHOULD IT???
//    template<unsigned int Degree>
//    void renderBoundsTree(const std::string &group, const std::shared_ptr<AbstractBoundsTree<AABB, Degree>> &boundsTree, const Color &color);
//
//    template<unsigned int Degree>
//    void renderBoundsTree(const std::string &group, const std::shared_ptr<AbstractBoundsTree<OBB, Degree>> &boundsTree, const Color &color);
//
//    template<unsigned int Degree>
//    void renderBoundsTree(const std::string &group, const std::shared_ptr<AbstractBoundsTree<Sphere, Degree>> &boundsTree, const Color &color);

    // Render primitives
    // We need to provide this methods because users can't create render models outside of the main thread
    // TODO render new object each time or use hashes // Not ideal, open question... let the user provide a hash or id by their choice
    template<unsigned int Degree>
    void renderAABBTree(const std::string &group, const std::shared_ptr<AABBTree<Degree>> &aabbTree, const Color &color);

    void renderBox(const std::string &group, const AABB &aabb, const Transformation& transformation=Transformation());
    void renderSphere(const std::string &group, const Sphere &sphere, const Transformation& transformation);
    void renderTriangle(const std::string &group, const VertexTriangle &triangle, const Transformation& transformation);
//    void renderLine(const std::string &group, const Sphere &sphere, const Transformation& transformation);
//    void renderSphere(const std::string &group, const AABB &aabb, const glm::mat4& transformationMatrix)
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
    std::function<void(RenderWidget* renderWidget, std::shared_ptr<const AbstractSolution> solution)> onSolutionNotified  = {};
    static Color defaultColors[];
private slots:
    void updateProgressBarSlot(int progress);
    void setStartButtonEnabledSlot(bool enabled);
    void setStopButtonEnabledSlot(bool enabled);
    void setStatusLabelSlot(const QString& status);
};

#endif //OPTIXMESHCORE_RENDERWIDGET_H
