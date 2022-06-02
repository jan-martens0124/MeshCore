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

typedef glm::vec4 Color;

class AbstractRenderModelListener{
public:
    virtual void notifyNameChanged(const std::string& oldName, const std::string& newName) const = 0;
    virtual void notifyColorChanged(const Color& oldColor, const Color& newColor) const = 0;
    virtual void notifyVisibleChanged(bool oldVisible, bool newVisible) const = 0;
};

class SimpleRenderModelListener: public AbstractRenderModelListener {
private:
    std::function<void()> onChanged = {};
    std::function<void(const std::string& oldName, const std::string& newName)> onNameChanged = {};
    std::function<void(const Color& oldColor, const Color& newColor)> onColorChanged = {};
    std::function<void(bool oldVisible, bool newVisible)> onVisibleChanged = {};

    void notifyColorChanged(const Color& oldColor, const Color& newColor) const override {
        if(this->onColorChanged) this->onColorChanged(oldColor, newColor);
        if(this->onChanged) this->onChanged();
    }

    void notifyVisibleChanged(bool oldVisible, bool newVisible) const override {
        if(this->onVisibleChanged) this->onVisibleChanged(oldVisible, newVisible);
        if(this->onChanged) this->onChanged();
    }

    void notifyNameChanged(const std::string& oldName, const std::string& newName) const override {
        if(this->onNameChanged) this->onNameChanged(oldName, newName);
        if(this->onChanged) this->onChanged();
    }

public:

    [[maybe_unused]] void setOnChanged(const std::function<void()> &newOnChanged) {
        this->onChanged = newOnChanged;
    }

    [[maybe_unused]] void setOnColorChanged(const std::function<void(const Color& oldColor, const Color& newColor)> &newOnColorChanged) {
        this->onColorChanged = newOnColorChanged;
    }

    [[maybe_unused]] void setOnVisibleChanged(const std::function<void(bool oldVisible, bool newVisible)> &newOnVisibleChanged) {
        this->onVisibleChanged = newOnVisibleChanged;
    }

    [[maybe_unused]] void setOnNameChanged(const std::function<void(const std::string& oldName, const std::string& newName)> &newOnNameChanged) {
        this->onNameChanged = newOnNameChanged;
    }
};

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
    glm::mat4 transformationMatrix;
    std::vector<std::shared_ptr<AbstractRenderModelListener>> listeners;

public:
//    AbstractRenderModel() = delete;
    explicit AbstractRenderModel(const glm::mat4& transformation, const std::string& name);
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
    [[nodiscard]] const glm::mat4 &getTransformation() const;
    virtual void setTransformationMatrix(const glm::mat4 &newTransformationMatrix);

    virtual RenderModelDetailDialog* getDetailsDialog(){
        // Show existing dialog if already exists
        if(this->detailDialog==nullptr){
            this->detailDialog = new RenderModelDetailDialog(this);
        }
        return this->detailDialog;
    };

    virtual QMenu* getContextMenu();

    void addListener(const std::shared_ptr<AbstractRenderModelListener> &listener) {
        this->listeners.push_back(listener);
    }

    void removeListener(const std::shared_ptr<AbstractRenderModelListener> &listener) {
        this->listeners.erase(std::remove(this->listeners.begin(), this->listeners.end(), listener), this->listeners.end());
    }
};

#endif //OPTIXMESHCORE_ABSTRACTRENDERMODEL_H
