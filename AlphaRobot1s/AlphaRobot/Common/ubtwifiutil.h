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

#ifndef UBTWIFIUTIL_H
#define UBTWIFIUTIL_H


#include <string>

using namespace std;


class UBTWifiUtil
{
public:
    UBTWifiUtil();

public: 
    /************************************
     * 名称: infosToJsonStr
     * 功能: infos To Json Str
     * 参数:
     *  [in]
     * 返回:
     * 时间: 2017/02/27
     * 作者: asu
     ************************************/
    static string getJsonStr_ConnectWifi(int nCmdID, const string& strESSID, const string& strPassKey);

private:

};

#endif // UBTWIFIUTIL_H
