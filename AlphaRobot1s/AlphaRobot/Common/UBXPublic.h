#ifndef UBXPUBLIC_H
#define UBXPUBLIC_H

#include <vector>
#include <set>
#include <map>
using namespace std;

#include <QString>
#include <QUuid>
#include <QMetaType>
#include <QList>
#include <QEvent>

#ifdef USEXML
#include <QDomDocument>
#include <QDomElement>
#endif


#define UBX_MEMCPY_INC(t, s) memcpy(&t, s, sizeof(t));s+=sizeof(t);

#define SAFE_DELETE(p)       if(p){ delete p; p = NULL;}
#define SAFE_DELETE_ARRAY(p) if(p){ delete [] p; p = NULL;}

#define OUT
#define IN

// ini 文件
#define		UBX_CONFIG_FILE		"AlphaRobot.ini"
// 配置文件的头部 (9个字节)
#define		UBX_FILE_TAG		"ubx-alpha"
//定制模块的头部(13字节)
#define		UBX_CUSTOMIZE_TAG	"ubx-customize"

// 配置文件的版本(4个字节)(VERSION 2.0)
#define		UBX_FILE_VERSION	2


// 工程文件扩展名
#define     UBX_FILE_EXT_OLD  ".aes"

// 新工程文件扩展名
#define     UBX_FILE_EXT      ".aesx"

// 新工程文件注册表扩展名
#define     UBX_REG_EXT      "aesxfile"

// 旧版动作文件扩展名
#define     TAB_FILE_EXT      ".tab"

// 动作二进制文件扩展名
#define     HTS_FILE_EXT      ".hts"
// 动作xml文件扩展名
#define     XML_FILE_EXT      ".xml"

// 音乐文件扩展名
#define     MUSIC_FILE_EXT   ".mp3"

// 升级文件扩展名
#define     BIN_FILE_EXT      ".bin"

// 模板文件扩展名
#define     UBX_TEMPLATE_EXT  ".tpl"

// 模板文件的头部 (12个字节)
#define     UBX_TEMPLATE_TAG  "ubx-template"

// 支持的图片格式
#define IMAGE_SUPPORT_FORMAT  "*.png *.jpg"

// 支持的视频格式
#define VIDEO_SUPPORT_FORMAT  ".mp4"
#define VIDEO_SUPPORT_FORMAT_WITHOUT_DOT "mp4"


// HTS文件的版本
#define		HTS_FILE_VERSION	2

// HTS文件的logo
#define		HTS_FILE_LOGO	"HTS"


// 模型的xml配置文件
#define     MODEL_XML_NAME     "Alpha1s.xml"
// 模型的数据文件
#define     MODEL_DATA_NAME    "Alpha1s.fbx"
#define     FBX_ASCII_TEXTURE  "FBX_ASCII_TEXTURE"


// ALPHA Socket通信端口号
#define ALPHA_SOCKET_PORT	6100
// SOCKET通讯标志
#define ALPHA_SOCKET_FLAG	0x1234
//MULTISOCKET通讯标志
#define ALPHA_MULTISOCKET_FLAG	0x1235
// SOCKET协议版本号
#define ALPHA_SOCKET_VERSION	0x0001

//////////////////////////////////////////////////////////////////////////
// 消息ID
// 登陆消息ID
#define ALPHA_MSG_LOGIN					1
//尝试向服务器获取服务器的IP和端口号信息
#define ALPHA_MSG_FILESERVERINFO		2
//获取配置动作配置文件列表
#define ALPHA_MSG_GETCFGFILES			3
//删除动作文件
#define ALPHA_MSG_DELETEACTIONFILE	    4
//执行动作表
#define ALPHA_MSG_PALYACTION			5
// 配置WIFI连接点
#define ALPHA_MSG_CONFIGUREWIFI			6
// 获取AP连接点
#define ALPHA_MSG_GETWIFILIST			7
//设置连接的WIFI
#define ALPHA_MSG_SETWIFICONNECT		8
// 设置舵机角度
#define ALPHA_MSG_SETMOTOANGLE			9
// 获取舵机角度值
#define ALPHA_MSG_GETMOTORANGLE			10
// 停止播放
#define ALPHA_MSG_STOPPLAY				11
// 发送MP3音频文件信息
#define ALPHA_MSG_MUSICRESOURCE	        12
// 发送希望从机器人上获取动作列表的名称
#define ALPHA_MSG_GETALPHACONFIGFILE    13
// 回应某一上传文件的大小，socket端口及IP
#define ALPHA_MSG_GETDETAILFILEINFO     14
//仿真耳朵数据
#define ALPHA_MSG_PLAYEARLEDDATA		17
//停止耳朵LED播放
#define ALPHA_MSG_STOPPLAYEARLED		18
//仿真眼睛数据
#define ALPHA_MSG_PLAYEYESLEDDATA		19
//停止眼睛LED播放
#define ALPHA_MSG_STOPPLAYEYELED		20
//更新胸部软件
#define ALPHA_MSG_UPDATE_CHES_SOFTWARE  21
//更新头部软件
#define ALPHA_MSG_UPDATE_HEADER_SOFTWARE  22


