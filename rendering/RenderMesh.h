//
// Created by Jonas on 18/05/2022.
//

#ifndef OPTIXMESHCORE_RENDERMESH_H
#define OPTIXMESHCORE_RENDERMESH_H


#include <QOpenGLShaderProgram>
#include "AbstractRenderModel.h"
#include "RenderAABB.h"

class RenderMesh: public AbstractRenderModel {
private:

    bool cullingEnabled = true;
    bool wireframeEnabled = false;
    bool boundingBoxEnabled = false;

    std::shared_ptr<QOpenGLShaderProgram> ambientShader;
    std::shared_ptr<QOpenGLShaderProgram> diffuseShader;

    QOpenGLBuffer* indexBuffer;
    QOpenGLBuffer* vertexBuffer;
    QOpenGLVertexArrayObject* vertexArray;

    RenderAABB boundingBox;

public:

//    RenderModel();
//    RenderModel(const RenderModel& other);
//    RenderMesh(RenderMesh&& other) noexcept;
    RenderMesh& operator=(RenderMesh&& other) noexcept;
    ~RenderMesh() override;

    RenderMesh(const WorldSpaceMesh &worldSpaceMesh,
                const std::shared_ptr<QOpenGLShaderProgram>& ambientShader,
                std::shared_ptr<QOpenGLShaderProgram> diffuseShader);
    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool lightMode);

    [[nodiscard]] bool isWireframeEnabled() const;
    [[nodiscard]] bool isCullingEnabled() const;
    [[nodiscard]] bool isBoundingBoxEnabled() const;


    void setWireframeEnabled(bool newWireframeEnabled);
    void setCullingEnabled(bool newCullingEnabled);
    void setBoundingBoxEnabled(bool newBoundingBoxEnabled);

};


#endif //OPTIXMESHCORE_RENDERMESH_H
