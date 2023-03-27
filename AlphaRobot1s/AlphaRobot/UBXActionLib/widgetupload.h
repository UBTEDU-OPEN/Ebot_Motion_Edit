/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：WidgetUpload.h
* 创建时间：2016/05/17 17:38
* 文件标识：
* 文件摘要：动作上传界面
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：
* 版本摘要：
*/
#ifndef WIDGETUPLOAD_H
#define WIDGETUPLOAD_H

#include <QWidget>
#include <QMouseEvent>
#include <QStringList>

#include "../zxLib/inotify.h"
#include "../zxLib/widget/ubxwidget.h"


namespace Ui {
class WidgetUpload;
}

class UBXACTIONLIBSHARED_EXPORT WidgetUpload : public UBXWidget, public INotify
{
    Q_OBJECT

public:
    explicit WidgetUpload(QWidget *parent = 0);
    ~WidgetUpload();    

public:
    RETRANSLATEUIFUNC

    void initUI();

    /************************************
    * 名称: setDefaultStyle
    * 功能: 设置窗体默认样式
    * 返回:   void
    * 时间:   2016/03/31
    * 作者:   hels
    ************************************/
    virtual void setDefaultStyle();

    /************************************
    * 名称: ClearContents
    * 功能: 清除界面内容
    * 时间: 2016/06/15 15:48
    * 作者: ZDJ
    */
    void ClearContents();

    void setActionName(const QString& actName);

    // 设置活动id
    void setSchemId(int nId) { m_nSchemId = nId; }

    /************************************
    * 名称: onQiniuNotify
    * 功能: qiniu upload file info to notify
    * 参数:
    *    @[in ] recvr: which to rec the info
    *    @[in ] nBlkCount:
    *    @[in ] nBlkIdx: current blk index
    *    @[in ] nBlkSize:
    *    @[in ] nBlkOffset: have upload size
    * 返回值:
    * 时间: 20160912
    * 作者: asu
    */
    virtual void onQiniuNotify(INotify* recvr, int nBlkCount, int nBlkIdx, int nBlkSize, int nBlkOffset)override;

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mouseMoveEvent(QMouseEvent*event);
    virtual void mousePressEvent(QMouseEvent*event);
    virtual void mouseReleaseEvent(QMouseEvent*event);


private slots:
    //上一步点击事件槽
    void onprestep_clicked();

    //上传点击事件槽
    void onupload_clicked();

    //封面点击事件槽
    void oncoverpic_clicked();

    //描述点击事件槽
    void ondescpic_clicked();

    /************************************
    * 名称: OnUploadProgress
    * 功能: 上传文件进度通知
    * 参数:
    *    @[in ] sRequestType: 请求类型字符串
    *    @[in ] nRequestID:  请求ID
    *    @[in ] nActionID:  动作ID
    *    @[in ] bytesUploaded: 已上传的数据长度
    *    @[in ] bytesTotal: 计划上传的数据总长度
    * 返回: void
    * 时间: 2016/05/27 19:27
    * 作者: ZDJ
    */
    void OnUploadProgress(const QString& sRequestType, int nRequestID, qint64 nActionID, qint64 bytesUploaded, qint64 bytesTotal);

    /************************************
    * 名称: onUploadActionResponse
    * 功能: 上传动作应答槽
    * 参数:
    *    @[in ] sRequestType: 请求类型字符串
    *    @[in ] nRequestID: 请求ID
    *    @[in ] nActionID: 动作ID
    *    @[in ] bytesData: 应答数据
    * 返回: void
    * 时间: 2016/06/15
    * 作者: ZDJ
    */
    void onUploadActionResponse(const QString& sRequestType, int nRequestID, qint64 nActionID, const QByteArray& bytesData);


    /************************************
    * 名称: onQiniuUploadFinish
    * 功能: qiniu上传动作应答槽
    * 参数:
    *    @[in ] nRequestID: 请求ID
    *    @[in ] bOk: 动作ID
    * 返回: void
    * 时间: 20160927
    * 作者: asu
    */
    void onQiniuUploadFinish(const int nRequestID, bool bOk);


    //关闭事件槽
    void onClose();

private:
    //repain the label of desc video
    void repainUploadPercentDesc(const int nUploadPercentDesc);

    //repain the label of Cover
    void repainUploadPercentCover(const int nUploadPercentCover);

public:
    QWidget*   m_pPreWidget;  //上一步窗口
private:
    Ui::WidgetUpload *ui;
    QPoint m_dragPosition;

    QAtomicInt  m_nRequestID;            //请求ID

    int         m_nUploadPercentInfo;    //信息上传进度百分比
    int         m_nUploadPercentHead;    //头像上传进度百分比
    int         m_nUploadPercentDesc;    //描述上传进度百分比

    int         m_nTryTimeForUpLoad;

    int         m_nSchemId; // 运营活动Id

    QString     m_sActionCoverImagePath; //动作封面图片路径
    QString     m_sActionDescFilePath;   //动作描述文件路径
    QStringList m_listActionDescFilePaths;//for delete the videos unused

    QString     m_sActionCoverImageUrl;  //动作封面图片Url
    QString     m_sActionDescImageUrl;   //动作描述图片Url
    QString     m_strActionVideoUrl; // 动作视频在七牛上的URL地址

    bool m_bSetedDescpicToBtn;
    bool m_bEnableReceptQiniuUploadNotify;    
};

#endif // WIDGETUPLOAD_H
