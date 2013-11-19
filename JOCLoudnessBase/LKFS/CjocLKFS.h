#pragma once
#include "..\BaseClasses\jocCommonHeader.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>

 class JOC_EXPORTS CjocLKFS
{
public:
	CjocLKFS(void);
	~CjocLKFS(void);

	void Ini (std::vector<double> ChannelGains, short nNumChannels);
	void SetTresholds (bool bThrsEnable, double dAbsLKFS = -70.0, double dRelLU = -10.0);
	void SetLRAPercentiles (double dLRALowPercentile, double dLRAHighPercentile);

	long CalcLKFS (std::vector<std::list<double>*> *pmeanListChannelsPointers, std::vector<double> *pLKFSres);
	double CalcLKFS (std::vector<double> ChannelMean);
	bool CalcLKFSRT (std::vector<std::list<double>*> *pmeanListChannelsPointers, double *pdRetVal, bool bIndB = false);

	double GetIntergratedMessure ();
	void ResetIntergratedMessure ();

	double GetLRAMessure ();

	static double LoudToDB (double d);
	static double DBToLoud (double dDb);
private:
	std::vector<double> m_ChannelGains;
	bool m_bThrsEnable;
	double m_dAbsTrshLKFS;
	double m_dRelTrshLU;

	double m_dLRALowPercentile;
	double m_dLRAHighPercentile;

	unsigned long m_lNumSamplesIntegratedNOGate;
	double m_dIntegratedMeanNOGate;	

	std::vector<double> m_MeansDec;
};

