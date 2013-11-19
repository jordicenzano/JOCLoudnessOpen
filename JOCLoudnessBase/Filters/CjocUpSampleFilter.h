#pragma once
#include <stdio.h>
#include "cjocfirfilter.h"

class JOC_EXPORTS CjocUpSampleFilter :
	public CjocFIRFilter
{
public:
	CjocUpSampleFilter(void);
	~CjocUpSampleFilter(void);

	//Calc coefs to adapt pre filter to input FS 
	void SetFreqSampling (unsigned long ulFsOriginal, long lUpSampleFactor);
	void SetFrecCut (unsigned long ulFs, long lFc);
		
private:
	unsigned long m_ulFsOriginal;
	long m_lUpSampleFactor;
	long m_lFc;

	void CalcCoefsUpSample (std::vector <double> *pACoefs, unsigned long ulFsOriginal, long lUpSampleFactor);
	void CalcCoefsFc (std::vector <double> *pACoefs, unsigned long ulFs, long lFc);

	double sinc (double dx);
};

