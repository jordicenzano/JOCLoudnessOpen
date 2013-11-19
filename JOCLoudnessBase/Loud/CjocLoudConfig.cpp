#include "CjocLoudConfig.h"
#include "..\BaseClasses\CjocUtils.h"

CjocLoudConfig::CjocLoudConfig(void)
{
	IniR128 ();

	m_channelGains.clear ();
	for (int nC = 0; nC < MAX_AUDIO_CHANNELS; nC++)
	{
		if ((nC >= 0)&&(nC < 3))
			m_channelGains.push_back (1.0);
		else if ((nC >= 3)&&(nC < 5))
			m_channelGains.push_back (1.41);		
		else if (nC >= 5)
			m_channelGains.push_back (0);	//Blank all channels > 5; EBU mode: LFE channel not into account
	}

	m_lReadBlockSizeInSamplesPerChannel = 1024; 

	m_bCalcTP = false; //Default = false (calc of TP es time consuming)
	m_lTPDecayTimeMS = 1700;
	m_dTPDecayValueDB = -20.0;
	m_lTPRefreshInterval = 250;

	m_lLPFFc = 0;
	m_nUpsF = 4;
}


CjocLoudConfig::~CjocLoudConfig(void)
{
}

void CjocLoudConfig::IniR128 ()
{
	m_bpreFilterCoefs = false;
	m_preFilterACoefs.clear ();
	m_preFilterBCoefs.clear ();

	m_brlbFilterCoefs = false;
	m_rlbFilterACoefs.clear ();
	m_rlbFilterBCoefs.clear ();

	//Set channel gains (L = R = C = 1.0 ; Ls = Rs = 1.41)
	/*m_channelGains.clear ();
	for (int nC = 0; nC < 128; nC++)
	{
		if ((nC >= 0)&&(nC < 3))
			m_channelGains.push_back (1.0);
		else if ((nC >= 3)&&(nC < 5))
			m_channelGains.push_back (1.41);		
		else if (nC >= 5)
			m_channelGains.push_back (0);	//Blank all channels > 5; EBU mode: LFE channel not into account
	}*/

	m_lAudioBlockDurationMomentaryMS = 400;
	m_dAudioBlockOverlappingMomentary = 0.75;
	
	m_lAudioBlockDurationShortTermMS = 3000;
	m_dAudioBlockOverlappingShortTerm = 0.75;

	m_lAudioBlockDurationLRAMS = 3000;
	m_dAudioBlockOverlappingLRA = 0.75; //Missundestood in EBU Tech 3342: Overlap > 0.66

	m_dLRAAbsoluteThresholdDB = -70.0;
	m_dLRARelativeThresholdDB = -20.0; //Error in EBU Tech 3342 page 8: If we use short term data the Relative treshhold is set in -10

	m_dLRALowPercentile = 10.0;
	m_dLRAHighPercentile = 95.0;
		
	m_bCalcLRA = true;
	
	m_IntergrateMode = enINTERGRATEMODE::INTEGRATED_MODE_BS17702_EU; 
	m_bGating = true;
	m_dAbsoluteThresholdDB = -70.0;
	m_dRelativeThresholdDB = -10.0;

	m_presetMode = enPRESETMODE::PRESET_MODE_R128_EU;

	m_lResultPrecission = 1;
	m_strResultUnits = L"LUFS";

	//m_lReadBlockSizeInSamplesPerChannel = 1024;
}

