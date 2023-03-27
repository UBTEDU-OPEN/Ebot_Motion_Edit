
/****************************************************************************************

Copyright (C) 2015 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

/////////////////////////////////////////////////////////////////////////
//
// This example illustrates how to display the content of a FBX or a OBJ file
// in a graphical window. This program is based on the OpenGL Utility Toolkit
// (GLUT). Start the program on the command line by providing a FBX or a
// OBJ file name. A menu is provided to select the current camera and the current
// animation stack.
//
// Among other things, the example illustrates how to:
// 1)  Use a custom memory allocator
// 2)  Import a scene from a .FBX, .DAE or .OBJ file;
// 3)  Convert the nurbs and patch attribute types of a scene into mesh
//     node attributes; And trianglate all meshes.
// 4)  Get the list of all the cameras in the scene;
// 5)  Find the current camera;
// 6)  Get the relevant settings of a camera depending on it's projection
//     type and aperture mode;
// 7)  Compute the local and global positions of a node;
// 8)  Compute the orientation of a camera;
// 9)  Compute the orientation of a light;
// 10)  Compute the shape deformation of mesh vertices;
// 11) Compute the link deformation of mesh vertices.
// 12) Display the point cache simulation of a mesh.
// 13) Get the list of all pose in the scene;
// 14) Show the scene using at a specific pose.
//
/////////////////////////////////////////////////////////////////////////
#include "FbxPlayer.h"
#include "SceneContext.h"
#include <GL/freeglut.h>
#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QtXml>
#include <QFile>
#include <QElapsedTimer>

#include "../UBXRobot3DView/ubxrobot3dwidget.h"
#include "../UBXRobot3DView/ubxrobot3ddisplay.h"
#include "robotDefine.h"
#include "../UBXBase/UBXBase.h"


using namespace  std;

/*——————————————————————————————————！！！！！！！！！！！！！！！——————————————————————————————————————————————*/
//              fbx sdk 所有的API操作都尽量在同一个线程内实现（FBX SDK IS NOT MULTITHREAD SAFE！）
/*——————————————————————————————————！！！！！！！！！！！！！！！——————————————————————————————————————————————*/

#define ID_TIMER 2


//_LARGE_INTEGER time_start;    /*开始时间*/
//_LARGE_INTEGER time_over;     /*结束时间*/
quint64 g_dwStartTime = 0;
quint64 g_dwEndTime = 0;
//bool g_bHighPrecisionSupported = FALSE;    //是否支持高精度计时器
//double dqFreq;                             /*计时器频率*/
//LARGE_INTEGER f;                           /*计时器频率*/

//int nTimerID = 0;

// 结构体定义区
// 节点旋转信息结构体
#define DEBUG_TIME_PER_FRAME 100 // 仿真时每帧时间
class FbxNodeEx
{
public:
    FbxNode *pFbxNode;
    int nID;
    int nDir;
    float InitAngle; //add by hels for reset的时候直接用FbxPlayer中得这个数据，3DView模块不需要重新解析和保存舵机数据
    float offset; //add by hels for 提供给控制界面控制条将3D坐标角度转为0-180°。（Offset < (offset + fAngle) < 180 + Offset）
    float fAngle;
    float fPreviousAngle;
    char * szNodeName;
    int nTimes;
public:
    FbxNodeEx()
    {
        pFbxNode = NULL;
        szNodeName = NULL;
        nID = 0;
        nDir = 0;
        fAngle = 0.0;
        fPreviousAngle = 0.0;
        nTimes = 0;
    }
    int GetID()
    {
        return nID;
    }
};

typedef struct _FbxNodeRotationInfo
{
  int nDir;
  float fRot;
  char* szNodeName;
  int nTime;
  _FbxNodeRotationInfo()
  {
    nDir = 0;
    fRot = 0;
    szNodeName = NULL;
    nTime = 0;
  }

}FbxNodeRotationInfo,*pFbxNodeRotationInfo;


//变量
UBXRobot3DWidget* g_pWidget = NULL;
UBXRobot3DDisplay* g_pDisPlay = NULL;
static bool gAutoQuit = false;
SceneContext * gSceneContext;
WId g_wid =NULL;
static const char * g_FBXfilepath =NULL;
bool g_bFileLoaded = false;
int g_nServoCount = 0;
FbxNodeEx    *pNodeArray =0;
FbxNodeRotationInfo **ppFbxNodeInfo =0;
typedef list<pFbxNodeRotationInfo> LstNodeInfo;
Qt::HANDLE g_dwMainThreadID = 0;
LstNodeInfo g_lstNodeInfo;
LstNodeInfo g_ManualLstNodeInfo;
//int g_nCurID = 0;
//int g_nCurAngle =0;
bool g_bIsPlayRotion =0;
bool g_bRotionPlayState = false;

bool g_bCover = false;                    // 后面数据是否覆盖前面数据
bool g_bStop = false;
bool g_bPlaying = false;

bool g_bIsPicked = false;//右键点击时，先记录是否选中过节点，当右键UP后复原
bool g_bIsLButtonDown = false;

QElapsedTimer g_elapsedTimer;//高精度定时器

/************************************
 * 名称: SmoothThread
 * 功能: 平滑线程，利用高精度定时器实现平滑播放动画
 * 时间:   2016/02/14
 * 作者:   hels
 ************************************/
SmoothThread::SmoothThread()
{
    m_bStop = false;
}

void SmoothThread::run()
{
    if (g_nServoCount>0)
    {
        ppFbxNodeInfo = new pFbxNodeRotationInfo[g_nServoCount];
    }
    else
        return;
    int nFrameTime = DEBUG_TIME_PER_FRAME;
    if (g_lstNodeInfo.empty())
    {
        return;
    }
    FbxNodeRotationInfo *pInfo = g_lstNodeInfo.front();
    LstNodeInfo::iterator itr = g_lstNodeInfo.begin();
    if(pInfo->nTime < 0)
    {
        return;
    }
    int nSlipTimes = 0;
    if (pInfo->nTime % nFrameTime)
    {
        nSlipTimes = pInfo->nTime / nFrameTime + 1;
    }
    else
        nSlipTimes = pInfo->nTime / nFrameTime;
    for (int j = 0 ;j < g_nServoCount; j++ )
    {
        if (itr == g_lstNodeInfo.end())
        {
            break;
        }
        pNodeArray[j].fAngle = (*itr)->fRot;
        pNodeArray[j].nTimes = (*itr)->nTime;
        pNodeArray[j].nDir = (*itr)->nDir;
        ppFbxNodeInfo[j] = (*itr);
        itr++;
    }
    for(int i = 0 ; i< nSlipTimes ; i++ )
    {
        int nSleepTime = 0;
        if (i == nSlipTimes - 1) //单帧动作最后一次平滑
        {
            nSleepTime = pInfo->nTime - i * DEBUG_TIME_PER_FRAME;
        }
        else
            nSleepTime = DEBUG_TIME_PER_FRAME;
        float fRotPerFrame = 0;
        for (int j = 0 ;j < g_nServoCount; j++ )
        {
            fRotPerFrame = ( ppFbxNodeInfo[j]->fRot - pNodeArray[j].fPreviousAngle ) / nSlipTimes;
            if ( i < nSlipTimes - 1)
            {
                pNodeArray[j].fAngle = pNodeArray[j].fPreviousAngle + (i+1)*fRotPerFrame;
            }
            else
            {
                pNodeArray[j].fAngle = ppFbxNodeInfo[j]->fRot;
            }
            emit SigTriggerModelFreshed();
        }

        //此处发消息，刷新3D模型
        emit SigNextFrame();

        double fDelayMs = 0;
        g_dwEndTime = (g_elapsedTimer.nsecsElapsed() - g_dwStartTime) / 1000000;//转为毫秒
        fDelayMs = nSleepTime - (int)g_dwEndTime;
        if(g_bCover || m_bStop)
        {
            break;
        }
        if(fDelayMs > 0)
        {
            msleep(fDelayMs);
        }
        g_dwStartTime = g_elapsedTimer.nsecsElapsed();
    }
    for (int j = 0 ;j < g_nServoCount; j++)
    {
        if (pNodeArray == NULL)
        {
            break;
        }
        if (pNodeArray[j].szNodeName)
        {
            pNodeArray[j].fPreviousAngle = pNodeArray[j].fAngle;
        }
    }
    if (ppFbxNodeInfo)
    {
        delete []ppFbxNodeInfo;
        ppFbxNodeInfo = NULL;
    }

}

