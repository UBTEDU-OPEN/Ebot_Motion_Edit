/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：actioncommentwidget.cpp
* 创建时间：2016/03/14
* 文件标识：
* 文件摘要：动作评论控件
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/08/09
* 版本摘要：
*/

#include <QScrollBar>

#include "actioncommentwidget.h"
#include "ui_actioncommentwidget.h"
#include "commentitemdelegate.h"
#include "ubxactionlibconstants.h"
#include "actupdatemangr.h"
#include "loginlibmgr.h"
#include "configs.h"
#include "widget/messagebox.h"


RETRANSLATEUIFUNCIMPL(ActionCommentWidget)
ActionCommentWidget::ActionCommentWidget(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::ActionCommentWidget),
    m_nRequestId(-1),
    m_nCommitCommentTimerId(0),
    m_nActionId(0),
    m_nCommentTotalCount(0),
    m_nCommentTotalPage(0),
    m_nCurrentPage(0),
    m_pModel(NULL),
    m_pCommentWidget(NULL),
    m_pCommentTree(NULL)
{
    ui->setupUi(this);

    setUpUI();

    initUI();
}

ActionCommentWidget::~ActionCommentWidget()
{
    clearModel();
    delete ui;
}

/************************************************************************************
* 名称: requestActionComment
* 功能: 请求动作评论数据
* 参数：[in] nActionId: 动作id
*            [in] nPageId: 评论页id
* 返回:   void
* 时间:   2016/09/19
* 作者:   hwx
*/
void ActionCommentWidget::requestActionComment(int nActionId, int nPageId)
{
    if (g_actupdatemgr)
    {
        m_nActionId = nActionId;
        m_nCurrentPage = nPageId;
        m_nRequestId = g_actupdatemgr->queryActionComment(nActionId, nPageId, Constants::HTTP_DEFAULT_REQUEST_ID);
    }
}

/************************************************************************************
* 名称: resetUI
* 功能: 清除界面数据
* 参数：
* 返回:   void
* 时间:   2016/09/26
* 作者:   hwx
*/
void ActionCommentWidget::resetUI()
{
    ui->labelCommentCnt->setText("0");
    if (m_pCommentWidget)
    {
        m_pCommentWidget->resetUI();
    }
    clearModel();
}

/************************************************************************************
* 名称: slotOnReceivedCommentData
* 功能: 获取请求评论数据的槽函数
* 参数：[in] data：评论数据
* 返回:   void
* 时间:   2016/09/19
* 作者:   hwx
*/
void ActionCommentWidget::slotOnReceivedCommentData(const QByteArray &data)
{
    if (!parseAllCommentData(data))
    {
        return;
    }

    m_pCommentTree->expandAll();

    setTotalCommentCnt(m_nCommentTotalCount);
    return;
}

/************************************************************************************
* 名称: slotOnDownloadReplierIconDone
* 功能: 完成评论者头像下载的槽函数
* 参数：
* 返回:   void
* 时间:   2016/09/20
* 作者:   hwx
*/
void ActionCommentWidget::slotOnDownloadReplierIconDone()
{
   m_pCommentTree->repaint();
   m_pCommentTree->update();
}

/************************************************************************************
* 名称: slotOnAddCommentFinished
* 功能: 用户添加评论响应结果
* 参数：@[out ] nRequestId: 评论请求ID
* 参数：@[out ] baResponse: 评论应答数据
* 返回:   void
* 时间:   2016/09/26
* 作者:   hwx
*/
void ActionCommentWidget::slotOnAddCommentFinished(int nRequestId, const QByteArray baResponse)
{
    if (nRequestId == m_nRequestId)
    {
        killTimer(m_nCommitCommentTimerId);

        // 解决添加评论的应答，可能会提示错误，如输入敏感字符等
        if (!baResponse.isEmpty())
        {
            int nRespCode = getResponseStatus(baResponse);
            if (HTTPRespCodeOK != nRespCode)
            {
                if (HTTPRespCodeIllegalWord == nRespCode)
                {
                    CMessageBox::critical(this, tr("Error"), tr("Commit comment failed, illegal word is not allowed!"));
                }
                else
                {
                    CMessageBox::critical(this, tr("Error"), tr("Commit comment failed!"));
                }
                if (m_pCommentWidget)
                {
                    m_pCommentWidget->setCommentButtonState(true);
                }
                return;
            }
        }

        // 清空历史数据
        resetUI();

        // 添加评论成功后重新向后台请求数据，更新评论
        requestActionComment(m_nActionId, 1);
    }
}

