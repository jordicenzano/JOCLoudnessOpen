#include "CjocRlbFilter.h"
#include <math.h>

CjocRlbFilter::CjocRlbFilter(void)
{
	m_ulFs = 48000;

	CjocIIRFilter::CjocIIRFilter(2);
}

void CjocRlbFilter::SetFreqSampling (unsigned long ulFs)
{
	m_ulFs = ulFs;

	//RLB FILTER - Order 2 = 3coefs
	//Coefs Fs = 48KHz
	double mACoefs[3] = {1.0, -1.99004745483398, 0.99007225036621};
	double mBCoefs[3] = {1.0, -2.0, 1.0};

	if (m_ulFs != 48000)
	{
		//Parameters of second order HPF
		double dFc = 60;	// Center freq

		CalcCoefs (mBCoefs, mACoefs, m_ulFs, dFc);
	}

	CjocIIRFilter::SetCoefs (mBCoefs,mACoefs,3);
}

CjocRlbFilter::~CjocRlbFilter(void)
{
}


void CjocRlbFilter::CalcCoefs (double *pmBCoefs, double *pmACoefs, unsigned long ulFs, double dFc)
{
    double dK = tan( (PI * dFc) / (double)ulFs );    
	double dsqrt2 = pow (2,0.5);
	double dDivider = (1.0 + dsqrt2 * dK + pow(dK,2));

	if (pmACoefs != NULL)
	{
		pmACoefs[0] = 1.0;
		pmACoefs[1] = (2.0 * (pow (dK,2.0) - 1)) / dDivider;
		pmACoefs[2] = (1.0 - (dsqrt2 *dK) + pow(dK,2.0))/dDivider;
	}

	if (pmBCoefs != NULL)
	{
		pmBCoefs[0] = 1.0/dDivider;
		pmBCoefs[1] = -2.0 / dDivider;
		pmBCoefs[2] = 1.0/dDivider;
	}
}
