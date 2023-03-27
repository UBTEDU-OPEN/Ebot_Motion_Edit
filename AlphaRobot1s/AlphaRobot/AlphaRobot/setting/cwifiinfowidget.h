/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CWifiInfoWidget
* 创建时间：2017/03/02
* 文件标识：
* 文件摘要：wifi info窗口
*
* 当前版本：1.0.0.0
* 作    者：asu
* 完成时间：2017/03/02
* 版本摘要：
*/

#ifndef CWIFIINFOWIDGET_H
#define CWIFIINFOWIDGET_H

#include <QWidget>

#include "zxObserverBase.h"
#include "RobotController.h"
#include "../zxLib/widget/ubxwidget.h"
#include "../zxLib/widget/cdialogcontainer.h"



typedef struct tagWIFIINFOS
{
    QString strAddress;
    QString strESSID;
    QString strProtocol;
    QString strMode;
    QString strEncryptionKey; //on,
    QString strIE;//IEEE 802.11i/WPA2 Version 1
    QString strGroupCipher; // CCMP;
    QString strPairwiseCiphers;//CCMP
    QString strAuthenticationSuites;//
    QString strQuality; //56/100
    QString strSignalLevel; //2/100
}WifiInfos;

namespace Ui {
class CWifiInfoWidget;
}

class CWifiInfoWidget : public UBXWidget , public zxObserverBase
{
    Q_OBJECT

public:
    explicit CWifiInfoWidget(QWidget *parent = 0);
    ~CWifiInfoWidget();

protected:
    virtual void customEvent(QEvent * event) override;
    RETRANSLATEUIFUNC    

    /**************************************************************************
    * 函数名: processGetRobotWifiListRsp
    * 功能: 处理机器人wifi列表
    *     * 参数:
    *    @[in ] wParam: 参数1，true/false
    *    @[in ] lParam: 参数2，info
    *
    * 时间: 20170302
    * 作者: asu
    */
    void processGetRobotWifiListRsp(WPARAM wParam, LPARAM lParam);

    /**************************************************************************
    * 函数名: processRobotConnectWifiRsp
    * 功能: 处理机器人connect wifi
    * 参数:
    *    @[in ] wParam: 参数1，true/false
    *    @[in ] lParam: 参数2，info
    *
    * 时间: 20170302
    * 作者: asu
    */
    void processRobotConnectWifiRsp(WPARAM wParam, LPARAM lParam);

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
    * 时间: 2017/03/02
    * 作者: asu
    */
    virtual void OnNotify(const zxObserverSubjectBase* pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam) override;

signals:
    void sigSendss();
    ADD_DIALOG_SIGNALS_TO_DIGALOGCONTAINER

private:
    void updateWifiInfoShow(const QList<WifiInfos>& listWifiInfos);
    void addWifiInfoToTable(const WifiInfos& wifiInfo);



private slots:
    void on_pushButtonScan_clicked();
    void on_tableWidgetWifiInfos_cellDoubleClicked(int row, int column);


private:
    Ui::CWifiInfoWidget *ui;

private:
    QList<WifiInfos>   m_lstWifiInfos;   // wifiinfo list
};

#endif // CWIFIINFOWIDGET_H