void ActionCommentWidget::timerEvent(QTimerEvent *event)
{
    int nTimerId = event->timerId();
    killTimer(m_nCommitCommentTimerId);
    if (m_nCommitCommentTimerId == nTimerId)
    {
        if (m_pCommentWidget)
        {
            m_pCommentWidget->setCommentButtonState(true);
        }
        QMessageBox::critical(this, tr("Error"), tr("Commit comment timeout!"));
        return;
    }
}

/************************************************************************************
* 名称: replyHost
* 功能: 评论动作发布者
* 参数：@[in] strComment: 评论内容
* 返回:   void
* 时间:   2016/09/24
* 作者:   hwx
*/
void ActionCommentWidget::replyHost(const QString &strComment)
{
    if(!g_userinfo.m_bLogin) // 没有登录不能评论
    {
        NotifyObservers(NOTIFY_MSG_LOGIN_REQUEST, NULL, NULL, NULL);
    }

    if (!g_userinfo.m_bLogin)
    {
        return;
    }

    if (g_actupdatemgr)
    {
        m_nRequestId =  g_actupdatemgr->addActionComment(m_nActionId, g_actupdatemgr->GetUserID(), 0, strComment, Constants::eTypeReplyHost, Constants::HTTP_DEFAULT_REQUEST_ID);
        startCommitCommentTimer();
    }
}

/************************************************************************************
* 名称: replyComment
* 功能: 回复评论
* 参数：@[in] nCommentId: 被回复的评论id
* 参数：@[in] strComment: 评论内容
* 返回:   void
* 时间:   2016/09/24
* 作者:   hwx
*/
void ActionCommentWidget::replyComment(quint64 nCommentId, const QString &strComment)
{
    if(!g_userinfo.m_bLogin) // 没有登录不能评论
    {
        NotifyObservers(NOTIFY_MSG_LOGIN_REQUEST, NULL, NULL, NULL);
    }

    if (!g_userinfo.m_bLogin)
    {
        return;
    }

    if (g_actupdatemgr)
    {
        m_nRequestId = g_actupdatemgr->addActionComment(m_nActionId, g_actupdatemgr->GetUserID(), nCommentId, strComment, Constants::eTypeReplyComment, Constants::HTTP_DEFAULT_REQUEST_ID);
        startCommitCommentTimer();
    }
}

/************************************************************************************
* 名称: slotOnReplyClicked
* 功能: 点击恢复评论图标的槽函数
* 参数：@[in] nCommentId: 被回复的评论id
* 参数：@[in] strReplyToWho: 被回复的对象
* 返回:   void
* 时间:   2016/09/24
* 作者:   hwx
*/
void ActionCommentWidget::slotOnReplyClicked(quint64 nCommentId, const QString &strReplyToWho)
{
    emit sigOnReplyClicked();

    if (m_pCommentWidget)
    {
        m_pCommentWidget->setReplyToWho(nCommentId, strReplyToWho);
    }
}

/************************************************************************************
* 名称: slotOnComment
* 功能: 用户评论的槽函数
* 参数：@[out ] nCommentId: 被回复的评论Id
* 参数：@[out ] strComment: 回复的对象
* 返回:   void
* 时间:   2016/09/24
* 作者:   hwx
*/
void ActionCommentWidget::slotOnComment(quint64 nCommentId, const QString &strComment)
{
    // 被评论的id小于等于0，则认为是回复楼主
    if (nCommentId <= 0)
    {
        replyHost(strComment);
    }
    else
    {
        replyComment(nCommentId, strComment);
    }
}

