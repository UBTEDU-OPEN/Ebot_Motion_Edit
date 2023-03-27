/****************************************************************************************

Copyright (C) 2015 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

#include "SceneContext.h"

#include "SceneCache.h"
#include "SetCamera.h"
#include "DrawScene.h"
#include "DrawText.h"
#include "targa.h"
#include "Common.h"
#include "FbxPlayer.h"
#include <QtXml>
#include <QCursor>


#include "robotDefine.h"
#include "../UBXBase/UBXBase.h"


//char* Unicode2Utf8(const char* unicode)
//{
//    int len;
//    len = WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)unicode, -1, NULL, 0, NULL, NULL);
//    char *szUtf8 = (char*)malloc(len + 1);
//    memset(szUtf8, 0, len + 1);
//    WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)unicode, -1, szUtf8, len, NULL,NULL);
//    return szUtf8;
//}

//char* Ansi2Unicode(const char* str)
//{
//    int dwUnicodeLen = MultiByteToWideChar(CP_ACP,0,str,-1,NULL,0);
//    if(!dwUnicodeLen)
//    {
//        return strdup(str);
//    }
//    size_t num = dwUnicodeLen*sizeof(wchar_t);
//    wchar_t *pwText = (wchar_t*)malloc(num);
//    memset(pwText,0,num);
//    MultiByteToWideChar(CP_ACP,0,str,-1,pwText,dwUnicodeLen);
//    return (char*)pwText;
//}

//char* ConvertAnsiToUtf8(const char* str)
//{
//    char* unicode = Ansi2Unicode(str);
//    char* utf8 = Unicode2Utf8(unicode);
//    free(unicode);
//    return utf8;
//}

namespace
{
    // Default file of ViewScene example
    const char * SAMPLE_FILENAME = MODEL_DATA_NAME;

    // Button and action definition
    const int LEFT_BUTTON = 0;
    const int MIDDLE_BUTTON = 1;
    const int RIGHT_BUTTON = 2;

    const int BUTTON_DOWN = 0;
    const int BUTTON_UP = 1;

    // Find all the cameras under this node recursively.
    void FillCameraArrayRecursive(FbxNode* pNode, FbxArray<FbxNode*>& pCameraArray)
    {
        if (pNode)
        {
            if (pNode->GetNodeAttribute())
            {
                if (pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eCamera)
                {
                    pCameraArray.Add(pNode);
                }
            }

            const int lCount = pNode->GetChildCount();
            for (int i = 0; i < lCount; i++)
            {
                FillCameraArrayRecursive(pNode->GetChild(i), pCameraArray);
            }
        }
    }

    // Find all the cameras in this scene.
    void FillCameraArray(FbxScene* pScene, FbxArray<FbxNode*>& pCameraArray)
    {
        pCameraArray.Clear();

        FillCameraArrayRecursive(pScene->GetRootNode(), pCameraArray);
    }

    // Find all poses in this scene.
    void FillPoseArray(FbxScene* pScene, FbxArray<FbxPose*>& pPoseArray)
    {
        const int lPoseCount = pScene->GetPoseCount();

        for (int i=0; i < lPoseCount; ++i)
        {
            pPoseArray.Add(pScene->GetPose(i));
        }
    }

    void PreparePointCacheData(FbxScene* pScene, FbxTime &pCache_Start, FbxTime &pCache_Stop)
    {
        // This function show how to cycle through scene elements in a linear way.
		const int lNodeCount = pScene->GetSrcObjectCount<FbxNode>();
        FbxStatus lStatus;

        for (int lIndex=0; lIndex<lNodeCount; lIndex++)
        {
            FbxNode* lNode = pScene->GetSrcObject<FbxNode>(lIndex);

            if (lNode->GetGeometry()) 
            {
                int i, lVertexCacheDeformerCount = lNode->GetGeometry()->GetDeformerCount(FbxDeformer::eVertexCache);

                // There should be a maximum of 1 Vertex Cache Deformer for the moment
                lVertexCacheDeformerCount = lVertexCacheDeformerCount > 0 ? 1 : 0;

                for (i=0; i<lVertexCacheDeformerCount; ++i )
                {
                    // Get the Point Cache object
                    FbxVertexCacheDeformer* lDeformer = static_cast<FbxVertexCacheDeformer*>(lNode->GetGeometry()->GetDeformer(i, FbxDeformer::eVertexCache));
                    if( !lDeformer ) continue;
                    FbxCache* lCache = lDeformer->GetCache();
                    if( !lCache ) continue;

                    // Process the point cache data only if the constraint is active
                    if (lDeformer->Active.Get())
                    {
                        if (lCache->GetCacheFileFormat() == FbxCache::eMaxPointCacheV2)
                        {
                            // This code show how to convert from PC2 to MC point cache format
                            // turn it on if you need it.
#if 0 
                            if (!lCache->ConvertFromPC2ToMC(FbxCache::eMCOneFile, 
                                FbxTime::GetFrameRate(pScene->GetGlobalTimeSettings().GetTimeMode())))
                            {
                                // Conversion failed, retrieve the error here
                                FbxString lTheErrorIs = lCache->GetStaus().GetErrorString();
                            }
#endif
                        }
                        else if (lCache->GetCacheFileFormat() == FbxCache::eMayaCache)
                        {
                            // This code show how to convert from MC to PC2 point cache format
                            // turn it on if you need it.
                            //#if 0 
                            if (!lCache->ConvertFromMCToPC2(FbxTime::GetFrameRate(pScene->GetGlobalSettings().GetTimeMode()), 0, &lStatus))
                            {
                                // Conversion failed, retrieve the error here
                                FbxString lTheErrorIs = lStatus.GetErrorString();
                            }
                            //#endif
                        }
						

                        // Now open the cache file to read from it
                        if (!lCache->OpenFileForRead(&lStatus))
                        {
                            // Cannot open file 
                            FbxString lTheErrorIs = lStatus.GetErrorString();

                            // Set the deformer inactive so we don't play it back
                            lDeformer->Active = false;
                        }
						else
						{
							// get the start and stop time of the cache
							FbxTime lChannel_Start;
							FbxTime lChannel_Stop;
							int lChannelIndex = lCache->GetChannelIndex(lDeformer->Channel.Get());	
							if(lCache->GetAnimationRange(lChannelIndex, lChannel_Start, lChannel_Stop))
							{
								// get the smallest start time
								if(lChannel_Start < pCache_Start) pCache_Start = lChannel_Start;

								// get the biggest stop time
								if(lChannel_Stop  > pCache_Stop)  pCache_Stop  = lChannel_Stop;
							}
						}
                    }
                }
            }
        }
    }

    // Load a texture file (TGA only now) into GPU and return the texture object name
    bool LoadTextureFromFile(const FbxString & pFilePath, unsigned int & pTextureObject)
    {
        if (pFilePath.Right(3).Upper() == "TGA")
        {
            tga_image lTGAImage;

            if (tga_read(&lTGAImage, pFilePath.Buffer()) == TGA_NOERR)
            {
                // Make sure the image is left to right
                if (tga_is_right_to_left(&lTGAImage))
                    tga_flip_horiz(&lTGAImage);

                // Make sure the image is bottom to top
                if (tga_is_top_to_bottom(&lTGAImage))
                    tga_flip_vert(&lTGAImage);

                // Make the image BGR 24
                tga_convert_depth(&lTGAImage, 24);

                // Transfer the texture date into GPU
                glGenTextures(1, &pTextureObject);
                glBindTexture(GL_TEXTURE_2D, pTextureObject);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
                glTexImage2D(GL_TEXTURE_2D, 0, 3, lTGAImage.width, lTGAImage.height, 0, GL_BGR,
                    GL_UNSIGNED_BYTE, lTGAImage.image_data);
                glBindTexture(GL_TEXTURE_2D, 0);

                tga_free_buffers(&lTGAImage);

                return true;
            }
        }

        return false;
    }

    // Bake node attributes and materials under this node recursively.
    // Currently only mesh, light and material.
    void LoadCacheRecursive(FbxNode * pNode, FbxAnimLayer * pAnimLayer, bool pSupportVBO)
    {
        // Bake material and hook as user data.
        const int lMaterialCount = pNode->GetMaterialCount();
        for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex)
        {
            FbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lMaterialIndex);
            if (lMaterial && !lMaterial->GetUserDataPtr())
            {
                FbxAutoPtr<MaterialCache> lMaterialCache(new MaterialCache);
                if (lMaterialCache->Initialize(lMaterial))
                {
                    lMaterial->SetUserDataPtr(lMaterialCache.Release());
                }
            }
        }

        FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
        if (lNodeAttribute)
        {
            // Bake mesh as VBO(vertex buffer object) into GPU.
            if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
            {
                FbxMesh * lMesh = pNode->GetMesh();
                if (pSupportVBO && lMesh && !lMesh->GetUserDataPtr())
                {
                    FbxAutoPtr<VBOMesh> lMeshCache(new VBOMesh);
                    if (lMeshCache->Initialize(lMesh))
                    {
                        lMesh->SetUserDataPtr(lMeshCache.Release());
                    }
                }
            }
            // Bake light properties.
            else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eLight)
            {
                FbxLight * lLight = pNode->GetLight();
                if (lLight && !lLight->GetUserDataPtr())
                {
                    FbxAutoPtr<LightCache> lLightCache(new LightCache);
                    if (lLightCache->Initialize(lLight, pAnimLayer))
                    {
                        lLight->SetUserDataPtr(lLightCache.Release());
                    }
                }
            }
        }

        const int lChildCount = pNode->GetChildCount();
        for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
        {
            LoadCacheRecursive(pNode->GetChild(lChildIndex), pAnimLayer, pSupportVBO);
        }
    }

    // Unload the cache and release the memory under this node recursively.
    void UnloadCacheRecursive(FbxNode * pNode)
    {
        // Unload the material cache
        const int lMaterialCount = pNode->GetMaterialCount();
        for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex)
        {
            FbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lMaterialIndex);
            if (lMaterial && lMaterial->GetUserDataPtr())
            {
                MaterialCache * lMaterialCache = static_cast<MaterialCache *>(lMaterial->GetUserDataPtr());
                lMaterial->SetUserDataPtr(NULL);
                delete lMaterialCache;
            }
        }

        FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
        if (lNodeAttribute)
        {
            // Unload the mesh cache
            if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
            {
                FbxMesh * lMesh = pNode->GetMesh();
                if (lMesh && lMesh->GetUserDataPtr())
                {
                    VBOMesh * lMeshCache = static_cast<VBOMesh *>(lMesh->GetUserDataPtr());
                    lMesh->SetUserDataPtr(NULL);
                    delete lMeshCache;
                }
            }
            // Unload the light cache
            else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eLight)
            {
                FbxLight * lLight = pNode->GetLight();
                if (lLight && lLight->GetUserDataPtr())
                {
                    LightCache * lLightCache = static_cast<LightCache *>(lLight->GetUserDataPtr());
                    lLight->SetUserDataPtr(NULL);
                    delete lLightCache;
                }
            }
        }

        const int lChildCount = pNode->GetChildCount();
        for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
        {
            UnloadCacheRecursive(pNode->GetChild(lChildIndex));
        }
    }

    // Bake node attributes and materials for this scene and load the textures.
    void LoadCacheRecursive(FbxScene * pScene, FbxAnimLayer * pAnimLayer, const char * pFbxFileName, bool pSupportVBO)
    {
        // Load the textures into GPU, only for file texture now
        const int lTextureCount = pScene->GetTextureCount();
        for (int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex)
        {
            FbxTexture * lTexture = pScene->GetTexture(lTextureIndex);
            FbxFileTexture * lFileTexture = FbxCast<FbxFileTexture>(lTexture);
            if (lFileTexture && !lFileTexture->GetUserDataPtr())
            {
                // Try to load the texture from absolute path
                const FbxString lFileName = lFileTexture->GetFileName();
                
                // Only TGA textures are supported now.
                if (lFileName.Right(3).Upper() != "TGA")
                {
                    FBXSDK_printf("Only TGA textures are supported now: %s\n", lFileName.Buffer());
                    continue;
                }

                GLuint lTextureObject = 0;
                bool lStatus = LoadTextureFromFile(lFileName, lTextureObject);

                const FbxString lAbsFbxFileName = FbxPathUtils::Resolve(pFbxFileName);
                const FbxString lAbsFolderName = FbxPathUtils::GetFolderName(lAbsFbxFileName);
                if (!lStatus)
                {
                    // Load texture from relative file name (relative to FBX file)
                    const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, lFileTexture->GetRelativeFileName());
                    lStatus = LoadTextureFromFile(lResolvedFileName, lTextureObject);
                }

                if (!lStatus)
                {
                    // Load texture from file name only (relative to FBX file)
                    const FbxString lTextureFileName = FbxPathUtils::GetFileName(lFileName);
                    const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, lTextureFileName);
                    lStatus = LoadTextureFromFile(lResolvedFileName, lTextureObject);
                }

                if (!lStatus)
                {
                    FBXSDK_printf("Failed to load texture file: %s\n", lFileName.Buffer());
                    continue;
                }

                if (lStatus)
                {
                    GLuint * lTextureName = new GLuint(lTextureObject);
                    lFileTexture->SetUserDataPtr(lTextureName);
                }
            }
        }

        LoadCacheRecursive(pScene->GetRootNode(), pAnimLayer, pSupportVBO);
    }

    // Unload the cache and release the memory fro this scene and release the textures in GPU
    void UnloadCacheRecursive(FbxScene * pScene)
    {
        const int lTextureCount = pScene->GetTextureCount();
        for (int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex)
        {
            FbxTexture * lTexture = pScene->GetTexture(lTextureIndex);
            FbxFileTexture * lFileTexture = FbxCast<FbxFileTexture>(lTexture);
            if (lFileTexture && lFileTexture->GetUserDataPtr())
            {
                GLuint * lTextureName = static_cast<GLuint *>(lFileTexture->GetUserDataPtr());
                lFileTexture->SetUserDataPtr(NULL);
                glDeleteTextures(1, lTextureName);
                delete lTextureName;
            }
        }

        UnloadCacheRecursive(pScene->GetRootNode());
    }
}

bool InitializeOpenGL()
{
    // Initialize GLEW.
    GLenum lError = glewInit();
    if (lError != GLEW_OK)
    {
        FBXSDK_printf("GLEW Error: %s\n", glewGetErrorString(lError));
        return false;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //glClearColor(1.0, 1.0, 1.0, 1.0); //太白
    glClearColor(54.0/255.0, 54.0/255.0, 54.0/255.0, 1.0);
//    glClearColor(200.0/255.0, 200.0/255.0, 200.0/255.0, 1.0);
//    glClearColor(180.0/255.0, 180.0/255.0, 180.0/255.0, 1.0);
//      glClearColor(100.0/255.0, 100.0/255.0, 100.0/255.0, 1.0);//太黑

//    glClearColor(0.0, 0.0, 0.0, 0.0);

    // OpenGL 1.5 at least.
    if (!GLEW_VERSION_1_5)
    {
        FBXSDK_printf("The OpenGL version should be at least 1.5 to display shaded scene!\n");
        return false;
    }

    return true;
}

SceneContext::SceneContext(const char * pFileName, int pWindowWidth, int pWindowHeight, bool pSupportVBO, int nRatio)
: mFileName(pFileName), mStatus(UNLOADED),
mSdkManager(NULL), mScene(NULL), mImporter(NULL), mCurrentAnimLayer(NULL), mSelectedNode(NULL),
mPoseIndex(-1), mCameraStatus(CAMERA_NOTHING), mPause(false), mShadingMode(SHADING_MODE_SHADED),
mSupportVBO(pSupportVBO), mCameraZoomMode(ZOOM_FOCAL_LENGTH),
mWindowWidth(pWindowWidth), mWindowHeight(pWindowHeight), mDrawText(new FbxDrawText)
,m_pMotorNodeArray(NULL), m_nServoCount(0), m_nPickStatus(false), m_nRatio(nRatio)
{

    mFileName = pFileName;
	// initialize cache start and stop time
	mCache_Start = FBXSDK_TIME_INFINITE;
	mCache_Stop  = FBXSDK_TIME_MINUS_INFINITE;

   // Create the FBX SDK manager which is the object allocator for almost 
   // all the classes in the SDK and create the scene.
   InitializeSdkObjects(mSdkManager, mScene);

   if (mSdkManager)
   {
       // Create the importer.
       int lFileFormat = -1;
       mImporter = FbxImporter::Create(mSdkManager,"");
       if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(mFileName, lFileFormat) )
       {
           // Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
           lFileFormat = mSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription( "FBX binary (*.fbx)" );
       }

       // Initialize the importer by providing a filename.
       if(mImporter->Initialize(mFileName, lFileFormat) == true)
       {
           // The file is going to be imported at 
           // the end of the first display callback.
           mWindowMessage = "Importing model ";
//           mWindowMessage += mFileName;
           mWindowMessage += "\nPlease wait!";

           // Set scene status flag to ready to load.
           mStatus = MUST_BE_LOADED;
       }
       else
       {
           mWindowMessage = "Unable to open file ";
           mWindowMessage += mFileName;
           mWindowMessage += "\nError reported: ";
           mWindowMessage += mImporter->GetStatus().GetErrorString();
           mWindowMessage += "\nEsc to exit";
       }
   }
   else
   {
       mWindowMessage = "Unable to create the FBX SDK manager";
       mWindowMessage += "\nEsc to exit";
   }
}

SceneContext::~SceneContext()
{
    FbxArrayDelete(mAnimStackNameArray);

    delete mDrawText;

    // Unload the cache and free the memory
    if (mScene)
    {
        UnloadCacheRecursive(mScene);
    }

    // Delete the FBX SDK manager. All the objects that have been allocated 
    // using the FBX SDK manager and that haven't been explicitly destroyed 
    // are automatically destroyed at the same time.
	DestroySdkObjects(mSdkManager, true);

    for (int i=0;i<m_nServoCount;i++)
    {
        if (m_pMotorNodeArray[i].szNodeName)
        {
            delete []m_pMotorNodeArray[i].szNodeName;
            m_pMotorNodeArray[i].szNodeName = NULL;
        }
    }
    if (m_pMotorNodeArray)
    {
        delete []m_pMotorNodeArray;
        m_pMotorNodeArray=NULL;
    }
}

bool SceneContext::LoadFile()
{
    bool lResult = false;
    // Make sure that the scene is ready to load.
    if (mStatus == MUST_BE_LOADED)
    {
        if (mImporter->Import(mScene) == true)
        {
            // Set the scene status flag to refresh 
            // the scene in the first timer callback.
            mStatus = MUST_BE_REFRESHED;

            // Convert Axis System to what is used in this example, if needed
            FbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
            FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
            if( SceneAxisSystem != OurAxisSystem )
            {
                OurAxisSystem.ConvertScene(mScene);
            }

            // Convert Unit System to what is used in this example, if needed
            FbxSystemUnit SceneSystemUnit = mScene->GetGlobalSettings().GetSystemUnit();
            if( SceneSystemUnit.GetScaleFactor() != 1.0 )
            {
                //The unit in this example is centimeter.
                FbxSystemUnit::cm.ConvertScene( mScene);
            }

            // Get the list of all the animation stack.
            mScene->FillAnimStackNameArray(mAnimStackNameArray);

            // Get the list of all the cameras in the scene.
            FillCameraArray(mScene, mCameraArray);

            // Convert mesh, NURBS and patch into triangle mesh
			FbxGeometryConverter lGeomConverter(mSdkManager);
            lGeomConverter.Triangulate(mScene, /*replace*/true,true);

            // Split meshes per material, so that we only have one material per mesh (for VBO support)
            lGeomConverter.SplitMeshesPerMaterial(mScene, /*replace*/true);

            // Bake the scene for one frame
            LoadCacheRecursive(mScene, mCurrentAnimLayer, mFileName, mSupportVBO);

            // Convert any .PC2 point cache data into the .MC format for 
            // vertex cache deformer playback.
            PreparePointCacheData(mScene, mCache_Start, mCache_Stop);

            // Get the list of pose in the scene
            FillPoseArray(mScene, mPoseArray);

            // Initialize the window message.
            mWindowMessage = " ";
