#pragma once

#include "..\BaseClasses\jocCommonHeader.h"
#include "..\BaseClasses\CjocQueue.h"
#include <stdio.h>

#define NUM_COEFS_LPF 32

class JOC_EXPORTS CjocFastUpsampler
{
public:
	CjocFastUpsampler(void);
	~CjocFastUpsampler(void);

	void Ini (unsigned long ulFsOriginal, int nUpSampleFactor);
		
	void UpSample(const double *pdSamplesIn, long lBLockSizeTotalSamples, long lDistBetweenSamples, double *pdSamplesOut);

private:
	unsigned long m_ulFsOriginal;
	int m_nUpSampleFactor;
	int m_nCoefs;

	double *m_pACoefs;
	CjocQueue m_pOldSamples;

	void FilterSample (double dSampleIn, double *pdSamplesOut);

	static void CalcCoefsLPF (double *pACoefs, int nNumCoefs, int nUpSampleFactor);
};

