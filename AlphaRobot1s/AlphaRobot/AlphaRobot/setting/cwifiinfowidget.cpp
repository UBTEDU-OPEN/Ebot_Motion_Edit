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

#include "cwifiinfowidget.h"
#include "ui_cwifiinfowidget.h"

#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include <QDebug>
#include <QInputDialog>

#include "../zxLib/widget/messagebox.h"
#include "../zxLib/widget/ubxwidget.h"
#include "../UBXRobotController/Robot/cbluetoothcomandparser.h"
#include "alpharobotconst.h"


const string STR_WIFI_INFO_TAG_ADDRESS = "Address";
const string STR_WIFI_INFO_TAG_ESSID = "ESSID";
const string STR_WIFI_INFO_TAG_PROTOCOL = "Protocol";
const string STR_WIFI_INFO_TAG_MODE = "Mode";
const string STR_WIFI_INFO_TAG_ENCRYPTIONKEY = "Encryption Key";
const string STR_WIFI_INFO_TAG_IE = "IE";
const string STR_WIFI_INFO_TAG_GROUPCIPHER = "Group Cipher";
const string STR_WIFI_INFO_TAG_PAIRWISECIPHER = "Pairwise Ciphers";
const string STR_WIFI_INFO_TAG_AUTHENTICATIONSUITES = "Authentication Suites";
const string STR_WIFI_INFO_TAG_QUALITY = "Quality";
const string STR_WIFI_INFO_TAG_SIGNALLEVEL = "Signal level";
const string STR_WIFI_INFO_TAG_NOISELEVEL = "Noise level";


//////////////////////////////////
const string STR_WIFI_INFO_JSON_TAG_CMDID = "ID";
const string STR_WIFI_INFO_JSON_TAG_FM = "C";
const string STR_WIFI_INFO_JSON_TAG_ESSID = "ESSID";
const string STR_WIFI_INFO_JSON_TAG_ENCRYPTION_KEY = "EncryptionKey";
const string STR_WIFI_INFO_JSON_TAG_IE = "IE";
const string STR_WIFI_INFO_JSON_TAG_QUALITY = "Quality";
const string STR_WIFI_INFO_JSON_TAG_SIGNAL_LEVEL = "SignalLevel";
//


////////////////
const int TABLE_WIFIINFO_COL_CHECKBOX = 0;  // checkebox列
const int TABLE_WIFIINFO_COL_NAME  = 1; // 名称列
const int TABLE_WIFIINFO_COL_ENCRYPTION_KEY = 2;  //
const int TABLE_WIFIINFO_COL_SIGNAL_LEVEL = 3;  //




CWifiInfoWidget::CWifiInfoWidget(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::CWifiInfoWidget)
{
    ui->setupUi(this);

    if(g_pRobotController)
    {
        AddSubscribe(g_pRobotController); //添加通信消息订阅
    }

    ui->tableWidgetWifiInfos->setColumnCount(4);

    ui->tableWidgetWifiInfos->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetWifiInfos->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidgetWifiInfos->setFrameShape(QFrame::NoFrame);
    ui->tableWidgetWifiInfos->verticalHeader()->hide();
    ui->tableWidgetWifiInfos->horizontalHeader()->setFixedHeight(34);
    ui->tableWidgetWifiInfos->setShowGrid(false);
    ui->tableWidgetWifiInfos->setMouseTracking(true);
    ui->tableWidgetWifiInfos->setAlternatingRowColors(true);
    ui->tableWidgetWifiInfos->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetWifiInfos->setFocusPolicy(Qt::NoFocus); // 去掉选择虚线框
//    ui->tableActionList->setSelectionMode(QAbstractItemView::ExtendedSelection); // 设置后选中单元格，光标滑开后字体颜色不是qss设置的颜色!!!!
    ui->tableWidgetWifiInfos->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//去除水平滚动条
    ui->tableWidgetWifiInfos->setColumnWidth(TABLE_WIFIINFO_COL_CHECKBOX, 10);
    ui->tableWidgetWifiInfos->setColumnWidth(TABLE_WIFIINFO_COL_NAME, 200);
    ui->tableWidgetWifiInfos->setColumnWidth(TABLE_WIFIINFO_COL_ENCRYPTION_KEY, 100);
    ui->tableWidgetWifiInfos->setColumnWidth(TABLE_WIFIINFO_COL_SIGNAL_LEVEL, 100);

    QHeaderView* pHeaderView =  ui->tableWidgetWifiInfos->horizontalHeader();
    pHeaderView->setDefaultAlignment(Qt::AlignCenter);


    const QString strStyle = ALPHA_DEFAULT_QSS_PATH;
    UBXWidget::setDefaultStyle(this, strStyle);

    ui->labelHint->hide();

    ui->pushButtonScan->setPressInterval(4000);
    ui->pushButtonScan->enablePressInterval(true);

    retranslateUi();
}

