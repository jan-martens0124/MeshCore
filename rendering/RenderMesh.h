//
// Created by Jonas on 18/05/2022.
//

#ifndef OPTIXMESHCORE_RENDERMESH_H
#define OPTIXMESHCORE_RENDERMESH_H


#include <QOpenGLShaderProgram>
#include "AbstractRenderModel.h"
#include "RenderAABB.h"
#include "RenderLine.h"
#include "PhongMaterial.h"

class RenderMesh: public AbstractRenderModel {
private:

    bool cullingEnabled = true;
    bool wireframeEnabled = false;
    bool surfaceEnabled = true;
    bool boundingBoxEnabled = false;
    bool axisEnabled = false;

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

    RenderMesh(const WorldSpaceMesh &worldSpaceMesh);
    void draw(const OpenGLWidget* openGLWidget, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool lightMode) override;

    [[nodiscard]] bool isWireframeEnabled() const;
    [[nodiscard]] bool isCullingEnabled() const;
    [[nodiscard]] bool isBoundingBoxEnabled() const;


    void setWireframeEnabled(bool newWireframeEnabled);
    void setCullingEnabled(bool newCullingEnabled);
    void setBoundingBoxEnabled(bool newBoundingBoxEnabled);

    QMenu* getContextMenu() override;

    RenderModelDetailDialog* createRenderModelDetailDialog(QWidget* parent) override;

    void setMaterial(const PhongMaterial& newMaterial) override;

    void setTransformation(const Transformation &newTransformation) override;

    bool isAxisEnabled() const;

    void setAxisEnabled(bool axisEnabled);
};


#endif //OPTIXMESHCORE_RENDERMESH_H