//            mWindowMessage += mFileName;
//            mWindowMessage += "\nClick on the right mouse button to enter menu.";
//            mWindowMessage = "\Rotate: Left Mouse Button.\n";
//            mWindowMessage += "\PAN: Left Mouse Button + Middle Mouse Button.\n";
//            mWindowMessage += "\ZOOM: Middle Mouse Button.\n";
//            mWindowMessage += "\nEsc to exit.";

            // Initialize the frame period.
            mFrameTime.SetTime(0, 0, 0, 1, 0, mScene->GetGlobalSettings().GetTimeMode());

            // Print the keyboard shortcuts.
//            FBXSDK_printf("Play/Pause Animation: Space Bar.\n");
//            FBXSDK_printf("Camera Rotate: Left Mouse Button.\n");
//            FBXSDK_printf("Camera Pan: Left Mouse Button + Middle Mouse Button.\n");
//            FBXSDK_printf("Camera Zoom: Middle Mouse Button.\n");

            lResult = true;
        }
        else
        {
            // Import failed, set the scene status flag accordingly.
            mStatus = UNLOADED;

            mWindowMessage = "Unable to import file ";
            mWindowMessage += mFileName;
            mWindowMessage += "\nError reported: ";
            mWindowMessage += mImporter->GetStatus().GetErrorString();
        }

        // Destroy the importer to release the file.
        mImporter->Destroy();
        mImporter = NULL;

        FbxCamera *lCamera =  GetCurrentCamera(mScene);
        if(lCamera)
        {
            //载入模型之后放大模型
            for(int i = 0; i < 37; i++)
            {
                CameraZoom(mScene,10,mCameraZoomMode);
            }
         //   mStatus = MUST_BE_REFRESHED;
            FbxCamera *lCamera =  GetCurrentCamera(mScene);
            if(lCamera)
            {
                mCamPosition = lCamera -> Position.Get();
                mCamCenter = lCamera -> InterestPosition.Get();
                mRoll = lCamera -> Roll.Get();
            }
            //平移模型
            CameraPan(mScene,mCamPosition,mCamCenter,mRoll, 1,-20);
//            mStatus = MUST_BE_REFRESHED;
        }
    }

    return lResult;
}

