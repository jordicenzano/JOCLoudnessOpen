#include "JocLoudnessConfig.h"


//Contructor in header file 

CJocLoudnessConfig::~CJocLoudnessConfig(void)
{
}

void CJocLoudnessConfig::Load (std::wstring &strFile)
{
	CjocIniConfig::Load (strFile);
}

void CJocLoudnessConfig::SetConfigData (CJocLoudnessConfig &Config, CjocLoudConfig *pcfgLoud, enAPP_MODE *pAppMode)
{
	if (pcfgLoud == NULL){return;}

	//Set Config data ********************
	std::wstring strTmp;

	//App Mode
	strTmp = CjocUtils::ToLower (Config.GetConfigValueS (CJocLoudnessConfig::cfgstrAction));
	if (strTmp == L"filterlpf")
	{
		if (pAppMode != NULL){*pAppMode = enAPP_MODE::APPMODE_FILTER_LPF;}
	}
	else if (strTmp == L"upsample")
	{
		if (pAppMode != NULL){*pAppMode = enAPP_MODE::APPMODE_UPSAMPLE;}
	}
	else
	{
		if (pAppMode != NULL){*pAppMode = enAPP_MODE::APPMODE_LOUD;}
	}
				
	//Channel map
	bool bFound = true;
	for (int n = 0; (n < CjocLoudConfig::MAX_AUDIO_CHANNELS)&&(bFound == true); n++)
	{
		std::wstring strMap;
		bFound = Config.GetConfigValueSFromVector (&strMap,CJocLoudnessConfig::cfgstrMapChanel,n);
		if (bFound == true)
		{
			if (n == 0){pcfgLoud->ClearChannelGain();}

			pcfgLoud->SetChannelGain (n,CjocLoudConfig::GetChannelGainFromDef (strMap));
		}
	}

	//Block size
	unsigned long ulBlockSize = Config.GetConfigValueUL (CJocLoudnessConfig::cfgnReadBlockSizeInSamplesPerChannel,0);
	if (ulBlockSize >0)
		pcfgLoud->SetReadBlockSizeInSamplesPerChannel (ulBlockSize);

	//TP Values 
	pcfgLoud->SetTPDecayTimeMS (Config.GetConfigValueUL (CJocLoudnessConfig::cfgdTPDecaytimeMS,0));
	pcfgLoud->SetTPDecayValueDB(Config.GetConfigValueD (CJocLoudnessConfig::cfgdTPDecayValueDB ,0));
	pcfgLoud->SetTPRefreshIntervalMS (Config.GetConfigValueUL (CJocLoudnessConfig::cfgdTPRefreshIntervalMS,0));
	pcfgLoud->SetTPCalc (Config.GetConfigValueI (CJocLoudnessConfig::cfgnTPCalc,0));

	//LFP Cut off freq
	pcfgLoud->SetLPFCutOff (Config.GetConfigValueUL (CJocLoudnessConfig::cfgdFrecCutOff,0));

	//LFP Cut off freq
	pcfgLoud->SetUpsamplefactor(Config.GetConfigValueUL (CJocLoudnessConfig::cfgdUpsamplefactor,0));

	//Preset
	strTmp = Config.GetConfigValueS  (CJocLoudnessConfig::cfgstrPreset);
	if (CjocUtils::ToLower (strTmp) == L"r128" )
	{
		pcfgLoud->IniR128 ();
	}
	else if (CjocUtils::ToLower (strTmp) == L"a85" )
	{
		pcfgLoud->IniA85 ();
	}
	else
	{//custom
		pcfgLoud->SetPresetMode (CjocLoudConfig::enPRESETMODE::PRESET_MODE_CUSTOM);

		int nTmp = 0;
		unsigned long ulTmp = 0;
		double dTmp = 0;

		//Pre filter coefs
		nTmp = Config.GetConfigValueI  (CJocLoudnessConfig::cfgnPreFilterCoefsAuto );
		if (nTmp == 0)
		{//Prefilter coefs
			pcfgLoud->SetUseCustomPreFilterCoefs (true);
			//Read coefs
			std::vector <double> preACoefs;
			std::vector <double> preBCoefs;
			bool bFound = true;
			int n = 0;
			double dCoef = 0;
			while (bFound == true)
			{
				bFound = Config.GetConfigValueDFromVector (&dCoef,CJocLoudnessConfig::cfgstrPreFilterACoefs ,n);
				if (bFound == true)
				{
					preACoefs.push_back (dCoef);
					n++;
				}
			}

			bFound = true;
			n = 0;
			dCoef = 0;
			while (bFound == true)
			{
				bFound = Config.GetConfigValueDFromVector (&dCoef,CJocLoudnessConfig::cfgstrPreFilterBCoefs,n);
				if (bFound == true)
				{
					preBCoefs.push_back (dCoef);
					n++;
				}
			}

			//Set coefs
			pcfgLoud->SetPreFilterCoefs (preACoefs,preBCoefs);
		}

		//Result config
		nTmp = Config.GetConfigValueI  (CJocLoudnessConfig::cfgnResultPrecission,-1);
		pcfgLoud->SetResultPrecission (nTmp);

		strTmp = Config.GetConfigValueS (CJocLoudnessConfig::cfgstrResultUnits);
		pcfgLoud->SetResultUnits (strTmp );

		//RLB filter coefs
		nTmp = Config.GetConfigValueI  (CJocLoudnessConfig::cfgnRLBPreFilterCoefsAuto);
		if (nTmp == 0)
		{//RLBfilter coefs
			pcfgLoud->SetUseCustomRLBFilterCoefs (true);
			//Read coefs
			std::vector <double> preACoefs;
			std::vector <double> preBCoefs;
			bool bFound = true;
			int n = 0;
			double dCoef = 0;
			while (bFound == true)
			{
				bFound = Config.GetConfigValueDFromVector (&dCoef,CJocLoudnessConfig::cfgstrRLBFilterACoefs ,n);
				if (bFound == true)
				{
					preACoefs.push_back (dCoef);
					n++;
				}
			}	

			bFound = true;
			n = 0;
			dCoef = 0;
			while (bFound == true)
			{
				bFound = Config.GetConfigValueDFromVector (&dCoef,CJocLoudnessConfig::cfgstrRLBFilterBCoefs ,n);
				if (bFound == true)
				{
					preBCoefs.push_back (dCoef);
					n++;
				}
			}	

			//Set coefs
			pcfgLoud->SetRLBFilterCoefs (preACoefs,preBCoefs);
		}

		//MomentaryAudioBlockDurationMS
		ulTmp = Config.GetConfigValueUL (CJocLoudnessConfig::cfgnMomentaryAudioBlockDurationMS ,0);
		if (ulTmp > 0)
			pcfgLoud->SetAudioBlockDurationMomentaryMS (ulTmp);

		//MomentaryAudioBlockOverlapping
		dTmp = Config.GetConfigValueD (CJocLoudnessConfig::cfgdMomentaryAudioBlockOverlapping ,0);
		if (dTmp > 0)
			pcfgLoud->SetAudioBlockOverlappingMomentary  (dTmp);

		//ShortTermAudioBlockDurationMS
		ulTmp = Config.GetConfigValueUL (CJocLoudnessConfig::cfgnShortTermAudioBlockDurationMS ,0);
		if (ulTmp > 0)
			pcfgLoud->SetAudioBlockDurationShortTermMS (ulTmp);

		//ShortTermAudioBlockOverlapping
		dTmp = Config.GetConfigValueD (CJocLoudnessConfig::cfgdShortTermAudioBlockOverlapping ,0);
		if (dTmp > 0)
			pcfgLoud->SetAudioBlockOverlappingShortTerm (dTmp);

		//ShortTermAudioBlockOverlapping
		dTmp = Config.GetConfigValueD (CJocLoudnessConfig::cfgdShortTermAudioBlockOverlapping ,0);
		if (dTmp > 0)
			pcfgLoud->SetAudioBlockOverlappingShortTerm (dTmp);
				
		//IntegrateAlg
		strTmp = Config.GetConfigValueS  (CJocLoudnessConfig::cfgstrIntegrateAlg);
		if (CjocUtils::ToLower (strTmp) == L"bs17700" )
		{
			pcfgLoud->SetIntegrateMode (CjocLoudConfig::enINTERGRATEMODE::INTEGRATED_MODE_BS1770_USA);
		}
		else
		{
			pcfgLoud->SetIntegrateMode (CjocLoudConfig::enINTERGRATEMODE::INTEGRATED_MODE_BS17702_EU);

			//IntegrateGating
			nTmp = Config.GetConfigValueI  (CJocLoudnessConfig::cfgnIntegrateGating);
			if (nTmp == 0)
			{
				pcfgLoud->SetUseGating (false);
			}
			else
			{
				pcfgLoud->SetUseGating (true);

				//IntegrateAbsoluteThresholdDB
				dTmp = Config.GetConfigValueD (CJocLoudnessConfig::cfgdIntegrateAbsoluteThresholdDB ,99999);
				if (dTmp != 99999)
					pcfgLoud->SetAbsoluteThresholdDB (dTmp);
	
				//IntegrateRelativeThresholdDB
				dTmp = Config.GetConfigValueD (CJocLoudnessConfig::cfgdIntegrateRelativeThresholdDB ,99999);
				if (dTmp != 99999)
					pcfgLoud->SetRelativeThresholdDB (dTmp);
	
			}
		}

		//Load LRA data
		//LRAAudioBlockDurationMS
		ulTmp = Config.GetConfigValueUL (CJocLoudnessConfig::cfgnLRAAudioBlockDurationMS ,0);
		if (ulTmp > 0)
			pcfgLoud->SetAudioBlockDurationLRAMS (ulTmp);

		//LRAAudioBlockOverlapping
		dTmp = Config.GetConfigValueD (CJocLoudnessConfig::cfgdLRAAudioBlockOverlapping,0);
		if (dTmp > 0)
			pcfgLoud->SetAudioBlockOverlappingLRA (dTmp);

		//LRAAbsoluteThresholdDB
		dTmp = Config.GetConfigValueD (CJocLoudnessConfig::cfgdLRAAbsoluteThresholdDB ,99999);
		if (dTmp != 99999)
			pcfgLoud->SetLRAAbsoluteThresholdDB (dTmp);
	
		//LRARelativeThresholdDB
		dTmp = Config.GetConfigValueD (CJocLoudnessConfig::cfgdLRARelativeThresholdDB ,99999);
		if (dTmp != 99999)
			pcfgLoud->SetLRARelativeThresholdDB (dTmp);

		//LRA low percentile
		dTmp = Config.GetConfigValueD (CJocLoudnessConfig::cfgdLRALowPercentile ,99999);
		if (dTmp != 99999)
			pcfgLoud->SetLRALowPercentile (dTmp);

		dTmp = Config.GetConfigValueD (CJocLoudnessConfig::cfgdLRAHighPercentile,99999);
		if (dTmp != 99999)
			pcfgLoud->SetLRAHighPercentile (dTmp);
			
		pcfgLoud->SetLRACalc (Config.GetConfigValueI (CJocLoudnessConfig::cfgnLRACalc,0));
	}
}
