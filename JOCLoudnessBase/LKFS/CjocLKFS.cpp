#include "CjocLKFS.h"
#include "..\baseclasses\CjocException.h"
#include <algorithm>

CjocLKFS::CjocLKFS(void)
{	
	m_ChannelGains.push_back (1.0); //L
	m_ChannelGains.push_back (1.0); //R
	m_ChannelGains.push_back (1.0); //C
	m_ChannelGains.push_back (1.41); //Ls
	m_ChannelGains.push_back (1.41); //Rs

	m_dIntegratedMeanNOGate = 0;
	m_lNumSamplesIntegratedNOGate = 0;
	
	m_bThrsEnable = false;
	m_dAbsTrshLKFS = -70.0;
	m_dRelTrshLU = -10.0;

	m_dLRALowPercentile = 10.0;
	m_dLRAHighPercentile = 95.0;

}

void CjocLKFS::Ini (std::vector<double> ChannelGains,short nNumChannels)
{
	if (nNumChannels > ChannelGains.size ()){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	m_ChannelGains.clear ();
	for (int n = 0 ; n < nNumChannels;n++)
		m_ChannelGains.push_back (ChannelGains[n]);
}

void CjocLKFS::SetTresholds (bool bThrsEnable,double dAbsLKFS, double dRelLU)
{
	m_bThrsEnable = bThrsEnable;
	m_dAbsTrshLKFS = dAbsLKFS;
	m_dRelTrshLU = dRelLU;
}

void CjocLKFS::SetLRAPercentiles (double dLRALowPercentile, double dLRAHighPercentile)
{
	m_dLRALowPercentile = dLRALowPercentile;
	m_dLRAHighPercentile = dLRAHighPercentile;
}

long CjocLKFS::CalcLKFS (std::vector<std::list<double>*> *pmeanListChannelsPointers, std::vector<double> *pLKFSres)
{
	unsigned long lNumCalcMeans = 0;
	std::vector<double> meanVals;

	if (pmeanListChannelsPointers == NULL){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if (pmeanListChannelsPointers->size () != m_ChannelGains.size ()){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	short nNumChannels = pmeanListChannelsPointers->size ();
	
	bool bEmpty = false;
	while (bEmpty  == false)
	{
		for (int nC = 0; nC <nNumChannels ;nC++)
		{	
			std::list<double> *pList = (std::list<double>*) (*pmeanListChannelsPointers)[nC];
			if (pList->empty ()==true)
				bEmpty = true;
		}

		if (bEmpty ==false)
		{
			meanVals.clear ();
			for (int nC = 0; nC <nNumChannels ;nC++)
			{
				double dVal = (*pmeanListChannelsPointers)[nC]->front ();
				(*pmeanListChannelsPointers)[nC]->pop_front ();
				
				meanVals.push_back (dVal);				
			}

			if (pLKFSres != NULL)
				pLKFSres->push_back (CalcLKFS(meanVals));

			lNumCalcMeans++;
		}
	}

	return lNumCalcMeans;
}

bool CjocLKFS::CalcLKFSRT (std::vector<std::list<double>*> *pmeanListChannelsPointers, double *pdRetVal, bool bIndB)
{
	bool bRet = false;
	double dRetValdec = 0;
	double dMean = 0;

	if (pmeanListChannelsPointers->size () > m_ChannelGains.size ())
		return bRet;

	if ((*pmeanListChannelsPointers)[0]->empty () == false)
	{
		for (int nC = 0 ; nC < (int)m_ChannelGains.size () ; nC++)
		{
			dMean = (*pmeanListChannelsPointers)[nC]->front ();
			(*pmeanListChannelsPointers)[nC]->pop_front ();

			dRetValdec  = dRetValdec + dMean * m_ChannelGains[nC];
		}
	
		if (pdRetVal != NULL)
		{
			*pdRetVal = dRetValdec;
			if (bIndB == true)
				*pdRetVal = (10.0 * log10(*pdRetVal) - 0.691);

			bRet = true;
		}
	}

	return bRet;
}

double CjocLKFS::CalcLKFS (std::vector<double> ChannelMean)
{
	if (ChannelMean.size () > m_ChannelGains.size ())
		return -std::numeric_limits<double>::quiet_NaN();

	double dValdec = 0;
	for (int nC = 0 ; nC < (int)m_ChannelGains.size () ; nC++)
		dValdec = dValdec + ChannelMean[nC] * m_ChannelGains[nC];

	//Used for no gated value
	if (m_lNumSamplesIntegratedNOGate > 0)
	{
		double dMul = (double)m_lNumSamplesIntegratedNOGate/(double)(m_lNumSamplesIntegratedNOGate+1);
		m_dIntegratedMeanNOGate = (m_dIntegratedMeanNOGate * dMul) + dValdec/((double)(m_lNumSamplesIntegratedNOGate+1));
	}
	else
	{
		m_dIntegratedMeanNOGate = dValdec;
	}
	m_lNumSamplesIntegratedNOGate++;

	//Used for gated value
	m_MeansDec.push_back (dValdec);
	
	return (10.0 * log10(dValdec) - 0.691);
}

double CjocLKFS::GetLRAMessure ()
{
	double dRet = 0;
	std::vector<double> lra;

	//TODO:Check double dAbsTrsh = pow (10.0, (m_dAbsTrshLKFS/10.0));
	double dAbsTrsh = DBToLoud(m_dAbsTrshLKFS);

	//Calc relavive gate using abs gate
	double dSum = 0;
	unsigned long lNumMeans = 0;
	for (unsigned long ln = 0; ln < m_MeansDec.size (); ln++)
	{
		if (m_MeansDec[ln] > dAbsTrsh)
		{
			dSum = dSum + m_MeansDec[ln];
			lNumMeans++;
		}
	}

	//Relative gate
	dSum = dSum/(double)lNumMeans;
	//TODO:Check double dRelGateLKFS = 10.0 * log10(dSum) - 0.691 + m_dRelTrshLU ;
	double dRelGateLKFS = LoudToDB (dSum) + m_dRelTrshLU ;

	//TODO:Check double dRelGate = pow (10.0, (dRelGateLKFS/10.0));
	double dRelGate = DBToLoud (dRelGateLKFS);

	//Calc final LKFS value
	lNumMeans = 0;
	dSum = 0;
	for (unsigned long ln = 0; ln < m_MeansDec.size (); ln++)
	{
		if ((m_MeansDec[ln] > dAbsTrsh)&&(m_MeansDec[ln] > dRelGate))
		{
			dSum = dSum + m_MeansDec[ln];
			lNumMeans++;

			lra.push_back (m_MeansDec[ln]);
		}
	}

	//Order vector
	std::sort(lra.begin() ,lra.end());

	//Get high - low
	unsigned long ulSize = lra.size (); 
	unsigned long ulIndexLow = ((double)ulSize * (m_dLRALowPercentile / 100.0) + 0.5);
	unsigned long ulIndexHigh = ((double)ulSize * (m_dLRAHighPercentile / 100.0) + 0.5);

	double dLow = 0.0;
	if ((ulIndexLow >= 0)&&(ulIndexLow <ulSize))
		dLow = lra[ulIndexLow];

	double dHigh = 0.0;
	if ((ulIndexHigh >= 0)&&(ulIndexHigh < ulSize))
		dHigh = lra[ulIndexHigh];
	
	//TODO:Check double dLowLKFS =  10.0 * log10(dLow) - 0.691;
	//TODO:Check double dHighLKFS = 10.0 * log10(dHigh) - 0.691;;
	double dLowLKFS = LoudToDB (dLow);
	double dHighLKFS = LoudToDB (dHigh);

	return (dHighLKFS - dLowLKFS);
}

double CjocLKFS::GetIntergratedMessure ()
{
	//TODO:Check double dRet = 10.0 * log10(m_dIntegratedMeanNOGate) - 0.691;
	double dRet = LoudToDB (m_dIntegratedMeanNOGate);

	if (m_bThrsEnable == true)
	{//Gated value 
		//double dAbsTrsh = pow (10.0, (m_dAbsTrshLKFS/10.0));
		//TODO check
		double dAbsTrsh = DBToLoud (m_dAbsTrshLKFS);

		//Calc relavive gate using abs gate
		double dSum = 0;
		unsigned long lNumMeans = 0;
		for (unsigned long ln = 0; ln < m_MeansDec.size (); ln++)
		{
			if (m_MeansDec[ln] > dAbsTrsh)
			{
				dSum = dSum + m_MeansDec[ln];
				lNumMeans++;
			}
		}

		//Relative gate
		dSum = dSum/(double)lNumMeans;

		//TODO:Check double dRelGateLKFS = 10.0 * log10(dSum) - 0.691 + m_dRelTrshLU ;
		double dRelGateLKFS = LoudToDB (dSum) + m_dRelTrshLU;
		
		//TODO:Check double dRelGate = pow (10.0, (dRelGateLKFS/10.0));
		double dRelGate = DBToLoud (dRelGateLKFS);

		//Calc final LKFS value
		lNumMeans = 0;
		dSum = 0;
		for (unsigned long ln = 0; ln < m_MeansDec.size (); ln++)
		{
			//double dVal = 10.0 * log10(m_MeansDec[ln]) - 0.691;
			if ((m_MeansDec[ln] > dAbsTrsh)&&(m_MeansDec[ln] > dRelGate))
			{
				dSum = dSum + m_MeansDec[ln];
				lNumMeans++;
			}
		}

		dRet = dSum/(double)lNumMeans;		
		//TODO:Check dRet = 10.0 * log10(dRet) - 0.691;
		dRet = LoudToDB (dRet);
	}

	return dRet;
}

void CjocLKFS::ResetIntergratedMessure ()
{
	m_dIntegratedMeanNOGate = 0;
	m_lNumSamplesIntegratedNOGate = 0;
	
	m_MeansDec.clear ();
}

CjocLKFS::~CjocLKFS(void)
{
}

double CjocLKFS::LoudToDB (double d)
{
	return 10.0 * log10(d) - 0.691;
}

double CjocLKFS::DBToLoud (double dDb)
{
	return pow(10.0, ((dDb / 10.0) + 0.0691));
}