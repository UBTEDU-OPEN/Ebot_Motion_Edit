/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：cmyalpha.cpp
* 创建时间：2016/12/01
* 文件标识：
* 文件摘要：我的机器人控件
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/12/01
* 版本摘要：
*/

#include <QCheckBox>
#include <QPainter>
#include <QScrollBar>

#include "cmyalpha.h"
#include "widget/messagebox.h"
#include "alpharobotconst.h"
#include "ubxupdateadministrator.h"
#include "configs.h"
#include "global.h"
#include "UBXBase.h"


#include "ui_cmyalpha.h"


CMyAlpha::CMyAlpha(QWidget *parent) :
    UBXWidget(parent),
    m_nCurEnterRow(INT_INVALID_VALUE),
    m_pCheckBoxSelAll(NULL),
    m_pBtnDeleteItem(NULL),
    ui(new Ui::CMyAlpha)
{
    ui->setupUi(this);
//    resize(800, 600);

    applyUBXWinFlags(this);

    setupUI();
    init();

    setDefaultStyle(this, MYALPHA_QSS_PATH);

    if(g_pRobotController)
    {
        AddSubscribe(g_pRobotController); //添加通信消息订阅
    }

    QScrollBar *pScrollBar = ui->tableActionList->verticalScrollBar();
    if (pScrollBar)
    {
        connect(pScrollBar, &QScrollBar::valueChanged, this, &CMyAlpha::slotOnTableScrollValueChanged);
    }

    //setEnableMove(true);


    ui->pbUpdate->setPressInterval(5000);
    ui->pbUpdate->enablePressInterval(true);

    ui->pbDelete->setPressInterval(1000);
    ui->pbDelete->enablePressInterval(true);

    retranslateUi();
}

CMyAlpha::~CMyAlpha()
{
    m_qDeleteActionList.clear();
    m_qReqActionPaths.clear();

    delete ui;
}

