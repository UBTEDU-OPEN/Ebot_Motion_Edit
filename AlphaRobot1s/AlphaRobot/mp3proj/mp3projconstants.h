/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：mp3projconstants.h
* 创建时间：2016/01/07
* 文件标识：
* 文件摘要：mp3proj模块常量定义
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/01/07
* 版本摘要：
*/

#include <QColor>
#include <QMetaType>

#ifndef MP3PROJCONSTANTS
#define MP3PROJCONSTANTS

namespace Constants
{
    //波形控件常量定义，波形下方每个方框为一个cell
    const int DEFAULT_CELL_COUNT = 10; //默认Cell个数（每1秒一个刻度）
    const int DEFAULT_CELL_WIDTH = 99; //每个Cell宽度
    const int DEFAULT_LINE_THICK = 1; //刻度线宽度
    const int DEFULAT_PLOT_THICK = 0;//波形图线宽
    const int DEFAULT_LINE_HEIGHT = 20; // 默认线高

    const int ADD_OFFSET = 5;//cellcount的偏移个数
    const int SELECT_LINE_HOVER_OFFSET = 10;  // 选择线hover范围

    //没有单独定义宽高的，取DEFAULT_CELL_WIDTH和DEFAULT_CELL_HEIGHT默认值
    const int DEFAULT_CELL_HEIGHT = 34;//Cell高度
    const int DEFAULT_CELL_TIME_HEIGHT = 26;//时间Cell高度
    const int DEFAULT_CELL_MUSIC_HEIGHT = 236;//音乐层Cell高度
    const int DEFAULT_MUSIC_LAYER_TOTAL_HEIGHT = DEFAULT_CELL_HEIGHT + DEFAULT_CELL_TIME_HEIGHT + DEFAULT_CELL_MUSIC_HEIGHT;

    const int MP3_SAMPLE_STEP = 3; // 采样步长，在采样率的基础上按步长再采样

    //相关颜色定义
    const QColor COLOR_SEL_LINE = Qt::red;//选择线颜色
    const QColor COLOR_MARK_FRAME_LINE = Qt::green; // 标记动作帧线颜色
    const QColor COLOR_MARK_BLOCK_LINE = Qt::blue;  // 标记动作组线颜色
    const QColor COLOR_LINE =  QColor("#5d5d5d");//刻度线颜色
    const QColor COLOR_DEF_BRUSH_CELL = QColor("#707070");//底部cell默认填充颜色
    const QColor COLOR_BRUSH_CELL = QColor("#8b7f64");//底部cell填充颜色
    const QColor COLOR_TIME_BG = QColor("#404040"); //时间背景颜色
    const QColor COLOR_TIME_FONT = QColor("#c4c4c4"); //时间字体颜色
    const QColor COLOR_MP3_BG = QColor(51, 51, 51); //时间线、波形、动作组窗体背景颜色
    const QColor COLOR_WAVE_BACK = QColor(51, 51, 51); //波形背景色
    const QColor COLOR_WAVE_LINE = QColor(221, 141, 95);//波形颜色
    const QColor COLOR_WAVE_BASE_LINE = Qt::red;//波形中间基线
    const QColor COLOR_GROUP_BLOCK_DEFAULT = QColor("#304865");//动作组默认颜色
    const QColor COLOR_GROUP_BLOCK_SELECT = QColor("#406189");//动作组选中颜色
    const QColor COLOR_ACTION_BLOCK_SELECT = QColor("#5781b5");//Action动作块选中颜色
    const QColor COLOR_RECT_SELECT_BORER = QColor("#FFBB77");  // 矩形选择框线条颜色
    const QColor COLOR_RECT_SELECT_FILL = QColor("#FFBB77");  // 矩形选择框填充颜色
    const QColor COLOR_ACTION_BLOCK_LINE_DIVIDE = QColor("#696969"); // 动作组分割线颜色
    const QColor COLOR_ACTION_FRAME_LINE_DIVEDE = QColor("#404040");  // 动作帧分割线颜色
    const QColor COLOR_ACTION_FRAME_SELECT_FILLED = QColor("#406189");  // 选中帧填充颜色

    // 鼠标状态
    enum emMouseState
    {
        eMouseStateUnknonw = 0,
        eMouseStatePressed,  // 鼠标按下
        eMouseStateMove,  // 鼠标移动
        eMouseStateDragMove, // 点击并拖动
        eMouseStateReleased, // 鼠标松开
    };

    enum emMoveDirection
    {
        eMoveDirNone = 0,
        eMoveDirLeft,
        eMoveDirRight,
    };

    struct MP3Info
    {
    public:
        QString m_strMP3FilePath;  // MP3文件路径
        int  m_nDuration;  // 音乐时长，以m为单位
        int m_nBPM;  // 音乐节拍

    public:
        MP3Info()
        {
            m_strMP3FilePath = "";
            m_nDuration = 0;
            m_nBPM = 0;
        }

        ~MP3Info()
        {
            m_strMP3FilePath = "";
            m_nDuration = 0;
            m_nBPM = 0;
        }

        MP3Info(const MP3Info& itemobj)
        {
            m_strMP3FilePath = itemobj.m_strMP3FilePath;
            m_nDuration = itemobj.m_nDuration;
            m_nBPM = itemobj.m_nBPM;
        }

        MP3Info& operator = (const MP3Info& itemobj)
        {
            m_strMP3FilePath = itemobj.m_strMP3FilePath;
            m_nDuration = itemobj.m_nDuration;
            m_nBPM = itemobj.m_nBPM;
            return *this;
        }
    };
}

Q_DECLARE_METATYPE(Constants::MP3Info)
#endif // MP3PROJCONSTANTS

