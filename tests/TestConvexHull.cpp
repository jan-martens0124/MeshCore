//
// Created by tolle on 20/09/2023.
//

#include <filesystem>
#include <iostream>
#include <glm/gtc/epsilon.hpp>
#include "utility/FileParser.h"

int main() {

    std::vector<std::string> objectFolders;
    objectFolders.emplace_back("../../meshcore/datasets/Liu et al. 2015/");
    objectFolders.emplace_back("../../meshcore/datasets/Tollenaere, J. et al/Items");
    objectFolders.emplace_back("../../meshcore/datasets/Tollenaere, J. et al/Containers");

    for (const auto &folder: objectFolders){
        for (const auto &entry : std::filesystem::directory_iterator(folder)) {

            auto start = std::chrono::high_resolution_clock::now();
            auto modelSpaceMesh = FileParser::loadMeshFile(entry.path().string());
            auto end = std::chrono::high_resolution_clock::now();
            auto fileLoadingTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            start = std::chrono::high_resolution_clock::now();
            auto optionalConvexHull = modelSpaceMesh->getConvexHull();
            end = std::chrono::high_resolution_clock::now();
            auto convexHullTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            // We should always get a value
            if(!optionalConvexHull.has_value()){
                return 1;
            }

            std::cout << "Computed convex hull for " << modelSpaceMesh->getName() << " in " << convexHullTime << " ms (File loading took " << fileLoadingTime << " ms)" << std::endl;
            auto convexHull = optionalConvexHull.value();

            // The result should be convex
            if(!convexHull->isConvex()){
                return 1;
            }

            // If the original mesh is convex, the resulting volume should be the same
            if(modelSpaceMesh->isConvex() && !glm::epsilonEqual(modelSpaceMesh->getVolume()/convexHull->getVolume(), 1.0f, 1e-5f)){
                printf("%s: Original convex mesh volume: %.6f, convex hull volume: %.6f\n", modelSpaceMesh->getName().c_str(), modelSpaceMesh->getVolume(), convexHull->getVolume());
                return 1;
            }

            // The volume can never be smaller than the original mesh
            if(convexHull->getVolume()/modelSpaceMesh->getVolume() < 1.0f - 1e-5f){
                printf("%s: Original mesh volume: %.6f, convex hull volume: %.6f\n", modelSpaceMesh->getName().c_str(), modelSpaceMesh->getVolume(), convexHull->getVolume());
                return 1;
            }
        }
    }

    return 0;
}