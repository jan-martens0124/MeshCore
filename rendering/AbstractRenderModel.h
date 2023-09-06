//
// Created by Jonas on 8/11/2021.
//

#ifndef OPTIXMESHCORE_ABSTRACTRENDERMODEL_H
#define OPTIXMESHCORE_ABSTRACTRENDERMODEL_H

#include <QOpenGLFunctions>
#include <glm/glm.hpp>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <qmenu.h>
#include <qaction.h>
#include <qcolordialog.h>
#include "RenderModelDetailDialog.h"
#include "Color.h"
#include "RenderListeners.h"

class AbstractRenderModel: protected QOpenGLFunctions {

private:
    std::string name;
    RenderModelDetailDialog* detailDialog = nullptr;

private:
    bool visible = true;
    Color color = Color(1.0f);

protected:
    QOpenGLBuffer* indexBuffer;
    QOpenGLBuffer* vertexBuffer;
    QOpenGLVertexArrayObject* vertexArray;
    Transformation transformation;
    std::vector<std::shared_ptr<AbstractRenderModelListener>> listeners;

public:
//    AbstractRenderModel() = delete;
    explicit AbstractRenderModel(const Transformation& transformation, const std::string& name);
    AbstractRenderModel(AbstractRenderModel&& other) noexcept;
    AbstractRenderModel& operator=(AbstractRenderModel&& other) noexcept;
    virtual ~AbstractRenderModel();

    virtual void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool lightMode) = 0;

public:
    [[nodiscard]] const std::string &getName() const;
    void setName(const std::string &newName);
    [[nodiscard]] bool isVisible() const;
    void setVisible(bool newVisible);
    [[nodiscard]] const Color &getColor() const;
    virtual void setColor(const Color &newColor);
    [[nodiscard]] glm::mat4 getTransformationMatrix() const;
    virtual void setTransformation(const Transformation &transformation);
    const Transformation& getTransformation() const;
    Transformation& getTransformation();

    RenderModelDetailDialog* getDetailsDialog(QWidget* parent);
    virtual RenderModelDetailDialog* createRenderModelDetailDialog(QWidget* parent);
    virtual QMenu* getContextMenu();

    void addListener(const std::shared_ptr<AbstractRenderModelListener> &listener) {
        this->listeners.push_back(listener);
    }

    void removeListener(const std::shared_ptr<AbstractRenderModelListener> &listener) {
        this->listeners.erase(std::remove(this->listeners.begin(), this->listeners.end(), listener), this->listeners.end());
    }
};

#endif //OPTIXMESHCORE_ABSTRACTRENDERMODEL_H
