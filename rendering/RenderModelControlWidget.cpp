//
// Created by Jonas on 19/05/2022.
//

#include "RenderModelControlWidget.h"
#include "forms/ui_RenderModelControlWidget.h"

RenderModelControlWidget::RenderModelControlWidget(const std::shared_ptr<AbstractRenderModel>& renderModel):
        ui(new Ui::RenderModelControlWidget),
        renderModel(renderModel),
        listener(std::make_shared<RenderModelListener>()) {

    ui->setupUi(this);

    ui->nameLabel->setText("TestName");
    ui->visibleCheckBox->setChecked(this->renderModel->isVisible());
    const auto &color = this->renderModel->getColor();
    ui->colorPushButton->setStyleSheet("background-color: " + QColor(color.r*255, color.g*255, color.b*255, color.a*255).name());

    listener->setOnColorChanged([&](const Color &newColor) {
        this->ui->colorPushButton->setStyleSheet("background-color: " + QColor(newColor.r*255, newColor.g*255, newColor.b*255, newColor.a*255).name());
    });

    listener->setOnVisibleChanged([&](bool visible) {
        this->ui->visibleCheckBox->setChecked(visible);
    });

    connect(ui->visibleCheckBox, &QCheckBox::clicked, [&](bool enabled) {
        this->renderModel->setVisible(enabled);
    });

    connect(ui->colorPushButton, &QPushButton::clicked, [=](){
        auto initialColor = this->renderModel->getColor();
        auto resultColor = QColorDialog::getColor(QColor(255.f*initialColor.r, 255.f*initialColor.g, 255.f*initialColor.b, 255.f*initialColor.a), nullptr, QString(), QColorDialog::ShowAlphaChannel);
        if(resultColor.isValid()){
            this->renderModel->setColor(Color(resultColor.red() / 255.f, resultColor.green() / 255.f, resultColor.blue() / 255.f, resultColor.alpha() / 255.f));
        }
    });
}

RenderModelControlWidget::~RenderModelControlWidget() {
    renderModel->removeListener(listener);
    delete ui;
}
