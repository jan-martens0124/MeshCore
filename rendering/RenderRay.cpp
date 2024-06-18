//
// Created by Jonas on 13/06/2024.
//

#include "RenderRay.h"

RenderRay::RenderRay(const Ray& ray, const Transformation &transformation, float widthLengthRatio): AbstractRenderModel(transformation, "Ray"){

    // Create vertices and triangles for an arrow pointing in the z direction
    std::vector<Vertex> arrowSpaceVertices;
    std::vector<IndexTriangle> arrowTriangles;
    {
        // Parameters
        const auto sectorCount = 32;

        const auto length = glm::length(ray.direction);
        const auto radius = length * widthLengthRatio / 3.0f;
        const auto headWidthFactor = 3.0f;
        const auto headBaseFactor = 0.66f;

        // Top and bottom vertex
        arrowSpaceVertices.push_back(Vertex{glm::vec3(0.0f, 0.0f, 0.0f)});
        arrowSpaceVertices.push_back(Vertex{glm::vec3(0.0f, 0.0f, length)});

        // Create the bottom circle of vertices
        const auto bottomCircleOffset = arrowSpaceVertices.size();
        for (size_t i = 0; i < sectorCount; ++i){
            const auto x = radius * std::cos(2 * glm::pi<float>() * float(i) / sectorCount);
            const auto y = radius * std::sin(2 * glm::pi<float>() * float(i) / sectorCount);
            arrowSpaceVertices.push_back(Vertex{glm::vec3(x, y, 0.0f)});
        }

        // Create the middle circle of vertices
        const auto middleCircleOffset = arrowSpaceVertices.size();
        for (size_t i = 0; i < sectorCount; ++i){
            const auto x = radius * std::cos(2 * glm::pi<float>() * float(i) / sectorCount);
            const auto y = radius * std::sin(2 * glm::pi<float>() * float(i) / sectorCount);
            arrowSpaceVertices.push_back(Vertex{glm::vec3(x, y, headBaseFactor * length)});
        }

        // Create the outer circle of vertices
        const auto outerCircleOffset = arrowSpaceVertices.size();
        for (size_t i = 0; i < sectorCount; ++i){
            const auto x = headWidthFactor * radius * std::cos(2 * glm::pi<float>() * float(i) / sectorCount);
            const auto y = headWidthFactor * radius * std::sin(2 * glm::pi<float>() * float(i) / sectorCount);
            arrowSpaceVertices.push_back(Vertex{glm::vec3(x, y, headBaseFactor * length)});
        }

        // Create triangles on the bottom circle
        for (size_t i = 0; i < sectorCount; ++i){
            arrowTriangles.push_back(IndexTriangle{bottomCircleOffset + i, 0, bottomCircleOffset + (i +1)%sectorCount});
        }

        // Create triangles on the shaft
        for (size_t i = 0; i < sectorCount; ++i){
            arrowTriangles.push_back(IndexTriangle{bottomCircleOffset + i, bottomCircleOffset + (i +1)%sectorCount, middleCircleOffset + i});
            arrowTriangles.push_back(IndexTriangle{middleCircleOffset + (i +1)%sectorCount, middleCircleOffset + i, bottomCircleOffset + (i +1)%sectorCount});
        }

        // Connect the middle to the outer circle
        for (size_t i = 0; i < sectorCount; ++i){
            arrowTriangles.push_back(IndexTriangle{middleCircleOffset + i, middleCircleOffset + (i +1)%sectorCount, outerCircleOffset + i});
            arrowTriangles.push_back(IndexTriangle{outerCircleOffset + (i +1)%sectorCount, outerCircleOffset + i, middleCircleOffset + (i +1)%sectorCount});
        }

        // Connect the outer circle to the top
        for (size_t i = 0; i < sectorCount; ++i){
            arrowTriangles.push_back(IndexTriangle{outerCircleOffset + i, outerCircleOffset + (i +1)%sectorCount, 1});
        }
    }

    // Transform the arrow vertices to align with the ray direction
    {
        Transformation rayTransformation;
        auto rotationAxis = glm::cross(ray.direction, glm::vec3(0,0,1));
        auto rotationAngle = -glm::acos(glm::dot(glm::normalize(ray.direction), glm::vec3(0,0,1)));
        rayTransformation.setRotation(Quaternion(rotationAxis, rotationAngle));
        rayTransformation.setPosition(ray.origin);

        for (auto &vertex: arrowSpaceVertices){
            vertex = rayTransformation.transformVertex(vertex);
        }
    }

    // Create the arrow mesh
    {
        auto arrowModelSpaceMesh = std::make_shared<ModelSpaceMesh>(arrowSpaceVertices, arrowTriangles);
        auto arrowWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(arrowModelSpaceMesh);
        this->arrowMesh = std::make_shared<RenderMesh>(*arrowWorldSpaceMesh);
    }
}

void RenderRay::draw(const OpenGLWidget *openGLWidget, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) {
    if(this->isVisible()){
        this->arrowMesh->draw(openGLWidget, viewMatrix, projectionMatrix, lightMode);
    }
}

void RenderRay::setMaterial(const PhongMaterial &material) {
    AbstractRenderModel::setMaterial(material);
    this->arrowMesh->setMaterial(material);
}