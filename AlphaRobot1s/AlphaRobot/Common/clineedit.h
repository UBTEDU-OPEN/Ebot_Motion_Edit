#ifndef CLINEEDIT_H
#define CLINEEDIT_H

#include <QLineEdit>
#include <QFont>


const int LINEEDIT_TEXT_MAX_METRIC = 300;

class CLineEdit : public QLineEdit
{
public:
    explicit CLineEdit(QWidget *parent = 0);
    ~CLineEdit();

public:
    static int getWidth(const QFont& font, const QString& strText);
    static int getHeight(const QFont& font);


    static void limitTextMertrics(QLineEdit& lineEdit, const int& nMaxTextMetric, const QFont& font);
    void setMaxTextMetric(const int& nMaxMetric);
    int getMaxTextMetric()const;
    void setEnableLimitMetrics(bool bEnableLimit);
    
    
    void setForbidChars(const QString& strForbid);
    void setTextMaxLenth(int nTextMaxLen);
    

public slots:
    void onTextChanged(const QString &text);

private:
    bool m_bEnableLimitMetrics;
    int m_nMaxTextMetric;
    
    int m_nTextLenth;    
    QString m_strForbid;
};

#endif // CLINEEDIT_H
