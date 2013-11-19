#pragma once
#include <stdio.h>
#include "cjocIIRfilter.h"

class JOC_EXPORTS CjocPreFilter :
	public CjocIIRFilter
{
public:
	CjocPreFilter(void);
	~CjocPreFilter(void);

	//Calc coefs to adapt pre filter to input FS 
	void SetFreqSampling (unsigned long ulFs);
		
private:
	unsigned long m_ulFs;

	void CalcCoefs (double *pmBCoefs, double *pmACoefs, unsigned long ulFs, int nGdb, double dFc);
};

