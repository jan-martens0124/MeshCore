//
// Created by Jonas on 30/11/2020.
//

#ifndef MESHCORE_RENDERWINDOW_H
#define MESHCORE_RENDERWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include "../meshcore/rendering/OpenGLRenderWidget.h"


QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMenuBar;
QT_END_NAMESPACE

class RenderWindow: public QMainWindow {
private:
    QMenuBar *menuBar;
    OpenGLRenderWidget* openGlRenderWidget;
public:
    RenderWindow();
    OpenGLRenderWidget *getOpenGlRenderWidget() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;

    void loadMesh();
    void displayApplicationInfo();
};


#endif //MESHCORE_RENDERWINDOW_H
