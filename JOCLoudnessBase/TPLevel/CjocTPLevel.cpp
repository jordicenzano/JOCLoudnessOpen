#include "CjocTPLevel.h"
#include "..\baseclasses\CjocException.h"


CjocTPLevel::CjocTPLevel(void)
{
	m_dTPValue = 0;
	m_dMaxTPValue= 0;

	m_lFs = 0;
	m_lDecayTimeMS = 0;
	m_dDecayValueDB = 0;
	m_dDecayFactor = 0.99999;

	m_lResfreshIntervalMS = 0;
	m_lResfreshIntervalSamples = 0;

	m_lSampleCounter = 0;

	m_lUpSamplerFactor = 4;

	m_pUpSampled = NULL;

	m_bIni = false;
}


CjocTPLevel::~CjocTPLevel(void)
{
	if (m_pUpSampled != NULL)
	{
		delete m_pUpSampled;
		m_pUpSampled = NULL;
	}
}

void CjocTPLevel::Ini (long lFs, long lDecayTimeMS, double dDecayValueDB ,long lResfreshIntervalMS, bool bRealTime)
{
	if ((lFs <= 0)||(lDecayTimeMS <= 0)){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	double lRefreshFreq = 1.0/((double)lResfreshIntervalMS/1000.0);
	if (lRefreshFreq > lFs){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	m_lFs = lFs;
	m_lResfreshIntervalMS = lResfreshIntervalMS;
	m_lResfreshIntervalSamples = (((double)m_lResfreshIntervalMS)/1000.0) * (double)lFs;

	m_lDecayTimeMS = lDecayTimeMS;
	double dAttFactor = pow (10.0,dDecayValueDB/10.0);
	double dDecayTimeS = m_lDecayTimeMS / 1000.0;
	m_dDecayFactor = pow (dAttFactor,1.0/((double)lFs * dDecayTimeS));

	m_lSampleCounter = 0;

	m_lUpSamplerFactor = 4;
	if (lFs >= 96000)
		m_lUpSamplerFactor = 2;

	//m_upSampler.SetFreqSampling (m_lFs,m_lUpSamplerFactorL);
	m_FastUpSampler.Ini (m_lFs,m_lUpSamplerFactor);

	if (m_pUpSampled != NULL)
	{
		delete m_pUpSampled;
		m_pUpSampled = NULL;
	}
	m_pUpSampled = new double[m_lUpSamplerFactor];
	if (m_pUpSampled == NULL){throw JOC_Error_(enjocErrors::err_out_of_memory);}

	m_bIni = true;
}
	
void CjocTPLevel::ProcTPSamples ()
{
	m_dTPValue = m_dTPValue * m_dDecayFactor;

	for (int nU = 0; nU < m_lUpSamplerFactor; nU++)
	{
		if (abs(m_pUpSampled[nU]) > m_dTPValue)
		{
			m_dTPValue = abs(m_pUpSampled[nU]);	
			if (m_dTPValue > m_dMaxTPValue)
				m_dMaxTPValue = m_dTPValue;
		}
	}
}

void CjocTPLevel::AddSamples (const double *pdSamples, long lBLockSizeTotalSamples, long lDistBetweenSamples)
{
	if (m_bIni == false){throw JOC_Error_(enjocErrors::err_bad_state);}

	for (long nS = 0; nS < lBLockSizeTotalSamples; nS = nS + lDistBetweenSamples)
	{	
		m_FastUpSampler.UpSample (&pdSamples[nS],1,1,m_pUpSampled);

		ProcTPSamples ();

		if (m_lSampleCounter >= m_lResfreshIntervalSamples)
		{
			m_queueTPValues.push_back (m_dTPValue);

			m_lSampleCounter = 0;
		}
		m_lSampleCounter++;
	}
}

double CjocTPLevel::AddSample (const double *pdSample)
{
	if (m_bIni == false){throw JOC_Error_(enjocErrors::err_bad_state);}

	m_FastUpSampler.UpSample (pdSample,1,1,m_pUpSampled);

	ProcTPSamples ();

	return m_dTPValue;
}

/*void CjocTPLevel::AddSamples (const double *pdSamples, long lBLockSizeTotalSamples, long lDistBetweenSamples)
{
	if (m_bIni == false){throw JOC_Error_(enjocErrors::err_bad_state);}

	for (long nS = 0; nS < lBLockSizeTotalSamples; nS = nS + lDistBetweenSamples)
	{		
		for (long nUps = 0; nUps < m_lUpSamplerFactorL; nUps++)
		{
			double dSample;
			if (nUps == 0) 
				dSample = abs(pdSamples[nS]);				
			else
				dSample = 0.0;

			m_upSampler.Filter (&dSample,1);

			m_dTPValue = m_dTPValue * m_dDecayFactor;
			
			if (dSample > m_dTPValue)
			{
				m_dTPValue = dSample;	
				if (m_dTPValue > m_dMaxTPValue)
					m_dMaxTPValue = m_dTPValue;
			}
		}
		
		if (m_lSampleCounter >= m_lResfreshIntervalSamples)
		{
			m_queueTPValues.push_back (m_dTPValue);

			m_lSampleCounter = 0;
		}
		m_lSampleCounter++;
	}
}*/

double CjocTPLevel::GetCurrentTPValue (bool bDB)
{
	if (bDB == false)
		return m_dTPValue;

	return TPFSToDb(m_dTPValue);
}

double CjocTPLevel::GetMaxTPValue (bool bDB)
{
	if (bDB == false)
		return m_dMaxTPValue;

	return TPFSToDb(m_dMaxTPValue);
}

void CjocTPLevel::ResetMaxTPValue ()
{
	m_dMaxTPValue= 0;
}

std::list<double>* CjocTPLevel::GetTPListValues ()
{
	return &m_queueTPValues;
}

long CjocTPLevel::GetRefreshIntervalSamples()
{
	return m_lResfreshIntervalSamples;
}

double CjocTPLevel::TPFSToDb (double dVal)
{
	return (20.0*log10(dVal));
}