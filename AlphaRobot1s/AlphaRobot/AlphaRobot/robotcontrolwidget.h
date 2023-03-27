/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：robotcontrolwidget.h
* 创建时间：2016/08/05
* 文件标识：
* 文件摘要：robot label widget ui
*
* 当前版本：1.0.0.0
* 作    者：asu
* 版本摘要：
*/

#ifndef ROBOTLABELUI_H
#define ROBOTLABELUI_H

#include <QWidget>
#include <QPushButton>
#include <QToolButton>


#include "../common/singleton.h"
#include "../zxLib/widget/ubxwidget.h"
#include "../zxLib/zxObserverBase.h"
#include "../zxLib/widget/ubxpushbutton.h"
#include "cmyalpha.h"


namespace Ui {
class CRobotControlWidget;
}

class CRobotControlWidget : public UBXWidget, public zxObserverBase
{
    Q_OBJECT

// 由于需要传入parent，如果定义成单例静态成员，会导致重复析构
//public:
//    SINGLETON_INSTANCE(CRobotControlWidget)

public:
    explicit CRobotControlWidget(QWidget *parent = 0);
    ~CRobotControlWidget();

public:
    static bool getBatteryStatus(const char* pBatteryStatus, int& nVoltageMil, int& nBatteryStatus, int& nQuantity);
    RETRANSLATEUIFUNC

public:
    /**************************************************************************
    * 函数名: OnNotify
    * 功能: 处理通知消息
    *     * 参数:
    *    @[in ] pSubject: 主题
    *    @[in ] nMsgID: 消息ID
    *    @[in ] dwContext: 上下文
    *    @[in ] wParam: 参数1
    *    @[in ] lParam: 参数2
    *
    * 时间: 2016/07/23 16:39
    * 作者: asu
    */
    virtual void OnNotify(const zxObserverSubjectBase* pSubject, int nMsgID,
                          DWORD dwContext, WPARAM wParam, LPARAM lParam) override;

public slots:
    /**************************************************************************
    * 函数名: slotOnPortOpenChanged
    * 功能: 断开连接机器人状态变化槽函数
    *     * 参数:
    *    @[in ] bOpen: 是否连接上机器人标志
    *
    * 时间: 2017/02/08
    * 作者: hwx
    */
    void slotOnPortOpenChanged(BOOL bOpen);

signals:
    void sigConnectRobot();
    void sigDisconnectRobot();
    void sigResetRobot();
    void sigRobotPowerOff(bool bUpdateUI);

    void sigOnPortOpenChanged(BOOL bOpen);

private slots:
    void onResetRobotBodyButtonClicked();
    void onDisconnectRobotButtonClicked();
    void onConnectRobotButtonClicked();
    void onPowerOffButtonClicked();
    void onBatteryButtonClicked();
    void onRobotHeadButtonClicked();
    void onRobotWifiButtonClicked();

    void onGetBatteryStatus();

    void onRobotProductTypeChange(int eUbtProductTypeOld, int eUbtProductTypeNew);

public:
    virtual bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;

protected:
    virtual void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    virtual void customEvent(QEvent * event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void setRobotHeadTip(const QString strTip);
    void setBattery(int eBatteryStatus, int nBatteryValue);

    /**************************************************************************
    * 函数名: setButtonState
    * 功能: 设置按钮状态
    *     * 参数:
    *    @[in ] bConnected: 是否连接上机器人标志
    *
    * 时间: 2017/02/08
    * 作者: hwx
    */
    void setButtonState(bool bConnected);
    void setButtonStateWifi(bool bConnected);
    void setButtonStateHead(bool bConnected);

private:
    // 记录鼠标按下的全局坐标位置，当鼠标释放时比较距离大小
    // 如果超过指定距离，则认为进行的操作是拖动全局控制按钮
    // 此时不要响应按钮事件（QPushButton的MouseMove无效!!!!）
    QPoint  m_mousePressedPos;

    UBXPushButton *m_pBtnRecoverGesture;
    UBXPushButton *m_pBtnDisconnectRobot;
    UBXPushButton *m_pBtnConnectRobot;
    UBXPushButton *m_pBtnPowerOff;
    UBXPushButton *m_pBtnRobotHead;
    UBXPushButton *m_pBtnRobotWifi;

    QToolButton *m_pBtnBattery;

    CMyAlpha *m_pMyAlpha;

    QWidget *m_pParentWidget;

private:
    Ui::CRobotControlWidget *ui;
};

#endif // ROBOTLABELUI_H
