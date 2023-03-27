/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：mainwindowwidget.h
* 创建时间：2016/03/01
* 文件标识：
* 文件摘要：自绘主窗体背景，并实现边框功能
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/01
* 版本摘要：
*/
#ifndef MAINWINDOWWIDGET_H
#define MAINWINDOWWIDGET_H

#include <QWidget>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QPainter>
#include <QButtonGroup>
#include <QTimer>

#include "../LibTreeNavigation/ILibTreeNavigation.h"
#include "../UBXMotionEditor/UBXMotionEditor.h"
#include "../UBXRobot3DView/IUBXRobot3DView.h"
#include "../LocalActSyncWidget/ILocalActSyncWidget.h"
#include "../UBXLogin/ILibLogin.h"
#include "titlebar.h"

#include "actionlibwidget.h"
#include "actionlistspage.h"
#include "downloadwidget.h"
#include "favoritespage.h"
#include "myuploadpage.h"
#include "actiondetailwidget.h"
#include "widgetupload.h"
#include "widgetuploadaddaction.h"
#include "widgetuploadaddcover.h"
#include "../zxLib/zxObserverBaseEx.h"

#include "dialogsetting.h"
#include "ubxupdateadministrator.h"
#include "businesspromotewidget.h"
#include "cpromotepopupwidget.h"
#include "filessaveasui.h"
#include "robotcontrolwidget.h"
#include "../zxLib/widget/ubxwidget.h"


class AlphaApplication;

namespace Ui {
class MainWindowWidget;
}

class MainWindowWidget : public UBXWidget, public IUBXRobot3DViewEvent,
        public ILocalActSyncWidgetEvent, public ILoginEvent, public zxObserverBaseEx
#ifdef Q_OS_WIN
        , public QAbstractNativeEventFilter
#endif
{
    Q_OBJECT
protected:
    explicit MainWindowWidget(QWidget *parent = 0);

public:
    ~MainWindowWidget();

public:
    static MainWindowWidget* instance();

public:    
    //IUBXRobot3DViewEvent接口实现
    virtual void IUBXRobot3DViewEvent_On3DModelLoaded();
    virtual bool IUBXRobot3DViewEvent_IsRobotDebuging();

    //ILoginEvent
    virtual void SetUserLoginStatus(QString text, QString ActionType, bool bIsLogin);
    virtual void setUserNickName(const QString &strNickName);

    /**************************************************************************
    * 函数名: CheckFile
    * 功能: 检查文件
    * 参数:
    *    @[in out] sFilePath: 检查的文件路径
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2015/08/25 16:12
    * 作者: ZDJ
    */
    BOOL CheckFile(QString& sFilePath);

    //设置窗体默认Style sheet样式
    void SetDefaultStyle();

    // 订阅消息
    void subscribeMsg();

    //创建file菜单
    void createFileMenu();

    // 创建help菜单
    void createHelpMenu();

    //从指定文件名读取文件数据
    bool ReadFromFile(QString strPathName);
    //拷贝文件
    void myCopyDirectory(QString strSource, QString strTarget);

    //拷贝文件并重命名
    void myCopyDirectoryAndRename(QString strSource, QString strTarget, QString sNewName);
    //popup窗体光标样式设置
    void UnsetCursor();
signals:
    //窗体相关信号发送
    void SigFullScreen();
    void SigMin();
    void SigMax();
    void SigClose();
    void SigNormal();
    void SigTextChange(QString text);    
    void SigRestore();

    // 断开状态发生变化的信号
    void sigOnPortOpenChanged(BOOL bOpen);

    // 机器人TF卡状态信号
    void sigOnRcvRobotTFCardStatus(bool bHasTFCard);

protected:
    //以下为重写父类相关函数
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent * event);
    void hideEvent(QHideEvent * event);
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent * event);

    virtual void keyPressEvent(QKeyEvent * event) Q_DECL_OVERRIDE;

    virtual void timerEvent(QTimerEvent *event);
    virtual void retranslateUi();

