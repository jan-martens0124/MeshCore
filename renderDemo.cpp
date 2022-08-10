//
// Created by Jonas Tollenaere on 4/11/2021.
//

#include <iostream>
#include <QtWidgets>
#include <thread>
#include "rendering/NewApplicationWindow.h"
#include "utility/FileParser.h"
#include "rendering/OpenGLWidget.h"

[[noreturn]] void run(RenderWidget* openGlRenderWidget);

int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    NewApplicationWindow window;
    window.show();

    std::thread thread(run, window.getRenderWidget());
    int returnCode = QApplication::exec();
    thread.join();
    return returnCode;
}

[[noreturn]] void run(RenderWidget* openGlRenderWidget){

    /// Here is where you would start of your code

    // Load some problem meshes
    std::shared_ptr<ModelSpaceMesh> modelSpaceMesh = FileParser::loadMeshFile("../../data/models/Everton/banana.stl");
    std::shared_ptr<WorldSpaceMesh> staticWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(modelSpaceMesh);

    std::shared_ptr<ModelSpaceMesh> modelSpaceMesh2 = FileParser::loadMeshFile("../../data/models/rocks/rock_008k.obj");
    std::shared_ptr<WorldSpaceMesh> dynamicWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(modelSpaceMesh2);

    std::cout << "Number of vertices: " << staticWorldSpaceMesh->getModelSpaceMesh()->getVertices().size() << std::endl;
    std::cout << "Number of vertices: " << dynamicWorldSpaceMesh->getModelSpaceMesh()->getVertices().size() << std::endl;

    glm::vec3 position(10.0f,0,0);

    staticWorldSpaceMesh->getModelTransformation().setPosition(position);
    staticWorldSpaceMesh->getModelTransformation().setYaw(1.0f);

    dynamicWorldSpaceMesh->getModelTransformation().setScale(2.0f);

    // Pas them to the renderer
    openGlRenderWidget->renderWorldSpaceMesh("Meshes", staticWorldSpaceMesh, Color(0.8, 0.8, 0.8, 0.6));
    openGlRenderWidget->renderWorldSpaceMesh("Meshes", dynamicWorldSpaceMesh, Color(0.75, 0.75, 0, 1));

}