#pragma once
#include "..\BaseClasses\jocCommonHeader.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <iostream>

class JOC_EXPORTS CjocMeanFast
{
public:
	enum enMEAN_MODE{ MEAN_OVERLAPPED, MEAN_TOTAL };

	CjocMeanFast(void);
	~CjocMeanFast(void);

	void Ini (enMEAN_MODE mode, long lFs = 48000, long lMeanIntervalms = 400, double dOverlappingFactor = 0.75);
	bool AddSamples (double *pdSamples, long lBLockSizeTotalSamples,long lDistBetweenSamples);

	std::list<double>* GetMeanList (); //Zero based
	
	bool IsMeanEmpty ();
	double GetMeanVal ();
	void ResetMean ();

	long GetMeamIntervalSamples ();

private:
	//Used in overlapped mode
	bool m_bIni;
	enMEAN_MODE m_mode;
	std::list <double> m_queueMean;
	
	//Store sums
	std::vector <double> m_Sums;

	int m_nSums;
	unsigned long m_lSamplesInInterval;
	unsigned long m_lSamplesOverlapped;
	unsigned long m_lGetMeanEveryXSamples;	

	unsigned long m_lNumSamplesLoaded;
	int m_nSumIndex;

	//Used in normal mode
	unsigned long m_lNumAcumulatedSamplesMean;
	double m_dAccumulatedMean;

	bool AddSample (double dSample);

};