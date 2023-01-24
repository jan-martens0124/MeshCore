//
// Created by Jonas on 29/03/2022.
//

#include "RenderWidget.h"
#include <QtWidgets>
#include <iomanip>
#include "forms/ui_renderwidget.h"
#include "RenderModelControlWidget.h"

RenderWidget::RenderWidget(QWidget *parent):
    QWidget(parent), ui(new Ui::RenderWidget)
    {
        ui->setupUi(this);
        ui->progressBar->setMinimumWidth(350);

        this->ui->taskSection->setVisible(false);

        // Connect the start and stop buttons
        connect(this->ui->startButton, &QPushButton::clicked, this, &RenderWidget::startCurrentTask);
        connect(this->ui->stopButton, &QPushButton::clicked, this, &RenderWidget::stopCurrentTask);
    }

RenderWidget::~RenderWidget() {
    delete ui;
    if(this->currentTask != nullptr){
        currentTask->stop();
        currentTask->join();
    }
    if(timerThread.joinable()){
        this->timerThread.join();
    }
}

OpenGLWidget *RenderWidget::getOpenGLWidget() const {
    return ui->openGLWidget;
}

void RenderWidget::renderWorldSpaceMesh(const std::string &group, const std::shared_ptr<WorldSpaceMesh> &worldSpaceMesh,  const Color& color) {
    QMetaObject::invokeMethod(this->getOpenGLWidget(), "renderWorldSpaceMeshSlot",
                              Qt::AutoConnection,
                              Q_ARG(std::string, group),
                              Q_ARG(std::shared_ptr<WorldSpaceMesh>, std::make_shared<WorldSpaceMesh>(*worldSpaceMesh)), // We should copy the actual worldSpaceMesh object here, otherwise the transformation could change before the render thread reads it
                              Q_ARG(Color, color),
                              Q_ARG(RenderWidget*, this));
}

void RenderWidget::addControlWidget(const std::string &group, const std::shared_ptr<AbstractRenderModel> &renderModel) {
    this->getOrAddGroupLayout(group)->addWidget(new RenderModelControlWidget(renderModel, this));
}

void RenderWidget::clear() {
    // clear all layouts, on the UI thread
    QMetaObject::invokeMethod(this, [&]{
        for (const auto &[group, layout]: groupLayouts){
            QLayoutItem* item;
            while((item = layout->takeAt(0))!=nullptr) {
                delete item->widget();
                delete item;
            }
        }
        groupLayouts.clear();
        QLayoutItem* item;
        while((item = this->ui->objectsVerticalLayout->takeAt(0))!=nullptr) {
            if(item->widget()){
                delete item->widget();
                delete item;
            }
            else if(item->layout()){
                delete item->layout();
            }
        }

        this->ui->objectsVerticalLayout->update();

        QMetaObject::invokeMethod(this->getOpenGLWidget(), "clear", Qt::AutoConnection);
    });
}

void RenderWidget::clearGroup(const std::string &group) {
    QMetaObject::invokeMethod(this, [&, group] {
        QLayoutItem* item;

        // Clear the items from the group layout
        auto groupLayout = this->getOrAddGroupLayout(group);
        while((item = groupLayout->takeAt(0))!=nullptr) {
            delete item->widget();
            delete item;
        }
        groupLayouts.erase(group);

        int i = 0;
        // Delete the groups title and horizontal line
        while((item = this->ui->objectsVerticalLayout->itemAt(i))!=nullptr) {
            if (item->widget()!=nullptr && item->widget()->objectName() == QString::fromStdString(group)) {
                item = this->ui->objectsVerticalLayout->takeAt(i);
                delete item->widget();
                delete item;
            }
            else if(item->layout()!=nullptr && item->layout()->objectName() == QString::fromStdString(group)) {
                item = this->ui->objectsVerticalLayout->takeAt(i);
                delete item->layout();
            }
            else{
                i++;
            }
        }

        this->ui->objectsVerticalLayout->update();

        QMetaObject::invokeMethod(this->getOpenGLWidget(), "clearGroup", Qt::AutoConnection, Q_ARG(std::string, group));
    });
}