CWifiInfoWidget::~CWifiInfoWidget()
{

    delete ui;
}

void CWifiInfoWidget::retranslateUi()
{
    ui->retranslateUi(this);

    //第一列给checkbox
    QStringList slstColNames;
    slstColNames << "" << tr("Wifi Name") << tr("Encryption Key") << tr("Signal Level");
    ui->tableWidgetWifiInfos->setHorizontalHeaderLabels(slstColNames); //设置表头

    setWindowTitle(tr("WIFI"));
}

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
void CWifiInfoWidget::OnNotify(const zxObserverSubjectBase *pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    zxEvent* pNewEvent = new zxEvent(nMsgID);
    pNewEvent->eEventType = zxEventTypeWindow;
    pNewEvent->lReceiver = (long)this;
    pNewEvent->dwMsgId = nMsgID;
    pNewEvent->wParam = wParam;
    pNewEvent->lParam = lParam;
    QCoreApplication::postEvent(this, pNewEvent);
}

void CWifiInfoWidget::customEvent(QEvent *event)
{
    zxEvent* pEvent = (zxEvent*)event;
    DWORD nMsgID = pEvent->dwMsgId;
    WPARAM wParam = pEvent->wParam;
    LPARAM lParam = pEvent->lParam;

    if (nMsgID == WM_MSG_GET_ROBOT_WIFI_LIST_RST)
    {
        processGetRobotWifiListRsp(wParam, lParam);
    }
    else if (nMsgID == WM_MSG_ROBOT_CONNECT_WIFI_RST)
    {
        processRobotConnectWifiRsp(wParam, lParam);
    }


    return;
}

