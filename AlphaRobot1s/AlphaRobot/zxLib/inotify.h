/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：INotify.h
* 创建时间：20160912
* 文件标识：
* 文件摘要：notify interface
*
* 当前版本：1.0.0.0
* 作    者：asu
* 完成时间：
* 版本摘要：
*/


#ifndef INOTIFY_H
#define INOTIFY_H


class ZXLIB_DLL INotify
{
public:
    INotify();
    ~INotify();

public:
    /************************************
    * 名称: onQiniuNotify
    * 功能: qiniu upload file info to notify
    * 参数:
    *    @[in ] recvr: which to rec the info
    *    @[in ] nBlkCount:
    *    @[in ] nBlkIdx: current blk index
    *    @[in ] nBlkSize:
    *    @[in ] nBlkOffset: have upload size
    * 返回值:
    * 时间: 20160912
    * 作者: asu
    */
    virtual void onQiniuNotify(INotify* recvr, int nBlkCount, int nBlkIdx, int nBlkSize, int nBlkOffset);


    /************************************
    * 名称: getPercentage
    * 功能: get percentage
    * 参数:
    *    @[in ] nBlkCount:
    *    @[in ] nBlkIdx: current blk index
    *    @[in ] nBlkSize:
    *    @[in ] nBlkOffset: have upload size
    * 返回值:
    * 时间: 20160912
    * 作者: asu
    */
    static int getPercentage(int nBlkCount, int nBlkIdx, int nBlkSize, int nBlkOffset);

};



#endif // INOTIFY_H
