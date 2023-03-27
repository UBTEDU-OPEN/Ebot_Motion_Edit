/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：UIMainLayerLayout.cpp
* 创建时间：2015/12/09 15:27
* 文件标识：
* 文件摘要：动作编辑模块界面设计类实现
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2015/12/09 15:27
* 版本摘要：
*/

#include <QMenu>
#include <QTimer>
#include <QMouseEvent>
#include <QClipboard>
#include <QMimeData>
#include <QApplication>
#include <QDir>
#include "../zxLib/widget/messagebox.h"
#include "widget/cubxintvalidator.h"
#include "LayerUI/ILayerUI.h"
#include "LayerUI/IUILayerManager.h"
#include "OptionsMotionWidget.h"

#include "ui_UIOptionMotionWidget.h"
#include "../UBXBase/UBXBase.h"
#include "../Common/cstringhandle.h"





//UIMainLayerLayout

//QListWidgetItem setData Role 定义
enum OptionItemRoles
{
    GroupID = Qt::UserRole + 1,
    ActionBlockDataID
};


//const int  GRID_COL_RUNTIME  = 16; //列表运行时间的列索引（0开始）
//const int  GRID_COL_ALLTIME  = GRID_COL_RUNTIME+1; //列表总时间的列索引

const int GRID_COL_WIDTH = 52;
const int GRID_COL_HEIGHT = 42;
const int GRID_COL_RUNTIME = 100;



#define    KEY_MINEDATA_COPYDATA    "CopyData"


bool CreateMotionLayOptionHandler(ILayerOptionHandler** ppHandler)
{
    COptionsMotionWidget * pProxy = new COptionsMotionWidget();
    if(pProxy)
    {
        *ppHandler = pProxy;
        return true;
    }
    return false;
}

COptionsMotionWidget::COptionsMotionWidget(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::COptionsMotionWidget),
    m_pGridTable(NULL),
    m_nCurrentGroupListSelectItem(-1),
    IsCompensatory(false),
    m_pEvent(NULL),
    m_pGroupListWidget(NULL),
    m_nCurrentGroupID(-1),
    m_pGroupManager(NULL),
    m_pMineData(NULL)
{
    ui->setupUi(this);

    m_nServoCount = getMotorCount();
    m_nRunTimeCol = m_nServoCount;
    m_nAllTimeCol = m_nRunTimeCol + 1;

    m_bLbtnDownDebug = false;
    m_pRobotView = NULL;
    ui->leBeatValue->setEnabled(false);
    m_nCol = 0;
    m_pTimerClicked = NULL;
    m_eTotalPlayStatus = ePlayStatusNone;
    m_bIsMotionSimulating = false;
    m_nTotalTime = 0;
    m_strTotalTime.clear();

    m_strFrameRunTime = QString("%1").arg(GRID_MIN_TIMEVALUE);

    connect(this, &COptionsMotionWidget::SigMotionPlayStop, this, &COptionsMotionWidget::OnMotionPlayStop);
    connect(this, &COptionsMotionWidget::SigSetControlState, this, &COptionsMotionWidget::OnSetControlState);

//    QRegExp regx("[1-9]{1}[0-9]{1,5}");         
//    QValidator *validator = new QRegExpValidator(regx, ui->leBeatValue);
    CUBXIntValidator *validator = new CUBXIntValidator(GRID_MIN_TIMEVALUE, GRID_MAX_TIMEVALUE, m_strFrameRunTime, ui->leBeatValue);
    ui->leBeatValue->setValidator(validator);
    connect(ui->leBeatValue, &QLineEdit::editingFinished, this, &COptionsMotionWidget::slotOnEditFrameRunTimeFinished);

    ui->m_ListActionGroup->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->m_ListActionGroup->setFocusPolicy(Qt::NoFocus);  // 去掉选中时的虚线框
    ui->btnPause->hide();
    connect(ui->btnPause, &QPushButton::clicked, this, &COptionsMotionWidget::on_btnUnitedSimulation_clicked);
    connect(ui->m_ListActionGroup, &QListWidget::itemPressed, this, &COptionsMotionWidget::slotOnGroupItemPressed, Qt::QueuedConnection);

    // 去掉QTableWidgetItem选中时的选线框
    ui->twActionList->setFrameShape(QFrame::NoFrame);//设置边框
//    ui->twActionList->setFocusPolicy(Qt::NoFocus);//must have to remove, or can not update the editting data

    ui->btnUnitedSimulation->setEnabled(false);
    ui->btnStop->setEnabled(false);
    ui->pbInsert->setEnabled(false);
    ui->pbInsertSplit->setEnabled(false);
    ui->pbModify->setEnabled(false);

    m_pMineData = new QMimeData;

    subscribeMsg();


    m_EditItemDefaultColor = Qt::white;
    m_EditItemAutoEditedColor = Qt::blue;
    m_EditItemInvalidColor = Qt::red;
    m_bAutoModifyItemData = false;
    connect(UBXBase::getInstance(), &UBXBase::sigAngleValueCheckWay, this, &COptionsMotionWidget::slotOnAngleValueCheckWay);

    m_ActItemCopyData.initData();
}

void COptionsMotionWidget::subscribeMsg()
{
    SetMsgType Msg;
    Msg.clear();
    Msg.insert(WM_MSG_EXECUTE_REDO);
    Msg.insert(WM_MSG_EXECUTE_UNDO);
    Msg.insert(NOTIFY_MSG_PLAY_MUSIC);
    Msg.insert(NOTIFY_MSG_PLAY_MOTION);
    Msg.insert(NOTIFY_MSG_UNITED_PLAY);
    Msg.insert(NOTIFY_MSG_STOP_PLAY);
    this->AddSubscribe(Msg);
    this->EnableRecvNotify(TRUE);
}

void COptionsMotionWidget::retranslateUi()
{
    ui->retranslateUi(this);

    //设置表格行标题
    if(m_pGridTable)
    {
        QStringList headerLabels;
        for(int i = 0; i < m_nServoCount; i++)
        {
            QString strTitle = "ID"+QString::number(i+1);
            headerLabels << strTitle;
        }
        headerLabels<<tr("runTime(ms)")<<tr("TotalTime(ms)");
        m_pGridTable->setHorizontalHeaderLabels(headerLabels);
    }
}

/**************************************************************************
 * 函数名: isMotionSimulating
 * 功能: 判断是否正在进行动作仿真或联合仿真
 * 参数:
 * 返回值: void
 * 时间: 2017/03/17
 * 作者: hwx
 */
bool COptionsMotionWidget::isMotionSimulating()
{
#if 0  // 重构改动较大，暂时不动
    if (!m_pEvent)
    {
        return false;
    }

    IUILayerManager *pLayerMgr = m_pEvent->ILayerOptionHandler_GetLayerManager();
    if (!pLayerMgr)
    {
        return false;
    }

    return pLayerMgr->IUILayerManager_IsSimulating();
#endif
    return m_bIsMotionSimulating;
}

bool COptionsMotionWidget::isCopyFrame()
{
    if (!m_pMineData)
    {
        return false;
    }

    QByteArray bArray = m_pMineData->data(KEY_MINEDATA_COPYDATA);
    int nTotalLen = bArray.length();
    if(nTotalLen <= 0)
    {
        return false;
    }

    return true;
}

/**************************************************************************
 * 函数名: doProcessSimulateMsg
 * 功能: 处理音乐层音乐、动作播放消息，主要用于播放时禁用当前界面按钮
 * 参数:
 *    @[in ] nMsgId: 消息ID
 *    @[in ] eStatus: 播放状态
 * 返回值: void
 * 时间: 2017/03/15
 * 作者: hwx
 */
void COptionsMotionWidget::doProcessSimulateMsg(int nMsgId, emPlayStatus eStatus)
{
    switch (eStatus) {
    case ePlayStatusPlaying:
    {
#if 1
        if (nMsgId == NOTIFY_MSG_PLAY_MUSIC || nMsgId == NOTIFY_MSG_PLAY_MOTION)
        {
            setSimulateButtonState(false);
        }
#endif
        //  音乐仿真时允许动作编辑
        if (nMsgId != NOTIFY_MSG_PLAY_MUSIC)
        {
            setMotionEditButtonState(false);
            m_bIsMotionSimulating = true;
        }
        break;
    }
    case ePlayStatusPause:
    case ePlayStatusStop:
    case ePlayStatusFinished:
    {
#if 1
        if (nMsgId == NOTIFY_MSG_PLAY_MUSIC || nMsgId == NOTIFY_MSG_PLAY_MOTION)
        {
            setSimulateButtonState(true);
        }
#endif
        setMotionEditButtonState(true);
        m_bIsMotionSimulating = false;
        break;
    }
    default:
        break;
    }
}

void COptionsMotionWidget::doProcessStopPlayMsg(int nMsgID, emPlayType eType)
{
    if (eType == ePlayTypeUnited)
    {
        on_btnStop_clicked();
    }
}

void COptionsMotionWidget::setSimulateButtonState(bool bEnable)
{
    ui->btnPause->setEnabled(bEnable);
    ui->btnStop->setEnabled(bEnable);
    ui->btnUnitedSimulation->setEnabled(bEnable);
}

void COptionsMotionWidget::setMotionEditButtonState(bool bEnalbe)
{
    ui->pbAdd->setEnabled(bEnalbe);
    ui->pbInsert->setEnabled(bEnalbe);
    ui->pbInsertSplit->setEnabled(bEnalbe);
    ui->pbModify->setEnabled(bEnalbe);
}

/**************************************************************************
* 函数名: Connect
* 功能:链接窗口
* 参数:
* 返回值:
* 时间:2015/12/25 16:36
* 作者: 周志平
* 改动日志
**************************************************************************/
bool COptionsMotionWidget::Connect(QWidget *pParent, ILayerOptionHandler_Event *pEvent)
{
    setParent(pParent);
    QLayout* layout = pParent->layout();
    layout->addWidget(this);
    initWidget();
    m_pEvent = pEvent;
    return true;
}

/**************************************************************************
* 函数名: initWidget
* 功能:初始化窗口
* 参数:
* 返回值:
* 时间:2015/12/25 16:36
* 作者: 周志平
* 改动日志
**************************************************************************/
void COptionsMotionWidget::initWidget()
{
    ui->leBeatValue->setText(QString::number(200));
    m_pGroupListWidget = ui->m_ListActionGroup;
    m_pGridTable = ui->twActionList;
    initTableWidget();

    connect(m_pGridTable->verticalHeader(), &QHeaderView::sectionClicked,
            this, &COptionsMotionWidget::onSectionClicked);
    connect(m_pGridTable->verticalHeader(), &QHeaderView::sectionDoubleClicked,
            this, &COptionsMotionWidget::onSectionDoubleClicked);
    connect(m_pGridTable->itemDelegate(), &QAbstractItemDelegate::commitData,
            this, &COptionsMotionWidget::onItemCommitData);

    connect(m_pGridTable, &QTableWidget::itemClicked, this, &COptionsMotionWidget::onItemClicked);

    m_pGridTable->verticalHeader()->installEventFilter(this);
}
/**************************************************************************
* 函数名: onMotionFrameCopy
* 功能:动作帧复制
* 参数:
* 返回值:
* 时间: 2016/01/23  14:57
* 作者: 周志平
* 改动日志   2016/01/23  14:57 使用QT剪贴板进行复制
**************************************************************************/
void COptionsMotionWidget::onMotionFrameCopy()
{
    if (m_nCurrentGroupID == -1)
        return;
    if (m_pGroupManager == NULL)
        return;
    if(m_pGridTable == NULL)
    {
        return;
    }

    CActionBlockGroupManager* pManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if (pManager == NULL)
        return;

    QList<QTableWidgetSelectionRange>ranges= m_pGridTable->selectedRanges();

    bool bIsFullRowSelected = isFullRowSelect(ranges);

    if(bIsFullRowSelected)
    {
        m_ActItemCopyData.initData();

        int nSelectRowCount = 0;
        const int nRangeCount=ranges.count();
        for(int ir = 0;ir < nRangeCount;ir++)
        {
            nSelectRowCount += ranges.at(ir).rowCount();
        }

        int nSelectRowCountTemp = 0;
        for (int i=0; i < nRangeCount; i++)
        {
            int nTopRow = ranges.at(i).topRow();
            int nBottomRow = ranges.at(i).bottomRow();
            for(int j = nTopRow ;j <= nBottomRow;j++)
            {
                bool bIsFullRowSelect = IsFullRowSelect(j);
                if(bIsFullRowSelect)
                {
                    nSelectRowCountTemp++;
                }
            }
        }

        vector<char> RawData;

        // 保存选择的行数
        AddVectorData(&RawData, (char*)&nSelectRowCountTemp, sizeof(nSelectRowCountTemp));

        QTableWidgetItem* pItem = NULL;

        for (int i=0; i < nRangeCount; i++)
        {
            int nTopRow = ranges.at(i).topRow();
            int nBottomRow = ranges.at(i).bottomRow();
            for(int j = nTopRow ;j <= nBottomRow;j++)
            {
                bool bIsFullRowSelect = IsFullRowSelect(j);
                if(bIsFullRowSelect)
                {

                    pItem = m_pGridTable->item(j,0);
                    int nBlockDataID = pItem->data(OptionItemRoles::ActionBlockDataID).toInt();
                    CActionBlockData* pBlockData = pManager->FindBlockData(nBlockDataID);
                    if (pBlockData == NULL)
                        continue;

                    char* pData = NULL;
                    int nLen = 0;
                    pBlockData->GetData(&pData, nLen);
                    if (pData)
                    {
                        // 先保存长度
                        AddVectorData(&RawData, (char*)&nLen, sizeof(nLen));
                        // 保存数据
                        AddVectorData(&RawData, pData, nLen);

                        delete [] pData;
                    }
                }

            }

        }

        int nTotalLen = RawData.size();
        char* pTotalData = new char[nTotalLen];
        CopyVertorToPointer(RawData, pTotalData);

    //    QClipboard *clipboard = QApplication::clipboard();
    //    clipboard->clear();
    //    QByteArray bArray(pTotalData,nTotalLen);
    //    QMimeData *mimeData = new QMimeData;
    //    mimeData->setData(KEY_MINEDATA_COPYDATA, bArray);
    //    clipboard->setMimeData(mimeData, QClipboard::Clipboard);
    //    if(mimeData)
    //    {
    //        delete mimeData;
    //        mimeData = NULL;
    //    }
    //    clipboard->setMimeData(m_pMineData, QClipboard::Clipboard);

        QByteArray bArray(pTotalData,nTotalLen);
        if(m_pMineData)
        {
            m_pMineData->setData(KEY_MINEDATA_COPYDATA, bArray);
        }


        if (pTotalData && nTotalLen)
        {
            delete [] pTotalData;
        }

        return;
    }


    bool bContinueRange = isContinueRange(ranges);
    if(bContinueRange && !bIsFullRowSelected)
    {
        int nTemp = 0;
        QByteArray bArray((char*)&nTemp, sizeof(nTemp));
        if(m_pMineData)
        {
            m_pMineData->setData(KEY_MINEDATA_COPYDATA, bArray);
        }

        getRangeData(m_ActItemCopyData, ranges, false);
    }
}
/**************************************************************************
* 函数名:onMotionFramePasteToEnd
* 功能:动作帧粘贴
* 参数:
* 返回值:
* 时间: 2016/01/23  14:57
* 作者: 周志平
* 改动日志   * 改动日志  2016/01/23  14:57 使用QT剪贴板进行粘贴
**************************************************************************/
void COptionsMotionWidget::onMotionFramePasteToEnd()
{

//    const QClipboard *clipboard = QApplication::clipboard();
//    const  QMimeData* mimeData = clipboard->mimeData();

    const  QMimeData* mimeData = m_pMineData;
    if(mimeData == NULL)
    {
        qDebug()<<tr("can't find mimeData point")<<endl;
        return;
    }
    QByteArray bArray = mimeData->data(KEY_MINEDATA_COPYDATA);
    char * buffer = bArray.data();
    int nTotalLen = bArray.length();
    if(nTotalLen <= 0)
    {
        return;
    }

    // 保存数据，以进行undo操作
    IOperation *pChangedMotionData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
    cloneBeforeChangedMotionData(pChangedMotionData);

    int nTotalRow = 0;
    UBX_MEMCPY_INC(nTotalRow, buffer);

    int nCurrentSelectRow = -1;
    for (int i=0; i<nTotalRow; i++)
    {
        int nLen =0;
        UBX_MEMCPY_INC(nLen, buffer);

        char* pData = new char[nLen];
        memcpy(pData, buffer, nLen);
        buffer += nLen;

        CActionBlockData* pActionBlockData = new CActionBlockData();
        pActionBlockData->SetData(pData, nLen);
        pActionBlockData->SetID(-1);

        if (pData)
        {
            delete [] pData;
        }

        int nCurrentID = GetCurrenGroupManagerID();
        if(i == nTotalLen - 1)
        {
            m_pEvent->ILayerOptionHandler_OnAddActionBlock(nCurrentID, pActionBlockData ,"", true);
        }
        else
        {
            m_pEvent->ILayerOptionHandler_OnAddActionBlock(nCurrentID, pActionBlockData);
        }

        if (nCurrentID != -1)
        {
            int nID = pActionBlockData->GetID();
            SetShowID(&nID, 1);

            char* pExtDate = NULL;
            int nExtLen = 0;
            pActionBlockData->GetExtBlockData(&pExtDate, nExtLen);

            QString strTmpDescription = WCHARToQString(pActionBlockData->GetDescription(), MAX_DESCRIPTION_SIZE*sizeof(WCHAR));

            nCurrentSelectRow = AddToTableWidget(pExtDate, nExtLen, strTmpDescription, pActionBlockData->GetRunTime(), pActionBlockData->GetAllTime(), pActionBlockData->GetID());
            if (pExtDate)
            {
                delete [] pExtDate;
            }
        }
    }
    setSelectRow(nCurrentSelectRow);

    // 保存数据，以进行undo操作
    cloneAfterChangedMotionData(pChangedMotionData);
}

