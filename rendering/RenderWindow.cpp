//
// Created by Jonas on 30/11/2020.
//

#include "RenderWindow.h"
#include "../meshcore/utility/FileParser.h"
#include <QtWidgets>
#include <iostream>
#include <filesystem>

RenderWindow::RenderWindow() {

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(16);
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

   this->openGlRenderWidget = new OpenGLRenderWidget(this);

//    taskRenderWidget->setMinimumSize(200, 100);
//    taskRenderWidget->setFocus();
    setCentralWidget(openGlRenderWidget);

    menuBar = new QMenuBar;
    auto* fileMenu = new QMenu(QString("File"), this);
    auto* viewMenu = new QMenu(QString("View"), this);
    auto* helpMenu = new QMenu(QString("Help"), this);
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(viewMenu);
    menuBar->addMenu(helpMenu);
    setMenuBar(menuBar);

    QAction* openAction = fileMenu->addAction(QString("Open..."));
    connect(openAction, &QAction::triggered, this, &RenderWindow::loadMesh);
    openAction->setShortcut(QKeySequence(QString("Ctrl+O")));

    QAction* exitAction = fileMenu->addAction(QString("Exit"));
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

//    QAction* zoomInAction = viewMenu->addAction(QString("Zoom in"));
//    connect(zoomInAction, &QAction::triggered, this, &ApplicationWindow::loadMesh);
//    zoomInAction->setShortcut(QKeySequence::ZoomIn);

    QAction* resetViewAction = viewMenu->addAction(QString("Reset view"));
    connect(resetViewAction, &QAction::triggered, openGlRenderWidget, &OpenGLRenderWidget::resetView);
    resetViewAction->setShortcut(QKeySequence(QString("Ctrl+0")));

    QAction* toggleWireFrame = viewMenu->addAction(QString("Enable Wireframe"));
    connect(toggleWireFrame, &QAction::triggered, openGlRenderWidget, &OpenGLRenderWidget::toggleWireframe);
    toggleWireFrame->setShortcut(QKeySequence(QString("Ctrl+W")));
    toggleWireFrame->setCheckable(true);

    QAction* toggleCulling = viewMenu->addAction(QString("Enable Culling"));
    connect(toggleCulling, &QAction::triggered, openGlRenderWidget, &OpenGLRenderWidget::toggleCullFace);
    toggleCulling->setCheckable(true);
    toggleCulling->setChecked(true);

    QAction* toggleLightMode = viewMenu->addAction(QString("Enable Light mode"));
    connect(toggleLightMode, &QAction::triggered, [=](bool enabled){
        openGlRenderWidget->setLightMode(enabled);
        openGlRenderWidget->update();
    });
    toggleLightMode->setCheckable(true);
    toggleLightMode->setChecked(openGlRenderWidget->isLightMode());



    QAction* aboutAction = helpMenu->addAction(QString("About Meshcore"));
    connect(aboutAction, &QAction::triggered, this, &RenderWindow::displayApplicationInfo);

    setMinimumSize(1280,720);
    setWindowTitle(tr("MeshCore"));
}

void RenderWindow::keyPressEvent(QKeyEvent* event){
    printf("\nkey event in ApplicationWindow: %i", event->key());
    std::cout << event->key() << std::endl;
}

void RenderWindow::loadMesh(){
    QString fileName = QFileDialog::getOpenFileName(this, QString("Select mesh file"), R"(C:\Users\tolle\CLionProjects\MeshCore\data\models)", QString("Mesh Files (*.stl *.obj)"));
    if(std::filesystem::exists(fileName.toStdString())){
        std::shared_ptr<ModelSpaceMesh> modelSpaceMesh = FileParser::parseFile(fileName.toStdString());
        const WorldSpaceMesh worldSpaceMesh = WorldSpaceMesh(modelSpaceMesh);
        openGlRenderWidget->addOrUpdateWorldSpaceMesh(worldSpaceMesh, Color(1,0,0,1));
    }
}

void RenderWindow::displayApplicationInfo(){
    auto messageBox = QMessageBox(this);
    messageBox.setInformativeText("Meshcore v0.0.0");
    messageBox.setWindowTitle("About MeshCore");
    messageBox.setText("Created by Jonas Tollenaere");
    messageBox.exec();
}

OpenGLRenderWidget *RenderWindow::getOpenGlRenderWidget() const {
    return openGlRenderWidget;
}
