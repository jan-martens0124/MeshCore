//
// Created by Jonas Tollenaere on 4/11/2021.
//

#include <iostream>
#include <QtWidgets>
#include <thread>
#include "rendering/ApplicationWindow.h"
#include "utility/FileParser.h"
#include "rendering/RenderSphere.h"

[[noreturn]] void run(RenderWidget* openGlRenderWidget);

int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    ApplicationWindow window;
    window.show();

    std::thread thread(run, window.getRenderWidget());


    Sphere sphere(glm::vec3(-5.0f, 0.0f, 0.0f), 5.0f);
    auto renderSphere = std::make_shared<RenderSphere>(sphere, Transformation(), window.getRenderWidget()->getOpenGLWidget()->getAmbientShader(), window.getRenderWidget()->getOpenGLWidget()->getDiffuseShader());
    renderSphere->setColor(Color(1.0f, 0.2f, 0.2f, 0.5f));
    window.getRenderWidget()->addOrUpdateRenderModel("Sphere", "0qd54f", renderSphere);

    int returnCode = QApplication::exec();
    thread.join();
    return returnCode;
}

[[noreturn]] void run(RenderWidget* openGlRenderWidget){

    /// Here is where you would start of your code

    // Load some problem meshes
    std::shared_ptr<ModelSpaceMesh> modelSpaceMesh = FileParser::loadMeshFile("../../data/models/Everton/banana.stl");
    std::shared_ptr<WorldSpaceMesh> bananaWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(modelSpaceMesh);

    std::shared_ptr<ModelSpaceMesh> modelSpaceMesh2 = FileParser::loadMeshFile("../../data/models/rocks/rock_008k.obj");
    std::shared_ptr<WorldSpaceMesh> rockWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(modelSpaceMesh2);

    std::cout << "Number of vertices: " << bananaWorldSpaceMesh->getModelSpaceMesh()->getVertices().size() << std::endl;
    std::cout << "Number of vertices: " << rockWorldSpaceMesh->getModelSpaceMesh()->getVertices().size() << std::endl;

    glm::vec3 position(10.0f,0,0);

    bananaWorldSpaceMesh->getModelTransformation().setPosition(position);
    bananaWorldSpaceMesh->getModelTransformation().setYaw(1.0f);

    rockWorldSpaceMesh->getModelTransformation().setScale(2.0f);

    // Pass them to the renderer
    openGlRenderWidget->renderWorldSpaceMesh("Meshes", rockWorldSpaceMesh, Color(0.8, 0.8, 0.8, 0.6));
    openGlRenderWidget->renderWorldSpaceMesh("Meshes", bananaWorldSpaceMesh, Color(0.75, 0.75, 0, 1));



    std::shared_ptr<WorldSpaceMesh> worldSpaceMesh = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../../data/public/Stoyan et al. 2005/Polyhedron5.obj"));
    openGlRenderWidget->renderWorldSpaceMesh("Meshes", worldSpaceMesh, Color(0.75, 0.75, 0, 1));

    auto result = worldSpaceMesh->getModelSpaceMesh()->getConvexHull();

//    std::shared_ptr<WorldSpaceMesh> worldSpaceMesh2 = std::make_shared<WorldSpaceMesh>(result.value());
//    openGlRenderWidget->renderWorldSpaceMesh("Meshes", worldSpaceMesh2, Color(0.75, 0.75, 0, 1));
//    std::cout << result.has_value() << std::endl;
}