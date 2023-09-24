//
// Created by Jonas on 18/07/2022.
//

#ifndef OPTIXMESHCORE_RENDERSPHERE_H
#define OPTIXMESHCORE_RENDERSPHERE_H

#include <QOpenGLShaderProgram>
#include "AbstractRenderModel.h"
#include "../core/Sphere.h"

class RenderSphere: public AbstractRenderModel { // Due to this AbstractRenderModel inheritance,
private:

    class SingletonUnitySphere: QOpenGLFunctions{
    private:
        SingletonUnitySphere();
    public:
        static std::shared_ptr<SingletonUnitySphere>& getInstance();
        QOpenGLBuffer* indexBuffer;
        QOpenGLBuffer* vertexBuffer;
        QOpenGLVertexArrayObject* vertexArray;
    };

    const Sphere sphere;

    bool cullingEnabled = true;
    bool wireframeEnabled = false;

    std::shared_ptr<QOpenGLShaderProgram> ambientShader;
    std::shared_ptr<QOpenGLShaderProgram> diffuseShader;

public:

    RenderSphere(const Sphere &sphere,
                 const Transformation& transformation,
                 const std::shared_ptr<QOpenGLShaderProgram>& ambientShader,
                 const std::shared_ptr<QOpenGLShaderProgram>& diffuseShader);

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, bool lightMode) override;

    RenderModelDetailDialog *createRenderModelDetailDialog(QWidget *parent) override;

    QMenu *getContextMenu() override;

    bool isCullingEnabled() const;

    void setCullingEnabled(bool newCullingEnabled);

    bool isWireframeEnabled() const;

    void setWireframeEnabled(bool newWireframeEnabled);

};


#endif //OPTIXMESHCORE_RENDERSPHERE_H
