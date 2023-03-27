/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：ActUpdateMangr.h
* 创建时间：2015/09/19 16:27
* 文件标识：
* 文件摘要：动作表更新管理器
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/09/19 16:27
* 版本摘要：
*/
#ifndef ACTUPDATEMANGR_H
#define ACTUPDATEMANGR_H

#include <QMap>
#include <QUuid>
#include "QtHttpOperSync.h"

#include "ubxactionlib_global.h"
#include "UBXPublic.h"
#include "actitemdatahandle.h"
#include "datamodel/businesspromotedata.h"
#include "../zxLib/inotify.h"
#include "ubxactionlibconstants.h"
#include "datamodel/usercommentitem.h"

#ifdef USEQINIU
extern "C"
{
#include "io.h"
#include "resumable_io.h"
#include "rs.h"
}
#endif

class HttpTransfer;
class HttpDownload;
class CMarkup;

extern const UINT NOTIFY_MSG_ACTLIB_QUERYLIST_SEND;     //发送查询动作列表数据消息
extern const UINT NOTIFY_MSG_ACTLIB_QUERYLIST_RECV;     //查询动作列表数据消息
extern const UINT NOTIFY_MSG_ACTLIB_DOWNLOAD_PROGRESS;  //动作表下载进度消息
extern const UINT NOTIFY_MSG_ACTLIB_DOWNLOAD_RESULT;    //动作表下载结果消息
extern const UINT NOTIFY_MSG_ACTLIB_DOWNLOAD_READY;     //准备下载动作表消息
extern const UINT NOTIFY_MSG_ACTLIB_SYNCACTION_PROCESS;        //同步动作进度通知消息



////帐号类型
//enum emAccountType
//{
//	eAccountTypeEmail = 1,   //邮箱
//	eAccountTypeCellphone    //手机
//};

//用户数据
typedef struct ACTIONUSER
{
    char  szUserName[512];  //用户名
    char  szUserPwd[512];   //用户密码
    int    nUserId;          //用户ID
    int    nAccountType;     //帐号类型, 1邮箱，2手机号

    STRUCT_INIT(ACTIONUSER)
}ACTIONUSER;

enum emRequestTypeHttp
{
    eReqTypeMyuploadActionsDetail = 1,
};

typedef struct SQUERYACTLISTPARA
{
    int nGroup;
    int nActType;
    int nPageNum;
    int nPageSize;
    int nOldRequestID;
    int nCountryCode;
    int nUserID;

    emRequestTypeHttp eRequestType;

    STRUCT_INIT(SQUERYACTLISTPARA)

}SQUERYACTLISTPARA;


//extern ACTIONUSER   g_actuser;   //全局动作表用户


//typedef QMap<QString, QByteArray>  MapStringDataType;  //字符串映射到数据流
//Q_DECLARE_METATYPE(MapStringDataType)


class UBXACTIONLIBSHARED_EXPORT CActUpdateMangr :public QObject, public zxObserverSubjectBase
{
    Q_OBJECT
public:
    CActUpdateMangr();
    virtual ~CActUpdateMangr();

public:   

    /**************************************************************************
    * 函数名: QueryActList
    * 功能: 查询动作列表
    * 参数:
    *    @[in ] nGroup: 动作分组
    *    @[in ] nActType: 动作类型
    *    @[in ] nPageNo: 页号
    *    @[in ] nPageSize: 页大小    
    *    @[in ] nOldRequestID: 请求ID
    *    @[in ] nCountryCode: 国家码
    * 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间: 2015/10/16 20:26
    * 作者: ZDJ
    */
    int QueryActList(int nGroup, int nActType, int nPageNo, int nPageSize, int nOldRequestID, int nCountryCode);

    /**************************************************************************
    * 函数名: QueryRecommandAct
    * 功能: 查询推荐动作
    * 参数:
    *    @[in ] nRecommendType: 动作分组
    *    @[in ] nOldRequestID: 请求ID
    *    @[in ] nCountryCode: 国家码
    * 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间: 2015/10/16 20:26
    * 作者: ZDJ
    */
    int QueryRecommendList(int nRecommendType, int nCountryCode, int nOldRequestID, const QString &strSysLanguage);

