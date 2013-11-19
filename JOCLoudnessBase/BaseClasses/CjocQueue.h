#pragma once

#include "jocCommonHeader.h"
#include "jocErrorsHeader.h"

class JOC_EXPORTS CjocQueue
{
public:
	CjocQueue(void);
	~CjocQueue(void);

	void Ini (int nSize, double dVal = 0.0);

	void PushFront  (double &dVal); 

	double GetAt(int &nIndex); //0 is the last

private:
	int m_nSize;
	int m_nFront;

	double *m_pData;
};