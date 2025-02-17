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
#include "RenderRay.h"

class RenderMesh: public AbstractRenderModel {
private:

    enum MeshTexture {
        DEFAULT,
        TRIANGLES,
        FACES
    };

    QOpenGLBuffer* faceIndexBuffer;
    QOpenGLBuffer* faceVertexBuffer;
    QOpenGLVertexArrayObject* faceVertexArray;

    bool cullingEnabled = true;
    bool wireframeEnabled = false;
    bool surfaceEnabled = true;
    bool boundingBoxEnabled = false;
    bool axisEnabled = false;
    bool normalsEnabled = false;

    MeshTexture renderedTexture = MeshTexture::DEFAULT;

    RenderAABB boundingBox;
    std::vector<std::shared_ptr<RenderLine>> axisRenderLines;
    std::vector<std::shared_ptr<RenderRay>> normalRenderRays;
    std::vector<Ray> normalRays;

    unsigned int numberOfVertices;
    unsigned int numberOfFaces;
    unsigned int numberOfTriangles;
    float unscaledVolume;
    float unscaledSurfaceArea;

public:
//    RenderModel();
//    RenderModel(const RenderModel& other);
//    RenderMesh(RenderMesh&& other) noexcept;
    RenderMesh& operator=(RenderMesh&& other) noexcept;
    ~RenderMesh() override = default;

    explicit RenderMesh(const WorldSpaceMesh &worldSpaceMesh);
    void draw(const OpenGLWidget* openGLWidget, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool lightMode) override;

    [[nodiscard]] bool isWireframeEnabled() const;
    [[nodiscard]] bool isCullingEnabled() const;
    [[nodiscard]] bool isBoundingBoxEnabled() const;
    [[nodiscard]] bool isAxisEnabled() const;
    [[nodiscard]] bool isSurfaceEnabled() const;
    [[nodiscard]] bool isNormalsEnabled() const;
    [[nodiscard]] MeshTexture getRenderedTexture() const;

    void setWireframeEnabled(bool newWireframeEnabled);
    void setCullingEnabled(bool newCullingEnabled);
    void setBoundingBoxEnabled(bool newBoundingBoxEnabled);
    void setAxisEnabled(bool axisEnabled);
    void setSurfaceEnabled(bool newSurfaceEnabled);
    void setNormalsEnabled(bool newNormalsEnabled);
    void setRenderedTexture(MeshTexture newRenderedTexture);

    QMenu* getContextMenu() override;

    RenderModelDetailDialog* createRenderModelDetailDialog(QWidget* parent) override;

    void setMaterial(const PhongMaterial& newMaterial) override;
    void setTransformation(const Transformation &newTransformation) override;

    void initializeNormals();
};


#endif //OPTIXMESHCORE_RENDERMESH_H