/**************************************************************************
* 函数名:onMotionFramePaste
* 功能:动作帧插入粘贴
* 参数:
* 返回值:
* 时间: 2016/01/23  14:57
* 作者: 周志平
* 改动日志   * 改动日志  2016/01/23  14:57 使用QT剪贴板进行插入
**************************************************************************/
void COptionsMotionWidget::onMotionFramePaste()
{
//    const QClipboard *clipboard = QApplication::clipboard();
//    const  QMimeData* mimeData = clipboard->mimeData();

    const  QMimeData* mimeData = m_pMineData;
    if(mimeData == NULL)
    {
        qDebug()<<tr("can't find mimeData point")<<endl;
        return;
    }
    QByteArray bArray = mimeData->data(KEY_MINEDATA_COPYDATA);
    char * buffer = bArray.data();
    int nTotalLen = bArray.length();
    if(nTotalLen <= 0)
    {
        return;
    }

    if (m_pGroupManager == NULL ||
            m_pGridTable == NULL)
        return;

    CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if(pGroupManager == NULL)
        return;

    // 保存历史数据
    IOperation *pChangeMotionData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
    cloneBeforeChangedMotionData(pChangeMotionData);

    const QList<QTableWidgetSelectionRange> ranges= m_pGridTable->selectedRanges();
    const bool bIsFullRowSelected = isFullRowSelect(ranges);
    if(bIsFullRowSelected)
    {
        int nSelectBlockID = getLastSelectRowID();
        int nTotalRow = 0;
        UBX_MEMCPY_INC(nTotalRow, buffer);

        for (int i = 0; i < nTotalRow; i++)
        {
            int nLen = 0;
            UBX_MEMCPY_INC(nLen, buffer);
            char* pData = new char[nLen];
            memcpy(pData, buffer, nLen);
            buffer += nLen;
            CActionBlockData* pActionBlockData = new CActionBlockData();
            pActionBlockData->SetData(pData, nLen);
            pActionBlockData->SetID(-1);
            if(pData)
            {
                delete [] pData;
            }

            // pGroupManager->InsertBeforeId(nSelectBlockID, pActionBlockData);
            pGroupManager->InsertAfterId(nSelectBlockID, pActionBlockData);

            int nCurrentShowID = pActionBlockData->GetID();
            SetShowID(&nCurrentShowID, 1);

            nSelectBlockID = nCurrentShowID;//for insertafter

            int nCurrentID = m_nCurrentGroupID;
            m_nCurrentGroupID = -1;
            if(i == nTotalRow - 1)
            {
                g_IsUpdateUIByData = true;
                m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, pActionBlockData->GetAllTime(), true);
                g_IsUpdateUIByData = false;
            }
            else
            {
                g_IsUpdateUIByData = true;
                m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, pActionBlockData->GetAllTime(), false);
                g_IsUpdateUIByData = false;
            }
        }
    }

    else if (!bIsFullRowSelected)
    {
        ActItemCopyData actCopyData;
        bool bGetRange = getRangeData(actCopyData, ranges, true);
        if(bGetRange
                && (actCopyData.nRow == m_ActItemCopyData.nRow)
                && (actCopyData.nColumn == m_ActItemCopyData.nColumn)
                && (m_ActItemCopyData.listItem.size() > 0))
        {
            const int nRowTop = ranges.at(0).topRow();
            const int nColumnLeft = ranges.at(0).leftColumn();

            for(int i = 0 ; i < m_ActItemCopyData.listItem.size(); i++)
            {
                const VectorInt& vecInt = m_ActItemCopyData.listItem.at(i);
                for(int j = 0; j < vecInt.size(); j++)
                {
                    int nTemp = vecInt.at(j);
                    int nRowTemp = nRowTop + i;
                    int nColumnTemp = nColumnLeft + j;
                    QTableWidgetItem *pItem = m_pGridTable->item(nRowTemp , nColumnTemp);
                    if(pItem)
                    {
                        pItem->setText(QString::number(nTemp));
                        modifyItemData(nRowTemp, nColumnTemp, false);
                    }
                }
            }
        }
    }

    UpdateUIByData();

    // 保存数据，以进行redo操作
    cloneAfterChangedMotionData(pChangeMotionData);
}

//int  COptionsMotionWidget::getSelectRowCount()
//{

//    int nSelectCount = 0;
//    for (int i=0; i<m_pGridTable->rowCount(); i++)
//    {
//        if (IsFullRowSelect(i))
//            nSelectCount++;
//    }

//    return nSelectCount;

//}
/**************************************************************************
* 函数名:getFirstSelectRowID
* 功能:获取选中的第一行
* 参数:
* 返回值:
* 时间:2016/01/23  14:57
* 作者: 周志平
* 改动日志 2016/03/17改为不需要选中一整行才可以插入
**************************************************************************/
int COptionsMotionWidget::getFirstSelectRowID()
{
    int nSelectID = -1;
    QList<QTableWidgetSelectionRange> ranges = m_pGridTable->selectedRanges();
    if (ranges.size() <= 0)
    {
        return nSelectID;
    }
    int nRow = ranges.first().topRow();
    QTableWidgetItem* pItem = m_pGridTable->item(nRow, 0);
    nSelectID= pItem->data(OptionItemRoles::ActionBlockDataID).toInt();
    return nSelectID;
}

int COptionsMotionWidget::getLastSelectRowID()
{
    int nSelectID = -1;
    QList<QTableWidgetSelectionRange> ranges = m_pGridTable->selectedRanges();
    if (ranges.size() <= 0)
    {
        return nSelectID;
    }
    int nRow = ranges.last().bottomRow();
    QTableWidgetItem* pItem = m_pGridTable->item(nRow, 0);
    nSelectID= pItem->data(OptionItemRoles::ActionBlockDataID).toInt();
    return nSelectID;
}

MOTOR_DATA *COptionsMotionWidget::GetMotorData(char *pData, int nLen, int nID)
{
    MOTOR_DATA* p = (MOTOR_DATA*)(pData + 4);
    return &p[nID];
}

void COptionsMotionWidget::SetMotorAngle(char *pData, int nLen, int nID, int nAngle)
{
    //int nCount = (nLen-4)/sizeof(MOTOR_DATA);
    MOTOR_DATA* p = (MOTOR_DATA*)(pData+4);
    p[nID].nAngle = nAngle;
}

void COptionsMotionWidget::ModleView(int nActionBlockID)
{
    if(NULL == m_pGroupManager ||
       NULL == m_pRobotView)
        return;
    CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if(NULL == pGroupManager)
        return;

    CActionBlockData* pActionBlockData = pGroupManager->FindBlockData(nActionBlockID);
    if(pActionBlockData)
    {
        char* pData = NULL;
        int nLen = 0;
        pActionBlockData->GetExtBlockData(&pData, nLen);
        if(pData && nLen)
        {
            m_pRobotView->SetMotorDatasManual(pData, nLen, 0);
            delete [] pData;
        }
    }
}

void COptionsMotionWidget::PlayBlock(int nActionBlockID)
{
    if(NULL == m_pGroupManager ||
       NULL == m_pEvent)
        return;

    CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if(NULL == pGroupManager)
        return;

    CActionBlockData* pActionBlockData = pGroupManager->FindBlockData(nActionBlockID);
    if(pActionBlockData)
    {
        char* pData = NULL;
        int nLen = 0;
        pActionBlockData->GetExtBlockData(&pData, nLen);
        if(pData && nLen)
        {
            if(m_pEvent)
                m_pEvent->ILayerOptionHandler_OnDataBlockPlaying(pActionBlockData);
            delete [] pData;
        }
    }
}

/**************************************************************************
* 函数名:ReleaseConnection
* 功能:  解除连接
* 参数:
* 返回值: void
* 时间:  2015/12/21 10:44
* 作者: 周志平
* 改动日志:2015/12/26 16:06  模拟解除链接时，所有变量重新初始化
**************************************************************************/
void COptionsMotionWidget::ReleaseConnection()
{
    disconnect();
    m_ShowId.clear();
    if(m_pGridTable != NULL)
    {
        m_pGridTable->clearContents();
        m_pGridTable->setRowCount(0);
        m_pGridTable = NULL;
    }

    IsCompensatory = false;
    m_nCurrentGroupListSelectItem = -1;

    m_nCurrentGroupID = -1;
    m_bLbtnDownDebug = false;
    m_ShowId.clear();

    delete this;
}


COptionsMotionWidget::~COptionsMotionWidget()
{
    delete ui;
    if(m_pGridTable)
    {
        delete m_pGridTable;
        m_pGridTable = NULL;
    }

    SAFE_DELETE(m_pMineData);
}

void COptionsMotionWidget::OnNotify(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    // 停止播放消息有顺序要求，需要在同一线程中
    if (nMsgID == NOTIFY_MSG_STOP_PLAY)
    {
        doProcessStopPlayMsg(nMsgID, (emPlayType)lParam);
        return;
    }

    zxEvent* pEvent = new zxEvent(nMsgID);
    pEvent->eEventType = zxEventTypeWindow;
    pEvent->lReceiver = (long)this;
    pEvent->dwMsgId = nMsgID;
    pEvent->dwContext = dwContext;
    pEvent->wParam = wParam;
    pEvent->lParam = lParam;
    QCoreApplication::postEvent(this, pEvent);
}

/**************************************************************************
* 函数名:eventFilter
* 功能:事件筛选器
* 参数:QObject *obj  对象
* 参数:QEvent *event 事件
* 返回值:bool
* 时间:2015/01/24 20:58
* 作者: 周志平
* 改动日志
**************************************************************************/
bool COptionsMotionWidget::eventFilter(QObject *obj, QEvent *event)
{
    int nType = event->type();
    if (nType == QEvent::ContextMenu)
    {
        CreateGridListPopMenuActions();
    }
    return QWidget::eventFilter(obj, event);
}

void COptionsMotionWidget::customEvent(QEvent *event)
{
    zxEvent* pEvent = (zxEvent*)event;
    DWORD nMsgID = pEvent->dwMsgId;
    DWORD nOperationCode = pEvent->dwContext;
    WPARAM wParam = pEvent->wParam;
    LPARAM lParam = pEvent->lParam;

    if ((nMsgID == WM_MSG_EXECUTE_REDO || nMsgID == WM_MSG_EXECUTE_UNDO) &&
            (nOperationCode == eCodeHandleFrameRunTime))
    {
        updateConfigRuntime((int)lParam);
    }
    else if (nMsgID == NOTIFY_MSG_PLAY_MOTION || nMsgID == NOTIFY_MSG_PLAY_MUSIC || nMsgID == NOTIFY_MSG_UNITED_PLAY)
    {
        doProcessSimulateMsg(nMsgID, (emPlayStatus)lParam);
    }
}

//void COptionsMotionWidget::mousePressEvent(QMouseEvent *event)
//{
//    if(event->button() == Qt::RightButton)
//    {
//        CreateGridListPopMenuActions();
//    }
//}

//bool COptionsMotionWidget::Connect(QWidget *pWnd)
//{

//}

//void COptionsMotionWidget::Disconnect()
//{

//}

QWidget *COptionsMotionWidget::GetWidget()
{
    return this;
}

bool COptionsMotionWidget::GetData(char **ppData, int &nLen)
{
    if(m_pMainLayer)
    {
        return m_pMainLayer->GetData(ppData, nLen);
    }
    return false;

}

bool COptionsMotionWidget::SetData(char *pData, int nLen)
{
    if(m_pMainLayer)
    {
        return m_pMainLayer->SetData(pData, nLen);
    }
    return false;
}

void COptionsMotionWidget::AdjustData()
{
    if(m_pMainLayer)
    {
        m_pMainLayer->AdjustData();
    }
}

void COptionsMotionWidget::UBXMotionEditor_Set3DRobotViewInterface(IUBXRobot3DView *pRobotView)
{
    if(m_pMainLayer)
    {
        m_pMainLayer->AdjustData();
    }
}

void COptionsMotionWidget::UBXMotionEditor_SetSocketSession(CIUBXSocketSession *pSession)
{
    //    if(m_pMainLayer)
    //    {
    //        m_pMainLayer->ActionLayerManager_SetSocketSession(pSession);
    //    }
}

void COptionsMotionWidget::UBXMotionEditor_UpdateScrollBar()
{
    //    if(m_pMainLayer)
    //    {
    //        m_pMainLayer->ActionLayerManager_SetSocketSession();
    //    }
}

void COptionsMotionWidget::UBXMotionEditor_SetMusicResource(QString strFilePath)
{

}

void COptionsMotionWidget::UBXMotionEditor_StopDebug()
{

}

