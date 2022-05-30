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

//template <unsigned int Degree>
//void RenderWidget::renderAABBTree(const std::string &group, const std::shared_ptr<AABBTree<Degree>> &aabbTree, const Color& color = Color(1.0f)) {
//    QMetaObject::invokeMethod(this->getOpenGLWidget(), "renderAABBTreeSlot",
//                              Qt::AutoConnection,
//                              Q_ARG(std::string, group),
//                              Q_ARG(std::shared_ptr<AABBTree>, aabbTree),
//                              Q_ARG(Color, color),
//                              Q_ARG(RenderWidget*, this));
//}

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
        groupLayouts.clear();
        QLayoutItem* item;
        while((item = this->ui->objectsVerticalLayout->takeAt(0))!=nullptr) {
            if(item->widget()){
                delete item->widget();
                delete item;
            }
            else if(item->layout()){
                delete item->layout();
            }
        }

        this->ui->objectsVerticalLayout->update();

        QMetaObject::invokeMethod(this->getOpenGLWidget(), "clear", Qt::AutoConnection);
    });
}

void RenderWidget::clearGroup(const std::string &group) {
    QMetaObject::invokeMethod(this, [&, group] {
        QLayoutItem* item;

        // Clear the items from the group layout
        auto groupLayout = this->getOrAddGroupLayout(group);
        while((item = groupLayout->takeAt(0))!=nullptr) {
            delete item->widget();
            delete item;
        }
        groupLayouts.erase(group);

        int i = 0;
        // Delete the groups title and horizontal line
        while((item = this->ui->objectsVerticalLayout->itemAt(i))!=nullptr) {
            if (item->widget()!=nullptr && item->widget()->objectName() == QString::fromStdString(group)) {
                item = this->ui->objectsVerticalLayout->takeAt(i);
                delete item->widget();
                delete item;
            }
            else if(item->layout()!=nullptr && item->layout()->objectName() == QString::fromStdString(group)) {
                item = this->ui->objectsVerticalLayout->takeAt(i);
                delete item->layout();
            }
            else{
                i++;
            }
        }

        this->ui->objectsVerticalLayout->update();

        QMetaObject::invokeMethod(this->getOpenGLWidget(), "clearGroup", Qt::AutoConnection, Q_ARG(std::string, group));
    });
}

QVBoxLayout *RenderWidget::getOrAddGroupLayout(const std::string &group) {

    // Find the group
    auto iterator = groupLayouts.find(group);

    // Add new group if not found
    if(iterator == groupLayouts.end()){
        const auto layout = new QVBoxLayout();
        layout->setObjectName(QString::fromStdString(group));

        auto titleLabel = new QLabel(this);
        titleLabel->setText(QString::fromStdString(group));
        titleLabel->setObjectName(QString::fromStdString(group));
        titleLabel->setMaximumSize(QSize(300, 300));
        titleLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        this->ui->objectsVerticalLayout->addWidget(titleLabel);

        iterator = groupLayouts.insert({group, layout}).first;
        this->ui->objectsVerticalLayout->addLayout(layout);

        auto line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setObjectName(QString::fromStdString(group));
        line->setFrameShadow(QFrame::Sunken);
        this->ui->objectsVerticalLayout->addWidget(line);
    }
    return iterator->second;
}