QVBoxLayout *RenderWidget::getOrAddGroupLayout(const std::string &group) {

    // Find the group
    auto iterator = groupLayouts.find(group);

    // Add new group if not found
    if(iterator == groupLayouts.end()){
        const auto layout = new QVBoxLayout();
        layout->setObjectName(QString::fromStdString(group));

        auto titleLabel = new QLabel(this);
        titleLabel->setText(QString::fromStdString(group));
        titleLabel->setObjectName(QString::fromStdString(group));
        titleLabel->setMaximumSize(QSize(300, 300));
        titleLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        this->ui->objectsVerticalLayout->addWidget(titleLabel);

        iterator = groupLayouts.insert({group, layout}).first;
        this->ui->objectsVerticalLayout->addLayout(layout);

        auto line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setObjectName(QString::fromStdString(group));
        line->setFrameShadow(QFrame::Sunken);
        this->ui->objectsVerticalLayout->addWidget(line);
    }
    return iterator->second;
}

void RenderWidget::renderBox(const std::string &group, const std::string& name,  const AABB &aabb, const Transformation& transformation, const Color& color) {
    QMetaObject::invokeMethod(this->getOpenGLWidget(), "renderBoxSlot", Qt::AutoConnection,
                              Q_ARG(std::string, group),
                              Q_ARG(std::string, name),
                              Q_ARG(AABB, aabb),
                              Q_ARG(Transformation, transformation),
                              Q_ARG(Color, color),
                              Q_ARG(RenderWidget*, this));
}

void RenderWidget::renderSphere(const std::string &group, const std::string& name, const Sphere &sphere, const Color &color) {
    QMetaObject::invokeMethod(this->getOpenGLWidget(), "renderSphereSlot", Qt::AutoConnection,
                              Q_ARG(std::string, group),
                              Q_ARG(std::string, name),
                              Q_ARG(Sphere, sphere),
                              Q_ARG(Color, color),
                              Q_ARG(RenderWidget*, this));
}

void RenderWidget::renderTriangle(const std::string& group, const std::string& name, const VertexTriangle& vertexTriangle, const Color &color) {
    QMetaObject::invokeMethod(this->getOpenGLWidget(), "renderTriangleSlot", Qt::AutoConnection,
                              Q_ARG(std::string, group),
                              Q_ARG(std::string, name),
                              Q_ARG(VertexTriangle, vertexTriangle),
                              Q_ARG(Color, color),
                              Q_ARG(RenderWidget*, this));
}

void RenderWidget::renderLine(const std::string &group, const Vertex &vertexA, const Vertex &vertexB,
                              const Color &color) {
    QMetaObject::invokeMethod(this->getOpenGLWidget(), "renderLineSlot", Qt::AutoConnection,
                                Q_ARG(std::string, group),
                                Q_ARG(Vertex, vertexA),
                                Q_ARG(Vertex, vertexB),
                                Q_ARG(Color, color),
                                Q_ARG(RenderWidget*, this));
}

void RenderWidget::notifySolution(const std::shared_ptr<const AbstractSolution>& solution) {
    if(this->onSolutionNotified) onSolutionNotified(this, solution);
}

void RenderWidget::notifyProgress(float progress) {
    int value = int(100*progress);
    QMetaObject::invokeMethod(this, "updateProgressBarSlot", Qt::AutoConnection, Q_ARG(int, value));
}

[[maybe_unused]] void RenderWidget::updateProgressBarSlot(int progress) {
    this->ui->progressBar->setValue(progress);
}

void RenderWidget::notifyStarted() {
    QMetaObject::invokeMethod(this, "setStatusLabelSlot", Qt::AutoConnection, Q_ARG(QString, QString::fromStdString("-")));
    QMetaObject::invokeMethod(this, "updateProgressBarSlot", Qt::AutoConnection, Q_ARG(int, 0));
    QMetaObject::invokeMethod(this, "setStopButtonEnabledSlot", Qt::AutoConnection, Q_ARG(bool, true));
    QMetaObject::invokeMethod(this, "setStartButtonEnabledSlot", Qt::AutoConnection, Q_ARG(bool, false));

    this->taskRunning = true;

    // Start timer thread
    timerThread = std::thread([&]{
        auto startMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        while(taskRunning){
            auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            auto elapsedMs = nowMs - startMs;
            //Format as HH:MM:SS
            auto hours = std::chrono::duration_cast<std::chrono::hours>(elapsedMs);
            elapsedMs -= std::chrono::milliseconds(std::chrono::hours(hours));
            auto minutes = std::chrono::duration_cast<std::chrono::minutes>(elapsedMs);
            elapsedMs -= std::chrono::milliseconds(std::chrono::minutes(minutes));
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsedMs);
//            elapsedMs -= std::chrono::milliseconds(std::chrono::seconds(seconds));
//            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedMs);
            std::stringstream ss;
            ss << std::setfill('0') << std::setw(2) << hours.count()
               << ":" << std::setfill('0') << std::setw(2) << minutes.count()
               << ":" << std::setfill('0') << std::setw(2) << seconds.count();
//               << "." << std::setfill('0') << std::setw(3) << milliseconds.count();

            QMetaObject::invokeMethod(this, "setTimeLabelSlot", Qt::AutoConnection, Q_ARG(QString, QString::fromStdString(ss.str())));

            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    });
    this->clear();
}