void CjocLoudConfig::IniA85 ()
{
	//A85 same pre filter curve as R128 (ITU R-1770)
	m_bpreFilterCoefs = false;
	m_preFilterACoefs.clear ();
	m_preFilterBCoefs.clear ();

	//A85 same rlb filter curve as R128 (ITU R-1770)
	m_brlbFilterCoefs = false;
	m_rlbFilterACoefs.clear ();
	m_rlbFilterBCoefs.clear ();

	//A85 same channel gains as R128 (ITU R-1770) (L = R = C = 1.0 ; Ls = Rs = 1.41)
	/*m_channelGains.clear ();
	for (int nC = 0; nC < 128; nC++)
	{
		if ((nC >= 0)&&(nC < 3))
			m_channelGains.push_back (1.0);
		else if ((nC >= 3)&&(nC < 5))
			m_channelGains.push_back (1.41);		
		else if (nC >= 5)
			m_channelGains.push_back (0);	//Blank all channels > 5; EBU mode: LFE channel not into account
	}*/

	//A85 doen't standarize intermediate values of loudness. We use the same values of sliding window as R128
	m_lAudioBlockDurationMomentaryMS = 400;
	m_dAudioBlockOverlappingMomentary = 0.75;
	
	//A85 doen't standarize intermediate values of loudness. We use the same values of sliding window as R128
	m_lAudioBlockDurationShortTermMS = 3000;
	m_dAudioBlockOverlappingShortTerm = 0.75;

	m_lAudioBlockDurationLRAMS = 3000;
	m_dAudioBlockOverlappingLRA = 0.66;

	m_dLRAAbsoluteThresholdDB = -70;
	m_dLRARelativeThresholdDB = -20;
	
	m_dLRALowPercentile = 10.0;
	m_dLRAHighPercentile = 95.0;

	m_bCalcLRA = false;

	//A85 uses ITU R-1770 with NO gating
	m_IntergrateMode = enINTERGRATEMODE::INTEGRATED_MODE_BS1770_USA;
	m_bGating = false;
	m_dAbsoluteThresholdDB = 0.0;
	m_dRelativeThresholdDB = 0.0;

	m_presetMode = enPRESETMODE::PRESET_MODE_E85_USA;

	m_lResultPrecission = 0;
	m_strResultUnits = L"LKFS";

	//m_lReadBlockSizeInSamplesPerChannel = 1024;
}

bool CjocLoudConfig::GetUseCustomPreFilterCoefs ()
{
	return m_bpreFilterCoefs;
}

bool CjocLoudConfig::GetUseCustomRLBFilterCoefs ()
{
	return m_brlbFilterCoefs;
}
	
void CjocLoudConfig::SetUseCustomPreFilterCoefs (bool bUseCustomCoefs)
{
	m_bpreFilterCoefs = bUseCustomCoefs;
}

void CjocLoudConfig::SetUseCustomRLBFilterCoefs (bool bUseCustomCoefs)
{
	m_brlbFilterCoefs = bUseCustomCoefs;
}

void CjocLoudConfig::GetPreFilterCoefs(bool *pbDefault,std::vector<double> *pAFilterCoefs,std::vector<double> *pBFilterCoefs)
{
	if (pbDefault != NULL){*pbDefault = m_bpreFilterCoefs;}

	if (pAFilterCoefs != NULL)
	{
		pAFilterCoefs->clear ();
		for (int n = 0; n < m_preFilterACoefs.size (); n++)
			pAFilterCoefs->push_back (m_preFilterACoefs[n]);
	}
	
	if (pBFilterCoefs != NULL)
	{
		pBFilterCoefs->clear ();
		for (int n = 0; n < m_preFilterBCoefs.size (); n++)
			pBFilterCoefs->push_back (m_preFilterBCoefs[n]);
	}
}

void CjocLoudConfig::GetRLBFilterCoefs(bool *pbDefault,std::vector<double> *pAFilterCoefs,std::vector<double> *pBFilterCoefs)
{
	if (pbDefault != NULL){*pbDefault = m_brlbFilterCoefs;}

	if (pAFilterCoefs != NULL)
	{
		pAFilterCoefs->clear ();
		for (int n = 0; n < m_rlbFilterACoefs.size (); n++)
			pAFilterCoefs->push_back (m_rlbFilterACoefs[n]);
	}
	
	if (pBFilterCoefs != NULL)
	{
		pBFilterCoefs->clear ();
		for (int n = 0; n < m_rlbFilterBCoefs.size (); n++)
			pBFilterCoefs->push_back (m_rlbFilterBCoefs[n]);
	}
}

void CjocLoudConfig::SetPreFilterCoefs(std::vector<double> &AFilterCoefs, std::vector<double> &BFilterCoefs)
{
	m_preFilterACoefs.clear ();
	for (int n = 0; n < AFilterCoefs.size (); n++)
		m_preFilterACoefs.push_back (AFilterCoefs[n]);

	m_preFilterBCoefs.clear ();
	for (int n = 0; n < BFilterCoefs.size (); n++)
		m_preFilterBCoefs.push_back (BFilterCoefs[n]);
}

void CjocLoudConfig::SetRLBFilterCoefs(std::vector<double> &AFilterCoefs, std::vector<double> &BFilterCoefs)
{
	m_rlbFilterACoefs.clear ();
	for (int n = 0; n < AFilterCoefs.size (); n++)
		m_rlbFilterACoefs.push_back (AFilterCoefs[n]);

	m_rlbFilterBCoefs.clear ();
	for (int n = 0; n < BFilterCoefs.size (); n++)
		m_rlbFilterBCoefs.push_back (BFilterCoefs[n]);
}

