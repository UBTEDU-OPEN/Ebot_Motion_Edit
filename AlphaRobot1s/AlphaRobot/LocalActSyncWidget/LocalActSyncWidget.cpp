/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：LocalActSyncWidget.cpp
* 创建时间：2016/01/28 15:33
* 文件标识：
* 文件摘要：本地动作同步窗口
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2016/01/28 15:33
* 版本摘要：
*/
#include "LocalActSyncWidget.h"
#include "ui_LocalActSyncWidget.h"

#include <QScrollBar>
#include <QStandardPaths>
#include <QProcess>
#include <QFileInfo>


#include "Util.h"
#include "../zxLib/configs.h"
#include "cubxudiskmanager.h"
#include "../common/filedirhandle.h"
#include "localactsyncconst.h"
#include "../zxLib/widget/messagebox.h"
#include "ubxrobotcontrolconst.h"

#include "filedirhandle.h"
#include "UBXRobotController.h"
#include "../UBXBase/UBXBase.h"
#include "../zxLib/widget/ctablewidgetitem.h"
#include "ubtlib.h"

const int TABLE_COL_CHECKBOX       = 0;
const int TABLE_COL_ID                = 1;
const int TABLE_COL_NAME            = 2;
const int TABLE_COL_SIZE              = 3;
const int TABLE_COL_DATE             = 4;
const int TABLE_COL_STATUS          = 5;



static TRANSFILEINFOLIST g_objTransFileList;  //需要传输的文件信息列表

bool CreateLocalActSyncWidget(ILocalActSyncWidget** ppLocalActSyncWidget)
{
  ILocalActSyncWidget *pProxy = new LocalActSyncWidget();
  if(pProxy)
  {
      *ppLocalActSyncWidget = pProxy;
      return true;
  }
  return false;
}

LocalActSyncWidget::LocalActSyncWidget(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::LocalActSyncWidget),
    m_nProgressCol(0),
    m_pEvent(NULL),
    m_nFileNameCol(2),
    m_bUdiskMode(false),
    m_nWaitForUdiskShowUpTimeoutId(0),
    m_nTryGetUdiskTime(LocalActSyncConstant::WAIT_FOR_UDISK_AVAILABLE_TIMEOUT/LocalActSyncConstant::QUERY_UDISK_SHOWUP_INTERNAL),
    m_bSyncing(false),
    m_bIsChangedUdiskQuiet(false)
{
    ui->setupUi(this);

//    QStringList listCols;
//    listCols << tr("") << tr("ID") << tr("Name") << tr("Size") << tr("Date modified") << tr("Status");

    ui->tableWidget_localact->setColumnCount(6);

    ui->tableWidget_localact->setColumnWidth(TABLE_COL_CHECKBOX, 40); //checkbox
    ui->tableWidget_localact->setColumnWidth(TABLE_COL_ID, 50);  //ID
    ui->tableWidget_localact->setColumnWidth(TABLE_COL_NAME, 320); //文件名
    ui->tableWidget_localact->setColumnWidth(TABLE_COL_SIZE, 200); //文件大小
    ui->tableWidget_localact->setColumnWidth(TABLE_COL_DATE, 200); //修改日期
    m_nProgressCol = TABLE_COL_STATUS;
    ui->tableWidget_localact->setColumnWidth(TABLE_COL_STATUS, 180);//状态

    ui->tableWidget_localact->setSpan(0,0,1,3);

//    ui->tableWidget_localact->setHorizontalHeaderLabels(m_cols); //设置表头
    ui->tableWidget_localact->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_localact->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_localact->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_localact->hideColumn(TABLE_COL_ID); //隐藏ID列

    ui->tableWidget_localact->setFrameShape(QFrame::NoFrame);
    ui->tableWidget_localact->setShowGrid(false);
    ui->tableWidget_localact->verticalHeader()->hide();
    ui->tableWidget_localact->horizontalHeader()->setFixedHeight(34);
    ui->tableWidget_localact->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//去除水平滚动条    

    QHeaderView* pHeaderView =  ui->tableWidget_localact->horizontalHeader();
    pHeaderView->setDefaultAlignment(Qt::AlignCenter);
    //单独设置文件列左对齐
    ui->tableWidget_localact->model()->setHeaderData(TABLE_COL_NAME, Qt::Horizontal, Qt::AlignCenter, Qt::TextAlignmentRole);

    connect(ui->tableWidget_localact, &CTableWidget::sigRowCountChanged, this, &LocalActSyncWidget::onRowCountChanged);
    ui->tableWidget_localact->startRowChangeTimer(1000);

    m_pCheckBoxSelAll = new QCheckBox(pHeaderView);
    m_pCheckBoxSelAll->setObjectName(QStringLiteral("checkBox_selall"));
    connect(m_pCheckBoxSelAll, &QCheckBox::clicked, this, &LocalActSyncWidget::onSelallClicked);
    pHeaderView->setIndexWidget(pHeaderView->model()->index(0, TABLE_COL_CHECKBOX), m_pCheckBoxSelAll);

    /****test 合并后去掉create*/
    //CreateRobotController();
    /********************/

    if(g_pRobotController)
    {
        AddSubscribe(g_pRobotController); //添加通信消息订阅
    }

    EnableCtrls(false);

//    QString sResDir;
//#ifdef Q_OS_MAC
//    sResDir = "/../res/localactsyncwidget.rcc";
//#else
//    sResDir = "/res/localactsyncwidget.rcc";
//#endif
//    QString rccPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()+ sResDir);
//    QResource::registerResource(rccPath);
    setDefaultStyle();

    ui->widgetBtnContainer->hide();
    ui->pushButton_downloadact->hide();    

    retranslateUi();
}

LocalActSyncWidget::~LocalActSyncWidget()
{
    if(g_pRobotController)
    {
        if(g_pRobotController->GetPortOpenState())
        {
            g_pRobotController->Req_Disconnect();
            //g_pRobotController->Req_ClosePort();
        }

        RemoveSubscribe(g_pRobotController); //移除通信消息订阅
        //SAFE_DELETE(g_pRobotController);

        /****test 合并后去掉create*/
        //ReleaseRobotController();
        /********************/
    }

    delete ui;
}

void LocalActSyncWidget::retranslateUi()
{
    ui->retranslateUi(this);

    QStringList listCols;
    listCols << tr("") << tr("ID") << tr("Name") << tr("Size") << tr("Date modified") << tr("Status");
    ui->tableWidget_localact->setHorizontalHeaderLabels(listCols); //设置表头
    ui->tableWidget_localact->retranslateUi();
}

/*
* brief:  初始化
* author: hels
* date:   2015-12-24
* param:  [in] parent 父窗体指针
* param:  [in] pEvent 事件通知接口指针
* return: void
*/
void LocalActSyncWidget::Connect(QWidget* parent, ILocalActSyncWidgetEvent* pEvent)
{
    m_pEvent = pEvent;
    if(parent)
    {
        QSize size = parent->size();
        setMaximumSize(size.width() * 3 / 4, size.height() * 4 / 5);
    }
}

/*
* brief:  释放资源,注意会销毁对象本身，调用此函数后，对象指针失效
* author: hels
* date:   2015-12-24
* param:  void
* return: void
*/
void LocalActSyncWidget::Disconnect()
{
    delete this;
}

//获取窗体
QWidget* LocalActSyncWidget::GetWidget()
{
    return this;
}