void RenderWidget::notifyFinished() {
    this->taskRunning = false;
    if(timerThread.joinable()) timerThread.join();
    QMetaObject::invokeMethod(this, "setStopButtonEnabledSlot", Qt::AutoConnection, Q_ARG(bool, false));
    QMetaObject::invokeMethod(this, "setStartButtonEnabledSlot", Qt::AutoConnection, Q_ARG(bool, true));
}

void RenderWidget::notifyStatus(const std::string &status) {
    QString qStatus = QString::fromUtf8(status.data(), status.size());
    QMetaObject::invokeMethod(this, "setStatusLabelSlot", Qt::AutoConnection, Q_ARG(QString, qStatus));
}

[[maybe_unused]] void RenderWidget::setStartButtonEnabledSlot(bool enabled) {
    this->ui->startButton->setEnabled(enabled);
}

[[maybe_unused]] void RenderWidget::setStopButtonEnabledSlot(bool enabled) {
    this->ui->stopButton->setEnabled(enabled);
}

[[maybe_unused]] void RenderWidget::setStatusLabelSlot(const QString& status) {
    this->ui->statusLabel->setText(status);
}

void RenderWidget::observeTask(AbstractTask *task, const std::function<void(RenderWidget* renderWidget, const std::shared_ptr<const AbstractSolution> solution)>& onTaskSolutionNotified) {

    // Clear currently observed task if needed
    if(this->currentTask!=nullptr){
        currentTask->unregisterObserver(this);
        this->clear();
        this->ui->taskSection->setVisible(false);
        this->onSolutionNotified = {};
    }

    // Set and observe new task
    this->currentTask = task;
    this->onSolutionNotified = onTaskSolutionNotified;
    if(task!=nullptr){
        currentTask->registerObserver(this);
        this->ui->taskSection->setVisible(true);
    }
}

void RenderWidget::startCurrentTask() {
    if(this->currentTask!=nullptr) this->currentTask->start();
}

void RenderWidget::stopCurrentTask() {
    if(this->currentTask!=nullptr) this->currentTask->stop();
}

void RenderWidget::addOrUpdateRenderModel(const std::string& group, const std::string& id, std::shared_ptr<AbstractRenderModel> renderModel) {
    QMetaObject::invokeMethod(this->getOpenGLWidget(), "addOrUpdateRenderModelSlot",
                              Qt::AutoConnection,
                              Q_ARG(std::string, group),
                              Q_ARG(std::string, id),
                              Q_ARG(std::shared_ptr<AbstractRenderModel>, renderModel), // We should copy the actual worldSpaceMesh object here, otherwise the transformation could change before the render thread reads it
                              Q_ARG(RenderWidget*, this));
}

void RenderWidget::captureScene() {
    QMetaObject::invokeMethod(this->getOpenGLWidget(), "captureSceneSlot",
                              Qt::AutoConnection);

}

void RenderWidget::captureSceneToFile(const std::string &fileName) {
    QMetaObject::invokeMethod(this->getOpenGLWidget(), "captureSceneToFileSlot",
                              Qt::AutoConnection,
                              Q_ARG(QString, QString::fromStdString(fileName)));
}

void RenderWidget::captureAnimation() {
    QMetaObject::invokeMethod(this->getOpenGLWidget(), "captureAnimationSlot",
                              Qt::AutoConnection);
}

[[maybe_unused]] void RenderWidget::setTimeLabelSlot(const QString &time) {
    this->ui->timeLabel->setText(time);
}
