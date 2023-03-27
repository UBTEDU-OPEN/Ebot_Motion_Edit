#ifndef QCHECKBOXHEADERVIEW
#define QCHECKBOXHEADERVIEW

#include <QtGui>
#include <QHeaderView>
#include <QStyleOptionButton>
#include <QStyle>

class QCheckboxHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    QCheckboxHeaderView(Qt::Orientation orientation, QWidget* parent = 0);

    bool isChecked() const { return isChecked_; }
    void setIsChecked(bool val);

signals:
    void checkBoxClicked(bool state);

protected:
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const;

    void mousePressEvent(QMouseEvent* event);

private:
    bool isChecked_;

    void redrawCheckBox();
};

#endif
