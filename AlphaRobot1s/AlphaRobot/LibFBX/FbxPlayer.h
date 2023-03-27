#ifndef FBXPLAYER_H
#define FBXPLAYER_H

#include <QWidget>
#include <QThread>
#include "UBXPublic.h"
class UBXRobot3DWidget;
class UBXRobot3DDisplay;


#define LIBFBX_3DMODEL_FRESH_SMOOTHLY  0
#define LIBFBX_3DMODEL_FRESH_IMMEDIATELY  1
//自定义函数区

bool InitFbxPlay(char *strFilePath,char *strXmlFileName, UBXRobot3DWidget* pWidget, UBXRobot3DDisplay* pDisplay, int nRatio);
bool GetFileLoadStatus();
void DestroyFbxViewWindow();
void PlaySetRotation1(int nDir, float fRot, const char* pNode , int nTime);
void ManualSetRotation1(int nDir, float fRot, const char* pNode , int nTime, TypeAngleChanged nChangeType);
//void SetRotationDate(int nID,int nAngle);
void SetRotionPlayState(bool bState);
void SetModelPlayState(bool bState);
void TriggerModelFreshed(int nType, TypeAngleChanged nChangeType);
const char * GetViewPortData(int& nWidth, int& nHeigth);

void DisplayCallback();
void ReshapeCallback(int width, int height);
void SetCursor(Qt::CursorShape shape);
void UpdateNodeDate(const char* szNodeName, float angle, float offset, TypeAngleChanged nChangeType);

//提供给UBXRobot3DView模块的函数，响应ubxrobot3ddiplay调用
void OnMouseEvent(QMouseEvent *event); //解析事件后对应调用MouseCallback
void OnMotionEvent(QMouseEvent *event); //解析后对应调用MotionCallback
void OnWheelEvent(QWheelEvent *event);
void OnAngleChanged(int nID, int value, int runTime, TypeAngleChanged nType);
void OnRotationSmooth(int nID, float fRot, int nTime);
int GetMotorCount();
QMap<int, int> GetMotorInfo();

//响应控制事件
void OnClickNode(int nID);
void OnReset();
void OnUpdateData();
 //QWidget *GetWidgetPtrByName(const QString& strWidget);
//QWidget* GetWidgetPtrByName(const QString& strWidget);

/************************************
 * 名称: SmoothThread
 * 功能: 平滑线程，利用高精度定时器实现平滑播放动画
 * 时间:   2016/02/14
 * 作者:   hels
 ************************************/
class SmoothThread : public QThread
{
    Q_OBJECT
public:
    SmoothThread();
    ~SmoothThread(){}

    bool m_bStop;
signals:
    void SigNextFrame();
    void SigTriggerModelFreshed();
protected:
    virtual void run();
};

#endif // FBXPLAYER_H