/**************************************************************************
* 函数名: processGetRobotWifiListRsp
* 功能: 处理机器人wifi列表
*     * 参数:
*    @[in ] wParam: 参数1，true/false
*    @[in ] lParam: 参数2，wifi info
*
* 时间: 20170302
* 作者: asu
*/
void CWifiInfoWidget::processGetRobotWifiListRsp(WPARAM wParam, LPARAM lParam)
{
    // 主要包括三种情况：
    // 1、wParam = true, lParam = wifi info     正在接收
    // 2、wParam = true，lParam = NULL         正常接收结束
    // 3、wParam = false，lParam = NULL       读取失败

    if (!wParam)
    {
        CMessageBox::critical(this, tr("Error"), tr("Scan wifi failed!"));
        ui->pushButtonScan->setEnabled(true);
        return;
    }

    char *pWifiInfo = (char *) lParam;

    if (pWifiInfo)
    {
        string strInfo = string(pWifiInfo);
        qDebug() <<"wifi info"<< strInfo.c_str();

        QByteArray data = strInfo.data();
        QJsonParseError jsonError;
        QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
        int nRet = -1;

        if(QJsonParseError::NoError == jsonError.error)
        {
            if(parseDocument.isObject())
            {
                QJsonObject obj = parseDocument.object();
                if(!obj.empty())
                {
                    WifiInfos wifiInfo;
                    wifiInfo.strESSID = obj.take(STR_WIFI_INFO_JSON_TAG_ESSID.data()).toString();
                    wifiInfo.strEncryptionKey = obj.take(STR_WIFI_INFO_JSON_TAG_ENCRYPTION_KEY.data()).toString();
                    wifiInfo.strIE = obj.take(STR_WIFI_INFO_JSON_TAG_IE.data()).toString();
                    wifiInfo.strQuality = obj.take(STR_WIFI_INFO_JSON_TAG_QUALITY.data()).toString();
                    wifiInfo.strSignalLevel = obj.take(STR_WIFI_INFO_JSON_TAG_SIGNAL_LEVEL.data()).toString();

                    m_lstWifiInfos.push_back(wifiInfo);
                }
//                if(obj.contains(STR_WIFI_INFO_JSON_TAG_CMDID.data()))
//                {
//                    const int nCmdID = obj.take(STR_WIFI_INFO_JSON_TAG_CMDID.data()).toInt();
//                    if(eBTCmdGetWifisListIng == nCmdID)
//                    {

//                        if(obj.contains(STR_WIFI_INFO_JSON_TAG_FM.data()))
//                        {
//                            QJsonValue valueFM = obj.take(STR_WIFI_INFO_JSON_TAG_FM.data());
//                            if(valueFM.isObject())
//                            {
//                                QJsonObject objectFM = valueFM.toObject();
//                                if(!objectFM.empty())
//                                {
//                                    WifiInfos wifiInfo;
//                                    wifiInfo.strESSID = objectFM.take(STR_WIFI_INFO_JSON_TAG_ESSID.data()).toString();
//                                    wifiInfo.strEncryptionKey = objectFM.take(STR_WIFI_INFO_JSON_TAG_ENCRYPTION_KEY.data()).toString();
//                                    wifiInfo.strIE = objectFM.take(STR_WIFI_INFO_JSON_TAG_IE.data()).toString();
//                                    wifiInfo.strQuality = objectFM.take(STR_WIFI_INFO_JSON_TAG_QUALITY.data()).toString();
//                                    wifiInfo.strSignalLevel = objectFM.take(STR_WIFI_INFO_JSON_TAG_SIGNAL_LEVEL.data()).toString();

//                                    m_lstWifiInfos.push_back(wifiInfo);
//                                }
//                            }
//                        }
//                    }
//                }
            }
        }
    }

    SAFE_DELETE(pWifiInfo);

    updateWifiInfoShow(m_lstWifiInfos);
//    cJSON_AddItemToObject(pRoot, "CmdID", cJSON_CreateNumber( 111));
//    cJSON_AddItemToObject(pRoot, "FM", pFormat);
//    cJSON_AddStringToObject(pFormat, "ESSID", wifiInfo.strESSID.c_str());
//    cJSON_AddStringToObject(pFormat, "EncryptionKey", wifiInfo.strEncryptionKey.c_str());
//    cJSON_AddStringToObject(pFormat, "IE", wifiInfo.strIE.c_str());
//    cJSON_AddStringToObject(pFormat, "Quality", wifiInfo.strQuality.c_str());
    //    cJSON_AddStringToObject(pFormat, "SignalLevel", wifiInfo.strSignalLevel.c_str());
}

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
void CWifiInfoWidget::processRobotConnectWifiRsp(WPARAM wParam, LPARAM lParam)
{
    emAnsWifiSettingStat eConnectRes = (emAnsWifiSettingStat)wParam;
   // QString strConnecting = tr("Connecting");
    QString strConnectWifi = tr("Connect WiFi");
    QString strOk = tr("successfully");
    QString strFail = tr("failed");

    switch (eConnectRes)
    {
    case eAnsConnecting:
        ui->labelHint->show();
        break;

    case eAnsConectSuccessed:
        ui->labelHint->hide();
        CMessageBox::information(this, strConnectWifi, strConnectWifi + " " + strOk);
        break;

    case eAnsConnectFailed:
        ui->labelHint->hide();
        CMessageBox::warning(this, strConnectWifi, strConnectWifi + " " + strFail);
        break;

    default:
        break;
    }

    return;
}

