#include "UIMainLayerLayoutToolsWidget.h"
#include "ui_UIMainLayerLayoutToolsWidget.h"

CUIMainLayerLayoutToolsWidget::CUIMainLayerLayoutToolsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CUIMainLayerLayoutToolsWidget)
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setVisible(false);
    ui->setupUi(this);
}

CUIMainLayerLayoutToolsWidget::~CUIMainLayerLayoutToolsWidget()
{
    delete ui;
}
