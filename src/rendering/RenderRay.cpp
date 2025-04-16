//
// Created by Jonas on 13/06/2024.
//

#include <glm/gtx/normal.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RenderRay.h"
#include "Exception.h"
#include "meshcore/rendering/OpenGLWidget.h"
#include "meshcore/core/IndexTriangle.h"

RenderRay::RenderRay(const Ray& ray, const Transformation &transformation, float widthLengthRatio): AbstractRenderModel(transformation, "Ray"){

    // Create vertices and triangles for an arrow pointing in the z direction
    std::vector<Vertex> arrowSpaceVertices;
    std::vector<IndexTriangle> arrowTriangles;
    {
        // Parameters
        const auto sectorCount = 32;

        const auto length = glm::length(ray.direction);
        const auto radius = length * widthLengthRatio / 2.0f;
        const auto headWidthFactor = 3.0f;
        const auto headBaseFactor = 1-(widthLengthRatio *6);

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
            arrowSpaceVertices.emplace_back(x, y, headBaseFactor * length);
        }

        // Create triangles on the bottom circle
        for (size_t i = 0; i < sectorCount; ++i){
            arrowTriangles.emplace_back(bottomCircleOffset + i, 0, bottomCircleOffset + (i +1)%sectorCount);
        }

        // Create triangles on the shaft
        for (size_t i = 0; i < sectorCount; ++i){
            arrowTriangles.emplace_back(bottomCircleOffset + i, bottomCircleOffset + (i +1)%sectorCount, middleCircleOffset + i);
            arrowTriangles.emplace_back(middleCircleOffset + (i +1)%sectorCount, middleCircleOffset + i, bottomCircleOffset + (i +1)%sectorCount);
        }

        // Connect the middle to the outer circle
        for (size_t i = 0; i < sectorCount; ++i){
            arrowTriangles.emplace_back(middleCircleOffset + i, middleCircleOffset + (i +1)%sectorCount, outerCircleOffset + i);
            arrowTriangles.emplace_back(outerCircleOffset + (i +1)%sectorCount, outerCircleOffset + i, middleCircleOffset + (i +1)%sectorCount);
        }

        // Connect the outer circle to the top
        for (size_t i = 0; i < sectorCount; ++i){
            arrowTriangles.emplace_back(outerCircleOffset + i, outerCircleOffset + (i +1)%sectorCount, 1);
        }
    }

    // Transform the arrow vertices to align with the ray direction
    {
        Transformation rayTransformation;
        rayTransformation.setPosition(ray.origin);

        glm::vec3 rotationAxis = glm::cross(ray.direction, glm::vec3(0,0,1));

        if(glm::length(rotationAxis)> 1e-4f){
            float rotationAngle = -glm::acos(glm::dot(glm::normalize(ray.direction), glm::vec3(0,0,1)));
            rayTransformation.setRotation(Quaternion(rotationAxis, rotationAngle));
        }

        for (auto &vertex: arrowSpaceVertices){
            vertex = rayTransformation.transformVertex(vertex);
        }
    }

    // Data for triangles
    std::vector<unsigned int> indices;
    std::vector<float> data;
    for(const auto& t : arrowTriangles) {
        std::array<Vertex, 3> triangleVertices{arrowSpaceVertices[t.vertexIndex0], arrowSpaceVertices[t.vertexIndex1], arrowSpaceVertices[t.vertexIndex2]};
        glm::vec3 normal = glm::triangleNormal(triangleVertices[0], triangleVertices[1], triangleVertices[2]);

        // We push each model space vertex once for each triangle because the normal is different for each triangle
        // (Adding up the normals in the shader doesn't provide visually satisfying results
        for (const auto &triangleVertex: triangleVertices){
            data.emplace_back(triangleVertex.x);
            data.emplace_back(triangleVertex.y);
            data.emplace_back(triangleVertex.z);
            data.emplace_back(normal.x);
            data.emplace_back(normal.y);
            data.emplace_back(normal.z);

            indices.emplace_back(indices.size());
        }
    }

    this->vertexBuffer->create();
    this->vertexBuffer->bind();
    this->vertexBuffer->allocate(&data.front(), data.size() * sizeof(float));

    this->vertexArray->create();
    this->vertexArray->bind();

    this->initializeOpenGLFunctions();

    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glEnableVertexAttribArray(1));

    this->indexBuffer->create();
    this->indexBuffer->bind();
    this->indexBuffer->allocate(&indices.front(), indices.size() * sizeof(unsigned int));

    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr));
    GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat))));
}

void RenderRay::draw(const OpenGLWidget *openGLWidget, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) {

    if(this->isVisible()){

        this->initializeOpenGLFunctions();
        this->vertexArray->bind();
        this->indexBuffer->bind();

        GL_CALL(glEnable(GL_CULL_FACE));
        GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

        const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * this->getTransformationMatrix();
        glm::vec3 viewSpaceLightDirection = glm::vec4(0, 0, 1, 1) * viewMatrix;
        const glm::vec3 modelLightDirection = glm::vec3(glm::vec4(viewSpaceLightDirection, 1.0f) *
                                                        this->getTransformationMatrix());
        glm::vec3 cameraPosition = glm::inverse(viewMatrix) * glm::vec4(0,0,0,1000);
        glm::vec3 modelSpaceCameraPosition = glm::vec3(glm::inverse(this->getTransformationMatrix()) * glm::vec4(cameraPosition, 1.0f));
        const float ambientLighting = 0.25f;
        const auto& material = this->getMaterial();

        auto diffuseColor = material.getDiffuseColor();
        auto specularColor = material.getSpecularColor();

        auto& shader = openGLWidget->getPhongShader();

        shader->bind();
        shader->setUniformValue("u_ModelViewProjectionMatrix", QMatrix4x4(glm::value_ptr(modelViewProjectionMatrix)).transposed());
        shader->setUniformValue("u_LightDirection", QVector3D(modelLightDirection.x, modelLightDirection.y, modelLightDirection.z));
        shader->setUniformValue("u_ViewPosition", QVector3D(modelSpaceCameraPosition.x, modelSpaceCameraPosition.y, modelSpaceCameraPosition.z));
        shader->setUniformValue("u_AmbientIntensity", ambientLighting);
        shader->setUniformValue("u_DiffuseIntensity", 1.0f - ambientLighting);
        shader->setUniformValue("u_SpecularIntensity", 0.5f);
        shader->setUniformValue("u_DiffuseColor", QVector4D(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a));
        shader->setUniformValue("u_SpecularColor", QVector4D(specularColor.r, specularColor.g, specularColor.b, specularColor.a));
        shader->setUniformValue("u_SpecularPower", 8.0f);

        GL_CALL(glDrawElements(GL_TRIANGLES, this->indexBuffer->size()/sizeof(unsigned int), GL_UNSIGNED_INT, nullptr));

    }
}

void RenderRay::setMaterial(const PhongMaterial &material) {
    AbstractRenderModel::setMaterial(material);
}

void RenderRay::setTransformation(const Transformation &transformation) {
    AbstractRenderModel::setTransformation(transformation);
}