SmoothThread* g_pSmoothThread = NULL;//

//CRITICAL_SECTION               g_crsFbxOperation;
//CRITICAL_SECTION               g_crsFbxNodeArray;
//CRITICAL_SECTION               g_crsFbxManuOperation;



//原始Demo函数定义区
void ExitFunction();
void CameraSelectionCallback(int pItem);
void CameraZoomModeCallback(int pItem);
void AnimStackSelectionCallback(int pItem);
void MenuSelectionCallback(int pItem);
void PoseSelectionCallback(int pItem);
void ShadingModeSelectionCallback(int pItem);
void TimerCallback(int nValue);
void DisplayCallback();
void ReshapeCallback(int pWidth, int pHeight);
void KeyboardCallback(unsigned char pKey, int, int);
void MouseCallback(int button, int state, int x, int y);
void MotionCallback(int x, int y);

//自定义函数区
//void MouseWheelCallback(int button, int dir, int x, int y);
void SetNodeData();
void PlaySimuRotation(int button, int dir, int x, int y);
void SetNodeRotationData(FbxNodeRotationInfo * pInfo, TypeAngleChanged nType);
void SetNodeRotationData(FbxNodeEx * pNodeEx);
void ApplyData2Model();
void ClearFbxNodeInfoList();
void ManualSimuRotation(int nDir,float fRot,const char * pNode, int nTime);
float GetCurrentAngleOfNode(const char * pNodeName, int nDir);
void ApplyData2Node(FbxNode * pNode, int nDir, float fRot, const char* pNodeName);
bool ReadMotorInfoFromXML(char* szXmlFile);


// Menu item ids.
#define PRODUCER_PERSPECTIVE_ITEM   100
#define PRODUCER_TOP_ITEM           101
#define PRODUCER_BOTTOM_ITEM        102
#define PRODUCER_FRONT_ITEM         103
#define PRODUCER_BACK_ITEM          104
#define PRODUCER_RIGHT_ITEM         105
#define PRODUCER_LEFT_ITEM          106
#define CAMERA_SWITCHER_ITEM        107
#define PLAY_ANIMATION              200


const int MENU_SHADING_MODE_WIREFRAME = 300;
const int MENU_SHADING_MODE_SHADED = 301;
const char * MENU_STRING_SHADING_MODE_WIREFRAME = "Wireframe";
const char * MENU_STRING_SHADING_MODE_SHADED = "Shaded";
const char * MENU_STRING_SHADING_MODE_DEFAULTCOLOR = "Defaultcolor";

const int MENU_ZOOM_FOCAL_LENGTH =          401;
const int MENU_ZOOM_POSITION     =          402;

const int MENU_EXIT = 400;

const int DEFAULT_WINDOW_WIDTH = 380;
const int DEFAULT_WINDOW_HEIGHT = 260;

static int g_nFbxWindow = 0;

const int LEFT_BUTTON = 0;
const int MIDDLE_BUTTON = 1;
const int RIGHT_BUTTON = 2;
const int WHEEL_UP = 3;
const int WHEEL_DOWN = 4;

const int BUTTON_DOWN  = 0;
const int BUTTON_UP = 1;

class MyMemoryAllocator
{
public:
    static void* MyMalloc(size_t pSize)
    {
        char *p = (char*)malloc(pSize+1);
        *p = '#';
        return p+1;
    }

    static void* MyCalloc(size_t pCount, size_t pSize)
    {
        char *p = (char*)calloc(pCount, pSize+1);
        *p = '#';
        return p+1;
    }

    static void* MyRealloc(void* pData, size_t pSize)
    {
        if (pData)
        {
            FBX_ASSERT(*((char*)pData-1)=='#');
            if (*((char*)pData-1)=='#')
            {
                char *p = (char*)realloc((char*)pData-1, pSize+1);
                *p = '#';
                return p+1;
            }
            else
            {   // Mismatch
                char *p = (char*)realloc((char*)pData, pSize+1);
                *p = '#';
                return p+1;
            }
        }
        else
        {
            char *p = (char*)realloc(NULL, pSize+1);
            *p = '#';
            return p+1;
        }
    }

    static void MyFree(void* pData)
    {
        if (pData==NULL)
            return;
        FBX_ASSERT(*((char*)pData-1)=='#');
        if (*((char*)pData-1)=='#')
        {
            free((char*)pData-1);
        }
        else
        {   // Mismatch
            free(pData);
        }
    }
};

