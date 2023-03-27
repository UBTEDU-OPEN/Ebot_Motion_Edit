#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QStyleOption>

#include "mp3widget.h"
#include "mp3container.h"
#include "cimportmp3progress.h"
#include "LayerUI/ILayerOptionHandler.h"
#include "../zxLib/widget/ubxwidget.h"


namespace Ui {
class MainWidget;
}

class MainWidget : public UBXWidget, public ILayerOptionHandler, public zxObserverBaseEx
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

public:
    // 观察者模式通知消息
    virtual void OnNotify(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam);

public:
    //ILayerOptionHandler接口实现
    virtual bool Connect(QWidget* pParent, ILayerOptionHandler_Event* pEvent);
    virtual void ReleaseConnection();
    virtual QWidget *GetWidget();
    virtual void SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID);
    virtual int GetCurrentSelectCol();
    virtual void SetTimeScale(int timeScale);
    virtual void UpdateMP3Action(int nGroupID, float fTime);
    virtual void UpdateMP3Action(int nCol);
    virtual void HasMotionData(bool hasData);//通知MP3是否有动作帧，以便刷新按钮状态
    virtual void SetCellCount(int nCount);
    virtual void MotionPlayStop(int nPlayStatus) Q_DECL_OVERRIDE;
    virtual bool IsMusicLoaded();//判断音乐是否加载，供Motion模块调用

    virtual void Set3DRobotViewInterface(IUBXRobot3DView* pRobotView) {}

    virtual void SetMusicResource(QString strFilePath);
    virtual void PlayMusic(int nStartTime , int nEndTime,emPlayStatus ePlayState);

    virtual void SetMusicBeatsTime(int nBeatTime) {}
    virtual int  GetCurrentPos() {return 0;}
    virtual void SetControlState(bool controlState, int nLayerType = 1);//ACTION_LAYER_MOTION		1
    virtual void InsertGroupByMenu(int nCol);
    virtual void AddGroupByMenu() {}
    virtual void LayerDebugStop() {}

signals:
    //定义成信号,停止函数调用在其他线程中，因此通过信号和槽来完成
    void SigMotionPlayStop(emPlayStatus eStatus);
    void sigOnUpdateMP3Info(MP3Info stMP3Info);
    void SigSetControlState(bool controlState, int nLayerType);
private slots:
    void OnMotionPlayStop(emPlayStatus eStatus);
    void OnSetControlState(bool controlState, int nLayerType);
    void OnReleaseMusic();
    void OnLoadedMusic(MP3Info stMP3Info);

    void OnFinishMusicPlaying();

    void OnLButtonDoubleClick(int nCol);
    void on_btnAddMusic_clicked();

    void on_playMP3_clicked();

    void on_stopMP3_clicked();

    void on_playMotion_clicked();

    void on_stopMotion_clicked();

    void on_btnDeleteMusic_clicked();

protected:
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *)
    {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
    virtual void customEvent(QEvent * event) override;

    virtual void retranslateUi() override;

private:
    void setDefaultStyle();
    void saveLastMusicPath(const QString &strMusicFile);
    QString getLastMusicPath();

    // 订阅消息
    void subscribeMsg();

    // 设置音乐、动作播放按钮、停止按钮、暂停按钮使能状态
    void setMP3PlayBtnState(bool bEnable);
    void setMotionPlayBtnState(bool bEnable);

    // 消息处理函数
    void doProcessUndoRedoMsg(emOperateCode eCode, WPARAM wParam, LPARAM lParam);
    void doProcessSimulateMsg(int nMsg, emPlayStatus eStatus);
    void doProcessStopPlayMsg(int nMsgID, emPlayType eType);
private:
    Ui::MainWidget *ui;
    mp3Container*  m_pMp3Container;
    ILayerOptionHandler_Event *m_pEvent;

    CImportMP3Progress *m_pImportMP3Progress;
    MP3Widget* m_pMP3Widget;

    emPlayStatus m_eMotionPlayStatus;

    bool m_bMotionPlaying;

    bool m_bMusicPlaying;
    bool m_bMusicPause;
    bool m_bHasMotionData;
};

#endif // MAINWIDGET_H
