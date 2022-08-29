//
// Created by Jonas on 2/06/2022.
//
#include <iostream>
#include <QtWidgets>
#include <utility/FileParser.h>
#include "rendering/ApplicationWindow.h"
#include "rendering/RenderBoxTree.h"
#include <thread>
#include "acceleration/AABBOctree.h"

void run(RenderWidget* renderWidget);

int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    NewApplicationWindow window;
    window.show();

    std::thread thread(run, window.getRenderWidget());
    int returnCode = QApplication::exec();
    thread.join();
    return returnCode;
}

void run(RenderWidget* renderWidget){

    // Load the item
    std::shared_ptr<WorldSpaceMesh> itemWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../../data/models/Everton/banana.stl"));

    // Load a container
    std::shared_ptr<WorldSpaceMesh> containerWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(FileParser::loadMeshFile("../../data/models/rocks/rock_008k.obj"));

    containerWorldSpaceMesh->getModelTransformation().factorScale(10.0f);

    containerWorldSpaceMesh->getModelTransformation().deltaPosition(-10.0f*containerWorldSpaceMesh->getModelSpaceMesh()->getBounds().getCenter()); // Put the container in the center of the world

    // Make the item small enough to fit inside the container
    itemWorldSpaceMesh->getModelTransformation().setScale(0.5f);

    // Pas them to the renderer
    renderWidget->renderWorldSpaceMesh("Items", itemWorldSpaceMesh, Color(1,1,0,1));
    renderWidget->renderWorldSpaceMesh("Container", containerWorldSpaceMesh, Color(1, 1, 1, 0.7));

    // Create the Octree which will speed up intersection tests with the container
    AABBOctree octree(containerWorldSpaceMesh->getModelSpaceMesh());
    auto renderTree = std::make_shared<RenderBoxTree>(octree, Transformation(), renderWidget->getOpenGLWidget()->getAmbientShader());
    renderTree->setVisible(false);
    renderWidget->addOrUpdateRenderModel("Container", "renderTree0", renderTree);

    // Test new random transformations for the item
    Random random;
    for(int i=0; i<1e5; i++){

        // 0. Sample a random change to position and orientation
        const Transformation originalTransformation = itemWorldSpaceMesh->getModelTransformation();

        itemWorldSpaceMesh->getModelTransformation().deltaPosition(glm::vec3(random.nextFloat()-0.5f, random.nextFloat()-0.5f, random.nextFloat()-0.5f));
        itemWorldSpaceMesh->getModelTransformation().deltaYaw(random.nextFloat()-0.5f);
        itemWorldSpaceMesh->getModelTransformation().deltaPitch(random.nextFloat()-0.5f);
        itemWorldSpaceMesh->getModelTransformation().deltaRoll(random.nextFloat()-0.5f);

        // 1. Check if at least one of the vertices is inside the container
        {
            // Transform the vertex to the right coordinate space, a good explanation about transformation matrices and coordinate spaces can be found here:
            // https://www.tapatalk.com/groups/ultimate3d_community/understanding-transformation-matrices-t5483.html
            const auto itemModelSpaceVertex = itemWorldSpaceMesh->getModelSpaceMesh()->getVertices().at(0); // This is the first vertex in the coordinate system of the item
            const auto worldSpaceVertex = itemWorldSpaceMesh->getModelTransformation().transformVertex(itemModelSpaceVertex); // This is the coordinate of the vertex where it is actually located in the world
            const auto containerModelSpaceVertex = containerWorldSpaceMesh->getModelTransformation().inverseTransformVertex(worldSpaceVertex); // This is the coordinate of the vertex when you bring it into the coordinate system of the container

            // A ray that starts in this vertex should intersect the outer container an uneven number of times if it's inside the container
            Ray ray(containerModelSpaceVertex, glm::vec3(0,0,1)); // Direction doesn't matter
            auto numberOfIntersections = octree.getNumberOfRayIntersections(ray);
            if(numberOfIntersections % 2 == 0){
                // This position is not feasible, revert to the previous transformation and go to the next iteration
                itemWorldSpaceMesh->getModelTransformation() = originalTransformation;
                continue;
            }

        }

        // 2. Check if none of the triangles of the item intersect the container
        {

            // Here we calculate a single matrix that can transform points from the coordinate system of the item to the coordinate system of the container
            const auto itemToContainerTransformation = containerWorldSpaceMesh->getModelTransformation().getInverseMatrix() * itemWorldSpaceMesh->getModelTransformation().getMatrix();

            const auto vertices = itemWorldSpaceMesh->getModelSpaceMesh()->getVertices();
            for (const auto &indexTriangle: itemWorldSpaceMesh->getModelSpaceMesh()->getTriangles()){

                // The IndexTriangle class stores only the indices of the vertices that make up the triangle, the VertexTriangle class stores actual vertices
                const auto vertexTriangle = VertexTriangle(vertices.at(indexTriangle.vertexIndex0), vertices.at(indexTriangle.vertexIndex1), vertices.at(indexTriangle.vertexIndex2));

                // Transform the triangle to the coordinate system of the container, can be used correctly for the intersection test with the octree
                const auto transformedTriangle = vertexTriangle.getTransformed(itemToContainerTransformation);

                // Check if the triangle intersects the container
                bool intersects = octree.intersectsTriangle(transformedTriangle);

                if(intersects){
                    // This position is not feasible, revert to the previous transformation and go to the next iteration
                    itemWorldSpaceMesh->getModelTransformation() = originalTransformation;
                    break;
                }

            }
        }

        // 3. If we reach this point the position is feasible, update the transformation of the item and render it
        renderWidget->renderWorldSpaceMesh("Items", itemWorldSpaceMesh, Color(1,1,0,1));

    }

    std::cout << "Finished" << std::endl;
}