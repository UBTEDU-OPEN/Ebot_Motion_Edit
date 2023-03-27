#include "QCheckboxHeaderView.h"

QCheckboxHeaderView::QCheckboxHeaderView(Qt::Orientation orientation, QWidget* parent /*= 0*/)
    : QHeaderView(orientation, parent)
{
    isChecked_ = true;
}

void QCheckboxHeaderView::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    if (logicalIndex == 0)
    {
        QStyleOptionButton option;

        option.rect = QRect(1, 10, 5, 5);
        option.state = QStyle::State_Enabled | QStyle::State_Active;

        if (isChecked_)
            option.state |= QStyle::State_On;
        else
            option.state |= QStyle::State_Off;
        //option.state |= QStyle::State_Off;

        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);
    }
}

void QCheckboxHeaderView::mousePressEvent(QMouseEvent* event)
{
    QPoint pos = event->pos();
    QRect rcCell;
    for(int i = 0; i < this->count(); i++)
    {

    }

    setIsChecked(!isChecked());

    emit checkBoxClicked(isChecked());
}

void QCheckboxHeaderView::redrawCheckBox()
{
    viewport()->update();
}

void QCheckboxHeaderView::setIsChecked(bool val)
{
    if (isChecked_ != val)
    {
        isChecked_ = val;

        redrawCheckBox();
    }
}
