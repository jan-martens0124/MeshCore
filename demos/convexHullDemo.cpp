//
// Created by tolle on 20/09/2023.
//


#include <iostream>
#include <thread>
#include <filesystem>

#include "meshcore/rendering/ApplicationWindow.h"
#include "meshcore/utility/FileParser.h"

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
        auto originalModelSpaceMesh = FileParser::loadMeshFile(MESHCORE_DATA_DIR + std::string("E. F. Silva et al. 2021/dragon.obj"));
        auto convexHull = originalModelSpaceMesh->getConvexHull();

        auto name = originalModelSpaceMesh->getName();

        originalModelSpaceMesh->setName("Original");
        convexHull->setName("Convex Hull");

        renderWidget->renderWorldSpaceMesh(name, std::make_shared<WorldSpaceMesh>(originalModelSpaceMesh), Color::White());
        renderWidget->renderWorldSpaceMesh(name, std::make_shared<WorldSpaceMesh>(convexHull), Color(1,0,0,0.5));
    }

    // Run a small benchmark
    auto map = MESHCORE_DATA_DIR + std::string("Tollenaere, J. et al/Containers");
    for (const auto &entry : std::filesystem::directory_iterator(map)) {

        auto start = std::chrono::high_resolution_clock::now();
        auto modelSpaceMesh = FileParser::loadMeshFile(entry.path().string());
        auto end = std::chrono::high_resolution_clock::now();
        auto fileLoadingTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        auto optionalConvexHull = modelSpaceMesh->getConvexHull();
        end = std::chrono::high_resolution_clock::now();
        auto convexHullTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "Computed convex hull for " << modelSpaceMesh->getName() << " in " << convexHullTime << " ms (File loading took " << fileLoadingTime << " ms)" << std::endl;
    }
}
