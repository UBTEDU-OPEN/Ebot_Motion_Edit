/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：WidgetUploadAddAction.h
* 创建时间：2016/05/18 17:15
* 文件标识：
* 文件摘要：动作上传添加动作界面
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2016/05/18 17:15
* 版本摘要：
*/
#ifndef WIDGETUPLOADADDACTION_H
#define WIDGETUPLOADADDACTION_H

#include <QWidget>
#include <QMouseEvent>


#include "../zxLib/widget/ubxwidget.h"

namespace Ui {
class WidgetUploadAddAction;
}

class UBXACTIONLIBSHARED_EXPORT WidgetUploadAddAction : public UBXWidget
{
    Q_OBJECT

public:
    explicit WidgetUploadAddAction(QWidget *parent = 0);
    ~WidgetUploadAddAction();

    /************************************
    * 名称: setDefaultStyle
    * 功能: 设置窗体默认样式
    * 返回:   void
    * 时间:   2016/03/31
    * 作者:   hels
    ************************************/
    virtual void setDefaultStyle();

    /************************************
    * 名称: getActionID
    * 功能: 获取动作ID
    * 时间: 2016/06/04 13:14
    * 作者: ZDJ
    */
    qint64 getActionID()const;


    /************************************
    * 名称: getActTotalTime
    * 功能: 获取动作total time, union is second
    * 时间: 2016/07/25
    * 作者: asu
    */
    qint64 getActTotalTime()const;

    /************************************
    * 名称: ClearContents
    * 功能: 清除界面内容
    * 时间: 2016/06/15 15:48
    * 作者: ZDJ
    */
    void ClearContents();

    /************************************
    * 名称: getZipPath
    * 功能: 获取zip文件路径
    * 时间: 2016/06/15 15:48
    * 作者: ZDJ
    */
    QString getZipPath();

    /************************************
    * 名称: setSchemId
    * 功能: 设置活动id
    * 参数: [in] nId 活动id
    * 时间: 2016/09/09 15:48
    * 作者: hwx
    */
    void setSchemId(int nId);

    RETRANSLATEUIFUNC

protected:
    virtual void paintEvent(QPaintEvent *);


    virtual void mouseMoveEvent(QMouseEvent* event);


    virtual void mousePressEvent(QMouseEvent* event);


    virtual void mouseReleaseEvent(QMouseEvent* event);

    virtual void showEvent(QShowEvent* event);

private slots:
    //动作选择点击事件槽
    void onactsel_clicked();
    
    //音频选择点击事件槽
    void onaudiosel_clicked();
    
    //下一步点击事件槽
    void onnextstep_clicked();

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

    //关闭事件槽
    void onClose();

public:
    QWidget*   m_pNextWidget;   //下一步窗口
    int         m_nUploadPercent;  //上传进度百分比

private:
    QPoint      m_dragPosition;

    int    m_nSchemId;   // 活动id

    QString     m_sActPath;        //文件路径
    QString     m_sAudioPath;     //music文件路径

    QString     m_sZipPath;        //zip文件路径
    QAtomicInt  m_nRequestID;      //请求ID


private:
    Ui::WidgetUploadAddAction *ui;
};

#endif // WIDGETUPLOADADDACTION_H
