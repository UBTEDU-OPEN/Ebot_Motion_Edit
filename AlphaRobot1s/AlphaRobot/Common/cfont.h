/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CFont
* 创建时间：20160928
* 文件标识：
* 文件摘要：handle the font
*
* 当前版本：1.0.0.0
* 作    者：asu
*/


#ifndef CFONT_H
#define CFONT_H


#include <QFont>


class CFont
{
public:
    CFont();

public:
    /*********************************************************************
    * 名称: getFont
    * 功能: get the font
    * 参数: [in]fontOld
    * 参数: [in]nSizeOffset   when "-" is change small
    * 参数: [in]bSetBold
    * 返回: QFont
    * 时间: 20160928
    * 作者: asu
    *********************************************************************/
    static QFont getFont(const QFont& fontOld, const int nSizeOffset, const bool bSetBold = false);

};

#endif // CFONT_H
