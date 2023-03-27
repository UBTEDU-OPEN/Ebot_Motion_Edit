#ifndef UIDIALOGBLOCKDESCRIPTION_H
#define UIDIALOGBLOCKDESCRIPTION_H

#include <QDialog>
#include <QLineEdit>
#include <QString>
#include <QStyleOption>
#include <QPainter>


namespace Ui {
class DialogBlockDescription;
}

class DialogBlockDescription : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBlockDescription(QWidget *parent = 0, QString strDescripionInfo = tr(""));
    ~DialogBlockDescription();

private:
    Ui::DialogBlockDescription *ui;

public:
    QString m_strBlockDescriptionInfo;
    float m_fStartTime;
    float m_fEndTime;
    void SetStartTime(float fStartTime);
    void SetEndTime(float fEndTime);
    void SetDescription(QString strDescription);
private slots:
    void on_btnOK_clicked();
    void on_btnCancel_clicked();
protected:
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *)
    {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
};

#endif // UIDIALOGBLOCKDESCRIPTION_H
