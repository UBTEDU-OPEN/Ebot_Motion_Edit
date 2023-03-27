#ifndef CTEXTEDIT_H
#define CTEXTEDIT_H


#include <QTextEdit>


const int TEXT_EDIT_CHARTS_DEFAULT_MAX_LENGTH = 150;


class CTextEdit : public QTextEdit
{
public:
    explicit CTextEdit(QWidget *parent);
    ~CTextEdit();

public:
    void setChartsMaxLength(const int& nMaxLen);
    int getChartsMaxLength()const;


protected:
    virtual void keyPressEvent(QKeyEvent *event);

protected slots:
    void onTextChanged();

private:
    void limitChartLength(const int& nMaxLen);

private:
    int m_nMaxLength;

};

#endif // CTEXTEDIT_H