// 登陆回应
#define ALPHA_MSG_RSP_LOGIN				1
//从服务器获取的IP和端口号信息
#define ALPHA_MSG_RSP_FILESERVERINFO    2
//回应获取配置动作配置文件列表
#define ALPHA_MSG_RSP_GTCFGFILES		3
//回应删除动作文件
#define ALPHA_MSG_RSP_DELETEACTIONFILE	4
//回复执行动作表
#define ALPHA_MSG_RSP_PALYACTION		5
// 回复配置WIFI连接点、
#define ALPHA_MSG_RSP_CONFIGUREWIFI		6
// 回复获取AP连接点
#define ALPHA_MSG_RSP_GETWIFILIST		7
// 回应设置连接的WIF
#define ALPHA_MSG_RSP_SETWIFICONNECT	8
// 回应读取舵机角度
#define ALPHA_MSG_RSP_READANGLE			10
// 回应音频信息
#define ALPHA_MSG_RSP_MUSICRESOURCE	    12
// 回应对应ubx文件的资源文件
#define ALPHA_MSG_RSP_GETALPHACONFIGFILE 13
// 回应某一上传文件的大小，socket端口及IP
#define ALPHA_MSG_RSP_GETDETAILFILEINFO 14


#ifndef Q_OS_WIN
typedef unsigned short   WCHAR;
#endif

typedef unsigned char    byte;


typedef unsigned char   byte;
typedef unsigned char   uchar;

typedef unsigned short  uint16;
//typedef unsigned int     uint32;
typedef unsigned int     UINT;

//typedef long long int int64;
//typedef int64_t         int64;

typedef vector<byte>  vecByteLst;
typedef vector<string> vecStrLst;


//广播
// 发送返回设备信息指令
#define ALPHA_MSG_DEVINFO       1

// 返回设备信息
#define ALPHA_RET_MSG_DEVINFO       2


#define REQUSET_ID_DEFAULT_INVALID    -1



using namespace std;

//for widget
const int WIDGET_LAYOUT_UBX_TREEWIDGET_MIN_WIDTH = 162;
const int WIDGET_LAYOUT_UBX_ROBOT_3D_CONTAINER_MIN_WIDTH = 315;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief NET_PATH_UBX
///// 服务器数据字段常量定义
///
const QString NET_PATH_UBX  = "/ubx";
const QString NET_PATH_ACTION  = NET_PATH_UBX + "/action";
const QString NET_PATH_DETAIL  = NET_PATH_ACTION + "/detail";
const QString NET_PATH_ACTION_DELETE  = NET_PATH_ACTION + "/delete";


const QString NET_PATH_APPLICATION_JSON  = "application/json";

const QString NET_ACTION_DATE_FORMAT  = "yyyy-MM-dd hh:mm:ss";


const QString NET_COLLECT_RELATION_ID  = "collectRelationId";
const QString NET_MODELS  = "models";
const QString NET_STATUS = "Status";
const QString NET_INFO = "info";
const QString NET_TOTAL_RECORDS_COUNT = "count";

const QString NET_ACTION_ID  = "actionId";
const QString NET_ACTION_TOTALPAGE = "totalPage";
const QString NET_ACTION_RESOURCE = "actionResource"; // 用户动作、官方动作
const QString NET_ACTION_TYPE = "actionSonType";//actionType (old actiontype str)
const QString NET_ACTION_NAME = "actionName";
const QString NET_ACTION_DESCIBER = "actionDesciber";
const QString NET_ACTION_HEADURL = "actionHeadUrl";// new动作预览图地址
const QString NET_ACTION_IMAGE_PATH = "actionImagePath";// 动作预览图地址
const QString NET_ACTION_VIDEO_PATH = "actionVideoPath";
const QString NET_ACTION_PATH = "actionPath";//m_sActDownloadURL
const QString NET_ACTION_TIME = "actionTime";//atcion m_dwActTimeLen
const QString NET_ACTION_PUBLIC_DATE = "actionDate";// 动作发布日期
const QString NET_ACTION_SIZE = "actionSize";// 动作文件大小
const QString NET_ACTION_DOWNLOAD_TIME = "actionDownloadTime";// 动作下载总量
const QString NET_ACTION_COMMENT_TIME = "actionCommentTime";// 动作评论总量
const QString NET_ACTION_PRAISE_TIME = "actionPraiseTime";// 动作点赞总量
const QString NET_ACTION_BROWN_TIME = "actionBrowseTime"; // 动作浏览次数
const QString NET_ACTION_USER = "actionUser";// 动作发布者
const QString NET_ACTION_TITLE = "actionTitle"; // 动作标题
const QString NET_ACTION_COLLECT_TIME = "actionCollectTime"; // 动作收藏总量
const QString NET_ACTION_STATUS = "actionStatus";
const QString NET_ACTION_HOT_VALUE = "hostSum"; // 动作热度值，下载数 + 评论数 + 浏览数 + 点赞数 + 收藏数

const QString NET_ACTION_INFO = "actionInfo"; // 收藏动作接口动作详细信息


const QString NET_ACTION_USER_ID = "actionUserId";
const QString NET_ACTION_ORIGINAL_ID = "actionOriginalId";


