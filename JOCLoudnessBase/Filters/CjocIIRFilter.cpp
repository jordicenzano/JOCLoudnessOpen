#include "CjocIIRFilter.h"
#include <math.h>
#include "..\baseclasses\CjocException.h"

CjocIIRFilter::CjocIIRFilter(int nOrder)
{
	m_pOldSamples = NULL;
	m_pACoefs= NULL;
	m_pBCoefs= NULL;
	m_nOrder = 0;

	if (nOrder >= 0)
	{
		m_nOrder = nOrder;

		m_pOldSamples = new double[nOrder];
		memset(m_pOldSamples,0,sizeof (double)*nOrder);

		m_pACoefs= new double[nOrder+1];
		memset(m_pACoefs,0,sizeof (double)*(nOrder+1));
		m_pBCoefs= new double[nOrder+1];
		memset(m_pBCoefs,0,sizeof (double)*(nOrder+1));
	}
}

void CjocIIRFilter::SetCoefs(double *pBCoefs, double *pACoefs,int nNumberCoefs)
{
	if (nNumberCoefs != (m_nOrder+1)){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if ((pACoefs==NULL)||(pBCoefs==NULL)){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if ((m_pACoefs == NULL)||(m_pBCoefs == NULL)){throw JOC_Error_(enjocErrors::err_out_of_memory);}

	memcpy (m_pACoefs,pACoefs,sizeof(double)*(m_nOrder+1));
	memcpy (m_pBCoefs,pBCoefs,sizeof(double)*(m_nOrder+1));
}

void CjocIIRFilter::SetCoefs(std::vector <double> &BCoefs, std::vector <double> &ACoefs)
{
	if (BCoefs.size () != ACoefs.size ()){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if (BCoefs.size () != (m_nOrder+1)){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if ((m_pACoefs == NULL)||(m_pBCoefs == NULL)){throw JOC_Error_(enjocErrors::err_out_of_memory);}

	for (int n = 0; n < BCoefs.size(); n++)
		m_pBCoefs[n] = BCoefs[n];

	for (int n = 0; n < ACoefs.size();n++)
		m_pACoefs[n] = ACoefs[n];
}

void CjocIIRFilter::SetCoefs(double da1, double da2,double db0, double db1, double db2)
{
	double mACoefs[3] = {1.0, da1,da2};
	double mBCoefs[3] = {db0, db1, db2};

	SetCoefs (mBCoefs,mACoefs,3);
}

double CjocIIRFilter::FilterSample (double dSampleIn)
{
	int nFilterCoefs = m_nOrder + 1;
	double Sx = dSampleIn;
	double Spx = 0;		
	double Ox = 0;		

	//A coefs 
	for (int nA = 0; nA < nFilterCoefs ;nA++)
	{
		if (nA == 0)
			Spx = Sx; //Spx = Sx * m_pACoefs[0];
		else
			Spx = Spx - m_pACoefs[nA]*m_pOldSamples[nA-1];
	}

	//B coefs 
	for (int nB = 0; nB < nFilterCoefs ;nB++)
	{
		if (nB == 0)
			Ox = Ox + m_pBCoefs[nB]*Spx;
		else
			Ox = Ox + m_pBCoefs[nB]*m_pOldSamples[nB-1];
	}

	//Set Old samples
	for (int nS = m_nOrder; nS > 0; nS--)
	{
		if ((nS - 2) < 0)
			m_pOldSamples[nS-1] = Spx;
		else
			m_pOldSamples[nS-1] = m_pOldSamples[nS-2];
	}

	return Ox;
}

void CjocIIRFilter::Filter (double *pdSamples, long lBLockSizeTotalSamples, long lDistBetweenSamples)
{
	for (long n = 0; n < lBLockSizeTotalSamples; n = n + lDistBetweenSamples)
		pdSamples[n] = FilterSample(pdSamples[n]);
}

void CjocIIRFilter::Filter (unsigned char *pSamplesBlock, long lBLockSizeBytes, int nChannelToFilter, int nNumChannels, int nSampleSizeBytes,CjocWav::enWavType WavType)
{
	if (nChannelToFilter >= nNumChannels){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if (pSamplesBlock == NULL){throw JOC_Error_(enjocErrors::err_bad_arguments);}
	
	if ((m_pACoefs == NULL)||(m_pBCoefs == NULL)){throw JOC_Error_(enjocErrors::err_out_of_memory);}

	if (m_nOrder > 0)
	{
		if (m_pOldSamples == NULL){throw JOC_Error_(enjocErrors::err_out_of_memory);}
	}

	int nAlign =  nNumChannels * nSampleSizeBytes;
	int nOffsetChannelBytes = nSampleSizeBytes * nChannelToFilter;
	
	int nFilterCoefs = m_nOrder + 1;

	long n = 0;
	long nPos = (n * nAlign) + nOffsetChannelBytes;

	while ((nPos + nSampleSizeBytes) <= lBLockSizeBytes)
	{
		double Ix = CjocWav::GetSampleValue (pSamplesBlock, nPos, nSampleSizeBytes,WavType);
		double Ox = FilterSample (Ix);

		int nRc = CjocWav::SetSampleValue (pSamplesBlock, nPos, nSampleSizeBytes, Ox, WavType);
		if (nRc != 0)
			printf ("CjocFilter: Truncated VALUE!!!!\n");

		n++;
		nPos = (n * nAlign) + nOffsetChannelBytes;
	}
}

CjocIIRFilter::~CjocIIRFilter(void)
{
	if (m_pOldSamples != NULL)
	{
		delete[] m_pOldSamples;
		m_pOldSamples = NULL;
	}

	if (m_pACoefs != NULL)
	{
		delete[] m_pACoefs;
		m_pACoefs = NULL;
	}

	if (m_pBCoefs != NULL)
	{
		delete[] m_pBCoefs;
		m_pBCoefs = NULL;
	}
}
