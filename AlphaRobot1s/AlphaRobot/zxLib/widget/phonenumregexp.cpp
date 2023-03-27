#include "phonenumregexp.h"

#include "../common/regexp.h"




CPhoneNumRegExp::CPhoneNumRegExp(QWidget *parent):
    QLineEdit(parent)
{
    this->setValidator(new QRegExpValidator(CRegExp::getRegExpPhoneNum(), this));
}

CPhoneNumRegExp::~CPhoneNumRegExp()
{

}
