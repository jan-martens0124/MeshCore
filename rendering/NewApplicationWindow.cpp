//
// Created by Jonas on 19/05/2022.
//

#include "NewApplicationWindow.h"
#include "../utility/FileParser.h"
#include <QtWidgets>
#include <iostream>
#include <filesystem>

NewApplicationWindow::NewApplicationWindow() {

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(16);
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    this->renderWidget = new RenderWidget(this);

//    taskRenderWidget->setMinimumSize(200, 100);
//    taskRenderWidget->setFocus();
    setCentralWidget(renderWidget);

    menuBar = new QMenuBar;
    auto* fileMenu = new QMenu(QString("File"), this);
    auto* viewMenu = new QMenu(QString("View"), this);
    auto* helpMenu = new QMenu(QString("Help"), this);
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(viewMenu);
    menuBar->addMenu(helpMenu);
    setMenuBar(menuBar);

    QAction* openAction = fileMenu->addAction(QString("Open..."));
    connect(openAction, &QAction::triggered, this, &NewApplicationWindow::loadMesh);
    openAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));

    QAction* screenshotAction = fileMenu->addAction(QString("Export openGL screenshot..."));
    connect(screenshotAction, &QAction::triggered, renderWidget, &RenderWidget::captureScene);
    screenshotAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));

    QAction* exitAction = fileMenu->addAction(QString("Exit"));
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    exitAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F4));

//    QAction* zoomInAction = viewMenu->addAction(QString("Zoom in"));
//    connect(zoomInAction, &QAction::triggered, this, &ApplicationWindow::loadMesh);
//    zoomInAction->setShortcut(QKeySequence::ZoomIn);

    QAction* resetViewAction = viewMenu->addAction(QString("Reset view"));
    connect(resetViewAction, &QAction::triggered, renderWidget->getOpenGLWidget(), &OpenGLWidget::resetView);
    resetViewAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_0));

    QAction* toggleWireFrame = viewMenu->addAction(QString("Enable Wireframe"));
    connect(toggleWireFrame, &QAction::triggered, renderWidget->getOpenGLWidget(),
            &OpenGLWidget::toggleWireframe);
    toggleWireFrame->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    toggleWireFrame->setCheckable(true);

    QAction* toggleCulling = viewMenu->addAction(QString("Enable Backface Culling"));
    connect(toggleCulling, &QAction::triggered, renderWidget->getOpenGLWidget(), &OpenGLWidget::toggleCullFace);
    toggleCulling->setCheckable(true);
    toggleCulling->setChecked(true);

    {
        QAction* toggleLightMode = viewMenu->addAction(QString("Enable Light mode"));
        connect(toggleLightMode, &QAction::triggered, [=](bool enabled){
            renderWidget->getOpenGLWidget()->setLightMode(enabled);
            renderWidget->getOpenGLWidget()->update();
        });
        toggleLightMode->setCheckable(true);
        toggleLightMode->setChecked(renderWidget->getOpenGLWidget()->isLightMode());
    }
    {
        QAction* togglePerspective = viewMenu->addAction(QString("Enable Perspective"));
        connect(togglePerspective, &QAction::triggered, [=](bool enabled){
            renderWidget->getOpenGLWidget()->setUsePerspective(enabled);
            renderWidget->getOpenGLWidget()->update();
        });
        togglePerspective->setCheckable(true);
        togglePerspective->setChecked(renderWidget->getOpenGLWidget()->isUsePerspective());
    }

    QAction* aboutAction = helpMenu->addAction(QString("About Meshcore"));
    connect(aboutAction, &QAction::triggered, this, &NewApplicationWindow::displayApplicationInfo);

    setMinimumSize(1280,720);
    setWindowTitle(tr("MeshCore"));
}

RenderWidget *NewApplicationWindow::getRenderWidget() const {
    return renderWidget;
}

void NewApplicationWindow::keyPressEvent(QKeyEvent* event){
    printf("\nkey event in ApplicationWindow: %i", event->key());
    std::cout << event->key() << std::endl;
}

void NewApplicationWindow::loadMesh(){
    QString fileName = QFileDialog::getOpenFileName(this, QString("Select mesh file"), R"(C:\Users\tolle\CLionProjects\MeshCore\data\models)", QString("Mesh Files (*.stl *.obj)"));
    if(std::filesystem::exists(fileName.toStdString())){
        std::shared_ptr<ModelSpaceMesh> modelSpaceMesh = FileParser::loadMeshFile(fileName.toStdString());
        const WorldSpaceMesh worldSpaceMesh = WorldSpaceMesh(modelSpaceMesh);
//        taskRenderWidget->addWorldSpaceMesh(worldSpaceMesh);
    }
}

void NewApplicationWindow::displayApplicationInfo(){
    auto messageBox = QMessageBox(this);
    messageBox.setInformativeText("Meshcore v0.0.2");
    messageBox.setWindowTitle("About MeshCore");
    messageBox.setText("Created by Jonas Tollenaere");
    messageBox.exec();
}
