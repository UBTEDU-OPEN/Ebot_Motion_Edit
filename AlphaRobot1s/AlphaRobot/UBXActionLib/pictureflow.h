/*
  PictureFlow - animated image show widget
  http://pictureflow.googlecode.com

  Copyright (C) 2008 Ariya Hidayat (ariya@kde.org)
  Copyright (C) 2007 Ariya Hidayat (ariya@kde.org)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef PICTUREFLOW_H
#define PICTUREFLOW_H

#include <QWidget>
#include <QMutex>
#include <QReadWriteLock>

// 图片滚动方向
enum emPictureFlowDirection
{
    eDirectionUnknown = 0,  // 未知状态
    eDirectionLeftToRight,  // 从左往右滚动
    eDirectionRightToLeft   // 从右往左滚动
};

class PictureFlowPrivate;

/*!
  Class PictureFlow implements an image show widget with animation effect 
  like Apple's CoverFlow (in iTunes and iPod). Images are arranged in form 
  of slides, one main slide is shown at the center with few slides on 
  the left and right sides of the center slide. When the next or previous 
  slide is brought to the front, the whole slides flow to the right or 
  the right with smooth animation effect; until the new slide is finally 
  placed at the center.

 */ 

//此类为第三方实现类，主要是用于展示CoverFlow效果
class PictureFlow : public QWidget
{
Q_OBJECT

  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
  Q_PROPERTY(QSize slideSize READ slideSize WRITE setSlideSize)
  Q_PROPERTY(int slideCount READ slideCount)
  Q_PROPERTY(int centerIndex READ centerIndex WRITE setCenterIndex)

public:

  enum ReflectionEffect
  {
    NoReflection,
    PlainReflection,
    BlurredReflection
  };

  enum ePictureSwitching
  {
      eNoSwitching = 0,
      eSwitchingPrevious,
      eSwitchingNext
  };

  /*!
    Creates a new PictureFlow widget.
  */  
  PictureFlow(QWidget* parent = 0);

  /*!
    Destroys the widget.
  */
  ~PictureFlow();

  /*!
    Returns the background color.
  */
  QColor backgroundColor() const;

  /*!
    Sets the background color. By default it is black.
  */
  void setBackgroundColor(const QColor& c);

  /*!
    Returns the dimension of each slide (in pixels).
  */  
  QSize slideSize() const;

  /*!
    Sets the dimension of each slide (in pixels).
  */  
  void setSlideSize(QSize size);

  /*!
    Returns the total number of slides.
  */
  int slideCount() const;

  /*!
    Returns QImage of specified slide.
  */  
  QImage slide(int index) const;

  /*!
    Returns the index of slide currently shown in the middle of the viewport.
  */  
  int centerIndex() const;

  /*!
    Returns the effect applied to the reflection.
  */  
  ReflectionEffect reflectionEffect() const;

  /*!
    Sets the effect applied to the reflection. The default is PlainReflection.
  */  
  void setReflectionEffect(ReflectionEffect effect);


public slots:

  /*!
    Adds a new slide.
  */  
  void addSlide(const QImage& image);

  /*!
    Adds a new slide.
  */  
  void addSlide(const QPixmap& pixmap);

  /*!
    Sets an image for specified slide. If the slide already exists,
    it will be replaced.
  */  
  void setSlide(int index, const QImage& image);

  /*!
    Sets a pixmap for specified slide. If the slide already exists,
    it will be replaced.
  */  
  void setSlide(int index, const QPixmap& pixmap);

  /*!
    Sets slide to be shown in the middle of the viewport. No animation 
    effect will be produced, unlike using showSlide.
  */  
  void setCenterIndex(int index);

  /*!
    Clears all slides.
  */
  void clear();

  /*!
    Shows previous slide using animation effect.
  */
  void showPrevious();

  /*!
    Shows next slide using animation effect.
  */
  void showNext();

  /*!
    Go to specified slide using animation effect.
  */
  void showSlide(int index);

  /*!
    Rerender the widget. Normally this function will be automatically invoked
    whenever necessary, e.g. during the transition animation.
  */
  void render();

  /*!
    Schedules a rendering update. Unlike render(), this function does not cause
    immediate rendering.
  */  
  void triggerRender();

  /************************************
  * 名称: getLatestCenterPictureIndex
  * 功能: 获取中间位置最新图片下标
  * 返回: int
  * 时间: 20160929
  * 作者:hwx
  ************************************/
  int getLatestCenterPictureIndex();

  /************************************
  * 名称: getPictureFlowDirection
  * 功能: 获取图片当前切换的方向
  * 返回: ePictureFlowDirection
  * 时间: 20160929
  * 作者:hwx
  ************************************/
  emPictureFlowDirection getPictureFlowDirection() { return m_ePictureFlowDirection; }

signals:
  void centerIndexChanged(int index);

  // add by hwx
  // 当点击中间图片时，发送该信号
  void sigCenterImageClicked();

protected:
  void paintEvent(QPaintEvent *event);
  void keyPressEvent(QKeyEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void resizeEvent(QResizeEvent* event);

private slots:
  void updateAnimation();

private:
  PictureFlowPrivate* d;

  // add by hwx
  // 正在切换图片的标识，此时如果点击中间区域图片，
  // 获取到的是未切换前的图片，明显不合理（此时图片正在切换）
  // 增加该标志以指示获取到正在切换的图片
  // eNoSwitching: 当前图片没有正在切换
  // eSwitchingPrevious:正在切换到前一页
  // eSwitchingNext: 正在切换到下一页
  ePictureSwitching m_nPictureSwitching;

  emPictureFlowDirection m_ePictureFlowDirection; // 图片滚动方向

  QReadWriteLock m_ReadWriteLock;
};

#endif // PICTUREFLOW_H