void COptionsMotionWidget::UBXMotionEditor_ReleaseMusicSource()
{

}

void COptionsMotionWidget::UBXMotionEditor_SetTreeView(ILibTreeNavigation *pView)
{

}

void COptionsMotionWidget::UBXMotionEditor_TreeToView(char *pData, int nLen, QPoint *nPoint, QString FileName)
{

}

bool COptionsMotionWidget::UBXMotionEditor_IsProjectModity()
{
    return true;
}

bool COptionsMotionWidget::UBXMotionEditor_IsRobotDebuging()
{
    return true;
}

void COptionsMotionWidget::UBXMotionEditor_SetProjectModity(bool bState)
{

}

/**************************************************************************
* 函数名: SetActionBlockDataManager
* 功能:   设置所有动作组
* 参数:   CActionBlockDataManager* pManager 动作组管理指针
* 参数:   int nCurrentGroupID   当前组ID
* 参数:   int nCurrentActionID  当前动作数组下标ID!!!!!!!!!
* 返回值:
* 时间:   2015/12/25 16:36
* 作者: 周志平
**************************************************************************/
void COptionsMotionWidget::SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionIndex)
{
    //    bool IsUpdata = true;
    //    if (m_nCurrentGroupID == nCurrentGroupID)
    //    {
    //        IsUpdata = false;
    //    }

    m_nCurrentGroupID = nCurrentGroupID;
    m_pGroupManager = pManager;

    if (nCurrentGroupID == -1)
    {
        ui->leBeatValue->setEnabled(false);
        m_nCurrentGroupListSelectItem = -1;
        m_ShowId.clear();
    }

    if (g_IsModifyGroup || g_IsUpdateUIByData)
    {
        UpdateUIByData();
    }

    if(m_pGroupListWidget == NULL)
    {
        return;
    }
    else
    {
        QListWidgetItem * pItem = NULL;
        m_nCurrentGroupListSelectItem = -1;
        for (int i=0; i<m_pGroupListWidget->count(); i++)
        {
            pItem= m_pGroupListWidget->item(i);
            if(pItem == NULL)
            {
                qDebug()<<"can't find pItem point"<<endl;
                break;
            }
            QVariant value = pItem->data(OptionItemRoles::GroupID);
            int nValue = value.toInt();
            if (nValue == nCurrentGroupID)
            {
                m_nCurrentGroupListSelectItem = i;
                break;
            }
        }
    }

    if (g_IsModifyGroup || g_IsUpdateUIByData)
    {
        QListWidgetItem * item =m_pGroupListWidget->item(m_nCurrentGroupListSelectItem);
        SetCurrentSelectItem(item,m_nCurrentGroupListSelectItem, nCurrentActionIndex);
        g_IsUpdateUIByData = false;
        g_IsModifyGroup  = false;
    }

    if (nCurrentActionIndex != -1 && m_nCurrentGroupListSelectItem != -1)
    {
        int nActionID = 0;
        if (pManager)
        {
            CActionBlockData *pData = pManager->GetAt(m_nCurrentGroupListSelectItem)->GetAt(nCurrentActionIndex);
            if (pData)
            {
                nActionID = pData->GetID();
                SetShowID(&nActionID,1);
                setSelectRow(nCurrentActionIndex);
            }
        }
    }
}

void COptionsMotionWidget::Set3DRobotViewInterface(IUBXRobot3DView *pRobotView)
{
    m_pRobotView = pRobotView;
}

void COptionsMotionWidget::SetControlState(bool controlState, int nLayerType)
{
    emit SigSetControlState(controlState, nLayerType);
}

void COptionsMotionWidget::InsertGroupByMenu(int nCol)
{
    if(m_pEvent && m_pRobotView)
    {
        char* pData = NULL;
        int nLen = 0;
        m_pRobotView->GetMotorDatas(&pData, nLen);
        if (pData && nLen)
        {
            QString tempValue = ui->leBeatValue->text();
            float fRunTime = tempValue.toFloat();//运行时间
            int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();
            CActionBlockData* pActionBlockData = new CActionBlockData();
            pActionBlockData->SetID(-1);
            WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
            memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
            QString strDescription = "Action";
            int nDescriptionLen = QStringToWCHAR(wstrDescription, strDescription);
            //wstrDescription[nDescriptionLen-1] ='\0';
            pActionBlockData->SetDescription(wstrDescription);
            pActionBlockData->SetRunTime(fRunTime / nScaleTime);
            pActionBlockData->SetAllTime(fRunTime / nScaleTime);
            pActionBlockData->SetExtBlockData(nLen, pData);

            m_pEvent->ILayerOptionHandler_InsertGroupByMenu(pActionBlockData, nCol);
        }
    }
}

void COptionsMotionWidget::MotionPlayStop(int nPlayStatus)
{
    emit SigMotionPlayStop((emPlayStatus)nPlayStatus);
}

void COptionsMotionWidget::FinishPlayingMusic()
{
    //ui->btnUnitedSimulation->setText(tr("United Simulation"));
    ui->btnPause->hide();
    ui->btnUnitedSimulation->show();
    m_eTotalPlayStatus = ePlayStatusFinished;
    resetPlayTime();

    NotifyObservers(NOTIFY_MSG_UNITED_PLAY, NULL, NULL, ePlayStatusStop);
}

void COptionsMotionWidget::MusicLoaded(const QString& title, int time)
{
    ui->labelMusicName->setText(title);
    m_strTotalTime = QString().sprintf("%02d:%02d:%02d",
                                       (int)(time) / 3600,
                                       ( (int)(time) % 3600 ) / 60,
                                       (int)(time) % 60);
    ui->labelMusicTime->setText(m_strTotalTime);
    m_nTotalTime = time;
    ui->musicSlider->setRange(0, time);
    resetPlayTime();
}

void COptionsMotionWidget::UpdateMusicPosition(quint64 position)
{
    int timeSec = position / 1000;//转为秒
    QString strPosition = QString().sprintf("%02d:%02d:%02d",
                                            (int)(timeSec) / 3600,
                                            ( (int)(timeSec) % 3600 ) / 60,
                                            (int)(timeSec) % 60);
    ui->labelMusicTime->setText(strPosition + '/' + m_strTotalTime);
    ui->musicSlider->setSliderPosition(timeSec);
}

void COptionsMotionWidget::copyMotionFrame()
{
    onMotionFrameCopy();
}

void COptionsMotionWidget::pasteMotionFrame()
{
    onMotionFramePaste();
}

QMimeData * COptionsMotionWidget::getCopyFrameData()
{
    return m_pMineData;
}

void COptionsMotionWidget::deleteMotionFrame()
{
    onMotionFrameDelete();
}

void COptionsMotionWidget::modifyMotionFrame()
{
    on_pbModify_clicked();
}

void COptionsMotionWidget::insertMotionFrame()
{
    on_pbInsert_clicked();
}

void COptionsMotionWidget::splitInsertMotionFrame()
{
    on_pbInsertSplit_clicked();
}

void COptionsMotionWidget::addFrameToPreMotionBlock()
{
    if (!m_pGroupManager)
    {
        return;
    }

    CActionBlockGroupManager *pActionBlockGroup = m_pGroupManager->GetFinalBlockData();
    if (!pActionBlockGroup)
    {
        return;
    }

    // 先选中指定的动作组，再添加动作帧
    // 否则只会在表格中添加新建动作帧，而不是
    // 追加到指定动作组后
    int nActionGroupId = pActionBlockGroup->GetID();
    addMotionFrame(nActionGroupId);

    int nRow =m_pGroupManager->GetIndexByID(nActionGroupId);
    if (nRow < 0)
    {
        return;
    }

//    // hwx 保持音乐层指针位置不变
//    if(m_pEvent)
//    {
//        m_pEvent->ILayerOptionHandler_OnSelectGroupManageChange(nActionGroupId, 0);
//    }

    // 刷新表格
    int nActionFrameCnt = pActionBlockGroup->GetActionCount();
    SetCurrentSelectItem(m_pGroupListWidget->item(nRow), nRow, nActionFrameCnt - 1);
}

void COptionsMotionWidget::createFrameInNewBlock()
{
    addMotionFrame(INVALID_BLOCK_ID);
}

void COptionsMotionWidget::mergeGroup()
{
    onGroupListMerge();
}

bool COptionsMotionWidget::hasCopyFrame()
{
    return isCopyFrame();
}

/**************************************************************************
* 函数名: on_pbInsert_clicked
* 功能:   动作添加按钮响应事件
* 参数:
* 返回值:
* 时间:   2015/12/25 16:36
* 作者: 周志平
**************************************************************************/
void COptionsMotionWidget::on_pbAdd_clicked()
{
    int nSelectBlockId = GetCurrenGroupManagerID();
    addMotionFrame(nSelectBlockId);
}

/**************************************************************************
* 函数名: on_pbInsert_clicked
* 功能:   动作插入按钮响应事件
* 参数:
* 返回值:
* 时间:   2015/12/25 16:36
* 作者: 周志平
**************************************************************************/
void COptionsMotionWidget::on_pbInsert_clicked()
{
    if (m_pGroupManager == NULL ||  m_pGridTable == NULL)
    {
        return;
    }

    CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if(pGroupManager == NULL)
    {
        return;
    }

    int nCurRow = m_pGridTable->currentRow();
    //fix bug 148
    //int nCount = m_pGridTable->rowCount();
    //for (int i = 0; i < nCount; i++)
    //    {
    //        bool bSelectFullLine = true;
    //        bSelectFullLine = IsFullRowSelect(nCurRow);

    //        if (bSelectFullLine)
    //        {
    QTableWidgetItem* pItem = m_pGridTable->item(nCurRow, 0);
    int nSelectBlockID = pItem->data(OptionItemRoles::ActionBlockDataID).toInt();
    if (!m_pEvent || !m_pRobotView)
    {
        return;
    }

    // 插入前保存历史数据，以进行undo操作
    IOperation *pChangeMotionData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
    cloneBeforeChangedMotionData(pChangeMotionData);

    char* pData = NULL;
    int nLen = 0;
    m_pRobotView->GetMotorDatas(&pData, nLen);
    if (pData && nLen)
    {
        float fRunTime = 0;
        if (ui->leBeatValue->isEnabled())
        {
            QString tempValue = ui->leBeatValue->text();
            fRunTime = tempValue.toFloat();
            if (fRunTime < 10)
            {
                fRunTime = 10;
            }
        }
        int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();//时间单位

        CActionBlockData* pActionBlockData = new CActionBlockData();
        pActionBlockData->SetID(pGroupManager->GetUnUserID());

        WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
        memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
        int nDescriptionLen = QStringToWCHAR(wstrDescription, "Action");
        //wstrDescription[nDescriptionLen-1] ='\0';
        pActionBlockData->SetDescription(wstrDescription);

        pActionBlockData->SetRunTime(fRunTime / nScaleTime);
        pActionBlockData->SetAllTime(fRunTime / nScaleTime);
        pActionBlockData->SetExtBlockData(nLen, pData);
        pGroupManager->InsertBeforeId(nSelectBlockID, pActionBlockData);

        int nCurrentShowID = pActionBlockData->GetID();
        SetShowID(&nCurrentShowID, 1);
        int nCurrentID = m_nCurrentGroupID;
        m_nCurrentGroupID = -1;
        g_IsUpdateUIByData = true;
        m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, pActionBlockData->GetAllTime(), true);
        g_IsUpdateUIByData = false;
        delete[] pData;
    }

    // 插入数据后保存数据，以便进行redo操作
    cloneAfterChangedMotionData(pChangeMotionData);

    setSelectRow(nCurRow);
    //        }
    //        else
    //        {
    //            CMessageBox::warning(NULL,"",tr("No Rows Selected!"));
    //        }
    //    }
}

/**************************************************************************
* 函数名: GetCurrenGroupManagerID
* 功能:   获取当前的动作组ID
* 参数:
* 返回值:
* 时间:   2015/12/25 16:36
* 作者: 周志平
**************************************************************************/
int COptionsMotionWidget::GetCurrenGroupManagerID()
{

    //        return 1;   //模拟，暂时设置为Group1

    if (m_nCurrentGroupListSelectItem == -1)
        return -1;

    if (m_pGroupManager == NULL)
        return -1;

    CActionBlockGroupManager* pGroupManager = m_pGroupManager->GetAt(m_nCurrentGroupListSelectItem);
    if (pGroupManager)
    {
        return pGroupManager->GetID();
    }

    return -1;
}
/**************************************************************************
* 函数名: SetShowID
* 功能:   保存需要显示的ID
* 参数:
* 返回值:
* 时间:   2015/12/25 16:36
* 作者: 周志平
**************************************************************************/
void COptionsMotionWidget::SetShowID(int *pID, int nCount)
{
    m_ShowId.clear();

    for(int i=0; i<nCount; i++)
    {
        m_ShowId.push_back(pID[i]);
    }
}

/**************************************************************************
* 函数名: AddToTableWidget
* 功能: 添加数据到列表
* 参数:
*    @[in ] pData: 数据缓冲区
*    @[in ] nLen: 数据长度
*    @[in ] BlockType: 动作类型
*    @[in ] fRunTime: 已变换的运行时间
*    @[in ] fAllTime: 已变换的总时间
*    @[in ] ActionBlockID: 动作id
* 返回值: int  返回当前行号
* 时间: 2015/12/10 17:38
* 作者: 周志平
*/
int COptionsMotionWidget::AddToTableWidget(char *pData, int nLen, QString BlockType, float fRunTime, float fAllTime, int ActionBlockID)
{

    int nLens = 0;
    memcpy(&nLens, pData,sizeof(int));
    int nCount =nLens/sizeof(MOTOR_DATA);
    MOTOR_DATA* pMotor = (MOTOR_DATA*)(pData + 4);

    if(m_pGridTable == NULL)
    {
        qDebug()<<"can't find m_pGridTable"<<endl;
        return -1;
    }

    int nRow=m_pGridTable->rowCount();
    m_pGridTable->insertRow(nRow);

    QString strtext = "";
    for(int i=0;i<nCount;i++)
    {
        strtext = QString::number(pMotor[i].nAngle);
        QTableWidgetItem* pItem = new QTableWidgetItem(strtext);
        pItem->setTextAlignment(Qt::AlignCenter);
        m_pGridTable->setItem(nRow,i,pItem);
    }
    int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();
    QString strRuntime = QString::number(fRunTime*nScaleTime);
    QTableWidgetItem*  pRuntimeItem = new QTableWidgetItem(strRuntime);
    pRuntimeItem->setTextAlignment(Qt::AlignCenter);
    m_pGridTable->setItem(nRow,nCount,pRuntimeItem);

    QString strAlltime = QString::number(fAllTime*nScaleTime);
    QTableWidgetItem* pAlltimeItem = new QTableWidgetItem(strAlltime);
    pAlltimeItem->setTextAlignment(Qt::AlignCenter);
    m_pGridTable->setItem(nRow,nCount+1,pAlltimeItem);


    QTableWidgetItem* pFirstItem = m_pGridTable->item(nRow,0);
    pFirstItem->setData(OptionItemRoles::ActionBlockDataID, ActionBlockID);

    m_pGridTable->setRowHeight(nRow, GRID_COL_HEIGHT);


    ///////////////////////////////////////
    if(m_bAutoModifyItemData)
    {
        changeItemColor(m_EditItemDefaultColor);
        m_bAutoModifyItemData = false;
    }
    ///////////////////////////////////////


    warningItemValueInvalid();

    return nRow;
}

