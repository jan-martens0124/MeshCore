//
// Created by Jonas on 31/05/2022.
//

#ifndef MESHCORE_RENDERMODELDETAILDIALOG_H
#define MESHCORE_RENDERMODELDETAILDIALOG_H


#include <QDialog>
#include <memory>

namespace Ui {
    class RenderModelDetailDialog;
}
class AbstractRenderModel;
class SimpleRenderModelListener;

class RenderModelDetailDialog: public QDialog {
Q_OBJECT
private:

public:
    explicit RenderModelDetailDialog(AbstractRenderModel* renderModel, QWidget* parent);
    ~RenderModelDetailDialog() override;

//    void addLayout(QLayout* layout);
//    void addWidget(QWidget* widget);
    void addTab(QWidget *widget, const QString &label);

private:
    Ui::RenderModelDetailDialog *ui;
    AbstractRenderModel* renderModel;
    std::shared_ptr<SimpleRenderModelListener> listener;

private:
    void applyChanges();

};


#endif //MESHCORE_RENDERMODELDETAILDIALOG_H
