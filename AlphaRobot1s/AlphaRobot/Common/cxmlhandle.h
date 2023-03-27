/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CXmlHandle
* 创建时间：2017/7/13
* 文件标识：
* 文件摘要：
*
* 当前版本：1.0.0.0
* 作    者：asu
* 完成时间：2017/7/13
* 版本摘要：
*/

#ifndef CXMLHANDLE_H
#define CXMLHANDLE_H


#include <QString>



class QDomDocument;


class CXmlHandle
{
public:
    CXmlHandle();
    virtual ~CXmlHandle();

public:
    static bool saveXml(const QString& strXmlFilePathOut, const QDomDocument& docXmlSaved);


};

#endif // CXMLHANDLE_H
