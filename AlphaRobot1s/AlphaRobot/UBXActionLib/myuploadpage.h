/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：MyUploadPage.h
* 创建时间：20160608
* 文件标识：
* 文件摘要：my upload page is the ui to show my upload
*
* 当前版本：1.0.0.0
* 作    者：asu
*/
#ifndef MYUPLOADPAGE_H
#define MYUPLOADPAGE_H

#include <QWidget>
#include <QButtonGroup>

#include "iactionlistitempreview.h"
#include "actupdatemangr.h"
#include "myuploadlistitempreview.h"
#include "../zxLib/widget/ubxmultipage.h"
#include "../zxLib/widget/ubxwidget.h"

namespace Ui {
class FavoritesPage;
}

class UBXACTIONLIBSHARED_EXPORT MyUploadPage : public UBXWidget
{
    Q_OBJECT

public:
    explicit MyUploadPage(QWidget *parent = 0);
    ~MyUploadPage();

public:    
    /************************************
    * 名称: requestUploadActions
    * 功能: 请求上传动作
    * 参数: [in]eActionType 动作类型
    * 返回:   void
    * 时间:   2016/12/22
    * 作者:   hwx
    ************************************/
    void requestUploadActions(emActionType eActionType);

    /************************************
    * 名称: setDefaultStyle
    * 功能: 设置窗体默认样式
    * 返回:   void
    * 时间:   2016/04/13
    * 作者:   hels
    ************************************/
    void setDefaultStyle();

    /************************************
    * 名称: InitActionListControl
    * 功能: 初始化收藏项列表控件
    * 返回:   void
    * 时间:   2016/04/13
    * 作者:   hels
    ************************************/
    void InitActionListControl();

    /************************************
    * 名称: getActionType
    * 功能: 获取当前选中的动作类型
    * 返回:   动作类型
    * 时间:   2017/02/15
    * 作者:   hwx
    ************************************/
    emActionType getActionType() { return m_emActionType; }

    RETRANSLATEUIFUNC

signals:
    /************************************
    * 名称: SigActionPreviewClicked
    * 功能: 转发DownloadItemDelegate信号，点击预览图时发送，通知外部处理
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   2016/04/15
    * 作者:   hels
    ************************************/
    void SigActionPreviewClicked(ActItemData itemData);

public slots:
    /************************************
    * 名称: onCancelMyUpload
    * 功能: action to cancel result
    * 参数: [in]requestID 请求ID
    * 参数: [in]nActionID 收藏总数
    * 参数: [in]nRet 请求结果
    * 返回:   void
    * 时间:   2016/06/08
    * 作者:   asu
    ************************************/
    void onCancelMyUpload(int requestID, int nActionID, int nRet);

protected:
    /************************************
    * 名称: InitBtns
    * 功能: 初始化按钮组
    * 返回:   void
    * 时间:   2016/04/13
    * 作者:   hels
    ************************************/
    void InitBtns();

    /************************************
    * 名称: InitPagesBtns
    * 功能: 初始化页码按钮组
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void InitPagesBtns();


    /************************************
    * 名称: paintEvent
    * 功能: 摘抄自Qt帮助文档：If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below
    * 返回:   void
    * 时间:   2016/04/13
    * 作者:   hels
    ************************************/
    void paintEvent(QPaintEvent *event);

    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    /************************************
    * 名称: ClearItems
    * 功能: 清空所有数据
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void ClearItems();


protected slots:
    /************************************
    * 名称: OnBtnsButtonToggled
    * 功能: 第一排按钮组槽函数
    * 参数: [in]int-id 发送信号的按钮id
    * 参数: [in]bool-bChecked 按钮是否选中
    * 返回:   void
    * 时间:   2016/04/13
    * 作者:   hels
    ************************************/
    void OnBtnsButtonToggled(int id, bool bChecked);

    /************************************
    * 名称: OnTotalMyUpload
    * 功能: 响应我的收藏总数
    * 参数: [in]requestID 请求ID
    * 参数: [in]nRecordCount  总数
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void OnTotalMyUpload(int requestID, int nRecordCount);

    /************************************
    * 名称: OnItemDate
    * 功能: 列表项数据处理槽函数
    * 参数: [in]requestID 请求ID
    * 参数: [in]itemData 列表项数据
    * 参数: [in]nTotalPage 总页数
    * 返回:   void
    * 时间:   2016/04/13
    * 作者:   hels
    ************************************/
    void OnItemDate(int requestID, ActItemData itemData, int nTotalPage);

    void OnItemUpdate();


    /************************************
    * 名称: onCurrentPageChanged
    * 功能: 当前页发生变化
    * 参数: [in]nCurPage 当前页
    * 返回:   void
    * 时间:   20160826
    * 作者:  asu
    ************************************/
    void onCurrentPageChanged(int nCurPage);

    /************************************
    * 名称: OnPrevPageClicked
    * 功能: 上一页按钮槽函数
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void OnPrevPageClicked();

    /************************************
    * 名称: OnNextPageClicked
    * 功能: 下一页按钮槽函数
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void OnNextPageClicked();

private:
//    /************************************
//    * 名称: SearchItem
//    * 功能: 根据已经保存的所有收藏项，选择指定类型（由于服务器不支持收藏的分页和分类查询，因此自己实现，后续支持了记得改掉）
//    * 参数: [in]emActType 动作类型
//    * 参数: [in]nPage 第几页
//    * 返回:   void
//    * 时间:   2016/04/14
//    * 作者:   hels
//    ************************************/
//    void SearchItem(emActionType emActType, int nPage);


    int m_nRequestID;
    int m_nPagebtns;//页码按钮个数

//    emActionGroup m_emActionGroup;//动作列表组类型
    emActionType m_emActionType;//动作类型

    MyUploadListItemPreview* m_pActListPreviewCol1;//第一列动作列表控件
    MyUploadListItemPreview* m_pActListPreviewCol2;
    MyUploadListItemPreview* m_pActListPreviewCol3;

    QButtonGroup m_btnGroupForButtons;//查询功能按钮组
    UBXMultiPage *m_pMultiPages;  // 多页显示控件

    QList<quint64> m_listActIDHadShow;

private:
     Ui::FavoritesPage *ui;
};

#endif // MYUPLOADPAGE_H