bool SceneContext::SetCurrentAnimStack(int pIndex)
{
    const int lAnimStackCount = mAnimStackNameArray.GetCount();
    if (!lAnimStackCount || pIndex >= lAnimStackCount)
    {
        return false;
    }

    // select the base layer from the animation stack
   FbxAnimStack * lCurrentAnimationStack = mScene->FindMember<FbxAnimStack>(mAnimStackNameArray[pIndex]->Buffer());
   if (lCurrentAnimationStack == NULL)
   {
       // this is a problem. The anim stack should be found in the scene!
       return false;
   }

   // we assume that the first animation layer connected to the animation stack is the base layer
   // (this is the assumption made in the FBXSDK)
   mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
  // mScene->SetCurrentAnimationStack(lCurrentAnimationStack);

   FbxTakeInfo* lCurrentTakeInfo = mScene->GetTakeInfo(*(mAnimStackNameArray[pIndex]));
   if (lCurrentTakeInfo)
   {
       mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
       mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
   }
   else
   {
       // Take the time line value
       FbxTimeSpan lTimeLineTimeSpan;
       mScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);

       mStart = lTimeLineTimeSpan.GetStart();
       mStop  = lTimeLineTimeSpan.GetStop();
   }

   // check for smallest start with cache start
   if(mCache_Start < mStart)
	   mStart = mCache_Start;

   // check for biggest stop with cache stop
   if(mCache_Stop  > mStop)  
	   mStop  = mCache_Stop;

   // move to beginning
   mCurrentTime = mStart;

   // Set the scene status flag to refresh 
   // the scene in the next timer callback.
   mStatus = MUST_BE_REFRESHED;

   return true;
}

bool SceneContext::SetCurrentCamera(const char * pCameraName)
{
    if (!pCameraName)
    {
        return false;
    }

    FbxGlobalSettings& lGlobalCameraSettings = mScene->GetGlobalSettings();
    lGlobalCameraSettings.SetDefaultCamera(pCameraName);
    mStatus = MUST_BE_REFRESHED;
    return true;
}

bool SceneContext::SetCurrentPoseIndex(int pPoseIndex)
{
    mPoseIndex = pPoseIndex;
    mStatus = MUST_BE_REFRESHED;
    return true;
}

