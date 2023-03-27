#include "stdafx.h"
#include "ubxrobot3ddisplay.h"
#include "FbxPlayer.h"
#include <QApplication>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

UBXRobot3DDisplay::UBXRobot3DDisplay(QGLContext *ctx)
    : m_pGLContext(ctx),
      m_bVisible(true)
{
    //定时刷新3D绘制
    m_timer.start(UBX3DVIEW_REFRESH_INTERVAL_TIME, this);
    m_bIsFirstFrame = true;
}

UBXRobot3DDisplay::~UBXRobot3DDisplay()
{
    m_timer.stop();
}

int UBXRobot3DDisplay::getMotorCount()
{
    return GetMotorCount();
}

QMap<int, int> UBXRobot3DDisplay::getMotorInfo()
{
    return GetMotorInfo();
}

void UBXRobot3DDisplay::onSetModelPlayState(bool bstate)
{
    SetModelPlayState(bstate);
}

void UBXRobot3DDisplay::onRotationSmooth(int nID, float fRot, int nTime)
{
    //3D模型运动
    OnRotationSmooth(nID, fRot, nTime);
}

void UBXRobot3DDisplay::onUpdateData()
{
    OnUpdateData();
}

void UBXRobot3DDisplay::onSize(int w, int h)
{
    //在线程事件中调用FbxPlayer对应的函数
    m_pGLContext->makeCurrent();
    ReshapeCallback(w, h);
}


void UBXRobot3DDisplay::onNextFrame()
{
    //在线程事件中调用FbxPlayer对应的函数
    //qDebug()<<"UBXRobot3DDisplay::onNextFrame()::"<<this->thread()<<"\n";
    m_pGLContext->makeCurrent();
    DisplayCallback();    
    m_pGLContext->swapBuffers();
}

void UBXRobot3DDisplay::onTriggerModelFreshed()
{
    TriggerModelFreshed(LIBFBX_3DMODEL_FRESH_SMOOTHLY, TAC_MotionEditor);
}

void UBXRobot3DDisplay::onClickNode(int nID)
{
    //在线程事件中调用FbxPlayer对应的函数
    OnClickNode(nID);
}

void UBXRobot3DDisplay::onReset()
{
    //在线程事件中调用FbxPlayer对应的函数
    OnReset();
}

void UBXRobot3DDisplay::onAngleChanged(int nID, int value, int nTime, TypeAngleChanged nType)
{
    //在线程事件中调用FbxPlayer对应的函数
    OnAngleChanged(nID, value, nTime, nType);
}


void UBXRobot3DDisplay::onMouseEvent(QMouseEvent *event)
{
    qDebug()<<"UBXRobot3DDisplay::"<<this->thread()<<"\n";
    //在线程事件中调用FbxPlayer对应的函数
    OnMouseEvent(event);
    delete event;
}

void UBXRobot3DDisplay::onMouseMotion(QMouseEvent *event)
{
   //在线程事件中调用FbxPlayer对应的函数
   OnMotionEvent(event);
   delete event;
}

void UBXRobot3DDisplay::onMouseWheel(QWheelEvent *event)
{
    //在线程事件中调用FbxPlayer对应的函数
    OnWheelEvent(event);
    delete event;
}

void UBXRobot3DDisplay::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_timer.timerId())
    {
        if(m_bVisible)
        {
            onNextFrame();
            if(m_bIsFirstFrame)
            {
                emit SigFirstFrameFinished();
                m_bIsFirstFrame = false;
            }
        }
    }
}

//获取是否可见
bool UBXRobot3DDisplay::isVisible()
{
    return m_bVisible;
}

//设置是否可见
void UBXRobot3DDisplay::setVisible(bool bVisible)
{
    m_bVisible = bVisible;
}
