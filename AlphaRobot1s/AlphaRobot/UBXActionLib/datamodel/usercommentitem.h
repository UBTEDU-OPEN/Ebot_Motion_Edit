/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：UserCommentItem.h
* 创建时间：2016/08/09
* 文件标识：
* 文件摘要：用户评论数据结构
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/08/09
* 版本摘要：
*/

#ifndef USERCOMMENTITEM_H
#define USERCOMMENTITEM_H

#include <QString>

class UserCommentItem
{
public:
    explicit UserCommentItem(QObject *pParent = 0);
    UserCommentItem(const UserCommentItem &item);
    virtual ~UserCommentItem();

    // 设置、获取评论id
    quint64 getReplyId() const;
    void setReplyId(const quint64 &nReplyId);

    // 设置、获取被评论的评论id
    quint64 getRepliedCommentId() const;
    void setRepliedCommentId(const quint64 &nRepliedId);

    // 设置、获取回复者名称
    QString getReplierName() const;
    void setReplierName(const QString &sName);

    // 设置、获取回复对象
    QString getReplyToWho() const;
    void setReplyToWho(const QString &sWho);

    // 设置、获取回复者像文件（含路径）
    QString getReplierIcon() const;
    void setReplierIcon(const QString &sPhoto);

    // 设置、获取用户评论
    QString getReplyMessage() const;
    void setReplyMessage(const QString &sMsg);

    // 设置、获取用户评论时间
    QString getReplyTime() const;
    void setReplyTime(const QString &sTime);

    // 设置、获取评论回复数据
    quint64 getReplyCount() const;
    void setReplyCount(const quint64 &nCount);

    /************************************
    * 名称: operator =
    * 功能:  复制运算符重载
    * 参数: [in]itemobj 赋值对象
    * 返回:   UserCommentItem 返回赋值结果
    * 时间:   2016/09/20
    * 作者:  hwx
    ************************************/
    UserCommentItem &operator = (const UserCommentItem& itemobj);

private:
    quint64 m_nReplyId; // 评论id
    quint64 m_nRepliedCommentId; // 被评论的评论id
    quint64 m_nReplyCount; // 回复数目

    QString m_sReplierName;  // 回复者名称
    QString m_sReplyToWho; // 回复的对象
    QString m_sReplierIcon;  // 回复者头像文件（含路径）
    QString m_sReplyMessage;   // 回复内容
    QString m_sReplyTime; // 回复时间,从后台返回的数据已经格式化
};

Q_DECLARE_METATYPE(UserCommentItem)

#endif // USERCOMMENTITEM_H

