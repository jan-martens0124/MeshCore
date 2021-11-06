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
    std::shared_ptr<ModelSpaceMesh> modelSpaceMesh = FileParser::parseFile("../../data/models/everton/banana.stl");
    std::shared_ptr<WorldSpaceMesh> staticWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(modelSpaceMesh);

    std::shared_ptr<ModelSpaceMesh> modelSpaceMesh2 = FileParser::parseFile("../../data/models/everton/banana.stl");
    std::shared_ptr<WorldSpaceMesh> dynamicWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(modelSpaceMesh2);

    std::cout << "Number of vertices: " << staticWorldSpaceMesh->getModelSpaceMesh()->getVertices().size() << std::endl;
    std::cout << "Number of vertices: " << dynamicWorldSpaceMesh->getModelSpaceMesh()->getVertices().size() << std::endl;

    // Pas them to the renderer
    openGlRenderWidget->addOrUpdateWorldSpaceMesh(*staticWorldSpaceMesh, Color(0.8, 0.8, 0.8, 0.6));
    openGlRenderWidget->addOrUpdateWorldSpaceMesh(*dynamicWorldSpaceMesh, Color(0.75, 0.75, 0, 1));

    while(true){
        dynamicWorldSpaceMesh->getModelTransformation().deltaRotation(0.001f, 0.0047f, 0.0023f);
        openGlRenderWidget->addOrUpdateWorldSpaceMesh(*dynamicWorldSpaceMesh, Color());
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}