/**************************************************************************
* 函数名: on_pushButton_addact_clicked
* 功能: 添加动作事件响应
* 时间: 2016/01/15 15:37
* 作者: ZDJ
*/
void LocalActSyncWidget::on_pushButton_addact_clicked()
{
    QString sFmtTitle = tr("Action File");
    QString sFmt;
#ifdef Q_OS_MAC
    sFmt.sprintf("%s(*%s *%s *%s *%s);;%s(*%s *%s *%s *%s)", sFmtTitle.toStdString().c_str(), UBX_FILE_EXT, UBX_FILE_EXT_OLD, TAB_FILE_EXT, HTS_FILE_EXT, sFmtTitle.toStdString().c_str(), UBX_FILE_EXT, UBX_FILE_EXT_OLD, TAB_FILE_EXT, HTS_FILE_EXT);
#else
    sFmt.sprintf("%s(*%s *%s *%s *%s)", sFmtTitle.toStdString().c_str(), UBX_FILE_EXT, UBX_FILE_EXT_OLD, TAB_FILE_EXT, HTS_FILE_EXT);
#endif
    QString sAppLocalDownLoadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QStringList sActPaths = QFileDialog::getOpenFileNames(NULL, tr("Open File"), "/", sFmt);
    if(sActPaths.size() <= 0)
    {
        return;
    }

    for(int i = 0; i < sActPaths.size();)
    {
        bool bFind = false;
        QString sFileName = CFileDirHandle::getFileName(sActPaths[i]);
        for(int j = 0; j < ui->tableWidget_localact->rowCount(); j++)
        {
            QTableWidgetItem* pItem = ui->tableWidget_localact->item(j, m_nFileNameCol);
            if(sFileName.compare(pItem->text()) == 0)
            {
                sActPaths.removeAt(i);
                bFind = true;
                break;
            }
        }
        if(!bFind)
            i++;
    }

    int nOldRowCount = ui->tableWidget_localact->rowCount();
    int nNewRowCount = nOldRowCount + sActPaths.size();
    ui->tableWidget_localact->setRowCount(nNewRowCount);

    for(int i = nOldRowCount, k = 0; i < nNewRowCount; i++, k++)
    {
        QString sTempStr = QDir::toNativeSeparators(sActPaths[k]);

        ACTFILEDATA* pNewData = new ACTFILEDATA;
        pNewData->m_sFilePath = sTempStr;
        pNewData->m_sFileName = CFileDirHandle::getFileName(sTempStr);

        QFileInfo fileinfo(sTempStr);
        pNewData->m_nFileSize = fileinfo.size();

        QDateTime timetemp = fileinfo.lastModified();
        sTempStr = timetemp.toString("yyyy-MM-dd hh:mm:ss");
        pNewData->m_sFileModTime = sTempStr;

        int nID = GetNewID();
        ui->tableWidget_localact->setUserData(nID, pNewData);

#if 0
        //复选框
        QCheckBox* pChkBox = new QCheckBox;        
        ui->tableWidget_localact->setCellWidget(i, TABLE_COL_CHECKBOX, pChkBox);
        connect(pChkBox, SIGNAL(clicked(bool)), this, SLOT(onTableWidgetCheckBoxClicked(bool)));

        //ID
        sTempStr.sprintf("%d", nID);
        QTableWidgetItem* pItem = new QTableWidgetItem(sTempStr);
        pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_localact->setItem(i, TABLE_COL_ID, pItem);

        //文件名称
        pItem = new QTableWidgetItem(pNewData->m_sFileName);
        pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_localact->setItem(i, TABLE_COL_NAME, pItem);

        //文件大小        
        sTempStr = GetFormatSizeString(fileinfo.size());
        pItem = new QTableWidgetItem(sTempStr);
        pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_localact->setItem(i, TABLE_COL_SIZE, pItem);

        //修改日期
        pItem = new QTableWidgetItem(pNewData->m_sFileModTime);
        pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_localact->setItem(i, TABLE_COL_DATE, pItem);

        //状态
        sTempStr = tr("Unsynchronized");
        pItem = new QTableWidgetItem(sTempStr);
        pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_localact->setItem(i, TABLE_COL_STATUS, pItem);
  #endif

        //复选框
        //ID
        //文件名称
        QString strFileName = pNewData->m_sFileName;
        //文件大小
        qint64 nFileSize = fileinfo.size();
        //修改日期
        QString fileModifyTime = pNewData->m_sFileModTime;
        addToTableWidget(i, nID, strFileName, nFileSize, fileModifyTime);


        ui->tableWidget_localact->setRowHeight(i, 52);
        SigAddedAction(pNewData->m_sFilePath);
    }
}

/**************************************************************************
* 函数名: on_pushButton_removeact_clicked
* 功能: 移除动作事件响应
* 时间: 2016/01/15 15:37
* 作者: ZDJ
*/
void LocalActSyncWidget::on_pushButton_removeact_clicked()
{

    for(int i = 0; i < ui->tableWidget_localact->rowCount(); )
    {
        QCheckBox* pChkBox = dynamic_cast<QCheckBox*>(ui->tableWidget_localact->cellWidget(i, TABLE_COL_CHECKBOX));
        if(!pChkBox || (pChkBox && !pChkBox->isChecked()))
        {
            i++;
            continue;
        }
        QTableWidgetItem* pItemId = ui->tableWidget_localact->item(i, TABLE_COL_ID);
        int nID = pItemId->text().toInt();
        ACTFILEDATA* pItemData = (ACTFILEDATA*)ui->tableWidget_localact->userData(nID);
        if(pItemData)
        {
            QString sFilePath = pItemData->m_sFilePath; //动作文件本地路径
            SigRemoveAction(sFilePath);
        }
        ui->tableWidget_localact->removeRow(i);
    }
}

