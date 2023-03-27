// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#ifndef STDAFX_H
#define STDAFX_H

#include <QString>
#include <QWidget>
#include <QStyleOption>


#include "MD5.h"
#include "Json/json.h"

#include "zxLib.h"
using namespace zxLib;
#include "Util.h"
#include "UBXPublic.h"

#ifdef WINDOWS
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 QString 构造函数将是显式的

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#include <afxdisp.h>        // MFC 自动化类
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC 数据库类
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO 数据库类
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <atlstr.h> // for QString

#include <windows.h>
#include <lm.h>

#include <afxcontrolbars.h>


#ifdef UBXLOGIN_EXPORTS
#define UBXLOGIN_DLL  __declspec(dllexport)
#else
#define UBXLOGIN_DLL  __declspec(dllimport)
#endif


#include <atlimage.h>
#include <afxinet.h>
#include <vector>
#include <map>
#include <list>
#include <string>
using namespace std;

#include <GdiPlus.h>
using namespace Gdiplus;

#include <WtsApi32.h>
#include <UserEnv.h>
#include <TlHelp32.h>
#pragma comment(lib,"WtsApi32.lib")
#pragma comment(lib,"UserEnv.lib")
#pragma comment(lib,"version.lib")

#include <ShlObj.h>
#include <XTToolkitPro.h>
#include "Util.h"
#include "Markup.h"
#include "HttpOper.h"
#include "json/json.h"
#include "StringFunc.h"
#include "MD5.h"

#include "UBXUIBase.h"
#include "IMGViewCtrl.h"
#include "ColorPanel.h"
#include "MyColorDialog.h"
#include "StaticEx.h"
#include "StaticBx.h"
#include "StaticTx.h"
#include "VEdit.h"
#endif
//extern QString  g_sImageDir;   //图片目录
//extern CFont    g_fontText;    //字体
//extern DWORD    g_dwTextRGBColor; //文字颜色
//extern DWORD    g_lkTextRGBColor; //文字颜色
//extern COLORBUTTONDATA g_tabbtncolordata[3];

//extern CFont    g_font_s; //小号字体
//extern CFont    g_font_x; //大号字体
//extern CFont    g_font_link; //超链接字体

#endif // STDAFX_H
