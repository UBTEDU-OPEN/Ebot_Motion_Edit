#ifndef _PROCESSHANDLE_H_
#define _PROCESSHANDLE_H_

class CProcessHandle
{
public:
	virtual void OnOutPut(char* pData, int nLen) = 0;
};
#endif  //_PROCESSHANDLE_H_