#ifdef Q_OS_WIN
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;
#endif
private:
    //初始化菜单栏
    void InitMenuBar();
    //初始化标题栏
    void InitTitleBar();
    //初始化编辑界面
    void InitEditPage();
    //初始化动作库界面
    void InitActionLibrayPage();
    //初始化同步界面
    void InitSyncPage();

    // 初始化机器人控制按钮
    void InitRobotControlWidget();

    // 更新机器人控制按钮显示位置
    void updateRobotControlWidgetPos();

    //设置光标样式
    void changeCursor(const QPoint& clientPoint);
    //改变窗体大小
    void changeSize(const QPoint& nowPt);

    void CreateLoginPopMenuActions();
    //绘制窗体边框
    void drawBorder(QPainter &painter);
    //填充窗体背景
    void drawBackGround(QPainter &painter);
    
    // 设置tabWidget tab信息
    void setUIMainLayerLayoutTabText(const QString &sFile, bool bIsTitleToSetToTabText);

    // 显示动作详细信息页
    void showActionDetailedPage(const ActItemData &itemData);

    // 清空撤销重做列表
    void clearUndoRedoList();

    // 设置撤销、重做按钮状态
    void setUndoBtnEnable(bool bEnable);
    void setRedoBtnEnable(bool bEnable);

public slots:
    //新建工程槽函数
    void slotNew();
    //关于槽函数
    void slotAbout();
    //打开工程槽函数
    void slotOpen();
    //保存槽函数
    void slotSave();
    //另存为槽函数
    void slotSaveAs();
    //导出到Hts槽函数
    void slotExportToHts();
    //to xml file
    void slotExportToXml();
    //退出槽函数
    void slotQuit();
    //点击工具栏tab（编辑、动作库等）切换槽函数
    void onClickTab(int nID);
    //关闭窗体槽函数
    void onClose();
    //最小化槽函数
    void onMin();
    //最大化槽函数
    void onMax();
    //窗口还原槽函数
    void onRestore();
    //用户登录槽函数
    void onUserLogin();
    //用户信息编辑槽函数
    void onUserMessageEdit();
    //用户登出槽函数
    void onUserLogout();
    //动作库界面导航栏按钮组槽函数
    void OnActionLibBGToggled(int id, bool checked);
    //下载按钮槽函数
    void OnDownloadBtnClicked();

    void onRefreshVersionIcon();
    void onRefreshSystemEmbedVersionIcon(int nStatus);
    void onRefreshVideoTutorialAlert();

    /************************************
    * 名称: slotOnPortStatusChanged
    * 功能: 机器人连接状态改变的槽函数
    * 返回:   void
    * 时间:   2016/07/27
    * 作者:   hwx
    * */
    void slotOnPortStatusChanged(bool bConnect);

    /************************************
    * 名称: slotOnSettingLanguage
    * 功能: 切换语言
    * 返回:   void
    * 时间:   2016/06/08
    * 作者:   hwx
    * */
    void slotOnSetting();

    /************************************
    * 名称: slotOnOpenHelpDoc
    * 功能: 打开帮助文档
    * 返回:   void
    * 时间:   2016/06/08
    * 作者:   hwx
    * */
    void slotOnOpenHelpDoc();

    /************************************
    * 名称: slotOnVideoTutorial
    * 功能: 打开视频教程帮助
    * 返回:   void
    * 时间:   20160822
    * 作者:   asu
    * */
    void slotOnVideoTutorial();

    /************************************
    * 名称: slotOnCheckUpdateSysEmbedVersion
    * 功能: 检查更新嵌入式软件版本
    * 返回:   void
    * 时间:   2016/07/22
    * 作者:   hwx
    * */
    void slotOnCheckUpdateSysEmbedVersion();

    //
    void slotOnShowSystemPCSoftVersion();

#ifdef Q_OS_MAC
    void slotOnAppStateChanged(Qt::ApplicationState state);
