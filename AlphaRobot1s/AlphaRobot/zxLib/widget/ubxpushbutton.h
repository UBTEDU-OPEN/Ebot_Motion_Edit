/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ubxpushbutton.h
* 创建时间：2017.01.24
* 文件标识：
* 文件摘要：UBX定制按钮
*
* 当前版本：1.0.0.0
* 作    者：hwx
*/

#ifndef UBXPUSHBUTTON_H
#define UBXPUSHBUTTON_H

#include <QPushButton>
#include <QTimer>


class ZXLIB_DLL UBXPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit UBXPushButton(QWidget *parent = 0);
    virtual ~UBXPushButton();

public:
    bool isPressInterval() const;
    void enablePressInterval(bool bSetPressInterval);

    int pressInterval() const;
    void setPressInterval(int nMsecondPressInterval);

public:
    void setEnabled(bool);
    void setDisabled(bool);

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

signals:
    void sigOnMouseDoubleClicked();

public slots:

private slots:
    void onReleased();
    void onTimeoutInterval();

private:
    bool m_bSetPressInterval;
    int m_nMsecondPressInterval;

    QTimer *m_pTimerPressInterval;
};

#endif // UBXPUSHBUTTON_H