    /**************************************************************************
    * 函数名: queryBusinessPromoteData
    * 功能: 请求运营活动推广数据
    * 参数:
    *    @[in ] nOldRequestID: 请求ID
    * 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间: 2016/08/30 20:26
    * 作者: hwx
    */
    int queryBusinessPromoteData(int nOldRequestID);

    /**************************************************************************
    * 函数名: queryRankActionData
    * 功能: 请求排行榜动作数据
    * 参数:
    *    @[in ] nOldRequestID: 请求ID
    *    @[in ] nOldRequestID: 活动ID
    * 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间: 2016/08/30 20:26
    * 作者: hwx
    */
    int queryRankActionData(int nOldRequestID, int nSchemeID);

    /**************************************************************************
    * 函数名: queryActionComment
    * 功能: 查询动作评论
    * 参数:
    *    @[in ] nActionId: 动作id
    *    @[in ] nPageId: page id
    *    @[in ] nOldRequestID: 请求ID
    * 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间: 2016/09/19 20:26
    * 作者: hwx
    */
    int queryActionComment(int nActionId, int nPageId, int nOldRequestId);

    /**************************************************************************
    * 函数名: addActionComment
    * 功能: 增加动作评论
    * 参数:
    *    @[in ] nActionId: 动作ID
    *    @[in] nUserId: 评论用户id
    *    @[in] nRepliedCommentId: 被回复的评论id
    *    @[in] strComment: 评论内容
    *    @[in] eType: 评论类型
    *    @[in ] nOldRequestID: 请求ID
    * 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间: 2016/09/20 9:48
    * 作者: hwx
    */
    int addActionComment(int nActionId, int nUserId, int nRepliedCommentId, const QString &strComment, Constants::emCommentType eType, int nOldRequestId);

    /**************************************************************************
    * 函数名: queryDownloadImage
    * 功能: 请求下载图片到指定目录
    * 参数:
    *    @[in ] strImgeUrl: 图片Url
    *    @[in] strDownloadPath: 下载保存的路径
    *    @[in ] nOldRequestID: 旧的请求ID
    * 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间: 2016/11/18
    * 作者: hwx
    */
    int queryDownloadImage(const QString &strImgeUrl, const QString strDownloadPath, int nOldRequestID);

    /************************************
    * 名称: QueryFavorites
    * 功能: 查询我的收藏
    * 参数: [in]nOldRequestID 旧的请求ID
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    int QueryFavorites(int nOldRequestID = -1);

    /**************************************************************************
    * 函数名: QueryActParise
    * 功能: 查询动作表点赞数量
    * 参数:
    *    @[in ] nActId: 动作ID
    *    @[out] actParise: 点赞数量
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2015/09/23 9:48
    * 作者: ZDJ
    */
    BOOL QueryActParise(int nActId, int& actParise);

    /**************************************************************************
    * 函数名: DownloadSelectAct
    * 功能: 下载动作表
    * 时间: 2015/09/22 18:30
    * 作者: ZDJ
    */
    void DownloadSelectAct();

    /**************************************************************************
    * 函数名: DownloadProc
    * 功能: 下载处理函数
    * 参数:
    *    @[in ] lpParam: 参数
    * 时间: 2015/09/23 20:15
    * 作者: ZDJ
    */
    void DownloadProc(LPVOID lpParam);

    /**************************************************************************
    * 函数名: QueryActListProc
    * 功能: 查询动作列表
    * 参数:
    *    @[in ] nGroup: 分组类型
    *    @[in ] nActType: 动作类型
    *    @[in ] nPageNum: 页号
    *    @[in ] nPageSize: 页大小
    *    @[in ] nOldRequestID: 请求ID
    *    @[in ] nCountryCode: 国家码
    *    @[out] actitemlist: 动作列表
    *    @[out] nTotalPage: 总页数
    * 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间: 2015/09/23 9:48
    * 作者: ZDJ
    */
//    int QueryActListProc(int nGroup, int nActType, int nPageNum, int nPageSize, int nOldRequestID, int nCountryCode, vector<ActItemData>& actitemlist, int& nTotalPage);
    int QueryActListProc(const SQUERYACTLISTPARA& inParas/*, vector<ActItemData>& actitemlist, int& nTotalPage*/);