#endif

    /************************************
    * 名称: OnMoreBtnClicked
    * 功能: 更多按钮槽函数
    * 参数: [in]actGroup 动作查询组类型
    * 参数: [in]actType 动作类型
    * 返回:   void
    * 时间:   2016/04/12
    * 作者:   hels
    ************************************/
    void OnMoreBtnClicked(emActionGroup actGroup, emActionType actType);

    /************************************
    * 名称: OnActionPreviewClicked
    * 功能: 点击预览图时响应
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   2016/04/15
    * 作者:   hels
    ************************************/
    void OnActionPreviewClicked(ActItemData itemData);

    /************************************
    * 名称: onFlowCoverActionPreviewClicked
    * 功能: 点击flow流图弹出的子窗口中的动作时的槽函数
    * 参数: [in] nPageId 页标识
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   2016/07/18
    * 作者:   hwx
    ************************************/
    void onFlowCoverActionPreviewClicked(int nPageId, ActItemData itemData);

    /************************************
    * 名称: slotOnRecommendFlowImageClicked
    * 功能: 点击flowcover推荐主题图片时响应
    * 参数: [in]pRecommendItem 对应的推荐动作数据
    * 返回:   void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void slotOnRecommendFlowImageClicked(RecommendActItemData *pRecommendItem);

    /************************************
    * 名称: slotOnBusinessPromoteImageClicked
    * 功能: 点击flowcover活动推广图片时响应
    * 参数: [in]pPromoteItem 活动推广数据
    * 返回:   void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void slotOnBusinessPromoteImageClicked(BusinessPromoteData *pPromoteItem);

    /************************************
    * 名称: slotOnUpdatePopUp
    * 功能: 显示活动推广弹窗
    * 参数: [in]strImageFile 图片文件
    * 返回:   void
    * 时间:   2016/08/31
    * 作者:   hwx
    ************************************/
    void slotOnUpdatePopUp(const QString &strImageFile);

    /************************************
    * 名称: slotOnActionUpload
    * 功能: 上传动作槽函数
    * 参数: [in]nSchemId 运营活动id
    * 返回:   void
    * 时间:   2016/09/09
    * 作者:   hwx
    ************************************/
    void slotOnActionUpload(int nSchemId);

    /************************************
    * 名称: OnGoBack
    * 功能: 响应详情页返回操作
    * 返回:   void
    * 时间:   2016/04/16
    * 作者:   hels
    ************************************/
    void OnGoBack();

    //动作上传按钮槽函数
    void OnActionUploadBtnClicked();

    /************************************
    * 名称: onUpdateDownLoadBtnStatus
    * 功能: update down load button status
    * 参数: void
    * 返回:   void
    * 时间:   20160602
    * 作者:   suhaiwu
    ************************************/
    void onUpdateDownLoadBtnStatus();

    /************************************
    * 名称: onRestoreDownLoadBtn
    * 功能: restore down load button status
    * 参数: void
    * 返回:   void
    * 时间:   20160607
    * 作者:   suhaiwu
    ************************************/
    void onRestoreDownLoadBtn();

    /************************************
    * 名称: slotOnDetailedWidgetMoreClicked
    * 功能: 动作详情页点击更多按钮的槽函数，跳转到动作页
    * 参数：[in] eGroup：动作组类，如最热的
    * 参数：[in] eType：动作类型，如舞蹈
    * 返回:   void
    * 时间:   20160607
    * 作者:   hwx
    ************************************/
    void slotOnDetailedWidgetMoreClicked(emActionGroup eGroup, emActionType eType);

    /************************************
    * 名称: slotOnDelayLoaded
    * 功能: 延时加载的槽函数
    * 返回:   void
    * 时间:   20160901
    * 作者:   hwx
    ************************************/
    void slotOnDelayLoaded();

    /************************************
    * 名称: slotOnMainWinShow
    * 功能: 主界面显示后的槽函数
    * 返回:   void
    * 时间:   20170216
    * 作者:   hwx
    ************************************/
    void slotOnMainWinShow();

    /**************************************************************************
    * 函数名: OnNotify
    * 功能: 通知消息
    * 参数:
    *    @[in ] nMsgID: 消息ID
    *    @[in ] dwContext: 消息上下文
    *    @[in ] wParam: 参数1
    *    @[in ] lParam: 参数2
    * 返回值: void
    * 时间: 2016/06/06
    * 作者: suhaiwu
    */
    virtual void OnNotify(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam);

    /**************************************************************************
    * 函数名: doProcessUTFCardMsg
    * 功能: 处理内存卡消息
    * 参数:
    *    @[in ] wParam: 是否插上内存卡标志
    *    @[in ] lParam:
    * 返回值: void
    * 时间: 2017/03/17
    * 作者: hwx
    */
    void doProcessUTFCardMsg(WPARAM wParam, LPARAM lParam);

    /**************************************************************************
    * 函数名: doProcessSimulateMsg
    * 功能: 处理播放音乐、动作消息
    * 参数:
    *    @[in ] nMsgID: 消息ID
    *    @[in ] eStatus: 播放状态
    * 返回值: void
    * 时间: 2017/03/17
    * 作者: hwx
    */
    void doProcessSimulateMsg(int nMsgID, emPlayStatus eStatus);

    // 设置按钮使能状态，仿真播放时禁用，播放停止时可用
    void setButtonStatus(bool bEnable);
    // when robot type change
    void onRobotProductTypeChange(int nOldType, int nNewType);

