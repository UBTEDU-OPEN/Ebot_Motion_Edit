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

#include "cfont.h"

const int DEFAULT_INVALID_VALUE = -1;

CFont::CFont()
{

}

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
QFont CFont::getFont(const QFont& fontOld, const int nSizeOffset, const bool bSetBold)
{
    QFont newFont = fontOld;
    int nFontSize = fontOld.pixelSize();
    if (DEFAULT_INVALID_VALUE < nFontSize)
    {
        nFontSize = nFontSize + nSizeOffset;
        if(nFontSize <= 0)
        {
            nFontSize = 1;
        }
        newFont.setPixelSize(nFontSize);
    }

    else
    {
        nFontSize = fontOld.pointSize();
        nFontSize = nFontSize + nSizeOffset;
        if(nFontSize <= 0)
        {
            nFontSize = 1;
        }
        newFont.setPointSize(nFontSize);
    }

    newFont.setBold(bSetBold);

    return newFont;
}