    /**************************************************************************
    * 函数名: OnActDownloadNotify
    * 功能: 下载进度通知
    * 时间: 2015/06/03 8:51
    * 作者: ZDJ
    */
    static void CALLBACK OnActDownloadNotify(LPVOID pUser, const QString& sFileName, qint64 nFileTotalSize, qint64 nFileCurSize);

    /**************************************************************************
    * 函数名: DownloadFile
    * 功能: 下载文件
    * 参数:
    * 	@[in ] sFileURL: 文件的网络地址，地址如http://www.test.com/test.exe
    * 	@[in ] sSavePath: 文件的保存路径
    * 	@[in ] procnotify: 通知回调
    * 	@[in ] hUser: 通知目标
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2015/05/15 9:32
    * 作者: ZDJ
    */
    BOOL DownloadFile(
            const QString& sFileURL,
            const QString& sSavePath,
            FILEDOWNPROCESSNOTIFY procnotify/* = NULL*/,
            LPVOID pUser /*= NULL*/);

    /**************************************************************************
    * 函数名: ActUserLogin
    * 功能: 动作表用户登录验证
    * 参数:
    *    @[in ] sUserName: 用户名
    *    @[in ] sPwd: 密码
    *    @[out] nUserId: 用户id
    * 返回值: 登录成功返回TRUE，失败返回FALSE
    * 时间: 2015/10/08 10:15
    * 作者: ZDJ
    */
    BOOL ActUserLogin(const QString& sUserName, const QString& sPwd, int& nUserId);

    /**************************************************************************
    * 函数名: GetLoginFlag
    * 功能: 获取登录标识
    * 时间: 2015/10/08 11:11
    * 作者: ZDJ
    */
    BOOL GetLoginFlag()
    {
        return m_bLoginFlag;
    }

    /**************************************************************************
    * 函数名: ActUserReg
    * 功能: 动作表用户注册
    * 参数:
    *    @[in ] sUserName: 用户名
    *    @[in ] sPwd: 密码
    *    @[in ] nType: 帐号类型，1为邮箱，2为手机号
    * 返回值: 注册成功返回TRUE，失败返回FALSE
    * 时间: 2015/10/08 10:15
    * 作者: ZDJ
    */
    BOOL ActUserReg(const QString& sUserName, const QString& sPwd, int nType);

    /**************************************************************************
    * 函数名: DownloadAct
    * 功能: 下载动作表
    * 参数:
    *    @[in ] actdata: 动作表
    * 返回值: int 请求ID信息
    * 时间: 2015/10/08 16:24
    * 作者: ZDJ
    */
    int DownloadAct(const ActItemData& actdata);

    /************************************
    * 名称: PauseDownload
    * 功能: 暂停下载
    * 参数: [in]nActionID 要暂停下载的Action ID
    * 返回:   void
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    void PauseDownload(int nActionID);

    /************************************
    * 名称: ResumeDownload
    * 功能: 重新下载
    * 参数: [in]itemData 下载项
    * 返回:   int 返回请求ID
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    int ResumeDownload(const ActItemData &itemData);

    /**************************************************************************
    * 函数名: GetDownloadedActList
    * 功能: 获取已下载的动作
    * 参数:
    *    @[out] mapActList: 动作列表
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2015/10/10 16:24
    * 作者: ZDJ
    */
    static BOOL GetDownloadedActList(QMap<int, ActItemData>& mapActList);

