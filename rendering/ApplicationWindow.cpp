//
// Created by Jonas on 19/05/2022.
//

#include "ApplicationWindow.h"
#include "../utility/FileParser.h"
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <iostream>
#include <filesystem>
#include <QGuiApplication>

ApplicationWindow::ApplicationWindow() {

    auto icon = QIcon(R"(..\..\meshcore\icons\logo3.svg)");
    QGuiApplication::setWindowIcon(icon);

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
    connect(openAction, &QAction::triggered, this, &ApplicationWindow::loadMesh);
    openAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));

    QAction* screenshotAction = fileMenu->addAction(QString("Export current frame..."));
    connect(screenshotAction, &QAction::triggered, renderWidget, &RenderWidget::captureScene);
    screenshotAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));

    QAction* animationAction = fileMenu->addAction(QString("Export GIF animation..."));
    connect(animationAction, &QAction::triggered, renderWidget, &RenderWidget::captureAnimation);
    animationAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));

    QAction* exitAction = fileMenu->addAction(QString("Exit"));
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    exitAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F4));

//    QAction* zoomInAction = viewMenu->addAction(QString("Zoom in"));
//    connect(zoomInAction, &QAction::triggered, this, &ApplicationWindow::loadMesh);
//    zoomInAction->setShortcut(QKeySequence::ZoomIn);

    QAction* resetViewAction = viewMenu->addAction(QString("Reset view"));
    connect(resetViewAction, &QAction::triggered, renderWidget, &RenderWidget::resetViewTransformation);
    resetViewAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_0));

//    QAction* toggleWireFrame = viewMenu->addAction(QString("Enable Wireframe"));
//    connect(toggleWireFrame, &QAction::triggered, renderWidget->getOpenGLWidget(),
//            &OpenGLWidget::toggleWireframe);
//    toggleWireFrame->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
//    toggleWireFrame->setCheckable(true);
//
//    QAction* toggleCulling = viewMenu->addAction(QString("Enable Backface Culling"));
//    connect(toggleCulling, &QAction::triggered, renderWidget->getOpenGLWidget(), &OpenGLWidget::toggleCullFace);
//    toggleCulling->setCheckable(true);
//    toggleCulling->setChecked(true);

    {
        QAction* toggleAxis = viewMenu->addAction(QString("Enable Axis"));
        connect(toggleAxis, &QAction::triggered, [=](bool enabled){
            renderWidget->getOpenGLWidget()->setAxisEnabled(enabled);
            renderWidget->getOpenGLWidget()->update();
        });
        toggleAxis->setCheckable(true);
        toggleAxis->setChecked(renderWidget->getOpenGLWidget()->isAxisEnabled());
        toggleAxis->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
    }
    {
        QAction* toggleLightMode = viewMenu->addAction(QString("Enable Light Mode"));
        connect(toggleLightMode, &QAction::triggered, [=](bool enabled){
            renderWidget->getOpenGLWidget()->setLightMode(enabled);
            renderWidget->getOpenGLWidget()->update();
        });
        toggleLightMode->setCheckable(true);
        toggleLightMode->setChecked(renderWidget->getOpenGLWidget()->isLightMode());
        toggleLightMode->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
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
    connect(aboutAction, &QAction::triggered, this, &ApplicationWindow::displayApplicationInfo);

    setMinimumSize(1280,720);
    setWindowTitle(tr("MeshCore"));
}

RenderWidget *ApplicationWindow::getRenderWidget() const {
    return renderWidget;
}

void ApplicationWindow::keyPressEvent(QKeyEvent* event){
    printf("\nkey event in ApplicationWindow: %i", event->key());
    std::cout << event->key() << std::endl;
}

void ApplicationWindow::loadMesh(){
    QString fileName = QFileDialog::getOpenFileName(this, QString("Select mesh file"), R"(C:\Users\tolle\CLionProjects\MeshCore\data\models)", QString("Mesh Files (*.stl *.obj)"));
    if(std::filesystem::exists(fileName.toStdString())){
        std::shared_ptr<ModelSpaceMesh> modelSpaceMesh = FileParser::loadMeshFile(fileName.toStdString());
        const auto worldSpaceMesh = std::make_shared<WorldSpaceMesh>(modelSpaceMesh);
        this->renderWidget->renderWorldSpaceMesh("File", worldSpaceMesh);
    }
}

void ApplicationWindow::displayApplicationInfo(){
    auto messageBox = QMessageBox(this);
    messageBox.setInformativeText("Meshcore v0.0.2");
    messageBox.setWindowTitle("About MeshCore");
    messageBox.setText("Created by Jonas Tollenaere");
    messageBox.exec();
}
