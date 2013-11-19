#pragma once
//#include "BaseClasses\CjocIniConfig.h"
#include"..\JOCLoudnessBase.h"

class JOC_EXPORTS CJocLoudnessConfig :
	public CjocIniConfig
{
public:
	enum enAPP_MODE{ APPMODE_LOUD, APPMODE_FILTER_LPF, APPMODE_UPSAMPLE};

	CJocLoudnessConfig(void)
	{
		//General 
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgstrAction,L"Action"));
		
		//Channel mapping
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgstrMapChanel,L"MapChanel"));

		//Read data
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgnReadBlockSizeInSamplesPerChannel,L"ReadBlockSizeInSamplesPerChannel"));

		//Preset 
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgstrPreset,L"Preset"));

		//PreFilterCoefs
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgnPreFilterCoefsAuto,L"PreFilterCoefsAuto"));
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgstrPreFilterACoefs,L"PreFilterACoefs"));
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgstrPreFilterBCoefs,L"PreFilterBCoefs"));

		//RLBFilterCoefs
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgnRLBPreFilterCoefsAuto,L"RLBPreFilterCoefsAuto"));
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgstrRLBFilterACoefs,L"RLBFilterACoefs"));
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgstrRLBFilterBCoefs,L"RLBFilterBCoefs"));

		//Momentary data
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgnMomentaryAudioBlockDurationMS,L"MomentaryAudioBlockDurationMS"));
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgdMomentaryAudioBlockOverlapping,L"MomentaryAudioBlockOverlapping"));

		//Short term data
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgnShortTermAudioBlockDurationMS,L"ShortTermAudioBlockDurationMS"));
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgdShortTermAudioBlockOverlapping,L"ShortTermAudioBlockOverlapping"));

		//Integrate mesure data
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgstrIntegrateAlg,L"IntegrateAlg"));
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgnIntegrateGating,L"IntegrateGating"));
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgdIntegrateAbsoluteThresholdDB,L"IntegrateAbsoluteThresholdDB"));
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgdIntegrateRelativeThresholdDB,L"IntegrateRelativeThresholdDB"));

		//Result precission
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgnResultPrecission ,L"ResultPrecission"));

		//Result units
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgstrResultUnits,L"ResultUnits"));	

		//TP data
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgdTPDecaytimeMS,L"TPDecaytimeMS"));	
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgdTPRefreshIntervalMS,L"TPRefreshIntervalMS"));	
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgdTPDecayValueDB,L"TPDecayValueDB"));	
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgnTPCalc,L"TPCalc"));	

		//LPF Test data
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgdFrecCutOff,L"LPFFrecCutOff"));	
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgdUpsamplefactor,L"upsamplefactor"));

		//LRA data
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgnLRAAudioBlockDurationMS,L"LRAAudioBlockDurationMS"));	
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgdLRAAudioBlockOverlapping,L"LRAAudioBlockOverlapping"));	
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgdLRAAbsoluteThresholdDB,L"LRAAbsoluteThresholdDB"));	
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgdLRARelativeThresholdDB,L"LRARelativeThresholdDB"));	
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgdLRALowPercentile,L"LRALowPercentile"));	
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgdLRAHighPercentile,L"LRAHighPercentile"));	
		m_configLoadedValues.insert (std::pair <int, std::wstring>(enConfigValues::cfgnLRACalc,L"LRACalc"));	
	}

	~CJocLoudnessConfig(void);

	void Load (std::wstring &strFile);

	static void SetConfigData (CJocLoudnessConfig &Config, CjocLoudConfig *pcfgLoud, enAPP_MODE *pAppMode);

	enum enConfigValues
	{
		//General 
		cfgstrAction = 0,		//Action to do (loud / filter)

		//Channel mapping
		cfgstrMapChanel = 1,	//MapChanel1

		//Read data
		cfgnReadBlockSizeInSamplesPerChannel = 17,	//ReadBlockSizeInSamplesPerChannel

		//Preset 
		cfgstrPreset = 18,		//Preset

		//PreFilterCoefs
		cfgnPreFilterCoefsAuto = 19,	//PreFilterCoefsAuto
		cfgstrPreFilterACoefs = 20,		//PreFilterACoefs
		cfgstrPreFilterBCoefs = 21,		//PreFilterBCoefs

		//RLBFilterCoefs
		cfgnRLBPreFilterCoefsAuto = 22,	//RLBPreFilterCoefsAuto
		cfgstrRLBFilterACoefs = 23,		//RLBFilterACoefs
		cfgstrRLBFilterBCoefs = 24,		//RLBFilterBCoefs

		//Momentary data
		cfgnMomentaryAudioBlockDurationMS =25, //MomentaryAudioBlockDurationMS
		cfgdMomentaryAudioBlockOverlapping =26, //MomentaryAudioBlockOverlapping

		//Short term data
		cfgnShortTermAudioBlockDurationMS =27, //ShortTermAudioBlockDurationMS
		cfgdShortTermAudioBlockOverlapping =28, //ShortTermAudioBlockOverlapping

		//Integrate mesure data
		cfgstrIntegrateAlg = 29,				//IntegrateAlg
		cfgnIntegrateGating = 30,				//IntegrateGating
		cfgdIntegrateAbsoluteThresholdDB = 31,	//IntegrateAbsoluteThresholdDB
		cfgdIntegrateRelativeThresholdDB = 32,	//IntegrateRelativeThresholdDB

		//Precission
		cfgnResultPrecission = 33,	//ResultPrecission

		//Units 
		cfgstrResultUnits = 34, //ResultUnits 

		//TP data
		cfgdTPDecaytimeMS = 35,	//TPDecaytimeMS 
		cfgdTPRefreshIntervalMS = 36,	//TPRefreshIntervalMS 
		cfgdTPDecayValueDB = 37,	//TPDecayValueDB 
		cfgnTPCalc = 38, //TPCalc

		//Filter test
		cfgdFrecCutOff = 39,	//LPFFrecCutOff
		cfgdUpsamplefactor = 40, //upsamplefactor

		//LRA
		cfgnLRAAudioBlockDurationMS = 41,	//"LRAAudioBlockDurationMS"
		cfgdLRAAudioBlockOverlapping = 42,	//"LRAAudioBlockOverlapping"
		cfgdLRAAbsoluteThresholdDB = 43, //LRAAbsoluteThresholdDB
		cfgdLRARelativeThresholdDB = 44, //LRARelativeThresholdDB
		cfgdLRALowPercentile = 45, //LRALowPercentile
		cfgdLRAHighPercentile = 46, //LRAHighPercentile
		cfgnLRACalc = 47	//LRACalc
	};

};