    /**************************************************************************
    * 函数名: AddDownloadedActInfo
    * 功能: 添加已下载的动作配置信息
    * 参数:
    *    @[in ] actdata: 动作配置信息
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2015/10/10 16:24
    * 作者: ZDJ
    */
    BOOL AddDownloadedActInfo(const ActItemData& actdata);


    /**************************************************************************
    * 函数名: GetDownloadingActList
    * 功能: 获取正在下载的动作
    * 参数:
    *    @[out] vecActList: 动作列表
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2015/10/10 16:24
    * 作者: ZDJ
    */
    static BOOL GetDownloadingActList(vector<ActItemData>& vecActList);

    /**************************************************************************
    * 函数名: getActList
    * 功能: 获取动作info
    * 参数:
    *    @[out] mapActList: 动作列表
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2016/05/30
    * 作者: suhaiwu
    */
    static BOOL getActList(const QString& strFilePath, QMap<int, ActItemData>& mapActList);

    /**************************************************************************
    * 函数名: AddDownloadingActInfo
    * 功能: 添加正在下载的动作配置信息
    * 参数:
    *    @[in ] actdata: 动作配置信息
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2015/10/10 16:24
    * 作者: ZDJ
    */
    BOOL AddDownloadingActInfo(const ActItemData& actdata);

    /**************************************************************************
    * 函数名: UpdateDownloadingActInfo
    * 功能: 更新正在下载的动作配置信息
    * 参数:
    *    @[in ] actdata: 动作配置信息
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2016/5/24 11:11
    * 作者: 周志平
    */
    BOOL UpdateDownloadingActInfo(const ActItemData& actdata);

    /**************************************************************************
    * 函数名: RemoveDownloadedActInfo
    * 功能: 移除已下载动作配置信息
    * 参数:
    *    @[in ] sActGuid: 动作GUID
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2015/10/16 19:10
    * 作者: ZDJ
    */
    static BOOL RemoveDownloadedActInfo(const QString& sActGuid);

    /**************************************************************************
    * 函数名: RemoveDownloadingActInfo
    * 功能: 移除正在下载动作配置信息
    * 参数:
    *    @[in ] sActGuid: 动作GUID
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2015/10/16 19:10
    * 作者: ZDJ
    */
    BOOL RemoveDownloadingActInfo(const QString& sActGuid);

    /**************************************************************************
    * 函数名: PariseAction
    * 功能: 动作表点赞
    * 参数:
    *    @[in ] nUserId: 用户ID
    *    @[in ] nActID: 动作表ID
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2015/10/17 17:16
    * 作者: ZDJ
    */
    BOOL pariseAction(int nUserId, int nActId);

    /**************************************************************************
    * 函数名: FavourAction
    * 功能: 收藏动作表
    * 参数:
    *    @[in ] nUserId: 用户ID
    *    @[in ] nActId: 动作ID
    * 返回值: 成功返回1，失败返回-1，已收藏返回0
    * 时间: 2015/09/23 9:48
    * 作者: ZDJ
    */
    int FavourAction(int nUserId, int nActId);

    /**************************************************************************
    * 函数名: UnfavourAction
    * 功能: 取消收藏动作表
    * 参数:
    *    @[in ] nUserId: 用户ID
    *    @[in ] nActId: 动作ID
    * 返回值: 成功返回1，失败返回-1
    * 时间: 2015/09/23 9:48
    * 作者: ZDJ
    */
    int UnfavourAction(int nUserId, int nActId);

    /**************************************************************************
    * 函数名: QueryActParise
    * 功能: 查询动作收藏状态
    * 参数:
    *    @[in ] nUserId: 用户ID
    *    @[in ] nActId: 动作ID
    * 返回值: 返回请求ID
    * 时间: 2015/09/23 9:48
    * 作者: ZDJ
    */
    int QueryActFavour(int nUserId, int nActId);

    /************************************
    * 名称: SetUserID
    * 功能: 设置登录用户ID
    * 参数: [in]userID 用户ID
    * 返回:   void
    * 时间:   2016/04/13
    * 作者:   hels
    ************************************/
    void SetUserID(int userID);