double CjocLoudConfig::GetChannelGain(int nChannel)
{
	double dRet = 0;
	if (nChannel < m_channelGains.size ())
	{
		dRet = m_channelGains[nChannel];
	}

	return dRet;
}

void CjocLoudConfig::ClearChannelGain()
{
	m_channelGains.clear ();
	for (int nC = 0; nC < MAX_AUDIO_CHANNELS; nC++)
		m_channelGains.push_back (0);	
}

void CjocLoudConfig::SetChannelGain (int nChannel, double dGain)
{
	if ((nChannel >= 0)&&(nChannel < MAX_AUDIO_CHANNELS))
		m_channelGains[nChannel] = dGain;
}

long CjocLoudConfig::GetAudioBlockDurationMomentaryMS()
{
	return m_lAudioBlockDurationMomentaryMS;
}

void CjocLoudConfig::SetAudioBlockDurationMomentaryMS(long lBlockDurMS)
{
	m_lAudioBlockDurationMomentaryMS = lBlockDurMS;
}

double CjocLoudConfig::GetAudioBlockOverlappingMomentary()
{
	return m_dAudioBlockOverlappingMomentary;
}

void CjocLoudConfig::SetAudioBlockOverlappingMomentary(double dOverlapping)
{
	m_dAudioBlockOverlappingMomentary = dOverlapping;
}

long CjocLoudConfig::GetAudioBlockDurationShortTermMS()
{
	return m_lAudioBlockDurationShortTermMS;
}

void CjocLoudConfig::SetAudioBlockDurationShortTermMS(long lBlockDurMS)
{
	m_lAudioBlockDurationShortTermMS = lBlockDurMS;
}
	
double CjocLoudConfig::GetAudioBlockOverlappingShortTerm()
{
	return m_dAudioBlockOverlappingShortTerm;
}

void CjocLoudConfig::SetAudioBlockOverlappingShortTerm(double dOverlapping)
{
	m_dAudioBlockOverlappingShortTerm = dOverlapping;
}

long CjocLoudConfig::GetAudioBlockDurationLRAMS()
{
	return m_lAudioBlockDurationLRAMS;
}
	
double CjocLoudConfig::GetAudioBlockOverlappingLRA()
{
	return m_dAudioBlockOverlappingLRA;
}
	
void CjocLoudConfig::SetAudioBlockDurationLRAMS(long lBlockDurMS)
{
	m_lAudioBlockDurationLRAMS = lBlockDurMS;
}
	
void CjocLoudConfig::SetAudioBlockOverlappingLRA(double dOverlapping)
{
	m_dAudioBlockOverlappingLRA= dOverlapping;
}

double CjocLoudConfig::GetLRAAbsoluteThresholdDB ()
{
	return m_dLRAAbsoluteThresholdDB;
}
	
void CjocLoudConfig::SetLRAAbsoluteThresholdDB (double dAbsTrshDB)
{
	m_dLRAAbsoluteThresholdDB = dAbsTrshDB;
}

double CjocLoudConfig::GetLRARelativeThresholdDB ()
{
	return m_dLRARelativeThresholdDB; 
}

void CjocLoudConfig::SetLRARelativeThresholdDB (double dRelTrshDB)
{
	m_dLRARelativeThresholdDB = dRelTrshDB;
}

void CjocLoudConfig::SetLRAPercentiles (double dLowPercentile, double dHighPercentile)
{
	m_dLRALowPercentile = dLowPercentile;
	m_dLRAHighPercentile = dHighPercentile;
}

void CjocLoudConfig::GetLRAPercentiles (double *pdLowPercentile, double *pdHighPercentile)
{
	if (pdLowPercentile != NULL){*pdLowPercentile = m_dLRALowPercentile;}
	if (pdHighPercentile != NULL){*pdHighPercentile = m_dLRAHighPercentile;}
}

void CjocLoudConfig::SetLRALowPercentile (double dPercentile)
{
	m_dLRALowPercentile = dPercentile;
}

void CjocLoudConfig::SetLRAHighPercentile (double dPercentile)
{
	m_dLRAHighPercentile = dPercentile;
}

double CjocLoudConfig::GetLRALowPercentile ()
{
	return m_dLRALowPercentile;
}

double CjocLoudConfig::GetLRAHighPercentile ()
{
	return m_dLRAHighPercentile;
}

CjocLoudConfig::enINTERGRATEMODE CjocLoudConfig::GetIntegrateMode()
{
	return m_IntergrateMode; 
}

