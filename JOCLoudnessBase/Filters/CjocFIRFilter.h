#pragma once

#include "..\BaseClasses\jocCommonHeader.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <list>

class JOC_EXPORTS CjocFIRFilter
{
public:

	CjocFIRFilter(void);
	~CjocFIRFilter(void);

	//Order N filter
	void SetCoefs(double *pACoefs,int nNumberCoefs);
	void SetCoefs(std::vector <double> &ACoefs);

	void Filter (double *pdSamples, long lBLockSizeTotalSamples, long lDistBetweenSamples = 1);
	
private:
	int m_nOrder;
	std::vector <double> m_ACoefs;

	std::list <double> m_pOldSamples;

	double FilterSample (double dSampleIn);
};

