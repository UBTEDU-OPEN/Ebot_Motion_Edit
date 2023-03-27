#include "textedit.h"


CTextEdit::CTextEdit(QWidget *parent) :
    QTextEdit(parent),
    m_nMaxLength(TEXT_EDIT_CHARTS_DEFAULT_MAX_LENGTH)
{
    connect(this, &CTextEdit::textChanged, this, &CTextEdit::onTextChanged);
}

CTextEdit::~CTextEdit()
{

}

void CTextEdit::setChartsMaxLength(const int &nMaxLen)
{
    m_nMaxLength = nMaxLen;
}

int CTextEdit::getChartsMaxLength() const
{
    return m_nMaxLength;
}

void CTextEdit::keyPressEvent(QKeyEvent *event)
{
    QTextEdit::keyPressEvent(event);
}

void CTextEdit::onTextChanged()
{
    limitChartLength(m_nMaxLength);
}

void CTextEdit::limitChartLength(const int& nMaxLen)
{
    QString textContent = this->toPlainText();
    int length = textContent.count();
    int maxLength = nMaxLen; // 最大字符数
    if(length > maxLength)
    {
        int position = this->textCursor().position();
        QTextCursor textCursor = this->textCursor();
        textContent.remove(position - (length - maxLength), length - maxLength);
        this->setText(textContent);
        textCursor.setPosition(position - (length - maxLength));
        this->setTextCursor(textCursor);
    }
}
