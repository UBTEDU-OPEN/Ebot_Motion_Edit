#ifndef STABLE
#define STABLE


#include <QWidget>
#include <QString>
#include <QPoint>
#include <QtMath>
#include <QDebug>
#include <QPen>

#include <vector>
using namespace std;

#include "../UBXBase/UBXBase.h"
#include "../Common/UBXPublic.h"
#include "../UBXBase/UBXCritSec.h"
#include "../LibTreeNavigation/ILibTreeNavigation.h"

#include "../Common/Util.h"
#include "robotDefine.h"

const int INVALID_BLOCK_ID = -1;
const int TIME_OFFSET_1 = 1;

//#define MAX_PATH  256

typedef struct
{
    int nTotalTime;		// 总的时间
    int nMyPos;         // 偏移值

}LAYER_CONFIGURE, *PLAYER_CONFIGURE;

typedef struct _UBXMusicInfo{
    const char* szFilePath;
    const char* szFileName;
    const char* szWavePath;
//    UBXStreamTime StreamTime;
    int           nBPM;
    //int           nLength;
    //UBXMusicInfo::
    _UBXMusicInfo()
    {
        szFileName = NULL;
        szFilePath= NULL;
        nBPM = 0;
        szWavePath = NULL;
        //nLength = sizeof(_UBXMusicInfo);
    }
} UBXMusicInfo;


#define SecsPerMinute   60000    //每分钟秒数
enum PLAYSTATE
{
    PLAYSTATE_STOP = 0 ,    //层停止
    PLAYSTATE_PLAYING  ,    //层播放
    PLAYSTATE_PAUSING  ,    //层暂停
};
#define  SINGLE_LAYER_DEBUG 1
#define  ALL_LAYER_DEBUG 2


#define GRID_MAX_TIMEVALUE  600000   //10minute(ms)
#define GRID_MIN_TIMEVALUE  200        //(ms)

//#define GRID_MAX_ANGLEVALUE  ROBOT_DEFALUT_ACT_ANGLE_VALUE_MAX  //180
#define GRID_MIN_ANGLEVALUE   ROBOT_DEFALUT_ACT_ANGLE_VALUE_MIN     //0

//舵机的最大运动时间
#define MOTOR_MAX_MOVING_TIME 5000   //ms

//舵机数（此处可以改进，通过读XML文件的方式读取舵机数）
//#define SERVO_COUNT 16

// 动作操作指令
enum emMenuAction
{
    eMenuNoAction = 0,

    // 操作动作帧
    eMenuMotionFrameModify,  // 修改动作帧
    eMenuMotionFrameInsert,  // 插入动作帧
    eMenuMotionFrameSplitInsert, // 插入拆分动作帧
    eMenuMotionFrameMarkFrame, // 标记动作帧
    eMenuMotionFrameCopy, // 拷贝动作帧
    eMenuMotionFramePaste, // 粘贴
    eMenuMotionFramePasteInNewGroup, // 新建动作组，并粘贴复制的动作帧
    eMenuMotionFrameDelete, // 删除动作帧
    eMenuMotionFrameMarkGroup, // 标记动作组
    eMenuMotionFrameAddFrameToPreGroup, // 新建动作帧并添加到上一个动作组
    eMenuMotionFrameCreateNewFrameInNewGroup, // 新建动作帧并添加到新建动作组

    // 操作动作组
    eMenuMotionBlockCopy,
    eMenuMotionBlockPaste,
    eMenuMotionBlockCut,
    eMenuMotionBlockDelete,
    eMenuMotionBlockPlay,
    eMenuMotionBlockCancelPlay,
    eMenuMotionBlockInsert,
    eMenuMotionBlockSaveTemplate,
    eMenuMotionBlockModifyName,
    eMenuMotionBlockMergeGroup,
};

extern bool g_IsUpdateUIByData;
extern bool g_IsModifyGroup;

#endif // STABLE