bool InitFbxPlay(char *strFilePath,char *strXmlFileName, UBXRobot3DWidget* pWidget, UBXRobot3DDisplay* pDisplay, int nRatio)
{
    // Set exit function to destroy objects created by the FBX SDK.
        // Use a custom memory allocator

        FbxSetMallocHandler(MyMemoryAllocator::MyMalloc);
        FbxSetReallocHandler(MyMemoryAllocator::MyRealloc);
        FbxSetFreeHandler(MyMemoryAllocator::MyFree);
        FbxSetCallocHandler(MyMemoryAllocator::MyCalloc);

        Q_ASSERT(pWidget != NULL);
        g_pWidget = pWidget;
        g_pDisPlay = pDisplay;
        //g_wid = wid;
//        char *progname = strFilePath;
//        g_FBXfilepath = progname;
//        int argc =1;
//        static char *argv[] = {strFilePath};
        ReadMotorInfoFromXML(strXmlFileName);
        // glut initialisation
        //glutInit(&argc, argv);
        //glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
        //glutInitWindowSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
        //glutInitWindowPosition(100, 100);
        //g_nFbxWindow = glutCreateWindow("UBX3DViews");
      //  WId widfbx = glutCreateWindow("UBX3DViews");
        //WId subWid = glutCreateSubWindow(macwid, 100, 100, DEFAULT_WINDOW_WIDTH,DEFAULT_WINDOW_HEIGHT);
//        setAccessibleName（"UBX3DViews");
        // Initialize OpenGL.
        const bool lSupportVBO = InitializeOpenGL();  
        // set glut callbacks
        //glutDisplayFunc(DisplayCallback);
        //glutReshapeFunc(ReshapeCallback);
        gSceneContext = new SceneContext(strFilePath, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, lSupportVBO, nRatio);
        gSceneContext ->GetDefaultMotorInfo(strXmlFileName);
//        g_bHighPrecisionSupported = QueryPerformanceFrequency(&f);  //是否支持高精度定时器

        //glutMainLoop();

        //GetWidgetPtrByName("UBX3DViews");
    return lSupportVBO;

}
//QWidget* GetWidgetPtrByName(const QString& strWidget)
//{
//  QWidgetList widgetLst;
//  QWidgetList topWid;
//  QWidget *pWidget = NULL;
//  QString strName;
//  widgetLst = qApp->allWidgets();
//  topWid = qApp->topLevelWidgets();
//  for(int i = 0; i < topWid.size(); i++)
//  {
//      pWidget = topWid.at(i);
//      strName = pWidget->windowTitle();
//      qDebug()<<"WidgetName:"<<strName<<endl;
//      if(strName == strWidget)
//      {
//            break;
//      }
//  }
//}

// Function to destroy objects created by the FBX SDK.
void ExitFunction()
{
    if(gSceneContext)
    {
        delete gSceneContext;
        gSceneContext =NULL;
    }

}

// Select the current camera.
void CameraSelectionCallback(int pItem)
{
    const FbxArray<FbxNode*> & lCameraArray = gSceneContext->GetCameraArray();
    if (pItem == PRODUCER_PERSPECTIVE_ITEM)
    {
        gSceneContext->SetCurrentCamera(FBXSDK_CAMERA_PERSPECTIVE);
    }
    else if (pItem == PRODUCER_TOP_ITEM)
    {
        gSceneContext->SetCurrentCamera(FBXSDK_CAMERA_TOP);
    }
    else if (pItem == PRODUCER_BOTTOM_ITEM)
    {
        gSceneContext->SetCurrentCamera(FBXSDK_CAMERA_BOTTOM);
    }
    else if (pItem == PRODUCER_FRONT_ITEM)
    {
        gSceneContext->SetCurrentCamera(FBXSDK_CAMERA_FRONT);
    }
    else if (pItem == PRODUCER_BACK_ITEM)
    {
        gSceneContext->SetCurrentCamera(FBXSDK_CAMERA_BACK);
    }
    else if (pItem == PRODUCER_RIGHT_ITEM)
    {
        gSceneContext->SetCurrentCamera(FBXSDK_CAMERA_RIGHT);
    }
    else if (pItem == PRODUCER_LEFT_ITEM)
    {
        gSceneContext->SetCurrentCamera(FBXSDK_CAMERA_LEFT);
    }
    else if (pItem == CAMERA_SWITCHER_ITEM)
    {
        gSceneContext->SetCurrentCamera(FBXSDK_CAMERA_SWITCHER);
    }
    else if (pItem >= 0 && pItem < lCameraArray.GetCount())
    {
        gSceneContext->SetCurrentCamera(lCameraArray[pItem]->GetName());
    }
}

void CameraZoomModeCallback(int pItem)
{
    if( pItem == MENU_ZOOM_FOCAL_LENGTH)
        gSceneContext->SetZoomMode( SceneContext::ZOOM_FOCAL_LENGTH);
    else
        gSceneContext->SetZoomMode(SceneContext::ZOOM_POSITION);
}

// Select the current animation stack and set the start, stop and current time.
void AnimStackSelectionCallback( int pItem )
{
    gSceneContext->SetCurrentAnimStack(pItem);
}

void PoseSelectionCallback(int pItem)
{
    gSceneContext->SetCurrentPoseIndex(pItem);
}

void ShadingModeSelectionCallback(int pItem)
{
    if (pItem == MENU_SHADING_MODE_WIREFRAME)
    {
        gSceneContext->SetShadingMode(SHADING_MODE_WIREFRAME);
    }
    else if (pItem == MENU_SHADING_MODE_SHADED)
    {
        gSceneContext->SetShadingMode(SHADING_MODE_SHADED);
    }
    else if (pItem == MENU_SHADING_MODE_SHADED)
    {
        gSceneContext->SetShadingMode(SHADING_MODE_DEFAULTCOLOR);
    }
}

// Exit the application from the main menu.
void MenuSelectionCallback(int pItem)
{
    if (pItem == PLAY_ANIMATION)
    {
        gSceneContext->SetCurrentPoseIndex(-1);
    }
    else if (pItem == MENU_EXIT)
    {
        exit(0);
    }
}


// Trigger the display of the current frame.
void TimerCallback(int nValue)
{
    // Ask to display the current frame only if necessary.
    switch(nValue)
    {

        case 0:
            if (gSceneContext->GetStatus() == SceneContext::MUST_BE_REFRESHED)
            {
                //glutPostRedisplay();
            }
            gSceneContext->OnTimerClick();
            //glutTimerFunc((unsigned int)gSceneContext->GetFrameTime().GetMilliSeconds(), TimerCallback, 0);
            break;

        case 1:
//        glutLeaveMainLoop();
            break;
        case 2:
    //        ApplyDataModel();
    //        glutPostRedisplay();
            break;
        case 3:
//             ManualSetRotation1(g_nCurID,g_nCurAngle);
        break;
    default:
        break;
        // Call the timer to display the next frame.
    }

}


// Refresh the application window.
void DisplayCallback()
{
    gSceneContext->OnDisplay();
    //glutSwapBuffers();
//    if(g_pWidget)
//    {
//        glFlush();
//        g_pWidget->swapBuffers();
//    }
    // Import the scene if it's ready to load.
    if (gSceneContext->GetStatus() == SceneContext::MUST_BE_LOADED)
    {
        // This function is only called in the first display callback
        // to make sure that the application window is opened and a
        // status message is displayed before.
        //qDebug()<<"befor Load File"<<g_bFileLoaded<<endl;
        g_bFileLoaded = gSceneContext->LoadFile();
        //qDebug()<<"after Load File"<<g_bFileLoaded<<endl;
        if (g_bFileLoaded)
        {
            FbxNode *pRootNode = gSceneContext ->GetScene() ->GetRootNode();
            for(int i = 0;i < g_nServoCount;i++)
            {
                if(pNodeArray[i].szNodeName)
                {
                    FbxNode *pNode = pRootNode ->FindChild(pNodeArray[i].szNodeName);
                    if(pNode == NULL)
                        continue;
                    pNodeArray[i].pFbxNode = pNode;
                    SetNodeRotationData(&pNodeArray[i]);
                }
            }
            //glutKeyboardFunc(KeyboardCallback);
            //glutMouseFunc(MouseCallback);
            //glutMotionFunc(MotionCallback);
//            glutMouseWheelFunc(MouseWheelCallback);
        }

//       CreateMenus();

        // Call the timer to display the first frame.
        //glutTimerFunc((unsigned int)gSceneContext->GetFrameTime().GetMilliSeconds(), TimerCallback, 0);
        //glutTimerFunc(200, TimerCallback, 1);
    }

    if( gAutoQuit ) exit(0);
}


