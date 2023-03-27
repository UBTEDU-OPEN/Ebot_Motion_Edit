#ifndef ZXLIBCONST_H
#define ZXLIBCONST_H

namespace zxLibConstConstants
{
    #define INFINITE            0xFFFFFFFF  // Infinite timeout

    // 评论控件运行的最大评论字符数
    const int COMMENT_MAX_CHARACTOR_NUMBER = 200;

    // 颜色值
    const QString UBX_COLOR_GREY = "#d3d3d3";
    const QString UBX_COLOR_WHITE = "#ffffff";
    const QString UBX_COLOR_DARK_RED = "#ca0813";
    const QString UBX_COLOR_DARK_GREY = "#858585";

    // 字体大小
    const int FONT_POINT_SIZE_10 = 14; // 10字体大小

    const int UBT_VID = 0x0483; // ubt的u盘设备Vid
    const int UBT_PID = 0x5720; // UBT的U盘设备Pid

    const QString UBT_DeviceLink = "USBSTOR\\Disk&Ven_UBT&Prod_SD_Flash_Disk";  //UBT的U盘设备链接
}

#endif // ZXLIBCONST

