#-------------------------------------------------
#
# Project created by QtCreator 2015-11-04T13:59:40
#
#-------------------------------------------------

QT       += widgets opengl xml

QT       -= gui

TARGET = LibFBX
TEMPLATE = lib
CONFIG += staticlib

DEFINES += FREEGLUT_STATIC


INCLUDEPATH += $$PWD/../Common \
    $$PWD/../trd/fbxsdk/include \
    $$PWD/../trd/glew/include \
    $$PWD/../trd/freeglut/include

SOURCES += FbxPlayer.cpp \
    Common.cxx \
    DrawScene.cxx \
    DrawText.cxx \
    GetPosition.cxx \
    GlFunctions.cxx \
    SceneCache.cxx \
    SceneContext.cxx \
    SetCamera.cxx \
    targa.cxx

HEADERS += FbxPlayer.h \
    fbxsdk/core/arch/fbxalloc.h \
    fbxsdk/core/arch/fbxarch.h \
    fbxsdk/core/arch/fbxdebug.h \
    fbxsdk/core/arch/fbxnew.h \
    fbxsdk/core/arch/fbxstdcompliant.h \
    fbxsdk/core/arch/fbxtypes.h \
    fbxsdk/core/base/fbxarray.h \
    fbxsdk/core/base/fbxbitset.h \
    fbxsdk/core/base/fbxcharptrset.h \
    fbxsdk/core/base/fbxcontainerallocators.h \
    fbxsdk/core/base/fbxdynamicarray.h \
    fbxsdk/core/base/fbxfile.h \
    fbxsdk/core/base/fbxfolder.h \
    fbxsdk/core/base/fbxhashmap.h \
    fbxsdk/core/base/fbxintrusivelist.h \
    fbxsdk/core/base/fbxmap.h \
    fbxsdk/core/base/fbxmemorypool.h \
    fbxsdk/core/base/fbxmultimap.h \
    fbxsdk/core/base/fbxpair.h \
    fbxsdk/core/base/fbxredblacktree.h \
    fbxsdk/core/base/fbxset.h \
    fbxsdk/core/base/fbxstatus.h \
    fbxsdk/core/base/fbxstring.h \
    fbxsdk/core/base/fbxstringlist.h \
    fbxsdk/core/base/fbxtime.h \
    fbxsdk/core/base/fbxtimecode.h \
    fbxsdk/core/base/fbxutils.h \
    fbxsdk/core/math/fbxaffinematrix.h \
    fbxsdk/core/math/fbxdualquaternion.h \
    fbxsdk/core/math/fbxmath.h \
    fbxsdk/core/math/fbxmatrix.h \
    fbxsdk/core/math/fbxquaternion.h \
    fbxsdk/core/math/fbxtransforms.h \
    fbxsdk/core/math/fbxvector2.h \
    fbxsdk/core/math/fbxvector4.h \
    fbxsdk/core/sync/fbxatomic.h \
    fbxsdk/core/sync/fbxclock.h \
    fbxsdk/core/sync/fbxsync.h \
    fbxsdk/core/sync/fbxthread.h \
    fbxsdk/core/fbxclassid.h \
    fbxsdk/core/fbxconnectionpoint.h \
    fbxsdk/core/fbxdatatypes.h \
    fbxsdk/core/fbxemitter.h \
    fbxsdk/core/fbxevent.h \
    fbxsdk/core/fbxeventhandler.h \
    fbxsdk/core/fbxlistener.h \
    fbxsdk/core/fbxloadingstrategy.h \
    fbxsdk/core/fbxmanager.h \
    fbxsdk/core/fbxmodule.h \
    fbxsdk/core/fbxobject.h \
    fbxsdk/core/fbxperipheral.h \
    fbxsdk/core/fbxplugin.h \
    fbxsdk/core/fbxplugincontainer.h \
    fbxsdk/core/fbxproperty.h \
    fbxsdk/core/fbxpropertydef.h \
    fbxsdk/core/fbxpropertyhandle.h \
    fbxsdk/core/fbxpropertypage.h \
    fbxsdk/core/fbxpropertytypes.h \
    fbxsdk/core/fbxquery.h \
    fbxsdk/core/fbxqueryevent.h \
    fbxsdk/core/fbxscopedloadingdirectory.h \
    fbxsdk/core/fbxscopedloadingfilename.h \
    fbxsdk/core/fbxstream.h \
    fbxsdk/core/fbxsymbol.h \
    fbxsdk/core/fbxsystemunit.h \
    fbxsdk/core/fbxxref.h \
    fbxsdk/fileio/collada/fbxcolladaanimationelement.h \
    fbxsdk/fileio/collada/fbxcolladaelement.h \
    fbxsdk/fileio/collada/fbxcolladaiostream.h \
    fbxsdk/fileio/collada/fbxcolladanamespace.h \
    fbxsdk/fileio/collada/fbxcolladatokens.h \
    fbxsdk/fileio/collada/fbxcolladautils.h \
    fbxsdk/fileio/collada/fbxreadercollada14.h \
    fbxsdk/fileio/collada/fbxwritercollada14.h \
    fbxsdk/fileio/fbx/fbxio.h \
    fbxsdk/fileio/fbx/fbxreaderfbx5.h \
    fbxsdk/fileio/fbx/fbxreaderfbx6.h \
    fbxsdk/fileio/fbx/fbxreaderfbx7.h \
    fbxsdk/fileio/fbx/fbxwriterfbx5.h \
    fbxsdk/fileio/fbx/fbxwriterfbx6.h \
    fbxsdk/fileio/fbx/fbxwriterfbx7.h \
    fbxsdk/fileio/fbxbase64coder.h \
    fbxsdk/fileio/fbxexporter.h \
    fbxsdk/fileio/fbxexternaldocreflistener.h \
    fbxsdk/fileio/fbxfiletokens.h \
    fbxsdk/fileio/fbxglobalcamerasettings.h \
    fbxsdk/fileio/fbxgloballightsettings.h \
    fbxsdk/fileio/fbxglobalsettings.h \
    fbxsdk/fileio/fbxgobo.h \
    fbxsdk/fileio/fbximporter.h \
    fbxsdk/fileio/fbxiobase.h \
    fbxsdk/fileio/fbxiopluginregistry.h \
    fbxsdk/fileio/fbxiosettings.h \
    fbxsdk/fileio/fbxiosettingspath.h \
    fbxsdk/fileio/fbxprogress.h \
    fbxsdk/fileio/fbxreader.h \
    fbxsdk/fileio/fbxstatistics.h \
    fbxsdk/fileio/fbxstatisticsfbx.h \
    fbxsdk/fileio/fbxwriter.h \
    fbxsdk/scene/animation/fbxanimcurve.h \
    fbxsdk/scene/animation/fbxanimcurvebase.h \
    fbxsdk/scene/animation/fbxanimcurvefilters.h \
    fbxsdk/scene/animation/fbxanimcurvenode.h \
    fbxsdk/scene/animation/fbxanimevalclassic.h \
    fbxsdk/scene/animation/fbxanimevalstate.h \
    fbxsdk/scene/animation/fbxanimevaluator.h \
    fbxsdk/scene/animation/fbxanimlayer.h \
    fbxsdk/scene/animation/fbxanimstack.h \
    fbxsdk/scene/animation/fbxanimutilities.h \
    fbxsdk/scene/constraint/fbxcharacter.h \
    fbxsdk/scene/constraint/fbxcharacternodename.h \
    fbxsdk/scene/constraint/fbxcharacterpose.h \
    fbxsdk/scene/constraint/fbxconstraint.h \
    fbxsdk/scene/constraint/fbxconstraintaim.h \
    fbxsdk/scene/constraint/fbxconstraintcustom.h \
    fbxsdk/scene/constraint/fbxconstraintparent.h \
    fbxsdk/scene/constraint/fbxconstraintposition.h \
    fbxsdk/scene/constraint/fbxconstraintrotation.h \
    fbxsdk/scene/constraint/fbxconstraintscale.h \
    fbxsdk/scene/constraint/fbxconstraintsinglechainik.h \
    fbxsdk/scene/constraint/fbxconstraintutils.h \
    fbxsdk/scene/constraint/fbxcontrolset.h \
    fbxsdk/scene/constraint/fbxhik2fbxcharacter.h \
    fbxsdk/scene/geometry/fbxblendshape.h \
    fbxsdk/scene/geometry/fbxblendshapechannel.h \
    fbxsdk/scene/geometry/fbxcache.h \
    fbxsdk/scene/geometry/fbxcachedeffect.h \
    fbxsdk/scene/geometry/fbxcamera.h \
    fbxsdk/scene/geometry/fbxcamerastereo.h \
    fbxsdk/scene/geometry/fbxcameraswitcher.h \
    fbxsdk/scene/geometry/fbxcluster.h \
    fbxsdk/scene/geometry/fbxdeformer.h \
    fbxsdk/scene/geometry/fbxgenericnode.h \
    fbxsdk/scene/geometry/fbxgeometry.h \
    fbxsdk/scene/geometry/fbxgeometrybase.h \
    fbxsdk/scene/geometry/fbxgeometryweightedmap.h \
    fbxsdk/scene/geometry/fbxlayer.h \
    fbxsdk/scene/geometry/fbxlayercontainer.h \
    fbxsdk/scene/geometry/fbxlight.h \
    fbxsdk/scene/geometry/fbxlimitsutilities.h \
    fbxsdk/scene/geometry/fbxline.h \
    fbxsdk/scene/geometry/fbxlodgroup.h \
    fbxsdk/scene/geometry/fbxmarker.h \
    fbxsdk/scene/geometry/fbxmesh.h \
    fbxsdk/scene/geometry/fbxnode.h \
    fbxsdk/scene/geometry/fbxnodeattribute.h \
    fbxsdk/scene/geometry/fbxnull.h \
    fbxsdk/scene/geometry/fbxnurbs.h \
    fbxsdk/scene/geometry/fbxnurbscurve.h \
    fbxsdk/scene/geometry/fbxnurbssurface.h \
    fbxsdk/scene/geometry/fbxopticalreference.h \
    fbxsdk/scene/geometry/fbxpatch.h \
    fbxsdk/scene/geometry/fbxproceduralgeometry.h \
    fbxsdk/scene/geometry/fbxshape.h \
    fbxsdk/scene/geometry/fbxskeleton.h \
    fbxsdk/scene/geometry/fbxskin.h \
    fbxsdk/scene/geometry/fbxsubdeformer.h \
    fbxsdk/scene/geometry/fbxsubdiv.h \
    fbxsdk/scene/geometry/fbxtrimnurbssurface.h \
    fbxsdk/scene/geometry/fbxvertexcachedeformer.h \
    fbxsdk/scene/geometry/fbxweightedmapping.h \
    fbxsdk/scene/shading/fbxbindingoperator.h \
    fbxsdk/scene/shading/fbxbindingsentryview.h \
    fbxsdk/scene/shading/fbxbindingtable.h \
    fbxsdk/scene/shading/fbxbindingtablebase.h \
    fbxsdk/scene/shading/fbxbindingtableentry.h \
    fbxsdk/scene/shading/fbxconstantentryview.h \
    fbxsdk/scene/shading/fbxentryview.h \
    fbxsdk/scene/shading/fbxfiletexture.h \
    fbxsdk/scene/shading/fbximplementation.h \
    fbxsdk/scene/shading/fbximplementationfilter.h \
    fbxsdk/scene/shading/fbximplementationutils.h \
    fbxsdk/scene/shading/fbxlayeredtexture.h \
    fbxsdk/scene/shading/fbxlayerentryview.h \
    fbxsdk/scene/shading/fbxoperatorentryview.h \
    fbxsdk/scene/shading/fbxproceduraltexture.h \
    fbxsdk/scene/shading/fbxpropertyentryview.h \
    fbxsdk/scene/shading/fbxsemanticentryview.h \
    fbxsdk/scene/shading/fbxshadingconventions.h \
    fbxsdk/scene/shading/fbxsurfacelambert.h \
    fbxsdk/scene/shading/fbxsurfacematerial.h \
    fbxsdk/scene/shading/fbxsurfacephong.h \
    fbxsdk/scene/shading/fbxtexture.h \
    fbxsdk/scene/fbxaxissystem.h \
    fbxsdk/scene/fbxcollection.h \
    fbxsdk/scene/fbxcollectionexclusive.h \
    fbxsdk/scene/fbxcontainer.h \
    fbxsdk/scene/fbxcontainertemplate.h \
    fbxsdk/scene/fbxdisplaylayer.h \
    fbxsdk/scene/fbxdocument.h \
    fbxsdk/scene/fbxdocumentinfo.h \
    fbxsdk/scene/fbxenvironment.h \
    fbxsdk/scene/fbxgroupname.h \
    fbxsdk/scene/fbxlibrary.h \
    fbxsdk/scene/fbxobjectfilter.h \
    fbxsdk/scene/fbxobjectmetadata.h \
    fbxsdk/scene/fbxobjectscontainer.h \
    fbxsdk/scene/fbxpose.h \
    fbxsdk/scene/fbxreference.h \
    fbxsdk/scene/fbxscene.h \
    fbxsdk/scene/fbxselectionnode.h \
    fbxsdk/scene/fbxselectionset.h \
    fbxsdk/scene/fbxtakeinfo.h \
    fbxsdk/scene/fbxthumbnail.h \
    fbxsdk/scene/fbxvideo.h \
    fbxsdk/utils/fbxclonemanager.h \
    fbxsdk/utils/fbxdeformationsevaluator.h \
    fbxsdk/utils/fbxembeddedfilesaccumulator.h \
    fbxsdk/utils/fbxgeometryconverter.h \
    fbxsdk/utils/fbxmanipulators.h \
    fbxsdk/utils/fbxmaterialconverter.h \
    fbxsdk/utils/fbxnamehandler.h \
    fbxsdk/utils/fbxprocessor.h \
    fbxsdk/utils/fbxprocessorshaderdependency.h \
    fbxsdk/utils/fbxprocessorxref.h \
    fbxsdk/utils/fbxprocessorxrefuserlib.h \
    fbxsdk/utils/fbxrenamingstrategy.h \
    fbxsdk/utils/fbxrenamingstrategybase.h \
    fbxsdk/utils/fbxrenamingstrategyfbx5.h \
    fbxsdk/utils/fbxrenamingstrategyfbx6.h \
    fbxsdk/utils/fbxrenamingstrategyfbx7.h \
    fbxsdk/utils/fbxrenamingstrategyutilities.h \
    fbxsdk/utils/fbxrootnodeutility.h \
    fbxsdk/utils/fbxusernotification.h \
    fbxsdk/fbxsdk_def.h \
    fbxsdk/fbxsdk_nsbegin.h \
    fbxsdk/fbxsdk_nsend.h \
    fbxsdk/fbxsdk_version.h \
    Common.h \
    DrawScene.h \
    DrawText.h \
    fbxsdk.h \
    GetPosition.h \
    GlFunctions.h \
    SceneCache.h \
    SceneContext.h \
    SetCamera.h \
    targa.h \
    ../Common/robotDefine.h

