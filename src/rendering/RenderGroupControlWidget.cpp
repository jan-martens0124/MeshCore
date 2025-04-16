//
// Created by tolle on 6/09/2023.
//

#include "RenderGroupControlWidget.h"
#include "forms/ui_rendergroupcontrolwidget.h"

RenderGroupControlWidget::RenderGroupControlWidget(const std::string &renderGroup, QWidget *parent, OpenGLWidget* openGLWidget): QWidget(parent), openGLWidget(openGLWidget), ui(new Ui::RenderGroupControlWidget), group(renderGroup) {
    ui->setupUi(this);
    ui->groupTitleLabel->setText(QString::fromStdString(group));
    ui->groupVisibleCheckBox->setChecked(true);

    connect(ui->groupVisibleCheckBox, &QCheckBox::stateChanged, this, [&, renderGroup]{
        if(ui->groupVisibleCheckBox->checkState() == Qt::CheckState::PartiallyChecked){
            return;
        }
        QMetaObject::invokeMethod(this->openGLWidget, "setGroupVisible", Qt::AutoConnection, Q_ARG(std::string, group), Q_ARG(bool, ui->groupVisibleCheckBox->isChecked()));
    });

    this->groupListener = std::make_shared<SimpleRenderGroupListener>();
    groupListener->setOnVisibleChanged([&](bool visible, bool uniform) {
        if(!uniform){
            ui->groupVisibleCheckBox->setCheckState(Qt::CheckState::PartiallyChecked);
        }
        else{
            ui->groupVisibleCheckBox->setChecked(visible);
            ui->groupVisibleCheckBox->setTristate(false);
            ui->groupVisibleCheckBox->update();
        }
    });
    this->openGLWidget->addGroupListener(group, groupListener);
}

RenderGroupControlWidget::~RenderGroupControlWidget() {
    this->openGLWidget->removeGroupListener(group, groupListener);
    delete ui;
}