    /************************************
    * 名称: GetUserID
    * 功能: 获取已经登录用户ID
    * 返回:   int 返回当前登录用户ID
    * 时间:   2016/04/13
    * 作者:   hels
    ************************************/
    int GetUserID();

    /************************************
    * 名称: DownLoadMedia
    * 功能: 下载动作视频文件
    * 参数: [in]nActionID 动作ID
    * 参数: [in]strUrl 动作路径
    * 返回:   void
    * 时间:   2016/04/16
    * 作者:   hels
    ************************************/
    void DownLoadMedia(int nActionID, const QString& strUrl);

    int AddDownLoadCount(int nActionID);


    /**************************************************************************
    * 函数名: saveFileActInfoToServer
    * 功能: 保全动作文件到服务器上
    * 参数:
    *    @[in ] sFilePaths: 上传的文件路径
    *    @[in ] actData: actionOriginalId must have to add.
    *    @[in ] nUserID: 用户ID
    *    @[in ] nOldRequestID: 旧请求ID
    *    @[out ] filePathPackedReturn: return the packaged path
    * 返回值: 成功返回正整数，出错返回其他值
    * 时间: 2016/08/15
    * 作者: asu
    */
    int saveFileActInfoToServer(const QString& sFilePath,
                                const ActItemData& actData,
                                int nUserID,
                                int nOldRequestID);

    /**************************************************************************
    * 函数名: UploadActionFile
    * 功能: 上传动作文件
    * 参数:
    *    @[in ] sFilePath: 文件路径
    *    @[in ] nActionOriginalId: 动作nActionOriginalId
    *    @[in ] nUserID: 用户ID
    *    @[in ] nOldRequestID: 旧请求ID
    * 返回值: 成功返回正整数，出错返回其他值
    * 时间: 2015/09/23 9:48
    * 作者: ZDJ
    */
    int UploadActionFile(const QString& sFilePath,
                         qint64 nActionOriginalId,
                         int nUserID,
                         int nOldRequestID);

    /************************************
    * 名称: UploadActionInfo
    * 功能: 上传动作信息
    * 参数:
    *    @[in ] actData: 动作数据
    *    @[in ] sHeadImgUrl: 头像url
    *    @[in ] sDescImgUrl: 描述图片url
    *    @[in ] nActionID: 动作ID
    *    @[in ] nUserID: 用户ID
    *    @[in ] nOldRequestID: 旧请求ID
    * 返回值: 成功返回正整数，出错返回其他值
    * 时间: 2016/05/27 19:27
    * 作者: ZDJ
    */
    int UploadActionInfo(const ActItemData &actData,
                         const QString& sHeadImgUrl,
                         const QString& sDescImgUrl,
                         qint64 nActionID,
                         int nUserID,
                         int nOldRequestID);

    /************************************
    * 名称: UploadActionImage
    * 功能: 上传动作图片，头像或描述图片
    * 参数:
    *    @[in ] eImgType: 图片类型
    *    @[in ] sFilePath: 文件路径
    *    @[in ] nActionID: 动作ID
    *    @[in ] nUserID: 用户ID
    *    @[in ] nOldRequestID: 旧请求ID
    * 返回值: 成功返回正整数，出错返回其他值
    * 时间: 2016/05/27 19:27
    * 作者: ZDJ
    */
    int UploadActionImage(emUploadFileType eImgType, const QString& sFilePath, qint64 nActionID, int nUserID, int nOldRequestID);

#ifdef USEQINIU

    /************************************
    * 名称: UploadActionDescVideo
    * 功能: 上传动作描述视频
    * 参数:
    *    @[in ] sFilePath: 文件路径
    *    @[in ] nActionID: 动作ID
    *    @[in ] nUserID: 用户ID
    *    @[in ] nOldRequestID: 旧请求ID
    *    @[out] strVideoUrl: 视频文件URL
    * 返回值: 成功返回正整数，出错返回其他值
    * 时间: 2016/05/27 19:27
    * 作者: ZDJ
    */
    int UploadActionDescVideo(const QString& sFilePath, qint64 nActionID, int nUserID, int nOldRequestID, QString &strVideoUrl, INotify* pNotifyRecvr = NULL);

#endif