void CjocLoudConfig::SetIntegrateMode(enINTERGRATEMODE integrateMode)
{
	m_IntergrateMode = integrateMode;
}

std::wstring CjocLoudConfig::GetIntegrateModeStr()
{
	std::wstring strIntMode;
	if (m_IntergrateMode == enINTERGRATEMODE::INTEGRATED_MODE_BS17702_EU )
		strIntMode = L"ITU-R-BS.1770";
	else if(m_IntergrateMode == enINTERGRATEMODE::INTEGRATED_MODE_BS1770_USA)
		strIntMode = L"ITU-R-BS.1770-2";

	return strIntMode;
}

bool CjocLoudConfig::GetUseGating()
{
	return m_bGating;
}
	
void CjocLoudConfig::SetUseGating(bool bUseGating)
{
	m_bGating = bUseGating;
}

double CjocLoudConfig::GetAbsoluteThresholdDB()
{
	return m_dAbsoluteThresholdDB;
}
	
void CjocLoudConfig::SetAbsoluteThresholdDB(double dAbsTrshDB)
{
	m_dAbsoluteThresholdDB = dAbsTrshDB;
}

double CjocLoudConfig::GetRelativeThresholdDB()
{
	return m_dRelativeThresholdDB;
}

void CjocLoudConfig::SetRelativeThresholdDB(double dRelTrshDB)
{
	m_dRelativeThresholdDB= dRelTrshDB;
}

CjocLoudConfig::enPRESETMODE CjocLoudConfig::GetPresetMode()
{
	return m_presetMode;
}

std::wstring CjocLoudConfig::GetPresetModeStr()
{
	std::wstring strPrMode = L"CUSTOM";
	if (m_presetMode == enPRESETMODE::PRESET_MODE_R128_EU)
		strPrMode = L"EBU R128";
	else if(m_IntergrateMode == enPRESETMODE::PRESET_MODE_E85_USA)
		strPrMode = L"ATSC A/85";

	return strPrMode;
}

void CjocLoudConfig::SetPresetMode(enPRESETMODE PresetMode)
{
	m_presetMode = PresetMode;
}

long CjocLoudConfig::GetReadBlockSizeInSamplesPerChannel ()
{
	return m_lReadBlockSizeInSamplesPerChannel;
}

void CjocLoudConfig::SetReadBlockSizeInSamplesPerChannel (long lBlockSize)
{
	m_lReadBlockSizeInSamplesPerChannel = lBlockSize;
}

CjocLoudConfig::CjocLoudConfig(const CjocLoudConfig& l)
{
	Copy (l);
}

void CjocLoudConfig::Copy(const CjocLoudConfig& l)
{
	this->m_bpreFilterCoefs = l.m_bpreFilterCoefs;	

	this->m_preFilterACoefs.clear ();
	for (long ln = 0; ln < l.m_preFilterACoefs.size (); ln++)
		this->m_preFilterACoefs.push_back (l.m_preFilterACoefs[ln]);

	this->m_preFilterBCoefs.clear ();
	for (long ln = 0; ln < l.m_preFilterBCoefs.size (); ln++)
		this->m_preFilterBCoefs.push_back (l.m_preFilterBCoefs[ln]);


	this->m_brlbFilterCoefs = l.m_brlbFilterCoefs ;	

	this->m_rlbFilterACoefs.clear ();
	for (long ln = 0; ln < l.m_rlbFilterACoefs.size (); ln++)
		this->m_rlbFilterACoefs.push_back (l.m_rlbFilterACoefs[ln]);

	this->m_rlbFilterBCoefs.clear ();
	for (long ln = 0; ln < l.m_rlbFilterBCoefs.size (); ln++)
		this->m_rlbFilterBCoefs.push_back (l.m_rlbFilterBCoefs[ln]);


	this->m_channelGains.clear ();
	for (long ln = 0; ln < l.m_channelGains.size (); ln++)
		this->m_channelGains.push_back (l.m_channelGains[ln]);

	this->m_lAudioBlockDurationMomentaryMS = l.m_lAudioBlockDurationMomentaryMS;	
	this->m_dAudioBlockOverlappingMomentary = l.m_dAudioBlockOverlappingMomentary;	

	this->m_lAudioBlockDurationShortTermMS = l.m_lAudioBlockDurationShortTermMS;	
	this->m_dAudioBlockOverlappingShortTerm = l.m_dAudioBlockOverlappingShortTerm;	

	this->m_lAudioBlockDurationLRAMS= l.m_lAudioBlockDurationLRAMS;	
	this->m_dAudioBlockOverlappingLRA= l.m_dAudioBlockOverlappingLRA;	
	this->m_dLRAAbsoluteThresholdDB = l.m_dLRAAbsoluteThresholdDB;	
	this->m_dLRARelativeThresholdDB = l.m_dLRARelativeThresholdDB;	
	this->m_dLRALowPercentile = l.m_dLRALowPercentile;
	this->m_dLRAHighPercentile = l.m_dLRAHighPercentile;

	this->m_bCalcLRA = l.m_bCalcLRA;	

	this->m_lTPDecayTimeMS = l.m_lTPDecayTimeMS;
	this->m_dTPDecayValueDB = l.m_dTPDecayValueDB;
	this->m_lTPRefreshInterval = l.m_lTPRefreshInterval;
	this->m_bCalcTP = l.m_bCalcTP;
	
	this->m_lLPFFc = l.m_lLPFFc;
	this->m_nUpsF = l.m_nUpsF;
	
	this->m_IntergrateMode = l.m_IntergrateMode;	
	this->m_bGating = l.m_bGating;	
	this->m_dAbsoluteThresholdDB = l.m_dAbsoluteThresholdDB;	
	this->m_dRelativeThresholdDB = l.m_dRelativeThresholdDB;	
	
	this->m_lReadBlockSizeInSamplesPerChannel = l.m_lReadBlockSizeInSamplesPerChannel;	

	this->m_presetMode = l.m_presetMode;

	this->m_lResultPrecission = l.m_lResultPrecission;

	this->m_strResultUnits = l.m_strResultUnits;
}

