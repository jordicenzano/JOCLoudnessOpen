#pragma once
#include <stdio.h>
#include "cjociirfilter.h"

class JOC_EXPORTS CjocRlbFilter :
	public CjocIIRFilter
{
public:
	CjocRlbFilter(void);
	~CjocRlbFilter(void);

	void SetFreqSampling (unsigned long ulFs);

private:
	unsigned long m_ulFs;

	void CalcCoefs (double *pmBCoefs, double *pmACoefs, unsigned long ulFs, double dFc);
};