/************************************************************************************
* 名称: slotOnCommentTreeScrollValueChanged
* 功能: 评论数滚动条值发生变化的槽函数
* 参数：@[out ] nValue：滚动条返回的值，介于minvalue~maxvalue之间
* 返回:   void
* 时间:   2016/10/14
* 作者:   hwx
*/
void ActionCommentWidget::slotOnCommentTreeScrollValueChanged(int nValue)
{
    QScrollBar *pScrollBar = qobject_cast<QScrollBar*>(sender());
    if (!pScrollBar)
    {
        return;
    }

    int nPageStep = pScrollBar->singleStep();
    int nMaxValue = pScrollBar->maximum();

    // 当滚动条滚动到底部时，才请求下一页评论数据
    if (nValue < nMaxValue)
    {
        return;
    }

    // 如果没有请求到最后一页，则请求下一页评论
    if (m_nCurrentPage < m_nCommentTotalPage)
    {
        m_nCurrentPage++;  // 加锁？？？
        requestActionComment(m_nActionId, m_nCurrentPage);
    }
}

/************************************************************************************
* 名称: setUpUI
* 功能: 构建界面
* 返回:   void
* 时间:   2016/09/19
* 作者:   hwx
*/
void ActionCommentWidget::setUpUI()
{
    m_pCommentTree = new CCommentTreeView(this);
    CommentItemDelegate* pItemDelegate = new CommentItemDelegate(m_pCommentTree);
    m_pCommentTree->setRootIsDecorated(false);
    m_pCommentTree->setSelectionMode(QAbstractItemView::NoSelection);
    m_pCommentTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pCommentTree->setHeaderHidden(true);
    m_pCommentTree->setFocusPolicy(Qt::NoFocus);
    m_pCommentTree->setItemDelegate(pItemDelegate);
    m_pCommentTree->setMouseTracking(true);
    m_pCommentTree->resizeColumnToContents(0);
    ui->horizontalLayoutAllComment->addWidget(m_pCommentTree);

    connect(pItemDelegate, &CommentItemDelegate::sigOnReplyClicked, this, &ActionCommentWidget::slotOnReplyClicked);

    QScrollBar *pScrollBar = m_pCommentTree->verticalScrollBar();
    connect(pScrollBar, &QScrollBar::valueChanged, this, &ActionCommentWidget::slotOnCommentTreeScrollValueChanged);

    m_pCommentWidget = new CCommentWidget(this);
    ui->horizontalLayoutComment->addWidget(m_pCommentWidget);

    connect(m_pCommentWidget, &CCommentWidget::sigOnComment, this, &ActionCommentWidget::slotOnComment);
}

/************************************************************************************
* 名称: initUI
* 功能: 初始化界面
* 返回:   void
* 时间:   2016/09/19
* 作者:   hwx
*/
void ActionCommentWidget::initUI()
{
    m_pCommentTree->setAutoScroll(false);
    m_pCommentTree->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_pModel = new QStandardItemModel(m_pCommentTree);
    m_pCommentTree->setModel(m_pModel);
}

/************************************************************************************
* 名称: clearModel
* 功能: 清除模型数据
* 返回:   void
* 时间:   2016/09/19
* 作者:   hwx
*/
void ActionCommentWidget::clearModel()
{
    if (m_pModel)
    {
        m_pModel->clear();
    }
}

/************************************************************************************
* 名称: startCommitCommentTimer
* 功能: 启动提交评论定时器
* 返回:   void
* 时间:   2016/09/19
* 作者:   hwx
*/
void ActionCommentWidget::startCommitCommentTimer()
{
    m_nCommitCommentTimerId = startTimer(Constants::COMMIT_COMMENT_TIMEOUT);
}

