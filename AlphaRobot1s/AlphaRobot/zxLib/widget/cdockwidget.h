#ifndef CDOCKWIDGET_H
#define CDOCKWIDGET_H

#include "ubxwidget.h"

#include <QDockWidget>

class ZXLIB_DLL CDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CDockWidget(QWidget *parent = 0);
    ~CDockWidget();

protected:
    virtual void changeEvent(QEvent *event);
    virtual void retranslateUi(){};

};

#endif // CDOCKWIDGET_H