private slots:
    void on_pbNew_clicked();
    void on_pbOpen_clicked();
    void on_pbSave_clicked();
    void on_pbSaveAs_clicked();

    // 双击标题栏的槽函数
    void slotOnDoubleClicked();

    void slotOnUndo();  // 撤销
    void slotOnRedo();  // 重做

    void slotOnUndoListSizeChanged(int nSize);  // undo列表size变化的槽函数
    void slotOnRedoListSizeChanged(int nSize);  // redo列表size变化的槽函数

private:
    void clearWaveDir();
    void clearMusicDir();

    void saveOpenProjectPath(const QString &strPath);
    QString getOpenProjectPath();

private:

    enum ResizeType
    {
        UnSet = -1,
        ResizeL,//左边光标样式
        ResizeLT,//左上角光标样式
        ResizeT,
        ResizeRT,
        ResizeR,
        ResizeRB,
        ResizeB,
        ResizeLB
    };

    QPixmap         *m_pPixmapBG;//背景图片，可以设置图片填充背景
    QPixmap         m_pixmapBorder;//边框图片
    QPoint          m_ptPre;//记录前一点，实现窗体移动
    ResizeType      m_resizeType;
    bool            m_bLeftDown;

    ILibTreeNavigation *   m_pITreeNavigation;
    QHBoxLayout* m_pTabWidget;
    IUBXMotionEditor* m_pIUBXmEditor;
    IUBXRobot3DView*    m_pIUBX3DView;
    ILocalActSyncWidget*   m_pILocalActSyncWidget;

    QByteArray m_bytearrGeometry;
    ILogin* m_pLoginDlg;

    ActionLibWidget *m_pActionLibWidget;    //动作页首页
    ActionListsPage *m_pActionListPage;     //动作详情页
    DownloadWidget  *m_pActionDownload;     //动作下载页
    FavoritesPage   *m_pFavoritesPage;      //我的收藏页
    MyUploadPage *m_pMyUploadPage;

    ActionDetailWidget *m_pActionDetail;    //动作详情页
    ActionRecommendPage *m_pActionRecommendPage; // 推荐动作页面
    BusinessPromoteWidget *m_pBusinessPromoteWidget; // 运营推广活动页

    QWidget*    m_pActionListCurWidget;
    WidgetUploadAddAction  *m_pActionUploadAddAct;     //动作上传页
    WidgetUpload  *m_pActionUploadMain;     //动作上传页

    int m_nIdForActionWidget;//进入详情页之前的窗体对应的按钮ID，用于处理返回操作

    QButtonGroup m_btnGroupForActionLib;//管理动作库Tab的导航栏按钮组

private:
    QMenu* m_FileMenu;
    QAction* m_newAct;
    QAction* m_aboutAct;
    QAction* m_openAct;
    QAction* m_saveAct;
    QAction* m_saveAsAct;
    QAction* m_exportHts;
    QAction* m_Quit;

    QMenu *m_pSystemMenu;  // 系统菜单
    QAction *m_pSettingAction; // 设置动作

    QMenu *m_pAAMenu;

    CMenu *m_pHelpMenu; // help菜单
    QAction *m_pOpenHelpDocAction; // 打开帮助文档动作
    QAction *m_pOpenVideoTutorialAction; //
    QAction *m_pActionCheckFirmwareVersion; // 查看固件版本
    QAction  *m_pActionRobotPCSoftVersion;  //


private:
    Ui::MainWindowWidget *ui;
    QMenuBar* m_pMenuBar;
    TitleBar* m_pTitleBar;

    SettingDialog *m_pSettingDialog; // 设置界面
    bool m_bRobotCtrlWidgetHasFirstShow; // 机器人全局控制按钮已经显示
    CRobotControlWidget *m_pRobotControlWidget;  // 机器人控制按钮

    QString m_strBntDownLoadsOldStyle;

//    QString m_strSaveFileName;
    SFileSaveAsDatas m_saveAsDatas;

    QTimer m_timer;

    CPromotePopUpWidget *m_pPromotePopUpWidget; // 推广弹窗

    static MainWindowWidget *m_pMainWindow;

    bool m_bProjectTitleTabTextHadSet;
};

#define pMainWindow  (MainWindowWidget::instance())

#endif // MAINWINDOWWIDGET_H
