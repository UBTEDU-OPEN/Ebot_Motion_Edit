/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ubxrobot3dcontrolwidget.h
* 创建时间：2016/01/04
* 文件标识：
* 文件摘要：3D Robot控制界面类
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/01/04
* 版本摘要：
*/


#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include "ubxrobot3dview_global.h"
#include <QWidget>
#include <QButtonGroup>
#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>
#include <QStyleOption>
#include <QPainter>
#include <QLabel>
#include <QPushButton>
#include <QMap>
#include "UBXPublic.h"
#include "../zxLib/widget/ubxwidget.h"

namespace Ui {
class UBXRobot3DControlWidget;
}

//读角度类型
enum emReadAngleType
{
    eReadAngleAll,    //全部回读
    eReadAngleHand,  //回读手部
    eReadAngleLeg,    //回读腿部
    eReadAngleSingle  //回读单个舵机
};

class UBXROBOT3DVIEWSHARED_EXPORT UBXRobot3DControlWidget : public UBXWidget, public zxObserverBase
{
    Q_OBJECT

public:
    explicit UBXRobot3DControlWidget(QWidget *parent = 0);
    ~UBXRobot3DControlWidget();

    //更新控制界面显示数据
    void UpdateNodeDate(const char* szNodeName, float angle, float offset, TypeAngleChanged nChangeType);

    /**************************************************************************
    * 函数名: OnNotify
    * 功能: 通知消息
    * 参数:
    *    @[in ] pSubject: 被观察者
    *    @[in ] nMsgID: 消息ID
    *    @[in ] dwContext: 消息上下文
    *    @[in ] wParam: 参数1
    *    @[in ] lParam: 参数2
    * 返回值: void
    * 时间: 2015/08/22 19:09
    * 作者: zdj
    */
    virtual void OnNotify(const zxObserverSubjectBase* pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam);

    /**************************************************************************
    * 函数名: EnableCtrls
    * 功能: 设置控件启用状态
    * 参数:
    *    @[in ] bEnable: 启用标识
    * 返回值: void
    * 时间: 2016/01/13 14:40
    * 作者: ZDJ
    */
    void EnableCtrls(bool bEnable);

    /**************************************************************************
    * 函数名: ResetRobot
    * 功能: 复位机器人
    * 时间: 2016/01/25 17:49
    * 作者: ZDJ
    */
    void ResetRobot();

    /**************************************************************************
    * 函数名: ReadDefaultAngle
    * 功能: 读取舵机默认角度
    * 参数:
    *    @[out] motorlist: 舵机数据列表
    * 返回值: void
    * 时间: 2016/01/26 10:15
    * 作者: ZDJ
    */
    void ReadDefaultAngle(vector<MOTOR_DATA*>& motorlist);

#if 0
    /**************************************************************************
    * 函数名: ReadModelXml
    * 功能: 读取3D模型的xml配置文件
    * 参数:
    *    @[in ] element: dom元素
    *    @[out] motorlist: 舵机数据列表
    * 返回值: void
    * 时间: 2016/01/26 10:15
    * 作者: ZDJ
    */
    void ReadModelXml(const QDomElement& element, vector<MOTOR_DATA*>& motorlist);
#endif

    RETRANSLATEUIFUNC

public slots:
    /**************************************************************************
    * 函数名: onResetGesture
    * 功能: reset the robot gesture.
    * 参数:
    * 返回值: void
    * 时间: 2016/08/08
    * 作者: asu
    */
    void onResetGesture();

    /**************************************************************************
    * 函数名: slotOnRobotPowerOff
    * 功能: 机器人掉电
    * 参数:
    * 返回值: void
    * 时间: 2017/02/08
    * 作者: hwx
    */
    void slotOnRobotPowerOff(bool bUpdateUI);

private:
    /**************************************************************************
    * 函数名: setChangedAngleProcess
    * 功能: 设置角度变化处理状态
    * 参数:
    *    @[in ] bState: 处理状态：true：处理中；false：空闲状态
    * 返回值: void
    * 时间: 2016/07/15 10:15
    * 作者:hwx
    */
    void setChangedAngleProcess(bool bState);

    /**************************************************************************
    * 函数名: startChangedAngleProcessTimer
    * 功能: 启动定时器处理机器人角度变化
    * 参数:
    *    @[in ] 参数nTime: 定时时间
    * @[in ] 参数bSendSignal：是否发送信号
    * 返回值: void
    * 时间: 2016/07/15 10:15
    * 作者:hwx
    */
    void startChangedAngleProcessTimer(int nTime, bool bSendSignal);

