//
// Created by Jonas on 1/12/2020.
//

#ifndef MESHCORE_RENDERMODEL_H
#define MESHCORE_RENDERMODEL_H

#include <glm/glm.hpp>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include "../core/WorldSpaceMesh.h"
#include "Exception.h"
#include "RenderAABB.h"

typedef glm::vec4 Color;

class RenderModel: protected QOpenGLFunctions {
private:

    bool cullingEnabled = true;
    bool wireframeEnabled = false;
    bool visible = true;
    bool boundingBoxEnabled = false;

    std::shared_ptr<QOpenGLShaderProgram> ambientShader;
    std::shared_ptr<QOpenGLShaderProgram> diffuseShader;

    QOpenGLBuffer* indexBuffer;
    QOpenGLBuffer* vertexBuffer;
    QOpenGLVertexArrayObject* vertexArray;
    Color color;
    glm::mat4 transformation;

    RenderAABB boundingBox;

public:
    void setTransformationMatrix(const glm::mat4 &transformationMatrix);

//    RenderModel();
//    RenderModel(const RenderModel& other);
    RenderModel(RenderModel&& other) noexcept;
    RenderModel& operator=(RenderModel&& other) noexcept;
    ~RenderModel();

    RenderModel(const WorldSpaceMesh &worldSpaceMesh,
                const std::shared_ptr<QOpenGLShaderProgram>& ambientShader,
                std::shared_ptr<QOpenGLShaderProgram> diffuseShader);
    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool lightMode);

    [[nodiscard]] bool isWireframeEnabled() const;
    [[nodiscard]] bool isCullingEnabled() const;
    [[nodiscard]] bool isVisible() const;
    [[nodiscard]] const Color &getColor() const;
    [[nodiscard]] bool isBoundingBoxEnabled() const;


    void setWireframeEnabled(bool wireframeEnabled);
    void setCullingEnabled(bool cullingEnabled);
    void setVisible(bool visible);
    void setColor(const Color& c);
    void setBoundingBoxEnabled(bool boundingBoxEnabled);

};


#endif //MESHCORE_RENDERMODEL_H
