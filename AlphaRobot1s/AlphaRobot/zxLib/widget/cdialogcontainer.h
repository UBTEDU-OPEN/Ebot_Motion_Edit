/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CDialogContainer
* 创建时间：20170715
* 文件标识：
* 文件摘要：the CDialog Container
*
* 当前版本：1.0.0.0
* 作    者：asu
*/

#ifndef CDIALOGCONTAINER_H
#define CDIALOGCONTAINER_H

#include <QWidget>

#include "mytitlebar.h"
#include "ubxdialog.h"




///////////////////which want to use the dialogContainer, must have to using DIALOG_SIGNALS_TO_DIGALOGCONTAINER
#define ADD_DIALOG_SIGNALS_TO_DIGALOGCONTAINER   void sigClicked(int);\
    void sigClose();


namespace Ui {
class CDialogContainer;
}

class ZXLIB_DLL CDialogContainer : public CUBXDialog
{
    Q_OBJECT

public:
    explicit CDialogContainer(QWidget *parent = 0);
    ~CDialogContainer();

public:
    void setContentWdg(QWidget *pWdg);
    int exec(QWidget* pWdg);


public slots:
    void onSigClicked(int nClick);
    virtual int exec();


private:
    Ui::CDialogContainer *ui;

    MyTitleBar m_myTitleBar;
    QWidget *m_pContentWdg;

    int m_clickBtnStatus;
};

#endif // CDIALOGCONTAINER_H
