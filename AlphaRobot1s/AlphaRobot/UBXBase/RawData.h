#ifndef _RAWDATA_H_
#define _RAWDATA_H_

class CRawData
{
public:
	virtual bool GetRawData(char** ppData, int &nLen) = 0;
	virtual bool SetRawData(char* pData, int nLen) = 0;
};
#endif  //_RAWDATA_H_