void SceneContext::OnTimerClick() const
{
    // Loop in the animation stack if not paused.
    if (mStop > mStart && !mPause)
    {
        // Set the scene status flag to refresh 
        // the scene in the next timer callback.
        mStatus = MUST_BE_REFRESHED;

        mCurrentTime += mFrameTime;

        if (mCurrentTime > mStop)
        {
            mCurrentTime = mStart;
        }
    }
    // Avoid displaying the same frame on 
    // and on if the animation stack has no length.
    else
    {
        // Set the scene status flag to avoid refreshing 
        // the scene in the next timer callback.
        mStatus = REFRESHED;
    }
}

// Redraw the scene
bool SceneContext::OnDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Test if the scene has been loaded yet.
    if (mStatus != UNLOADED && mStatus != MUST_BE_LOADED)
    {
        glPushAttrib(GL_ENABLE_BIT);
        glPushAttrib(GL_LIGHTING_BIT);
        glEnable(GL_DEPTH_TEST);
        // Draw the front face only, except for the texts and lights.

        //add by hels windows版本注释了这行，如果开启会导致机器人模式渲染不全
        //glEnable(GL_CULL_FACE);

        // Set the view to the current camera settings.
        SetCamera(mScene, mCurrentTime, mCurrentAnimLayer, mCameraArray,
            mWindowWidth, mWindowHeight);

        FbxPose * lPose = NULL;
        if (mPoseIndex != -1)
        {
            lPose = mScene->GetPose(mPoseIndex);
        }

        // If one node is selected, draw it and its children.
        FbxAMatrix lDummyGlobalPosition;

        if(m_nPickStatus)
        {
            if(m_pOptimalPickeNode)
            {
                InitializeLights(mScene, mCurrentTime, lPose);
                DrawNodeRecursiveAndShowPickNode(mScene->GetRootNode(), mCurrentTime, mCurrentAnimLayer, lDummyGlobalPosition, lPose);
            }
            else
            {
                InitializeLights(mScene, mCurrentTime, lPose);
                DrawNodeRecursive(mScene->GetRootNode(), mCurrentTime, mCurrentAnimLayer, lDummyGlobalPosition, lPose, mShadingMode);
            }
        }
        else
        {
            if (mSelectedNode)
            {
                // Set the lighting before other things.
                InitializeLights(mScene, mCurrentTime, lPose);
                DrawNodeRecursive(mSelectedNode, mCurrentTime, mCurrentAnimLayer, lDummyGlobalPosition, lPose, mShadingMode);
            }
            // Otherwise, draw the whole scene.
            else
            {
                InitializeLights(mScene, mCurrentTime, lPose);
                DrawNodeRecursive(mScene->GetRootNode(), mCurrentTime, mCurrentAnimLayer, lDummyGlobalPosition, lPose, mShadingMode);
            }
        }


        glPopAttrib();
        glPopAttrib();
    }
    return true;
}

void SceneContext::SetFreshed()
{
    mStatus = SceneContext::MUST_BE_REFRESHED;
}

void SceneContext::OnReshape(int pWidth, int pHeight)
{
    glViewport(0, 0, (GLsizei)pWidth, (GLsizei)pHeight);
    mWindowWidth = pWidth;
    mWindowHeight = pHeight;
}

void SceneContext::OnKeyboard(unsigned char pKey)
{
    // Zoom In on '+' or '=' keypad keys
    if (pKey == 43 || pKey == 61)
    {
        FbxCamera* lCamera = GetCurrentCamera(mScene);
        if(lCamera)
        {
            //double lOriginalAperture = sqrt(lCamera->GetApertureWidth());
            CameraZoom(mScene, 10, mCameraZoomMode);
            mStatus = MUST_BE_REFRESHED;
        }
    }

    // Zoom Out on '-' or '_' keypad keys
    if (pKey == 45 || pKey == 95)
    {
        FbxCamera* lCamera = GetCurrentCamera(mScene);
        if(lCamera)
        {
            //double lOriginalAperture = sqrt(lCamera->GetApertureWidth()); 
            CameraZoom(mScene, 0 - 10, mCameraZoomMode);
            mStatus = MUST_BE_REFRESHED;
        }
    }

    // 'N' enable/disable normal display
    if (pKey == 'N' || pKey == 'n')
    {
        //gOGLScene->GetShadingManager()->SetDrawNormal(!gOGLScene->GetShadingManager()->IsDrawNormal());
    }

    // Pause and unpause when spacebar is pressed.
    if (pKey == ' ')
    {
        SetPause(!GetPause());
    }
}

void SceneContext::OnMouseWheel(int dir, int x, int y)
{
    CameraZoom(mScene,10 *dir, mCameraZoomMode);
    mStatus = MUST_BE_REFRESHED;
}
void SceneContext::OnMouse(int pButton, int pState, int pX, int pY)
{
    // Move the camera (orbit, zoom or pan) with the mouse.
    FbxCamera* lCamera = GetCurrentCamera(mScene);
    if (lCamera)
    {
        mCamPosition = lCamera->Position.Get();
        mCamCenter = lCamera->InterestPosition.Get();
        mRoll = lCamera->Roll.Get();
    }
    mLastX = pX;
    mLastY = pY;

    switch (pButton)
    {
    case RIGHT_BUTTON:
        // ORBIT (or PAN)
        switch (pState)
        {
        case BUTTON_DOWN:
            if (mCameraStatus == CAMERA_ZOOM)
            {
                mCameraStatus = CAMERA_PAN;
            }
            else
            {
                mCameraStatus = CAMERA_ORBIT;
            }
            break;

        default: // BUTTON_UP事件
            if (mCameraStatus == CAMERA_PAN)
            {
                mCameraStatus = CAMERA_ZOOM;
            }
            else
            {
                mCameraStatus = CAMERA_NOTHING;
            }
            break;
        }
        break;
    case LEFT_BUTTON:

        if (pState==BUTTON_DOWN)
        {
           DoPickNodeByScreenPos(pX,pY);
        }
        else
        {
            m_nPickStatus=false;
            mStatus=MUST_BE_REFRESHED;
            SetRotionPlayState(false);
        }
        break;
    default:
        break;
    }
}

void SceneContext::OnMouseMotion(int pX, int pY)
{
    int motion;

    switch (mCameraStatus)
    {
    default:
        break;
    case CAMERA_ORBIT:
        CameraOrbit(mScene, mCamPosition, mRoll, pX-mLastX, mLastY-pY);
        mStatus = MUST_BE_REFRESHED;
        break;

    case CAMERA_ZOOM:
        motion = mLastY-pY;
        CameraZoom(mScene, motion, mCameraZoomMode);
        mLastY = pY;
        mStatus = MUST_BE_REFRESHED;
        break;

    case CAMERA_PAN:
        CameraPan(mScene, mCamPosition, mCamCenter, mRoll, pX-mLastX, mLastY-pY);
        mStatus = MUST_BE_REFRESHED;
        break;
    }
    if(m_nPickStatus)
    {
        OnMove(m_pOptimalPickeNode,pX - mLastX,mLastY - pY);
        mStatus = MUST_BE_REFRESHED;
    }
}

void SceneContext::SetSelectedNode(FbxNode * pSelectedNode)
{
    mSelectedNode = pSelectedNode;
    mStatus = MUST_BE_REFRESHED;
}

void SceneContext::SetShadingMode(ShadingMode pMode)
{
    mShadingMode = pMode;
    mStatus = MUST_BE_REFRESHED;
}

