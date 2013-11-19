#include "CjocFIRFilter.h"
#include <math.h>
#include "..\baseclasses\CjocException.h"

CjocFIRFilter::CjocFIRFilter(void)
{
	m_nOrder = 0;
}

void CjocFIRFilter::SetCoefs(double *pACoefs,int nNumberCoefs)
{
	if (nNumberCoefs <= 0){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if (pACoefs==NULL){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	m_nOrder = nNumberCoefs;

	for (int n = 0; n < nNumberCoefs;n++)
		m_ACoefs.push_back (pACoefs[n]);		
}

void CjocFIRFilter::SetCoefs(std::vector <double> &ACoefs)
{
	m_nOrder = ACoefs.size ();

	for (int n = 0; n < ACoefs.size();n++)
		m_ACoefs.push_back (ACoefs[n]);
}

double CjocFIRFilter::FilterSample (double dSampleIn)
{
	m_pOldSamples.push_front (dSampleIn);
	if (m_pOldSamples.size () >= m_nOrder)
		m_pOldSamples.pop_back ();

	int n = 0;
	double dOut = 0;
	std::list <double>::iterator it = m_pOldSamples.begin ();

	while ((n < m_ACoefs.size())&&(it != m_pOldSamples.end ()))
	{
		double dSum = m_ACoefs[n] * (*it);
		dOut = dOut + dSum;

		n++;
		it++;
	}

	return dOut;
}

void CjocFIRFilter::Filter (double *pdSamples, long lBLockSizeTotalSamples, long lDistBetweenSamples)
{
	for (long n = 0; n < lBLockSizeTotalSamples; n = n + lDistBetweenSamples)
		pdSamples[n] = FilterSample(pdSamples[n]);
}

CjocFIRFilter::~CjocFIRFilter(void)
{
	m_pOldSamples.clear ();
	m_ACoefs.clear ();	
}
