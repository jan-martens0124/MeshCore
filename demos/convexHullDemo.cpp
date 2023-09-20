//
// Created by tolle on 20/09/2023.
//

#include "rendering/ApplicationWindow.h"

#include <iostream>
#include <QtWidgets>
#include <thread>
#include <filesystem>
#include "utility/FileParser.h"
#include "rendering/RenderSphere.h"

void run(RenderWidget* renderWidget);

int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    ApplicationWindow window;
    window.show();

    std::thread workerThread(run, window.getRenderWidget());

    int returnCode = QApplication::exec();
    workerThread.join();
    return returnCode;
}

void run(RenderWidget* renderWidget){

    // Visualize an example
    {
        auto originalModelSpaceMesh = FileParser::loadMeshFile("../datasets/E. F. Silva et al. 2021/dragon.obj");
        auto optionalConvexHull = originalModelSpaceMesh->getConvexHull();

        auto name = originalModelSpaceMesh->getName();

        if(!optionalConvexHull.has_value()){
            std::cout << "Could not compute convex hull for " << name << std::endl;
        }
        else{
            originalModelSpaceMesh->setName("Original");
            auto convexHull = optionalConvexHull.value();
            convexHull->setName("Convex Hull");


            renderWidget->renderWorldSpaceMesh(name, std::make_shared<WorldSpaceMesh>(originalModelSpaceMesh), Color::White());
            renderWidget->renderWorldSpaceMesh(name, std::make_shared<WorldSpaceMesh>(convexHull), Color(1,0,0,0.5));
        }
    }

    // Run a small benchmark
    auto map = "../../meshcore/datasets/Tollenaere, J. et al/Containers";
    for (const auto &entry : std::filesystem::directory_iterator(map)) {

        auto start = std::chrono::high_resolution_clock::now();
        auto modelSpaceMesh = FileParser::loadMeshFile(entry.path().string());
        auto end = std::chrono::high_resolution_clock::now();
        auto fileLoadingTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        auto optionalConvexHull = modelSpaceMesh->getConvexHull();
        end = std::chrono::high_resolution_clock::now();
        auto convexHullTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        if(!optionalConvexHull.has_value()){
            std::cout << "Could not compute convex hull for " << modelSpaceMesh->getName() << std::endl;
            continue;
        }
        else{
            std::cout << "Computed convex hull for " << modelSpaceMesh->getName() << " in " << convexHullTime << " ms (File loading took " << fileLoadingTime << " ms)" << std::endl;
        }

    }
}
