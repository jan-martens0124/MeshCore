//
// Created by Jonas Tollenaere on 4/11/2021.
//

#include <iostream>
#include <QtWidgets>
#include <thread>
#include "rendering/ApplicationWindow.h"
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

    /// Here is where you would start code running on a separate thread

    // Load some meshes
    std::shared_ptr<WorldSpaceMesh> bananaWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/E. F. Silva et al. 2021/banana.stl"));
    std::shared_ptr<WorldSpaceMesh> rockWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/E. F. Silva et al. 2021/stone_1.obj"));

    std::cout << "Number of vertices: " << bananaWorldSpaceMesh->getModelSpaceMesh()->getVertices().size() << std::endl;
    std::cout << "Number of vertices: " << rockWorldSpaceMesh->getModelSpaceMesh()->getVertices().size() << std::endl;

    glm::vec3 position(10.0f,0,0);

    bananaWorldSpaceMesh->getModelTransformation().setPosition(position);
    bananaWorldSpaceMesh->getModelTransformation().setYaw(1.0f);
    rockWorldSpaceMesh->getModelTransformation().setScale(0.5f);
    rockWorldSpaceMesh->getModelTransformation().setPosition(glm::vec3(0,0,-15));

    // Pass them to the renderer
    renderWidget->renderWorldSpaceMesh("Meshes", rockWorldSpaceMesh, Color(0.8, 0.8, 0.8, 0.6));
    renderWidget->renderWorldSpaceMesh("Meshes", bananaWorldSpaceMesh, Color(0.75, 0.75, 0, 1));

    // Render a primitive shape as well
    renderWidget->renderSphere("Sphere", "Sphere2", Sphere(glm::vec3(-15.0f, 0.0f, 0.0f), 5.0f), Color(1.0f, 0.2f, 0.2f, 0.5f));