macx:{
    LIBS += -L$$PWD/../trd/fbxsdk/lib/osx -lfbxsdk
    PRE_TARGETDEPS += $$PWD/../trd/fbxsdk/lib/osx/libfbxsdk.a

    LIBS += -L$$PWD/../trd/glew/lib/osx -lGLEW64
    PRE_TARGETDEPS += $$PWD/../trd/glew/lib/osx/libGLEW64.a
}else:win32:{    
    CONFIG(debug, debug|release){
        LIBS += -L$$PWD/../trd/fbxsdk/lib/win32/debug -llibfbxsdk-md
        PRE_TARGETDEPS += $$PWD/../trd/fbxsdk/lib/win32/debug/libfbxsdk-md.lib

        #LIBS += -L$$PWD/../trd/glew/lib/win32/debug -lglew32sd
        #PRE_TARGETDEPS += $$PWD/../trd/glew/lib/win32/debug/glew32sd.lib
    }else{
        LIBS += -L$$PWD/../trd/fbxsdk/lib/win32/release -llibfbxsdk-md
        PRE_TARGETDEPS += $$PWD/../trd/fbxsdk/lib/win32/release/libfbxsdk-md.lib

        #LIBS += -L$$PWD/../trd/glew/lib/win32/release -lglew32s
        #PRE_TARGETDEPS += $$PWD/../trd/glew/lib/win32/release/glew32s.lib
    }

    DEPENDPATH += $$PWD/../trd/freeglut/lib/win32
    LIBS += -L$$PWD/../trd/freeglut/lib/win32 -lfreeglut_static
    PRE_TARGETDEPS += $$PWD/../trd/freeglut/lib/win32/freeglut_static.lib

    DEPENDPATH += $$PWD/../trd/glew/lib/win32
    LIBS += -L$$PWD/../trd/glew/lib/win32 -lglew32
}

macx:{
    OpenGL = -F/Library/Frameworks
    QMAKE_CFLAGS += $$OpenGL
    QMAKE_CXXFLAGS += $$OpenGL
    QMAKE_OBJECTIVE_CXXFLAGS += $$OpenGL
    LIBS += $$OpenGL
    LIBS += -framework OpenGL
}

macx:{
#INCLUDEPATH +=/opt/X11/include
#DEPENDPATH +=/opt/X11/include

#LIBS +=-L/opt/X11/lib/ -lglut.3
#LIBS +=-L/opt/X11/lib/ -lGL.1
#LIBS +=-L/opt/X11/lib/ -lGLU.1
}else:win32:{
    #LIBS += -L$$PWD/../trd/freeglut/lib/win32 -lfreeglut_static
    #PRE_TARGETDEPS += $$PWD/../trd/freeglut/lib/win32/freeglut_static.lib
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXBase/release/ -lUBXBase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXBase/debug/ -lUBXBase
else:unix: LIBS += -L$$OUT_PWD/../UBXBase/ -lUBXBase

INCLUDEPATH += $$PWD/../UBXBase
DEPENDPATH += $$PWD/../UBXBase