/************************************************************************************
* 函数名: parseAllCommentData
* 功能: 解析获取评论数据，保存到数据模型
* 参数:
*    @[in ] bytearrayComment: 评论数据
* 返回值: 解析成功或失败
* 时间: 2016/09/20 9:48
* 作者: hwx
*/
bool ActionCommentWidget::parseAllCommentData(const QByteArray &bytearrayComment)
{
    QString strError = "";
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(bytearrayComment, &jsonError);
    if(QJsonParseError::NoError != jsonError.error)
    {
        strError = QString("parseCommentData: Parse json string error.  【%1】%2").arg(jsonError.error).arg(jsonError.errorString());
        AlphaRobotLog::getInstance()->printError(strError);
        return false;
    }

    if(!parseDocument.isObject())
    {
        AlphaRobotLog::getInstance()->printError(QString("parseCommentData: json doc is not object"));
        return false;
    }

    QJsonObject objRoot = parseDocument.object();

    // 获取评论总数
    if (!objRoot.contains(NET_COMMENT_COUNT))
    {
        AlphaRobotLog::getInstance()->printError(QString("parseCommentData: Not contains [%1]").arg(NET_COMMENT_COUNT));
        return false;
    }
    m_nCommentTotalCount = objRoot.value(NET_COMMENT_COUNT).toInt();

    // 获取总页数
    if (!objRoot.contains(NET_COMMENT_TOTAL_PAGE))
    {
        AlphaRobotLog::getInstance()->printError(QString("parseCommentData: Not contains [%1]").arg(NET_COMMENT_TOTAL_PAGE));
        return false;
    }
    m_nCommentTotalPage = objRoot.value(NET_COMMENT_TOTAL_PAGE).toInt();

    // 获取models字段
    if (!objRoot.contains(NET_MODELS))
    {
        AlphaRobotLog::getInstance()->printError(QString("parseCommentData: Not contains [%1]").arg(NET_MODELS));
        return false;
    }

    QJsonValue valueModels = objRoot.value(NET_MODELS);
    if(!valueModels.isArray())
    {
        AlphaRobotLog::getInstance()->printError(QString("parseCommentData: [%1] is not array").arg(NET_MODELS));
        return false;
    }

    QStandardItem *pRootItem = m_pModel->invisibleRootItem();
    QJsonArray arrayModels = valueModels.toArray();
    for (int nId = 0; nId < arrayModels.size(); nId++)
    {
        QJsonValue valueCommentItem = arrayModels.at(nId);
        if (!valueCommentItem.isObject())
        {
            AlphaRobotLog::getInstance()->printError(QString("parseCommentData: json value valueComment is not object"));
            continue;
        }

        QJsonObject objCommentItem = valueCommentItem.toObject();
        if (!parseCommentItem(objCommentItem, pRootItem))
        {
            continue;
        }
    }
    return true;
}

/************************************************************************************
* 函数名: getResponseStatus
* 功能: 获取应答结果
* 参数:
*    @[in ] bytearrayData: 应答数据
* 返回值: Http应答码
* 时间: 2017/02/16
* 作者: hwx
*/
HTTPRespCode ActionCommentWidget::getResponseStatus(const QByteArray &bytearrayData)
{
    QString strError = "";
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(bytearrayData, &jsonError);
    if(QJsonParseError::NoError != jsonError.error)
    {
        strError = QString("getResponseStatus: Parse json string error.  【%1】%2").arg(jsonError.error).arg(jsonError.errorString());
        AlphaRobotLog::getInstance()->printError(strError);
        return HTTPRespCodeError;
    }

    if(!parseDocument.isObject())
    {
        AlphaRobotLog::getInstance()->printError(QString("getResponseStatus: json doc is not object"));
        return HTTPRespCodeError;
    }

    QJsonObject objRoot = parseDocument.object();

    int nRespCode = -1;

    // 返回状态
    if (objRoot.contains(NET_INFO))
    {
        nRespCode = objRoot.value(NET_INFO).toString().toInt();
    }
    AlphaRobotLog::getInstance()->printError(QString("getResponseStatus: Response Code = %1").arg(nRespCode));
    return (HTTPRespCode)nRespCode;
}

