/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：actioncommentwidget.h
* 创建时间：2016/03/14
* 文件标识：
* 文件摘要：动作评论控件
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/08/09
* 版本摘要：
*/

#ifndef ACTIONCOMMENTWIDGET_H
#define ACTIONCOMMENTWIDGET_H

#include <QWidget>
#include <QStandardItemModel>

#include "datamodel/usercommentitem.h"
#include "ccommenttreeview.h"
#include "widget/ccommentwidget.h"
#include "ubxactionlibconstants.h"
#include "../zxLib/widget/ubxwidget.h"


using namespace Constants;

namespace Ui {
class ActionCommentWidget;
}

class ActionCommentWidget : public UBXWidget
{
    Q_OBJECT

public:
    explicit ActionCommentWidget(QWidget *parent = 0);
    ~ActionCommentWidget();

    /************************************************************************************
    * 名称: requestActionComment
    * 功能: 请求动作评论数据
    * 参数：[in] nActionId: 动作id
    *            [in] nPageId: 评论页id
    * 返回:   void
    * 时间:   2016/09/19
    * 作者:   hwx
    */
    void requestActionComment(int nActionId, int nPageId);

    /************************************************************************************
    * 名称: resetUI
    * 功能: 清除界面数据
    * 参数：
    * 返回:   void
    * 时间:   2016/09/26
    * 作者:   hwx
    */
    void resetUI();
    RETRANSLATEUIFUNC

public slots:

     /************************************************************************************
     * 名称: slotOnReceivedCommentData
     * 功能: 获取请求评论数据的槽函数
     * 参数：[in] data：评论数据
     * 返回:   void
     * 时间:   2016/09/19
     * 作者:   hwx
    */
     void slotOnReceivedCommentData(const QByteArray &data);

     /************************************************************************************
     * 名称: slotOnDownloadReplierIconDone
     * 功能: 完成评论者头像下载的槽函数
     * 参数：
     * 返回:   void
     * 时间:   2016/09/20
     * 作者:   hwx
    */
     void slotOnDownloadReplierIconDone();

     /************************************************************************************
     * 名称: slotOnAddCommentFinished
     * 功能: 用户添加评论响应结果
     * 参数：@[out ] nRequestId: 评论请求ID
     * 参数：@[out ] baResponse: 评论应答数据
     * 返回:   void
     * 时间:   2016/09/26
     * 作者:   hwx
    */
     void slotOnAddCommentFinished(int nRequestId, const QByteArray baResponse);

signals:
     /************************************************************************************
     * 名称: sigOnReplyClicked
     * 功能: 用户点击回复评论图标的信号
     * 参数：
     * 返回:   void
     * 时间:   2016/11/25
     * 作者:   hwx
    */
     void sigOnReplyClicked();

protected:
     virtual void timerEvent(QTimerEvent *event);

private slots:
     /************************************************************************************
     * 名称: slotOnReplyClicked
     * 功能: 点击回复评论图标的槽函数
     * 参数：@[in] nCommentId: 被回复的评论id
     * 参数：@[in] strReplyToWho: 被回复的对象
     * 返回:   void
     * 时间:   2016/09/24
     * 作者:   hwx
     */
     void slotOnReplyClicked(quint64 nCommentId, const QString &strReplyToWho);

     /************************************************************************************
     * 名称: slotOnComment
     * 功能: 用户评论的槽函数
     * 参数：@[out ] nCommentId: 被回复的评论Id
     * 参数：@[out ] strComment: 回复的对象
     * 返回:   void
     * 时间:   2016/09/24
     * 作者:   hwx
     */
     void slotOnComment(quint64 nCommentId, const QString &strComment);

     /************************************************************************************
     * 名称: slotOnCommentTreeScrollValueChanged
     * 功能: 评论数滚动条值发生变化的槽函数
     * 参数：@[out ] nValue：滚动条返回的值，介于minvalue~maxvalue之间
     * 返回:   void
     * 时间:   2016/10/14
     * 作者:   hwx
     */
     void slotOnCommentTreeScrollValueChanged(int nValue);

private:
     /************************************************************************************
     * 名称: setUpUI
     * 功能: 构建界面
     * 返回:   void
     * 时间:   2016/09/19
     * 作者:   hwx
    */
     void setUpUI();

