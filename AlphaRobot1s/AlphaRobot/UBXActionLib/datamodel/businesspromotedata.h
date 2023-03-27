/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：businesspromotedata.h
* 创建时间：2016/08/17
* 文件标识：
* 文件摘要：运营推广活动数据结构
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/08/17
* 版本摘要：
*/

#ifndef BUSINESSPROMOTEDATA_H
#define BUSINESSPROMOTEDATA_H

#include <QString>
#include <QVector>

#include "datamodel/userrankitem.h"

class  BusinessPromoteData
{
public:
    explicit BusinessPromoteData();
    ~BusinessPromoteData();

    // 设置、获取活动ID
    void setId(int nId) { m_nId = nId; }
    int getId() { return m_nId; }

    // 设置、获取类型
    void setType(int nType) { m_nType = nType; }
    int getType() { return m_nType; }

    // 设置、获取名称
    void setName(const QString &sName) { m_strName = sName; }
    QString getName() { return m_strName; }

    // 设置、获取时间信息
    void setDateTime(const QString &sTime) { m_strDateTime = sTime; }
    QString getStringDateTime() { return m_strDateTime; }

    // 设置、获取推广活动封面(banner)图片Url
    void setCoverImageUrl(const QString &strImageFile) { m_strCoverImageUrl = strImageFile; }
    QString getCoverImageUrl() const { return m_strCoverImageUrl; }

    // 设置、获取推广活动详情页图片Url
    void setDetailedImageUrl(const QString &strUrl) { m_strDetailedImageUrl = strUrl; }
    QString getDetailedImageUrl() { return m_strDetailedImageUrl; }

    // 设置、获取活动描述信息
    void setDescriptionMsg(const QString &strMsg) { m_strDescriptionMsg = strMsg; }
    QString getDescriptionMsg() const { return m_strDescriptionMsg; }

    // 添加、获取用户排名数据
    void addUserRankItem(UserRankItem item);
     QVector<UserRankItem> &getAllUserRankItem() { return m_vecUserRankItems; }
     void clearRankActionList(); // 清除排行榜数据

private:
     int m_nId;   // 活动ID
     int m_nType; // 活动类型

     QString m_strName;  // 活动名称
     QString m_strDateTime; // 活动时间
     QString m_strCoverImageUrl;  // 商业推广活动封面图片Url，下载到本地后是本地文件（含路径）
     QString m_strDetailedImageUrl; // 商业推广活动详情页图片Url，下载到本地后是本地文件（含路径）
     QString m_strDescriptionMsg;  // 活动描述

    QVector<UserRankItem> m_vecUserRankItems; // 用户排名数据
};

#endif // BUSINESSPROMOTEDATA_H