/************************************************************************************
* 函数名: parseCommentItem
* 功能: 解析获取评论数据项，保存到数据模型
* 参数:
*    @[in ] objCommentItem: 评论数据项
*    @[in ] pParentItem: 数据模型parent项指针
* 返回值: 解析成功或失败
* 时间: 2016/09/20 9:48
* 作者: hwx
*/
bool ActionCommentWidget::parseCommentItem(const QJsonObject &objCommentItem, QStandardItem *pParentItem)
{
    if (!pParentItem)
    {
        return false;
    }

    UserCommentItem commentItemData;

    // replyId
    if (objCommentItem.contains(NET_COMMENT_REPLY_ID))
    {
        commentItemData.setReplyId(objCommentItem.value(NET_COMMENT_REPLY_ID).toInt());
    }

    // hostName
    if (objCommentItem.contains(NET_COMMENT_REPLIER_NAME))
    {
        commentItemData.setReplierName(objCommentItem.value(NET_COMMENT_REPLIER_NAME).toString());
    }

    // replyTime
    if (objCommentItem.contains(NET_COMMENT_REPLY_TIME))
    {
        commentItemData.setReplyTime(objCommentItem.value(NET_COMMENT_REPLY_TIME).toString());
    }

    // replyDetail
    if (objCommentItem.contains(NET_COMMENT_REPLY_DETAIL))
    {
        commentItemData.setReplyMessage(objCommentItem.value(NET_COMMENT_REPLY_DETAIL).toString());
    }

    // hostIcon
    if (objCommentItem.contains(NET_COMMENT_REPLIER_ICON))
    {
        commentItemData.setReplierIcon(objCommentItem.value(NET_COMMENT_REPLIER_ICON).toString());
    }

    // replyTo
    if (objCommentItem.contains(NET_COMMENT_REPLY_TO))
    {
        commentItemData.setReplyToWho(objCommentItem.value(NET_COMMENT_REPLY_TO).toString());
    }

    // upReplyId
    if (objCommentItem.contains(NET_COMMENT_REPLIED_ID))
    {
        commentItemData.setRepliedCommentId(objCommentItem.value(NET_COMMENT_REPLIED_ID).toInt());
    }

    // 下载评论头像
    downloadReplierIcon(commentItemData);

    // replyList,
    QJsonArray arraySubComment;
    if (objCommentItem.contains(NET_COMMENT_REPLY_LIST))
    {
        QJsonValue valueReplyList = objCommentItem.value(NET_COMMENT_REPLY_LIST);
        if (!valueReplyList.isArray())
        {
            AlphaRobotLog::getInstance()->printError(QString("parseCommentItem: [%1] is not array").arg(NET_COMMENT_REPLY_LIST));
            return false;
        }

        arraySubComment = valueReplyList.toArray();
    }

    // 回复数
    commentItemData.setReplyCount(arraySubComment.size());

    QStandardItem* pCommentItem = new QStandardItem();
    pParentItem->appendRow(pCommentItem);
    pCommentItem->setData(Constants::COMMENT_ITEM_TYPE, Constants::CommentItemType);
    QVariant variantData;
    variantData.setValue(commentItemData);
    pCommentItem->setData(variantData, Constants::CommentItemData);

    // replyList
    for (int nId = 0; nId < arraySubComment.size(); nId++)
    {
        QJsonValue valueCommentItem = arraySubComment.at(nId);
        if (!valueCommentItem.isObject())
        {
            AlphaRobotLog::getInstance()->printError(QString("parseCommentItem: json value valueComment is not object"));
            continue;
        }

        QJsonObject objCommentItem = valueCommentItem.toObject();
        if (!parseCommentItem(objCommentItem, pCommentItem))
        {
            continue;
        }
    }

    return true;
}

/************************************************************************************
* 函数名: downloadReplierIcon
* 功能: 下载评论者头像
* 参数:
*    @[in ] commentItem: 评论数据
* 返回值:
* 时间: 2016/09/20 9:48
* 作者: hwx
*/
void ActionCommentWidget::downloadReplierIcon(UserCommentItem &commentItem)
{
    if (g_actupdatemgr)
    {
        QString strUrl = commentItem.getReplierIcon();
        if (strUrl.isEmpty())
        {
            return;
        }

        // 设置本地文件路径，覆盖原来url
        QFileInfo fileInfo(strUrl);
        QString previewImageDir = CConfigs::instance()->getLocalPreviewImgDir();
        QString strLocalImgFielName = CConfigs::instance()->getFilePath(previewImageDir, fileInfo.fileName());
        commentItem.setReplierIcon(strLocalImgFielName);

        g_actupdatemgr->downloadReplierIcon(strUrl);
    }
}

/************************************************************************************
* 函数名: setTotalCommentCnt
* 功能: 设置评论总数
* 参数:
*    @[in ] nCnt: 评论总数
* 返回值:
* 时间: 2016/09/20 9:48
* 作者: hwx
*/
void ActionCommentWidget::setTotalCommentCnt(int nCnt)
{
    ui->labelCommentCnt->setText(QString("%1").arg(nCnt));
}
