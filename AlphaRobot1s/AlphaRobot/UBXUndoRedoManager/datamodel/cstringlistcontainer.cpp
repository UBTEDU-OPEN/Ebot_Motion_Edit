/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：cactiontemplatefilesmanager.cpp
* 创建时间：2017/01/17
* 文件标识：动作模板文件管理
* 文件摘要：
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/17
* 版本摘要：
*/

#include <QDebug>
#include "cstringlistcontainer.h"

CStringListContainer::CStringListContainer()
{

}

CStringListContainer::~CStringListContainer()
{
    clearFiles();
}

/************************************
* 名称: addFile
* 功能: 添加动作文件，不会判断文件是否存在
* 参数: [in] strFile: 文件名称，绝对路径
* 返回:  void
* 时间:   2017/01/17
* 作者:   hwx
************************************/
void CStringListContainer::addFile(const QString &strFile)
{
    if (strFile.isEmpty() || m_lstFiles.contains(strFile))
    {
        return;
    }

    m_lstFiles.append(strFile);
}

/************************************
* 名称: removeFile
* 功能: 删除动作文件，不会判断文件是否存在
* 参数: [in] strFile: 文件名称，绝对路径
* 返回:  void
* 时间:   2017/01/17
* 作者:   hwx
************************************/
void CStringListContainer::removeFile(const QString &strFile)
{
    for (int i = 0; i < m_lstFiles.size(); i++)
    {
        QString str = m_lstFiles[i];
        qDebug() << str  << endl;
    }

    if (strFile.isEmpty() || !m_lstFiles.contains(strFile))
    {
        return;
    }

    m_lstFiles.removeAll(strFile);
}

void CStringListContainer::removeFiles(QList<QString> &lstFiles)
{
    for (int i = 0; i < lstFiles.size(); i++)
    {
        removeFile(lstFiles[i]);
    }
}

/************************************
* 名称: clearFiles
* 功能: 清空文件列表
* 参数:
* 返回:  void
* 时间:   2017/01/17
* 作者:   hwx
************************************/
void CStringListContainer::clearFiles()
{
    m_lstFiles.clear();
}

/************************************
* 名称: setFiles
* 功能: 设置文件列表
* 参数:
* 返回:  void
* 时间:   2017/01/17
* 作者:   hwx
************************************/
void CStringListContainer::setFiles(QList<QString> &lstFiles)
{
   clearFiles();
   m_lstFiles = lstFiles;
}