/**************************************************************************
* 函数名: on_pushButton_downloadact_clicked
* 功能: 同步动作事件响应
* 时间: 2016/01/15 15:37
* 作者: ZDJ
*/
void LocalActSyncWidget::on_pushButton_downloadact_clicked()
{
    if(m_bSyncing)
    {
        //正在同步中则取消同步
        if(g_pRobotController && g_pRobotController->GetPortOpenState())
        {
            g_objTransFileList.Reset();
            g_pRobotController->Req_CancelTransFile();
        }

        m_bSyncing = false;

        ui->pushButton_downloadact->setText(tr("Syncing action"));

        for(int i = 0; i < ui->tableWidget_localact->rowCount(); i++)
        {
            QCheckBox* pChkBox = dynamic_cast<QCheckBox*>(ui->tableWidget_localact->cellWidget(i, TABLE_COL_CHECKBOX));
            QTableWidgetItem* pItem = ui->tableWidget_localact->item(i, m_nProgressCol);
            if(pItem && (pChkBox && pChkBox->isChecked()))
            {
                if(ui->tableWidget_localact->cellWidget(i, m_nProgressCol))
                {
                    ui->tableWidget_localact->removeCellWidget(i, m_nProgressCol);
                   //pItem->setText(tr("Unsynchronized"));
                    CTableWidgetItem *pItemCtable = dynamic_cast<CTableWidgetItem*>(pItem);
                    if(pItemCtable)
                    {
                        pItemCtable->setTextRetranslate("Unsynchronized", tr("Unsynchronized"));
                    }
                }
            }
        }

        EnableCtrls(false, false);
//        Sleep(2000);
//        EnableCtrls(true, true);
        return;
    }

    if(!g_pRobotController || (g_pRobotController && !g_pRobotController->GetPortOpenState()))
    {
        CMessageBox::warning(this, "", tr("Please connect to the robot"));
        return;
    }

    g_objTransFileList.Reset();

    for(int i = 0; i < ui->tableWidget_localact->rowCount(); i++)
    {
        QCheckBox* pChkBox = dynamic_cast<QCheckBox*>(ui->tableWidget_localact->cellWidget(i, TABLE_COL_CHECKBOX));
        if(!pChkBox || (pChkBox && !pChkBox->isChecked()))
            continue;

        QTableWidgetItem* pItemId = ui->tableWidget_localact->item(i, TABLE_COL_ID);
        int nID = pItemId->text().toInt();
        ACTFILEDATA* pItemData = (ACTFILEDATA*)ui->tableWidget_localact->userData(nID);
        if(!pItemData)
            continue;

        TRANSFILEINFO transfileinfo;
        QString sFileName = pItemData->m_sFileName;
        const QString sFilePath = pItemData->m_sFilePath; //动作文件本地路径
        QString sTransFilePath = sFilePath;

        //#pragma region 检查文件格式
        char* pHtsData = NULL;
        int nHtsLen = 0;

        int nMotorCount = getMotorCount();
        bool bIsNewVersion = nMotorCount > 16;  //用舵机数量判断版本，有所不妥

        QString sExt = CUtil::GetFileExt(sFilePath);
        sExt = sExt.toLower();
        if(sExt == UBX_FILE_EXT_OLD || sExt == UBX_FILE_EXT)
        {
            try
            {
                BOOL bRet = FALSE;
                if(!bIsNewVersion)
                    bRet = CreateHtsDataFromAes(sFilePath.toStdString(), pHtsData, nHtsLen);
                else
                    bRet = CreateHtsDataFromAes(sFilePath.toStdString(), 0, 0, "", pHtsData, nHtsLen);

                if(!bRet)
                {
                    CMessageBox::warning(this, "", sFilePath + tr("create hts data failed."));
                    continue;
                }
            }
            catch(...)
            {
                SAFE_DELETE_ARRAY(pHtsData);
                CMessageBox::warning(this, "", sFilePath + tr("syncing failed."));
                continue;
            }
        }
        if(pHtsData)
        {
#ifdef Q_OS_WIN
            sTransFilePath = sFilePath.left(sFilePath.lastIndexOf('.')) + HTS_FILE_EXT; //hts文件路径
#else
            QString sTransFileDir = CConfigs::getLocalTempDir();
            QDir dir;
            bool exist = dir.exists(sTransFileDir);
            if(!exist)
            {
                dir.mkdir(sTransFileDir);
            }
            sTransFilePath = QDir::toNativeSeparators(sTransFileDir + '/' + sFileName.left(sFileName.lastIndexOf('.')) + HTS_FILE_EXT);
#endif

            sFileName = CFileDirHandle::getFileName(sTransFilePath);


            QFile fileobj(sTransFilePath);
            if(fileobj.open(QIODevice::WriteOnly))
            {
                fileobj.write(pHtsData, nHtsLen);
                fileobj.close();
            }
            else
            {
                CMessageBox::warning(this, "", sFilePath + tr("create hts file failed."));
            }
            SAFE_DELETE_ARRAY(pHtsData);
        }
        //#pragma endregion

        if(!PathFileExists(sTransFilePath))
        {
            CMessageBox::warning(this, "", sTransFilePath + tr("does not exist"));
            continue;
        }

        QString sDestFilePath = "action/" + sFileName;
        if(g_pRobotController->IsSupportNewProtocol())
        {
            if(pItemData->m_bLocalFile)
                sDestFilePath = "action/my creation/" + sFileName; //本地文件传输到新的子目录
            else
                sDestFilePath = "action/my download/" + sFileName; //下载的文件传输到新的子目录
        }

        char* pGuid = NULL;
        char* pSrcFile = NULL;
        char* pTransFile = NULL;
        char* pDestFile = NULL;

        int nGuidLen = UnicodeToAnsi(pItemData->m_sGuid, pGuid);
        int nSrcFileLen = UnicodeToAnsi(sFilePath, pSrcFile);
        int nTransFileLen = UnicodeToAnsi(sTransFilePath, pTransFile);
        int nDestFileLen = UnicodeToAnsi(sDestFilePath, pDestFile);

        memcpy(transfileinfo.sGuid, pGuid, nGuidLen);
        memcpy(transfileinfo.sSrcFilePath, pSrcFile, nSrcFileLen);
        memcpy(transfileinfo.sTransFilePath, pTransFile, nTransFileLen);
        memcpy(transfileinfo.sDestFilePath, pDestFile, nDestFileLen);

        SAFE_DELETE_ARRAY(pGuid);
        SAFE_DELETE_ARRAY(pSrcFile);
        SAFE_DELETE_ARRAY(pTransFile);
        SAFE_DELETE_ARRAY(pDestFile);

        transfileinfo.lFileSize = CUtil::GetFileSize(sTransFilePath);
        transfileinfo.lTransTimeBegin = QDateTime::currentDateTime().toTime_t(); //记录传输开始时间

        g_objTransFileList.vecTransFileInfo.push_back(transfileinfo);        
    }

    if(g_objTransFileList.vecTransFileInfo.size() > 0)
    {
//        m_dlgProgress.Reset();
//        m_dlgProgress.ShowWindow(SW_SHOW);
//        m_dlgProgress.CenterWindow();

        TRANSFILEINFO* pTransFileInfo = &g_objTransFileList.vecTransFileInfo[0];
        QString sGuidTemp = QString::fromStdString(pTransFileInfo->sGuid);

        int nItem = FindItemIndex(sGuidTemp);
        QProgressBar* pProgressbar = new QProgressBar;
        ui->tableWidget_localact->setCellWidget(nItem, m_nProgressCol, pProgressbar);

        QTableWidgetItem* pItem = FindItem(sGuidTemp);
        pItem->setText(NULLSTR);

        QString sTextOri = ui->pushButton_downloadact->text();
        ui->pushButton_downloadact->setText(tr("cancel sync"));
        m_bSyncing = true;
        EnableCtrls(false, true);

        OnNotify(g_pRobotController, WM_MSG_ACTION_DOWNLOAD_PREPARE, NULL, NULL, NULL);

        /////////////////////////////////Log////////////////////////////////////////
        QString strLog = QString("Start transfer file %1\n").arg(QString(pTransFileInfo->sSrcFilePath));
        AlphaRobotLog::getInstance()->printDebug(strLog);
        /////////////////////////////////Log////////////////////////////////////////

        BOOL bRet = g_pRobotController->Req_TransFile(pTransFileInfo, emTranFileSynAction);
        if(!bRet)
        {
            //CMessageBox::warning(this, "", "传输文件请求失败");

            //QProgressBar* pProgressBar = FindItemProgress(sGuidTemp);
#if 0
            SAFE_DELETE(pProgressbar);
#endif
            ui->tableWidget_localact->removeCellWidget(nItem, m_nProgressCol);

            pItem->setText(tr("Failed"));
            AlphaRobotLog::getInstance()->printDebug(QString("LocalActSyncWidget::OnDownloadEnd, Failed3"));

            ui->pushButton_downloadact->setText(sTextOri);
            m_bSyncing = false;
            EnableCtrls(true, true);
        }
    }
    else
    {
        CMessageBox::warning(this, "", tr("Please select action table"));
    }
}

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
void LocalActSyncWidget::OnNotify(const zxObserverSubjectBase* pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    if(nMsgID == WM_MSG_ROBOT_CONNECT)
    {
        if(!lParam)
        {
            if(wParam)
            {
                //ui->pushButton_send->setEnabled(true);
                //ui->pushButton_open->setEnabled(true);
                //ui->pushButton_open->setText(tr("Disconnect"));

                m_bUdiskMode = false;
                EnableCtrls(true);

                // 连接上机器人后检测内存卡是否存在，如果不存在则不支持同步动作，掉电操作，
                // 下载动作列表不能添加动作到同步列表，请求到状态后会重新设置对应的按钮状态
                if (g_pRobotController)
                {
                    g_pRobotController->Req_RobotDeviceStatus();
                }

                emit PortOpenChanged(TRUE);
            }
            else
            {
                //ui->pushButton_open->setEnabled(true);
                CMessageBox::warning(this, tr("Warning"), tr("Failed to connect robot!"));
            }
        }
    }
    else if(nMsgID == WM_MSG_ROBOT_DISCONNECT)
    {
        if(!lParam)
        {
            if(wParam)
            {
                //ui->pushButton_open->setEnabled(true);
                //ui->pushButton_open->setText(tr("Connect To Robot"));

                EnableCtrls(false);

                emit PortOpenChanged(FALSE);
            }
            else
            {

            }
        }
    }
    else if(nMsgID == WM_MSG_ROBOT_TRANSDATA)
    {
        const char* pStr = (const char*)lParam;

//        QString sResult = ui->textEdit_recvdata->toPlainText();
//        for(int i = 0; i < wParam; i++)
//        {
//            QString sTemp;
//            sTemp.sprintf("%02X ", (byte)pStr[i]);

//            sResult += sTemp;
//        }

//        sResult += "\n";
        //ui->textEdit_recvdata->setText(sResult);
    }
    else if(nMsgID == WM_MSG_ACTION_DOWNLOAD_PREPARE)
    {

    }
    else if(nMsgID == WM_MSG_ACTION_DOWNLOAD_ING)
    {
        OnDownloadProgress(wParam, lParam);
    }
    else if(nMsgID == WM_MSG_ACTION_DOWNLOAD_END)
    {
        OnDownloadEnd(wParam, lParam);
    }
    else if (nMsgID == WM_MSG_TRANS_FILE_CANCEL)
    {
        onTranFileCancel(wParam, lParam);
    }
    else if(nMsgID == WM_MSG_ROBOT_SWITCHMODE_RESULT)
    {
        if(wParam)
        {
            m_bUdiskMode = true;

            EnableCtrls(false);
            if (!m_bIsChangedUdiskQuiet)
            {
                CMessageBox::information(this, "", tr("The robot has been sucessfully switched to removable disk, "
                                                      "please re-plug the USB cable or press the reset button on the back of the robot to switch back to the robot mode!"));

                popUpRobotUdisk();  // 自动弹出移动盘
            }
        }
        else
        {
             ui->pushButton_udiskmode->setEnabled(true);
            CMessageBox::warning(this, "", tr("Switch failed."));
        }
    }
    else if (nMsgID == WM_MSG_TFCARD_NO_CAPACITY)
    {
        processTFCardCapacityRsp(wParam, lParam);
    }
}

