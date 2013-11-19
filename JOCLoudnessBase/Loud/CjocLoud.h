#pragma once
#include "..\BaseClasses\jocCommonHeader.h"
#include "..\BaseClasses\CjocCrono.h"
#include <stdio.h>
#include "CjocLoudConfig.h"
#include <vector>
#include "..\Filters\CjocPreFilter.h"
#include "..\Filters\CjocRlbFilter.h"
#include "..\Mean\CjocMeanFast.h"
#include "..\LKFS\CjocLKFS.h"
#include "..\TPLevel\CjocTPLevel.h"


class JOC_EXPORTS CjocLoud : 
	public CjocCrono
{
public:
	CjocLoud(void);
	~CjocLoud(void);

	//1
	void SetConfig (CjocLoudConfig &config);

	//2
	void Ini (unsigned long ulSampleRate, short sChannels);
	
	//3
	unsigned long AddSamples (double *pdSamples, unsigned long ulNumSamples, bool *pNewMomentarySample = NULL);
	int AddSample (double *pdSamples, unsigned long ulNumChannels, double *pdM, double *pdTP = NULL, double *pdST = NULL);

	//4
	double GetIntegratedValue(bool bReset = false);
	void ResetIntegratedValue();

	double GetLRAValue(bool bReset = false);
	void ResetLRAValue();

	//5
	std::wstring GetLoudDataStr();
	std::wstring GetIntegrateValStr();
	std::wstring GetLRAValStr();

	void SaveFile (std::wstring &strFile);

	//Get data
	long GetMomentaryUpdateIntervalSamples();
	long GetShortTermUpdateIntervalSamples();
	long GetLRAUpdateIntervalSamples();

private:

	enum enSTATE{ST_NO_INI = 0, ST_CONFIGURED = 1, ST_INI = 2, ST_STARTED = 3, ST_STOPPED = 4};

	enSTATE m_state;
	unsigned long m_ulSampleRate;
	short m_sChannels;

	CjocLoudConfig m_config;

	std::vector<CjocPreFilter*> m_prefilters;
	std::vector<CjocRlbFilter*> m_rlbfilters;

	//TP
	std::vector<CjocTPLevel*> m_TPmeters;
	std::vector<std::list<double>*> m_TPListChannelsValue;

	//Mean
	std::vector<CjocMeanFast*> m_meansqfiltersMomentary;
	std::vector<CjocMeanFast*> m_meansqfiltersShortTerm;
	std::vector<CjocMeanFast*> m_meansqfiltersLRA;
	std::vector<CjocMeanFast*> m_meansqfiltersPlain;

	//LKFS
	CjocLKFS m_calcLKFSM; //Momentary and Integrate
	CjocLKFS m_calcLKFSST; //Short term
	CjocLKFS m_calcLKFSLRA; //LRA
	CjocLKFS m_calcLKFSPlain; //Plain

	std::vector<double> m_channelGains;
	std::vector<std::list<double>*> m_meanListChannelsPointersMomentary;
	std::vector<std::list<double>*> m_meanListChannelsPointersShortTerm;
	std::vector<std::list<double>*> m_meanListChannelsPointersLRA;
	std::vector<std::list<double>*> m_meanListChannelsPointersPlain;

	//Results
	std::vector<double> m_MomentaryLoudnesslkfs;
	std::vector<double> m_ShortTermLoudnesslkfs;
	std::vector<double> m_LRALoudnesslkfs;
	double m_dLRA;
	double m_dPlainLoudnessLKFS;
	double m_dIntegratedLoudnessLKFS;

	long m_lTPUpdateIntervalSamples;

	long m_lMomentaryUpdateIntervalSamples;
	long m_lShortTermUpdateIntervalSamples;
	long m_lLRAUpdateIntervalSamples;

	//Flags
	bool m_bNewMomentarySample;
	bool m_bNewShortTermSample;
	bool m_bNewPlainSample;
	bool m_bNewLRASample;

	//For RT
	double m_dLastST;
};

