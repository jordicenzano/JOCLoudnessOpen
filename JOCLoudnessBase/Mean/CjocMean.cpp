#include "CjocMean.h"
 #include <limits>

CjocMean::CjocMean(void)
{
	m_bIni = false;
	m_lSamplesInInterval = 0;
	m_lSamplesOverlapped  = 0;
	m_lGetMeanEveryXSamples = 0;
	m_lMeanIntervalms = 0;

	m_lSamplesLoadedFromLastMean = 0;

	m_resType = enMEAN_RESULT_TYPE::RESULT_10LOG;
	m_meanType = enMEAN_TYPE::MEAN_SQ;
}

bool CjocMean::AddSample (double dSample)
{
	bool bRc = false;

	m_queueSamples.push_back (dSample);
	m_lSamplesLoadedFromLastMean++;

	if (m_queueSamples.size () > m_lSamplesInInterval)
		m_queueSamples.pop_front ();

	if (m_lSamplesLoadedFromLastMean >= m_lGetMeanEveryXSamples)
	{
		double dMean = CalcMean();
		m_queueMean.push_back (dMean);
		m_lSamplesLoadedFromLastMean = 0;
		bRc = true;
	}

	return bRc;
}

bool CjocMean::AddSamples (double *pdSamples, long lBLockSizeTotalSamples, long lDistBetweenSamples)
{
	bool bRc = false;

	for (long n = 0; n < lBLockSizeTotalSamples; n = n + lDistBetweenSamples)
	{
		if (AddSample(pdSamples[n]) == true)
		{//At least 1 Mean added
			bRc = true;
		}

	}

	return bRc;
}

double CjocMean::CalcMean()
{
	double dRet = 0.0;

	if (m_queueSamples.size () < m_lSamplesInInterval)
	{
		if (m_resType == enMEAN_RESULT_TYPE::RESULT_10LOG)
			dRet = (10 * log10(dRet));
	}
	else
	{
		list<double>::iterator it;
		for (it = m_queueSamples.begin (); it != m_queueSamples.end(); it++)
		{
			if (m_meanType == enMEAN_TYPE::MEAN_SQ)
				dRet = dRet + (*it)*(*it);
			else
				dRet = dRet + abs(*it);
		}

		dRet = dRet / m_lSamplesInInterval;
	
		if (m_resType == enMEAN_RESULT_TYPE::RESULT_10LOG)
			dRet = (10 * log10(dRet));
	}

	return dRet;
}

long CjocMean::GetMeanSize ()
{
	return m_queueMean.size ();
}

bool CjocMean::IsMeanEmpty ()
{
	bool bRc = false;

	if (m_queueMean.size () <= 0)
		bRc = true;

	return bRc;
}

double CjocMean::GetMeanVal ()
{
	double dVal = -numeric_limits<double>::quiet_NaN();

	if (m_queueMean.size () > 0)
	{
		dVal = m_queueMean.front ();
		m_queueMean.pop_front ();

		if (m_resType == enMEAN_RESULT_TYPE::RESULT_10LOG)
			dVal = (10 * log10(dVal));
	}
	return dVal;
}

list<double>* CjocMean::GetMeanList ()
{
	return &m_queueMean;
}

CjocMean::~CjocMean(void)
{

}

void CjocMean::SetResultFormat (enMEAN_RESULT_TYPE resType)
{
	m_resType = resType;
}

long CjocMean::Ini (long lFs, long lMeanIntervalms, double dOverlappingFactor, enMEAN_TYPE meanType)
{
	if (m_bIni == true)
		return -1;

	if ((lFs <= 0)||(lMeanIntervalms <= 0))
		return -2;

	if ((dOverlappingFactor <= 0.0)||(dOverlappingFactor >= 1.0))
		return -3;
	
	m_meanType = meanType;
	m_lMeanIntervalms = lMeanIntervalms;
	m_lSamplesInInterval = (long) ((double)lFs * ((double)lMeanIntervalms / 1000.0));
	m_lSamplesOverlapped = (long) ( (double)m_lSamplesInInterval * dOverlappingFactor);

	m_lGetMeanEveryXSamples = m_lSamplesInInterval - m_lSamplesOverlapped;
	if (m_lGetMeanEveryXSamples <= 0)
		return -4;

	m_bIni = true;

	return 0;
}