/**************************************************************************
* 函数名: onPortOpenClick
* 功能: 打开端口事件响应
* 时间: 2016/01/15 15:37
* 作者: ZDJ
*/
void LocalActSyncWidget::onPortOpenClick()
{
    if(g_pRobotController)
    {
        if(!g_pRobotController->GetPortOpenState())
        {
            //ui->pushButton_open->setEnabled(false);
            g_pRobotController->Req_Connect();
        }
        else
        {
            g_pRobotController->Req_Disconnect();
            //g_pRobotController->Req_ClosePort();

            //ui->pushButton_send->setEnabled(false);
            //ui->pushButton_open->setEnabled(true);
            //ui->pushButton_open->setText(tr("Connect To Robot"));

            EnableCtrls(false);

            emit PortOpenChanged(FALSE);
        }
    }
}

/**************************************************************************
* 函数名: onRowCountChanged
* 功能: refrash the tabel checkbox state
* 返回值: void
* 时间: 20161107
* 作者: asu
*/
void LocalActSyncWidget::onRowCountChanged(int)
{
    if(m_pCheckBoxSelAll)
    {
        onTableWidgetCheckBoxClicked(false);
    }
}

/**************************************************************************
* 函数名: OnDownloadProgress
* 功能: 下载进度消息响应
* 时间: 2015/10/18 13:43
* 作者: ZDJ
*/
LRESULT LocalActSyncWidget::OnDownloadProgress(WPARAM wParam, LPARAM lParam)
{
    SENDDATAOBJ* pCurSendFileInfo = (SENDDATAOBJ*)lParam;
    if (!pCurSendFileInfo)
    {
        return 0;
    }

    if (pCurSendFileInfo->m_eType != emTranFileSynAction)  // 只处理文件同步
    {
        return 0;
    }

    int nIndex = FindItemIndex(pCurSendFileInfo->m_sGuid);
    if (nIndex <= -1)
    {
        return 0;
    }

    TRANSFILEINFO* pTransInfo = g_objTransFileList.FindDataByGuid(pCurSendFileInfo->m_sGuid.toStdString().c_str());
    if (!pTransInfo)
    {
        return 0;
    }

    QProgressBar* pCurProgressbar = FindItemProgress(pTransInfo->sGuid);
    QTableWidgetItem* pItem = ui->tableWidget_localact->item(nIndex, m_nProgressCol);
    if (!pCurProgressbar || !pItem)
    {
        return 0;
    }

    // 文件同步过程中出错
    if (!wParam)
    {
        QString strTransFilePath = AnsiToUnicode(pCurSendFileInfo->m_szTransFilePath);
        AlphaRobotLog::getInstance()->printError(QString("Action download to robot failed: %1").arg(strTransFilePath));
        pCurProgressbar->close();
        //pItem->setText(tr("Unsynchronized"));
        CTableWidgetItem *pItemCtable = dynamic_cast<CTableWidgetItem*>(pItem);
        if(pItemCtable)
        {
            pItemCtable->setTextRetranslate("Unsynchronized", tr("Unsynchronized"));
        }
        syncNextAction();
        //CMessageBox::critical(NULL, tr(""), tr("Action download to robot failed: ") + strTransFilePath);  // 2016070416版本固件取消传输会返回05类型错误
        return 0;
    }

    pTransInfo->lSendedSize = pCurSendFileInfo->m_nCurSendSize; //当前文件已传输的长度

    //int lTotolSize = g_objTransFileList.GetTotalSize();
    //int lAllSendedSize = g_objTransFileList.GetSendedSize();
    int lTotolSize = pCurSendFileInfo->m_senddata.datalen;   //计划发送的当前文件长度
    int lAllSendedSize = pCurSendFileInfo->m_nCurSendSize; //当前文件已经发送的数据长度

    int nProgress = lAllSendedSize * 1.0 / lTotolSize * 100;
    int nLastProgress = pCurProgressbar->value();

    if(nProgress - nLastProgress >= 1)
    {
        pCurProgressbar->setValue(nProgress);
#if 0
        QString sShortName = CFileDirHandle::getFileName(pTransInfo->sSrcFilePath);
        QString sInfo = tr("Syncing... ") + sShortName + tr("，Time Remaining");

        qint64 lCurTime = QDateTime::currentDateTime().toTime_t();//单位秒
        qint64 lDifTime = (lCurTime - pTransInfo->lTransTimeBegin); //时间差
        if(lDifTime == 0)
            lDifTime = 1;
        int nTransRate = lAllSendedSize / lDifTime;
        if(nTransRate == 0)
            nTransRate = 1;
        int nRemainTime = (lTotolSize - lAllSendedSize) / nTransRate;

        QString sTimeStr;
        if(nRemainTime >= 3600)
        {
            sTimeStr.sprintf("%02d:%02d:%02d", nRemainTime / 3600, nRemainTime % 3600 / 60, nRemainTime % 3600 % 60);
        }
        else
        {
            sTimeStr.sprintf("%02d:%02d", nRemainTime / 60, nRemainTime % 60);
        }

        sInfo += " " + sTimeStr + " ...";
#endif
    }

    return 0;
}