// Resize the application window.
void ReshapeCallback(int pWidth, int pHeight)
{
    if(gSceneContext)
    {
        gSceneContext->OnReshape(pWidth, pHeight);
    }
}


// Exit the application from the keyboard.
void KeyboardCallback(unsigned char pKey, int /*pX*/, int /*pY*/)
{

    if(gSceneContext && g_bFileLoaded)
    {
        gSceneContext->OnKeyboard(pKey);
    }

}

/************************************
 * 名称: OnMouseEvent
 * 功能: mouseEvent信号响应函数，由UBXRobot3DViewQtGL发出
 * 访问权限:    全局
 * 参数: [in]MouseEvent *event 鼠标事件指针
 * 返回:   void
 * 时间:   2015/12/28
 * 作者:   hels
 ************************************/
void OnMouseEvent(QMouseEvent *event)
{
    Q_ASSERT(event != NULL);
    int button = -1;
    int state = -1;
    int x = event->pos().x();
    int y = event->pos().y();

    if(event->button() == Qt::RightButton)
    {
        //右键事件
        button = RIGHT_BUTTON;
        switch(event->type())
        {
            case QEvent::MouseButtonPress:
                state = BUTTON_DOWN;
                break;
            case QEvent::MouseButtonRelease:
                state = BUTTON_UP;
                break;
        }
    }
    else if(event->button() == Qt::LeftButton)
    {
        //左键事件
        button = LEFT_BUTTON;
        switch(event->type())
        {
            case QEvent::MouseButtonPress:
                state = BUTTON_DOWN;
                break;
            case QEvent::MouseButtonRelease:
                state = BUTTON_UP;
                break;
        }
    }
    MouseCallback(button, state, x, y);
}

/************************************
 * 名称: OnWheelEvent
 * 功能: wheelEvent信号响应函数，由UBXRobot3DViewQtGL发出
 * 访问权限:    全局
 * 参数: [in]QWheelEvent *event 滚轮事件指针
 * 返回:   void
 * 时间:   2015/12/28
 * 作者:   hels
 ************************************/
void OnWheelEvent(QWheelEvent *event)
{
    int button = WHEEL_UP;
    int x = event->pos().x();
    int y = event->pos().y();
    if(event->delta() > 0)
    {
        button = WHEEL_UP;
    }
    else
    {
        button = WHEEL_DOWN;
    }
    MouseCallback(button, -1, x, y);
}

void MouseCallback(int button, int state, int x, int y)
{
    if(!g_bFileLoaded)
        return;

    switch (button)
    {
    case RIGHT_BUTTON:
        g_bIsLButtonDown = false;
        if(state == BUTTON_DOWN)
        {
            SetCursor(Qt::ClosedHandCursor);
            g_bIsPicked = gSceneContext->GetPickState();
            //新增了左键和checkbox选中节点的功能，在进行右键旋转时先取消选中，否则会改变角度
            gSceneContext->SetPickState(false);
        }
        else
        {
            SetCursor(Qt::ArrowCursor);
            //右键抬起时复原
            gSceneContext->SetPickState(g_bIsPicked);
        }
        break;
    case LEFT_BUTTON:
        if(state == BUTTON_UP)
        {
            SetCursor(Qt::ArrowCursor);
            g_bIsLButtonDown = false;
        }
        else
            g_bIsLButtonDown = true;
        break;
    case MIDDLE_BUTTON:
        break;
    case WHEEL_UP:
         gSceneContext ->OnMouseWheel(1,x,y);
        break;
    case WHEEL_DOWN:
         gSceneContext ->OnMouseWheel(-1,x,y);
         g_bIsLButtonDown = false;
        break;
    default:
        break;
    }
    if(gSceneContext && g_bFileLoaded)
    {
        gSceneContext->OnMouse(button, state, x, y);
    }

}

/************************************
 * 名称: OnMotionEvent
 * 功能: mouseMotion信号响应函数，由UBXRobot3DViewQtGL发出
 * 访问权限:    public
 * 参数: [in]QMouseEvent *event 鼠标事件指针
 * 返回:   void
 * 时间:   2015/12/28
 * 作者:   hels
 ************************************/
void OnMotionEvent(QMouseEvent *event)
{
    Q_ASSERT(event != NULL);
    Q_UNUSED(event);
    int x = event->pos().x();
    int y = event->pos().y();
    MotionCallback(x, y);
}

void MotionCallback(int x, int y)
{
    if(gSceneContext && g_bFileLoaded)
    {
        gSceneContext->OnMouseMotion(x, y);
    }
}

bool GetFileLoadStatus()
{
    return g_bFileLoaded;
}

/**************************************************************************
* 函数名: SetNodeData
* 功 能: 将最新的一帧关节数据平滑的拆分位多帧数据
* 参 数: void
* 时 间: 2015年6月25日15:33:15
* 返回值: void
* 作 者: Nie
**************************************************************************/
//windows版本的代码，改为Qt的实现方式
//void SetNodeData()
//{

//}

