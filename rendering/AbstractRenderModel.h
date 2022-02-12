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

class AbstractRenderModel: protected QOpenGLFunctions {

protected:
    QOpenGLBuffer* indexBuffer;
    QOpenGLBuffer* vertexBuffer;
    QOpenGLVertexArrayObject* vertexArray;
    bool visible = true;
    Color color;
    glm::mat4 transformationMatrix;

public:
//    AbstractRenderModel() = delete;
    explicit AbstractRenderModel(const glm::mat4& transformation);
    AbstractRenderModel(AbstractRenderModel&& other) noexcept;
    AbstractRenderModel& operator=(AbstractRenderModel&& other) noexcept;
    virtual ~AbstractRenderModel();

    virtual void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool lightMode) = 0;

    [[nodiscard]] bool isVisible() const;
    void setVisible(bool visible);
    [[nodiscard]] const Color &getColor() const;
    void setColor(const Color &newColor);
    [[nodiscard]] const glm::mat4 &getTransformation() const;
    virtual void setTransformationMatrix(const glm::mat4 &newTransformationMatrix);

    virtual void showContextMenu(const QPoint &position){
        QMenu contextMenu(QString("Context menu"));

        QAction* visibleAction = contextMenu.addAction(QString("Visible"));
        QObject::connect(visibleAction, &QAction::triggered, [=](bool enabled){
            this->visible = enabled;
        }); // TODO openGLWidget should update, how?
        visibleAction->setCheckable(true);
        visibleAction->setChecked(this->visible);
        contextMenu.addAction(visibleAction);

        QAction* colorAction = contextMenu.addAction(QString("Color"));
        QObject::connect(colorAction, &QAction::triggered, [=](){
            auto initialColor = this->getColor();
            auto resultColor = QColorDialog::getColor(QColor(255.f*initialColor.r, 255.f*initialColor.g, 255.f*initialColor.b, 255.f*initialColor.a), nullptr, QString(), QColorDialog::ShowAlphaChannel);
            if(resultColor.isValid()){
                this->setColor(Color(resultColor.red() / 255.f, resultColor.green() / 255.f, resultColor.blue() / 255.f, resultColor.alpha() / 255.f));

//                taskWidget->getOpenGLRenderWidget()->update();         // TODO openGLWidget should update, how?
            }
        });

        contextMenu.addAction(colorAction);



        contextMenu.exec(position);
    }
};

#endif //OPTIXMESHCORE_ABSTRACTRENDERMODEL_H