    /************************************
    * 名称: QueryCreateActions
    * 功能: 查询我的新建动作
    * 参数: [in]nOldRequestID 旧的请求ID
    * 返回:   void
    * 时间:   2016/06/08
    * 作者:   Zhouzp
    ************************************/
    int QueryCreateActions(int nOldRequestID, int nActType, int nCurrPageNum, int nPageSize);

    /**************************************************************************
    * 函数名: DeleteCreateActions
    * 功能: 删除我的新建动作
    * 参数:
    *    @[in ] nUserId: 用户ID
    *    @[in ] nActId: 动作ID
    * 返回值: 成功返回1，失败返回-1
    * 时间:   2016/06/08
    * 作者:   Zhouzp
    */
    int DeleteCreateActions(/*int nUserId, */int nActId);

    /************************************
    * 名称: removeRedundantActionFiles
    * 功能: for remove the redundant action files which is not consistent to download xml
    * 返回:   void
    * 时间:   2016/06/01
    * 作者:   suhaiwu
    ************************************/
    static void removeRedundantActionFiles();


    /************************************
    * 名称: removeRedundantActionInfos
    * 功能: for remove the redundant action infos on downloaded xml which is not consistent to dir files
    * 返回:   void
    * 时间:   2016/06/01
    * 作者:   suhaiwu
    ************************************/
    static void removeRedundantActionInfos();

    /**************************************************************************
    * 函数名: ZipFile
    * 功能: 压缩为zip文件
    * 参数:
    *    @[in ] vecFilePathList: 待压缩文件路径列表
    *    @[in ] sZipPath: zip文件路径
    * 返回值: 成功返回true，失败返回false
    * 时间: 20160818
    * 作者: asu
    */
    static bool zipFiles(const vector<QString>& vecFilePathList, const QString& sZipPath);

    /************************************************************************************
   * 函数名: downloadReplierIcon
   * 功能: 下载评论者头像
   * 参数:
   *    @[in ] strUrl: 评论者头像URL
   * 返回值:
   * 时间: 2016/09/20 9:48
   * 作者: hwx
   */
    void downloadReplierIcon(const QString &strUrl);

signals:
    /************************************
    * 名称: SigItemDate
    * 功能: 从服务器获取到列表项信息后发送
    * 参数: [in]requestID 请求ID，用于区分是否是自己关注的请求(由postQueryActList返回)
    * 参数: [in]itemData 列表项数据
    * 参数: [in]nTotalPage 一共有多少页
    * 返回:   void
    * 时间:   2016/03/25
    * 作者:   hels
    ************************************/
    void SigItemDate(int requestID, ActItemData itemData, int nTotalPage);

    void SigItemUpdate();

    /************************************
    * 名称: sigAddRecommendAction
    * 功能: 通知主界面增加推荐动作
    * 参数: [out]actionItem 动作数据
    * 返回:   void
    * 时间:   2016/06/04
    * 作者:   hwx
    ************************************/
    void sigAddRecommendAction(RecommendActItemData *pRecommendItem);

    /************************************
    * 名称: sigOnAddPromoteItem
    * 功能: 添加推广活动数据
    * 参数: [out]pPromoteItem 推广活动数据
    * 返回:   void
    * 时间:   2016/08/31
    * 作者:   hwx
    ************************************/
    void sigOnAddPromoteItem(BusinessPromoteData *pPromoteItem);

    /************************************
    * 名称: sigOnUpdatePromoteData
    * 功能: 更新推广活动数据，更新排行榜
    * 参数: [out]pPromoteItem 推广活动数据
    * 返回:   void
    * 时间:   2016/08/31
    * 作者:   hwx
    ************************************/
    void sigOnUpdatePromoteData(BusinessPromoteData *pPromoteItem);

