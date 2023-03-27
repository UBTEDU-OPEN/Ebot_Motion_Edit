#include "cdockwidget.h"




CDockWidget::CDockWidget(QWidget *parent):
     QDockWidget(parent)
{

}

CDockWidget::~CDockWidget()
{

}

void CDockWidget::changeEvent(QEvent *event)
{
    QDockWidget::changeEvent(event);

    switch(event->type())
    {

    case QEvent::LanguageChange:
    {
        //ui->retranslateUi(this);
        retranslateUi();
    }
    }
}
