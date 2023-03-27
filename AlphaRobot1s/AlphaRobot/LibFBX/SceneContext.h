/****************************************************************************************

Copyright (C) 2015 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

#ifndef _SCENE_CONTEXT_H
#define _SCENE_CONTEXT_H

#include "GlFunctions.h"
#include "GetPosition.h"
#include <iostream>
using namespace std;
class FbxDrawText;

// This class is responsive for loading files and recording current status as
// a bridge between window system such as GLUT or Qt and a specific FBX scene.

class FbxMotorNodeEx
{
public:
    int nID;
    int nDir;
    float fAngleOffset;
    float fAngle;
    float fPreviousAngle;
    char *szNodeName;
    int nTimes;
public:
    FbxMotorNodeEx()
    {
        szNodeName = NULL;
        nID = 0;
        nDir = 0;
        fAngle = 0;
        fPreviousAngle = 0;
        fAngleOffset = 0;
        nTimes = 0;
    }
};

class SceneContext
{
public:
    enum Status
    {
        UNLOADED,               // Unload file or load failure;
        MUST_BE_LOADED,         // Ready for loading file;
        MUST_BE_REFRESHED,      // Something changed and redraw needed;
        REFRESHED               // No redraw needed.
    };
    Status GetStatus() const { return mStatus; }

    // Initialize with a .FBX, .DAE or .OBJ file name and current window size.
    SceneContext(const char * pFileName, int pWindowWidth, int pWindowHeight, bool pSupportVBO, int nRatio);
    ~SceneContext();

    // Return the FBX scene for more informations.
    const FbxScene * GetScene() const { return mScene; }
    // Load the FBX or COLLADA file into memory.
    bool LoadFile();

    // The time period for one frame.
    const FbxTime GetFrameTime() const { return mFrameTime; }

    // Call this method when redraw is needed.
    bool OnDisplay();
    // Call this method when window size is changed.
    void OnReshape(int pWidth, int pHeight);
    // Call this method when keyboard input occurs.
    void OnKeyboard(unsigned char pKey);
    // Call this method when mouse buttons are pushed or released.
    void OnMouse(int pButton, int pState, int pX, int pY);
    void OnMouseWheel(int dir, int x, int y);
    // Call this method when mouse is moved.
    void OnMouseMotion(int pX, int pY);
    // Call this method when timer is finished, for animation display.
    void OnTimerClick() const;

    // Methods for creating menus.
    // Get all the cameras in current scene, including producer cameras.
    const FbxArray<FbxNode *> & GetCameraArray() const { return mCameraArray; }
    // Get all the animation stack names in current scene.
    const FbxArray<FbxString *> & GetAnimStackNameArray() const { return mAnimStackNameArray; }
    // Get all the pose in current scene.
    const FbxArray<FbxPose *> & GetPoseArray() const { return mPoseArray; }

    // The input index is corresponding to the array returned from GetAnimStackNameArray.
    bool SetCurrentAnimStack(int pIndex);
    // Set the current camera with its name.
    bool SetCurrentCamera(const char * pCameraName);
    // The input index is corresponding to the array returned from GetPoseArray.
    bool SetCurrentPoseIndex(int pPoseIndex);
    // Set the currently selected node from external window system.
    void SetSelectedNode(FbxNode * pSelectedNode);
    // Set the shading mode, wire-frame or shaded.
    void SetShadingMode(ShadingMode pMode);

    // Pause the animation.
    void SetPause(bool pPause) { mPause = pPause; }
    // Check whether the animation is paused.
    bool GetPause() const { return mPause; }
    void SetFreshed();

    //手动拖动关节相关函数
    bool GetDefaultMotorInfo(char* szXmlFile);
    void DoPickNodeByScreenPos(int pX, int pY);
    void FindPickNodeRecursive(FbxNode* pNode);
    void FindPickNode(FbxNode* pNode);
    void SavePickNode(FbxNode* pNode, GLdouble z);
    bool GetOptimalPickNode();
    void SetPickNodeCursor(FbxNode* pNode); //设置光标状态
    void OnMove(FbxNode *pNode,int dX, int dY);
    void SaveAngleDate(int nAngle,const char* pNode);
    void GetPreAngleDate();
    void DrawNodeRecursiveAndShowPickNode(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer,
        FbxAMatrix& pParentGlobalPosition, FbxPose* pPose);
    GLbyte * SaveViewPortDate(int& nWidth, int& nHeigth);
    float GetMaxNum(float a,float b);
    float GetMinNum(float a,float b);
    bool IsCurPosInsideNodeBox(FbxNode* pNode, GLdouble *z);
    GLdouble GetPickNodeZ()
    {
        if(m_pOptimalPickeNode)
        {
            return m_OptimalZ;
        }
        return 0;
    }
    void SetPickState(int state);
    int GetPickState()
    {
        return m_nPickStatus;
    }

    enum CameraZoomMode
    {
        ZOOM_FOCAL_LENGTH,
        ZOOM_POSITION
    };
    CameraZoomMode  GetZoomMode()        { return mCameraZoomMode; }
    void            SetZoomMode( CameraZoomMode pZoomMode);       

private:
    // Display information about current status in the left-up corner of the window.
    void DisplayWindowMessage();
    // Display a X-Z grid.
    void DisplayGrid(const FbxAMatrix & pTransform);

    enum CameraStatus
    {
        CAMERA_NOTHING,
        CAMERA_ORBIT,
        CAMERA_ZOOM,
        CAMERA_PAN
    };

    FbxString mFileName;
    mutable Status mStatus;
    mutable FbxString mWindowMessage;

    FbxManager * mSdkManager;
    FbxScene * mScene;
    FbxImporter * mImporter;
    FbxAnimLayer * mCurrentAnimLayer;
    FbxNode * mSelectedNode;

    int mPoseIndex;
    FbxArray<FbxString*> mAnimStackNameArray;
    FbxArray<FbxNode*> mCameraArray;
    FbxArray<FbxPose*> mPoseArray;

    mutable FbxTime mFrameTime, mStart, mStop, mCurrentTime;
	mutable FbxTime mCache_Start, mCache_Stop;

    // Data for camera manipulation
    mutable int mLastX, mLastY;
    mutable FbxVector4 mCamPosition, mCamCenter;
    mutable double mRoll;
    mutable CameraStatus mCameraStatus;

    bool mPause;
    ShadingMode mShadingMode;
    bool mSupportVBO;

    //camera zoom mode
    CameraZoomMode mCameraZoomMode;

    int mWindowWidth, mWindowHeight;
    // Utility class for draw text in OpenGL.
    FbxDrawText * mDrawText;

    //手动拖动关节相关变量
    FbxNode* m_pOptimalPickeNode; //保存选中节点时根据z深度判断重叠选中多个节点时，优先选中的节点
    GLdouble m_OptimalZ;
    //3D模型（group层次有问题），因此需要在其他位置记录小腿部位的关节
    FbxNode* m_pNode9Mesh;
    FbxNode* m_pNode10Mesh;
    FbxNode* m_pNode11Mesh;

    FbxNode* m_pNode15Mesh;
    FbxNode* m_pNode14Mesh;
    FbxNode* m_pNode16Mesh;
    //FbxNode * m_PickNode;
    GLdouble mposX, mposY, mposZ;
    int m_nServoCount;
    FbxMotorNodeEx   *m_pMotorNodeArray;
    int m_nPickStatus;
    int m_nRatio;
};

// Initialize GLEW, must be called after the window is created.
bool InitializeOpenGL();

#endif // _SCENE_CONTEXT_H