const QString NET_SUBJECT_RECOMMEND_ID = "recommendId"; // 推荐ID
const QString NET_SUBJECT_RECOMMEND_TYPE = "recommendType"; // 推荐类型
const QString NET_SUBJECT_RECOMMEND_ORDER = "recommendOrder"; // 推荐等级
const QString NET_SUBJECT_RECOMMEND_FORWARD_TYPE = "recommendForwardType"; // 主题跳转类型
const QString NET_SUBJECT_RECOMMEND_IMAGE = "recommendImage";  // 推荐动作图片
const QString NET_ACTION_CLICK_FORWARD = "clickForward"; // 点击推荐动作图片时跳转到对应action

const QString NET_SCHEME_INFO = "schemeInfo";  // 运营活动信息
const QString NET_SCHEME_ACTION = "schemeAction"; // 运营活动动作列表
const QString NET_SCHEME_ID = "schemeId";
const QString NET_SCHEME_TYPE = "schemeType";
const QString NET_SCHEME_COVER_IMAGE = "schemeIcon";  // 自动弹出图片
const QString NET_SCHEME_DETAILED_IMAGE = "schemeImage"; // banner页中的图片
const QString NET_SCHEME_DATE= "schemeDate";
const QString NET_SCHEME_NAME = "schemeName";
const QString NET_SCHEME_CONTENT= "schemeContext";
const QString NET_USER_ID = "userId";
const QString NET_USER_NAME = "userName";
const QString NET_USER_IMAGE = "userImage";

// 动作评论字段
const QString NET_COMMENT_COUNT = "count";
const QString NET_COMMENT_TOTAL_PAGE = "totalPage";
const QString NET_COMMENT_REPLY_ID = "replyId";
const QString NET_COMMENT_REPLIER_NAME = "hostName";
const QString NET_COMMENT_REPLY_TIME = "replyTime";
const QString NET_COMMENT_REPLY_DETAIL = "replyDetail";
const QString NET_COMMENT_REPLIER_ICON = "hostIcon";
const QString NET_COMMENT_REPLY_TO = "replyTo";
const QString NET_COMMENT_REPLIED_ID = "upReplyId";
const QString NET_COMMENT_REPLY_LIST = "replyList";

//const QString SETTING_LANGUAGE_SETTING_GROUP = "LanguageSettingWidget";
//const QString SETTING_LANGUAGE_SETTING_GROUP_LANGUAGE_KEYWORD = "language";

//const QString SETTING_ROBOT_SETTING_GROUP = "RobotSetting";
//const QString SETTING_ROBOT_SETTING_GROUP_ROBOT_TYPE_KEYWORD = "RobotType";


// 动作类型对应图标资源
const QString ICON_ACTION_TYPE_DANCE = ":/res/images/icon_type_dance.png";
const QString ICON_ACTION_TYPE_MUSIC = ":/res/images/icon_type_music.png";
const QString ICON_ACTION_TYPE_SPORTS = ":/res/images/icon_type_sports.png";
const QString ICON_ACTION_TYPE_STORY = ":/res/images/icon_type_story.png";
const QString ICON_ACTION_TYPE_EDUCATE = ":/res/images/icon_type_educate.png";
const QString ICON_ACTION_TYPE_DEFAULT = ICON_ACTION_TYPE_DANCE;

const QString UBX_DEFAULT_IMAGE = "DefaultImage.jpg";
const QString UBX_LOGIN_HEAD_IMAGE = "loginhead.png";
const QString UBX_DISPLAY_IMAGE_1= "display1.png";

// 颜色值
const QString UBX_COLOR_GREY = "#d3d3d3";
const QString UBX_COLOR_WHITE = "#ffffff";
const QString UBX_COLOR_DARK_RED = "#ca0813";
const QString UBX_COLOR_DARK_GREY = "#858585";

// 字体大小
const int FONT_PIXEL_SIZE_10 = 10;
const int FONT_PIXEL_SIZE_12 = 12;
const int FONT_PIXEL_SIZE_14 = 14; // 14字体大小
const int FONT_PIXEL_SIZE_16 = 16; // 16字体大小
const int FONT_PIXEL_SIZE_18 = 18; // 18字体大小
const int FONT_PIXEL_SIZE_20 = 20;

// 多语言关键字
const QString LANGUAGE_CHINESE_POSTFIX = "zh_CN";  // 简体中文
const QString LANGUAGE_ENGLISH_POSTFIX = "en";  // 英文
const QString LANGUAGE_HUNGARY_POSTFIX = "zh_HU";  // 匈牙利文

const int DEFAULT_INVALID_NOINIT_INT_NUMBER = 0;
const int INVALID_INTEGER = -1;

const int BUTTON_HOME_PAGE  = 0;
const int BUTTON_ACTIONS_LIBRARY      = 1;
const int BUTTON_FAVORITES    = 2;
const int BUTTON_MYUPLOADS  = 3;
const int BUTTON_RECOMMEND_SUBJECT_PAGE = 4;
const int BUTTON_BUSINESS_PROMOTE_PAGE = 5;


#define SERVER_DOMAIN_SUFFIX  "/ubx"  //服务器域名后缀，如/ubx

#define MIN(a, b) (((a) > (b))? (b):(a))
#define MAX(a, b) (((a) > (b))? (a):(b))



//#define UBT_OK    0
//#define UBT_ERROR -1


const int ROBOT_TOTAL_MOTOR_NUMBER = 16;
const int ROBOT_ID_DEFAULT_START_NUMBER = 1;