/**************************************************************************
* 函数名: OnDownloadEnd
* 功能: 下载结束消息响应
* 时间: 2015/10/18 13:43
* 作者: ZDJ
*/
LRESULT LocalActSyncWidget::OnDownloadEnd(WPARAM wParam, LPARAM lParam)
{
    SENDDATAOBJ* pCurSendFileInfo = (SENDDATAOBJ*)lParam;
    if (!pCurSendFileInfo)
    {
        return 0;
    }

    if (pCurSendFileInfo->m_eType != emTranFileSynAction)  // 只处理文件同步
    {
        return 0;
    }

    int nIndex = FindItemIndex(pCurSendFileInfo->m_sGuid);
    if (nIndex <= -1)
    {
        return 0;
    }

    QTableWidgetItem* pItemId = ui->tableWidget_localact->item(nIndex, TABLE_COL_ID);
    QTableWidgetItem* pItem = ui->tableWidget_localact->item(nIndex, m_nProgressCol);

    //设置动作单元已同步状态
    //QString sGuid = pCurSendFileInfo->m_sGuid;
    //for(int i = 0; i < m_vecActWnd.size(); i++)
    //{
    //ACTDATAWND* pActDataWnd = m_vecActWnd[i];
    int nID = pItemId->text().toInt();
    ACTFILEDATA* pData = (ACTFILEDATA*)ui->tableWidget_localact->userData(nID);
    if(pCurSendFileInfo->m_sGuid.compare(pData->m_sGuid) == 0)
    {
        //CLocalActUnit* pUnit = (CLocalActUnit*)pActDataWnd->pWnd;
#if 0
        SAFE_DELETE(pProgressBar);
#endif
        ui->tableWidget_localact->removeCellWidget(nIndex, m_nProgressCol);

        AlphaRobotLog::getInstance()->printDebug(QString("Download file %1 finished").arg(QString(pCurSendFileInfo->m_szSrcPath)));

        if(wParam)
        {
            //pUnit->GetSyncDoneCtrl()->ShowWindow(SW_SHOW);
            pItem->setText(tr("Successful"));
        }
        else
        {
            //pUnit->GetSyncDoneCtrl()->ShowWindow(SW_HIDE);

            //QString sFileName = CFileDirHandle::getFileName(pCurSendFileInfo->m_szSrcPath);
            //CMessageBox::warning(this, "", sFileName + "同步失败");

            QString strTransFilePath = AnsiToUnicode(pCurSendFileInfo->m_szTransFilePath);
            AlphaRobotLog::getInstance()->printError(QString("Action download to robot failed: %1").arg(strTransFilePath));

            /////////////////////////////////Log////////////////////////////////////////
            AlphaRobotLog::getInstance()->printDebug(QString("LocalActSyncWidget::OnDownloadEnd, Failed2"));
            pItem->setText(tr("Failed"));
        }

        //break;
    }

    syncNextAction();
    return 0;
}

/**************************************************************************
* 函数名: onTranFileCancel
* 功能: 取消文件传输结果响应
* 时间: 2016/7/18 13:43
* 作者: hwx
*/
LRESULT LocalActSyncWidget::onTranFileCancel(WPARAM wParam, LPARAM lParam)
{
    EnableCtrls(true, false);
    if (wParam)
    {
        AlphaRobotLog::getInstance()->printDebug(tr("Synchronize cancel successfully"));
        return 0;
    }

    CMessageBox::warning(this, "", tr("Synchronize cancel failed."));
    return 0;
}

void LocalActSyncWidget::processTFCardCapacityRsp(WPARAM wParam, LPARAM lParam)
{
    if (wParam && !lParam)
    {
        CMessageBox::critical(NULL, tr(""), tr("Robot TFCard has no capacity. "));
        on_pushButton_downloadact_clicked(); // 取消同步
        return;
    }
}

/**************************************************************************
* 函数名: FindItemProgress
* 功能: 获取行进度条
* 参数:
*    @[in ] sGuid：guid值
* 返回值: 行进度条
* 时间: 2016/01/13 17:49
* 作者: ZDJ
*/
QProgressBar* LocalActSyncWidget::FindItemProgress(const QString& sGuid)
{
    int nIndex = FindItemIndex(sGuid);
    if(nIndex != -1)
    {
        return (QProgressBar*)ui->tableWidget_localact->cellWidget(nIndex, m_nProgressCol);
    }

    return NULL;
}

/**************************************************************************
* 函数名: FindItem
* 功能: 获取列表项
* 参数:
*    @[in ] sGuid：guid值
* 返回值: 列表项
* 时间: 2016/01/13 17:49
* 作者: ZDJ
*/
QTableWidgetItem* LocalActSyncWidget::FindItem(const QString& sGuid)
{
    int nIndex = FindItemIndex(sGuid);
    if(nIndex != -1)
    {
        return ui->tableWidget_localact->item(nIndex, m_nProgressCol);
    }

    return NULL;
}

/**************************************************************************
* 函数名: FindItemIndex
* 功能: 获取行索引
* 参数:
*    @[in ] sGuid：guid值
* 返回值: 行索引
* 时间: 2016/01/13 17:49
* 作者: ZDJ
*/
int LocalActSyncWidget::FindItemIndex(const QString& sGuid)
{
    for(int i = 0; i < ui->tableWidget_localact->rowCount(); i++)
    {
        QTableWidgetItem* pItemId = ui->tableWidget_localact->item(i, TABLE_COL_ID);
        int nID = pItemId->text().toInt();
        ACTFILEDATA* pData = (ACTFILEDATA*)ui->tableWidget_localact->userData(nID);
        if(pData->m_sGuid.compare(sGuid) == 0)
        {
            return i;
        }
    }

    return -1;
}

/**************************************************************************
* 函数名: on_pushButton_udiskmode_clicked
* 功能: U盘模式点击事件响应
* 时间: 2016/01/15 15:37
* 作者: ZDJ
*/
void LocalActSyncWidget::on_pushButton_udiskmode_clicked()
{
    // 针对连续多次点击的问题，如果第一次已经切换，则不予处理
    AlphaRobotLog::getInstance()->printDebug("changed udisk mode");
    ui->pushButton_udiskmode->setEnabled(false);

    if(g_pRobotController && g_pRobotController->GetPortOpenState())
    {
        g_pRobotController->Req_SwitchMode(eRobotSwitchModeUdisk);
    }
    else
    {
        CMessageBox::warning(this, "", tr("Please connect to the robot"));
    }
}

/**************************************************************************
* 函数名: EnableCtrls
* 功能: 设置控件启用状态
* 参数:
*    @[in ] bEnable: 启用标识
*    @[in ] bIsSyncing: 同步状态
* 返回值: void
* 时间: 2016/01/13 14:40
* 作者: ZDJ
*/
void LocalActSyncWidget::EnableCtrls(bool bEnable, bool bIsSyncing)
{
    ui->pushButton_addact->setEnabled(bEnable);
    ui->pushButton_removeact->setEnabled(bEnable);
    if(!bIsSyncing)
    {
        ui->pushButton_downloadact->setEnabled(bEnable);
    }

    ui->pushButton_udiskmode->setEnabled(bEnable);    
    if(eUbtProduct1E == UBXBase::getInstance()->getCurrentRobotProductType())
    {
        ui->pushButton_udiskmode->setEnabled(false);
    }

    //ui->tableWidget_localact->setEnabled(bEnable);
    m_pCheckBoxSelAll->setEnabled(bEnable);
    ui->pbAddLocalAct->setEnabled(bEnable);

    if(!bEnable && !bIsSyncing)
    {
        m_bSyncing = false;
        ui->pushButton_downloadact->setText(tr("Syncing action"));

        for(int i = 0; i < ui->tableWidget_localact->rowCount(); i++)
        {
            QCheckBox* pChkBox = dynamic_cast<QCheckBox*>(ui->tableWidget_localact->cellWidget(i, TABLE_COL_CHECKBOX));
            QTableWidgetItem* pItem = ui->tableWidget_localact->item(i, m_nProgressCol);
            if(pItem && (pChkBox && pChkBox->isChecked()))
            {
                if(ui->tableWidget_localact->cellWidget(i, m_nProgressCol))
                {
                    ui->tableWidget_localact->removeCellWidget(i, m_nProgressCol);
                    //pItem->setText(tr("Unsynchronized"));
                    CTableWidgetItem *pItemCtable = dynamic_cast<CTableWidgetItem*>(pItem);
                    if(pItemCtable)
                    {
                        pItemCtable->setTextRetranslate("Unsynchronized", tr("Unsynchronized"));
                    }
                }
            }
        }
    }

    for(int i = 0; i < ui->tableWidget_localact->rowCount(); i++)
    {
        QCheckBox* pChkBox = dynamic_cast<QCheckBox*>(ui->tableWidget_localact->cellWidget(i, 0));
        if(pChkBox)
        {
            pChkBox->setEnabled(bEnable);
        }
    }
}