void CMyAlpha::retranslateUi()
{
    ui->retranslateUi(this);

    ui->m_pTitleBar->setTitleLabel(tr("My Alpha"));

    //第一列给checkbox
    QStringList slstColNames;
//    slstColNames << tr("ID") << "" << tr("Action Name") << tr("Operate");
    slstColNames << "" << tr("Action Name") << tr("Operate");
    ui->tableActionList->setHorizontalHeaderLabels(slstColNames); //设置表头
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
* 时间: 2016/12/02
* 作者: hwx
*/
void CMyAlpha::OnNotify(const zxObserverSubjectBase *pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    zxEvent* pNewEvent = new zxEvent(nMsgID);
    pNewEvent->eEventType = zxEventTypeWindow;
    pNewEvent->lReceiver = (long)this;
    pNewEvent->dwMsgId = nMsgID;
    pNewEvent->wParam = wParam;
    pNewEvent->lParam = lParam;
    QCoreApplication::postEvent(this, pNewEvent);
}

/************************************
* 名称: requestRobotActionList
* 功能: 请求机器人动作列表
* 参数：
* 返回:   void
* 时间:   2016/12/01
* 作者:   hwx
************************************/
void CMyAlpha::requestRobotActionList()
{
    // 由于取数据时是pop出来的，因此每次都需要重装
    initReqActionPath();
    requestNextActionPath();
//    if (g_pRobotController)
//    {
//        QString strActionDir = CConfigs::getRobotDirMyDownload();
//        if (strActionDir.isEmpty())
//        {
//            return;
//        }
//        QByteArray baActionDir = strActionDir.toUtf8();
//        g_pRobotController->Req_RobotActionList(baActionDir.data(), baActionDir.size());
//    }
}

/************************************
* 名称: checkSystemEmbedUpdate
* 功能: 检查嵌入式版本更新
* 参数：
* 返回:   void
* 时间:   2016/12/01
* 作者:   hwx
************************************/
void CMyAlpha::checkSystemEmbedUpdate()
{
    UBXUpdateAdministrator::getInstance()->checkUpdateSystemsEmbedded(false);
}

/************************************
* 名称: slotOnGetSystemEmbedVersionInfo
* 功能: 获取嵌入式系统版本信息的槽函数
*     * 参数:
*    @[in ] strCurrentVersion: 当前版本
*    @[in ] strLatestVersion: 最新版本
*    @[in ] nStatus: 是否需要更新
* 返回:   void
* 时间:   2016/12/01
* 作者:   hwx
************************************/
void CMyAlpha::slotOnGetSystemEmbedVersionInfo(const QString &strCurrentVersion, const QString &strLatestVersion, int nStatus)
{
    ui->labCurVersionText->setText(strCurrentVersion);
    ui->labLatestVersionText->setText(strLatestVersion);

    if (nStatus == CHECKRESULT_NEEDUPDATE)
    {
        ui->pbUpdate->show();
    }
    else
    {
        ui->pbUpdate->hide();
    }
}

/**************************************************************************
* 函数名: slotOnPortOpenChanged
* 功能: 机器人连接状态变化的槽函数
*     * 参数:
*    @[in ] bConnected: 是否连接上机器人
*
* 时间: 2017/03/29
* 作者: hwx
*/
void CMyAlpha::slotOnPortOpenChanged(BOOL bConnected)
{
    if (!bConnected)
    {
        this->close();
    }
}

void CMyAlpha::customEvent(QEvent *event)
{
    zxEvent* pEvent = (zxEvent*)event;
    DWORD nMsgID = pEvent->dwMsgId;
    WPARAM wParam = pEvent->wParam;
    LPARAM lParam = pEvent->lParam;

    if (nMsgID == WM_MSG_GET_ROBOT_ACTION_LIST_RST)
    {
        processGetRobotActionListRsp(wParam, lParam);
        return;
    }
    else if (nMsgID == WM_MSG_DELETE_ROBOT_ACTION_RST)
    {
        processDeleteRobotActionRsp((emDeleteRobotActionRsp)wParam, (char*)lParam);
        return;
    }
    return;
}

void CMyAlpha::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

bool CMyAlpha::eventFilter(QObject *watched, QEvent *event)
{
    if (!watched || !event)
    {
        return false;
    }

    if (watched == ui->tableActionList)
    {
        if(event->type() == QEvent::Leave || event->type() == QEvent::HoverLeave)
        {
            if(m_nCurEnterRow != INT_INVALID_VALUE)
            {
                for(int col = 0; col < ui->tableActionList->columnCount(); col++)
                {
                    QTableWidgetItem* pItem = ui->tableActionList->item(m_nCurEnterRow, col);
                    if(pItem)
                    {
                        pItem->setBackground(COLOR_ITEM_HOVER_CLEAR);
                    }
                }
                m_nCurEnterRow = INT_INVALID_VALUE;
            }

            if (m_pBtnDeleteItem)
            {
                m_pBtnDeleteItem->hide();
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

/**************************************************************************
* 函数名: slotOnTableRowChanged
* 功能: 表格行数发生变化槽函数
*     * 参数:
*    @[in ] nRowCount: 当前行数
*
* 时间: 2016/12/02
* 作者: hwx
*/
void CMyAlpha::slotOnTableRowChanged(int nRowCount)
{
    updateSelAllCheckBoxState();
}

/**************************************************************************
* 函数名: slotOnRowCheckBoxStateChanged
* 功能: 行选择框状态发生变化
*     * 参数:
*    @[in ] state: 选中状态
*
* 时间: 2016/12/02
* 作者: hwx
*/
void CMyAlpha::slotOnRowCheckBoxStateChanged(int state)
{
    updateSelAllCheckBoxState();
}

/**************************************************************************
* 函数名: slotOnSelAllCheckBoxClicked
* 功能: 全选按钮状态发生变化
*     * 参数:
*    @[in ] checked: 选中状态
*
* 时间: 2016/12/02
* 作者: hwx
*/
void CMyAlpha::slotOnSelAllCheckBoxClicked(bool checked)
{
    Qt::CheckState state = Qt::Unchecked;
    if(checked)
    {
        state = Qt::Checked;
    }

    CTableWidget::setAllCheckBoxs(ui->tableActionList, MY_ALPHA_TABLE_CHECKBOX_COL, state);
}

/**************************************************************************
* 函数名: slotOnDeleteSelectItemsButtonClicked
* 功能: 点击表格下方的删除按钮，删除表格中选中的动作
*  参数:
* 时间: 2016/12/02
* 作者: hwx
*/
void CMyAlpha::slotOnDeleteSelectItemsButtonClicked()
{
    m_qDeleteActionList.clear();
     for (int row = 0; row  < ui->tableActionList->rowCount(); row++)
     {
         QCheckBox *pCheckBox = dynamic_cast<QCheckBox *>(ui->tableActionList->cellWidget(row, MY_ALPHA_TABLE_CHECKBOX_COL));
         if (!pCheckBox)
         {
             continue;
         }

         if (!pCheckBox->isChecked())
         {
             continue;
         }

         // 不直接从表格取数据，免得以后表格显示信息改变后需要调整
         QTableWidgetItem *pItem = ui->tableActionList->item(row, MY_ALPHA_TABLE_ACTION_NAME_COL);
         if (!pItem)
         {
             continue;
         }

         QString strActionName = pItem->data(Qt::UserRole).toString();
         if (strActionName.isEmpty())
         {
             continue;
         }

         m_qDeleteActionList.syncPush(strActionName);
     }

     // 发送指定删除机器人中的动作
     deleteOneActionInQueue();
}

/**************************************************************************
* 函数名: slotOnDeleteRowItemButtonClicked
* 功能: 点击表格某行中的删除按钮，删除指定行动作
*  参数:
* 时间: 2016/12/02
* 作者: hwx
*/
void CMyAlpha::slotOnDeleteRowItemButtonClicked()
{
    if (ui->tableActionList->rowCount() <= m_nCurEnterRow)
    {
        return;
    }

    // 不直接从表格取数据，免得以后表格显示信息改变后需要调整
    QTableWidgetItem *pItem = ui->tableActionList->item(m_nCurEnterRow, MY_ALPHA_TABLE_ACTION_NAME_COL);
    if (!pItem)
    {
        return;
    }

    QString strActionName = pItem->data(Qt::UserRole).toString();
    if (strActionName.isEmpty())
    {
        return;
    }

    m_qDeleteActionList.syncPush(strActionName);

    // 发送指定删除机器人中的动作
    deleteOneActionInQueue();
}

/**************************************************************************
* 函数名: slotOnTableCellEnter
* 功能: 光标进入指定单元格的槽函数
*     * 参数:
*    @[in ] nRow: 行
*    @[in ] nCol: 列
*
* 时间: 2016/12/02
* 作者: hwx
*/
void CMyAlpha::slotOnTableCellEnter(int nRow, int nCol)
{
//    // 如果在同一行滑动
//    if (m_nCurEnterRow == nRow)
//    {
//        return;
//    }

    // 重设hover行背景色
    resetHoverRow(nRow, nCol);

    // 鼠标滑过时显示操作按钮
    QTableWidgetItem *pItem = ui->tableActionList->item(nRow, MY_ALPHA_TABLE_OPERATE_COL);
    if(!pItem || !m_pBtnDeleteItem)
    {
        return;
    }

    int nHeaderHeight = ui->tableActionList->horizontalHeader()->height();

    m_nCurEnterRow = nRow;

    QRect itemRect = ui->tableActionList->visualItemRect(pItem);
    QPoint startsPt;
    startsPt.setX(itemRect.left() + (itemRect.width() - m_pBtnDeleteItem->width()) / 2);
    startsPt.setY(itemRect.top() + nHeaderHeight + (itemRect.height() - m_pBtnDeleteItem->height()) / 2);
    m_pBtnDeleteItem->setGeometry(startsPt.x(), startsPt.y(), m_pBtnDeleteItem->width(), m_pBtnDeleteItem->height());

    m_pBtnDeleteItem->show();
}

/**************************************************************************
* 函数名: slotOnUpdateButtonClicked
* 功能: 更新版本槽函数
* 参数:
* 时间: 2016/12/02
* 作者: hwx
*/
void CMyAlpha::slotOnUpdateButtonClicked()
{
    UBXUpdateAdministrator::getInstance()->updateSystemEmbedVersion();
}

/**************************************************************************
* 函数名: slotOnTableScrollValueChanged
* 功能: 表格滚动条变化的槽函数
* 参数:
* 时间: 2017/04/05
* 作者: hwx
*/
void CMyAlpha::slotOnTableScrollValueChanged(int nValue)
{
    slotOnTableCellEnter(m_nCurEnterRow, 0);
}

/**************************************************************************
* 函数名: setUpUI
* 功能: 处理机器人动作列表
*     * 参数:
*
* 时间: 2016/12/02
* 作者: hwx
*/
void CMyAlpha::setupUI()
{
//    //第一列给checkbox
//    QStringList slstColNames;
////    slstColNames << tr("ID") << "" << tr("Action Name") << tr("Operate");
//    slstColNames << "" << tr("Action Name") << tr("Operate");
//    ui->tableActionList->setHorizontalHeaderLabels(slstColNames); //设置表头

    ui->tableActionList->setColumnCount(MY_ALPHA_TABLE_COLUMN_NUMBER);

    ui->tableActionList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableActionList->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableActionList->setFrameShape(QFrame::NoFrame);
    ui->tableActionList->verticalHeader()->hide();
    ui->tableActionList->horizontalHeader()->setFixedHeight(34);
    ui->tableActionList->setShowGrid(false);
    ui->tableActionList->setMouseTracking(true);
    //ui->tableActionList->setAlternatingRowColors(true);
    ui->tableActionList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableActionList->setFocusPolicy(Qt::NoFocus); // 去掉选择虚线框
//    ui->tableActionList->setSelectionMode(QAbstractItemView::ExtendedSelection); // 设置后选中单元格，光标滑开后字体颜色不是qss设置的颜色!!!!
    ui->tableActionList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//去除水平滚动条
    ui->tableActionList->setColumnWidth(MY_ALPHA_TABLE_CHECKBOX_COL, MY_ALPHA_LIST_CHECKBOX_COL_WIDTH);
    ui->tableActionList->setColumnWidth(MY_ALPHA_TABLE_ACTION_NAME_COL, MY_ALPHA_LIST_ACTION_NAME_COL_WIDTH);
    ui->tableActionList->setColumnWidth(MY_ALPHA_TABLE_OPERATE_COL, MY_ALPHA_LIST_OPERATION_COL_WIDTH);
    ui->tableActionList->startRowChangeTimer(1000);
    ui->tableActionList->installEventFilter(this);

    connect(ui->tableActionList, &CTableWidget::sigRowCountChanged, this, &CMyAlpha::slotOnTableRowChanged);
    connect(ui->tableActionList, &CTableWidget::cellEntered, this, &CMyAlpha::slotOnTableCellEnter);

    QHeaderView* pHeaderView =  ui->tableActionList->horizontalHeader();
    pHeaderView->setDefaultAlignment(Qt::AlignCenter);

    //单独设置文件列左对齐
   // ui->tableActionList->model()->setHeaderData(MY_ALPHA_TABLE_ACTION_NAME_COL, Qt::Horizontal, Qt::AlignLeft, Qt::TextAlignmentRole);

    m_pCheckBoxSelAll = new QCheckBox(pHeaderView);
    m_pCheckBoxSelAll->setObjectName(QStringLiteral("checkBox_selall"));
    connect(m_pCheckBoxSelAll, &QCheckBox::clicked, this, &CMyAlpha::slotOnSelAllCheckBoxClicked);
    QModelIndex indexColSelAll = pHeaderView->model()->index(0, MY_ALPHA_TABLE_CHECKBOX_COL);
    pHeaderView->setIndexWidget(indexColSelAll, m_pCheckBoxSelAll);

    ui->labAlaphaRobot->setScaledContents(true);

    m_pBtnDeleteItem = new QPushButton(ui->tableActionList);
    m_pBtnDeleteItem->setObjectName(QStringLiteral("pbActionItemDelete"));
    m_pBtnDeleteItem->setFixedSize(BTN_DEFAULT_WITH, BTN_DEFAULT_HEIGHT);
    m_pBtnDeleteItem->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pBtnDeleteItem->hide();
    connect(m_pBtnDeleteItem, &QPushButton::clicked, this, &CMyAlpha::slotOnDeleteRowItemButtonClicked);

    connect(ui->pbDelete, &QPushButton::clicked, this, &CMyAlpha::slotOnDeleteSelectItemsButtonClicked);
    connect(ui->pbUpdate, &QPushButton::clicked, this, &CMyAlpha::slotOnUpdateButtonClicked);

    connect(ui->m_pTitleBar, &MyTitleBar::sigClose, this, &CMyAlpha::close);
}

void CMyAlpha::init()
{
    m_qDeleteActionList.setCapacity(MAX_DELETE_ACTION_NUMBER);
    setDeleteButtonState(false);
    ui->pbUpdate->hide();

    setEnableMove(true);

    ui->labCurVersionText->setText(tr("Unknown"));
    ui->labLatestVersionText->setText(tr("Unknown"));

    loadAlphaRobotImage();
}

void CMyAlpha::initReqActionPath()
{
    m_qReqActionPaths.push(CConfigs::getRobotDirAction());
    m_qReqActionPaths.push(CConfigs::getRobotDirMyDownload());
}

/**************************************************************************
* 函数名: processGetRobotActionListRsp
* 功能: 处理机器人动作列表
*     * 参数:
*    @[in ] wParam: 参数1，true/false
*    @[in ] lParam: 参数2，动作名称
*
* 时间: 2016/12/02
* 作者: hwx
*/
void CMyAlpha::processGetRobotActionListRsp(WPARAM wParam, LPARAM lParam)
{
    // 主要包括三种情况：
    // 1、wParam = true, lParam = 动作名称     正在接收机器人动作
    // 2、wParam = true，lParam = NULL         正常接收结束
    // 3、wParam = false，lParam = NULL       读取机器人动作失败

    if (!wParam)
    {
        CMessageBox::critical(this, tr("Error"), tr("Get robot action list failed!"));
        return;
    }

    char *pActionName = (char *) lParam;
    if (!pActionName)
    {
        updateTable();
        AlphaRobotLog::getInstance()->printDebug(QString("Get robot action list from %1 success!").arg(m_strCurReqActionPath));
        requestNextActionPath();
        return;
    }

    QString strActionName(pActionName);

    // 需要拼成完整动作路径，由于嵌入式是unix系统，因此使用"/"
  //  QString strActionFilePath = CConfigs::getFilePath(m_strCurReqActionPath, strActionName); // m_strCurReqActionPath + "/" + strActionName;
    QString strActionFilePath = QDir::fromNativeSeparators(CConfigs::getFilePath(m_strCurReqActionPath, strActionName));

    addActionToVector(strActionFilePath);
    //AlphaRobotLog::getInstance()->printDebug(QString("processRobotActionList: Get robot action %1").arg(strActionName));

    SAFE_DELETE(pActionName);
}

/**************************************************************************
* 函数名: processDeleteRobotActionRsp
* 功能: 处理删除机器人动作响应
*     * 参数:
*    @[in ] eRsp: 响应类型
*    @[in ] pActionName: 动作名称
* 时间: 2016/12/02
* 作者: hwx
*/
void CMyAlpha::processDeleteRobotActionRsp(emDeleteRobotActionRsp eRsp, char * pActionName)
{
    QString strAction(pActionName);
    switch (eRsp) {
    case eDeleteRobotActionRspOk:
    {
        AlphaRobotLog::getInstance()->printDebug(QString("Delete action %1 successfully").arg(strAction));

        // 从数据模型中删除数据
        deleteActionFromVector(strAction);

        // 刷新界面
        updateTable();

        break;
    }
    case eDeleteRobotActionRspFailed:
    {
        AlphaRobotLog::getInstance()->printDebug(QString("Delete action  failed: %1").arg(strAction));
        CMessageBox::critical(this, tr("Error"), tr("Delete action  failed!"));
        break;
    }
    case eDeleteRobotActionRspFileNotExist:
    {
        AlphaRobotLog::getInstance()->printDebug(QString("Delete action failed. Action not exist: %1").arg(strAction));
        CMessageBox::critical(this, tr("Error"), tr("Delete action failed. Action not exist!"));
        break;
    }
    case eDeleteRobotActionRspActionIsPlaying:
    {
        AlphaRobotLog::getInstance()->printDebug(QString("Delete action failed. Action is playing: %1").arg(strAction));
        CMessageBox::critical(this, tr("Error"), tr("Delete action failed. Action is playing!"));
        break;
    }
    case eDeleteRobotActionRspTimeout:
    {
        AlphaRobotLog::getInstance()->printDebug(QString("Delete action timeout: %1").arg(strAction));
        CMessageBox::critical(this, tr("Error"), tr("Delete action timeout!"));
        break;
    }
    case eDeleteRobotActionRspNullFile:
    {
        AlphaRobotLog::getInstance()->printDebug("Delete action file failed, null file");
        break;
    }
    default:
        break;
    }

    // 按顺序删除下一个动作
    deleteOneActionInQueue();
}

/************************************
* 名称: requestRobotAction
* 功能: 请求指定路径下的机器人动作列表
* 参数：[in]strPath: 路径
* 返回:   void
* 时间:   2016/12/01
* 作者:   hwx
************************************/
void CMyAlpha::requestRobotAction(const QString &strPath)
{
    m_strCurReqActionPath = strPath;
    if (g_pRobotController)
    {
        if (m_strCurReqActionPath.isEmpty())
        {
            return;
        }
        QByteArray baActionDir = m_strCurReqActionPath.toUtf8();

        emUbtProductType eProductType = UBXBase::getInstance()->getCurrentRobotProductType();
        if(eUbtProduct1E == eProductType)
        {
            g_pRobotController->Req_RobotResInfos(eResInfoTypeActList, eResInfoCmdFlagRestartGetInfo, baActionDir.data(), baActionDir.size());
        }
        else
        {
             g_pRobotController->Req_RobotActionList(baActionDir.data(), baActionDir.size());
        }
    }
}

/************************************
* 名称: requestNextActionPath
* 功能: 请求读取下一个机器人动作路径
* 参数：
* 返回:   void
* 时间:   2016/12/01
* 作者:   hwx
************************************/
void CMyAlpha::requestNextActionPath()
{
    if (m_qReqActionPaths.size() <= 0)
    {
        return;
    }

    QString strActionPath = "";
    if (!m_qReqActionPaths.pop(strActionPath))
    {
        AlphaRobotLog::getInstance()->printDebug("Pop action path to read failed");
        return;
    }

    if (strActionPath.isEmpty())
    {
        return;
    }

    requestRobotAction(strActionPath);
    AlphaRobotLog::getInstance()->printDebug(QString("Start request action from path %1").arg(m_strCurReqActionPath));
}

void CMyAlpha::loadAlphaRobotImage(const QString &strImageFile)
{
    QString sImageFile = strImageFile;
    QPixmap pixmapAlphaRobot;
    if (!sImageFile.isEmpty() && !pixmapAlphaRobot.load(sImageFile))
    {
        return;
    }
    else if (!pixmapAlphaRobot.load(":/res/images/myalpha.png"))
    {
        return;
    }

    pixmapAlphaRobot.scaled(ui->labAlaphaRobot->height(), ui->labAlaphaRobot->width(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->labAlaphaRobot->setPixmap(pixmapAlphaRobot);
}

void CMyAlpha::addActionToVector(const QString &strName)
{
    if (m_lstActions.contains(strName))
    {
        return;
    }

    m_lstActions.append(strName);
}

void CMyAlpha::deleteActionFromVector(const QString &strAction)
{
    if (m_lstActions.contains(strAction))
    {
        m_lstActions.removeOne(strAction);
    }
}

void CMyAlpha::deleteOneActionInQueue()
{
    if (m_qDeleteActionList.size() <= 0)
    {
        return;
    }

    QString strAction = "";
    if (!m_qDeleteActionList.pop(strAction))
    {
        AlphaRobotLog::getInstance()->printDebug("Pop action to delete failed");
        return;
    }

    if (strAction.isEmpty())
    {
        return;
    }

    deleteActionItem(strAction);
}

void CMyAlpha::deleteActionItem(const QString &strAction)
{
    // 从机器人中删除动作
    if (g_pRobotController)
    {
        QByteArray baActionName = strAction.toUtf8().data();
        g_pRobotController->Req_DeleteRobotAction(baActionName.data(), baActionName.size());
    }
}

void CMyAlpha::updateTable()
{
    ui->tableActionList->clearContents();
    ui->tableActionList->setRowCount(0);

    qSort(m_lstActions.begin(), m_lstActions.end());
    for (int n = 0; n < m_lstActions.size(); n++)
    {
        QString strActionPath = m_lstActions[n];
        if (strActionPath.isEmpty())
        {
            continue;
        }

        addActionToTable(strActionPath);
    }

    ui->labActionCnt->setText(tr("Total action count ") + QString("%1").arg(m_lstActions.size()));
}

void CMyAlpha::addActionToTable(const QString &strAction)
{
    if (strAction.isEmpty())
    {
        return;
    }

    int nRowCount = ui->tableActionList->rowCount();
    ui->tableActionList->insertRow(nRowCount);

    // checkbox
    QCheckBox *pCheckBox = new QCheckBox(this);
    ui->tableActionList->setCellWidget(nRowCount, MY_ALPHA_TABLE_CHECKBOX_COL, pCheckBox);
    connect(pCheckBox, &QCheckBox::stateChanged, this, &CMyAlpha::slotOnRowCheckBoxStateChanged);

//    // ID
//    QTableWidgetItem *pItemID = new QTableWidgetItem;
//    pItemID->setTextAlignment(Qt::AlignCenter);
//    pItemID->setText(QString("%1").arg(nRowCount + 1));
//    ui->tableActionList->setItem(nRowCount, MY_ALPHA_TABLE_ID_COL, pItemID);

    // Action Name
    QFont font = QApplication::font();
    font.setPixelSize(FONT_PIXEL_SIZE_12);

    QFileInfo fileInfo(strAction);

    QTableWidgetItem *pItemActionName = new QTableWidgetItem;
    pItemActionName->setFont(font);
    pItemActionName->setForeground(QColor("#d3d3d3"));
    pItemActionName->setText(fileInfo.baseName());
    pItemActionName->setData(Qt::UserRole, strAction);
    ui->tableActionList->setItem(nRowCount, MY_ALPHA_TABLE_ACTION_NAME_COL, pItemActionName);

    // 给操作按钮占位
    QTableWidgetItem *pItemOperate = new QTableWidgetItem;
    ui->tableActionList->setItem(nRowCount, MY_ALPHA_TABLE_OPERATE_COL, pItemOperate);
}

void CMyAlpha::updateSelAllCheckBoxState()
{
    Qt::CheckState eCheckState = CTableWidget::getAllCheckBoxCheckedState(ui->tableActionList, MY_ALPHA_TABLE_CHECKBOX_COL);
    bool bIsSelAll = true;
    bool bDeleteBtnEnable = false;
    switch (eCheckState) {
    case Qt::Unchecked:
    {
        bDeleteBtnEnable = false;
        bIsSelAll = false;
        break;
    }
    case Qt::PartiallyChecked:
    {
        bDeleteBtnEnable =true;
        bIsSelAll = false;
        break;
    }
    case Qt::Checked:
    {
        bDeleteBtnEnable =true;
        bIsSelAll = true;
        break;
    }
    default:
        break;
    }
    m_pCheckBoxSelAll->setChecked(bIsSelAll);
    setDeleteButtonState(bDeleteBtnEnable);
}


void CMyAlpha::resetData()
{
    ui->labCurVersionText->setText(tr("Unknown"));
    ui->labLatestVersionText->setText(tr("Unknown"));
    ui->pbUpdate->hide();
    setDeleteButtonState(false);

    ui->tableActionList->clearContents();
    ui->tableActionList->setRowCount(0);
    m_lstActions.clear();
//    m_qReqActionPaths.clear();
    m_qDeleteActionList.clear();
    m_strCurReqActionPath.clear();
}

void CMyAlpha::setDeleteButtonState(bool bEnable)
{
    ui->pbDelete->setEnabled(bEnable);
}

void CMyAlpha::resetHoverRow(int nRow, int nCol)
{
    if (m_nCurEnterRow == nRow)
    {
        return;
    }

    for(int col = 0; col < ui->tableActionList->columnCount(); col++)
    {
        //清除上一次hover状态
        QTableWidgetItem* pItem = ui->tableActionList->item(m_nCurEnterRow, col);
        if(pItem)
        {
            pItem->setBackground(COLOR_ITEM_HOVER_CLEAR);
        }

        //hover本次状态
        pItem = ui->tableActionList->item(nRow, col);
        if(pItem)
        {
            pItem->setBackground(COLOR_ITEM_HOVER);
        }
    }
}
