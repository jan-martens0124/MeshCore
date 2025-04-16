//
// Created by tolle on 6/09/2023.
//

#ifndef EXTENDEDMESHCORE_RENDERGROUPCONTROLWIDGET_H
#define EXTENDEDMESHCORE_RENDERGROUPCONTROLWIDGET_H


#include <QWidget>
#include "AbstractRenderModel.h"
#include "meshcore/rendering/OpenGLWidget.h"

namespace Ui {
    class RenderGroupControlWidget;
}

class RenderGroupControlWidget: public QWidget {
Q_OBJECT
public:
    explicit RenderGroupControlWidget(const std::string& renderGroup, QWidget* parent, OpenGLWidget* openGLWidget);
    ~RenderGroupControlWidget() override;

private:
    OpenGLWidget* openGLWidget;
    std::string group;
    Ui::RenderGroupControlWidget *ui;
    std::shared_ptr<SimpleRenderGroupListener> groupListener;
};


#endif //EXTENDEDMESHCORE_RENDERGROUPCONTROLWIDGET_H
