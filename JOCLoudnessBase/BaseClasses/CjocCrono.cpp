#include "CjocCrono.h"
#include "CjocUtils.h"

CjocCrono::CjocCrono(void)
{
	m_bActivate  = false;
	m_lStart = 0;
}


CjocCrono::~CjocCrono(void)
{
	
}

void CjocCrono::CronoActivate (bool bActivate)
{
	m_bActivate = bActivate;
}

void CjocCrono::CronoStart()
{
	if (m_bActivate ==true)
		m_lStart = clock();
}

long CjocCrono::CronoEnd()
{
	if (m_bActivate ==true)
		return (clock() -  m_lStart);
	else
		return 0;
}

void CjocCrono::CronoReset ()
{
	if (m_bActivate ==true)
	{
		m_lStart = 0;
		m_AccStrTimes.clear ();
		m_AccnTimes.clear ();
	}
}

long CjocCrono::CronoEnd(std::wstring strName)
{
	long lGapTime = 0;

	if (m_bActivate ==true)
	{
		mapStrTime::iterator iter;

		lGapTime = CjocCrono::CronoEnd ();

		iter = m_AccStrTimes.find (strName);
		if (iter != m_AccStrTimes.end ())
		{
			iter->second = iter->second + lGapTime;
		}
		else
		{
			m_AccStrTimes.insert (std::pair <std::wstring ,long>(strName,lGapTime));
		}
	}

	return lGapTime;
}

long CjocCrono::CronoEnd(int nId)
{
	long lGapTime = 0;

	if (m_bActivate ==true)
	{
		mapIntTime::iterator iter;

		lGapTime = CjocCrono::CronoEnd ();

		iter = m_AccnTimes.find (nId);
		if (iter != m_AccnTimes.end ())
		{
			iter->second = iter->second + lGapTime;
		}
		else
		{
			m_AccnTimes.insert (std::pair <int ,long>(nId,lGapTime));
		}
	}

	return lGapTime;
}

long CjocCrono::CronoGetGapTime (std::wstring strName)
{
	long lRet = 0;

	mapStrTime::iterator iter;
	iter = m_AccStrTimes.find (strName);
	if (iter != m_AccStrTimes.end ())
	{
		lRet = iter->second;
	}

	return lRet;
}

long CjocCrono::CronoGetGapTime (int nId)
{
	long lRet = 0;

	mapIntTime::iterator iter;
	iter = m_AccnTimes.find (nId);
	if (iter != m_AccnTimes.end ())
		lRet = iter->second;

	return lRet;
}

long CjocCrono::CronoGetTotalTime ()
{
	long lRet = 0;

	mapIntTime::iterator iter;
	iter = m_AccnTimes.begin ();
	while (iter != m_AccnTimes.end ())
	{
		lRet = lRet + iter->second;

		iter++;
	}

	return lRet;
}

std::wstring CjocCrono::GetStrTimes (mapStrTime &CronoStrTimes)
{
	std::wstring strRet;

	mapStrTime::iterator iter;

	iter = CronoStrTimes.begin ();
	int n = 0;
	long lTotal = 0;
	while (iter != CronoStrTimes.end ())
	{
		std::wstring strName = iter->first;
		long lTime = iter->second;
		double dPercent = 0;
		if (n == 0)
		{
			lTotal = lTime;
			dPercent  = 100;
		}
		else 
		{
			if (lTotal > 0)
				dPercent  = (((double)lTime / (double)lTotal) * 100.0);
			else
				dPercent = 100;
		}

		strRet = strRet + strName + L": " + CjocUtils::lToStr (lTime) + L" (" + CjocUtils::dToStr (dPercent,2) + L"%%)\n";

		iter++;
		n++;
	}

	return strRet;
}