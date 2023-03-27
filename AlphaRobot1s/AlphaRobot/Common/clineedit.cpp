#include "clineedit.h"


#include "cstringhandle.h"

CLineEdit::CLineEdit(QWidget *parent) :
    QLineEdit(parent),
    m_bEnableLimitMetrics(true),
    m_nMaxTextMetric(LINEEDIT_TEXT_MAX_METRIC),
    m_nTextLenth(-1)
{
    connect(this, &CLineEdit::textChanged, this, &CLineEdit::onTextChanged);
}

CLineEdit::~CLineEdit()
{

}

int CLineEdit::getWidth(const QFont &font, const QString &strText)
{
    QFontMetrics fm(font);
    return fm.width(strText);//pixelsWide
}

int CLineEdit::getHeight(const QFont &font)
{
    QFontMetrics fm(font);
    return fm.height();
}

void CLineEdit::setMaxTextMetric(const int &nMaxMetric)
{
    if(nMaxMetric >= 0)
    {
        m_nMaxTextMetric = nMaxMetric;
        setEnableLimitMetrics(true);
    }
}

int CLineEdit::getMaxTextMetric() const
{
    return m_nMaxTextMetric;
}

void CLineEdit::setEnableLimitMetrics(bool bEnableLimit)
{
    m_bEnableLimitMetrics = bEnableLimit;
}

void CLineEdit::setForbidChars(const QString &strForbid)
{
    m_strForbid = strForbid;
}

void CLineEdit::setTextMaxLenth(int nTextMaxLen)
{
    m_nTextLenth = nTextMaxLen;
}

void CLineEdit::onTextChanged(const QString &text)
{
    if(m_bEnableLimitMetrics && (0 <= m_nMaxTextMetric))
    {
        limitTextMertrics(*this, m_nMaxTextMetric, this->font());
    }

    if(!m_strForbid.isEmpty())
    {
        QString textContent = this->text();
        if(!textContent.isEmpty())
        {
            bool bContainForbid = CStringHandle::isContainsChar(textContent, m_strForbid, false);
            if(bContainForbid)
            {
                int nCursorPosition = this->cursorPosition();
                textContent.remove(nCursorPosition - 1, 1);
                this->setText(textContent);
                this->setCursorPosition(nCursorPosition - 1);
            }
        }
    }

    if(m_nTextLenth > 0)
    {
        QString textContent = this->text();
        if(textContent.length() > m_nTextLenth)
        {
            this->setText(textContent.left(m_nTextLenth));
        }
    }
}

void CLineEdit::limitTextMertrics(QLineEdit& lineEdit, const int& nMaxTextMetric, const QFont& font)
{
    QString textContent = lineEdit.text();
    if(textContent.isEmpty())
    {
        return;
    }

    int nCurrentMetric = getWidth(font, textContent);

    if(nCurrentMetric > nMaxTextMetric)
    {
        int nCursorPosition = lineEdit.cursorPosition();
        textContent.remove(nCursorPosition - 1, 1);
        lineEdit.setText(textContent);
        lineEdit.setCursorPosition(nCursorPosition - 1);
    }
}
