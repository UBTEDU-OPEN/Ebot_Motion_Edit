/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：mp3Container.h
* 创建时间：2016/01/12
* 文件标识：
* 文件摘要：MP3窗体界面的容器类，负责MP3波形，滚动条等的创建
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/01/12
* 版本摘要：
*/


#ifndef MP3CONTAINER_H
#define MP3CONTAINER_H

#include <QWidget>
#include <QScrollBar>
#include <QStyleOption>
#include "mp3widget.h"
#include "../zxLib/widget/ubxwidget.h"


namespace Ui {
class mp3Container;
}

class mp3Container : public UBXWidget
{
    Q_OBJECT

public:
    MP3Widget* GetMP3Widget()
    {
        return m_pMp3Widget;
    }

    explicit mp3Container(QWidget *parent = 0);
    ~mp3Container();

public slots:
    //hels 设置滚动条的最大最小范围，以及步进大小
    void OnRangeChanged(int min, int max, int nStep);
    //hels 音乐播放时，同步滚动条的位置
    void OnCellIndexChanged(int index);

protected:
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *)
    {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
    virtual void wheelEvent(QWheelEvent *event);


private:
    Ui::mp3Container *ui;

    MP3Widget* m_pMp3Widget;//mp3波形窗体
    QScrollBar* m_pScrollbar;//滚动条
};

#endif // MP3CONTAINER_H