///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * 为了区分是什么操作导致的角度改变，libFBX需要判断是否通知controlview界面刷新
 * 1-拖动3D模型改变角度（预留给libFBX模块）
 * 2-controlview的控制按钮（slider和+，-按钮）
 * 3-reset特殊操作,需要刷新界面角度时也可以使用，不会改变界面选中的舵机号
 * 4-掉电（预留）
 * 5-动作编辑模块
 * 6-机器人回读数据改变角度
*/
enum TypeAngleChanged
{
    TAC_3DModel = 1,
    TAC_ControlView,
    TAC_RESET,
    TAC_POWERLOST,
    TAC_MotionEditor,
    TAC_ReadBack,
    TAC_MotionEditerManual
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//检查更新类型
enum emCheckUpdateType
{
   //emCheckUpdateType 为从服务器后台上查询检查需求更新的类型
    eCheckUpateTypeUnknown,
    eCheckUpdateTypeWindows,        //更新Windows端软件
    eCheckUpdateTypeOldRobot1S,     //更新旧版1S机器人端软件
    eCheckUpdateTypeNewRobot1S,     //更新新版1S机器人端软件, alpha1s_v
    eCheckUpdateTypeRobot1P,        //更新1P机器人端软件, alpha1p_v
    eCheckUpdateTypeNewRobot1SV2,  //更新新版1SV2.0机器人端软件, alpha1_v2.0
    eCheckUpdateTypeMac,           //更新Mac端软件
    eCheckUpdateTypeRobot1SP,  // 更新机器人端1SP版本软件，alpha1_v2.06
    eCheckUpdateTypeRobot1PV2, // 更新机器人端1PV2版本软件，alpha1_v2.05
    eCheckUpdateTypeRobot1PPlus, // 更新机器人端1P Plus版本软件，alpha1_v2.07



    eCheckUpdateTypeRobot1E      //not define
};

//UBT产品类型
enum emUbtProductType
{
    //产品大分类类型
    eUbtProductUnknown = 0,
    eUbtProduct1S1P = 1,  //1s，包括alpha1, alpha1p等
    eUbtProduct2  = 2,     //2



    eUbtProduct1E = 100, //not define
};

//Alpha1S的产品版本类型
enum emProduct1SVersionType
{
    //////////////////////////////////////////////////////////////////
    // 考虑到所有机器人型号都可以共用同一
    // 嵌入式固件版本，后续所有的嵌入式都同一
    // 使用eProduct1SVersion1S1P
    /////////////////////////////////////////////////////////////////
    eProduct1SVersionOld = 1,  //旧版1S
    eProduct1SVersionNew,     //新版1S
    eProduct1SVersion1P,       //1P
    eProduct1SVersion1S1P,  //1SV2/1PV2/1SP/1PPlus
};

//运行平台类型
enum emProductOnSystemType
{
    eProductOnSystemEmbedded = 0,    //嵌入式
    eProductOnSystemAndroid,          //安卓
    eProductOnSystemWindows,         //Windows
    eProductOnSystemMac,             //Mac
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


//动作类型
enum emActionType
{
    //0:所有动作,1:舞蹈,2:寓言故事,3: 运动, 4:儿歌, 5:教育, 6:其他
    eActTypeAll = 0,
    eActTypeDance,
    eActTypeStory,
    eActTypeSport,
    eActTypeMusic,
    eActTypeEducation,
    eActTypeOther,
};

//动作查询分组
enum emActionGroup
{
//    eActGroupAll = 0,
//    eActGroupOfficalAct,    //官方动作
//    eActGroupLastUpdate,    //最近更新
//    eActGroupHotAct,        //最热动作
//    eActGroupMostComment,   //最多评论
//    eActFavorites           //我的收藏
    eActGroupAllLastUpdate = 0,//all 最近更新
    eActGroupAllHotest,//最热动作
    eActGroupAllMostComment,//最多评论

    eActGroupOfficalLastUpdate,//官方, 最近更新
    eActGroupOfficalHotest,//最热动作
    eActGroupOfficalMostComment,//最多评论

    eActGroupUserLastUpdate,//User, 最近更新
    eActGroupUserHotset,//最热动作
    eActGroupUserMostComment,//最多评论


    eActGroupDefaultAct = eActGroupAllLastUpdate
};

enum emActionResource
{
    eActionResourceOfficial = 0,
    eActionResourceIShow,
};

enum emActionStatus
{
//9 审核中 1 审核成功 2 审核不通过
    eActStatusUnknow = 0,
    eActStatusApproved = 1,
    eActStatusNoApproved = 2,

