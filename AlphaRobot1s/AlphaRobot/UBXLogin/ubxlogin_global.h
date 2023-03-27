/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ubxlogin_global.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块dll导出头文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#ifndef UBXLOGIN_GLOBAL_H
#define UBXLOGIN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(UBXLOGIN_LIBRARY)
#  define UBXLOGINSHARED_EXPORT Q_DECL_EXPORT
#else
#  define UBXLOGINSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // UBXLOGIN_GLOBAL_H