/**************************************************************************
* 函数名: onSelallClicked
* 功能: 全选复选框点击事件响应
* 时间: 2016/01/15 15:37
* 作者: ZDJ
*/
void LocalActSyncWidget::onSelallClicked()
{
    Qt::CheckState state = m_pCheckBoxSelAll->checkState();
    CTableWidget::setAllCheckBoxs(ui->tableWidget_localact, TABLE_COL_CHECKBOX, state);
}

/**************************************************************************
* 函数名: onTableWidgetCheckBoxClicked
* 功能: 行首复选框点击响应
* 参数:
*    @[in ] bChecked: 选择标识
* 返回值: void
* 时间: 2016/01/15 15:37
* 作者: ZDJ
*/
void LocalActSyncWidget::onTableWidgetCheckBoxClicked(bool bChecked)
{
    QCheckBox* pSender = qobject_cast<QCheckBox*>(sender());
    Qt::CheckState eAllSelect = Qt::Checked;
    for(int i = 0; i < ui->tableWidget_localact->rowCount(); i++)
    {
        QCheckBox* pChkbox = (QCheckBox*)ui->tableWidget_localact->cellWidget(i, TABLE_COL_CHECKBOX);
        if(pSender == pChkbox)
        {
            if(bChecked)
            {
                ui->tableWidget_localact->selectRow(i);
            }
            else
            {
                //Qt默认提供的QTableWidget没有提供移除选中的功能，自行实现
                //先记录所有选中行
                QList<int> listSelected;
                for(int j = 0; j < ui->tableWidget_localact->rowCount(); j++)
                {
                    QCheckBox* pChkbox = (QCheckBox*)ui->tableWidget_localact->cellWidget(j, TABLE_COL_CHECKBOX);

                    if(pChkbox && (pChkbox->checkState() == Qt::Checked))
                    {
                        listSelected.append(j);
                    }
                }
                ui->tableWidget_localact->clearSelection();

                foreach (int nSel, listSelected)
                {
                    if(nSel != i)
                    {
                        ui->tableWidget_localact->selectRow(nSel);
                    }
                }
            }
        }
        if(pChkbox && (pChkbox->checkState() == Qt::Unchecked))
        {
            eAllSelect = Qt::Unchecked;
            break;
        }
    }

    if(ui->tableWidget_localact->rowCount() == 0)
    {
        eAllSelect = Qt::Unchecked;
    }

    m_pCheckBoxSelAll->setCheckState(eAllSelect);
}

void LocalActSyncWidget::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/localactsyncwidgetdefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

/**************************************************************************
* 函数名: GetNewID
* 功能: 获取新增ID
* 返回值: 新增ID
* 时间: 2016/01/15 15:37
* 作者: ZDJ
*/
int LocalActSyncWidget::GetNewID()
{
    static int nID = 1;
    return nID++;
}

/**************************************************************************
* 函数名: popUpRobotUdisk
* 功能: 弹出机器人移动U盘
* 返回值:
* 时间: 2016/11/12 15:37
* 作者: hwx
*/
void LocalActSyncWidget::popUpRobotUdisk()
{
    m_nTryGetUdiskTime = LocalActSyncConstant::WAIT_FOR_UDISK_AVAILABLE_TIMEOUT/LocalActSyncConstant::QUERY_UDISK_SHOWUP_INTERNAL,

    m_nWaitForUdiskShowUpTimeoutId = startTimer(LocalActSyncConstant::QUERY_UDISK_SHOWUP_INTERNAL);
}

/**************************************************************************
* 函数名: openDirectory
* 功能: 调用系统接口打开指定目录
*    @[in ] strDir: 指定打开的路径
* 返回值:
* 时间: 2016/11/12 15:37
* 作者: hwx
*/
void LocalActSyncWidget::openDirectory(const QString &strDir)
{
    QDir dir(strDir);
    if (!dir.exists())
    {
        AlphaRobotLog::getInstance()->printError(QString("Dir %1 not exist").arg(strDir));
        return;
    }
#ifdef Q_OS_WIN
    QProcess::startDetached("explorer " + strDir);
#else
    QStringList strList;
    strList.clear();
    QProcess::startDetached("open" , strList << strDir);
#endif

}

/**************************************************************************
* 函数名: syncNextAction
* 功能: 同步下一个动作
*
* 返回值:
* 时间: 2017/04/05
* 作者: hwx
*/
void LocalActSyncWidget::syncNextAction()
{
    //继续传输下一个动作文件
    int nLastIndex = g_objTransFileList.vecTransFileInfo.size() - 1;
    if(g_objTransFileList.nCurIndex < nLastIndex)
    {
        g_objTransFileList.nCurIndex++;
        TRANSFILEINFO* pTransFileInfo = &g_objTransFileList.vecTransFileInfo[g_objTransFileList.nCurIndex];
        if (!pTransFileInfo)
        {
            return;
        }

        int nItem = FindItemIndex(pTransFileInfo->sGuid);
        QProgressBar* pProgressbar = new QProgressBar;
        ui->tableWidget_localact->setCellWidget(nItem, m_nProgressCol, pProgressbar);

        QString sGuidTemp = QString::fromStdString(pTransFileInfo->sGuid);
        QTableWidgetItem* pItem = FindItem(sGuidTemp);
        pItem->setText(NULLSTR);

        /////////////////////////////////Log////////////////////////////////////////
        QString strLog = QString("Start transfer file %1\n").arg(QString(pTransFileInfo->sSrcFilePath));
        AlphaRobotLog::getInstance()->printDebug(strLog);
        /////////////////////////////////Log////////////////////////////////////////

        BOOL bRet = g_pRobotController->Req_TransFile(pTransFileInfo, emTranFileSynAction);
        if(!bRet)
        {
            QString sShortName = CFileDirHandle::getFileName(pTransFileInfo->sSrcFilePath);
            //AfxMessageBox(LANGUAGE_LOAD(ID_TEXT_TRANSFILE_REQ) + sShortName + LANGUAGE_LOAD(ID_TEXT_FAILED));

            //m_dlgProgress.ShowWindow(SW_HIDE);
            //m_dlgProgress.Reset();
#if 0
            SAFE_DELETE(pProgressbar);
#endif
            ui->tableWidget_localact->removeCellWidget(nItem, m_nProgressCol);

            /////////////////////////////////Log////////////////////////////////////////
            AlphaRobotLog::getInstance()->printDebug(QString("LocalActSyncWidget::OnDownloadEnd, Failed1"));
            pItem->setText(tr("Failed"));
        }
    }
    else
    {
        //传输完成
        //m_dlgProgress.ShowWindow(SW_HIDE);
        //m_dlgProgress.Reset();
        m_bSyncing = false;
        EnableCtrls(true, true);
        ui->pushButton_downloadact->setText(tr("Syncing action"));
    }
}

