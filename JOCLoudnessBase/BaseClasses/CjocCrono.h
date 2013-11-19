#pragma once

#include "jocCommonHeader.h"
#include <time.h>
#include <map>

class JOC_EXPORTS CjocCrono
{
public:
	typedef std::map <std::wstring ,long> mapStrTime;

	//Time values
	enum enCronoValues
	{ 
		CRONO_WAV_LOAD = 1,
		CRONO_WAV_SAVE = 2,
		CRONO_FILTER_PRE = 3,
		CRONO_FILTER_RLB = 4,
		CRONO_MEAN_FAST_MOMENTARY = 5,
		CRONO_MEAN_FAST_SHORTTERM = 6,
		CRONO_MEAN_FAST_PLAIN = 7,
		CRONO_LKFS_MOMENTARY = 8,
		CRONO_LKFS_SHORTTERM = 9,
		CRONO_SAVE_LOUD_DATA = 10,
		CRONO_TP_METTER = 11,
		CRONO_TEST_LPF = 12,
		CRONO_TEST_UPSAMPLE_INSERT = 13,
		CRONO_TEST_UPSAMPLE_LPF = 14,
		CRONO_LKFS_LRA = 15,
		CRONO_MEAN_FAST_LRA = 16
	};


	CjocCrono(void);
	~CjocCrono(void);

	void CronoActivate (bool bActivate = true);

	void CronoStart();
	long CronoEnd();
	long CronoEnd(std::wstring strName);
	long CronoEnd(int nId);

	void CronoReset ();

	long CronoGetGapTime (std::wstring strName);
	long CronoGetGapTime (int nId);
	long CronoGetTotalTime ();

	//First entry is total time
	static std::wstring GetStrTimes (mapStrTime &CronoStrTimes);
private:
	bool m_bActivate;

	long m_lStart;
		
	typedef std::map <int ,long> mapIntTime;
	
	mapStrTime m_AccStrTimes;
	mapIntTime m_AccnTimes;

	//bool ExsitsKeyInMap (int nId);
};