//    {
//        std::shared_ptr<WorldSpaceMesh> testMesh = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/nonconvex/stl/polytope1.stl"));
//    }
//
//    {
//        std::shared_ptr<WorldSpaceMesh> testA = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2005/polytope1.obj"));
//        std::shared_ptr<WorldSpaceMesh> testB = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2005/polytope2.obj"));
//        std::shared_ptr<WorldSpaceMesh> testC = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2005/polytope3.obj"));
//        std::shared_ptr<WorldSpaceMesh> testD = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2005/polytope4.obj"));
//        std::shared_ptr<WorldSpaceMesh> testE = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2005/polytope5.obj"));
//        std::shared_ptr<WorldSpaceMesh> testF = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2005/polytope6.obj"));
//        std::shared_ptr<WorldSpaceMesh> testG = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2005/polytope7.obj"));
//
//        assert(testA->getModelSpaceMesh()->isConvex());
//        assert(testB->getModelSpaceMesh()->isConvex());
//        assert(testC->getModelSpaceMesh()->isConvex());
//        assert(testD->getModelSpaceMesh()->isConvex());
//        assert(testE->getModelSpaceMesh()->isConvex());
//        assert(testF->getModelSpaceMesh()->isConvex());
//        assert(testG->getModelSpaceMesh()->isConvex());
//
//        // Render them
//        renderWidget->renderWorldSpaceMesh("Stoyan 2005", testA, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2005", testB, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2005", testC, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2005", testD, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2005", testE, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2005", testF, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2005", testG, Color(0.8, 0.8, 0.8, 1.0));
//    }
//
//
//    {
//        std::shared_ptr<WorldSpaceMesh> testA = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/convex/polytope1.obj"));
//        std::shared_ptr<WorldSpaceMesh> testB = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/convex/polytope2.obj"));
//        std::shared_ptr<WorldSpaceMesh> testC = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/convex/polytope3.obj"));
//        std::shared_ptr<WorldSpaceMesh> testD = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/convex/polytope4.obj"));
//        std::shared_ptr<WorldSpaceMesh> testE = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/convex/polytope5.obj"));
//
////        assert(testA->getModelSpaceMesh()->isConvex());
//        assert(testB->getModelSpaceMesh()->isConvex());
//        assert(testC->getModelSpaceMesh()->isConvex());
//        assert(testD->getModelSpaceMesh()->isConvex());
//        assert(testE->getModelSpaceMesh()->isConvex());
//
//        // Render them
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Convex", testA, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Convex", testB, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Convex", testC, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Convex", testD, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Convex", testE, Color(0.8, 0.8, 0.8, 1.0));
//    }
//
//    {
//        std::shared_ptr<WorldSpaceMesh> testA = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/nonconvex/polytope1.obj"));
//        std::shared_ptr<WorldSpaceMesh> testB = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/nonconvex/polytope2.obj"));
//        std::shared_ptr<WorldSpaceMesh> testC = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/nonconvex/polytope3.obj"));
//        std::shared_ptr<WorldSpaceMesh> testD = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/nonconvex/polytope4.obj"));
//        std::shared_ptr<WorldSpaceMesh> testE = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/nonconvex/polytope5.obj"));
//        std::shared_ptr<WorldSpaceMesh> testF = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/nonconvex/polytope6.obj"));
//        std::shared_ptr<WorldSpaceMesh> testG = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/nonconvex/polytope7.obj"));
//        std::shared_ptr<WorldSpaceMesh> testH = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/nonconvex/polytope8.obj"));
//        std::shared_ptr<WorldSpaceMesh> testI = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/nonconvex/polytope9.obj"));
//        std::shared_ptr<WorldSpaceMesh> testJ = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets/Stoyan et al. 2004/nonconvex/polytope10.obj"));
//
//
//        // Render them
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testA, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testB, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testC, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testD, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testE, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testF, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testG, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testH, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testI, Color(0.8, 0.8, 0.8, 1.0));
//        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testJ, Color(0.8, 0.8, 0.8, 1.0));
//    }

    {
        std::shared_ptr<WorldSpaceMesh> testA = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets\\Stoyan et al. 2004\\nonconvex\\stl\\Stoyan_dataset\\Stoyan\\Stoyan1.stl"));
        std::shared_ptr<WorldSpaceMesh> testB = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets\\Stoyan et al. 2004\\nonconvex\\stl\\Stoyan_dataset\\Stoyan\\Stoyan2.stl"));
        std::shared_ptr<WorldSpaceMesh> testC = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets\\Stoyan et al. 2004\\nonconvex\\stl\\Stoyan_dataset\\Stoyan\\Stoyan3.stl"));
        std::shared_ptr<WorldSpaceMesh> testD = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets\\Stoyan et al. 2004\\nonconvex\\stl\\Stoyan_dataset\\Stoyan\\Stoyan4.stl"));
        std::shared_ptr<WorldSpaceMesh> testE = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets\\Stoyan et al. 2004\\nonconvex\\stl\\Stoyan_dataset\\Stoyan\\Stoyan5.stl"));
        std::shared_ptr<WorldSpaceMesh> testF = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets\\Stoyan et al. 2004\\nonconvex\\stl\\Stoyan_dataset\\Stoyan\\Stoyan6.stl"));
        std::shared_ptr<WorldSpaceMesh> testG = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets\\Stoyan et al. 2004\\nonconvex\\stl\\Stoyan_dataset\\Stoyan\\Stoyan7.stl"));
        std::shared_ptr<WorldSpaceMesh> testH = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets\\Stoyan et al. 2004\\nonconvex\\stl\\Stoyan_dataset\\Stoyan\\Stoyan8.stl"));
        std::shared_ptr<WorldSpaceMesh> testI = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets\\Stoyan et al. 2004\\nonconvex\\stl\\Stoyan_dataset\\Stoyan\\Stoyan9.stl"));
        std::shared_ptr<WorldSpaceMesh> testJ = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../datasets\\Stoyan et al. 2004\\nonconvex\\stl\\Stoyan_dataset\\Stoyan\\Stoyan10.stl"));


        // Render them
        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testA, Color(0.8, 0.8, 0.8, 1.0));
        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testB, Color(0.8, 0.8, 0.8, 1.0));
        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testC, Color(0.8, 0.8, 0.8, 1.0));
        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testD, Color(0.8, 0.8, 0.8, 1.0));
        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testE, Color(0.8, 0.8, 0.8, 1.0));
        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testF, Color(0.8, 0.8, 0.8, 1.0));
        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testG, Color(0.8, 0.8, 0.8, 1.0));
        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testH, Color(0.8, 0.8, 0.8, 1.0));
        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testI, Color(0.8, 0.8, 0.8, 1.0));
        renderWidget->renderWorldSpaceMesh("Stoyan 2004 Non-Convex", testJ, Color(0.8, 0.8, 0.8, 1.0));
    }

}