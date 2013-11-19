#pragma once

#include "..\BaseClasses\jocCommonHeader.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include "..\WAV\CjocWav.h"


class JOC_EXPORTS CjocIIRFilter
{
public:

	CjocIIRFilter(int nOrder = 2);
	~CjocIIRFilter(void);

	//Order 2 filter
	void SetCoefs(double da1, double da2,double db0, double db1, double db2);	

	//Order N filter
	void SetCoefs(double *pBCoefs, double *pACoefs,int nNumberCoefs);
	void SetCoefs(std::vector <double> &BCoefs, std::vector <double> &ACoefs);

	void Filter (unsigned char *pSamplesBlock, long lBLockSizeBytes, int nChannelToFilter, int nNumChannels, int nSampleSizeBytes, CjocWav::enWavType WavType = CjocWav::enWavType::WAV_TYPE_PCM_INT);
	void Filter (double *pdSamples, long lBLockSizeTotalSamples, long lDistBetweenSamples = 1);

private:
	int m_nOrder;
	double *m_pOldSamples;
	
	double *m_pACoefs;
	double *m_pBCoefs;

	double FilterSample (double dSampleIn);
};