/**************************************************************************
* 函数名: ApplyData2Model
* 功 能: 将一帧数据（可能是原始，也可能是拆分之后的）应用到FBX模型之中
* 参 数: void
* 时 间: 2015年6月25日15:34:34
* 返回值: void
* 作 者: Nie
**************************************************************************/
void ApplyData2Model(TypeAngleChanged nType)
{
    if (!g_ManualLstNodeInfo.empty() )
    {
        FbxNodeRotationInfo *pInfo = g_ManualLstNodeInfo.front();
        SetNodeRotationData(pInfo, nType);
        if (pInfo && !pNodeArray)
        {
            delete []pInfo->szNodeName;
            pInfo->szNodeName = NULL;
            delete pInfo;
            pInfo = NULL;
        }
        g_ManualLstNodeInfo.pop_front();
        //glutTimerFunc(10,time)
    }
}
/**************************************************************************
* 函数名: GetCurrentAngleOfNode
* 功 能: 获取当前模型中某一节点的角度值
* 参 数: const char* pNodeName(in,节点名称)  int nDir(in,节点转动方向)
* 时 间: 2015年6月25日15:35:38
* 返回值: 某个节点的当前角度值（float）
* 作 者: Nie
**************************************************************************/
float GetCurrentAngleOfNode(const char* pNodeName , int nDir)
{
    FbxDouble3 d(0,0,0);
    FbxNodeEx *pDstNodeEx = NULL;
    for (int i = 0;i<g_nServoCount;i++)
    {
        if (pNodeArray[i].szNodeName)
        {
            if (!strcmp(pNodeArray[i].szNodeName,pNodeName))
            {
                pDstNodeEx = &pNodeArray[i];

                break;
            }
        }
    }
    float f = 0;
    if (pDstNodeEx)
    {
        f = pDstNodeEx->fAngle;
        if (nDir < 0)
        {
            f = -f;
        }
    }
    return f;
    // 	FbxNode *pRootNode = gSceneContext->GetScene()->GetRootNode();
    // 	FbxNode * pDstNode = pRootNode->FindChild(pNodeName);
    // 	if (pDstNode == NULL)
    // 	{
    // 		return 0;
    // 	}
    // 	d = pDstNode->LclRotation.Get();
    // 	int f = d.mData[abs(nDir) - 1];
    // 	if (nDir >= 0)
    // 	{
    // 		return f;
    // 	}
    // 	else
    // 		return  -f;

}
/**************************************************************************
* 函数名: ClearFbxNodeInfoList
* 功 能: 清空节点信息列表
* 参 数: void
* 时 间: 2015年6月25日15:36:58
* 返回值: void
* 作 者: Nie
**************************************************************************/
void ClearFbxNodeInfoList()
{
//    WaitForSingleObject(hQuitEvent,2000);
    if (!g_lstNodeInfo.empty())
    {
        for (LstNodeInfo::iterator iter = g_lstNodeInfo.begin(); iter != g_lstNodeInfo.end(); iter++)
        {
            if (*iter)
            {
                delete []((*iter)->szNodeName);
                (*iter)->szNodeName = NULL;
                delete *iter;
                *iter = NULL;
            }
        }
        g_lstNodeInfo.clear();
    }

    if (!g_ManualLstNodeInfo.empty())
    {
        for (LstNodeInfo::iterator iter = g_ManualLstNodeInfo.begin(); iter != g_ManualLstNodeInfo.end(); iter++)
        {
            if (*iter)
            {
                delete []((*iter)->szNodeName);
                (*iter)->szNodeName = NULL;
                delete *iter;
                *iter = NULL;
            }
        }
        g_ManualLstNodeInfo.clear();
    }

}

/**************************************************************************
* 函数名: ManualSetRotation1
* 功 能: 模型立即旋转接口（设置之后模型立即从当期状态运动到设置位置，中间没有平滑过渡）
* 参 数: int nDir(in,转动方向)float fRot(in,角度值) const char* pNode(in,节点名称) int nTime(in,时间)
* 时 间: 2015年6月25日15:50:02
* 返回值: void
* 作 者: Nie
**************************************************************************/
void ManualSimuRotation(int nDir, float fRot, const char* pNode , int nTime, TypeAngleChanged nChangeType)
{
    FbxNodeRotationInfo *pNodeInfo = new FbxNodeRotationInfo();
    pNodeInfo->nDir = nDir;
    pNodeInfo->fRot = fRot;
    pNodeInfo->szNodeName = new char[strlen(pNode) + 1];
    if (nTime)
    {
        pNodeInfo->nTime = nTime;
    }
    strcpy(pNodeInfo->szNodeName,pNode);
    g_ManualLstNodeInfo.push_back(pNodeInfo);
    TriggerModelFreshed(LIBFBX_3DMODEL_FRESH_IMMEDIATELY, nChangeType);
//	glutTimerFunc(1,TimerCallback,2);
//    ::PostMessage(g_hWnd,WM_3DMODEL_FRESH, LIBFBX_3DMODEL_FRESH_IMMEDIATELY ,0);
}

/**************************************************************************
* 函数名: PlaySimuRotation
* 功 能: 模型平滑播放函数（设置之后模型平滑的从当期状态运动到设置位置，中间有平滑过渡）
* 参 数: int nDir(in,转动方向)float fRot(in,角度值) const char* pNode(in,节点名称) int nTime(in,时间)
* 时 间: 2015年6月25日15:50:57
* 返回值: void
* 作 者: Nie
**************************************************************************/
void PlaySimuRotation(int nDir, float fRot, const char* pNode , int nTime)
{
    FbxNodeRotationInfo *pNodeInfo = new FbxNodeRotationInfo();
    pNodeInfo->nDir = nDir;
    pNodeInfo->fRot = fRot;
    pNodeInfo->szNodeName = new char[strlen(pNode) + 1];
    if (nTime)
    {
        pNodeInfo->nTime = nTime;
    }
    strcpy(pNodeInfo->szNodeName,pNode);
    g_lstNodeInfo.push_back(pNodeInfo);

    int nSize = g_lstNodeInfo.size();
    int nDataCnt = nSize / g_nServoCount;
    //保证新的舵机数据到来之后覆盖之前数据，并停止当前操作，应用新的数据

    if (nSize   ==  g_nServoCount)
    {
        if(g_pSmoothThread)
        {
            g_pSmoothThread->m_bStop = true;
            g_pSmoothThread->quit();
            g_pSmoothThread->wait();
            delete g_pSmoothThread;
            g_pSmoothThread = NULL;
        }
        g_pSmoothThread = new SmoothThread();
        if(g_pDisPlay)
        {
            QObject::connect(g_pSmoothThread, &SmoothThread::SigTriggerModelFreshed,
                             g_pDisPlay, &UBXRobot3DDisplay::onTriggerModelFreshed);
            QObject::connect(g_pSmoothThread, &SmoothThread::SigNextFrame,
                             g_pDisPlay, &UBXRobot3DDisplay::onNextFrame);
        }
        g_elapsedTimer.restart();
        g_dwStartTime = g_elapsedTimer.nsecsElapsed();
        g_pSmoothThread->start();
    }
    else if (nSize   ==   2 * g_nServoCount)
    {
        //停止当前的FBX模型更新操作
        //删除之前的一组（16个）舵机数据，将新的数据覆盖到List列表前面，从而覆盖旧的动作，执行新的动作
        g_bCover = true;
        if(g_pSmoothThread)
        {
            g_pSmoothThread->m_bStop = true;
            g_pSmoothThread->quit();
            g_pSmoothThread->wait();
            delete g_pSmoothThread;
            g_pSmoothThread = NULL;
        }
        g_pSmoothThread = new SmoothThread();
        if(g_pDisPlay)
        {
            QObject::connect(g_pSmoothThread, &SmoothThread::SigTriggerModelFreshed,
                             g_pDisPlay, &UBXRobot3DDisplay::onTriggerModelFreshed);
            QObject::connect(g_pSmoothThread, &SmoothThread::SigNextFrame,
                             g_pDisPlay, &UBXRobot3DDisplay::onNextFrame);
        }
        for (int  i = 0 ; i< g_nServoCount ; i++)
        {
            FbxNodeRotationInfo *pInfo = g_lstNodeInfo.front();
            if (pInfo)
            {
                delete []pInfo->szNodeName;
                pInfo->szNodeName = NULL;
                delete pInfo;
                pInfo = NULL;
            }
            g_lstNodeInfo.pop_front();
        }
        g_bCover = false;
        g_elapsedTimer.restart();
        g_dwStartTime = g_elapsedTimer.nsecsElapsed();
        g_pSmoothThread->start();
    }

}
/************************************
* 名称: SetNodeRotationData
* 功能: 给FBX节点设置运动数据
* 访问权限:    public
* 参数: [@in]FbxNodeEx * pNodeEx 扩展节点信息
* 返回:   void
* 时间:   2015/8/17 15:55
* 作者:   Nelson_Nie
************************************/
void SetNodeRotationData(FbxNodeEx *pNodeEx)
{
    if (!pNodeEx && !pNodeArray)
    {
        return;
    }
    FbxNode *pCmpNode = pNodeEx->pFbxNode;
    if (pCmpNode)
    {
        ApplyData2Node(pCmpNode , pNodeEx->nDir , pNodeEx->fAngle , pNodeEx->szNodeName);
    }
}
/************************************
* 名称: SetNodeRotationData
* 功能: 给FBX节点设置运动数据
* 访问权限:    public
* 参数: [@in]FbxNodeRotationInfo * pInfo
* 返回:   void
* 时间:   2015/8/17 15:58
* 作者:   Nelson_Nie
************************************/
void SetNodeRotationData(FbxNodeRotationInfo *pInfo, TypeAngleChanged nType)
{
    if (!pInfo || !pNodeArray)
    {
        return;
    }
    int nNum = -1;
    for (int i = 0 ;i<g_nServoCount;i++)
    {
        if (pNodeArray[i].szNodeName)
        {
            if (!strcmp(pNodeArray[i].szNodeName,pInfo->szNodeName))
            {
                nNum = i;
                break;
            }
        }
    }
    if (nNum != -1 )
    {
        pNodeArray[nNum].fAngle = pInfo->fRot;
        pNodeArray[nNum].nDir = pInfo->nDir;
        ApplyData2Node(pNodeArray[nNum].pFbxNode , pInfo->nDir , pInfo->fRot , pInfo->szNodeName);
        pNodeArray[nNum].fPreviousAngle = pInfo->fRot;
        //if(gSceneContext->GetPickState() && g_bIsLButtonDown)
        UpdateNodeDate(pNodeArray[nNum].szNodeName, pNodeArray[nNum].fAngle, pNodeArray[nNum].offset, nType);
    }
}