    /**************************************************************************
    * 函数名: setReadBackButtonStatus
    * 功能: 设置回读按钮状态，防止用户联系多次点击
    * 参数:
    *    @[in ]
    * 返回值: void
    * 时间: 2016/07/20 10:15
    * 作者:hwx
    */
    void setReadBackButtonStatus(bool bEnable);

    /**************************************************************************
    * 函数名: readNextMotor
    * 功能: 读取下一个舵机角度
    * 参数:
    *    @[in ]
    * 返回值: void
    * 时间: 2016/08/04 10:15
    * 作者:hwx
    */
    void readNextMotor();

signals:
    //操作控件时发射以下信号，用于通知外部数据有改变
    void clickNode(int nID);
    void angleChanged(int nID, int value, int nTime, TypeAngleChanged nType);
    void reset();

private slots:
    //各个控制控件槽函数
    void onButtonToggled(int id, bool checked);
    void onSliderAnglevalueChanged(int value);
    void on_pushButtonMinus_clicked();
    void on_pushButtonPlus_clicked();
    void on_pushButtonReset_clicked();
    void on_pushButtonLegReadback_clicked();
    void on_pushButtonHandReadBack_clicked();
    void on_pushButtonReadAll_clicked();

    void slotOnReadAll(bool bUpdateUI);

    void onPowerOffCurrentNode();

    //区分判断什么时候设置机器人角度，因为直接拖动3D模型也会改变，不能直接通过QSider的valueChanged来做，否则重复设置了
    //void onSliderMoved(int value);
    //void onSliderPressed();

    /**************************************************************************
    * 函数名: on_pushButtonPowerOff_clicked
    * 功能: 掉电消息响应
    * 时间: 2016/01/28 17:39
    * 作者: ZDJ
    */
    void on_pushButtonPowerOff_clicked();

    //void on_pushButtonOpenPort_clicked();

public slots:
    /**************************************************************************
    * 函数名: OnPortOpenChanged
    * 功能: 端口改变消息响应
    * 时间: 2016/01/28 17:39
    * 作者: ZDJ
    */
    void OnPortOpenChanged(BOOL bOpen);

    /**************************************************************************
    * 函数名: slotOnChangedRobotAngleTimeout
    * 功能: 改变机器人定时时间到达的槽函数
    * 时间: 2016/07/12
    * 作者: hwx
    */
    void slotOnChangedRobotAngleTimeout();

signals:
    /**************************************************************************
    * 函数名: PortOpenChanged
    * 功能: 端口改变事件
    * 时间: 2016/01/28 17:39
    * 作者: ZDJ
    */
    void PortOpenChanged(BOOL bOpen);

protected:
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *)
    {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
    virtual void resizeEvent(QResizeEvent * event);
    virtual void timerEvent(QTimerEvent* event);
    virtual void customEvent(QEvent* event);

private:
    bool changeRobotAngle(bool bEmitSignal = true, int nTime = 200);
private:
    static QMap<QString, int> groupNameID;
    QMap<int, QPushButton*> m_mapNodeRadios;

private:
    Ui::UBXRobot3DControlWidget *ui;
    QButtonGroup m_btnGroup;//管理radiobutton
    QLabel* m_pLabelSliderNum;

    vector<MOTOR_DATA*> m_vecDefaultMotorDataList; //默认舵机数据
    emReadAngleType  m_eReadAngleType;
    TypeAngleChanged m_eTypeAngleChanged;
    int   m_nConnectTimerId; //连接机器人时钟id
    int   m_nDisConnectTimerId; //断开连接机器人时钟id

    bool m_bReadAngleUpdateUI;  // 读取角度时是否需要更新界面，机器人掉电时不需要更新界面3D模型角度

    bool m_bIsReading;//记录是否是回读导致的slider数据改变，这样在valuechanged中就不能再次去set真实机器人的angle，
                      //否则会导致机器人舵机上电（拖动单个舵机改变角度就会上电）

    int m_nServoCount; //舵机数量

    // add by huangwuxian 2016.7.12 禅道2627
    // 如果在界面滑动滑动条，会不断触发信号发送数据；
    // 当发送数据的时间间隔很短（不超过35ms）时，固件
    // 接收无响应，导致软件卡死崩溃。增加定时器，当连续
    // 发送数据时，如果在40ms内连续发送多次数据，忽略除
    // 第一次外的其他数据
    QTimer *m_pChangedAngleProcessTimer;  // 处理机器人角度变化的定时器

};

#endif // CONTROLWIDGET_H
