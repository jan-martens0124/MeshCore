//
// Created by Jonas Tollenaere on 4/11/2021.
//

#include <iostream>
#include <QtWidgets>
#include <core/FileParser.h>
#include "rendering/RenderWindow.h"
#include <thread>

[[noreturn]] void run(OpenGLRenderWidget* openGlRenderWidget);

int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    RenderWindow window;
    window.show();

    std::thread thread(run, window.getOpenGlRenderWidget());
    int returnCode = QApplication::exec();
    thread.join();
    return returnCode;
}

[[noreturn]] void run(OpenGLRenderWidget* openGlRenderWidget){

    /// Here is where you would start of your code

    // Load some problem meshes
    std::shared_ptr<ModelSpaceMesh> modelSpaceMesh = FileParser::parseFile("../data/models/everton/banana.stl");
    std::shared_ptr<WorldSpaceMesh> staticWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(modelSpaceMesh);

    std::shared_ptr<ModelSpaceMesh> modelSpaceMesh2 = FileParser::parseFile("../data/models/everton/banana.stl");
    std::shared_ptr<WorldSpaceMesh> dynamicWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(modelSpaceMesh2);

    std::cout << "Number of vertices: " << staticWorldSpaceMesh->getModelSpaceMesh()->getVertices().size() << std::endl;
    std::cout << "Number of vertices: " << dynamicWorldSpaceMesh->getModelSpaceMesh()->getVertices().size() << std::endl;

    glm::vec3 position(100.0f,0,0);

    staticWorldSpaceMesh->getModelTransformation().setPosition(position);
    staticWorldSpaceMesh->getModelTransformation().setYaw(1.0f);

    // Pas them to the renderer
    openGlRenderWidget->addOrUpdateWorldSpaceMesh(*staticWorldSpaceMesh, Color(0.8, 0.8, 0.8, 0.6));
    openGlRenderWidget->addOrUpdateWorldSpaceMesh(*dynamicWorldSpaceMesh, Color(0.75, 0.75, 0, 1));

}