/**************************************************************************
* 函数名: DestroyFbxViewWindow
* 功 能: 释放资源（定时器，new出的内存等等）
* 参 数: void
* 时 间: 2015年6月25日15:32:06
* 返回值: void
* 作 者: Nie
**************************************************************************/
void DestroyFbxViewWindow()
{
//    g_bStop = true;
//    g_bCover = true;
//    KillTimer(NULL,nTimerID);
//    WaitForSingleObject(CRotEvent,INFINITE);

    for (int i=0;i<g_nServoCount;i++)
    {
        if (pNodeArray[i].szNodeName)
        {
            delete []pNodeArray[i].szNodeName;
            pNodeArray[i].szNodeName = NULL;
        }
    }
    if (pNodeArray)
    {
        delete []pNodeArray;
        pNodeArray = NULL;
    }
    //清空list列表
    ClearFbxNodeInfoList();
    g_bFileLoaded = false;
    ExitFunction();
    //glutDestroyWindow(g_nFbxWindow);
}

/**************************************************************************
* 函数名: PlaySetRotation1
* 功 能: 模型平滑旋转接口（设置之后模型平滑的从当期状态运动到设置位置，中间有平滑过渡）
* 参 数: int nDir(in,转动方向)float fRot(in,角度值) const char* pNode(in,节点名称) int nTime(in,时间)
* 时 间: 2015年6月25日15:50:57
* 返回值: void
* 作 者: Nie
**************************************************************************/
void PlaySetRotation1(int nDir, float fRot, const char* pNode ,int nTime)
{
    //AfxGetApp()
    if (!g_bFileLoaded ||g_bStop)
    {
        return;
    }
//    EnterCriticalSection(&g_crsFbxOperation);
//    DWORD dwCurrentThreadId = ::GetCurrentThreadId();
    // 由于FBX SDK在多线程下操作是不安全的，所以需要判断当前调用该函数的
    // 是主线程还是其他线程，是主线程可以在该线程内直接调用FBX SDK，否则
    // 发送消息到主线程在调用FBX SDK
    //是否主线程调用该函数（即同一线程）
//    if (dwCurrentThreadId == g_dwMainThreadId)
//    {
        PlaySimuRotation(nDir,fRot,pNode,nTime);
//    }
//    else
//        PlaySimuRotation(nDir,fRot,pNode,nTime);
//    LeaveCriticalSection(&g_crsFbxOperation);
}

/**************************************************************************
* 函数名: ManualSetRotation1
* 功 能: 模型立即旋转接口（设置之后模型立即从当期状态运动到设置位置，中间没有平滑过渡）
* 参 数: int nDir(in,转动方向)float fRot(in,角度值) const char* pNode(in,节点名称) int nTime(in,时间)
* 时 间: 2015年6月25日15:50:02
* 返回值: void
* 作 者: Nie
**************************************************************************/
void ManualSetRotation1(int nDir, float fRot, const char* pNode , int nTime, TypeAngleChanged nType)
{
    if (!g_bFileLoaded || g_bStop /*|| g_bPlaying*/)
    {
        return;
    }
//    DWORD dwCurrentThreadId = ::GetCurrentThreadId();
   Qt::HANDLE handle = QThread::currentThreadId();
    // 由于FBX SDK在多线程下操作是不安全的，所以需要判断当前调用该函数的
    // 是主线程还是其他线程，是主线程可以在该线程内直接调用FBX SDK，否则
    // 发送消息到主线程再调用FBX SDK
    // 是否主线程调用该函数（即同一线程）
    if (handle == g_dwMainThreadID)
    {
        ManualSimuRotation(nDir,fRot,pNode,nTime, nType);
        if (gSceneContext)
        {
            gSceneContext->SaveAngleDate(fRot,pNode);    //保存数据
        }
    }
    else
    {
        ManualSimuRotation(nDir,fRot,pNode,nTime, nType);
        if (gSceneContext)
        {
            gSceneContext->SaveAngleDate(fRot,pNode);    //保存数据
        }
    }
}

//void SetRotationDate(int nID,int nAngle)
//{
//    g_nCurID = nID;
//    g_nCurAngle = nAngle;
//}

void SetRotionPlayState(bool bState)
{
    g_bRotionPlayState =bState;
    if (g_bRotionPlayState)
    {
        //glutTimerFunc(200, TimerCallback, 3);
    }
}