void CWifiInfoWidget::updateWifiInfoShow(const QList<WifiInfos> &listWifiInfos)
{
    ui->tableWidgetWifiInfos->clearContents();
    ui->tableWidgetWifiInfos->setRowCount(0);
    for(int i = 0; i < listWifiInfos.count(); ++i)
    {
        addWifiInfoToTable(listWifiInfos.at(i));
    }
}

void CWifiInfoWidget::addWifiInfoToTable(const WifiInfos& wifiInfo)
{
    if (wifiInfo.strESSID.isEmpty())
    {
        return;
    }

    int nRowCount = ui->tableWidgetWifiInfos->rowCount();
    ui->tableWidgetWifiInfos->insertRow(nRowCount);

    // checkbox

    //Name
    QFont font = QApplication::font();
    font.setPixelSize(FONT_PIXEL_SIZE_12);

    QTableWidgetItem *pItemName = new QTableWidgetItem;
    pItemName->setFont(font);
//    pItemName->setForeground(QColor("#d3d3d3"));
    pItemName->setText(wifiInfo.strESSID);
    ui->tableWidgetWifiInfos->setItem(nRowCount, TABLE_WIFIINFO_COL_NAME, pItemName);


    //
    QTableWidgetItem *pItemEncryptionKey = new QTableWidgetItem;
    pItemEncryptionKey->setFont(font);
    pItemEncryptionKey->setText(wifiInfo.strEncryptionKey);
    ui->tableWidgetWifiInfos->setItem(nRowCount, TABLE_WIFIINFO_COL_ENCRYPTION_KEY, pItemEncryptionKey);

    //
    QTableWidgetItem *pItemSignalLevel = new QTableWidgetItem;
    pItemSignalLevel->setFont(font);
    pItemSignalLevel->setText(wifiInfo.strSignalLevel);
    ui->tableWidgetWifiInfos->setItem(nRowCount, TABLE_WIFIINFO_COL_SIGNAL_LEVEL, pItemSignalLevel);
}

void CWifiInfoWidget::on_pushButtonScan_clicked()
{
    m_lstWifiInfos.clear();
    g_pRobotController->Req_RobotWifiScan(eWifiInfosWifiListRestartGet);
}

void CWifiInfoWidget::on_tableWidgetWifiInfos_cellDoubleClicked(int row, int column)
{
    QTableWidgetItem* pItemESSID = ui->tableWidgetWifiInfos->item(row, TABLE_WIFIINFO_COL_NAME);
    QTableWidgetItem* pItemEncryptionKey = ui->tableWidgetWifiInfos->item(row, TABLE_WIFIINFO_COL_ENCRYPTION_KEY);

    if(pItemESSID && pItemEncryptionKey)
    {
        const QString strESSID = pItemESSID->text().trimmed();
        const QString strEncryptionKey = pItemEncryptionKey->text().trimmed();
        if(strESSID.isEmpty())
        {
            return;
        }

        bool bEncryKey = false;
        if(strEncryptionKey.contains("on", Qt::CaseInsensitive))
        {
            bEncryKey = true;
        }

        bool bOk = true;
        QString strPassKey;
        if(bEncryKey)
        {
            bOk = false;
            strPassKey = QInputDialog::getText(this,
                                               tr("Connect WiFi"),
                                               strESSID, QLineEdit::Normal,
                                               tr("Please input the password"),
                                               &bOk);
        }

        if(bOk && !strESSID.isEmpty())
        {
            if(bEncryKey && strPassKey.isEmpty())
            {
                CMessageBox::critical(this, tr("Error"), tr("Please input the password"));
                return;
            }

            g_pRobotController->Req_RobotConnectWifi(strESSID, strPassKey);
            ui->labelHint->show();
        }
    }
}