/**************************************************************************
* 函数名: OnPortOpenChanged
* 功能: 端口改变消息响应
* 时间: 2016/01/28 17:39
* 作者: ZDJ
*/
void LocalActSyncWidget::OnPortOpenChanged(BOOL bOpen)
{
    if(bOpen)
    {
        //端口已连接
        //ui->pushButton_open->setEnabled(true);
        //ui->pushButton_open->setText(tr("Disconnect"));

        EnableCtrls(true);
    }
    else
    {
        //端口已关闭
        //ui->pushButton_open->setEnabled(true);
        //ui->pushButton_open->setText(tr("Connect To Robot"));

        EnableCtrls(false);
    }
}

/************************************
* 名称: OnSyncingAction
* 功能: 响应添加动作到同步列表，主要提供给外部模块同步动作（如下载列表模块）
* 参数: [in]vecActtion 动作文件路径
* 返回:   void
* 时间:   2016/04/08
* 作者:   hels
************************************/
void LocalActSyncWidget::OnSyncingAction(QVector<ActItemData> &vecActtion)
{
    int nRow = ui->tableWidget_localact->rowCount();
    foreach (ActItemData itemData, vecActtion)
    {
       /////////////////////////// action.hts file
        ACTFILEDATA* pNewData = new ACTFILEDATA;
        pNewData->m_bLocalFile = false;

        pNewData->m_sGuid = itemData.m_sGuid;

        QString sHtsDir = itemData.m_sLocalPath.left(itemData.m_sLocalPath.lastIndexOf(QDir::separator()));
        const QString sHtsShortName = itemData.m_sActName + HTS_FILE_EXT;// maybe file name not the same to action name, such as "#春晚.hts"
        QString sHtsFilePath;

        QDir dir(sHtsDir);
        dir.setFilter(QDir::Files);
        dir.setSorting(QDir::Name);
        QFileInfoList filelist = dir.entryInfoList();

        foreach(QFileInfo fileinfo, filelist)
        {
            QString sExt = '.' + fileinfo.suffix();
            if(sExt.compare(HTS_FILE_EXT, Qt::CaseInsensitive) == 0)
            {
                sHtsFilePath = QDir::toNativeSeparators(fileinfo.filePath());
//                sHtsShortName = fileinfo.fileName();// maybe file name not the same to action name, such as "#春晚.hts"
                break;
            }
        }

        if(sHtsFilePath.isEmpty())
        {
            CMessageBox::warning(this, "", CFileDirHandle::getFileName(sHtsShortName) + tr("does not exist"));
            continue;
        }

        bool bExist = false;

        QString sInputDir =  itemData.m_sLocalPath.left(itemData.m_sLocalPath.lastIndexOf(QDir::separator()));
        for(int i = 0; i < ui->tableWidget_localact->rowCount(); i++)
        {
            QTableWidgetItem* pItemId = ui->tableWidget_localact->item(i, TABLE_COL_ID);
            int nID = pItemId->text().toInt();
            ACTFILEDATA* pItemData = (ACTFILEDATA*)ui->tableWidget_localact->userData(nID);
            if(pItemData)
            {
//                QString sTempDir = pItemData->m_sFilePath.left(pItemData->m_sFilePath.lastIndexOf(QDir::separator()));
//                if(sTempDir == sInputDir)
//                {
//                    bExist = true;
//                    break;
//                }

                const QString sTempPath = pItemData->m_sFilePath;
                if(sTempPath == sHtsFilePath)
                {
                    bExist = true;
                    break;
                }
            }
        }

        if(bExist)
            continue;

        ui->tableWidget_localact->insertRow(nRow);

        pNewData->m_sFilePath = sHtsFilePath;
        pNewData->m_sFileName = sHtsShortName;

        QFileInfo fileinfo(sHtsFilePath);

        pNewData->m_nFileSize = fileinfo.size();

        QDateTime timetemp = fileinfo.lastModified();
        QString strTemp = timetemp.toString("yyyy-MM-dd hh:mm:ss");
        pNewData->m_sFileModTime = strTemp;

        int nID = GetNewID();
        ui->tableWidget_localact->setUserData(nID, pNewData);

#if 0
        //复选框
        QCheckBox* pChkBox = new QCheckBox;
        ui->tableWidget_localact->setCellWidget(nRow, TABLE_COL_CHECKBOX, pChkBox);
        connect(pChkBox, SIGNAL(clicked(bool)), this, SLOT(onTableWidgetCheckBoxClicked(bool)));

        //ID
        strTemp.sprintf("%d", nID);
        QTableWidgetItem* pItem = new QTableWidgetItem(strTemp);
        pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_localact->setItem(nRow, TABLE_COL_ID, pItem);

        //文件名称
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // 由于用户上传动作后，可能存在动作名称同名问题，后台为了防止
        // 出现动作同名的情况，统一根据上传时间按照数字来命名，导致在
        // 下载列表显示的名称和同步列表显示的完全对应不上的问题，因此
        // 在同步动作列表中由显示hts文件名调整为显示动作名称，以保持两
        // 者一致性（但是用户下载到本地和同步到机器人的动作，在查看时
        // 仍然会出现无法对应上的情况）
        // 问题：从PC端同步的动作，在手机端看到的是一串数字;
//        pItem = new QTableWidgetItem(pNewData->m_sFileName);
        pItem = new QTableWidgetItem(itemData.m_sActName);
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_localact->setItem(nRow, TABLE_COL_NAME, pItem);

        //文件大小
        strTemp = GetFormatSizeString(fileinfo.size());
        pItem = new QTableWidgetItem(strTemp);
        pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_localact->setItem(nRow, TABLE_COL_SIZE, pItem);

        //修改日期
        pItem = new QTableWidgetItem(pNewData->m_sFileModTime);
        pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_localact->setItem(nRow, TABLE_COL_DATE, pItem);

        //状态
        strTemp = tr("Unsynchronized");
        pItem = new QTableWidgetItem(strTemp);
        pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_localact->setItem(nRow, TABLE_COL_STATUS, pItem);
#endif
        //复选框
        //ID

        //文件名称
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // 由于用户上传动作后，可能存在动作名称同名问题，后台为了防止
        // 出现动作同名的情况，统一根据上传时间按照数字来命名，导致在
        // 下载列表显示的名称和同步列表显示的完全对应不上的问题，因此
        // 在同步动作列表中由显示hts文件名调整为显示动作名称，以保持两
        // 者一致性（但是用户下载到本地和同步到机器人的动作，在查看时
        // 仍然会出现无法对应上的情况）
        // 问题：从PC端同步的动作，在手机端看到的是一串数字;
////        pItem = new QTableWidgetItem(pNewData->m_sFileName);
//        pItem = new QTableWidgetItem(itemData.m_sActName);
        QString strFileName = pNewData->m_sFileName; //
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        //文件大小
        qint64 nFileSize = pNewData->m_nFileSize;
        //修改日期
        QString fileModifyTime = pNewData->m_sFileModTime;

        //状态
        addToTableWidget(nRow, nID, strFileName, nFileSize, fileModifyTime);

        ui->tableWidget_localact->setRowHeight(nRow, 52);
        emit SigAddedAction(sHtsFilePath);

/////////////////////////////////////////////////////////////////////////
/////////////////////////////for mp3
        bool bNeedLoadMP3 = false;
        if(eUbtProduct1E == UBXBase::getInstance()->getCurrentRobotProductType())
        {
            bNeedLoadMP3 = true;
        }
        if(bNeedLoadMP3)
        {
            pNewData = new ACTFILEDATA;
            pNewData->m_bLocalFile = false;

            pNewData->m_sGuid = itemData.m_sGuid + MUSIC_FILE_EXT;//hts guid is the same to mp3 guid so not get the right file info to download

            QString sMP3FilePath =  CFileDirHandle::generateFileNameBySuffix( sHtsFilePath, MUSIC_FILE_EXT);
            QFileInfo fileInfoMP3(sMP3FilePath);
//            QString sMP3ShortName = fileInfoMP3.fileName();
            const QString sMP3ShortName = CFileDirHandle::generateFileNameBySuffix(sHtsShortName, MUSIC_FILE_EXT);//// maybe file name not the same to action name, such as "#春晚.hts"

            if(QFile::exists(sMP3FilePath))
            {
                bool bExist = false;

                for(int i = 0; i < ui->tableWidget_localact->rowCount(); i++)
                {
                    QTableWidgetItem* pItemId = ui->tableWidget_localact->item(i, TABLE_COL_ID);
                    int nID = pItemId->text().toInt();
                    ACTFILEDATA* pItemData = (ACTFILEDATA*)ui->tableWidget_localact->userData(nID);
                    if(pItemData)
                    {
                        const QString sTempPath = pItemData->m_sFilePath;
                        if(sTempPath == sMP3FilePath)
                        {
                            bExist = true;
                            break;
                        }
                    }
                }

                if(bExist)
                    continue;

                ui->tableWidget_localact->insertRow(nRow);

                pNewData->m_sFilePath = sMP3FilePath;
                pNewData->m_sFileName = sMP3ShortName;

                pNewData->m_nFileSize = fileInfoMP3.size();

                QDateTime timetemp = fileInfoMP3.lastModified();
                QString strTemp = timetemp.toString("yyyy-MM-dd hh:mm:ss");
                pNewData->m_sFileModTime = strTemp;

                int nID = GetNewID();
                ui->tableWidget_localact->setUserData(nID, pNewData);


                addToTableWidget(nRow, nID, pNewData->m_sFileName, pNewData->m_nFileSize, pNewData->m_sFileModTime);
                ui->tableWidget_localact->setRowHeight(nRow, 52);
            }

        }

    }

    if(!ui->widgetBtnContainer->isVisible() && ui->tableWidget_localact->rowCount() > 0)
    {
        ui->pbAddLocalAct->hide();
        ui->widgetBtnContainer->show();
        ui->pushButton_downloadact->show();
    }
}

