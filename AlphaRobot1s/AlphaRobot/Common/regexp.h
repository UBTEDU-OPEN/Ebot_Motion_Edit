#ifndef CREGEXP_H
#define CREGEXP_H

#include <QRegExp>
#include <QRegExpValidator>


const int PHONE_NUM_MAX_LENGTH = 20;



class CRegExp : public QRegExp
{
public:
    explicit CRegExp(QRegExp parent);
    ~CRegExp();

public:
    static QRegExp getRegExpAnyCharts(const int& nMinLen, const int& nMaxLen);
    static QRegExp getRegExpAnyLetters(const int& nMinLen, const int& nMaxLen);
    static QRegExp getRegExpAnyNumbers(const int& nMinLen, const int& nMaxLen);
    static QRegExp getRegExpPhoneNum();
    static QRegExp getRegExpVersionNumDot();

};

#endif // CREGEXP_H