    eActStatusApproving = 9
};

enum emPlayType
{
    ePlayTypeNone = 0,
    ePlayTypeMusic,   // 音乐仿真
    ePlayTypeMotion,  // 动作仿真
    ePlayTypeUnited,  // 联合仿真
};

enum emPlayStatus
{
    ePlayStatusNone = 0,
    ePlayStatusPlaying,
    ePlayStatusPause,
    ePlayStatusStop, // 播放过程中停止播放
    ePlayStatusFinished, // 正常播放结束
};

//上传图片类型
enum emUploadFileType
{
    eUploadFileHeadImg = 1,  //上传头像图片
    eUploadFileDescImg,      //上传描述图片
    eUploadFileDescVideo,    //上传描述视频
};

// 文件传输类型，用于区分是哪种类型的传输
// 如同步动作，版本更新等
enum emTranFileType
{
    emUnknown = 0,
    emTranFileSynAction, // 同步动作
    emTranFileUpdateSysEmbed // 更新嵌入式版本
};

enum emRequestType
{
    eRequestGet,
    eRequestPost
};

// 标题栏tab页
enum emTitleBarTabId
{
    eTitleBarTabEdit = 0,   // 编辑页
    eTitleBarTabActionLib, // 动作库页
    eTitleBarTabActionSync // 动作同步页
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct __stMotorInfo
{
    int nID;		// 舵机ID号
    int nAngle;		// 舵机角度（0~360）    

    __stMotorInfo()
    {
        nID = 0;
        nAngle = 0;
    }
    __stMotorInfo(int nMotoID, int nMotoData)
    {
        this->nID = nMotoID;
        this->nAngle = nMotoData;
    }

}MOTOR_DATA, *PMOTOR_DATA;

typedef vector<MOTOR_DATA> vecMotorData;

typedef struct __DevInfo  //设备信息结构体
{
    QString strDevname;    //设备名
    QString strOsVersion;  //版本号
    QString strIpAddress;  //IP地址
    QString strMacAddress; //MAC地址
} DevInfo, *pDevInfo;

typedef struct __FileServerInfo_DL //用于传送（下载：下载到机器人功能）UBX文件的结构体
{
    int nPort;
    QString strIp;
    QString strFileName;
} FileServerInfo_DL, *pFileServerInfo_DL;

typedef struct __FileServerInfo_UL  //用于传送（上传：导出功能）UBX文件的结构体
{
    int nPort;
    unsigned int nFileLen;
    QString strIp;
    QString strFileName;
} FileServerInfo_UL, *pFileServerInfo_UL;

typedef struct _TransferListValue        //传输列表中（下载对话框中）一行的信息结构体
{
    QString FileName ;
    float   fTranspeed;
    unsigned int    nDataTransferred;
    int     nStatus ;                 // 0-等待传输 1-正在传输 -1-传输失败  2-传输成功
    bool    bType;                    // 0-下载到机器人  1-上传到PC
    unsigned int     nFileLen;
}TransferListValue, *pTransferListValue;


//动作项数据
struct ActItemData
{
public:
    QString     m_sGuid;          //唯一标识
    int         m_nItemID;        //项ID
    qint64      m_nActionOriginalId; //!!!only for upload file of zip

    QString     m_sImageHeadUrl;  //new预览图片地址
    QString     m_sImageURL;      //预览图片地址,now is to desc image
    QString     m_sVideoURL;      //预览视频地址
    QString     m_sActName;       //动作名称
    QString     m_sActDesc;       //动作描述
    QString     m_sPublishTime;   //动作发布时间
    qint64      m_nActSize;       //动作文件大小
    QString     m_sActDownloadURL;//动作下载地址
    QString   m_strSchemeName; // 活动名称
    int         m_nActType;       //动作类型
    int         m_nActResource; // 官方动作、用户动作
    int         m_nSchemId;     // 运营活动id
    qint64      m_dwActTimeLen;   //动作时长,单位秒(后台单位是秒，hts单位是ms)
    quint64     m_nActionDownloads; //下载总量
    quint64     m_nActionComments;//评论总量
    quint64     m_nActionLikes;//点赞总量
    quint64     m_nActionCollectTime;// 收藏总数
    int         m_nActionStatus;//publish status, 9 正在审核 1 审核通过 2 审核不通过
    QString     m_sPublisher;//发布者ID
    QString     m_sActionTitle;//动作标题
    QString     m_sLocalPath;//本地文件路径
    quint64     m_nHasDownloadedBytes;//已经下载字节数
    quint64     m_nActionBrownTime; // 动作浏览次数
    quint64     m_nActionHotValue; // 动作热度值

    int m_nUserId;  // 动作对应用户id
    QString m_sUserName;  // 动作对应用户名
    QString m_sUserImageUrl; // 动作对应用户头像文件Url，下载到本地后指本地文件（含路径）

    ActItemData()
    {
        m_sGuid = QUuid::createUuid().toString();
        m_nItemID = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_nActionOriginalId = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_sImageHeadUrl = "";
        m_sImageURL = "";
        m_sVideoURL = "";
        m_sActName = "";
        m_sActDesc = "";
        m_sPublishTime = "";
        m_strSchemeName = "";
        m_nActSize = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_sActDownloadURL = "";
        m_nActType = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_nActResource = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_nSchemId = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_dwActTimeLen = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_nActionDownloads = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_nActionComments = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_nActionLikes = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_nActionCollectTime = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_nActionStatus = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_sPublisher = "";
        m_sActionTitle = "";
        m_sLocalPath = "";
        m_nUserId = -1;
        m_sUserName = "";
        m_sUserImageUrl = "";
        m_nHasDownloadedBytes = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_nActionBrownTime = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_nActionHotValue = DEFAULT_INVALID_NOINIT_INT_NUMBER;
    }