    /************************************
    * 名称: sigOnDownloadCommentReplierIconDone
    * 功能: 下载评论者图标完成
    * 返回:   void
    * 时间:   2016/09/20
    * 作者:   hwx
    ************************************/
    void sigOnDownloadCommentReplierIconDone();

    /************************************
    * 名称: sigOnReceivedActionComment
    * 功能: 通知其他控件获取到动作评论数据
    * 参数: [out]data 动作评论数据
    * 返回:   void
    * 时间:   2016/09/19
    * 作者:   hwx
    ************************************/
    void sigOnReceivedActionComment(const QByteArray &data);

    /************************************
    * 名称: SigTotalPage
    * 功能: 从服务器获取页码总数后发送信号
    * 参数: [in]requestID 请求ID
    * 参数: [in]nTotalPage 页码总数
    * 返回:   void
    * 时间:   2016/03/25
    * 作者:   hels
    ************************************/
    void SigTotalPage(int requestID, int nTotalPage);    

    /************************************
    * 名称: sigRecordCount
    * 功能: 从服务器获取记录总数后发送信号
    * 参数: [in]requestID 请求ID
    * 参数: [in]nTotalRecordCount   records总数
    * 返回:   void
    * 时间:   2016/06/15
    * 作者:   asu
    ************************************/
    void sigRecordCount(int requestID, int nTotalRecordCount);

    /************************************
    * 名称: SigTotalFavorites
    * 功能: 通知我的收藏总数
    * 参数: [in]requestID 请求ID
    * 参数: [in]nTotals 收藏总数
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void SigTotalFavorites(int requestID, int nTotals);

    /************************************
    * 名称: SigTotalMyUploads
    * 功能: 通知我的收藏总数
    * 参数: [in]requestID 请求ID
    * 参数: [in]nTotals 收藏总数
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void SigTotalMyUploads(int requestID, int nTotals);


    /************************************
    * 名称: SigIsCollectResult
    * 功能: 通知查询是否收藏结果
    * 参数: [in]requestID 请求ID
    * 参数: [in]nActionID 收藏总数
    * 参数: [in]nRet 请求结果
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void SigIsCollectResult(int requestID, int nActionID, int nRet);

    //sig the cancel myupload ret
    void SigCancelMyUpload(int requestID, int nActionID, int nRet);

    /************************************
    * 名称: SigStartDownload
    * 功能: 通知外部开始下载
    * 参数: [in]requestID 对应的请求ID
    * 参数: [in]itemData 下载项
    * 参数: [in]bytesReceived 已经接收到的字节数
    * 参数: [in]bytesTotal 总共的字节数
    * 返回:   void
    * 时间:   2016/04/05
    * 作者:   hels
    ************************************/
    void SigStartDownload(int requestID, ActItemData itemData,  qint64 bytesReceived, qint64 bytesTotal);

    /************************************
    * 名称: SigDownloadProgress
    * 功能: 通知外部下载进度
    * 参数: [in]requestID 对应的请求ID
    * 参数: [in]nActionID 下载项的ID
    * 参数: [in]bytesReceived 已经接收到的字节数
    * 参数: [in]bytesTotal 总共的字节数
    * 返回:   void
    * 时间:   2016/04/01
    * 作者:   hels
    ************************************/
    void SigDownloadProgress(int requestID, int nActionID, qint64 bytesReceived, qint64 bytesTotal);

    /************************************
    * 名称: SigHasPausedDownload
    * 功能: 通知外部已经完成暂停下载
    * 参数: [in]nActionID 暂停的action ID
    * 返回:   void
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    void SigHasPausedDownload(int nActionID);

    void SigIsAddDownLoadResult(int requestID, int nActionID, int nRet);

    /************************************
    * 名称: SigUploadProgress
    * 功能: 文件上传进度信号
    * 参数:
    *    @[in ] sRequestType: 请求类型字符串
    *    @[in ] nRequestID: 请求ID
    *    @[in ] nActionID: 动作ID
    *    @[in ] bytesUploaded: 已上传的数据长度
    *    @[in ] bytesTotal: 计划上传的数据总长度
    * 返回: void
    * 时间: 2016/05/27 19:27
    * 作者: ZDJ
    */
    void SigUploadProgress(const QString& sRequestType, int nRequestID, qint64 nActionID, qint64 bytesUploaded, qint64 bytesTotal);

