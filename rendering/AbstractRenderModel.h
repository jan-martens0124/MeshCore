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

typedef glm::vec4 Color;

class AbstractRenderModelListener{
public:
    virtual void notifyColorChanged(const Color& newColor) const = 0;
    virtual void notifyVisibleChanged(bool visible) const = 0;
};

class RenderModelListener: public AbstractRenderModelListener {
private:
    std::function<void()> onChanged = {};
    std::function<void(const Color& newColor)> onColorChanged = {};
    std::function<void(bool visible)> onVisibleChanged = {};

    void notifyColorChanged(const Color& newColor) const override {
        if(this->onColorChanged) this->onColorChanged(newColor);
        if(this->onChanged) this->onChanged();
    }

    void notifyVisibleChanged(bool visible) const override {
        if(this->onVisibleChanged) this->onVisibleChanged(visible);
        if(this->onChanged) this->onChanged();
    }

public:

    [[maybe_unused]] void setOnChanged(const std::function<void()> &newOnChanged) {
        this->onChanged = newOnChanged;
    }

    [[maybe_unused]] void setOnColorChanged(const std::function<void(const Color& newColor)> &newOnColorChanged) {
        this->onColorChanged = newOnColorChanged;
    }

    [[maybe_unused]] void setOnVisibleChanged(const std::function<void(bool visible)> &newOnVisibleChanged) {
        this->onVisibleChanged = newOnVisibleChanged;
    }
};

class AbstractRenderModel: protected QOpenGLFunctions {

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
    explicit AbstractRenderModel(const glm::mat4& transformation);
    AbstractRenderModel(AbstractRenderModel&& other) noexcept;
    AbstractRenderModel& operator=(AbstractRenderModel&& other) noexcept;
    virtual ~AbstractRenderModel();

    virtual void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool lightMode) = 0;

    [[nodiscard]] bool isVisible() const;
    void setVisible(bool newVisible);
    [[nodiscard]] const Color &getColor() const;
    virtual void setColor(const Color &newColor);
    [[nodiscard]] const glm::mat4 &getTransformation() const;
    virtual void setTransformationMatrix(const glm::mat4 &newTransformationMatrix);

    virtual void showContextMenu(const QPoint &position){
        QMenu contextMenu(QString("Context menu"));

        QAction* visibleAction = contextMenu.addAction(QString("Visible"));
        QObject::connect(visibleAction, &QAction::triggered, [=](bool enabled){
            this->setVisible(enabled);
        });
        visibleAction->setCheckable(true);
        visibleAction->setChecked(this->isVisible());
        contextMenu.addAction(visibleAction);

        QAction* colorAction = contextMenu.addAction(QString("Color"));
        QObject::connect(colorAction, &QAction::triggered, [=](){
            auto initialColor = this->getColor();
            auto resultColor = QColorDialog::getColor(QColor(255.f*initialColor.r, 255.f*initialColor.g, 255.f*initialColor.b, 255.f*initialColor.a), nullptr, QString(), QColorDialog::ShowAlphaChannel);
            if(resultColor.isValid()){
                this->setColor(Color(resultColor.red() / 255.f, resultColor.green() / 255.f, resultColor.blue() / 255.f, resultColor.alpha() / 255.f));
            }
        });

        contextMenu.addAction(colorAction);



        contextMenu.exec(position);
    }

    void addListener(const std::shared_ptr<AbstractRenderModelListener> &listener) {
        this->listeners.push_back(listener);
    }

    void removeListener(const std::shared_ptr<AbstractRenderModelListener> &listener) {
        this->listeners.erase(std::remove(this->listeners.begin(), this->listeners.end(), listener), this->listeners.end());
    }
};

#endif //OPTIXMESHCORE_ABSTRACTRENDERMODEL_H
