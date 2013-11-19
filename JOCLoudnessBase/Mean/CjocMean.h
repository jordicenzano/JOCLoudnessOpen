#pragma once
#include "..\BaseClasses\jocCommonHeader.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <iostream>

 using namespace std;


class JOC_EXPORTS CjocMean
{
public:

	enum enMEAN_TYPE{ MEAN_SQ, MEAN};
	enum enMEAN_RESULT_TYPE{ RESULT_DEC, RESULT_10LOG};

	CjocMean(void);
	~CjocMean(void);

	long Ini (long lFs, long lMeanIntervalms, double dOverlappingFactor, enMEAN_TYPE meanType = enMEAN_TYPE::MEAN_SQ);
	void SetResultFormat (enMEAN_RESULT_TYPE resType);

	bool AddSamples (double *pdSamples, long lBLockSizeTotalSamples, long lDistBetweenSamples);

	long GetMeanSize ();
	bool IsMeanEmpty ();

	double GetMeanVal ();

	list<double>* GetMeanList ();

private:
	bool m_bIni;
	unsigned long m_lSamplesInInterval;
	unsigned long m_lSamplesOverlapped;
	unsigned long m_lGetMeanEveryXSamples;
	unsigned long m_lSamplesLoadedFromLastMean;
	unsigned long m_lMeanIntervalms;
	enMEAN_TYPE m_meanType;
	
	enMEAN_RESULT_TYPE m_resType;

   list <double> m_queueSamples;
   list <double> m_queueMean;

   bool AddSample (double dSample);
   
   double CalcMean();
};