void SceneContext::DisplayWindowMessage()
{
    glColor3f(1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, mWindowWidth, 0, mWindowHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Display message in the left up corner of the window
    const float lX = 5;
    const float lY = static_cast<float>(mWindowHeight) - 20;
    glTranslatef(lX, lY, 0);

    mDrawText->SetPointSize(15.f);
    mDrawText->Display(mWindowMessage.Buffer());

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void SceneContext::DisplayGrid(const FbxAMatrix & pTransform)
{
    glPushMatrix();
    glMultMatrixd(pTransform);

    // Draw a grid 500*500
    glColor3f(0.3f, 0.3f, 0.3f);
    glLineWidth(1.0);
    const int hw = 500;
    const int step = 20;
    const int bigstep = 100;
    int       i;

    // Draw Grid
    for (i = -hw; i <= hw; i+=step) {

        if (i % bigstep == 0) {
            glLineWidth(2.0);
        } else {
            glLineWidth(1.0);
        }
        glBegin(GL_LINES);
        glVertex3i(i,0,-hw);
        glVertex3i(i,0,hw);
        glEnd();
        glBegin(GL_LINES);
        glVertex3i(-hw,0,i);
        glVertex3i(hw,0,i);
        glEnd();

    }

    // Write some grid info
    const GLfloat zoffset = -2.f;
    const GLfloat xoffset = 1.f;
    mDrawText->SetPointSize(4.f);
    for (i = -hw; i <= hw; i+=bigstep)
    {

        FbxString scoord;
        int lCount;

        // Don't display origin
        //if (i == 0) continue;
        if (i == 0) {
            scoord = "0";
            lCount = (int)scoord.GetLen();
            glPushMatrix();
            glVertex3f(i+xoffset,0,zoffset);
            glRotatef(-90,1,0,0);
            
            mDrawText->Display(scoord.Buffer());

            glPopMatrix();

            continue;
        }

        // X coordinates
        scoord = "X: ";
        scoord += i;
        lCount = (int)scoord.GetLen();

        glPushMatrix();
        glTranslatef(i+xoffset,0,zoffset);
        glRotatef(-90,1,0,0);
        mDrawText->Display(scoord.Buffer());
        glPopMatrix();

        // Z coordinates
        scoord = "Z: ";
        scoord += i;
        lCount = (int)scoord.GetLen();

        glPushMatrix();
        glTranslatef(xoffset,0,i+zoffset);
        glRotatef(-90,1,0,0);
        mDrawText->Display(scoord.Buffer());
        glPopMatrix();

    }

    glPopMatrix();
}


void SceneContext::SetZoomMode( CameraZoomMode pZoomMode)
{
    if( pZoomMode == ZOOM_POSITION)
    {
        mCameraZoomMode = ZOOM_POSITION;
    }
    else
    {
        mCameraZoomMode =  ZOOM_FOCAL_LENGTH;
    }
    
}

/**************************************************************************
* 函数名: DoPickNodeByScreenPos
* 功 能: 根据屏幕坐标挑出3D模型相应节点并标记
* 参 数: int pX
* 参 数: int pY      当前屏幕坐标
* 时 间: 2015年8月21日
* 返回值: 无
* 作 者: 周志平
**************************************************************************/

void SceneContext::DoPickNodeByScreenPos(int pX, int pY)
{
    FbxNode *pNode=mScene->GetRootNode();
//    if (m_PickNode)
//    {
//        m_PickNode=NULL;
//    }
    pX = pX*m_nRatio;
    pY = pY*m_nRatio;
    m_pOptimalPickeNode = NULL;
    m_pNode9Mesh = NULL;
    m_pNode10Mesh = NULL;
    m_pNode11Mesh = NULL;
    m_pNode14Mesh = NULL;
    m_pNode15Mesh = NULL;
    m_pNode16Mesh = NULL;
    GLint      viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat   winX, winY, winZ;
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    winX = (float)pX;
    winY = viewport[3] -(float)pY;//+20; add by hels 不知道为什么+20，先注释掉
    glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    if (abs(winZ-1.0)<1E-6)
    {
        return;
    }
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &mposX, &mposY, &mposZ);
    FindPickNodeRecursive(pNode); //递归寻找子节点
    if(m_pOptimalPickeNode)
    {
        m_nPickStatus=true;
//        if (m_PickNode==NULL)
//        {
//            return;
//        }
        SetPickNodeCursor(m_pOptimalPickeNode); //设置光标状态
        GetPreAngleDate();            //获取上一次舵机位置
        for (int i = 0 ;i<m_nServoCount;i++)  //更新控制区UI
        {
            const char *s =m_pOptimalPickeNode->GetName();
            if (strcmp(s,m_pMotorNodeArray[i].szNodeName) == 0)
            {
                int nID=m_pMotorNodeArray[i].nID;
                int nAngle = m_pMotorNodeArray[i].fAngle;
                int nAngleOffset = m_pMotorNodeArray[i].fAngleOffset;
//                SetRotationDate(nID,nAngle-nAngleOffset);
                UpdateNodeDate(m_pMotorNodeArray[i].szNodeName,
                               nAngle,
                               nAngleOffset, TAC_3DModel);
                break;
            }
        }
        mStatus=MUST_BE_REFRESHED;
        SetRotionPlayState(true);
    }
}

/**************************************************************************
* 函数名: FindPickNodeRecursive
* 功 能: 遍历所有节点，查找是否有节点被选中
* 参 数: FbxNode* pNode   节点名，从该节点开始遍历
* 时 间: 2015年8月21日
* 返回值: 无
* 作 者: 周志平
**************************************************************************/
void SceneContext::FindPickNodeRecursive(FbxNode* pNode)
{

    if (pNode->GetNodeAttribute())
    {

        FindPickNode(pNode);

    }

    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
    {
        FindPickNodeRecursive(pNode->GetChild(lChildIndex));
    }
}

/**************************************************************************
* 函数名: FindPickNode
* 功 能: 查找当前节点是否被选中
* 参 数: FbxNode* pNode   节点名
* 时 间: 2015年8月21日
* 返回值: 无
* 作 者: 周志平
**************************************************************************/
void SceneContext::FindPickNode(FbxNode* pNode)
{
    const char *s = pNode->GetName();

    //add by hels去掉之前很多非舵机位置模型节点的判断，如Object040，Box011等这些外壳，这些判断的存在，使得舵机节点更不容易选中
    GLdouble pickZ;

    /************************* right hand *************************************/
    if (strcmp(s,"Cylinder034")==0)          //right hand3 1号舵机，肩关节
    {
        if (IsCurPosInsideNodeBox(pNode, &pickZ))
        {
            SavePickNode(pNode->GetParent(), pickZ);
            return;
        }
    }
    else if (strcmp(s,"Object039")==0)      //right hand2 2号舵机，手臂处关节
    {
        if (IsCurPosInsideNodeBox(pNode, &pickZ))
        {
            SavePickNode(pNode->GetParent()->GetParent()->GetParent(), pickZ);
            return;
        }
    }
    else if (strcmp(s,"Object029")==0)      //right hand1 3号舵机，手腕关节
    {
        if (IsCurPosInsideNodeBox(pNode, &pickZ))
        {
            SavePickNode(pNode->GetParent(), pickZ);
            return;
        }
    }

    /************************* right hand *************************************/


    /************************* left hand *************************************/
    else if (strcmp(s,"Cylinder036")==0)          //left hand3 4号舵机，肩关节
    {
        if (IsCurPosInsideNodeBox(pNode, &pickZ))
        {
            SavePickNode(pNode->GetParent(), pickZ);
            return;
        }
    }
    else if (strcmp(s,"Object148")==0)      //left hand2 5号舵机，手臂关节
    {
        if (IsCurPosInsideNodeBox(pNode, &pickZ))
        {
            SavePickNode(pNode->GetParent()->GetParent()->GetParent(), pickZ);
            return;
        }
    }
    else if (strcmp(s,"Object147")==0)      //left hand1，左手3D模型group少一层要注意，6号舵机，手腕关节
    {
        if (IsCurPosInsideNodeBox(pNode, &pickZ))
        {
            SavePickNode(pNode->GetParent(), pickZ);
            return;
        }
    }

    /************************* right hand *************************************/



     /********************************* left leg5 ************************************************/
    else if (strcmp(s,"Object137")==0)    // left leg5 12号舵机背面
    {
        if (IsCurPosInsideNodeBox(pNode, &pickZ))
        {
            SavePickNode(pNode->GetParent()->GetParent()->GetParent()->GetParent(), pickZ);
            return;
        }
    }
    else if (strcmp(s,"Object138")==0)    // left leg5 12号舵机前面
    {
        if (IsCurPosInsideNodeBox(pNode, &pickZ))
        {
            SavePickNode(pNode->GetParent()->GetParent()->GetParent()->GetParent(), pickZ);
            return;
        }
    }
     /********************************* left leg5 ************************************************/

    /********************************* left leg4 ************************************************/
    else if (strcmp(s,"Object122")==0)          //left leg4 14号舵机
    {
        if (IsCurPosInsideNodeBox(pNode, &pickZ))
        {
            //SavePickNode(pNode->GetParent(), pickZ);
            SavePickNode(m_pNode14Mesh, pickZ);
            return;
        }
    }
    else if (strcmp(s,"Object123")==0)          //left leg4 13号舵机
    {
        if (IsCurPosInsideNodeBox(pNode, &pickZ))
        {
            SavePickNode(pNode->GetParent()->GetParent(), pickZ);
            return;
        }
    }
    /************************************ left leg4 ************************************************/

    /******************************* left leg3 ******************************************************/
    else if (strcmp(s,"Object131")==0)          //left leg3 15号舵机
    {
        if (IsCurPosInsideNodeBox(pNode, &pickZ))
        {
            //SavePickNode(pNode->GetParent(), pickZ);
            SavePickNode(m_pNode15Mesh, pickZ);
            return;
        }
    }
    /******************************* left leg3 ******************************************************/


    /******************************* left leg2 ******************************************************/
    else if (strcmp(s,"Object128")==0)        //left leg2 16号舵机
    {
        if (IsCurPosInsideNodeBox(pNode, &pickZ))
        {
            //SavePickNode(pNode->GetParent(), pickZ);
            SavePickNode(m_pNode16Mesh, pickZ);
            return;
        }
    }

    /******************************* left leg2 ******************************************************/


    /******************************* left leg1 ******************************************************/
    else if (strcmp(s,"Object144")==0)        //left leg1 鞋子位置
    {
        m_pNode14Mesh = pNode->GetParent()->GetParent()->GetParent()->GetParent()->GetParent()->GetParent();//获取14号舵机控制的关节组
        m_pNode15Mesh = pNode->GetParent()->GetParent()->GetParent()->GetParent()->GetParent();
        m_pNode16Mesh = pNode->GetParent()->GetParent()->GetParent()->GetParent();
        if (IsCurPosInsideNodeBox(pNode, &pickZ))
        {
            SavePickNode(pNode->GetParent(), pickZ);
            return;
        }
    }

    /******************************* left leg1 ******************************************************/

    /********************************* right leg5 ************************************************/
   else if (strcmp(s,"Object064")==0)    // right leg5 7号舵机，关节前面
   {
       if (IsCurPosInsideNodeBox(pNode, &pickZ))
       {
           SavePickNode(pNode->GetParent()->GetParent(), pickZ);
           return;
       }
   }
   else if (strcmp(s,"Object065")==0)    // right leg5 7号舵机，关节背后
   {
       if (IsCurPosInsideNodeBox(pNode, &pickZ))
       {
           SavePickNode(pNode->GetParent()->GetParent(), pickZ);
           return;
       }
   }
    /********************************* right leg5 ************************************************/

   /********************************* right leg4 ************************************************/
   else if (strcmp(s,"Object057")==0)          //right leg4 9号舵机
   {
       if (IsCurPosInsideNodeBox(pNode, &pickZ))
       {
            //SavePickNode(pNode->GetParent()->GetParent(), pickZ);
           SavePickNode(m_pNode9Mesh, pickZ);
           return;
       }
   }
   else if (strcmp(s,"Object058")==0)          //right leg4 8号舵机
   {
       if (IsCurPosInsideNodeBox(pNode, &pickZ))
       {
            SavePickNode(pNode->GetParent()->GetParent(), pickZ);
           return;
       }
   }
   /************************************ right leg4 ************************************************/

   /******************************* right leg3 ******************************************************/
   else if (strcmp(s,"Object049")==0)          //right leg3 10号舵机
   {
       if (IsCurPosInsideNodeBox(pNode, &pickZ))
       {
           //SavePickNode(pNode->GetParent()->GetParent(), pickZ);
           SavePickNode(m_pNode10Mesh, pickZ);
           return;
       }
   }
   /******************************* right leg3 ******************************************************/


   /******************************* right leg2 ******************************************************/
   else if (strcmp(s,"Object041")==0)        //right leg2 11号舵机
   {
       if (IsCurPosInsideNodeBox(pNode, &pickZ))
       {
           //SavePickNode(pNode->GetParent()->GetParent(), pickZ);
           SavePickNode(m_pNode11Mesh, pickZ);
           return;
       }
   }

   /******************************* right leg2 ******************************************************/


   /******************************* right leg1 ******************************************************/
   else if (strcmp(s,"Object030")==0)        //right leg1 鞋子节点
   {
       m_pNode9Mesh = pNode->GetParent()->GetParent()->GetParent();//鞋子和object49可以获取到9号舵机控制的关节组
       m_pNode10Mesh = pNode->GetParent()->GetParent();//鞋子和11号舵机处才能要到10号舵机控制的关节组
       m_pNode11Mesh = pNode->GetParent(); //只有鞋子节点处能要到11号舵机控制的关节组

       if (IsCurPosInsideNodeBox(pNode, &pickZ))
       {
           SavePickNode(pNode->GetParent(), pickZ);
           return;
       }
   }

   /******************************* right leg1 ******************************************************/
}

/**************************************************************************
* 函数名: SavePickNode
* 功 能: 保存被选中的节点到buffer，有可能同时又多个节点被选中
* 参 数: FbxNode* pNode   节点
* 时 间: 2015年8月21日
* 返回值: 无
* 作 者: 周志平
**************************************************************************/
void SceneContext::SavePickNode(FbxNode* pNode, GLdouble z)
{
    const char* szNodename = pNode->GetName();
    if(m_pOptimalPickeNode == NULL)
    {
        m_OptimalZ = z;
        m_pOptimalPickeNode = pNode;
    }
    else
    {
        if (z > m_OptimalZ)
        {
            m_OptimalZ = z;
            m_pOptimalPickeNode = pNode;
        }
    }
}

/**************************************************************************
* 函数名: GetOptimalPickNode
* 功 能: 从buffer中选择最优节点
* 参 数: 无
* 时 间: 2015年8月21日
* 返回值: bool 如果选中了节点，则返回真，否则返回假;
* 作 者: 周志平
**************************************************************************/
//bool SceneContext::GetOptimalPickNode()
//{
//    FbxNode * pNode = NULL;
//    if (mPickArray.GetCount()<1)
//    {
//        return false;
//    }
    
//    for (int i=0;i<mPickArray.GetCount();i++)
//    {
//        pNode=mPickArray.GetAt(i);
//        //add by Leo Ho 通过节点所在z的深度来判断先点中的项，以防重叠情况下选中多个节点，设置错误的问题
//        FbxVector4 pBBoxMin;
//        FbxVector4 pBBoxMax;
//        FbxVector4 pBBoxCenter;
//        if (!pNode->EvaluateGlobalBoundingBoxMinMaxCenter (pBBoxMin, pBBoxMax, pBBoxCenter))
//        {
//            continue;
//        }
        
//        if(pBBoxCenter[2])
        
//        const char *s = pNode->GetName();
//        if ((strcmp(s,"right hand4") == 0) || (strcmp(s,"left hand4") == 0) || (strcmp(s,"right hand1") == 0) || (strcmp(s,"left hand1") == 0)
//            ||(strcmp(s,"left leg2") == 0) || (strcmp(s,"right leg2") == 0) || (strcmp(s,"head2") == 0))
//        {
//            m_PickNode=pNode;
//         (QCursor(Qt::OpenHandCursor));
//            return true;                                               //如果有高优先级节点被选中，则返回高优先级节点
//        }
//    }
//    for (int i=0;i<mPickArray.GetCount();i++)
//    {
//        pNode=mPickArray.GetAt(i);
//        const char *s = pNode->GetName();
//        if ((strcmp(s,"right hand2") == 0) || (strcmp(s,"left hand2") == 0)||(strcmp(s,"left leg3") == 0) || (strcmp(s,"right leg3") == 0))
//        {
//            m_PickNode=pNode;
////            SetCursor(LoadCursor(NULL,IDC_SIZENS ));
//            return true;                                               //如果有中优先级节点被选中，则返回中优先级节点
//        }

//    }
//    m_PickNode=mPickArray.GetAt(0);                                     //如果没有高中优先级节点被选中，返回最先选中节点
////    SetCursor(LoadCursor(NULL,IDC_SIZENS ));
//    return true;

//}

/**************************************************************************
* 函数名: SetPickNodeCursor
* 功 能: 根据选中的节点设置光标状态
* 参 数: 无
* 时 间: 2015年8月21日
* 返回值: bool 如果选中了节点，则返回真，否则返回假;
* 作 者: 周志平
**************************************************************************/
void SceneContext::SetPickNodeCursor(FbxNode* pNode)//设置光标状态
{
    const char *s = pNode->GetName();
    if ((strcmp(s,"rightleg5") == 0) || (strcmp(s,"leftleg5") == 0))
    {
        SetCursor(Qt::SizeHorCursor);
    }
    else
    {
        SetCursor(Qt::SizeVerCursor);
    }
}
/**************************************************************************
* 函数名: GetMaxNum
* 功 能: 获取较大数函数
* 参 数: float a
* 参 数: float b
* 时 间: 2015年8月21日
* 返回值:int 返回较大值
* 作 者: 周志平
**************************************************************************/
float SceneContext::GetMaxNum(float a,float b)
{
    return(a>=b?a:b);
}
/**************************************************************************
* 函数名: GetMinNum
* 功 能: 获取较小数函数
* 参 数: float a
* 参 数: float b
* 时 间: 2015年8月21日
* 返回值:int 返回较小值
* 作 者: 周志平
**************************************************************************/
float SceneContext::GetMinNum(float a,float b)
{
    return(a<=b?a:b);
}

/**************************************************************************
* 函数名: IsCurPosInsideNodeBox
* 功 能: 判断当前坐标是否在节点内部
* 参 数: FbxNode* pNode 当前节点
* 时 间: 2015年8月21日
* 返回值: bool 如果在节点内部，则返回真，否则返回假;
* 作 者: 周志平
**************************************************************************/
bool SceneContext::IsCurPosInsideNodeBox(FbxNode* pNode, GLdouble *z)   //判断当前坐标是否在节点内部
{

    if (pNode==NULL)
    {
        return false;
    }
    FbxVector4 RectMin;
    FbxVector4 RectMax;
    FbxVector4 pBBoxMin;
    FbxVector4 pBBoxMax;
    FbxVector4 pBBoxCenter;
    bool bRslt = false;
    if (!pNode->EvaluateGlobalBoundingBoxMinMaxCenter (pBBoxMin, pBBoxMax, pBBoxCenter))
    {
        return bRslt;
    }
    const char *s = pNode->GetName();
    if ((strcmp(s,"alpha2_polySurface4856") == 0) ||(strcmp(s,"polySurface4621") == 0) ||(strcmp(s,"alpha2_polySurface4857") == 0))
    {
        RectMin[0]=GetMinNum(pBBoxMin[0],pBBoxMax[0])-1;
        RectMin[1]=GetMinNum(pBBoxMin[1],pBBoxMax[1])-1;
        RectMin[2]=GetMinNum(pBBoxMin[2],pBBoxMax[2])-1;

        RectMax[0]=GetMaxNum(pBBoxMin[0],pBBoxMax[0])+1;
        RectMax[1]=GetMaxNum(pBBoxMin[1],pBBoxMax[1])+1;
        RectMax[2]=GetMaxNum(pBBoxMin[2],pBBoxMax[2])+5;
    }                                                                 //脖子关节比较难选中，范围放宽一点
    //模型中肩关节那个节点不好选中，适当放大肩关节到胸部外壳处
    else if(strcmp(s,"Cylinder034")==0)  //1号舵机，right hand肩关节
    {
        RectMin[0]=GetMinNum(pBBoxMin[0],pBBoxMax[0])-1;
        RectMin[1]=GetMinNum(pBBoxMin[1],pBBoxMax[1])-1;

        RectMax[0]=GetMaxNum(pBBoxMin[0],pBBoxMax[0])+2;
        RectMax[1]=GetMaxNum(pBBoxMin[1],pBBoxMax[1])+2;
    }
    else if(strcmp(s,"Cylinder036")==0) //4号舵机，left hand肩关节
    {
        RectMin[0]=GetMinNum(pBBoxMin[0],pBBoxMax[0])-2;
        RectMin[1]=GetMinNum(pBBoxMin[1],pBBoxMax[1])-2;

        RectMax[0]=GetMaxNum(pBBoxMin[0],pBBoxMax[0])+1;
        RectMax[1]=GetMaxNum(pBBoxMin[1],pBBoxMax[1])+1;
    }
    else
    {
        RectMin[0]=GetMinNum(pBBoxMin[0],pBBoxMax[0])-1;
        RectMin[1]=GetMinNum(pBBoxMin[1],pBBoxMax[1])-1;
        //RectMin[2]=GetMinNum(pBBoxMin[2],pBBoxMax[2])-1;

        RectMax[0]=GetMaxNum(pBBoxMin[0],pBBoxMax[0])+1;
        RectMax[1]=GetMaxNum(pBBoxMin[1],pBBoxMax[1])+1;
        //RectMax[2]=GetMaxNum(pBBoxMin[2],pBBoxMax[2])+1;
    }

    //add by hels 先不进行深度的比较
    if (((mposX>RectMin[0])&&(mposX<RectMax[0]))&&((mposY>RectMin[1])&&(mposY<RectMax[1]))/*&&((mposZ>RectMin[2])&&(mposZ<RectMax[2]))*/)
    {
        *z = RectMax[2];
        bRslt=true;
    }
    return bRslt;
}

void SceneContext::SetPickState(int state)
{
    m_nPickStatus = state;
    mStatus=MUST_BE_REFRESHED;
    SetRotionPlayState(state);
}
/**************************************************************************
* 函数名: OnMove
* 功 能: 根据鼠标移动的距离，拖动3D模型
* 参 数: FbxNode* pNode 当前节点
* 参 数: int dX x坐标差
* 参 数: int dY y坐标差
* 时 间: 2015年8月21日
* 返回值: 无
* 作 者: 周志平
**************************************************************************/
void SceneContext::OnMove(FbxNode *pNode,int dX, int dY)
{
    if ((NULL == pNode) || (NULL == m_pMotorNodeArray))
    {
        return;
    }

    if ((dX == 0) && (dY == 0))
    {
        return;
    }

    int dValue = 0;

    const char *sNodeName =pNode->GetName();
    const char * strNodeNameRightLeg5 = "rightleg5";
    const char * strNodeNameLeftleg5 = "leftleg5";

    if ((strcmp(sNodeName, strNodeNameRightLeg5) == 0) || (strcmp(sNodeName, strNodeNameLeftleg5) == 0))
    {
        dValue = dX;
    }
    else
    {
        dValue = dY;
    }

    if (0 == dValue)
    {
        return;
    }

    for (int i = 0; i<m_nServoCount; i++)
    {
        if (m_pMotorNodeArray[i].szNodeName)
        {
            if (!strcmp(m_pMotorNodeArray[i].szNodeName, sNodeName))
            {
                int nDir = m_pMotorNodeArray[i].nDir;
                const int nID = m_pMotorNodeArray[i].nID;
                int initOffset = m_pMotorNodeArray[i].fAngleOffset;
                //int nCurAngle = m_pMotorNodeArray[i].fAngle;
                int nOffset=0;

                int ndValueTemp = dValue;
                const int nMaxValueMove =  UBXBase::getInstance()->getDefaultActAngleValueMax(nID);  //ROBOT_DEFALUT_ACT_ANGLE_VALUE_MAX;//180
                if( (0 - nMaxValueMove) > dValue )
                {
                    ndValueTemp = 0 - nMaxValueMove;
                }
                else if(nMaxValueMove < dValue)
                {
                    ndValueTemp = nMaxValueMove;
                }

                if ((nID == 1) || (nID == 5) || (nID == 6) ||
                    (nID == 8) || (nID == 11) ||
                    (nID == 14) || (nID == 15))
                {
                    nOffset = m_pMotorNodeArray[i].fAngle - ndValueTemp;
                }
                else
                {
                    nOffset = m_pMotorNodeArray[i].fAngle + ndValueTemp;
                }

                const int nNodeAngleMin = initOffset;
                const int nNodeAngleMax = nMaxValueMove + initOffset; //180+ initOffset;//

                if(nOffset < nNodeAngleMin)
                {
                    nOffset = nNodeAngleMin;
                }
                else if (nOffset > nNodeAngleMax)
                {
                    nOffset = nNodeAngleMax;
                }

                if ((nOffset >= nNodeAngleMin) && (nOffset <= nNodeAngleMax))
                {
                    if( !strcmp(strNodeNameRightLeg5, sNodeName) ||  !strcmp(strNodeNameLeftleg5, sNodeName))
                    {
//                        int maxAngleForOneTime = 3;
//                        if(0 > nOffset )
//                        {
//                            maxAngleForOneTime = 0 - maxAngleForOneTime;
//                        }

//                        const int nTimes = nOffset/maxAngleForOneTime;
//                        for(int index = 0; index < nTimes; ++index)
//                        {
//                            const int nTempAngle = maxAngleForOneTime*(index+1);
//                            if( (0 < maxAngleForOneTime) && (nTempAngle >= m_pMotorNodeArray[i].fAngle) ||
//                                    (0 > maxAngleForOneTime) && (nTempAngle <= m_pMotorNodeArray[i].fAngle))
//                            {
//                                ManualSetRotation1(nDir, nTempAngle , sNodeName, 200, TAC_3DModel);                          //   3D视图更新
//                            }
//                        }
//                        ManualSetRotation1(nDir, nOffset, sNodeName, 200, TAC_3DModel);                           //   3D视图更新


                        if(!strcmp(strNodeNameRightLeg5, sNodeName))
                        {
                                for (int index = 0; index < m_nServoCount; ++index)
                                {
                                    if(!strcmp(strNodeNameLeftleg5, m_pMotorNodeArray[index].szNodeName))
                                    {
                                        const int nLeftLeg5Angle = m_pMotorNodeArray[index].fAngle;
                                        if(nLeftLeg5Angle < nOffset)
                                        {
                                            nOffset = nLeftLeg5Angle;
                                        }

                                        break;
                                    }
                                }
                        }
                        else  if(!strcmp(strNodeNameLeftleg5, sNodeName))
                        {
                                for (int index = 0; index < m_nServoCount; ++index)
                                {
                                    if(!strcmp(strNodeNameRightLeg5, m_pMotorNodeArray[index].szNodeName))
                                    {
                                        const int nRightLeg5Angle = m_pMotorNodeArray[index].fAngle;
                                        if(nRightLeg5Angle > nOffset)
                                        {
                                            nOffset = nRightLeg5Angle;
                                        }

                                        break;
                                    }
                                }
                        }

                        ManualSetRotation1(nDir, nOffset, sNodeName, 200, TAC_3DModel);                           //   3D视图更新
                    }
                    else
                    {
                        ManualSetRotation1(nDir, nOffset, sNodeName, 200, TAC_3DModel);                           //   3D视图更新
    //                    SetRotationDate(m_pMotorNodeArray[i].nID,nOffset - initOffset);       //   使机器人做动作，更更新UI
                    }
                }
            }
        }
    }
}

/**************************************************************************
* 函数名: GetDefaultMotorInfo
* 功 能: 根据Xml文件内容，获取默认的舵机位置
* 参 数: char* szXmlFile  xml文件名
* 时 间: 2015年8月21日
* 返回值: 如果获取到内容，返回真，否则返回假
* 作 者: 周志平
**************************************************************************/
bool SceneContext::GetDefaultMotorInfo(char* szXmlFile)
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
          m_nServoCount = iObjCnt - 2;
          m_pMotorNodeArray = new FbxMotorNodeEx[iObjCnt - 2];
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
                        m_pMotorNodeArray[iNum].nID = atoi(szValue);
                        continue;
                    }
                    else if(strcmp("Name",szKey) == 0)             // NAME
                    {
                        m_pMotorNodeArray[iNum].szNodeName = new char[strlen(szValue) + 1];
                        strcpy(m_pMotorNodeArray[iNum].szNodeName , szValue);
                        continue;
                    }
                    else if(strcmp("InitAngle",szKey) == 0)         // INIANGLE
                    {
                        float fIniAngle = atof(szValue);
                        m_pMotorNodeArray[iNum].fPreviousAngle += fIniAngle;
                        m_pMotorNodeArray[iNum].fAngle += fIniAngle;
                        continue;
                    }
                    else if(strcmp("Offset",szKey) ==0)             // OFFSET
                    {
                        float fOffset = atof(szValue);
                        m_pMotorNodeArray[iNum].fPreviousAngle +=fOffset;
                        m_pMotorNodeArray[iNum].fAngle +=fOffset;
                        m_pMotorNodeArray[iNum].fAngleOffset = fOffset;
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
                            m_pMotorNodeArray[iNum].nDir += (i + 1) * nAxis[i];
                        }
                        continue;
                    }

                }
             }
             iNum ++;
             bRet &=true;
          }
            //转到下一个兄弟节点
          NodePtr = NodePtr.nextSibling();
      }
      return bRet;
}