    ActItemData(const ActItemData& itemobj)
    {
        m_sGuid = itemobj.m_sGuid;
        m_nItemID = itemobj.m_nItemID;
        m_nActionOriginalId = itemobj.m_nActionOriginalId;
        m_sImageHeadUrl = itemobj.m_sImageHeadUrl;
        m_sImageURL = itemobj.m_sImageURL;
        m_sVideoURL = itemobj.m_sVideoURL;
        m_sActName = itemobj.m_sActName;
        m_sActDesc = itemobj.m_sActDesc;
        m_sPublishTime = itemobj.m_sPublishTime;
        m_nActSize = itemobj.m_nActSize;
        m_sActDownloadURL = itemobj.m_sActDownloadURL;
        m_nActType = itemobj.m_nActType;
        m_nActResource = itemobj.m_nActResource;
        m_nSchemId = itemobj.m_nSchemId;
        m_strSchemeName = itemobj.m_strSchemeName;
        m_dwActTimeLen = itemobj.m_dwActTimeLen;
        m_nActionDownloads = itemobj.m_nActionDownloads;
        m_nActionComments = itemobj.m_nActionComments;
        m_nActionLikes = itemobj.m_nActionLikes;
        m_nActionCollectTime = itemobj.m_nActionCollectTime;
        m_nActionBrownTime = itemobj.m_nActionBrownTime;
        m_nActionHotValue = itemobj.m_nActionHotValue;
        m_nActionStatus = itemobj.m_nActionStatus;
        m_sPublisher = itemobj.m_sPublisher;
        m_sActionTitle = itemobj.m_sActionTitle;
        m_sLocalPath = itemobj.m_sLocalPath;
        m_nUserId = itemobj.m_nUserId;
        m_sUserName = itemobj.m_sUserName;
        m_sUserImageUrl = itemobj.m_sUserImageUrl;
        m_nHasDownloadedBytes = itemobj.m_nHasDownloadedBytes;
    }

    ActItemData& operator = (const ActItemData& itemobj)
    {
        m_sGuid = itemobj.m_sGuid;
        m_nItemID = itemobj.m_nItemID;
        m_nActionOriginalId = itemobj.m_nActionOriginalId;
        m_sImageHeadUrl = itemobj.m_sImageHeadUrl;
        m_sImageURL = itemobj.m_sImageURL;
        m_sVideoURL = itemobj.m_sVideoURL;
        m_sActName = itemobj.m_sActName;
        m_sActDesc = itemobj.m_sActDesc;
        m_sPublishTime = itemobj.m_sPublishTime;
        m_nActSize = itemobj.m_nActSize;
        m_sActDownloadURL = itemobj.m_sActDownloadURL;
        m_nActType = itemobj.m_nActType;
        m_nActResource = itemobj.m_nActResource;
        m_nSchemId = itemobj.m_nSchemId;
        m_strSchemeName = itemobj.m_strSchemeName;
        m_dwActTimeLen = itemobj.m_dwActTimeLen;
        m_nActionDownloads = itemobj.m_nActionDownloads;
        m_nActionComments = itemobj.m_nActionComments;
        m_nActionLikes = itemobj.m_nActionLikes;
        m_nActionCollectTime = itemobj.m_nActionCollectTime;
        m_nActionBrownTime = itemobj.m_nActionBrownTime;
        m_nActionHotValue = itemobj.m_nActionHotValue;
        m_nActionStatus = itemobj.m_nActionStatus;
        m_sPublisher = itemobj.m_sPublisher;
        m_sActionTitle = itemobj.m_sActionTitle;
        m_sLocalPath = itemobj.m_sLocalPath;
        m_nUserId = itemobj.m_nUserId;
        m_sUserName = itemobj.m_sUserName;
        m_sUserImageUrl = itemobj.m_sUserImageUrl;
        m_nHasDownloadedBytes = itemobj.m_nHasDownloadedBytes;
        return *this;
    }
};
//将ActItemData声明为元对象，方便在Qt框架中传送
Q_DECLARE_METATYPE(ActItemData)

struct RecommendActItemData
{
public:
    int m_nRecommendID;  // id
    int m_nRecommendType; // 主题类型
    int m_nRecomendOrder; // 推荐等级
    int m_nRecommendForwardType;  // 主题跳转类型

    QString m_strRecommendImage;  // banner主题图片

    QList<ActItemData *> m_lstActionItems;
public:
    RecommendActItemData()
    {
        m_nRecommendID = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_nRecommendType = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        m_nRecommendForwardType = DEFAULT_INVALID_NOINIT_INT_NUMBER;

        m_strRecommendImage = "";
    }

    ~RecommendActItemData()
    {
        for (int i = 0; i < m_lstActionItems.size(); i++)
        {
            SAFE_DELETE(m_lstActionItems[i]);
        }
        m_lstActionItems.clear();
    }