/**************************************************************************
 * 函数名: addMotionFrame
 * 功能: 添加动作帧到指定的block
 * 参数:
 *    @[in ] nBlockId: block id
 * 返回值: void
 * 时间: 2016/12/08
 * 作者: hwx
 */
void COptionsMotionWidget::addMotionFrame(int nBlockId)
{
    //  AddToTableWidget();
    //AddSampleListToTableWidget();
    //不能直接使用数字10作为时间刻度
    if (!m_pEvent)
    {
        return;
    }

    int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue() ;//时间单位（ms）
    if (!m_pEvent || !m_pRobotView)
    {
        return;
    }

    char* pData = NULL;
    int nLen = 0;

    m_pRobotView->GetMotorDatas(&pData, nLen);
    if (!pData || nLen <= 0)
    {
        return;
    }

    // 修改前先保存历史数据，以便进行undo操作
    IOperation *pChangedMotionData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
    cloneBeforeChangedMotionData(pChangedMotionData);

    float fRunTime = 0;
    if(ui->leBeatValue->isEnabled())
    {
        QString tempValue;
        tempValue = ui->leBeatValue->text();
        fRunTime = tempValue.toFloat();
        if (fRunTime < 10)
        {
            fRunTime = 10;
        }
    }

    CActionBlockData* pActionBlockData = new CActionBlockData();
    pActionBlockData->SetID(-1);
    WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
    memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
    QString strDescription = "Action";
    int nDescriptionLen = QStringToWCHAR(wstrDescription, strDescription);
    //wstrDescription[nDescriptionLen-1] ='\0';
    pActionBlockData->SetDescription(wstrDescription);
    pActionBlockData->SetRunTime(fRunTime / nScaleTime);
    pActionBlockData->SetAllTime(fRunTime / nScaleTime);//设置总时间
    pActionBlockData->SetExtBlockData(nLen, pData);

    g_IsUpdateUIByData = true;   //这里默认需要修改
    g_IsModifyGroup = true;      //这里默认需要修改
    m_pEvent->ILayerOptionHandler_OnAddActionBlock(nBlockId, pActionBlockData, "", true);
    g_IsUpdateUIByData = false;
    //                       float fAllTime = fRunTime*(nID+1);
    if (nBlockId != INVALID_BLOCK_ID)
    {
        int nID = pActionBlockData->GetID();

        SetShowID(&nID, 1);
        QString strTmpDescription = WCHARToQString(pActionBlockData->GetDescription(), MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
        int nRow = AddToTableWidget(pData, nLen, strTmpDescription, pActionBlockData->GetRunTime(), pActionBlockData->GetAllTime(), nID);
        setSelectRow(nRow);
        //                            ShowSelectActionBlock();
    }
    else
    {
        int nID = pActionBlockData->GetID();
        SetShowID(&nID, 1);
        //                            ShowSelectActionBlock();
    }
    delete [] pData;

    // 完成后修改后保存数据，以便进行redo操作
    cloneAfterChangedMotionData(pChangedMotionData);
}

void COptionsMotionWidget::UpdateTableWidgetShow(int nRow, CActionBlockData *pActionBlock)
{
    if(pActionBlock == NULL ||
            m_pRobotView == NULL ||
            m_pGridTable == NULL)
        return;

    char* pData = NULL;
    int nLen = 0;
    pActionBlock->GetExtBlockData(&pData, nLen);
    int nCount = m_pRobotView->GetMotorCount();
    for(int i = 0; i < nCount; i++)
    {
        MOTOR_DATA* pMotor = GetMotorData(pData, nLen, i);
        QTableWidgetItem* item = m_pGridTable->item(nRow, i);
        item->setText(QString::number(pMotor->nAngle));
    }
}

/**************************************************************************
* 函数名: on_pbInsertSplit_clicked
* 功能:   动作插入按钮按下事件
* 参数:
* 返回值:
* 时间:   2015/12/25 16:36
* 作者: 周志平
**************************************************************************/
void COptionsMotionWidget::on_pbInsertSplit_clicked()
{
 ////////////////////////////////////////////////////////////////////////////////////////////////////
 // 插入拆分问题暂时不改动，考虑在下一个版本中更新
 //////////////////////////////////////////////////////////////////////////////////////////////////
    if (m_pGroupManager == NULL || m_pGridTable == NULL)
    {
        return;
    }

    CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if(pGroupManager == NULL)
    {
        return;
    }

    // 获取当前选中的行，作为插入拆分的行；如果当前没有选中整行，则默认取最后一行
    int nCurrentRow = m_pGridTable->currentRow();
    bool IsRowSelected = IsTableHasSelectRow();
//    if (!IsRowSelected)
//    {
//        nCurrentRow = m_pGridTable->rowCount() - 1;
//    }

    if (nCurrentRow < 0)
    {
        return;
    }

    // 由于精度问题，时间会稍有不同
    float fOldEndTime = pGroupManager->GetEndTime();
    if (!m_pEvent || !m_pRobotView)
    {
        return;
    }

    QTableWidgetItem* pItem = m_pGridTable->item(nCurrentRow, 0);
    int nSelectBlockID = pItem->data(OptionItemRoles::ActionBlockDataID).toInt();
    char* pData = NULL;
    int nLen = 0;
    m_pRobotView->GetMotorDatas(&pData, nLen);
    if (!pData || nLen <= 0)
    {
        return;
    }

    CActionBlockData *pSelectBlockData = pGroupManager->FindBlockData(nSelectBlockID);
    if (NULL == pSelectBlockData)
    {
        return;
    }

    int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue() ;//时间单位（ms）
    float fAverageAllTime =pSelectBlockData->GetAllTime()/2;  //平均总时间
    //四舍五入保留一位小数
    if (fAverageAllTime * nScaleTime  < GRID_MIN_TIMEVALUE)
    {
        CMessageBox::warning(this, "", tr("Average All Time can't less than 200 ms!"));
        return;
    }

    float fAverageRunTime = pSelectBlockData->GetRunTime()/2;  //平均运动时间
    if (fAverageRunTime * nScaleTime  < GRID_MIN_TIMEVALUE)
    {
        CMessageBox::warning(this, "", tr("Average Run Time can't less than 200 ms!"));
        return;
    }

    // 保存插入拆分前的数据，已进行undo操作
    IOperation *pChangedMotionData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
    cloneBeforeChangedMotionData(pChangedMotionData);

    // 修改选定帧动作时间
    pSelectBlockData->SetAllTime(fAverageAllTime);
    pSelectBlockData->SetRunTime(fAverageRunTime);

    // 新建新动作
    CActionBlockData* pActionBlockData = new CActionBlockData();
    pActionBlockData->SetID(pGroupManager->GetUnUserID());

    WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
    memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
    int nDescriptionLen = QStringToWCHAR(wstrDescription, "Action");
    //wstrDescription[nDescriptionLen-1] ='\0';
    pActionBlockData->SetDescription(wstrDescription);

    pActionBlockData->SetRunTime(fAverageRunTime);
    pActionBlockData->SetAllTime(fAverageAllTime);
    pActionBlockData->SetExtBlockData(nLen, pData);
    pGroupManager->InsertAfterId(nSelectBlockID, pActionBlockData);

    char* pdatas = NULL;
    int nLenss = 0;
    pGroupManager->GetExtBlockData(&pdatas, nLenss);
    delete [] pdatas;

    if (nLenss == 12 || nLenss == 14)
    {
        pdatas = "analysisRecoup";
    } else {
        pdatas = "analysis";
    }
    nLenss = strlen(pdatas);
    pGroupManager->SetExtBlockData(nLenss, pdatas);

    int nCurrentShowID = pActionBlockData->GetID();
    SetShowID(&nCurrentShowID, 1);
    int nCurrentID = m_nCurrentGroupID;
    m_nCurrentGroupID = -1;
    pGroupManager->RecalculateTime();
    g_IsUpdateUIByData = true;
    m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, pGroupManager->GetEndTime() - fOldEndTime, true);
    g_IsUpdateUIByData = false;
    setSelectRow(nCurrentRow + 1);
    delete[] pData;

    cloneAfterChangedMotionData(pChangedMotionData);

    return;
}
void COptionsMotionWidget::on_pbModify_clicked()
{
    if(m_pGridTable == NULL || m_pGroupManager == NULL)
        return;

    CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if(pGroupManager == NULL)
        return;

    // 保存修改前的数据，进行undo操作
    IOperation *pChangedMotionData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
    cloneBeforeChangedMotionData(pChangedMotionData);

    for(int i = 0; i<m_pGridTable->rowCount();++i)
    {
        int nRow = i;
        if( (nRow >= 0) && IsFullRowSelect(nRow) )
        {
            QTableWidgetItem * pItem = m_pGridTable->item(i,0);
            int nBlockDataID = pItem->data(OptionItemRoles::ActionBlockDataID).toInt();
            if(m_pEvent && m_pRobotView)
            {
                char* pData = NULL;
                int nLen = 0;
                m_pRobotView->GetMotorDatas(&pData, nLen);
                if(pData && nLen)
                {
                    CActionBlockData* pBlockData = pGroupManager->FindBlockData(nBlockDataID);
                    if (pBlockData == NULL)
                        return;
                    pBlockData->SetExtBlockData(nLen, pData);
                    int nCurrentID = pBlockData->GetID();
                    SetShowID(&nCurrentID, 1);
                    if(m_pEvent)
                    {
                        m_pEvent->ILayerOptionHandler_OnActionBlockDataChange(m_nCurrentGroupID, nCurrentID, 0);
                    }
                    UpdateTableWidgetShow(i, pBlockData);
                }
            }
        }
    }

    // 保存修改后的数据，用来进行redo操作
    cloneAfterChangedMotionData(pChangedMotionData);
}

