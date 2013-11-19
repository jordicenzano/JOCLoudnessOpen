#include "CjocFastUpsampler.h"
#include "..\BaseClasses\CjocUtils.h"

CjocFastUpsampler::CjocFastUpsampler(void)
{
	m_ulFsOriginal = 0;
	m_nUpSampleFactor = 0;
	m_nCoefs = NUM_COEFS_LPF;
	m_pACoefs = NULL;
}


CjocFastUpsampler::~CjocFastUpsampler(void)
{
}

void CjocFastUpsampler::Ini (unsigned long ulFsOriginal, int nUpSampleFactor)
{
	m_ulFsOriginal = ulFsOriginal;
	m_nUpSampleFactor = nUpSampleFactor;

	m_nCoefs = NUM_COEFS_LPF;
	if (CjocUtils::modulus (m_nCoefs,2) == 0)
		m_nCoefs++;

	if (m_pACoefs != NULL)
	{
		delete m_pACoefs;
		m_pACoefs = NULL;
	}
	m_pACoefs = new double[m_nCoefs];

	CalcCoefsLPF (m_pACoefs, m_nCoefs, m_nUpSampleFactor);

	m_pOldSamples.Ini (m_nCoefs,0.0);
}

void CjocFastUpsampler::UpSample(const double *pdSamplesIn, long lBLockSizeTotalSamples, long lDistBetweenSamples, double *pdSamplesOut)
{
	long no = 0;
	for (long ni = 0; ni < lBLockSizeTotalSamples; ni = ni + lDistBetweenSamples)
	{
		FilterSample(pdSamplesIn[ni],&pdSamplesOut[no]);

		no = no + m_nUpSampleFactor;
	}
}

void CjocFastUpsampler::FilterSample (double dSampleIn, double *pdSamplesOut)
{
	//Pag 806 proakis manolakis

	m_pOldSamples.PushFront (dSampleIn);

	for (int nA=0; nA < m_nUpSampleFactor; nA++)	//1 in -> m_lUpSampleFactor out
	{
		int nT = 0;
		pdSamplesOut[nA] = 0;
		for (int nc = nA; nc < m_nCoefs; nc = nc + m_nUpSampleFactor)
		{
			pdSamplesOut[nA] = pdSamplesOut[nA] + (m_pACoefs[nc] * m_pOldSamples.GetAt (nT));
			nT++;			
		}

	}
}

void CjocFastUpsampler::CalcCoefsLPF (double *pACoefs, int nNumCoefs, int nUpSampleFactor)
{	
	double dFcRel = (double)1.0/(2.0*((double)nUpSampleFactor));

	//TODO:???
	dFcRel = dFcRel/2.0;

	long lCoefsLim = (nNumCoefs-1)/2;
	double dnumCoefs = nNumCoefs;

	long lPos = 0;
	for (long ln = -lCoefsLim; ln <= +lCoefsLim; ln++)
	{
		double dn = ln;
		double dhn;
		double dwn = 0.54 + 0.46 * cos (2.0 * PI * dn/(double)dnumCoefs);

		if (ln == 0)
			dhn = 2.0 * dFcRel;
		else
			dhn = sin (2.0 * PI * dFcRel * dn) / (PI * dn);

		//Adapt windows & upsampler factor
		dhn = (dwn * dhn) * (double)nUpSampleFactor;				

		pACoefs[lPos] = dhn;
		lPos++;
	}
}