    /************************************
    * 名称: sigUploadActionResponse
    * 功能: 上传动作应答信号
    * 参数:
    *    @[in ] sRequestType: 请求类型字符串
    *    @[in ] nRequestID: 请求ID
    *    @[in ] nActionID: 动作ID
    *    @[in ] bytesData: 应答数据
    * 返回: void
    * 时间: 2016/06/15
    * 作者: ZDJ
    */
    void sigUploadActionResponse(const QString& sRequestType, int nRequestID, qint64 nActionID, const QByteArray& bytesData);

    /************************************
    * 名称: sigOnDownloadFinish
    * 功能: 动作下载完成后，通知更新进度条和下载状态
    * 参数: nActionId：动作id
    * 返回:   void
    * 时间:   2016/07/14
    * 作者:   hwx
    ************************************/
    void sigOnDownloadFinish(int nActionID);

    /************************************
    * 名称: sigOnHttpRequestFinished
    * 功能: Http请求结束
    * 参数: [in]requestID 请求的ID号
    * 参数: [in]baResponse 请求应答数据
    * 返回: void
    * 时间: 2016/09/26
    * 作者: hwx
    */
    void sigOnHttpRequestFinished(int nRequestId, const QByteArray baResponse);

    /************************************
    * 名称: sigOndownLoadImageDone
    * 功能: 下载图片完成
    * 返回:   void
    * 时间:   2016/11/18
    * 作者:   hwx
    ************************************/
    void sigOndownLoadImageDone(int nRequestId);

protected slots:
    /************************************
    * 名称: OninishedDownload
    * 功能: 下载完成槽函数
    * 参数: [in]requestID 请求的ID号
    * 参数: [in]itemData 对应动作列表数据信息
    * 返回:   void
    * 时间:   2016/04/01
    * 作者:   hels
    ************************************/
    void OnFinishedDownload(int requestID, ActItemData itemData);


private:
    /**************************************************************************
    * 函数名: setXMLData
    * 功能: set data to xml
    * 参数:
    *    @[int]
    * 返回值:void
    * 时间: 2016/06/21
    * 作者: suhaiwu
    */
    static void setXMLData(CMarkup& xmlloader, const ActItemData& actdata);

    /**************************************************************************
    * 函数名: getXMLData
    * 功能: get xml datal
    * 参数:
    *    @[int]
    * 返回值:void
    * 时间: 2016/06/21
    * 作者: suhaiwu
    */
    static void getXMLData(const CMarkup& xmlloader, ActItemData& actdata);

    /**************************************************************************
    * 函数名: RemoveActInfo
    * 功能: 移除已下载动作配置信息
    * 参数:
    *    @[in ] sActGuid: 动作GUID
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2016/06/21
    * 作者: asu
    */
    static BOOL RemoveActInfo(const QString& strFilePath, const QString& sActGuid);


    /**************************************************************************
    * 函数名: AddActInfo
    * 功能: 添加动作配置信息
    * 参数:
    *    @[in ] actdata: 动作配置信息
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2016/06/21
    * 作者: asu
    */
    BOOL AddActInfo(const QString& strFilePath, const ActItemData& actdata)const;

private:
    QtHttpOperSync*   m_pHttpOper;        //http操作类
    BOOL              m_bLoginFlag;       //登录标识

    HttpTransfer*     m_pHttpTrans;       //http传输类,负责动作列表查询
    HttpDownload*     m_pHttpDownload;    //http下载类，负责动作下载

    int               m_nUserID;          //用户ID

    int m_nOldRequestIDOfQiniuUpload;
};

UBXACTIONLIBSHARED_EXPORT extern CActUpdateMangr*    g_actupdatemgr;   //更新动作表管理器

#endif


