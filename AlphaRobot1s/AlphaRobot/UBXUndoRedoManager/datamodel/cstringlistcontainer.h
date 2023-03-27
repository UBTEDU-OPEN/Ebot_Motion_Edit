/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：cactiontemplatefilesmanager.h
* 创建时间：2017/01/17
* 文件标识：动作模板文件管理，封装成container的目的是为了参数传递list<T>
* 文件摘要：
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/17
* 版本摘要：
*/

#ifndef CSTRINGLISTCONTAINER_H
#define CSTRINGLISTCONTAINER_H

#include <QFileInfo>
#include <QList>
#include <QString>

#include "ubxundoredomanager_global.h"

class UBXUNDOREDOMANAGER_API CStringListContainer
{
public:
    explicit CStringListContainer();
    ~CStringListContainer();

public:
    /************************************
    * 名称: addFile
    * 功能: 添加动作文件，不会判断文件是否存在
    * 参数: [in] strFile: 文件名称，绝对路径
    * 返回:  void
    * 时间:   2017/01/17
    * 作者:   hwx
    ************************************/
    void addFile(const QString &strFile);

    /************************************
    * 名称: removeFile
    * 功能: 删除动作文件，不会判断文件是否存在
    * 参数: [in] strFile: 文件名称，绝对路径
    * 返回:  void
    * 时间:   2017/01/17
    * 作者:   hwx
    ************************************/
    void removeFile(const QString &strFile);
    void removeFiles(QList<QString> &lstFiles);

    /************************************
    * 名称: clearFiles
    * 功能: 清空文件列表
    * 参数:
    * 返回:  void
    * 时间:   2017/01/17
    * 作者:   hwx
    ************************************/
    void clearFiles();

    /************************************
    * 名称: setFiles
    * 功能: 设置文件列表
    * 参数:
    * 返回:  void
    * 时间:   2017/01/17
    * 作者:   hwx
    ************************************/
    void setFiles(QList<QString> &lstFiles);

    // 获取所有文件列表
    QList<QString> &getFiles() { return m_lstFiles; }

private:
    QList<QString>  m_lstFiles;   // 动作模板列表
};

#endif // CSTRINGLISTCONTAINER_H