/************************************
* 名称: slotOnReceiveRobotTFCardStatus
* 功能: 机器人没有插入TF卡的响应函数，同步动作按钮设置为不可用状态
* 参数: bHasTFCard: 是否存在TF卡标识
* 返回:   void
* 时间:   2016/10/25
* 作者:   hwx
************************************/
void LocalActSyncWidget::slotOnReceiveRobotTFCardStatus(bool bHasTFCard)
{
    EnableCtrls(bHasTFCard);
}

/************************************
* 名称: slotOnIsSystemEmbedVersionNeedCopyUpdate
* 功能: 嵌入式系统是否需要切换到U盘模式拷贝升级的槽函数
* 参数: bMustUpdate: 是否需要切U盘拷贝升级
* 返回:   void
* 时间:   2016/11/09
* 作者:   hwx
************************************/
void LocalActSyncWidget::slotOnIsSystemEmbedVersionNeedCopyUpdate(bool bMustUpdate)
{
    m_bIsChangedUdiskQuiet = bMustUpdate;
    if (m_bIsChangedUdiskQuiet && ui->pushButton_udiskmode->isEnabled()) // 如果没有插入内存卡，不支持转U盘
    {
          on_pushButton_udiskmode_clicked();
    }
}

void LocalActSyncWidget::on_pbAddLocalAct_clicked()
{
    on_pushButton_addact_clicked();
    if(ui->tableWidget_localact->rowCount() > 0)
    {
        ui->pbAddLocalAct->hide();
        ui->widgetBtnContainer->show();
        ui->pushButton_downloadact->show();
    }
}

void LocalActSyncWidget::addToTableWidget(int nRow, int nID, QString fileName, qint64 nFileSize, QString fileModifyTime)
{
    //复选框
    QCheckBox* pChkBox = new QCheckBox;
    ui->tableWidget_localact->setCellWidget(nRow, TABLE_COL_CHECKBOX, pChkBox);
    connect(pChkBox, SIGNAL(clicked(bool)), this, SLOT(onTableWidgetCheckBoxClicked(bool)));

    //ID
    QString strTemp;
    strTemp.sprintf("%d", nID);
    QTableWidgetItem* pItem = new QTableWidgetItem(strTemp);
    pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget_localact->setItem(nRow, TABLE_COL_ID, pItem);

    //文件名称
    pItem = new QTableWidgetItem(fileName);
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget_localact->setItem(nRow, TABLE_COL_NAME, pItem);

    //文件大小
    strTemp = GetFormatSizeString(nFileSize);
    pItem = new QTableWidgetItem(strTemp);
    pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget_localact->setItem(nRow, TABLE_COL_SIZE, pItem);

    //修改日期
    pItem = new QTableWidgetItem(fileModifyTime);
    pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget_localact->setItem(nRow, TABLE_COL_DATE, pItem);

    //状态
    strTemp = tr("Unsynchronized");
    CTableWidgetItem* pItemCTable = new CTableWidgetItem("");
    pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget_localact->setItem(nRow, TABLE_COL_STATUS, pItemCTable);
    ui->tableWidget_localact->addRetranslateColumn(TABLE_COL_STATUS);
    pItemCTable->setRetranslateContext("LocalActSyncWidget");
    pItemCTable->setTextRetranslate("Unsynchronized", strTemp);
}

void LocalActSyncWidget::resizeEvent(QResizeEvent *event)
{
    if(NULL != ui->tableWidget_localact)
    {        
        int width = this->size().width() - 20;

        ui->tableWidget_localact->setColumnWidth(TABLE_COL_CHECKBOX, width * 0.04); //checkbox 4%
        //ui->tableWidget_localact->setColumnWidth(nCol++, width * 0.05);  //ID列
        //ui->tableWidget_localact->hideColumn(m_nIdCol); //隐藏ID列
        ui->tableWidget_localact->setColumnWidth(TABLE_COL_NAME, width * 0.40); //文件名
        ui->tableWidget_localact->setColumnWidth(TABLE_COL_SIZE, width* 0.20); //文件大小
        ui->tableWidget_localact->setColumnWidth(TABLE_COL_DATE, width* 0.20); //修改日期
        ui->tableWidget_localact->setColumnWidth(TABLE_COL_STATUS, width * 0.16);//状态
    }
}

void LocalActSyncWidget::timerEvent(QTimerEvent *event)
{
    int nTimerId = event->timerId();
     if (nTimerId == m_nWaitForUdiskShowUpTimeoutId)
    {
        m_nTryGetUdiskTime--;
        AlphaRobotLog::getInstance()->printDebug(QString("Current try time: %1").arg(m_nTryGetUdiskTime));
        QString strUdiskRootPath = CUBXUdiskManager::getUdiskRootPath();
        if (strUdiskRootPath.isEmpty())
        {
            if (m_nTryGetUdiskTime < 0)
            {
                killTimer(m_nWaitForUdiskShowUpTimeoutId);
                AlphaRobotLog::getInstance()->printError("Get null udisk root path when time out");
            }
            return;
        }

        killTimer(m_nWaitForUdiskShowUpTimeoutId);

        AlphaRobotLog::getInstance()->printDebug("Get open udisk: " + strUdiskRootPath);
        openDirectory(strUdiskRootPath);
    }
}
