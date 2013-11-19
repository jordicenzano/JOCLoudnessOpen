#include "CjocMeanFast.h"
#include "..\baseclasses\CjocException.h"

CjocMeanFast::CjocMeanFast(void)
{
	//Overlapped mode
	m_bIni = false;
	m_mode = enMEAN_MODE::MEAN_OVERLAPPED;
	m_lSamplesInInterval = 0;
	m_lSamplesOverlapped = 0;
	m_lGetMeanEveryXSamples = 0;
	m_nSums = 0;
	m_lNumSamplesLoaded = 0;
	m_nSumIndex = 0;

	//Normal mode
	m_lNumAcumulatedSamplesMean = 0;
	m_dAccumulatedMean = 0.0;
}

CjocMeanFast::~CjocMeanFast(void)
{
}

void CjocMeanFast::Ini (enMEAN_MODE mode,long lFs, long lMeanIntervalms, double dOverlappingFactor)
{
	if (m_bIni == true){throw JOC_Error_(enjocErrors::err_bad_state);}

	if ((lFs <= 0)||(lMeanIntervalms <= 0)){throw JOC_Error_(enjocErrors::err_bad_arguments);}
		
	if ((dOverlappingFactor <= 0.0)||(dOverlappingFactor >= 1.0)){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	m_mode = mode;

	if (m_mode == enMEAN_MODE::MEAN_OVERLAPPED)
	{
		m_lSamplesInInterval = (long) ((double)lFs * ((double)lMeanIntervalms / 1000.0));
		m_lSamplesOverlapped = (long) ( (double)m_lSamplesInInterval * dOverlappingFactor);
		m_lGetMeanEveryXSamples = m_lSamplesInInterval - m_lSamplesOverlapped;

		if (m_lGetMeanEveryXSamples <= 0){throw JOC_Error_(enjocErrors::err_bad_arguments);}

		m_nSums = m_lSamplesInInterval / m_lGetMeanEveryXSamples;
		for (int nS = 0;nS < m_nSums  ;nS++)
			m_Sums.push_back (0.0);

		m_lNumSamplesLoaded = 0;
		m_nSumIndex = 0;
	}
	else
	{
		m_lNumAcumulatedSamplesMean = 0;
		m_dAccumulatedMean = 0.0;
	}

	m_bIni = true;
}

bool CjocMeanFast::AddSamples (double *pdSamples, long lBLockSizeTotalSamples,long lDistBetweenSamples)
{
	bool bRc = false;

	//Check
	//if (lBLockSizeTotalSamples % m_nChannels != 0) -> error

	for (long n = 0; n < lBLockSizeTotalSamples; n = n + lDistBetweenSamples)
	{
		if (AddSample(pdSamples[n]) == true)
		{//At least 1 Mean added
			bRc = true;
		}
	}

	return bRc;
}

bool CjocMeanFast::AddSample (double dSample)
{
	bool bRc = false;

	if (m_mode == enMEAN_MODE::MEAN_OVERLAPPED)
	{
		for (int n = 0; n < m_nSums; n++)
			m_Sums[n] = m_Sums[n] + (dSample*dSample);

		m_lNumSamplesLoaded++;

		if ((m_lNumSamplesLoaded % m_lGetMeanEveryXSamples) == 0)
		{
			double dMean = m_Sums[m_nSumIndex]  / (double)m_lSamplesInInterval;
			m_queueMean.push_back (dMean);
			bRc = true;
			m_Sums[m_nSumIndex] = 0.0;

			m_nSumIndex++;
			if (m_nSumIndex >= m_nSums)
				m_nSumIndex = 0;
		}

		if (m_lNumSamplesLoaded >= m_lSamplesInInterval)
			m_lNumSamplesLoaded = 0;
	}
	else
	{
		double dAddVal = dSample*dSample;

		if (m_lNumAcumulatedSamplesMean > 0)
		{
			double dMul = (double)m_lNumAcumulatedSamplesMean/(double)(m_lNumAcumulatedSamplesMean+1);
			m_dAccumulatedMean = (m_dAccumulatedMean * dMul) + dAddVal/((double)(m_lNumAcumulatedSamplesMean+1));
		}
		else
		{
			m_dAccumulatedMean = dAddVal;
		}
		m_lNumAcumulatedSamplesMean++;

		bRc = true;
	}
	return bRc;
}

std::list<double>* CjocMeanFast::GetMeanList ()
{
	return &m_queueMean;
}

bool CjocMeanFast::IsMeanEmpty ()
{
	bool bRc = false;

	if (m_queueMean.size () <= 0)
		bRc = true;

	return bRc;
}

double CjocMeanFast::GetMeanVal ()
{
	double dVal = -std::numeric_limits<double>::quiet_NaN();

	if (m_mode == enMEAN_MODE::MEAN_OVERLAPPED)
	{
		if (m_queueMean.size () > 0)
		{
			dVal = m_queueMean.front ();
			m_queueMean.pop_front ();
		}
	}
	else
	{
		dVal = m_dAccumulatedMean;
	}

	return dVal;
}

void CjocMeanFast::ResetMean ()
{
	m_queueMean.clear ();
	m_Sums.clear ();

	m_lNumSamplesLoaded = 0;
	m_nSumIndex = 0;

	m_lNumAcumulatedSamplesMean = 0;
	m_dAccumulatedMean = 0.0;
}

long CjocMeanFast::GetMeamIntervalSamples ()
{
	return m_lGetMeanEveryXSamples;
}