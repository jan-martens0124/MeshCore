//
// Created by Jonas on 31/05/2022.
//

#include <QtGui>
#include "RenderModelDetailDialog.h"
#include "forms/ui_rendermodeldetaildialog.h"

#include "AbstractRenderModel.h"

RenderModelDetailDialog::~RenderModelDetailDialog() {
    delete ui;
}

RenderModelDetailDialog::RenderModelDetailDialog(AbstractRenderModel* renderModel):
    ui(new Ui::RenderModelDetailDialog),
    renderModel(renderModel),
    listener(std::make_shared<RenderModelListener>()){

    ui->setupUi(this);


    this->ui->nameLineEdit->setText(QString::fromStdString(renderModel->getName()));
    listener->setOnNameChanged([this](const std::string& name){
        QMetaObject::invokeMethod(this, [&]{
            this->ui->nameLineEdit->setText(QString::fromStdString(name));
        });
    });
    connect(this->ui->nameLineEdit, &QLineEdit::textChanged, [=](const QString& text){
        renderModel->setName(text.toStdString());
    });

//    this->ui->scaleLineEdit->setText(QString::number(renderModel->getTransformation().getScale()));

    this->ui->scaleLineEdit->setValidator(new QDoubleValidator(this));
    this->ui->xLineEdit->setValidator(new QDoubleValidator(this));
    this->ui->yLineEdit->setValidator(new QDoubleValidator(this));
    this->ui->zLineEdit->setValidator(new QDoubleValidator(this));


    renderModel->addListener(listener);

    this->ui->applyButton->setEnabled(false);

    connect(this->ui->confirmButton, &QPushButton::clicked, [=](){
        this->applyChanges();
        this->close();
    });

    connect(this->ui->cancelButton, &QPushButton::clicked, [=](){
        this->close();
    });

    connect(this->ui->applyButton, &QPushButton::clicked, [=](){
        this->applyChanges();
    });
}

void RenderModelDetailDialog::applyChanges() {
    // TODO :)
}

void RenderModelDetailDialog::addLayout(QLayout *layout) {
    this->ui->verticalLayout->addLayout(layout);
}

void RenderModelDetailDialog::addWidget(QWidget *widget) {
    this->ui->verticalLayout->addWidget(widget);
}

