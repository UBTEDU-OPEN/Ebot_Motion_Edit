/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CActItemDataHandle
* 创建时间：20160818
* 文件标识：
* 文件摘要：handle the action item data(ActItemData)
*
* 当前版本：1.0.0.0
* 作    者：asu
*/

#ifndef CACTITEMDATAHANDLE_H
#define CACTITEMDATAHANDLE_H

#include <QMetaType>

#include "UBXPublic.h"

typedef QMap<QString, QByteArray>  MapStringDataType;  //字符串映射到数据流
Q_DECLARE_METATYPE(MapStringDataType)

class CActItemDataHandle
{
public:
    explicit CActItemDataHandle();
    ~CActItemDataHandle();

public:
    //ActItemData to MapStringDataType
    static void ActItemDataToMapStrType(const ActItemData& actData, MapStringDataType&  mapParams);

};

#endif // CACTITEMDATAHANDLE_H
