#pragma once
#include "..\BaseClasses\jocCommonHeader.h"
#include <stdio.h>
#include <vector>

class JOC_EXPORTS CjocLoudConfig
{
public:

	static const int MAX_AUDIO_CHANNELS = 16;

	static double GetChannelGainFromDef (std::wstring &strChannel);

	enum enPRESETMODE{ PRESET_MODE_R128_EU, PRESET_MODE_E85_USA, PRESET_MODE_CUSTOM};
	enum enINTERGRATEMODE{ INTEGRATED_MODE_BS17702_EU, INTEGRATED_MODE_BS1770_USA};

	CjocLoudConfig(void);
	~CjocLoudConfig(void);
	CjocLoudConfig::CjocLoudConfig(const CjocLoudConfig& l);
	CjocLoudConfig& operator= (const CjocLoudConfig& l)
	{
		if (this != &l)
		{
			this->Copy (l);
		}
		return *this;
	}

	bool GetUseCustomPreFilterCoefs ();
	bool GetUseCustomRLBFilterCoefs ();
	void SetUseCustomPreFilterCoefs (bool bUseCustomCoefs);
	void SetUseCustomRLBFilterCoefs (bool bUseCustomCoefs);
	
	void GetPreFilterCoefs(bool *pbDefault,std::vector<double> *pAFilterCoefs,std::vector<double> *pBFilterCoefs);
	void GetRLBFilterCoefs(bool *pbDefault,std::vector<double> *pAFilterCoefs,std::vector<double> *pBFilterCoefs);

	void SetPreFilterCoefs(std::vector<double> &AFilterCoefs, std::vector<double> &BFilterCoefs);
	void SetRLBFilterCoefs(std::vector<double> &AFilterCoefs, std::vector<double> &BFilterCoefs);

	double GetChannelGain(int nChannel);
	void ClearChannelGain ();
	void SetChannelGain (int nChannel, double dGain);

	long GetAudioBlockDurationMomentaryMS();
	double GetAudioBlockOverlappingMomentary();
	void SetAudioBlockDurationMomentaryMS(long lBlockDurMS);
	void SetAudioBlockOverlappingMomentary(double dOverlapping);
	
	long GetAudioBlockDurationShortTermMS();
	double GetAudioBlockOverlappingShortTerm();
	void SetAudioBlockDurationShortTermMS(long lBlockDurMS);
	void SetAudioBlockOverlappingShortTerm(double dOverlapping);

	long GetAudioBlockDurationLRAMS();
	double GetAudioBlockOverlappingLRA();
	void SetAudioBlockDurationLRAMS(long lBlockDurMS);
	void SetAudioBlockOverlappingLRA(double dOverlapping);
	double GetLRAAbsoluteThresholdDB ();
	void SetLRAAbsoluteThresholdDB (double dAbsTrshDB);
	double GetLRARelativeThresholdDB ();
	void SetLRARelativeThresholdDB (double dRelTrshDB);
	void SetLRAPercentiles (double dLowPercentile, double dHighPercentile);
	void GetLRAPercentiles (double *pdLowPercentile, double *pdHighPercentile);
	double GetLRALowPercentile ();
	double GetLRAHighPercentile ();
	void SetLRALowPercentile (double dPercentile);
	void SetLRAHighPercentile (double dPercentile);

	enINTERGRATEMODE GetIntegrateMode();
	std::wstring GetIntegrateModeStr();
	void SetIntegrateMode(enINTERGRATEMODE integrateMode);

	bool GetUseGating();
	void SetUseGating(bool bUseGating);
	double GetAbsoluteThresholdDB();
	void SetAbsoluteThresholdDB(double dAbsTrshDB);
	double GetRelativeThresholdDB();
	void SetRelativeThresholdDB(double dRelTrshDB);

	long GetReadBlockSizeInSamplesPerChannel ();
	void SetReadBlockSizeInSamplesPerChannel (long lBlockSize);
	
	long GetResultPrecission ();
	void SetResultPrecission(long lPrecission);

	std::wstring GetResultUnits();
	void SetResultUnits(std::wstring &strUnits);

	enPRESETMODE GetPresetMode();
	std::wstring GetPresetModeStr();
	void SetPresetMode(enPRESETMODE PresetMode);

	long GetTPDecayTimeMS ();
	void SetTPDecayTimeMS (long lTPDecayTime);

	double GetTPDecayValueDB();
	void SetTPDecayValueDB (double dTPDecayValueDB);

	long GetTPRefreshIntervalMS ();
	void SetTPRefreshIntervalMS (long lTPRefreshInterval);

	long GetLPFCutOff ();
	void SetLPFCutOff (long lFc);

	void SetUpsamplefactor (int nUpsF);
	int GetUpsamplefactor ();

	bool GetLRACalc ();
	void SetLRACalc (bool bCalc);
	bool GetTPCalc ();
	void SetTPCalc (bool bCalc);

	void IniR128 ();
	void IniA85 ();
	
private:
	void Copy (const CjocLoudConfig& l);

//Prefilter coefs
	bool m_bpreFilterCoefs;
	std::vector<double> m_preFilterACoefs;
	std::vector<double> m_preFilterBCoefs;

//RlbFilter coefs
	bool m_brlbFilterCoefs;
	std::vector<double> m_rlbFilterACoefs;
	std::vector<double> m_rlbFilterBCoefs;

//Channel gains
	std::vector<double> m_channelGains;

//Momentary data
	long m_lAudioBlockDurationMomentaryMS;
	double m_dAudioBlockOverlappingMomentary;
	
//Short term data
	long m_lAudioBlockDurationShortTermMS;
	double m_dAudioBlockOverlappingShortTerm;

//LRA data
	long m_lAudioBlockDurationLRAMS;
	double m_dAudioBlockOverlappingLRA;
	double m_dLRAAbsoluteThresholdDB;
	double m_dLRARelativeThresholdDB;
	double m_dLRALowPercentile;
	double m_dLRAHighPercentile;
	bool m_bCalcLRA;

//Integrated value (only standarized value for A85)
	enINTERGRATEMODE m_IntergrateMode;
	bool m_bGating;
	double m_dAbsoluteThresholdDB;
	double m_dRelativeThresholdDB;

//Preset Mode
	enPRESETMODE m_presetMode;

//Internal config
	long m_lReadBlockSizeInSamplesPerChannel;

//Result precision
	long m_lResultPrecission;

//Result units
	std::wstring m_strResultUnits;

//TP values
	long m_lTPDecayTimeMS;
	double m_dTPDecayValueDB;
	long m_lTPRefreshInterval;
	bool m_bCalcTP;

//LPF Filter value
	long m_lLPFFc;
	int m_nUpsF;
};

