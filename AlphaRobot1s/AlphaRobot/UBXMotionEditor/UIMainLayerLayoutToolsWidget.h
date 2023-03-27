#ifndef CUIMAINLAYERLAYOUTTOOLSWIDGET_H
#define CUIMAINLAYERLAYOUTTOOLSWIDGET_H

#include <QWidget>


class ILayoutTools_Event
{
public:
    virtual void OnSrcollChange(int nPos) = 0;
    virtual void EnablePlay(int nLayerType, bool bState) = 0;
};

namespace Ui {
class CUIMainLayerLayoutToolsWidget;
}

class CUIMainLayerLayoutToolsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CUIMainLayerLayoutToolsWidget(QWidget *parent = 0);
    ~CUIMainLayerLayoutToolsWidget();

private:
    Ui::CUIMainLayerLayoutToolsWidget *ui;
};

#endif // CUIMAINLAYERLAYOUTTOOLSWIDGET_H