/**************************************************************************
* 函数名: DrawNodeExceptPickNodeRecursive
* 功 能: 重绘除了当前节点及其子节点以外的所有节点
* 参 数: FbxNode* pNode  从该节点开始重绘
* 参 数: FbxTime& pTime  当前时间
* 参 数: FbxAnimLayer* pAnimLayere 动画指针
* 参 数: FbxAMatrix& pParentGlobalPosition 父节点的世界坐标
* 参 数: FbxPose* pPose  当前姿势指针
* 时 间: 2015年8月21日
* 返回值: 无
* 作 者: 周志平
**************************************************************************/
void SceneContext::DrawNodeRecursiveAndShowPickNode(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer,
    FbxAMatrix& pParentGlobalPosition, FbxPose* pPose)
{

    FbxAMatrix lGlobalPosition = GetGlobalPosition(pNode, pTime, pPose, &pParentGlobalPosition);
        if (pNode->GetNodeAttribute())
        {
            // Geometry offset.
            // it is not inherited by the children.
            FbxAMatrix lGeometryOffset = GetGeometry(pNode);
            FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;
            if (pNode == m_pOptimalPickeNode)
            {
                  const char* Name = pNode->GetName();
                DrawNode(pNode, pTime, pAnimLayer, pParentGlobalPosition, lGlobalOffPosition, pPose, SHADING_MODE_DEFAULTCOLOR);
            }
            else
            {
                DrawNode(pNode, pTime, pAnimLayer, pParentGlobalPosition, lGlobalOffPosition, pPose, SHADING_MODE_SHADED);
            }

        }

        const int lChildCount = pNode->GetChildCount();
        for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
        {
            if (pNode == m_pOptimalPickeNode)
            {
                const char* Name = pNode->GetName();
                DrawNodeRecursive(pNode->GetChild(lChildIndex), pTime, pAnimLayer, lGlobalPosition, pPose,SHADING_MODE_DEFAULTCOLOR);
            }
            else
            {
                DrawNodeRecursiveAndShowPickNode(pNode->GetChild(lChildIndex), pTime, pAnimLayer, lGlobalPosition, pPose);
            }

        }
}

