/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：UBTWifiUtil
* 创建时间：2017/02/27
* 文件标识：
* 文件摘要：辅助函数类
*
* 当前版本：1.0.0.0
* 作    者：asu
* 完成时间：2017/02/27
* 版本摘要：
*/


#include "ubtwifiutil.h"

#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include <QDebug>


UBTWifiUtil::UBTWifiUtil()
{

}


string UBTWifiUtil::getJsonStr_ConnectWifi(int nCmdID, const string &strESSID, const string &strPassKey)
{    
    QJsonObject jsonObject;

    jsonObject.insert("ESSID", QString::fromStdString(strESSID));
    jsonObject.insert("PassKey", QString::fromStdString(strPassKey));
   // jsonObject.insert("C", jsonObjectSon);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObject);
    QByteArray byte = jsonDoc.toJson();
    QString strJson(jsonDoc.toJson());

    return strJson.toStdString();
}