/**************************************************************************
* 函数名: ApplyData2Node
* 功 能: 将一帧中的某一个舵机数据应用都该舵机对应的节点之上
* 参 数: FbxNode * pNode(in,节点指针) int nDir(in,节点转动方向)
float fRot(in,节点新的角度值) const char* pNodeName(in,节点的名称)
* 时 间: 2015年6月25日15:45:10
* 返回值: void
* 作 者: Nie
**************************************************************************/
void ApplyData2Node(FbxNode * pNode, int nDir, float fRot, const char* pNodeName)
{
    if (pNode == NULL)
    {
        return;
    }
    FbxDouble3 lChange(0,0,0);   //代表三个方向的旋转自由度
    FbxDouble3 lBefore(0,0,0);
    lBefore = pNode->LclRotation.Get();
    lChange = lBefore;
    if (nDir == 1 || nDir == -1)
    {
        lChange.mData[0] = (nDir) * fRot;
    }
    else if (nDir == 2 || nDir == -2)
    {
        lChange.mData[1] = (nDir / 2) * fRot;
    }
    else if (nDir == 3 || nDir == -3)
    {
        lChange.mData[2] = (nDir / 3) * fRot;
    }
    else
        return;
    FbxDouble3 l1(0,0,0);
    FbxDouble3 l2(0,0,0);
    l1 = pNode->LclRotation.Get();
    pNode->LclRotation.Set(lChange);
    l2 = pNode->LclRotation.Get();

    //添加这句之后可以执行整个Group转动，否则只有其中心轴会转动
    const char *s = pNode->GetName();
    FbxAMatrix rootNodeGlobalRotation = pNode->EvaluateGlobalTransform();
    //设置重绘
    /*gSceneContext->SetFreshed();*/
    /*DisplayCallback();*/
    /*
    * 经过多次测试，注释掉下面一句的话则不会产生内存泄露
    */
    //glutPostRedisplay();
}

/**************************************************************************
* 函数名: ReadMotorInfoFromXML
* 功 能: 从机器人FBX模型xml配置文件中读取舵机，关机信息
* 参 数: char* szXmlFile(in,机器人XML配置文件路径)
* 时 间: 2015年6月25日15:39:30
* 返回值: true(成功) false(是啊比)
* 作 者: Nie
**************************************************************************/
bool ReadMotorInfoFromXML(char* szXmlFile)
{
    bool bRet =false;
   QString strXmlFile(szXmlFile);
   QFile file(strXmlFile);
   if(!file.open(QIODevice::ReadOnly))
       return false;
    QDomDocument XMLDoc;
    if(!XMLDoc.setContent(&file))
    {
        file.close();
        return false;
    }
    file.close();
    // 获取doc的第一个节点，即XML说明
      QDomNode  firstNode = XMLDoc.firstChild();
      //输出XML说明，nodeName()为"XML"，nodeValue()为版本和编码信息
//      qDebug()<<qPrintable(firstNode.nodeName())<<qPrintable(firstNode.nodeValue())<<endl;


    //返回根元素
      QDomElement  docELem = XMLDoc.documentElement();
      try
      {
          QDomNodeList iObjList = docELem.childNodes();
          int iObjCnt = iObjList.count();
          if (iObjCnt == 0)
          {
              return false;
          }
          g_nServoCount = iObjCnt - 4;
          pNodeArray = new FbxNodeEx[iObjCnt - 4];
      }
      catch (...)
      {
          return false;
      }
    //返回根节点的第一个子节点
      QDomNode NodePtr = docELem.firstChild();

      int iNum = 0;
      while(!NodePtr.isNull())
      {
          //如果节点是元素
          if(NodePtr.isElement())
          {
              //将其转化为元素
             QDomElement elem = NodePtr.toElement();
             //返回元素标记和id属性的值
//             qDebug()<<qPrintable(elem.tagName())<<qPrintable(elem.attribute("Motor"))<<endl;

             //获得元素e的所有子节点的列表
             QDomNodeList list = elem.childNodes();

             //遍历该列表
             for(int i = 0; i < list.count(); i++)
             {
                QDomNode node = list.at(i);
                if(node.isElement())
                {

                    QString strKey = node.toElement().tagName();
                    QByteArray byteKey = strKey.toLatin1();
                    char * szKey = byteKey.data();
                    QString strValue = node.toElement().text();
                    QByteArray byteValue = strValue.toLatin1();
                    char * szValue = byteValue.data();
//                    qDebug()<<""<<qPrintable(node.toElement().tagName())<<qPrintable(node.toElement().text())<<endl;
                    if(strcmp("ID",szKey) == 0)                    // ID
                    {
                        pNodeArray[iNum].pFbxNode = NULL;
                        pNodeArray[iNum].nID = atoi(szValue);
                        continue;
                    }
                    else if(strcmp("Name",szKey) == 0)             // NAME
                    {
                        pNodeArray[iNum].szNodeName = new char[strlen(szValue) + 1];
                        strcpy(pNodeArray[iNum].szNodeName , szValue);
                        continue;
                    }
                    else if(strcmp("InitAngle",szKey) == 0)         // INIANGLE
                    {
                        float fIniAngle = atof(szValue);
                        pNodeArray[iNum].fPreviousAngle += fIniAngle;
                        pNodeArray[iNum].fAngle += fIniAngle;
                        pNodeArray[iNum].InitAngle = fIniAngle;
                        continue;
                    }
                    else if(strcmp("Offset",szKey) ==0)             // OFFSET
                    {
                        float fOffset = atof(szValue);
                        pNodeArray[iNum].fPreviousAngle +=fOffset;
                        pNodeArray[iNum].fAngle +=fOffset;
                        pNodeArray[iNum].offset = fOffset;
                        continue;
                    }
                    else if(strcmp("Axis",szKey) ==0)               // AXIS
                    {
                        int nAxis[3] = {0};
                        char strTemp[100] = "";
                        strncpy(strTemp, szValue, strlen(szValue));
                        char* pStart = strTemp;
                        nAxis[0] = atof(pStart);
                        for (int i=1; i<3; i++)
                        {
                            char *pFind = strchr(pStart, ',');
                            if (pFind == NULL)
                                continue;

                            nAxis[i] = atof(pFind+1);
                            pStart = pFind+1;
                        }

                        for (int i = 0 ; i<3;i++ )
                        {
                            // 轴方向（有六个方向）
                            // 1代表[ 1  0  0]  2代表[0  1  0]  3代表[0  0  1]
                            //-1代表[-1  0  0] -2代表[0 -1  0] -3代表[0  0 -1]
                            pNodeArray[iNum].nDir += (i + 1) * nAxis[i];
                        }
                        continue;
                    }

                }
             }
             UpdateNodeDate(pNodeArray[iNum].szNodeName, pNodeArray[iNum].fAngle, pNodeArray[iNum].offset, TAC_RESET);
             iNum ++;
             bRet &=true;
          }
            //转到下一个兄弟节点
          NodePtr = NodePtr.nextSibling();
      }
      return bRet;
}

