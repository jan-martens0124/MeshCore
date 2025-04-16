//
// Created by Jonas on 13/02/2023.
//


#include <memory>

#include "meshcore/core/ModelSpaceMesh.h"
#include "meshcore/utility/FileParser.h"
#include "meshcore/acceleration/AABBVolumeHierarchy.h"
#include "meshcore/rendering/ApplicationWindow.h"

int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    ApplicationWindow window;
    window.show();
    RenderWidget* renderWidget = window.getRenderWidget();

    std::thread t([&]() {
        auto meshA = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../../meshcore/datasets/E. F. Silva et al. 2021/pig.obj"));
        auto meshB = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../../meshcore/datasets/E. F. Silva et al. 2021/dragon.obj"));
        meshB->getModelTransformation().setPositionX(10);

        while(true){

            Quaternion dRotationA(0.08f, 0.03f, 0.02f);
//            meshA->getModelTransformation().factorRotation(dRotationA);
            Quaternion dRotationB(0.01f, 0.02f, 0.03f);
            meshB->getModelTransformation().factorRotation(dRotationB);

            Transformation dTransformationA;
            dTransformationA.setRotation(dRotationA);
            meshA->getModelTransformation()*=dTransformationA;


            auto test = meshA->getModelTransformation() * meshB->getModelTransformation();


            glm::vec3 closestPointA, closestPointB;


            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            auto distance = Distance::distance(*meshA, *meshB, &closestPointA, &closestPointB);

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::cout << "Distance: " << distance << " in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;


            auto delta = closestPointA - closestPointB;
            auto distance2 = glm::length(delta);
//            assert(glm::abs((distance2 - distance)/distance) <= 1e-5f);

            renderWidget->clearGroup("Closest Points");
            renderWidget->renderWorldSpaceMesh("Meshes", meshA, Color(1, 1, 1, 0.6));
            renderWidget->renderWorldSpaceMesh("Meshes", meshB, Color(1, 1, 1, 0.6));

            renderWidget->renderLine("Closest Points", "Line", closestPointA, closestPointB, Color(1, 0, 0, 1));

            if(Intersection::intersect(*meshA, *meshB)) {
                std::cout << "Distance on intersection: " << distance << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    QApplication::exec();
}