    void addItem(ActItemData *pItem)
    {
        if (m_lstActionItems.contains(pItem))
        {
            return;
        }

        m_lstActionItems.append(pItem);
    }

//    RecommendActItemData(const RecommendActItemData &otherItem) : ActItemData(otherItem)
//    {
//        m_nRecommendID = otherItem.m_nRecommendID;
//        m_nRecommendType = otherItem.m_nRecommendType;
//        m_strRecommendImage = otherItem.m_strRecommendImage;
//    }

//    RecommendActItemData &operator = (const RecommendActItemData &otherItem)
//    {
//        if (this == &otherItem)
//        {
//            return *this;
//        }

//        ActItemData::operator =(otherItem);

//        m_nRecommendID = otherItem.m_nRecommendID;
//        m_nRecommendType = otherItem.m_nRecommendType;
//        m_strRecommendImage = otherItem.m_strRecommendImage;

//       return *this;
//    }
};




////////////////////////////////////////////////////////////////////////////////////////
/*****************************/
//全局变量定义BEGIN
/*****************************/

#include "../zxLib/zxLib.h"
using namespace zxLib;

//extern zxThreadPool g_threadpool; //全局线程池

#define ALPHA_APP_TYPE 1  //客户端类型
#define MAX_DESCRIPTION_SIZE 30   //分组描述字符数

#define RECOMMEND_TYPE 3 // recommend类型

//extern emUbtProductType UBXBase::getInstance()->getCurrentRobotProductType();
extern QString    g_sActPath;                       //动作库路径
extern QString    g_sDownloadedActInfoXMLPath;      //已下载动作列表配置文件路径
extern QString    g_sDownloadingActInfoXMLPath;     //正在下载动作列表配置文件路径

extern QString      g_sServer;  //服务器地址
extern QString      g_sUpdateURL;  //更新地址

//动作类型前缀
//extern const QString g_sActTypePrefix[];


extern const QString g_sIsRecordLogAppName;    //是否记录日志节名
extern const QString g_sIsRecordLogKeyName;    //是否记录日志键名
extern bool          g_bIsRecordLogValue;      //是否记录日志键值

#if 1
extern int           g_nBigFrameDelay;    //大帧延时，单位毫秒
extern int           g_nSmallFrameDelay;  //小帧延时，单位毫秒
#endif

/*****************************/
//全局变量定义END
/*****************************/



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*****************************/
//全局函数定义BEGIN
/*****************************/
int AddVectorData(vector<char>* pVerData, char* pData, int nLen);
void ModifyVertorData(vector<char>& pVerData, int nPos, char* pData, int nLen);
int CopyVertorToPointer(vector<char>& pVerData, char* pData);

/**************************************************************************
* 函数名: UnicodeToAnsi
* 功能: Unicode字符串转换为Ansi字符串
* 参数:
*    @[in ] sUnicodeStr: Unicode字符串
*    @[out] pOutStr: Ansi字符串，需在外释放
* 返回值: Ansi字符串长度
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
int UnicodeToAnsi(const QString& sUnicodeStr, char*& pOutStr);

/**************************************************************************
* 函数名: UnicodeToUtf8
* 功能: Unicode字符串转换为Utf8字符串
* 参数:
*    @[in ] sUnicodeStr: Unicode字符串
*    @[out] pOutStr: Utf8字符串，需在外释放
* 返回值: Utf8字符串长度
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
int UnicodeToUtf8(const QString& sUnicodeStr, char*& pOutStr);

/**************************************************************************
* 函数名: AnsiToUnicode
* 功能: Ansi字符串转换为Unicode字符串
* 参数:
*    @[in ] pszAnsiStr: Ansi字符串
* 返回值: Unicode字符串
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
QString AnsiToUnicode(const char* pszAnsiStr);

/**************************************************************************
* 函数名: AnsiToUtf8
* 功能: Ansi字符串转换为Utf8字符串
* 参数:
*    @[in ] pszAnsiStr: Ansi字符串
*    @[out] pOutStr: Utf8字符串，需在外释放
* 返回值: Utf8字符串长度
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
int AnsiToUtf8(const char* pszAnsiStr, char*& pOutStr);

/**************************************************************************
* 函数名: Utf8ToAnsi
* 功能: Utf8字符串转换为Ansi字符串
* 参数:
*    @[in ] pszUtf8Str: Utf8字符串
*    @[out] pOutStr: Ansi字符串，需在外释放
* 返回值: Ansi字符串长度
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
int Utf8ToAnsi(const char* pszUtf8Str, char*& pOutStr);

/**************************************************************************
* 函数名: Utf8ToAnsiEx
* 功能: Utf8字符串转换为Ansi字符串
* 参数:
*    @[in ] pszUtf8Str: Utf8字符串
*    @[in ] nUtf8Len: Utf8数据长度
* 返回值: Ansi字符串
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
string Utf8ToAnsiEx(const byte *pszUtf8Str, int nUtf8Len);

/**************************************************************************
* 函数名: Utf8ToUnicode
* 功能: Utf8字符串转换为Unicode字符串
* 参数:
*    @[in ] pszUtf8Str: Utf8字符串
* 返回值: Unicode字符串长度
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
QString Utf8ToUnicode(const char* pszUtf8Str);

/**************************************************************************
* 函数名: AnsiToWChar
* 功能: Ansi字符串转换为WChar字符串
* 参数:
*    @[in ] pszAnsiStr: Ansi字符串
*    @[in ] nAnsiLen: Ansi字符串字节长度
*    @[out] pWCharStr: WChar字符串
* 返回值: WChar字符串字节长度
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
int AnsiToWChar(const char* pszAnsiStr, int nAnsiLen, WCHAR* pWCharStr);

/**************************************************************************
* 函数名: WCharToAnsi
* 功能: WChar字符串转换为Ansi字符串
* 参数:
*    @[in ] pWCharStr: WChar字符串
*    @[in ] nWCharLen: WChar字符串字节长度
*    @[out] pszAnsiStr: Ansi字符串
* 返回值: Ansi字符串字节长度
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
int WCharToAnsi(const WCHAR* pWCharStr, int nWCharLen, char* pszAnsiStr);

/**************************************************************************
* 函数名: PathFileExists
* 功能: 判断路径是否存在
* 参数:
*    @[in ] sPath: 路径
* 返回值: 存在返回true，失败返回false
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
bool PathFileExists(const QString& sPath);

/**************************************************************************
* 函数名: GetFormatSizeString
* 功能: 获取格式化后的大小字符串
* 参数:
*    @[in ] nSize: 大小
* 返回值: 格式化后的大小字符串
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
QString GetFormatSizeString(qint64 nSize);

/**************************************************************************
* 函数名: WCSLen
* 功能: 获取WCHAR字符串元素长度
* 参数:
*    @[in ] pWchar: WCHAR字符串
* 返回值: 字符串元素长度
* 时间: 2015/12/25 17:38
* 作者: ZZP
*/
int  WCSLen(WCHAR * pWchar);

/**************************************************************************
* 函数名: QStringToWCHAR
* 功能: QString字符串转换为WCHAR字符串
* 参数:
*    @[out] pWchar: WCHAR字符串
*    @[in ] str: QString字符串
* 返回值: WCHAR字符串长度
* 时间: 2015/12/25 17:38
* 作者: ZZP
*/
int  QStringToWCHAR(OUT WCHAR* pWchar, IN const QString& str);

/**************************************************************************
* 函数名: WCHARToQString
* 功能: WCHAR字符串转换为QString字符串
* 参数:
*    @[in ] pwchar: WCHAR字符串
*    @[in ] nbyteLen: WCHAR字符串长度
* 返回值: QString字符串
* 时间: 2015/12/25 17:38
* 作者: ZZP
*/
QString  WCHARToQString(IN WCHAR* pwchar, IN int nbyteLen);

//四舍五入函数，nDigits为四舍五入的位数，如nDigits为2 那么Round(2.378)为2.38
float Round(float fValue , int nDigits);

//时间字符串定制函数
QString TimeStringCustomise(int nHour , int nMinute ,int nSec);


/**************************************************************************
* 函数名: QtPostMessage
* 功能: 发送消息
* 参数:
*    @[in ] pDest: 目标窗口
*    @[in ] dwMsg: 消息ID
*    @[in ] dwContext: 消息上下文
*    @[in ] wParam: 消息数据长度
*    @[in ] lParam: 消息数据
* 返回值: void
* 时间: 2016/02/01
* 作者: ZDJ
*/
void QtPostMessage(QObject* pDest, DWORD dwMsg, DWORD dwContext, WPARAM wParam, LPARAM lParam);

#ifdef QUAZIP_ENABLE

/**************************************************************************
* 函数名: UnZipFile
* 功能: 解压zip文件
* 参数:
*    @[in ] sZipPath: zip文件路径
*    @[in ] pszDestDir: 解压文件夹路径，为NULL时解压到当前文件夹
* 返回值: 成功返回true，失败返回false
* 时间: 2016/03/10 16:59
* 作者: ZDJ
*/
bool UnZipFile(const QString& sZipPath, const char* pszDestDir = NULL);

/**************************************************************************
* 函数名: ZipFile
* 功能: 压缩为zip文件
* 参数:
*    @[in ] vecFilePathList: 待压缩文件路径列表
*    @[in ] sZipPath: zip文件路径
* 返回值: 成功返回true，失败返回false
* 时间: 2016/03/10 16:59
* 作者: ZDJ
*/
bool ZipFile(const vector<QString>& vecFilePathList, const QString& sZipPath);

#endif


/**************************************************************************
* 函数名: LoadConfig
* 功能: 加载配置数据
* 时间: 2016/04/26 17:52
* 作者: ZDJ
*/
void LoadConfig();

#ifdef Q_OS_WIN
/**************************************************************************
* 函数名: delayUsec
* 功能: 微秒级延时
* 参数:
*    @[in ] uDelay: 延时时长，单位微秒
* 返回值: void
* 时间: 2016/06/06
* 作者: ZDJ
*/
void delayUsec(int uDelay);
#endif

#ifdef USEXML
/**************************************************************************
* 函数名: ReadModelXml
* 功能: 读取3D模型的xml配置文件
* 参数:
*    @[in ] element: dom元素
*    @[out] motorlist: 舵机数据列表
* 返回值: void
* 时间: 2016/01/26 10:15
* 作者: ZDJ
*/
void ReadModelXml(const QDomElement& element, vector<MOTOR_DATA*>& motorlist);

/**************************************************************************
* 函数名: ReadModelXml
* 功能: 读取3D模型的xml配置文件
* 参数:
*    @[in ] sXmlFilePath: xml文件路径
*    @[out] motorlist: 舵机数据列表
* 返回值: void
* 时间: 2016/01/26 10:15
* 作者: ZDJ
*/
void ReadModelXml(const QString& sXmlFilePath, vector<MOTOR_DATA*>& motorlist);

/**************************************************************************
* 函数名: getMotorCount
* 功能: 获取舵机数量
* 时间: 2016/06/23
* 作者: ZDJ
*/
int getMotorCount();

#endif


/*****************************/
//全局函数定义END
/*****************************/

/**************************************************************************
* 函数名: containItem
* 功能: 判断是否包含指定动作Item
* 时间: 2016/06/23
* 作者: hwx
*/
bool containItem(ActItemData &item, vector<ActItemData> &vecItems);

#endif

