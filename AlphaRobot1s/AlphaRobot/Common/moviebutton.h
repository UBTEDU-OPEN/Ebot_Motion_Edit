#ifndef MOVIEBUTTON_H
#define MOVIEBUTTON_H

#include <QPushButton>

class QPainter;
class QMovie;


class MovieButton : public QPushButton
{
    Q_OBJECT

public:
    explicit MovieButton(QWidget *parent = 0);
    ~MovieButton();

public:
    /**************************************************************************
    * 函数名: setMovie
    * 功能: set the gif of button
    * 参数:
    *    @[in ] strMoviePath:   path of the gif
    * 返回值: void
    * 时间: 2016/06/06
    * 作者: suhaiwu
    */
    void setMovie(const QString& strMoviePath);

    /**************************************************************************
    * 函数名: setMovie
    * 功能: set start or stop
    * 参数:
    *    @[in ] bStart:
    * 返回值: void
    * 时间: 2016/06/07
    * 作者: suhaiwu
    */
    void setMovie(bool bStart);

private slots:
    /**************************************************************************
    * 函数名: onIconChged
    * 功能: repain the icon of button
    * 参数:
    *    @[in ]
    * 返回值: void
    * 时间: 2016/06/06
    * 作者: suhaiwu
    */
    void onIconChged(int);

private:
    QMovie *m_pMovie;
    QString m_strMoviePath;
};

#endif // MOVIEBUTTON_H