     /************************************************************************************
     * 名称: initUI
     * 功能: 初始化界面
     * 返回:   void
     * 时间:   2016/09/19
     * 作者:   hwx
    */
     void initUI();

     /************************************************************************************
    * 函数名: parseAllCommentData
    * 功能: 解析获取所有评论数据，保存到数据模型
    * 参数:
    *    @[in ] bytearrayComment: 评论数据
    * 返回值: 解析成功或失败
    * 时间: 2016/09/20 9:48
    * 作者: hwx
    */
    bool parseAllCommentData(const QByteArray &bytearrayComment);

    /************************************************************************************
   * 函数名: getResponseStatus
   * 功能: 获取应答结果
   * 参数:
   *    @[in ] bytearrayData: 应答数据
   * 返回值: Http应答码
   * 时间: 2017/02/16
   * 作者: hwx
   */
    HTTPRespCode getResponseStatus(const QByteArray &bytearrayData);

    /************************************************************************************
   * 函数名: parseCommentItem
   * 功能: 解析获取评论数据项，保存到数据模型
   * 参数:
   *    @[in ] objCommentItem: 评论数据项
   *    @[in ] pParentItem: 数据模型parent项
   * 返回值: 解析成功或失败
   * 时间: 2016/09/20 9:48
   * 作者: hwx
   */
    bool parseCommentItem(const QJsonObject &objCommentItem, QStandardItem *pParentItem);

    /************************************************************************************
   * 函数名: downloadReplierIcon
   * 功能: 下载评论者头像
   * 参数:
   *    @[in ] commentItem: 评论数据
   * 返回值:
   * 时间: 2016/09/20 9:48
   * 作者: hwx
   */
    void downloadReplierIcon(UserCommentItem &commentItem);

    /************************************************************************************
    * 名称: replyHost
    * 功能: 评论动作发布者
    * 参数：@[in] strComment: 评论内容
    * 返回:   void
    * 时间:   2016/09/24
    * 作者:   hwx
    */
    void replyHost(const QString &strComment);

    /************************************************************************************
    * 名称: replyComment
    * 功能: 回复评论
    * 参数：@[in] nCommentId: 被回复的评论id
    * 参数：@[in] strComment: 评论内容
    * 返回:   void
    * 时间:   2016/09/24
    * 作者:   hwx
   */
    void replyComment(quint64 nCommentId, const QString &strComment);

    /************************************************************************************
   * 函数名: setTotalCommentCnt
   * 功能: 设置评论总数
   * 参数:
   *    @[in ] nCnt: 评论总数
   * 返回值:
   * 时间: 2016/09/20 9:48
   * 作者: hwx
   */
    void setTotalCommentCnt(int nCnt);

    /************************************************************************************
    * 名称: clearModel
    * 功能: 清除模型数据
    * 返回:   void
    * 时间:   2016/09/19
    * 作者:   hwx
   */
    void clearModel();

    /************************************************************************************
    * 名称: startCommitCommentTimer
    * 功能: 启动提交评论定时器
    * 返回:   void
    * 时间:   2016/09/19
    * 作者:   hwx
   */
    void startCommitCommentTimer();

private:
     int m_nRequestId; // 请求id
     int m_nCommitCommentTimerId; // 提交评论定时ID

     quint64 m_nActionId; // 动作id
     quint64 m_nCurrentPage; // 当前请求评论页
     quint64 m_nCommentTotalCount; // 动作评论总数
     quint64 m_nCommentTotalPage; // 动作评论页数

     CCommentTreeView *m_pCommentTree;  // 评论树
     CCommentWidget *m_pCommentWidget; // 评论控件
     QStandardItemModel *m_pModel; // 数据模型

private:
    Ui::ActionCommentWidget *ui;
};

#endif // ACTIONCOMMENTWIDGET_H
