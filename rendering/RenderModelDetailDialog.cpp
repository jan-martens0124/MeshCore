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

RenderModelDetailDialog::RenderModelDetailDialog(AbstractRenderModel* renderModel, QWidget* parent): QDialog(parent),
    ui(new Ui::RenderModelDetailDialog),
    renderModel(renderModel),
    listener(std::make_shared<SimpleRenderModelListener>()){

    ui->setupUi(this);
    setWindowFlag(Qt::WindowStaysOnTopHint);

    this->ui->nameLineEdit->setText(QString::fromStdString(renderModel->getName()));
    listener->setOnNameChanged([this](const std::string& oldName, const std::string& newName){
        QMetaObject::invokeMethod(this, [&]{
            this->ui->nameLineEdit->setText(QString::fromStdString(newName));
        });
    });
    connect(this->ui->nameLineEdit, &QLineEdit::textEdited, [=](const QString& text){
        renderModel->setName(text.toStdString());
    });

//    this->ui->scaleLineEdit->setReadOnly(true);
//    this->ui->xLineEdit->setReadOnly(true);
//    this->ui->yLineEdit->setReadOnly(true);
//    this->ui->zLineEdit->setReadOnly(true);
//    this->ui->yawLineEdit->setReadOnly(true);
//    this->ui->pitchLineEdit->setReadOnly(true);
//    this->ui->rollLineEdit->setReadOnly(true);

    // Add a listener to change the transformation values
    listener->setOnTransformationChanged([this](const Transformation& oldTransformation, const Transformation& newTransformation){
        QMetaObject::invokeMethod(this, [&]{
            this->ui->scaleLineEdit->setText(QString::number(newTransformation.getScale()));
            this->ui->xLineEdit->setText(QString::number(newTransformation.getPosition().x));
            this->ui->yLineEdit->setText(QString::number(newTransformation.getPosition().y));
            this->ui->zLineEdit->setText(QString::number(newTransformation.getPosition().z));
            this->ui->yawLineEdit->setText(QString::number(newTransformation.getRotation().getYaw()));
            this->ui->pitchLineEdit->setText(QString::number(newTransformation.getRotation().getPitch()));
            this->ui->rollLineEdit->setText(QString::number(newTransformation.getRotation().getRoll()));
        });
    });

    // Editing
    this->ui->scaleLineEdit->setValidator(new QDoubleValidator(this));
    this->ui->xLineEdit->setValidator(new QDoubleValidator(this));
    this->ui->yLineEdit->setValidator(new QDoubleValidator(this));
    this->ui->zLineEdit->setValidator(new QDoubleValidator(this));
    this->ui->yawLineEdit->setValidator(new QDoubleValidator(this));
    this->ui->pitchLineEdit->setValidator(new QDoubleValidator(this));
    this->ui->rollLineEdit->setValidator(new QDoubleValidator(this));
    connect(this->ui->scaleLineEdit, &QLineEdit::textEdited, [=](const QString& text){
        if(!text.endsWith(".")){
            auto transformation = renderModel->getTransformation();
            transformation.setScale(text.toFloat());
            renderModel->setTransformation(transformation);
        }
    });

    this->ui->yawLineEdit->setEnabled(false);
    this->ui->pitchLineEdit->setEnabled(false);
    this->ui->rollLineEdit->setEnabled(false);

    connect(this->ui->yawLineEdit, &QLineEdit::textEdited, [=](const QString& text){
//        auto transformation = renderModel->getTransformation();
//        transformation.setYaw(text.toFloat());
//        renderModel->setTransformation(transformation);
    });
    connect(this->ui->pitchLineEdit, &QLineEdit::textEdited, [=](const QString& text){
//        auto transformation = renderModel->getTransformation();
//        transformation.setPitch(text.toFloat());
//        renderModel->setTransformation(transformation);
    });
    connect(this->ui->rollLineEdit, &QLineEdit::textEdited, [=](const QString& text){
//        auto transformation = renderModel->getTransformation();
//        transformation.setRoll(text.toFloat());
//        renderModel->setTransformation(transformation);
    });
    connect(this->ui->xLineEdit, &QLineEdit::textEdited, [=](const QString& text){
        auto transformation = renderModel->getTransformation();
        transformation.setPositionX(text.toFloat());
        renderModel->setTransformation(transformation);
    });
    connect(this->ui->yLineEdit, &QLineEdit::textEdited, [=](const QString& text){
        auto transformation = renderModel->getTransformation();
        transformation.setPositionY(text.toFloat());
        renderModel->setTransformation(transformation);
    });
    connect(this->ui->zLineEdit, &QLineEdit::textEdited, [=](const QString& text){
        auto transformation = renderModel->getTransformation();
        transformation.setPositionZ(text.toFloat());
        renderModel->setTransformation(transformation);
    });

    // We have to set the initial transformation as well
    this->ui->scaleLineEdit->setText(QString::number(renderModel->getTransformation().getScale()));
    this->ui->xLineEdit->setText(QString::number(renderModel->getTransformation().getPosition().x));
    this->ui->yLineEdit->setText(QString::number(renderModel->getTransformation().getPosition().y));
    this->ui->zLineEdit->setText(QString::number(renderModel->getTransformation().getPosition().z));
    this->ui->yawLineEdit->setText(QString::number(renderModel->getTransformation().getRotation().getYaw()));
    this->ui->pitchLineEdit->setText(QString::number(renderModel->getTransformation().getRotation().getPitch()));
    this->ui->rollLineEdit->setText(QString::number(renderModel->getTransformation().getRotation().getRoll()));


    this->renderModel->addListener(listener);

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
    // TODO :) (do we even want to be able to make changes through the ui?)
}

//void RenderModelDetailDialog::addLayout(QLayout *layout) {
//    this->ui->verticalLayout->addLayout(layout);
//}
//
//void RenderModelDetailDialog::addWidget(QWidget *widget) {
//    this->ui->verticalLayout->addWidget(widget);
//}

void RenderModelDetailDialog::addTab(QWidget *widget, const QString &label) {
    this->ui->verticalTabWidget->addTab(widget, label);
}