/**************************************************************************
* 函数名: onMotionFrameDelete
* 功能:   动作删除按钮按下事件
* 参数:
* 返回值:
* 时间:   2015/12/25 16:36
* 作者: 周志平
* 改动日志：
**************************************************************************/
void COptionsMotionWidget::onMotionFrameDelete()
{
    if(m_pGridTable == NULL ||
            m_pGroupManager == NULL)
    {
        qDebug()<<"can't find m_pGridTable"<<endl;
        return;
    }

    CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if(pGroupManager == NULL)
    {
        return;
    }

    // 删除前保存数据
    IOperation *pChangedMotionData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
    cloneBeforeChangedMotionData(pChangedMotionData);

    bool bUpdate = false;
    // 最后一次删除的ID
    int nLastDeleteID = -1;
    float nTimeInc = 0;
    //fix bug 135
    for(int i = m_pGridTable->rowCount(); i >= 0 ; i--)
    {
        bool bSelectFullLine = true;
        bSelectFullLine = IsFullRowSelect(i);

        if (bSelectFullLine)
        {
            bUpdate = true;

            QTableWidgetItem * pItem = m_pGridTable->item(i,0);
            int nActionBlockID = pItem->data(OptionItemRoles::ActionBlockDataID).toInt();
            CActionBlockData* pActionBlockData = pGroupManager->FindBlockData(nActionBlockID);
            if(pActionBlockData == NULL)
            {
                continue;
            }

            QString tempDescription = WCHARToQString(pActionBlockData->GetDescription(),MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
            int nLens = 0;
            char* pData = NULL;
            pGroupManager->GetExtBlockData(&pData, nLens);
            delete[] pData;

            nTimeInc += pActionBlockData->GetAllTime();
            pGroupManager->DeleteLayerBlock(nActionBlockID);

            //检查删除后，是否还存在补偿帧
            bool IsHaveCompensatory = false;
            vector<CActionBlockData*> vecDataBlock;
            pGroupManager->GetAllBlockData(vecDataBlock);
            for (int i = 0; i < vecDataBlock.size(); i++)
            {
                CActionBlockData* pTempBlockData = vecDataBlock[i];
                QString strDescription = WCHARToQString(pTempBlockData->GetDescription(),MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
                if (!strDescription.compare("Recoup", Qt::CaseInsensitive))
                {
                    IsHaveCompensatory = true;
                    break;
                }
            }
            if (!IsHaveCompensatory)
            {
                char *pdatas = NULL;
                int nLenss = 0;
                pGroupManager->GetExtBlockData(&pdatas, nLenss);
                delete []  pdatas;

                if (nLenss == 6 || nLenss == 12)
                {
                    pdatas = "motion";
                } else {
                    pdatas = "analysis";
                }
                nLenss = strlen(pdatas);
                pGroupManager->SetExtBlockData(nLenss, pdatas);
            }

            // 如果删除动作帧后，整个动作组已经没有动作帧，则需要删除动作组
            if (pGroupManager->GetActionCount() < 1)
            {
                // 删除动作帧时，已经拷贝了数据进行撤销重做，
                // 这里删除动作帧不用拷贝数据进行撤销重做
                m_pEvent->ILayerOptionHandler_OnDeleteGroupBlock(false);
                nTimeInc = 0; // add by huagnwuxian 2017.01.13 删除block时已经调整过了，不再需要进行调整
                pGroupManager = NULL;
            }

            nLastDeleteID = i;
            deleteSampleListItem(i);
        }
        else
       {
            continue;
        }
    }

//    // add by huangwuxian 2017.01.12
//    // 删除以后需要重新计算时间
//    pGroupManager->RecalculateTime();

    // 滚动到最后一条删除的记录
    if (nLastDeleteID != -1)
    {
        if (nLastDeleteID < m_pGridTable->rowCount()-1)
        {
            int nShowID = m_pGridTable->item(nLastDeleteID+1, 0)->data(OptionItemRoles::ActionBlockDataID).toInt();
            if (pGroupManager)
            {
                CActionBlockData* pActionBlockData = pGroupManager->FindBlockData(nShowID);
                if (pActionBlockData)
                {
                    nShowID = pActionBlockData->GetID();
                }
                else
                {
                    nShowID = -1;
                }
            }
            else
            {
                nShowID = -1;
            }
        }
        else
        {
            nLastDeleteID = -1;
        }
    }

    if (bUpdate)
    {
        int nCurrentID = m_nCurrentGroupID;
        m_nCurrentGroupID = -1;
        g_IsUpdateUIByData = true;
//        m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, -nTimeInc, true);
        m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, -nTimeInc, true);
        g_IsUpdateUIByData = false;

        if (nLastDeleteID != -1)
        {
            SetShowID(&nLastDeleteID, 1);
            setSelectRow(nLastDeleteID);
        }
        //            else
        //            {
        //                // 滚动到最后一行
        //                if (m_Grid.GetRowCount())
        //                    m_Grid.EnsureVisible(m_Grid.GetRowCount()-1, 0);
        //            }
    }

    // 保存删除后的数据
    cloneAfterChangedMotionData(pChangedMotionData);
}

/**************************************************************************
* 函数名: IsFullRowSelect
* 功能:   当前行是否被全部选中
* 参数:
* 返回值:
* 时间:   2015/12/25 16:36
* 作者: 周志平
**************************************************************************/
bool COptionsMotionWidget::IsFullRowSelect(int nRow)
{
    if(m_pGridTable == NULL)
        return false;
    for (int j=1; j<m_pGridTable->columnCount(); ++j)
    {

        QTableWidgetItem * ptwiActionItem = m_pGridTable->item(nRow,j);
        if (m_pGridTable->isItemSelected(ptwiActionItem) == false)    //判断是否选中一行
        {
            return false;
        }
    }

    return true;
}

bool COptionsMotionWidget::isFullRowSelect(const QList<QTableWidgetSelectionRange> &ranges)
{
    int nRangeCount = ranges.count();
    if(nRangeCount <= 0)
    {
        return false;
    }

    for (int i=0; i < nRangeCount; i++)
    {
        int nTopRow = ranges.at(i).topRow();
        int nBottomRow = ranges.at(i).bottomRow();
        for(int j = nTopRow ;j <= nBottomRow;j++)
        {
            bool bIsFullRowSelect = IsFullRowSelect(j);
            if(!bIsFullRowSelect)
            {
                return false;
            }
        }
    }

    return true;
}

bool COptionsMotionWidget::isContinueRange(const QList<QTableWidgetSelectionRange> &selectRange)
{
    int nRangeCount = selectRange.count();
    if(nRangeCount != 1)
    {
        return false;
    }

    return true;
}

bool COptionsMotionWidget::getRangeData(ActItemCopyData &itemData, const QList<QTableWidgetSelectionRange> &selectRange, bool bOnlyRowColumn)
{
    itemData.initData();
    if(!isContinueRange(selectRange))
    {
        return false;
    }

    for(int i = 0; i < 1; i++)
    {
        const QTableWidgetSelectionRange& range = selectRange.at(i);
        int nTopRow = range.topRow();
        int nBottomRow = range.bottomRow();
        for(int j = nTopRow; (j <= nBottomRow) && !bOnlyRowColumn; j++)
        {
            int nLeftColumn = range.leftColumn();
            int nRightColumn = range.rightColumn();

            VectorInt vectorInt;
            for(int k = nLeftColumn; k <= nRightColumn; k++)
            {
                QTableWidgetItem *pItem = m_pGridTable->item(j,k);
                if(!pItem)
                {
                   itemData.initData();
                   return false;
                }

                int nTemp = pItem->text().toInt();
                vectorInt.push_back(nTemp);
            }

            itemData.listItem.push_back(vectorInt);
        }

        itemData.nRow = range.rowCount();
        itemData.nColumn = range.columnCount();
    }

    return true;
}

/**************************************************************************
* 函数名: IsTableHasSelectRow
* 功能:   当前行是否被选中
* 参数:
* 返回值:
* 时间:   2015/12/25 16:36
* 作者: 周志平
**************************************************************************/
bool COptionsMotionWidget::IsTableHasSelectRow()
{
    if(m_pGridTable == NULL)
        return false;
    for(int i=0; i<m_pGridTable->rowCount(); i++)
    {
        if (IsFullRowSelect(i) == true)
            return true;
    }

    return false;
}

/**************************************************************************
* 函数名: ShowSelectActionBlock
* 功能:   显示选中的动作块
* 参数:
* 返回值:
* 时间:   2015/12/25 16:36
* 作者: 周志平
* 改动日志  2015/12/25 16:36 函数被注释了
**************************************************************************/
void COptionsMotionWidget::setSelectRow(int row)
{
    if (!m_pGridTable)
    {
        return;
    }

    if(row >= m_pGridTable->rowCount())
    {
        return;
    }

    m_pGridTable->selectRow(row);
}
/**************************************************************************
* 函数名:initTableWidget
* 功能:初始化 动作表格
* 参数:
* 返回值:
* 时间:* 时间:2016/01/07 14:08
* 作者: 周志平
* 改动日志
**************************************************************************/
void COptionsMotionWidget::initTableWidget()
{
    //    QTableWidget *table =ui->twActionList;
    if(m_pGridTable == NULL)
    {
        qDebug()<<"m_pGridTable is NULL"<<endl;
        return;
    }


    //if(m_nServoCount == 16)
    {
        m_pGridTable->setColumnCount(m_nServoCount + 2);
        //    table->setRowCount();
        for(int i = 0;i<m_pGridTable->columnCount()-2;i++)
        {
            m_pGridTable->setColumnWidth(i, GRID_COL_WIDTH);
        }
        m_pGridTable->setColumnWidth(m_pGridTable->columnCount() - 1,GRID_COL_RUNTIME);
        m_pGridTable->setColumnWidth(m_pGridTable->columnCount() - 2,GRID_COL_RUNTIME);

        //设置表格行标题
        retranslateUi();
    }

    m_pGridTable->setFrameShape(QFrame::NoFrame);
    m_pGridTable->setShowGrid(false); //不显示格子线
    m_pGridTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    m_pGridTable->horizontalHeader()->setFixedHeight(GRID_COL_HEIGHT);
    m_pGridTable->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    m_pGridTable->verticalHeader()->setFixedWidth(GRID_COL_WIDTH);

    //    //设置表格行标题的对齐方式
    //    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    //    //设置行表题
    //    QStringList rowLabels;
    //    rowLabels << "Line1" << "Line2" << "Line3" << "Line4";
    //    table->setVerticalHeaderLabels(rowLabels);


    //    //设置每一行的高度
    //    for(int i = 0; i < 4; i++)
    //        table->setRowHeight(i, 22);

    //自动调整最后一列的宽度使它和表格的右边界对齐
    //    table->horizontalHeader()->setStretchLastSection(true);

    //设置表格的选择方式
    m_pGridTable->setSelectionBehavior(QAbstractItemView::SelectItems);

    //设置编辑方式
    m_pGridTable->setEditTriggers(QAbstractItemView::DoubleClicked);
}

/**************************************************************************
* 函数名:InsertSampleListToTableWidget
* 功能:插入动作组例子，后面需要切换为复制后的数据
* 参数:int nCurRow
* 返回值:
* 时间:2016/01/07 14:08
* 作者: 周志平
* 改动日志
**************************************************************************/
void COptionsMotionWidget::InsertSampleListToTableWidget(int nCurRow)
{


    int cols=m_pGridTable->columnCount();
    //           int rows=ui->twActionList->rowCount();
    //           qDebug()<<rows;
    m_pGridTable->insertRow(nCurRow);
    QTableWidgetItem* pItem = NULL;
    for(int i=0;i<cols;i++)
    {
        pItem = new QTableWidgetItem(/*"new"+*/QString::number(nCurRow+1));
        pItem->setTextAlignment(Qt::AlignCenter);
        m_pGridTable->setItem(nCurRow,i,pItem);
    }
    m_pGridTable->setRowHeight(nCurRow, GRID_COL_HEIGHT);
    m_pGridTable->selectRow(nCurRow);
}

/**************************************************************************
* 函数名:CopySampleSelsctItem
* 功能:复制动作组例子，后面需要切换为时间层传过来的数据
* 参数:
* 返回值:
* 时间:2016/01/07
* 作者: 周志平
* 改动日志
**************************************************************************/
bool COptionsMotionWidget::CopySampleSelsctItem()
{

    //    bool bRslt =false;
    QTableWidgetItem * item =  m_pGridTable->currentItem();
    if(item == NULL)
    {
        return false;
    }
    int nRow = item->row();
    for (int j=0; j<m_pGridTable->columnCount(); j++)
    {
        QTableWidgetItem * ptwiActionItem = m_pGridTable->item(nRow,j);
        if (m_pGridTable->isItemSelected(ptwiActionItem) ==false)    //判断是否选中一行
        {
            //            bRslt = false ;de
            return false;
        }
        QString strText = ptwiActionItem->text();
    }
    return true;
}

/**************************************************************************
* 函数名: UpdateUIByData
* 功能:更新动作组列表
* 参数:
* 返回值:
* 时间:2016/01/07 14:00
* 作者: 周志平
* 改动日志
**************************************************************************/
void COptionsMotionWidget::UpdateUIByData()
{
    if(m_pGroupManager ==NULL)
    {
        return;
    }
    if(m_pGroupListWidget ==NULL)
    {
        return;
    }
    if((m_pGroupListWidget->count() == m_pGroupManager->GetCount()) && !g_IsModifyGroup)
    {
        m_pGroupListWidget->setItemSelected(m_pGroupListWidget->item(m_nCurrentGroupListSelectItem),true);
        return;
    }
    m_pGroupListWidget->clear();
    for(int i = 0;i<m_pGroupManager->GetCount();i++)
    {
        CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(i);
        QString strDescription = WCHARToQString(pGroup->GetDescription(), MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
        QListWidgetItem * pItem = new QListWidgetItem(strDescription);
        m_pGroupListWidget->insertItem(i,pItem);
        int nID = pGroup->GetID();
        pItem->setData(OptionItemRoles::GroupID,nID);
    }
    m_pGroupListWidget->show();

    m_pEvent->ILayerOptionHandler_HasMotionData(m_pGroupListWidget->count() > 0);
    if(m_pGroupListWidget->count() > 0)
    {
        ui->btnUnitedSimulation->setEnabled(true);
        ui->btnStop->setEnabled(true);
        ui->pbInsert->setEnabled(true);
        ui->pbInsertSplit->setEnabled(true);
        ui->pbModify->setEnabled(true);
    }
    else
    {
        ui->btnUnitedSimulation->setEnabled(false);
        ui->btnStop->setEnabled(false);
        ui->pbInsert->setEnabled(false);
        ui->pbInsertSplit->setEnabled(false);
        ui->pbModify->setEnabled(false);
    }
}

/**************************************************************************
* 函数名: SetCurrentSelectItem
* 功能:设置选中项
* 参数:
* 返回值:
* 时间:2016/01/07 14:00
* 作者: 周志平
* 改动日志
**************************************************************************/
void COptionsMotionWidget::SetCurrentSelectItem(QListWidgetItem * item, int nIndex, int nSelRow)
{
    if(m_pGroupListWidget == NULL)
    {
        return;
    }

    if(m_pGroupManager)
    {
        m_pGroupListWidget->clearSelection();
        QList<int> lstGroupID = m_pGroupManager->getGroupIDSelected();
        for(int i = 0; i < lstGroupID.size(); i++)
        {
            QListWidgetItem * pItem = COptionsMotionWidget::getListWidgetItem(lstGroupID.at(i));
            if(pItem)
            {
                m_pGroupListWidget->setItemSelected(pItem, true);
            }
        }
    }

    if(m_nCurrentGroupListSelectItem !=-1)
    {
        m_pGroupListWidget->setItemSelected(item, true);//此处Windows版加了图标
    }
    if(nIndex == -1)
    {
        if(m_pGridTable->winId())
        {
            m_pGridTable->clearContents();
            m_pGridTable->setRowCount(0);
        }
        return;
    }

    m_pGroupListWidget->setItemSelected(item,true);
    m_nCurrentGroupListSelectItem = nIndex;
    if(m_pGroupManager)
    {
        CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(nIndex);
        if(pGroup)
        {
            char* pData = NULL;
            int nLen = 0;
            pGroup->GetExtBlockData(&pData, nLen);
            delete[] pData;
            if(nLen == 8 || nLen == 14)
            {
                IsCompensatory = true;
            }
            else
            {
                IsCompensatory = false;
            }
            m_nCurrentGroupID = pGroup->GetID();
            UpdataGridByGroupManager(nSelRow, pGroup);
        }
    }

}
/**************************************************************************
* 函数名:UpdataGridByGroupManager
* 功能:根据动作组更新动作表格数据
* 参数:nSelRow: 当前选中你的block在数组中的下标
* 返回值:
* 时间:2016/01/07
* 作者: 周志平
* 改动日志
**************************************************************************/
void COptionsMotionWidget::UpdataGridByGroupManager( int nSelRow, CActionBlockGroupManager *pGroup)
{
    if(m_pGridTable == NULL)
    {
        return;
    }
    m_pGridTable->clearContents();
    m_pGridTable->setRowCount(0);
    int nRow = -1;
    for(int i = 0; i<pGroup->GetActionCount(); i++)
    {
        CActionBlockData* pBlockData = pGroup->GetAt(i);
        if(pBlockData)
        {
            char* pData = NULL;
            int nLen = 0;
            pBlockData->GetExtBlockData(&pData, nLen);

            QString strDestription = WCHARToQString(pBlockData->GetDescription(), MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
            if(pData && nLen)
            {
                nRow = AddToTableWidget(pData, nLen, strDestription, pBlockData->GetRunTime(),
                                        pBlockData->GetAllTime(), pBlockData->GetID());
                delete[] pData;
            }
        }
        ui->leBeatValue->setEnabled(true);
    }

    // 默认选中最后一行
    if (nSelRow >=0 && nSelRow <= nRow)
    {
        setSelectRow(nSelRow);
    }
}
/**************************************************************************
* 函数名:deleteSampleListItem
* 功能:删除动作组例子，后面需要切换为时间层传过来的数据
* 参数:int nRow
* 返回值:
* 时间:2016/01/07
* 作者: 周志平
* 改动日志
**************************************************************************/
void COptionsMotionWidget::deleteSampleListItem(int nRow)
{
    //    QTableWidgetItem * item = ui->tableWidget->currentItem();
    //    if(item==Q_NULLPTR)return;
    //    int nRow = item ->row();
    if(nRow>=0)
        m_pGridTable->removeRow(nRow);
    //    if(ui->)
    //     ui->twActionList->selectRow(nRow-1);
}

/**************************************************************************
* 函数名:on_btnAddGroup_clicked
* 功能:临时函数，用来添加新的动作组，有了时间层控件后需要把这个函数删除
* 参数:
* 返回值:
* 时间:2015/12/25 16:41
* 作者: 周志平
* 改动日志
**************************************************************************/
//void COptionsMotionWidget::on_btnAddGroup_clicked()
//{
//    //    AddActionGroup();

//    SetActionBlockDataManager(m_pGroupManager, -1,-1);
//    //    if(m_pEvent)
//    //    {
//    //        m_pEvent->ILayerOptionHandler_OnSelectGroupManageChange(-1,-1);

//    //    }
//    int nScaleTime = 10 ;//时间单位（ms）
//    if (m_pEvent && m_pRobotView)
//    {
//        char* pData = NULL;
//        int nLen = 0;

//        m_pRobotView->GetMotorDatas(&pData, nLen);
//        if (pData && nLen)
//        {
//            float fRunTime = 0;
//            QString tempValue;
//            tempValue = ui->leBeatValue->text();
//            fRunTime = tempValue.toFloat();
//            if (fRunTime < 10)
//            {
//                fRunTime = 10;
//            }
//            CActionBlockData* pActionBlockData = new CActionBlockData();
//            pActionBlockData->SetID(-1);
//            WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
//            memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
//            QString strDescription = QObject::tr("Action");
//            int nDescriptionLen = QStringToWCHAR(wstrDescription, strDescription);
//            wstrDescription[nDescriptionLen-1] ='\0';
//            pActionBlockData->SetDescription(wstrDescription);
//            pActionBlockData->SetRunTime(fRunTime / nScaleTime);
//            pActionBlockData->SetAllTime(fRunTime / nScaleTime);//设置总时间
//            pActionBlockData->SetExtBlockData(nLen, pData);
//            int nCurrentID = GetCurrenGroupManagerID();
//            g_IsUpdateUIByData = true;   //这里默认需要修改
//            m_pEvent->ILayerOptionHandler_OnAddActionBlock(nCurrentID, pActionBlockData);
//            //                       float fAllTime = fRunTime*(nID+1);
//            if (nCurrentID != -1)
//            {
//                int nID = pActionBlockData->GetID();

//                SetShowID(&nID, 1);
//                QString strTmpDescription = WCHARToQString(pActionBlockData->GetDescription(), MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
//                AddToTableWidget(pData, nLen, strTmpDescription, pActionBlockData->GetRunTime(), pActionBlockData->GetAllTime(), nID);
//                //                            ShowSelectActionBlock();
//            }
//            else
//            {
//                int nID = pActionBlockData->GetID();
//                SetShowID(&nID, 1);
//                //                            ShowSelectActionBlock();
//            }
//            //            delete [] pData;
//        }
//    }

//}

/**************************************************************************
* 函数名:on_btnDeleteGroup_clicked
* 功能:动作组列表点击事件，由PC版移植过来，用来添加选中左边的勾勾图标，现在还没有图标
* 参数:const QModelIndex &index
* 返回值:
* 时间:2016/01/07
* 作者: 周志平
* 改动日志
**************************************************************************/
//void COptionsMotionWidget::on_m_ListActionGroup_clicked(const QModelIndex &index)
//{
//    int nRow = index.row();
//    if(nRow != -1 )
//    {
//        SetCurrentSelectItem(m_pGroupListWidget->item(nRow),nRow);
//        if(m_pEvent)
//        {
//            m_pEvent->ILayerOptionHandler_OnSelectGroupManageChange(m_nCurrentGroupID,0);
//        }
//    }
//}


//void COptionsMotionWidget::on_m_ListActionGroup_itemClicked(QListWidgetItem *item)
//{
//    if(item == NULL)
//    {
//        return;
//    }
//    SetCurrentSelectItem(item, item->);
//}

/**************************************************************************
* 函数名:on_btnDeleteGroup_clicked
* 功能:删除动作组响应函数
* 参数:
* 返回值:
* 时间:2016/01/07
* 作者: 周志平
* 改动日志
**************************************************************************/
//void COptionsMotionWidget::on_btnDeleteGroup_clicked()
//{
//    if(m_pGroupListWidget == NULL ||
//            m_pEvent == NULL)
//    {
//        return;
//    }
//    QList<QListWidgetItem*> listItems = m_pGroupListWidget->selectedItems();
//    for(int i = 0; i < listItems.count(); i++)
//    {
//        QListWidgetItem* pItem = listItems.at(i);
//        int nGroupID = pItem->data(OptionItemRoles::GroupID).toInt();
//        CActionBlockGroupManager* pGroup = m_pGroupManager->FindBlockData(nGroupID);
//        if(pGroup)
//        {
//            int startTime = pGroup->GetStartTime();
//            m_pEvent->ILayerOptionHandler_SetSelectGroupBlock(startTime + 2);
//        }
//    }

//    m_pEvent->ILayerOptionHandler_OnDeleteGroupBlock();

//    if(m_pGridTable ==NULL)
//    {
//        return;
//    }
//    for(int i = m_pGridTable->rowCount(); i>=0;--i)
//    {
//        deleteSampleListItem(i);
//    }
//}
/**************************************************************************
* 函数名:onGroupListDelete
* 功能:动作组右键响应事件
* 参数:QListWidgetItem *item
* 返回值:
* 时间:* 时间:2016/01/07
* 作者: 周志平
* 改动日志
**************************************************************************/
void COptionsMotionWidget::slotOnGroupItemPressed(QListWidgetItem *item)
{
    if(item==Q_NULLPTR)
    {
        return;
    }

    if(qApp->mouseButtons() == Qt::LeftButton || qApp->mouseButtons() == Qt::RightButton)
    {
        //m_lstGroupIDSelect.clear();

        QMap<int, int> mapRow;

        if((qApp->mouseButtons() == Qt::LeftButton)
                && ((QApplication::keyboardModifiers() == Qt::ControlModifier)
                                                        || (QApplication::keyboardModifiers() == Qt::ShiftModifier)))
        {
            QList<QListWidgetItem*> listItem = m_pGroupListWidget->selectedItems();
            for(int i = 0; i<listItem.size(); i++)
            {
                const int nRow = m_pGroupListWidget->row(listItem.at(i));
                mapRow.insert(nRow, nRow);
            }
        }

        else if(qApp->mouseButtons() == Qt::RightButton)
        {
            QList<QListWidgetItem*> listItem = m_pGroupListWidget->selectedItems();
            for(int i = 0; i<listItem.size(); i++)
            {
                const int nRow = m_pGroupListWidget->row(listItem.at(i));
                mapRow.insert(nRow, nRow);
            }
        }

        const int nItemSelectRow = m_pGroupListWidget->row(item);
        mapRow.insert(nItemSelectRow, nItemSelectRow);

        m_pGroupManager->clearAllGroupIDSelect();
        QMap<int, int>::iterator iter = mapRow.begin();
        while (iter != mapRow.end())
        {
            CActionBlockGroupManager* pGroupManager = m_pGroupManager->GetAt(iter.key());
            if (pGroupManager)
            {
                //m_lstGroupIDSelect.push_back(pGroupManager->GetID());
                m_pGroupManager->setGroupIDSelected(pGroupManager->GetID(), true);
            }

            ++iter;
        }

        //////
        int nRow = m_pGroupListWidget->row(item);
        if(nRow != -1 )
        {
            SetCurrentSelectItem(item,nRow, 0);
            if(!m_pEvent || !m_pGroupManager)
            {
                return;
            }

//            CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(nRow);
//            if (!pGroup)
//            {
//                return;
//            }

            //m_pEvent->ILayerOptionHandler_OnSelectGroupManageChange(m_nCurrentGroupID, 0);//当动作组修改后复制可能会选择上一个动作块
            m_pEvent->ILayerOptionHandler_OnSelectActionManageChange(m_nCurrentGroupID, 0);//by asu,
        }

        if (qApp->mouseButtons() == Qt::RightButton)
        {
            CreateGroupListPopMenuActions();
        }
    }
}

/***********************************************************************
 * 函数名： createPopMenuActions
 * 功能：  创建弹出菜单
 * 返回值： void
 * 作者：  周志平
 ***********************************************************************/
void COptionsMotionWidget::CreateGroupListPopMenuActions()
{
    //创建菜单、菜单项
    QMenu* pop_menu = new QMenu();
    QAction* DeleteAction = new QAction(tr("Delete"),this);
    QAction* ModifyAction = new QAction(tr("Modify group name"),this);
    QAction* CopyAction = new QAction(tr("Copy"),this);
    QAction* PasteAction = new QAction(tr("Paste"),this);
    QAction* mergeAction = new QAction(tr("Merge"),this);

    pop_menu->addAction(DeleteAction);
    pop_menu->addAction(ModifyAction);
    pop_menu->addAction(CopyAction);
    pop_menu->addAction(PasteAction);
    pop_menu->addAction(mergeAction);

    if(m_pGroupManager->getGroupIDSelected().size() < 2)
    {
        mergeAction->setEnabled(false);
    }else {
        DeleteAction->setEnabled(false);
        ModifyAction->setEnabled(false);
        CopyAction->setEnabled(false);
        PasteAction->setEnabled(false);
    }

    if (isMotionSimulating())
    {
        DeleteAction->setEnabled(false);
        ModifyAction->setEnabled(false);
        CopyAction->setEnabled(false);
        PasteAction->setEnabled(false);
        mergeAction->setEnabled(false);
    }

    QAbstractItemDelegate::connect(DeleteAction,&QAction::triggered,this,&COptionsMotionWidget::onGroupListDelete);
    QAbstractItemDelegate::connect(ModifyAction,&QAction::triggered,this,&COptionsMotionWidget::onGroupModifyName);
    QAbstractItemDelegate::connect(CopyAction,&QAction::triggered,this,&COptionsMotionWidget::onGroupListCopy);
    QAbstractItemDelegate::connect(PasteAction,&QAction::triggered,this,&COptionsMotionWidget::onGroupListPaste);
    QAbstractItemDelegate::connect(mergeAction,&QAction::triggered,this,&COptionsMotionWidget::onGroupListMerge);


    //菜单出现的位置为当前鼠标的位置
    QFile styleSheet(":/res/qss/menudefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        pop_menu->setStyleSheet(styleSheet.readAll());
    }
    pop_menu->exec(QCursor::pos());
    pop_menu->deleteLater();
    //连接信号与槽
}

/**************************************************************************
* 函数名:CreateGridListPopMenuActions
* 功能:动作表鼠标右键菜单创建
* 参数:
* 返回值:
* 时间:2016/01/07
* 作者: 周志平
* 改动日志
**************************************************************************/
void COptionsMotionWidget::CreateGridListPopMenuActions()
{
    //创建菜单、菜单项
    QMenu* pop_menu = new QMenu();
    QAction* CopyAction = new QAction(tr("Copy"),this);
    QAction* pPasteAction = new QAction(tr("Paste"),this);
//    QAction* AddCopyAction = new QAction(tr("Add replication entry"),this);
    QAction* DeleteAction = new QAction(tr("Delete"),this);

    pop_menu->addAction(CopyAction);
    pop_menu->addAction(pPasteAction);
//    pop_menu->addAction(AddCopyAction);
    pop_menu->addAction(DeleteAction);

    //连接信号与槽
    QAbstractItemDelegate::connect(CopyAction,&QAction::triggered,this,&COptionsMotionWidget::onMotionFrameCopy);
    QAbstractItemDelegate::connect(pPasteAction,&QAction::triggered,this,&COptionsMotionWidget::onMotionFramePaste);
//    QAbstractItemDelegate::connect(AddCopyAction,&QAction::triggered,this,&COptionsMotionWidget::onMotionFramePasteToEnd);
    QAbstractItemDelegate::connect(DeleteAction,&QAction::triggered,this,&COptionsMotionWidget::onMotionFrameDelete);

    int nCurRow = m_pGridTable->currentRow();
    bool bSelectFullLine = IsFullRowSelect(nCurRow);
    if(!bSelectFullLine)
    {
        CopyAction->setEnabled(false);
        pPasteAction->setEnabled(false);
        DeleteAction->setEnabled(false);
    }

    if (isMotionSimulating())
    {
        CopyAction->setEnabled(false);
        pPasteAction->setEnabled(false);
//        AddCopyAction->setEnabled(false);
        DeleteAction->setEnabled(false);
    }

//    const QClipboard *clipboard = QApplication::clipboard();
//    const  QMimeData* mimeData = clipboard->mimeData(QClipboard::Clipboard);
//    if(mimeData != NULL)
//    {
//        QByteArray bArray = mimeData->data(KEY_MINEDATA_COPYDATA);
//        int nTotalLen = bArray.length();
//        if(nTotalLen <= 0)
//        {
//            InsertCopyAction->setEnabled(false);
//            AddCopyAction->setEnabled(false);
//        }
//    }

    if(!isCopyFrame())
    {
        pPasteAction->setEnabled(false);
//        AddCopyAction->setEnabled(false);
    }


    ////////////////for range select
    const QList<QTableWidgetSelectionRange> ranges= m_pGridTable->selectedRanges();
    bool bIsContinueRange = isContinueRange(ranges);
    if(bIsContinueRange)
    {
        CopyAction->setEnabled(true);

        ActItemCopyData copyData;
        getRangeData(copyData, ranges, true);
        if((copyData.nRow == m_ActItemCopyData.nRow)
                && (copyData.nColumn == m_ActItemCopyData.nColumn)
                && (m_ActItemCopyData.listItem.size() > 0))
        {
            pPasteAction->setEnabled(true);


            ////////////////////////////
            QTableWidgetItem *item = m_pGridTable->item(1,2);
            QRect rect = m_pGridTable->visualItemRect(item);
            //1、画字
            QPainter painter(m_pGridTable);
            painter.setPen(Qt::red);
            painter.drawRect(rect);
            ///////////////////////////
        }
    }

    //菜单出现的位置为当前鼠标的位置
    QFile styleSheet(":/res/qss/menudefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        pop_menu->setStyleSheet(styleSheet.readAll());
    }

    pop_menu->exec(QCursor::pos());
    pop_menu->deleteLater();
}

/**************************************************************************
* 函数名:onGroupListDelete
* 功能:删除动作组
* 参数:
* 返回值:
* 时间:2016/01/24 20:36
* 作者: 周志平
* 改动日志
**************************************************************************/
void COptionsMotionWidget::onGroupListDelete()
{
    //    QListWidgetItem* pItem = m_pGroupListWidget->currentItem();
    int nIndex = m_pGroupListWidget->currentRow();
    if(nIndex == -1)
    {
        return;
    }
    else
    {
        //        int nIndex = pItem->data(Qt::DecorationRole).toInt();
        CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(nIndex);
        if(pGroup == NULL)
        {
            qDebug()<<tr("new pGroup Fail in COptionsMotionWidget class")<<endl;
            return;
        }
        int nStartTime = pGroup->GetStartTime();
        m_pEvent->ILayerOptionHandler_SetSelectGroupBlock(nStartTime + 2);
    }
    m_pEvent->ILayerOptionHandler_OnDeleteGroupBlock();
    m_pEvent->ILayerOptionHandler_HasMotionData(m_pGroupListWidget->count() > 0);
    if(m_pGroupListWidget->count() > 0)
    {
        ui->btnUnitedSimulation->setEnabled(true);
        ui->btnStop->setEnabled(true);
        ui->pbInsert->setEnabled(true);
        ui->pbInsertSplit->setEnabled(true);
        ui->pbModify->setEnabled(true);
    }
    else
    {
        ui->btnUnitedSimulation->setEnabled(false);
        ui->btnStop->setEnabled(false);
        ui->pbInsert->setEnabled(false);
        ui->pbInsertSplit->setEnabled(false);
        ui->pbModify->setEnabled(false);
    }
}

void COptionsMotionWidget::updateConfigRuntime(int nTimeMs)
{
    ui->leBeatValue->setText(QString("%1").arg(nTimeMs));
}

void COptionsMotionWidget::cloneBeforeChangedMotionData(IOperation *pHandle)
{
    if (!m_pGroupManager || !pHandle)
    {
        return;
    }

    pHandle->cloneDataBeforeChanged((LPARAM)m_pGroupManager, getCurrentSelTimeCol());
    return;
}

void COptionsMotionWidget::cloneAfterChangedMotionData(IOperation *pHandle)
{
    if (!pHandle || !m_pEvent)
    {
        return;
    }

    pHandle->cloneDataAfterChanged((LPARAM)m_pGroupManager, getCurrentSelTimeCol());
}

/**************************************************************************
 * 函数名: getCurrentSelTimeCol
 * 功能: 获取当前时间，以粒度为单位
 * 参数:
 * 返回值: void
 * 时间: 2017/03/17
 * 作者: hwx
 */
quint64 COptionsMotionWidget::getCurrentSelTimeCol()
{
    IUILayerManager *pUILayerMgr = getLayerManger();
    if (!pUILayerMgr)
    {
        return 0;
    }

    return pUILayerMgr->GetCurrentSelectCol();
}

IUILayerManager *COptionsMotionWidget::getLayerManger()
{
    if (!m_pEvent)
    {
        return NULL;
    }

    return m_pEvent->ILayerOptionHandler_GetLayerManager();
}

void COptionsMotionWidget::resetPlayTime()
{
    ui->labelMusicTime->setText(m_strTotalTime);
    ui->musicSlider->setSliderPosition(0);
}

/**************************************************************************
* 函数名:onGroupModifyName
* 功能:修改动作组名
* 参数:
* 返回值:
* 时间:2016/01/24 20:36
* 作者: 周志平
* 改动日志
**************************************************************************/
void COptionsMotionWidget::onGroupModifyName()
{
    CActionBlockGroupManager* pGroup = m_pGroupManager->FindBlockData(GetCurrenGroupManagerID());
    if(pGroup)
    {
        IOperation *pChangedMotionData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
        cloneBeforeChangedMotionData(pChangedMotionData);
        float fStartTime = pGroup->GetStartTime();
        m_pEvent->ILayerOptionHandler_ModifyGroupName(fStartTime +1);
        cloneAfterChangedMotionData(pChangedMotionData);
    }
}

/**************************************************************************
* 函数名:onGroupListCopy
* 功能:动作组拷贝
* 参数:
* 返回值:
* 时间:2016/01/24 20:36
* 作者: 周志平
* 改动日志
**************************************************************************/

void COptionsMotionWidget::onGroupListCopy()
{
    m_pEvent->ILayerOptionHandler_CopyGroup();
}

/**************************************************************************
* 函数名:onGroupListPaste
* 功能:动作组粘贴
* 参数:
* 返回值:
* 时间:2016/01/24 20:36
* 作者: 周志平
* 改动日志
**************************************************************************/

void COptionsMotionWidget::onGroupListPaste()
{
    CActionBlockGroupManager* pGroup = m_pGroupManager->FindBlockData(GetCurrenGroupManagerID());
    if(pGroup)
    {
        int nStartTime = pGroup->GetStartTime();
        m_pEvent->ILayerOptionHandler_PasteGroup(nStartTime + 1);
    }
}

void COptionsMotionWidget::onGroupListMerge()
{
    if(m_pGroupManager->getGroupIDSelected().size() <= 1)
    {
        return;
    }

    // 保存历史数据
    IOperation* pChangedMotionData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
    cloneBeforeChangedMotionData(pChangedMotionData);

    g_IsModifyGroup = true;
    CActionBlockGroupManager* pGroupData = NULL;
    // 第一个合并组
    pGroupData = m_pGroupManager->FindBlockData(m_pGroupManager->getGroupIDSelectedFirst());
    float incTime = 0;

    QList<int> lstID = m_pGroupManager->getGroupIDSelected();
    for (int i = 1; i < lstID.size(); i++)
    {
        const int nGroupID = lstID.at(i);
        CActionBlockGroupManager* waitInsertGroup = m_pGroupManager->FindBlockData(nGroupID);
        int nIndexWaitInsertGroup = m_pGroupManager->getIndex(waitInsertGroup);
        CActionBlockGroupManager* pWaitMoveWhenDelectGroup = m_pGroupManager->GetAt(nIndexWaitInsertGroup + 1);

        float fStartTime = waitInsertGroup->GetStartTime();
        float fEndTime = waitInsertGroup->GetEndTime();

        float fDif = fEndTime - fStartTime;

        for (int j =0; j < waitInsertGroup->GetActionCount(); j++)
        {
            CActionBlockData* pActionBlockData = waitInsertGroup->GetAt(j);

            // 先NEW内存出来，然后复制过去
            CActionBlockData* pNewBlockData = new CActionBlockData();
            char* pData = NULL;
            int nBlockLen = 0;
            pActionBlockData->GetData(&pData, nBlockLen);
            if (pData && nBlockLen)
            {
                pNewBlockData->SetData(pData, nBlockLen);
                delete [] pData;
                pNewBlockData->SetID(pGroupData->GetUnUserID());
            }
            pGroupData->AddLayerBlock(pNewBlockData);
        }

        incTime += fDif;

//        // Modified Flag
//        if (m_pLayerManager)
//        {
//            m_pLayerManager->IUILayerManager_SetModifiedState(true);
//        }
        m_pGroupManager->DeleteLayerBlock(nGroupID);
        if(pWaitMoveWhenDelectGroup)
        {
            m_pGroupManager->MoveGroup(pWaitMoveWhenDelectGroup, 0 - fDif);
        }

        removeGroupListItem(nGroupID);
    }
    //pGroupData->SetEndTime(pGroupData->GetEndTime() + incTime);//time is err

    m_pEvent->ILayerOptionHandler_OnActionBlockDataChange(pGroupData->GetID(), pGroupData->GetAt(0)->GetID(), incTime);
    UpdateUIByData();
    g_IsModifyGroup = false;


    // 保存修改后的数据
    cloneAfterChangedMotionData(pChangedMotionData);
}

void COptionsMotionWidget::slotOnEditFrameRunTimeFinished()
{
    IOperation *pHandle = UBXUndoRedoManager::getInstance()->createOperation(Constants::eCodeHandleFrameRunTime);
    pHandle->cloneDataBeforeChanged(m_strFrameRunTime.toInt(), NULL);
    m_strFrameRunTime = ui->leBeatValue->text().trimmed();
    pHandle->cloneDataAfterChanged(m_strFrameRunTime.toInt(), NULL);
}

void COptionsMotionWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/**************************************************************************
* 函数名: on_twActionList_itemPressed
* 功能:动作组鼠标点击事件
* 参数:QTableWidgetItem *item
* 返回值:
* 时间:2016/01/24 20:36
* 作者: 周志平
* 改动日志
**************************************************************************/
void COptionsMotionWidget::on_twActionList_itemPressed(QTableWidgetItem *item)
{
    if(qApp->mouseButtons() == Qt::RightButton) //右键
    {
        if(item==Q_NULLPTR)
        {
            return;
        }
        else
        {

            CreateGridListPopMenuActions();
        }

    }
}

//hels 用于判断选中行，以便通知MP3模块切换选中项
void COptionsMotionWidget::onSectionClicked(int logicalIndex)
{    
    if(NULL == m_pGroupManager||
            NULL == m_pGridTable ||
            m_nCurrentGroupListSelectItem == -1)
    {
        return;
    }

    m_bLbtnDownDebug = true;
    QTableWidgetItem* pItem = m_pGridTable->item(logicalIndex, 0);
    if(NULL == pItem)
        return;
    int nActionBlockID = pItem->data(OptionItemRoles::ActionBlockDataID).toInt();
    if(nActionBlockID >= 0)
    {
        CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(m_nCurrentGroupListSelectItem);
        if(pGroup == NULL)
        {
            return;
        }

        if(m_pEvent)
        {
            m_pEvent->ILayerOptionHandler_OnSelectActionManageChange(pGroup->GetID(), logicalIndex);
        }
        m_nCol = nActionBlockID;
        if(NULL == m_pTimerClicked)
        {
            m_pTimerClicked = new QTimer(this);
            connect(m_pTimerClicked, &QTimer::timeout, this, &COptionsMotionWidget::OnTimeOut);
        }
        m_pTimerClicked->start(500);
    }
    m_ShowId.clear();
}

//hels 响应行号双击事件
void COptionsMotionWidget::onSectionDoubleClicked(int logicalIndex)
{
    if(NULL == m_pGroupManager||
       NULL == m_pGridTable)
        return;

    //先停止单击事件启动的定时器
    if(m_pTimerClicked)
    {
        m_pTimerClicked->stop();
    }

    m_bLbtnDownDebug = false;
    QTableWidgetItem* pItem = m_pGridTable->item(logicalIndex, 0);
    if(NULL == pItem)
        return;
    int nActionBlockID = pItem->data(OptionItemRoles::ActionBlockDataID).toInt();
    if(nActionBlockID >= 0)
    {
        m_nCol = nActionBlockID;
        PlayBlock(nActionBlockID);
    }
}

//hels 响应编辑Item信号
void COptionsMotionWidget::onItemCommitData(QWidget *editor)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
#if 1
    if(m_pGridTable && lineEdit)
    {
        int nRow = m_pGridTable->currentRow();
        int nColumn = m_pGridTable->currentColumn();
        modifyItemData(nRow, nColumn, true);
    }


    ///////////////////////////////////////
    if(m_bAutoModifyItemData)
    {
        changeItemColor(m_EditItemDefaultColor);
        m_bAutoModifyItemData = false;
    }
    ///////////////////////////////////////

    if(m_pGridTable)
    {
        int nRow = m_pGridTable->currentRow();
        int nColumn = m_pGridTable->currentColumn();
        setWarningItemValueInvalid(nRow, nColumn);
    }

#else
    if(m_pGridTable && lineEdit)
    {
        int nScaleTime = 10;//时间单位
        if (m_pEvent)
        {
            nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();
        }

        int nRow = m_pGridTable->currentRow();
        int nColumn = m_pGridTable->currentColumn();

        QTableWidgetItem* pItem0 = m_pGridTable->item(nRow, 0);
        int nActionBlockID = pItem0->data(OptionItemRoles::ActionBlockDataID).toInt();
        CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
        if (pGroupManager)
        {
            QTableWidgetItem* pItem = m_pGridTable->item(nRow, nColumn);
            CActionBlockData* pBlockData = pGroupManager->FindBlockData(nActionBlockID);
            if (pBlockData)
            {
                // 舵机数据
                if (nColumn < m_nRunTimeCol)
                {
                    QString strAngle = lineEdit->text();
                    if (strAngle.isEmpty())
                    {
                        strAngle.sprintf("%d",GRID_MIN_ANGLEVALUE);
                        pItem->setText(strAngle);
                    }
                    bool bOk = false;
                    int nAngle = strAngle.toInt(&bOk);                    

                    bool bRxsOk = true;
                    if (isDigitStr(strAngle) == -1)
                    {
                        bRxsOk = false;
                    }
                    if (bRxsOk)
                    {
                        if ((strAngle.indexOf('0') == 0) &&( strAngle.length()!= 1))
                        {
                            bRxsOk = false;
                        }
                    }

                    if (nAngle < GRID_MIN_ANGLEVALUE
                            || (nAngle > UBXBase::getInstance()->getDefaultActAngleValueMax(1))
                            || (bOk == false)
                            || (bRxsOk == false))
                    {
                        // 数据不合格，恢复数据
                        char* pData = NULL;
                        int nLen = 0;
                        pBlockData->GetExtBlockData(&pData, nLen);
                        if (pData && nLen)
                        {
                            MOTOR_DATA* pMoto = GetMotorData(pData, nLen, nColumn);
                            if (pMoto)
                            {
                                strAngle.sprintf("%d", pMoto->nAngle);
                                pItem->setText(strAngle);
                            }
                            delete [] pData;
                        }
                    }
                    else
                    {
                        // 更新最新的数据
                        char* pData = NULL;
                        int nLen = 0;
                        pBlockData->GetExtBlockData(&pData, nLen);
                        if (pData && nLen)
                        {
                            // 保存历史数据
                            IOperation *pChangedMotionData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
                            cloneBeforeChangedMotionData(pChangedMotionData);

                            // Modified Flag
                            SetMotorAngle(pData, nLen, nColumn, nAngle);
                            if (m_pEvent)
                            {
                                m_pEvent->ILayerOptionHandler_SetModifiedState(true);
                            }
                            pBlockData->SetExtBlockData(nLen, pData);
                            delete [] pData;

                            // 保存修改后的数据
                            cloneAfterChangedMotionData(pChangedMotionData);
                        }
                    }
                }
                else if (nColumn == m_nRunTimeCol
                         || nColumn == m_nAllTimeCol)//修改了运动时间或总时间
                {
                    bool bIsRunTimeCol = (nColumn == m_nRunTimeCol);

                    QString sTempTime = lineEdit->text();
                    if (sTempTime.isEmpty())
                    {
                        sTempTime.sprintf("%d", GRID_MIN_TIMEVALUE);
                        pItem->setText(sTempTime);
                    }

                    bool bOk = false;
                    float fRealTime = sTempTime.toFloat(&bOk);
                    if (!bOk || fRealTime > GRID_MAX_TIMEVALUE || fRealTime  < GRID_MIN_TIMEVALUE)
                    {
                        //四舍五入
                        int nTempTime = (bIsRunTimeCol ? pBlockData->GetRunTime() : pBlockData->GetAllTime()) * nScaleTime + 0.5;
                        sTempTime.sprintf("%d", nTempTime);
                        pItem->setText(sTempTime);
                    }

                    QString sRunTime = m_pGridTable->item(nRow, m_nRunTimeCol)->text();
                    QString sAllTime = m_pGridTable->item(nRow, m_nAllTimeCol)->text();

                    if(sRunTime.toFloat() > sAllTime.toFloat())
                    {
                        if(bIsRunTimeCol)
                        {
                            if(sRunTime.toFloat() <= GRID_MAX_TIMEVALUE)
                            {
                                QTableWidgetItem* pItemAllTime = m_pGridTable->item(nRow, m_nAllTimeCol);
                                pItemAllTime->setText(sRunTime);//设置总时间为运行时间
                            }
                            else
                            {
                                //四舍五入
                                int nTempTime = pBlockData->GetRunTime() * nScaleTime + 0.5;
                                sTempTime.sprintf("%d", nTempTime);
                                pItem->setText(sTempTime);
                            }
                        }
                        else
                        {
                            if(sAllTime.toFloat() <= GRID_MAX_TIMEVALUE)
                            {
                                QTableWidgetItem* pItemRunTime = m_pGridTable->item(nRow, m_nRunTimeCol);
                                pItemRunTime->setText(sAllTime);//设置运行时间为总时间
                            }
                            else
                            {
                                //四舍五入
                                int nTempTime = pBlockData->GetAllTime() * nScaleTime + 0.5;
                                sTempTime.sprintf("%d", nTempTime);
                                pItem->setText(sTempTime);
                            }
                        }
                    }

                    //前面判断运行时间和总时间时有更改数据，因此这里重新获取一次
                    sRunTime = m_pGridTable->item(nRow, m_nRunTimeCol)->text();
                    sAllTime = m_pGridTable->item(nRow, m_nAllTimeCol)->text();


                    float fRunTime = sRunTime.toFloat();
                    float fAllTime = sAllTime.toFloat();
                    float fAllTimeInc = fAllTime / nScaleTime - pBlockData->GetAllTime();//改变的时间

                    // 保存历史数据
                    IOperation *pChangedMotionData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
                    cloneBeforeChangedMotionData(pChangedMotionData);

                    pBlockData->SetRunTime(fRunTime / nScaleTime);
                    pBlockData->SetAllTime(fAllTime / nScaleTime);

                    if(m_pEvent)
                    {
                        m_pEvent->ILayerOptionHandler_OnActionBlockDataChange(m_nCurrentGroupID, nActionBlockID, fAllTimeInc);//分组总时间改变通知
                    }

                    // 保存修改后的数据
                    cloneAfterChangedMotionData(pChangedMotionData);
                }
            }
        }
    }
#endif

}

void COptionsMotionWidget::modifyItemData(int nRow, int nColumn, bool bSaveToRedo)
{
    if(m_pGridTable && (nRow >= 0) && (nColumn >= 0))
    {
        int nRowCount = m_pGridTable->rowCount();
        int nColumnCount = m_pGridTable->columnCount();
        if((nRow > nRowCount) || (nColumn > nColumnCount))
        {
            return ;
        }

        int nScaleTime = 10;//时间单位
        if (m_pEvent)
        {
            nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();
        }

        QTableWidgetItem* pItem0 = m_pGridTable->item(nRow, 0);
        int nActionBlockID = pItem0->data(OptionItemRoles::ActionBlockDataID).toInt();
        CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
        if (pGroupManager)
        {
            QTableWidgetItem* pItem = m_pGridTable->item(nRow, nColumn);
            CActionBlockData* pBlockData = pGroupManager->FindBlockData(nActionBlockID);
            if (pBlockData)
            {
                // 舵机数据
                if (nColumn < m_nRunTimeCol)
                {
                    QString strAngle = pItem->text();
                    if (strAngle.isEmpty())
                    {
                        strAngle.sprintf("%d",GRID_MIN_ANGLEVALUE);
                        pItem->setText(strAngle);
                    }
                    bool bOk = false;
                    int nAngle = strAngle.toInt(&bOk);
                    bool bRxsOk = true;
                    if (!CStringHandle::isDigitStr(strAngle))
                    {
                        bRxsOk = false;
                    }
                    if (bRxsOk)
                    {
                        if ((strAngle.indexOf('0') == 0) &&( strAngle.length()!= 1))
                        {
                            bRxsOk = false;
                        }
                    }

                    const int nID = nColumn + 1;

                    if (nAngle < GRID_MIN_ANGLEVALUE
                            || (nAngle > UBXBase::getInstance()->getDefaultActAngleValueMax(nID))
                            || (bOk == false)
                            || (bRxsOk == false))
                    {
                        // 数据不合格，恢复数据
                        char* pData = NULL;
                        int nLen = 0;
                        pBlockData->GetExtBlockData(&pData, nLen);
                        if (pData && nLen)
                        {
                            MOTOR_DATA* pMoto = GetMotorData(pData, nLen, nColumn);
                            if (pMoto)
                            {
                                strAngle.sprintf("%d", pMoto->nAngle);
                                pItem->setText(strAngle);
                            }
                            delete [] pData;
                        }
                    }
                    else
                    {
                        // 更新最新的数据
                        char* pData = NULL;
                        int nLen = 0;
                        pBlockData->GetExtBlockData(&pData, nLen);
                        if (pData && nLen)
                        {
                            IOperation *pChangedMotionData = NULL;
                            if(bSaveToRedo)
                            {
                                // 保存历史数据
                                pChangedMotionData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
                                cloneBeforeChangedMotionData(pChangedMotionData);
                            }

                            // Modified Flag
                            SetMotorAngle(pData, nLen, nColumn, nAngle);
                            if (m_pEvent)
                            {
                                m_pEvent->ILayerOptionHandler_SetModifiedState(true);
                            }
                            pBlockData->SetExtBlockData(nLen, pData);
                            delete [] pData;

                            if(bSaveToRedo)
                            {
                                // 保存修改后的数据
                                cloneAfterChangedMotionData(pChangedMotionData);
                            }
                        }
                    }
                }
                else if (nColumn == m_nRunTimeCol
                         || nColumn == m_nAllTimeCol)//修改了运动时间或总时间
                {
                    bool bIsRunTimeCol = (nColumn == m_nRunTimeCol);

                    QString sTempTime = pItem->text();
                    if (sTempTime.isEmpty())
                    {
                        sTempTime.sprintf("%d", GRID_MIN_TIMEVALUE);
                        pItem->setText(sTempTime);
                    }

                    bool bOk = false;
                    float fRealTime = sTempTime.toFloat(&bOk);
                    if (!bOk || fRealTime > GRID_MAX_TIMEVALUE || fRealTime  < GRID_MIN_TIMEVALUE)
                    {
                        //四舍五入
                        int nTempTime = (bIsRunTimeCol ? pBlockData->GetRunTime() : pBlockData->GetAllTime()) * nScaleTime + 0.5;
                        sTempTime.sprintf("%d", nTempTime);
                        pItem->setText(sTempTime);
                    }

                    QString sRunTime = m_pGridTable->item(nRow, m_nRunTimeCol)->text();
                    QString sAllTime = m_pGridTable->item(nRow, m_nAllTimeCol)->text();

                    if(sRunTime.toFloat() > sAllTime.toFloat())
                    {
                        if(bIsRunTimeCol)
                        {
                            if(sRunTime.toFloat() <= GRID_MAX_TIMEVALUE)
                            {
                                QTableWidgetItem* pItemAllTime = m_pGridTable->item(nRow, m_nAllTimeCol);
                                pItemAllTime->setText(sRunTime);//设置总时间为运行时间
                            }
                            else
                            {
                                //四舍五入
                                int nTempTime = pBlockData->GetRunTime() * nScaleTime + 0.5;
                                sTempTime.sprintf("%d", nTempTime);
                                pItem->setText(sTempTime);
                            }
                        }
                        else
                        {
                            if(sAllTime.toFloat() <= GRID_MAX_TIMEVALUE)
                            {
                                QTableWidgetItem* pItemRunTime = m_pGridTable->item(nRow, m_nRunTimeCol);
                                pItemRunTime->setText(sAllTime);//设置运行时间为总时间
                            }
                            else
                            {
                                //四舍五入
                                int nTempTime = pBlockData->GetAllTime() * nScaleTime + 0.5;
                                sTempTime.sprintf("%d", nTempTime);
                                pItem->setText(sTempTime);
                            }
                        }
                    }

                    //前面判断运行时间和总时间时有更改数据，因此这里重新获取一次
                    sRunTime = m_pGridTable->item(nRow, m_nRunTimeCol)->text();
                    sAllTime = m_pGridTable->item(nRow, m_nAllTimeCol)->text();


                    float fRunTime = sRunTime.toFloat();
                    float fAllTime = sAllTime.toFloat();
                    float fAllTimeInc = fAllTime / nScaleTime - pBlockData->GetAllTime();//改变的时间

                    IOperation *pChangedMotionData = NULL;
                    if(bSaveToRedo)
                    {
                        // 保存历史数据
                        pChangedMotionData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
                        cloneBeforeChangedMotionData(pChangedMotionData);
                    }

                    pBlockData->SetRunTime(fRunTime / nScaleTime);
                    pBlockData->SetAllTime(fAllTime / nScaleTime);

                    if(m_pEvent)
                    {
                        m_pEvent->ILayerOptionHandler_OnActionBlockDataChange(m_nCurrentGroupID, nActionBlockID, fAllTimeInc);//分组总时间改变通知
                    }

                    if(bSaveToRedo)
                    {
                        // 保存修改后的数据
                        cloneAfterChangedMotionData(pChangedMotionData);
                    }
                }
            }
        }
    }
}

//hels 单击Motion Action时，触发，通知3D模型改变舵机角度
void COptionsMotionWidget::OnTimeOut()
{
    if(m_pTimerClicked)
    {
        m_pTimerClicked->stop();
    }

    if(m_nCol >= 0)
    {
        ModleView(m_nCol);
    }
}

void COptionsMotionWidget::OnMotionPlayStop(emPlayStatus eStatus)
{
    //ui->btnUnitedSimulation->setText(tr("United Simulation"));
    ui->btnPause->hide();
    ui->btnUnitedSimulation->show();
    m_eTotalPlayStatus = eStatus;

    if (eStatus == ePlayStatusStop)
    {
        IUILayerManager *pLayerMgr = getLayerManger();
        if (!pLayerMgr)
        {
            return;
        }

        pLayerMgr->updateMotionWidget(0);

        //qDebug() << "COptionsMotionWidget::OnMotionPlayStop" << endl;
    }

    // 联合仿真时，动作播放完了，但是音乐没有播放完，此时不要发送停止播放的消息
//    // 暂停播放时不要发消息
//    if (eStatus == ePlayStatusStop)
//    {
//        NotifyObservers(NOTIFY_MSG_PLAY_MOTION, NULL, NULL, ePlayStatusStop);
//    }
}

void COptionsMotionWidget::OnSetControlState(bool controlState, int nLayerType)
{
    if(controlState)
    {
        ui->leBeatValue->setEnabled(true);
        ui->m_ListActionGroup->setEnabled(true);
        ui->twActionList->setEnabled(true);
        ui->pbAdd->setEnabled(true);
        ui->pbInsert->setEnabled(true);
        ui->pbInsertSplit->setEnabled(true);
        ui->pbModify->setEnabled(true);
    }
    else
    {
        ui->leBeatValue->setEnabled(false);
        ui->m_ListActionGroup->setEnabled(false);
        ui->twActionList->setEnabled(false);
        ui->pbAdd->setEnabled(false);
        ui->pbInsert->setEnabled(false);
        ui->pbInsertSplit->setEnabled(false);
        ui->pbModify->setEnabled(false);
    }
}

void COptionsMotionWidget::on_btnUnitedSimulation_clicked()
{
    if (m_pEvent)
    {
        emPlayType ePlayType = m_pEvent->ILayerOptionHandler_GetPlayType();
        if (ePlayType != ePlayTypeUnited)
        {
            NotifyObservers(NOTIFY_MSG_STOP_PLAY, NULL, NULL, ePlayType);
        }
    }

    if(m_eTotalPlayStatus == ePlayStatusPlaying)
    {
        //ui->btnUnitedSimulation->setText(tr("United Simulation"));
        ui->btnPause->hide();
        ui->btnUnitedSimulation->show();
        if(m_pEvent)
        {
            m_pEvent->ILayerOptionHanlder_PauseUnitedSimulation();
        }
        m_eTotalPlayStatus = ePlayStatusPause;
        NotifyObservers(NOTIFY_MSG_UNITED_PLAY, NULL, NULL, ePlayStatusPause);
    }
    else
    {
        if (m_eTotalPlayStatus != ePlayStatusPause)
        {
            on_btnStop_clicked();
        }

        //ui->btnUnitedSimulation->setText(tr("Pause"));
        ui->btnUnitedSimulation->hide();
        ui->btnPause->show();
        if(m_pEvent)
        {
            m_pEvent->ILayerOptionHanlder_UnitedSimulation();
        }
        m_eTotalPlayStatus = ePlayStatusPlaying;
        NotifyObservers(NOTIFY_MSG_UNITED_PLAY, NULL, NULL, ePlayStatusPlaying);
    }
}

void COptionsMotionWidget::on_btnStop_clicked()
{
    if(m_pEvent)
    {
        //ui->btnUnitedSimulation->setText(tr("United Simulation"));
        ui->btnPause->hide();
        ui->btnUnitedSimulation->show();
        m_pEvent->ILayerOptionHanlder_StopUnitedSimulation();
    }

    NotifyObservers(NOTIFY_MSG_UNITED_PLAY, NULL, NULL, ePlayStatusStop);
}

void COptionsMotionWidget::changeItemColor(Qt::GlobalColor color)
{
    if(m_pGridTable)
    {
        int nRow = m_pGridTable->rowCount();
        int nColumn = m_pGridTable->columnCount() - 2;
        for(int i = 0; i < nRow; i++)
        {
            for(int j = 0; j < nColumn; j++)
            {
                QTableWidgetItem* pItem = m_pGridTable->item(i, j);
                if(pItem)
                {
                    pItem->setTextColor(color);
                }
            }
        }
    }
}


void COptionsMotionWidget::autoModifyItemValue()
{
    if(m_pGridTable)
    {
        int nRow = m_pGridTable->rowCount();
        int nColumn = m_pGridTable->columnCount() - 2;
        for(int i = 0; i < nRow; i++)
        {
            for(int j = 0; j < nColumn; j++)
            {
                QTableWidgetItem* pItem = m_pGridTable->item(i, j);
                if(pItem)
                {
                    int nAngle = pItem->text().toInt();
                    pItem->setTextColor(m_EditItemDefaultColor);
                    const int nMaxAngle = UBXBase::getInstance()->getDefaultActAngleValueMax( j + 1);
                    if((nAngle < GRID_MIN_ANGLEVALUE)
                            || (nAngle > nMaxAngle))
                    {
                        pItem->setTextColor(m_EditItemAutoEditedColor);
                        if(nAngle < GRID_MIN_ANGLEVALUE)
                        {
                            nAngle = GRID_MIN_ANGLEVALUE;
                        }
                        if(nAngle > nMaxAngle)
                        {
                            nAngle = nMaxAngle;
                        }
                        pItem->setText(QString::number(nAngle));
                        modifyItemData(i, j, true);
                        m_bAutoModifyItemData = true;
                    }
                }
            }
        }
    }
}

void COptionsMotionWidget::warningItemValueInvalid()
{
    if(m_pGridTable)
    {
        m_bAutoModifyItemData = false;
        int nRow = m_pGridTable->rowCount();
        int nColumn = m_pGridTable->columnCount() - 2;
        for(int i = 0; i < nRow; i++)
        {
            for(int j = 0; j < nColumn; j++)
            {
                setWarningItemValueInvalid(i, j);
            }
        }
    }
}

void COptionsMotionWidget::setWarningItemValueInvalid(int nRow, int nColum)
{
    if((nRow < 0) || (nColum < 0))
    {
        return;
    }

    if( nColum > 15)
    {
        return;
    }

    QTableWidgetItem* pItem = m_pGridTable->item(nRow, nColum);
    if(pItem)
    {
        int nAngle = pItem->text().toInt();
        pItem->setTextColor(m_EditItemDefaultColor);
        const int nMaxAngle = UBXBase::getInstance()->getDefaultActAngleValueMax(nColum + 1);
        if((nAngle < GRID_MIN_ANGLEVALUE)
                || (nAngle > nMaxAngle))
        {
            pItem->setTextColor(m_EditItemInvalidColor);
        }
    }
}

bool COptionsMotionWidget::removeGroupListItem(int nGroupID)
{
    QListWidgetItem *pItem = getListWidgetItem(nGroupID);
    if(!pItem)
    {
        return false;
    }

    m_pGroupListWidget->removeItemWidget(pItem);
    m_pGroupListWidget->update();
    return true;
}

QListWidgetItem *COptionsMotionWidget::getListWidgetItem(int nGroupID)
{
    QListWidgetItem* pItemTemp = NULL;
    if(nGroupID < 0)
    {
        return pItemTemp;
    }

    const int nCountItem = m_pGroupListWidget->count();
    for(int i = 0; i< nCountItem; i++)
    {
        QListWidgetItem *pItem = m_pGroupListWidget->item(i);
        if (pItem)
        {
            QVariant valueID = pItem->data(OptionItemRoles::GroupID);
            const int nTempGroupID = valueID.toInt();
            if(nGroupID == nTempGroupID)
            {
                pItemTemp = pItem;
                return pItemTemp;
            }
        }
    }
    return pItemTemp;
}

void COptionsMotionWidget::slotOnAngleValueCheckWay(emAngleChectWayType nWayType)
{
    if(eAngleCheckWayAuto == nWayType)
    {
        autoModifyItemValue();
    }
    if(eAngleCheckWayManual == nWayType)
    {
        warningItemValueInvalid();
    }
}

void COptionsMotionWidget::onItemClicked(QTableWidgetItem *item)
{
    if(m_pGridTable && item)
    {
        m_pGridTable->editItem(item);
    }
}

ActItemCopyData::ActItemCopyData()
{
    initData();
}

ActItemCopyData::~ActItemCopyData()
{
    initData();
}

void ActItemCopyData::initData()
{
    nRow = 0;
    nColumn = 0;
    listItem.clear();
}