double CjocLoudConfig::GetChannelGainFromDef (std::wstring &strChannel)
{
	double dRet = 0;

	if (CjocUtils::ToLower (CjocUtils::Trim ( strChannel)) == L"l")
		dRet = 1.0;
	else if (CjocUtils::ToLower (CjocUtils::Trim ( strChannel)) == L"r")
		dRet = 1.0;
	else if (CjocUtils::ToLower (CjocUtils::Trim ( strChannel)) == L"c")
		dRet = 1.0;
	else if (CjocUtils::ToLower (CjocUtils::Trim ( strChannel)) == L"ls")
		dRet = 1.41;
	else if (CjocUtils::ToLower (CjocUtils::Trim ( strChannel)) == L"rs")
		dRet = 1.41;

	return dRet;
}

long CjocLoudConfig::GetResultPrecission ()
{
	return m_lResultPrecission;
}

void CjocLoudConfig::SetResultPrecission(long lPrecission)
{
	m_lResultPrecission = lPrecission;
}

std::wstring CjocLoudConfig::GetResultUnits()
{
	return m_strResultUnits;
}

void CjocLoudConfig::SetResultUnits(std::wstring &strUnits)
{
	m_strResultUnits = strUnits;
}

long CjocLoudConfig::GetTPDecayTimeMS ()
{
	return m_lTPDecayTimeMS;
}

long CjocLoudConfig::GetTPRefreshIntervalMS ()
{
	return m_lTPRefreshInterval;
}

void CjocLoudConfig::SetTPDecayTimeMS (long lTPDecayTime)
{
	m_lTPDecayTimeMS = lTPDecayTime;
}

double CjocLoudConfig::GetTPDecayValueDB ()
{
	return m_dTPDecayValueDB;
}

void CjocLoudConfig::SetTPDecayValueDB (double dTPDecayValueDB)
{
	m_dTPDecayValueDB = dTPDecayValueDB;
}

void CjocLoudConfig::SetTPRefreshIntervalMS (long lTPRefreshInterval)
{
	m_lTPRefreshInterval = lTPRefreshInterval;
}

long CjocLoudConfig::GetLPFCutOff ()
{
	return m_lLPFFc;
}

void CjocLoudConfig::SetLPFCutOff (long lFc)
{
	m_lLPFFc = lFc;
}

void CjocLoudConfig::SetUpsamplefactor (int nUpsF)
{
	m_nUpsF = nUpsF;
}

int CjocLoudConfig::GetUpsamplefactor ()
{
	return m_nUpsF;
}

bool CjocLoudConfig::GetLRACalc ()
{
	return m_bCalcLRA;
}

void CjocLoudConfig::SetLRACalc (bool bCalc)
{
	m_bCalcLRA = bCalc;
}

bool CjocLoudConfig::GetTPCalc ()
{
	return m_bCalcTP;
}

void CjocLoudConfig::SetTPCalc (bool bCalc)
{
	m_bCalcTP = bCalc;
}