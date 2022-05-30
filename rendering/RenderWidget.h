//
// Created by Jonas on 29/03/2022.
//

#ifndef OPTIXMESHCORE_RENDERWIDGET_H
#define OPTIXMESHCORE_RENDERWIDGET_H

#include <unordered_map>
#include <iostream>
#include "AbstractRenderModel.h"
#include "../core/WorldSpaceMesh.h"
#include "../../tasks/AbstractTaskObserver.h"
#include "OpenGLWidget.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class RenderWidget; }
QT_END_NAMESPACE

class RenderWidget: public QWidget, public AbstractTaskObserver {
Q_OBJECT
public:
    explicit RenderWidget(QWidget *parent = nullptr);
    ~RenderWidget() override;
    [[nodiscard]] OpenGLWidget* getOpenGLWidget() const;

private:
    Ui::RenderWidget *ui;
    std::unordered_map<std::string, QVBoxLayout *> groupLayouts;
    QVBoxLayout * getOrAddGroupLayout(const std::string &group);

public:
    void clear();
    void clearGroup(const std::string &group);
    void renderWorldSpaceMesh(const std::string &group, const std::shared_ptr<WorldSpaceMesh> &worldSpaceMesh, const Color &color);

    void addControlWidget(const std::string &group, const std::shared_ptr<AbstractRenderModel> &renderModel);

};


#endif //OPTIXMESHCORE_RENDERWIDGET_H
