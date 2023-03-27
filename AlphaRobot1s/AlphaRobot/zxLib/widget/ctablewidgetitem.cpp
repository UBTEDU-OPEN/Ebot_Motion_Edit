#include "ctablewidgetitem.h"


CTableWidgetItem::CTableWidgetItem(const QString &text):
    QTableWidgetItem(text)
{

}

CTableWidgetItem::~CTableWidgetItem()
{

}

void CTableWidgetItem::setTextRetranslate(const QString &strRetranslateContextParentName, const QString &strKey, const QString &strTr)
{
    m_strRetranslateContext = strRetranslateContextParentName;
    m_strTextKeyForRetranslate = strKey;
    this->setText(strTr);
}

void CTableWidgetItem::setTextRetranslate(const QString &strKey, const QString &strTr)
{
    m_strTextKeyForRetranslate = strKey;
    this->setText(strTr);
}

QString CTableWidgetItem::textKeyForRetranslate() const
{
    return m_strTextKeyForRetranslate;
}

bool CTableWidgetItem::isEmptyRetranslateKey() const
{
    return m_strTextKeyForRetranslate.isEmpty();
}

QString CTableWidgetItem::retranslateContext() const
{
    return m_strRetranslateContext;
}

void CTableWidgetItem::setRetranslateContext(const QString &strRetranslateContext)
{
    m_strRetranslateContext = strRetranslateContext;
}
