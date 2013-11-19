#include "CjocPreFilter.h"
#include <math.h>

CjocPreFilter::CjocPreFilter(void)
{
	m_ulFs = 48000;

	CjocIIRFilter::CjocIIRFilter(2);
}

void CjocPreFilter::SetFreqSampling (unsigned long ulFs)
{
	m_ulFs = ulFs;

	//PRE FILTER - Order 2 = 3coefs
	//Coefs Fs = 48KHz
	double mACoefs[3] = {1.0, -1.69065929318241, 0.73248077421585};
	double mBCoefs[3] = {1.53512485958697, -2.69169618940638, 1.19839281085285};

	if (m_ulFs != 48000)
	{
		//Parameters of second order HF shelvoing filter
		int nGdb = 4;  		// Gain in DB
		double dFc = 1650;	// Center freq

		CalcCoefs (mBCoefs, mACoefs, m_ulFs, nGdb, dFc);
	}

	CjocIIRFilter::SetCoefs (mBCoefs,mACoefs,3);
}

CjocPreFilter::~CjocPreFilter(void)
{
}


void CjocPreFilter::CalcCoefs (double *pmBCoefs, double *pmACoefs, unsigned long ulFs, int nGdb, double dFc)
{
    double dK = tan( (PI * dFc) / (double)ulFs );    
    double dV0 = pow(10.0, (double)nGdb / 20.0);
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
		pmBCoefs[0] = (dV0 + pow (2.0*dV0,0.5)*dK + pow(dK,2.0))/dDivider;
		pmBCoefs[1] = (2.0 * (pow(dK,2.0) - dV0)) / dDivider;
		pmBCoefs[2] = (dV0 - pow (2.0*dV0,0.5)*dK + pow(dK,2.0))/dDivider;
	}
}
