/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：UserCommentItem.cpp
* 创建时间：2016/08/09
* 文件标识：
* 文件摘要：用户推广活动数据结构
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/08/09
* 版本摘要：
*/

#include "datamodel/usercommentitem.h"

UserCommentItem::UserCommentItem(QObject *pParent) : m_nReplyId(0),
    m_nReplyCount(0),
    m_nRepliedCommentId(0)
{

}

UserCommentItem::UserCommentItem(const UserCommentItem &item)
{
    this->m_nReplyId = item.getReplyId();
    this->m_nRepliedCommentId = item.getRepliedCommentId();
    this->m_nReplyCount = item.getReplyCount();

    this->m_sReplierName = item.getReplierName();
    this->m_sReplyToWho = item.getReplyToWho();
    this->m_sReplierIcon = item.getReplierIcon();
    this->m_sReplyMessage = item.getReplyMessage();
    this->m_sReplyTime = item.getReplyTime();
}

UserCommentItem::~UserCommentItem()
{

}

QString UserCommentItem::getReplierName() const
{
    return m_sReplierName;
}

void UserCommentItem::setReplierName(const QString &sName)
{
    m_sReplierName = sName;
}

QString UserCommentItem::getReplierIcon() const
{
    return m_sReplierIcon;
}

void UserCommentItem::setReplierIcon(const QString &sPhoto)
{
    m_sReplierIcon = sPhoto;
}

QString UserCommentItem::getReplyMessage() const
{
    return m_sReplyMessage;
}

void UserCommentItem::setReplyMessage(const QString &sMsg)
{
    m_sReplyMessage = sMsg;
}

QString UserCommentItem::getReplyTime() const
{
    return m_sReplyTime;
}

void UserCommentItem::setReplyTime(const QString &sTime)
{
    m_sReplyTime = sTime;
}

quint64 UserCommentItem::getReplyCount() const
{
    return m_nReplyCount;
}

void UserCommentItem::setReplyCount(const quint64 &nCount)
{
    m_nReplyCount = nCount;
}

/************************************
* 名称: operator =
* 功能:  复制运算符重载
* 参数: [in]itemobj 赋值对象
* 返回:   UserCommentItem 返回赋值结果
* 时间:   2016/09/20
* 作者:  hwx
************************************/
UserCommentItem &UserCommentItem::operator =(const UserCommentItem &itemobj)
{
    if (&itemobj == this)
    {
        return *this;
    }

    this->m_nReplyId = itemobj.getReplyId();
    this->m_nRepliedCommentId = itemobj.getRepliedCommentId();
    this->m_nReplyCount = itemobj.getReplyCount();

    this->m_sReplierName = itemobj.getReplierName();
    this->m_sReplyToWho = itemobj.getReplyToWho();
    this->m_sReplierIcon = itemobj.getReplierIcon();
    this->m_sReplyMessage = itemobj.getReplyMessage();
    this->m_sReplyTime = itemobj.getReplyTime();

    return *this;
}

quint64 UserCommentItem::getRepliedCommentId() const
{
    return m_nRepliedCommentId;
}

void UserCommentItem::setRepliedCommentId(const quint64 &nRepliedId)
{
    m_nRepliedCommentId = nRepliedId;
}

quint64 UserCommentItem::getReplyId() const
{
    return m_nReplyId;
}

void UserCommentItem::setReplyId(const quint64 &nReplyId)
{
    m_nReplyId = nReplyId;
}

QString UserCommentItem::getReplyToWho() const
{
    return m_sReplyToWho;
}

void UserCommentItem::setReplyToWho(const QString &sWho)
{
    m_sReplyToWho = sWho;
}
