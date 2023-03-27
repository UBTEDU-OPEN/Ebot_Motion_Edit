#ifndef UBXUNDOREDOMANAGERCONST
#define UBXUNDOREDOMANAGERCONST

#include <QEvent>
#include <QColor>

#include "ubxundoredomanager_global.h"

namespace Constants {

    const int MAX_REDO_UNDO_NUMBER = 10; // 支持撤销重做的最大次数
    const int MARK_LINE_TOP_ICON_WIDTH = 16; // 标记线顶部图标宽度

    const int MARK_LINE_THIN_DEFAULT = 1;  // 标记线宽度

    const QColor COLOR_MARK_LINE = QColor("#E1E100"); // 动作帧标记线颜色
    const QColor COLOR_MARK_BLOCK = QColor("#A3D1D1");  // 动作组标记线颜色

    const QString KEY_WORD_MOTION = "motion";

    // 执行操作类型
    enum emExecuteType
    {
        eExecuteUnknonw = 0,
        eExecuteUndo, // 执行undo操作
        eExecuteRedo,  // 指定redo操作
    };

    // 模块域
    enum emModuleDomain
    {
        eModuleDomainMP3Proj = 0,  // MP3模块
        eModuleDomainMotionEditor,  // 动作编辑模块
        eModuleDomainNavigation,      // 导航栏模块
    };

    // 操作类型
    enum emOperateCode
    {
        eCodeHandleMotionData = 0,    // 动作数据
        eCodeHandleMarkLine,  // 标记线
        eCodeHandleFrameRunTime,  // 动作编辑配置的动作帧运行时间
        eCodeHandleLoadMusicFile,  // 加载的音乐文件
        eCodeHandleTemplateFile,    // 删除动作模板文件
        eCodeHandleSearchText,  // 搜索文本
    };

    // 标记线类型
    enum emMarkLineType
    {
        eMarkLineTypeUnkonw = 0,
        eMarkLineTypeFrame,  // 标记动作帧
        eMarkLineTypeBlock,  // 标记动作组
    };

    // 标记线状态
    enum emMarkLineState
    {
        eMarkLineStateNone = 0,
        eMarkLineStatePressed,  // 点击状态
        eMarkLineStateSelected, // 选中状态
        eMarkLineStateUnavailable, // 不可用状态，主要用于拖动光标选择框时
    };

}

#endif // UBXUNDOREDOMANAGERCONST

