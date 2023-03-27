/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ILibTreeNavigation.h
* 创建时间：2015/12/04 14:00
* 文件标识：
* 文件摘要：动作模板库导航树图抽象类
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2015/12/04 14:00
* 版本摘要：
*/
#ifndef ILIBTREENAVIGATION_H
#define ILIBTREENAVIGATION_H

#include "stable.h"

//class ITreeNavigationEvent
//{

//public:
//    virtual bool TreeView(char* pData, int nLen, QPoint* nPoint, QString FileName) = 0;

//};

class ILibTreeNavigation
{

public:
    virtual bool AddNewTreeNode(char* pData, int nLen, QString strName) = 0;
    virtual bool connect(QWidget* pParent/*,ITreeNavigationEvent* pNotify*/) = 0;
    virtual void disconnect() = 0;
    virtual QWidget* GetWidget() = 0;
    virtual void SetFlowCtrl(IUBXMotionEditor* pFlowCtrl) = 0;
};

bool CreateTreeNavigation(ILibTreeNavigation**  ppUBXInavigationMotify);

#endif // ILIBTREENAVIGATION_H