/**************************************************************************
* 函数名: TriggerModelFreshed
* 功 能: 立即刷新模型的状态，通过设置模型相关函数发送到句柄为g_hWnd的串口函数，然后从主线程调用避免多线程引起的fbxSDK崩溃问题
* 参 数: int nType（0-模型平滑运动 1-模型立即运动）
* 时 间: 2015年6月25日15:59:02
* 返回值: void
* 作 者: Nie
**************************************************************************/
void  TriggerModelFreshed(int nType, TypeAngleChanged nChangeType)
{
    // 	glutDisplayFunc(DisplayCallback);
    // 	glutReshapeFunc(ReshapeCallback);
    // 	glutKeyboardFunc(KeyboardCallback);
    // 	glutMouseFunc(MouseCallback);
    // 	glutMotionFunc(MotionCallback);
    // 	glutMouseWheelFunc(MouseWheelCallback);
    //	nTimerID = SetTimer(NULL, ID_TIMER, 5, TimerProc);
//    DWORD dwCurrentThreadId = ::GetCurrentThreadId();
    switch(nType)
    {
    case LIBFBX_3DMODEL_FRESH_SMOOTHLY:
        {
//             EnterCriticalSection(&g_crsFbxNodeArray);
            for (int i = 0 ;i<g_nServoCount;i++ )
            {
                SetNodeRotationData(&pNodeArray[i]);
            }
//            LeaveCriticalSection(&g_crsFbxNodeArray);
        }
        //glutPostRedisplay();
        break;
    case LIBFBX_3DMODEL_FRESH_IMMEDIATELY:
        {

            ApplyData2Model(nChangeType);
            //glutPostRedisplay();
        }
        break;
    }
}


void UpdateNodeDate(const char *szNodeName, float angle, float offset, TypeAngleChanged nChangeType)
{
    if(g_pWidget)
        emit g_pWidget->SigUpdateNodeDate(szNodeName, angle, offset, nChangeType);
}


/************************************
 * 名称: SetCursor
 * 功能: 修改3D窗体鼠标指针
 * 访问权限:    全局
 * 参数: [in]Qt::CursorShape shape
 * 返回:   void
 * 时间:   2015/12/28
 * 作者:   hels
 ************************************/
void SetCursor(Qt::CursorShape shape)
{
    if(qApp)
    {
        qApp->setOverrideCursor(shape);
    }
}

/************************************
 * 名称: OnClickNode
 * 功能: 响应UBXRobot3DView选中舵机节点信号
 * 访问权限:    global
 * 参数: [in]int nID 选中的舵机节点ID
 * 返回:   void
 * 时间:   2015/12/31
 * 作者:   hels
 ************************************/
void OnClickNode(int nID)
{
    if (!pNodeArray)
    {
        return;
    }
    int nNum = -1;
    for (int i = 0 ;i<g_nServoCount;i++)
    {
        if (pNodeArray[i].nID == nID)
        {
            nNum = i;
            break;
        }
    }
    if (nNum != -1 )
    {
        
        if(gSceneContext)
        {
            gSceneContext->SavePickNode(pNodeArray[nNum].pFbxNode, gSceneContext->GetPickNodeZ() + 1);
            gSceneContext->SetPickState(true);
            UpdateNodeDate(pNodeArray[nNum].szNodeName, pNodeArray[nNum].fAngle, pNodeArray[nNum].offset, TAC_3DModel);
        }
    }
}

/************************************
 * 名称: OnAngleChanged
 * 功能: 响应UBXRobot3DView控制舵机旋转信号
 * 访问权限:    global
 * 参数: [in]int nID 选中的舵机节点ID
 * 参数: [in]int value 改变的角度值
 * 参数: [in]nType 引起角度改变的原因
 * 返回:   void
 * 时间:   2015/12/31
 * 作者:   hels
 ************************************/
void OnAngleChanged(int nID, int value, int nTime, TypeAngleChanged nType)
{
    AlphaRobotLog::getInstance()->printDebug(QString("3d display on angle changed, montor ID: %1, value: %2, time: %3, type: %4").arg(nID).arg(value).arg(nTime).arg(nType));
    if (!pNodeArray)
    {
        return;
    }
    int nNum = -1;
    for (int i = 0 ;i<g_nServoCount;i++)
    {
        if (pNodeArray[i].nID == nID)
        {
            nNum = i;
            break;
        }
    }
    if (nNum != -1 )
    {
        if(gSceneContext)
        {
            int nDir = pNodeArray[nNum].nDir;
            int initOffset = pNodeArray[nNum].offset;
            int nOffset= value + initOffset;
            const int nID = pNodeArray[nNum].nID;

            /////////////////////////////////////////////////////////////////////////////////
            //for modify the value of big 255 to 255
            const int maxOffset = (UBXBase::getInstance()->getDefaultActAngleValueMax(nID) + initOffset);
            if(nOffset > maxOffset)
            {
                nOffset = maxOffset;
            }
            //////////////////////////////////////////////////////////////////////////////////



            if (nOffset >= (initOffset) && nOffset <= (UBXBase::getInstance()->getDefaultActAngleValueMax(nID) + initOffset))
            {
                ManualSetRotation1(nDir, nOffset, pNodeArray[nNum].szNodeName, nTime, nType);                            //   3D视图更新
//                SetRotationDate(pNodeArray[nNum].nID, nOffset - initOffset);       //   使机器人做动作，更更新UI
            }
        }
    }
}

void OnReset()
{
    if (!pNodeArray)
    {
        return;
    }

    for (int i = 0 ;i<g_nServoCount;i++)
    {
        ManualSetRotation1(pNodeArray[i].nDir, pNodeArray[i].InitAngle + pNodeArray[i].offset, pNodeArray[i].szNodeName, 200, TAC_RESET);
//        SetRotationDate(pNodeArray[i].nID, pNodeArray[i].InitAngle);
    }
}

int GetMotorCount()
{
    return g_nServoCount;
}

QMap<int, int> GetMotorInfo()
{
    QMap<int, int> motorInfo;
    if (!pNodeArray)
    {
        return motorInfo;
    }

    for (int i = 0 ;i<g_nServoCount;i++)
    {
        //将3D角度转为0-180°范围再返回
        motorInfo.insert(pNodeArray[i].nID, pNodeArray[i].fAngle - pNodeArray[i].offset);
    }
    return motorInfo;
}

void SetModelPlayState(bool bState)
{
    g_bPlaying = bState;
}

void OnRotationSmooth(int nID, float fRot, int nTime)
{
    if (!pNodeArray)
    {
        return;
    }
    int nNum = -1;
    for (int i = 0 ;i<g_nServoCount;i++)
    {
        if (pNodeArray[i].nID == nID)
        {
            nNum = i;
            break;
        }
    }
    if (nNum != -1 )
    {
        if(gSceneContext)
        {
            int nDir = pNodeArray[nNum].nDir;
            int initOffset = pNodeArray[nNum].offset;
            int nOffset= fRot + initOffset;

            //先不进行0-180°的判断，有部分老的工程会超过几度，导致最后3D模型角度错乱
            //if (nOffset >= (initOffset) && nOffset <= (180+ initOffset))
            {
                PlaySetRotation1(nDir, nOffset, pNodeArray[nNum].szNodeName, nTime);
            }
        }
    }
}

void OnUpdateData()
{
    if (!pNodeArray)
    {
        return;
    }
    for (int i = 0 ;i<g_nServoCount;i++)
    {
        UpdateNodeDate(pNodeArray[i].szNodeName, pNodeArray[i].fAngle, pNodeArray[i].offset, TAC_RESET);
    }
}
