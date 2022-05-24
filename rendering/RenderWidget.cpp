//
// Created by Jonas on 29/03/2022.
//

#include "RenderWidget.h"
#include <QtWidgets>
#include "forms/ui_renderwidget.h"
#include "RenderModelControlWidget.h"

RenderWidget::RenderWidget(QWidget *parent):
    QWidget(parent), ui(new Ui::RenderWidget)
    {
        ui->setupUi(this);
        ui->progressBar->setMinimumWidth(350);
    }

RenderWidget::~RenderWidget() {
    delete ui;
}

OpenGLWidget *RenderWidget::getOpenGLWidget() const {
    return ui->openGLWidget;
}

void RenderWidget::renderWorldSpaceMesh(const std::string &group, const std::shared_ptr<WorldSpaceMesh> &worldSpaceMesh,  const Color& color = Color(1.0f)) {
    QMetaObject::invokeMethod(this->getOpenGLWidget(), "renderWorldSpaceMeshSlot",
                              Qt::AutoConnection,
                              Q_ARG(std::string, group),
                              Q_ARG(std::shared_ptr<WorldSpaceMesh>, worldSpaceMesh),
                              Q_ARG(Color, color),
                              Q_ARG(RenderWidget*, this));
}

void RenderWidget::addControlWidget(const std::string &group, const std::shared_ptr<AbstractRenderModel> &renderModel) {
    this->getOrAddGroupLayout(group)->addWidget(new RenderModelControlWidget(renderModel));
}

void RenderWidget::clear() {
    // clear all layouts, on the UI thread
    QMetaObject::invokeMethod(this, [&]{
        for (const auto &[group, layout]: groupLayouts){
            QLayoutItem* item;
            while((item = layout->takeAt(0))!=nullptr) {
                delete item->widget();
                delete item;
            }
        }
        QLayoutItem* item;
        while((item = this->ui->objectsVerticalLayout->takeAt(0))!=nullptr) {
            delete item->widget();
            delete item;
        }
        QMetaObject::invokeMethod(this->getOpenGLWidget(), "clear", Qt::AutoConnection);
    });
}

void RenderWidget::clearGroup(const std::string &group) {
    QMetaObject::invokeMethod(this, [&, group] {
        QLayoutItem* item;
        while((item = getOrAddGroupLayout(group)->takeAt(0))!=nullptr) {
            delete item->widget();
            delete item;
        }
        int i = 0;
        while((item = this->ui->objectsVerticalLayout->takeAt(i))!=nullptr) {
            if(item->widget()!=nullptr && item->widget()->objectName().toStdString() == group) {
                delete item->widget();
                delete item;
            }
            else{
                i++;
            }
        }
        QMetaObject::invokeMethod(this->getOpenGLWidget(), "clearGroup", Qt::AutoConnection, Q_ARG(std::string, group));
    });
}

QVBoxLayout *RenderWidget::getOrAddGroupLayout(const std::string &group) {
    // Find the group
    auto iterator = groupLayouts.find(group);

    // Add new group if not found
    if(iterator == groupLayouts.end()){
        const auto layout = new QVBoxLayout();

        auto line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setObjectName(QString::fromStdString(group));
        line->setFrameShadow(QFrame::Sunken);
        this->ui->objectsVerticalLayout->addWidget(line);

        auto titleLabel = new QLabel(this);
        titleLabel->setText(QString::fromStdString(group));
        titleLabel->setObjectName(QString::fromStdString(group));
        titleLabel->setMaximumSize(QSize(300, 300));
        titleLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        this->ui->objectsVerticalLayout->addWidget(titleLabel);

        iterator = groupLayouts.insert({group, layout}).first;
        this->ui->objectsVerticalLayout->addLayout(layout);
    }
    return iterator->second;
}
