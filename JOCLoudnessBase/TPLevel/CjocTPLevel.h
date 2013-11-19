#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <iostream>

#include "..\BaseClasses\jocCommonHeader.h"
#include "..\FastUpSampler\CjocFastUpsampler.h"

//#include "..\Filters\CjocUpSampleFilter.h"



class JOC_EXPORTS CjocTPLevel
{
public:
	CjocTPLevel(void);
	~CjocTPLevel(void);

	void Ini (long lFs, long lDecayTimeMS = 1700, double dDecayValueDB = -20.0, long lResfreshIntervalMS = 250, bool bRealTime = false); //Decay factor is the ms to reach half value

	void AddSamples (const double *pdSamples, long lBLockSizeTotalSamples, long lDistBetweenSamples = 1);
	double AddSample (const double *pdSample);

	double GetCurrentTPValue (bool bDB = true);
	double GetMaxTPValue (bool bDB = true);
	void ResetMaxTPValue ();

	long GetRefreshIntervalSamples();

	std::list<double>* GetTPListValues (); //Zero based

	static double TPFSToDb (double dVal);
private:
	bool m_bIni;

	double m_dTPValue;
	double m_dMaxTPValue;

	long m_lFs;
	long m_lDecayTimeMS;
	double m_dDecayValueDB;
	double m_dDecayFactor;
	long m_lResfreshIntervalMS;
	long m_lResfreshIntervalSamples;

	long m_lSampleCounter;

	long m_lUpSamplerFactor;

	CjocFastUpsampler m_FastUpSampler;
	double *m_pUpSampled;
	//CjocUpSampleFilter m_upSampler;
	
	std::list <double> m_queueTPValues;

	void ProcTPSamples ();
};

