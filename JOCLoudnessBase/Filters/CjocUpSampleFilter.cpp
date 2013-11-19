#include "CjocUpSampleFilter.h"
#include "..\BaseClasses\CjocUtils.h"
#include <math.h>

CjocUpSampleFilter::CjocUpSampleFilter(void)
{
	m_ulFsOriginal = 48000;
	m_lUpSampleFactor = 4;
	m_lFc = 10000;

	CjocFIRFilter::CjocFIRFilter();
}

void CjocUpSampleFilter::SetFreqSampling (unsigned long ulFs, long lUpSampleFactor)
{
	m_ulFsOriginal = ulFs;
	m_lUpSampleFactor = lUpSampleFactor;

	std::vector <double> ACoefs;

	CalcCoefsUpSample (&ACoefs, m_ulFsOriginal, m_lUpSampleFactor);

	CjocFIRFilter::SetCoefs (ACoefs);
}

void CjocUpSampleFilter::SetFrecCut (unsigned long ulFs, long lFc)
{
	m_ulFsOriginal = ulFs;
	m_lUpSampleFactor = 0;
	m_lFc = lFc;

	std::vector <double> ACoefs;

	CalcCoefsFc (&ACoefs, m_ulFsOriginal, m_lFc);

	CjocFIRFilter::SetCoefs (ACoefs);

}

CjocUpSampleFilter::~CjocUpSampleFilter(void)
{
}


void CjocUpSampleFilter::CalcCoefsUpSample (std::vector <double> *pACoefs, unsigned long ulFsOriginal, long lUpSampleFactor)
{
	 //int nN = 0.75 / (5000.0/((double)ulFsOriginal*(double)lUpSampleFactor));
	 int nN = 11;//0.75 / (5000.0/((double)ulFsOriginal*(double)lUpSampleFactor));

	 if (CjocUtils::modulus (nN,2) == 0)
		 nN++;

	 double dnN = nN;
	 
	 long lFc = (ulFsOriginal/2);
	 long lNewFs = ulFsOriginal * lUpSampleFactor;
	 double dFcRel = (double)lFc/(double)lNewFs;

	 int nNLimit = nN/2;
	 for (int n = -nNLimit; n <= nNLimit; n++)
	 {
		 double dn = n;
		 double dwn = 0.54 + 0.46*cos (2.0*PI*dn/dnN);
		 double dx = dn * 2.0 * PI * dFcRel;
		 double dhdn = sinc (dx);
		 
		 pACoefs->push_back (dwn*dhdn);
	 }
}

void CjocUpSampleFilter::CalcCoefsFc (std::vector <double> *pACoefs, unsigned long ulFs, long lFc)
{
	 int nN = 5.0 / (5000.0/(double)ulFs);

	 if (CjocUtils::modulus (nN,2) == 0)
		 nN++;

	 double dnN = nN;

	 double dFcRel = (double)lFc/(double)ulFs;
	 
	 int nNLimit = nN/2;
	 for (int n = -nNLimit; n <= nNLimit; n++)
	 {
		 double dn = n;
		 double dwn = 0.54 + 0.46*cos (2.0*PI*dn/dnN);
		 double dx = dn * 2.0 * PI * dFcRel;
		 double dhdn = sinc (dx);

		 pACoefs->push_back (dwn*dhdn);
	 }
}

double CjocUpSampleFilter::sinc (double dx)
{
	double dRet = 1.0;
	if (dx != 0)
		dRet = sin(dx)/dx;

	return dRet;
}
