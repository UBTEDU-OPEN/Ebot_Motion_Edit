/**************************************************************************
* 功能: showing gif to pushbutton
* 时间: 2016/06/06
* 作者: suhaiwu
*/

#include "moviebutton.h"

#include <QPainter>
#include <QMovie>

#include "Util.h"


MovieButton::MovieButton(QWidget *parent) :
    QPushButton(parent),
    m_pMovie(NULL)
{

}

MovieButton::~MovieButton()
{
    SAFE_DELETE(m_pMovie);
}

/**************************************************************************
* 函数名: setMovie
* 功能: set the gif of button
* 参数:
*    @[in ] strMoviePath:   path of the gif
* 返回值: void
* 时间: 2016/06/06
* 作者: suhaiwu
*/
void MovieButton::setMovie(const QString &strMoviePath)
{
    if((strMoviePath != m_strMoviePath) || (NULL == m_pMovie))
    {
        SAFE_DELETE(m_pMovie);

        m_strMoviePath = strMoviePath;
        m_pMovie = new QMovie(strMoviePath, QByteArray(), this);

        if(m_pMovie)
        {
            connect(m_pMovie, &QMovie::frameChanged, this, &MovieButton::onIconChged);
        }
    }

    setMovie(true);
}

/**************************************************************************
* 函数名: setMovie
* 功能: set start or stop
* 参数:
*    @[in ] bStart:
* 返回值: void
* 时间: 2016/06/07
* 作者: suhaiwu
*/
void MovieButton::setMovie(bool bStart)
{
    if(m_pMovie && bStart)
    {
        if(QMovie::Running != m_pMovie->state())
        {
            m_pMovie->start();
        }
    }

    else if(m_pMovie && !bStart)
    {
        if(QMovie::Running == m_pMovie->state())
        {
            m_pMovie->stop();
        }
    }
}

/**************************************************************************
* 函数名: onIconChged
* 功能: repain the icon of button
* 参数:
*    @[in ]
* 返回值: void
* 时间: 2016/06/06
* 作者: suhaiwu
*/
void MovieButton::onIconChged(int)
{
    QPixmap currFrame = m_pMovie->currentPixmap();
    static const int width = this->width();
    static const int height = this->height();

    QPixmap pixmap(width, height);
    pixmap.fill( Qt::transparent );
    QPainter painter( &pixmap );
    Qt::TransformationMode mode = Qt::SmoothTransformation;
    currFrame = currFrame.scaled(width, height, Qt::IgnoreAspectRatio, mode);
    painter.drawPixmap(0,  0,  currFrame);
    setIcon(QIcon( pixmap));
}
