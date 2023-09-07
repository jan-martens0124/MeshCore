//
// Created by Jonas on 18/05/2022.
//

#ifndef OPTIXMESHCORE_RENDERMESH_H
#define OPTIXMESHCORE_RENDERMESH_H


#include <QOpenGLShaderProgram>
#include "AbstractRenderModel.h"
#include "RenderAABB.h"
#include "RenderLine.h"

class RenderMesh: public AbstractRenderModel {
private:

    bool cullingEnabled = true;
    bool wireframeEnabled = false;
    bool surfaceEnabled = true;
    bool boundingBoxEnabled = false;
    bool axisEnabled = false;

    std::shared_ptr<QOpenGLShaderProgram> ambientShader;
    std::shared_ptr<QOpenGLShaderProgram> diffuseShader;

    RenderAABB boundingBox;
    std::vector<std::shared_ptr<RenderLine>> axisRenderLines;

    unsigned int numberOfVertices;
    unsigned int numberOfTriangles;
    float unscaledVolume;
    float unscaledSurfaceArea;

public:
//    RenderModel();
//    RenderModel(const RenderModel& other);
//    RenderMesh(RenderMesh&& other) noexcept;
    RenderMesh& operator=(RenderMesh&& other) noexcept;
    ~RenderMesh() override = default;

    RenderMesh(const WorldSpaceMesh &worldSpaceMesh,
                const std::shared_ptr<QOpenGLShaderProgram>& ambientShader,
                const std::shared_ptr<QOpenGLShaderProgram>& diffuseShader);
    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool lightMode) override;

    [[nodiscard]] bool isWireframeEnabled() const;
    [[nodiscard]] bool isCullingEnabled() const;
    [[nodiscard]] bool isBoundingBoxEnabled() const;


    void setWireframeEnabled(bool newWireframeEnabled);
    void setCullingEnabled(bool newCullingEnabled);
    void setBoundingBoxEnabled(bool newBoundingBoxEnabled);

    QMenu* getContextMenu() override;

    RenderModelDetailDialog* createRenderModelDetailDialog(QWidget* parent) override;

    void setColor(const Color &newColor) override;

    void setTransformation(const Transformation &newTransformation) override;

    const std::vector<std::shared_ptr<RenderLine>> &getAxisRenderLines() const;

    void setAxisRenderLines(const std::vector<std::shared_ptr<RenderLine>> &axisRenderLines);

    bool isAxisEnabled() const;

    void setAxisEnabled(bool axisEnabled);
};


#endif //OPTIXMESHCORE_RENDERMESH_H