/**************************************************************************
* 函数名: SaveAngleDate
* 功 能: 保存当前节点舵机位置到buffer
* 参 数: int nAngle  舵机位置
* 参 数: FbxNode* pNode  节点
* 时 间: 2015年8月21日
* 返回值: 无
* 作 者: 周志平
**************************************************************************/
void SceneContext::SaveAngleDate(int nAngle,const char* pNode)
{
    for (int i = 0 ;i<m_nServoCount;i++)
    {
        if (strcmp(m_pMotorNodeArray[i].szNodeName,pNode)==0)
        {
            m_pMotorNodeArray[i].fPreviousAngle=nAngle;
            break;
        }
    }
}
/**************************************************************************
* 函数名: GetPreAngleDate
* 功 能: 从buffer中获取上一次舵机状态
* 时 间: 2015年8月21日
* 返回值: 无
* 作 者: 周志平
**************************************************************************/
void SceneContext::GetPreAngleDate()
{
    if (m_pMotorNodeArray)
    {
        for (int i = 0 ;i<m_nServoCount;i++)
        {
            m_pMotorNodeArray[i].fAngle = m_pMotorNodeArray[i].fPreviousAngle;   //读取上一次舵机状态
        }
    }
}

/**************************************************************************
* 函数名: SaveViewPortDate
* 功 能: 获取屏幕像素数据
* 参 数: int& nWidth     视口宽度
* 参 数: int& nHeigth    视口高度
* 时 间: 2015年8月21日
* 返回值: GLbyte*        字节流指针
* 作 者: 周志平
**************************************************************************/
GLbyte * SceneContext::SaveViewPortDate(int& nWidth, int& nHeigth)
{
    GLint    viewPort[4];
    GLbyte* colorArr=NULL;
    glGetIntegerv(GL_VIEWPORT, viewPort);
    nWidth = viewPort[2];
    nHeigth = viewPort[3];
    colorArr = new GLbyte[ nWidth * nHeigth * 4 ];
    glReadPixels(viewPort[0], viewPort[1], viewPort[2], viewPort[3], GL_RGBA /*GL_BGRA*/, GL_UNSIGNED_BYTE, colorArr);
    return colorArr;
}
