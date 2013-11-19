#include "CjocLoud.h"
#include "..\BaseClasses\CjocException.h"
#include "..\BaseClasses\CjocUtils.h"

CjocLoud::CjocLoud(void)
{
	m_state = enSTATE::ST_NO_INI;
	m_ulSampleRate = 0;
	m_sChannels = 0;

	m_dPlainLoudnessLKFS = 0.0;
	m_dIntegratedLoudnessLKFS = 0.0;
	m_dLRA = 0.0;

	m_bNewMomentarySample = false;
	m_bNewShortTermSample = false;
	m_bNewPlainSample = false;
	m_bNewLRASample = false;

	m_lMomentaryUpdateIntervalSamples = 0;
	m_lShortTermUpdateIntervalSamples = 0;
	m_lLRAUpdateIntervalSamples = 0;

	m_lTPUpdateIntervalSamples = 0;

	m_dLastST = 0;

	m_config.IniR128 ();
}

CjocLoud::~CjocLoud(void)
{
	//Clean
	for (int nC = 0 ; nC < m_sChannels ; nC++)
	{		
		if (m_TPmeters[nC] != NULL)
			delete m_TPmeters[nC];

		if (m_prefilters[nC] != NULL)
			delete m_prefilters[nC];

		if (m_rlbfilters[nC] != NULL)
			delete m_rlbfilters[nC];

		if (m_meansqfiltersMomentary[nC] != NULL)
			delete m_meansqfiltersMomentary[nC];

		if (m_meansqfiltersShortTerm[nC] != NULL)
			delete m_meansqfiltersShortTerm[nC];

		if (m_meansqfiltersPlain[nC] != NULL)
			delete m_meansqfiltersPlain[nC];
	}

	m_TPmeters.clear ();
	m_prefilters.clear ();
	m_rlbfilters.clear ();
	m_meansqfiltersMomentary.clear ();
	m_meansqfiltersShortTerm.clear ();
	m_meansqfiltersPlain.clear ();
}

void CjocLoud::SetConfig (CjocLoudConfig &config)
{
	if (m_state != enSTATE::ST_NO_INI){throw JOC_Error_(enjocErrors::err_bad_state);}
	
	m_config = config;

	m_state = enSTATE::ST_CONFIGURED;
}

void CjocLoud::Ini (unsigned long ulSampleRate, short sChannels)
{
	if (m_state != enSTATE::ST_CONFIGURED){throw JOC_Error_(enjocErrors::err_bad_state);}

	m_ulSampleRate = ulSampleRate;
	m_sChannels = sChannels;

	m_dLastST = 0;

	//INi components
	for (int nC = 0 ; nC < m_sChannels; nC++)
	{		
		//TP
		CjocTPLevel *pTPMetter = new CjocTPLevel  ();
		pTPMetter->Ini (m_ulSampleRate,m_config.GetTPDecayTimeMS(),m_config.GetTPDecayValueDB (), m_config.GetTPRefreshIntervalMS());
		m_TPListChannelsValue.push_back (pTPMetter->GetTPListValues ());
		if (nC == 0){m_lTPUpdateIntervalSamples = pTPMetter->GetRefreshIntervalSamples ();}
		m_TPmeters.push_back (pTPMetter);
		
		//PRE
		CjocPreFilter *ppreFilter = new CjocPreFilter ();
		if (m_config.GetUseCustomPreFilterCoefs () == false)
		{	//Auto calc coefs
			(*ppreFilter).SetFreqSampling (m_ulSampleRate); 
		}
		else
		{	//Set coefs
			std::vector<double> ACoefs;
			std::vector<double> BCoefs;
			m_config.GetPreFilterCoefs (NULL,&ACoefs,&BCoefs);
			(*ppreFilter).SetCoefs (BCoefs,ACoefs);
		}
		m_prefilters.push_back (ppreFilter );
		
		//RLB
		CjocRlbFilter *prlbFilter = new CjocRlbFilter ();
		if (m_config.GetUseCustomRLBFilterCoefs() == false)
		{	//Auto calc coefs
			(*prlbFilter).SetFreqSampling (m_ulSampleRate); 
		}
		else
		{	//Set coefs
			std::vector<double> ACoefs;
			std::vector<double> BCoefs;
			m_config.GetRLBFilterCoefs (NULL,&ACoefs,&BCoefs);
			(*prlbFilter).SetCoefs (BCoefs,ACoefs);
		}
		m_rlbfilters.push_back (prlbFilter);

		//Momentary
		CjocMeanFast *pMeanSqM = new CjocMeanFast ();
		pMeanSqM->Ini (CjocMeanFast::enMEAN_MODE::MEAN_OVERLAPPED,m_ulSampleRate, m_config.GetAudioBlockDurationMomentaryMS () ,m_config.GetAudioBlockOverlappingMomentary ());
		m_meanListChannelsPointersMomentary.push_back (pMeanSqM->GetMeanList ());
		if (nC == 0){m_lMomentaryUpdateIntervalSamples = pMeanSqM->GetMeamIntervalSamples ();}
		m_meansqfiltersMomentary.push_back (pMeanSqM);

		//Short term
		CjocMeanFast *pMeanSqST = new CjocMeanFast ();
		pMeanSqST->Ini (CjocMeanFast::enMEAN_MODE::MEAN_OVERLAPPED,m_ulSampleRate, m_config.GetAudioBlockDurationShortTermMS (),m_config.GetAudioBlockOverlappingShortTerm ());
		m_meanListChannelsPointersShortTerm.push_back (pMeanSqST->GetMeanList ());
		if (nC == 0){m_lShortTermUpdateIntervalSamples = pMeanSqST->GetMeamIntervalSamples ();}
		m_meansqfiltersShortTerm.push_back (pMeanSqST);

		//LRA
		CjocMeanFast *pMeanSqLRA = new CjocMeanFast ();
		pMeanSqLRA->Ini (CjocMeanFast::enMEAN_MODE::MEAN_OVERLAPPED,m_ulSampleRate, m_config.GetAudioBlockDurationLRAMS (),m_config.GetAudioBlockOverlappingLRA ());
		m_meanListChannelsPointersLRA.push_back (pMeanSqLRA->GetMeanList ());
		if (nC == 0){m_lLRAUpdateIntervalSamples = pMeanSqLRA->GetMeamIntervalSamples ();}
		m_meansqfiltersLRA.push_back (pMeanSqLRA);

		//Plain
		CjocMeanFast *pMeanSqPLAIN = new CjocMeanFast ();
		pMeanSqPLAIN->Ini (CjocMeanFast::enMEAN_MODE::MEAN_TOTAL);
		m_meanListChannelsPointersPlain.push_back (pMeanSqPLAIN->GetMeanList ());
		m_meansqfiltersPlain.push_back (pMeanSqPLAIN);

		//Set channel gains
		m_channelGains.push_back (m_config.GetChannelGain (nC));
	}
	
	m_calcLKFSST.Ini (m_channelGains, m_sChannels);
	m_calcLKFSST.SetTresholds (false);

	m_calcLKFSM.Ini (m_channelGains, m_sChannels);
	m_calcLKFSM.SetTresholds (m_config.GetUseGating (),m_config.GetAbsoluteThresholdDB (),m_config.GetRelativeThresholdDB ()); 
	m_calcLKFSM.ResetIntergratedMessure();

	m_calcLKFSLRA.Ini (m_channelGains, m_sChannels);
	m_calcLKFSLRA.SetTresholds (true,m_config.GetLRAAbsoluteThresholdDB (),m_config.GetLRARelativeThresholdDB ()); 
	m_calcLKFSLRA.SetLRAPercentiles (m_config.GetLRALowPercentile(),m_config.GetLRAHighPercentile());
	m_calcLKFSLRA.ResetIntergratedMessure();

	m_calcLKFSPlain.Ini (m_channelGains, m_sChannels);

	//Results
	m_MomentaryLoudnesslkfs.clear();
	m_ShortTermLoudnesslkfs.clear ();
	m_LRALoudnesslkfs.clear ();
	m_dPlainLoudnessLKFS = 0.0;
	m_dIntegratedLoudnessLKFS = 0.0;
	m_dLRA = 0.0;

	//Flags
	m_bNewMomentarySample = false;
	m_bNewShortTermSample = false;
	m_bNewPlainSample = false;
	m_bNewLRASample = false;

	m_state = enSTATE::ST_INI;
}

int CjocLoud::AddSample (double *pdSamples, unsigned long ulNumChannels, double *pdM, double *pdTP, double *pdST)
{
	int nRet = 0;
	double dTmp;

	if ((m_state != enSTATE::ST_INI)&&(m_state != enSTATE::ST_STARTED)){throw JOC_Error_(enjocErrors::err_bad_state);}

	m_state = enSTATE::ST_STARTED;

	if ((ulNumChannels > 0)&&(pdSamples != NULL))
	{
		for (int nC = 0 ; nC < m_sChannels ; nC++)
		{
			//TP
			if (m_config.GetTPCalc ()  == true)
			{
				CronoStart();
				
				dTmp = m_TPmeters[nC]->AddSample (&pdSamples[nC]);
				nRet = nRet | 2;
				if (pdTP != NULL){*pdTP = dTmp;}

				CronoEnd (enCronoValues::CRONO_TP_METTER);
			}

			CronoStart();
			m_prefilters[nC]->Filter (&pdSamples[nC], ulNumChannels, m_sChannels);
			CronoEnd (enCronoValues::CRONO_FILTER_PRE);

			CronoStart();
			m_rlbfilters[nC]->Filter (&pdSamples[nC], ulNumChannels, m_sChannels);
			CronoEnd (enCronoValues::CRONO_FILTER_RLB);

			//Momentary: Calc mean sq for each channel every X samples with determinated overlapping factor
			CronoStart();
			m_meansqfiltersMomentary[nC]->AddSamples (&pdSamples[nC], ulNumChannels, m_sChannels);
			CronoEnd (enCronoValues::CRONO_MEAN_FAST_MOMENTARY);

			//NEW V2
			//ST
			CronoStart();
			//Short Term: Calc mean sq for each channel every X samples with determinated overlapping factor
			m_meansqfiltersShortTerm[nC]->AddSamples (&pdSamples[nC], ulNumChannels, m_sChannels);								
			CronoEnd (enCronoValues::CRONO_MEAN_FAST_SHORTTERM);

		}

		//Momentary & Integrated 
		CronoStart();
		bool bRc = m_calcLKFSM.CalcLKFSRT (&m_meanListChannelsPointersMomentary, &dTmp, false);
		if (bRc == true)
		{
			m_bNewMomentarySample = true;
			nRet = nRet | 1;
			if (pdM != NULL){*pdM = dTmp;}
		}		
		CronoEnd (enCronoValues::CRONO_LKFS_MOMENTARY);

		//NEW V2
		CronoStart();
		//Short term
		bRc = m_calcLKFSST.CalcLKFSRT (&m_meanListChannelsPointersShortTerm, &dTmp, false);
		if (bRc == true)
		{
			m_bNewShortTermSample = true;		
			nRet = nRet | 4;

			m_dLastST = dTmp;
			if (pdST != NULL){*pdST = dTmp;}
		}
		else
		{
			if (pdST != NULL){*pdST = m_dLastST;}
		}	
		CronoEnd (enCronoValues::CRONO_LKFS_SHORTTERM);
	}

	return nRet;
}

unsigned long CjocLoud::AddSamples (double *pdSamples, unsigned long ulNumSamples,bool *pNewMomentarySample )
{
	if ((m_state != enSTATE::ST_INI)&&(m_state != enSTATE::ST_STARTED)){throw JOC_Error_(enjocErrors::err_bad_state);}

	m_state = enSTATE::ST_STARTED;

	if ((ulNumSamples > 0)&&(pdSamples != NULL))
	{
		for (int nC = 0 ; nC < m_sChannels ; nC++)
		{
			//TP
			if (m_config.GetTPCalc ()  == true)
			{
				CronoStart();
				m_TPmeters[nC]->AddSamples (&pdSamples[nC], ulNumSamples, m_sChannels);
				CronoEnd (enCronoValues::CRONO_TP_METTER);
			}

			CronoStart();
			m_prefilters[nC]->Filter (&pdSamples[nC], ulNumSamples, m_sChannels);
			CronoEnd (enCronoValues::CRONO_FILTER_PRE);

			CronoStart();
			m_rlbfilters[nC]->Filter (&pdSamples[nC], ulNumSamples, m_sChannels);
			CronoEnd (enCronoValues::CRONO_FILTER_RLB);

			//Momentary: Calc mean sq for each channel every X samples with determinated overlapping factor
			CronoStart();
			m_meansqfiltersMomentary[nC]->AddSamples (&pdSamples[nC], ulNumSamples, m_sChannels);
			CronoEnd (enCronoValues::CRONO_MEAN_FAST_MOMENTARY);

			//ST
			CronoStart();
			//Short Term: Calc mean sq for each channel every X samples with determinated overlapping factor
			m_meansqfiltersShortTerm[nC]->AddSamples (&pdSamples[nC], ulNumSamples, m_sChannels);								
			CronoEnd (enCronoValues::CRONO_MEAN_FAST_SHORTTERM);

			//Plain mean
			CronoStart();
			m_meansqfiltersPlain[nC]->AddSamples (&pdSamples[nC], ulNumSamples, m_sChannels);											
			CronoEnd (enCronoValues::CRONO_MEAN_FAST_PLAIN);

			//LRA
			if (m_config.GetLRACalc () == true)
			{
				CronoStart();
				m_meansqfiltersLRA[nC]->AddSamples (&pdSamples[nC], ulNumSamples, m_sChannels);											
				CronoEnd (enCronoValues::CRONO_MEAN_FAST_LRA);
			}
		}

		//Momentary & Integrated 
		CronoStart();
		long  lRc = m_calcLKFSM.CalcLKFS (&m_meanListChannelsPointersMomentary, &m_MomentaryLoudnesslkfs);
		if (lRc > 0)
			m_bNewMomentarySample = true;
		
		CronoEnd (enCronoValues::CRONO_LKFS_MOMENTARY);

		CronoStart();
		//Short term
		lRc = m_calcLKFSST.CalcLKFS (&m_meanListChannelsPointersShortTerm, &m_ShortTermLoudnesslkfs);
		if (lRc > 0)
			m_bNewShortTermSample = true;		

		m_bNewPlainSample = true;
		CronoEnd (enCronoValues::CRONO_LKFS_SHORTTERM);

		//LRA
		if (m_config.GetLRACalc () == true)
		{
			CronoStart();
			lRc = m_calcLKFSLRA.CalcLKFS (&m_meanListChannelsPointersLRA, &m_LRALoudnesslkfs);
			if (lRc > 0)
				m_bNewLRASample = true;		

			CronoEnd (enCronoValues::CRONO_LKFS_LRA);
		}
	}

	return ulNumSamples;
}

double CjocLoud::GetIntegratedValue(bool bReset)
{
	double dRet = -std::numeric_limits<double>::quiet_NaN();
	
	if ((m_state == enSTATE::ST_STARTED)||(m_state == enSTATE::ST_STOPPED))
	{
		if (m_config.GetIntegrateMode () == CjocLoudConfig::enINTERGRATEMODE::INTEGRATED_MODE_BS17702_EU)
		{//EU
			dRet = m_calcLKFSM.GetIntergratedMessure();
		}
		else
		{//USA
			std::vector<double> vmeanvals;
			for (int nC = 0 ; nC < m_sChannels ; nC++)
			{
				vmeanvals.push_back (m_meansqfiltersPlain[nC]->GetMeanVal ());
			}
			dRet = m_calcLKFSPlain.CalcLKFS (vmeanvals);
		}
	}

	if (bReset == true)
		ResetIntegratedValue();

	return dRet;
}

void CjocLoud::ResetIntegratedValue()
{
	if (m_state > ST_NO_INI)
	{
		if (m_config.GetIntegrateMode () == CjocLoudConfig::enINTERGRATEMODE::INTEGRATED_MODE_BS17702_EU)
		{//EU
			m_calcLKFSM.ResetIntergratedMessure ();
			m_calcLKFSST.ResetIntergratedMessure ();
		}
		else
		{//USA
			m_calcLKFSPlain.ResetIntergratedMessure ();
			for (int nC = 0 ; nC < m_sChannels ; nC++)
			{
				m_meansqfiltersPlain[nC]->ResetMean ();
			}
		}
	}
}

double CjocLoud::GetLRAValue(bool bReset)
{
	double dRet = 0.0;
	
	if ((m_state == enSTATE::ST_STARTED)||(m_state == enSTATE::ST_STOPPED))
	{
		if (m_config.GetLRACalc () == true)
			dRet = m_calcLKFSLRA.GetLRAMessure ();
	}

	if (bReset == true)
		ResetLRAValue();

	return dRet;

}

void CjocLoud::ResetLRAValue()
{
	if (m_state > ST_NO_INI)
	{
		if (m_config.GetLRACalc () == true)
			m_calcLKFSLRA.ResetIntergratedMessure ();
	}
}


void CjocLoud::SaveFile (std::wstring &strFile)
{
	if (strFile.empty () == true){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if ((m_state != enSTATE::ST_STARTED)&&(m_state != enSTATE::ST_STOPPED)){throw JOC_Error_(enjocErrors::err_bad_state );}

	CronoStart();

	std::wstring strOut =  GetLoudDataStr();

	FILE *pFile = NULL;	
	pFile = _wfopen (strFile.c_str (),L"w");
	
	if (pFile == NULL){throw JOC_Error_arg_(enjocErrors::err_creating_file_for_write,strFile);}

	fputs(CjocUtils::ConvertFromwstr (strOut).c_str () ,pFile);

	fclose(pFile);

	CronoEnd (enCronoValues::CRONO_SAVE_LOUD_DATA);
}

std::wstring CjocLoud::GetIntegrateValStr()
{
	std::wstring strOut;

	if ((m_state != enSTATE::ST_STARTED)&&(m_state != enSTATE::ST_STOPPED)){throw JOC_Error_(enjocErrors::err_bad_state );}
	
	char szFormat[32];
	long lPrec = m_config.GetResultPrecission ();
	if (lPrec > 0)
	{
		sprintf(szFormat," .%df",m_config.GetResultPrecission ());
		szFormat[0] = '%';
	}
	else
	{
		strcpy (szFormat,"%f");
	}

	std::string srtUnits = CjocUtils::ConvertFromwstr (m_config.GetResultUnits ());

	char szLine[16384];
	char szAux[512];

	sprintf(szAux,szFormat,GetIntegratedValue());
	sprintf(szLine,"Integrate value (%s) = %s",srtUnits.c_str (), szAux);
	strOut.append (CjocUtils::ConvertFromChar (szLine));

	return strOut;
}

std::wstring CjocLoud::GetLRAValStr()
{
	std::wstring strOut;

	if ((m_state != enSTATE::ST_STARTED)&&(m_state != enSTATE::ST_STOPPED)){throw JOC_Error_(enjocErrors::err_bad_state );}
	
	if (m_config.GetLRACalc () == true)
	{
		char szFormat[32];
		long lPrec = m_config.GetResultPrecission ();
		if (lPrec > 0)
		{
			sprintf(szFormat," .%df",m_config.GetResultPrecission ());
			szFormat[0] = '%';
		}
		else
		{
			strcpy (szFormat,"%f");
		}

		std::string srtUnits = CjocUtils::ConvertFromwstr (m_config.GetResultUnits ());

		char szLine[16384];
		char szAux[512];

		sprintf(szAux,szFormat,GetLRAValue ());
		sprintf(szLine,"Loudness range (LU) = %s",szAux);
		strOut.append (CjocUtils::ConvertFromChar (szLine));
	}

	return strOut;
}

std::wstring CjocLoud::GetLoudDataStr()
{
	std::wstring strOut;

	if ((m_state != enSTATE::ST_STARTED)&&(m_state != enSTATE::ST_STOPPED)){throw JOC_Error_(enjocErrors::err_bad_state );}
	
	char szFormat[32];
	long lPrec = m_config.GetResultPrecission ();
	if (lPrec > 0)
	{
		sprintf(szFormat," .%df",m_config.GetResultPrecission ());
		szFormat[0] = '%';
	}
	else
	{
		strcpy (szFormat,"%f");
	}

	std::string srtUnits = CjocUtils::ConvertFromwstr (m_config.GetResultUnits ());

	char szLine[16384];
	char szAux[512];

	strOut.append (L"# Loudness data\n\n");

	sprintf(szAux,szFormat,GetIntegratedValue());
	sprintf(szLine,"Integrate value (%s) = %s\n\n",srtUnits.c_str (), szAux);
	strOut.append (CjocUtils::ConvertFromChar (szLine));

	if (m_config.GetLRACalc () == true)
	{
		sprintf(szAux,szFormat,GetLRAValue ());
		sprintf(szLine,"Loudness range (LU) = %s\n\n",szAux);
		strOut.append (CjocUtils::ConvertFromChar (szLine));
	
		//LRA 			
		sprintf(szLine,"LRA loudness sample interval (secs) = %f\n",(double)((double)m_lLRAUpdateIntervalSamples)/(double)m_ulSampleRate );
		strOut.append (CjocUtils::ConvertFromChar (szLine));
		sprintf(szLine,"LRA loudness sample interval (audio samples) = %d\n",m_lLRAUpdateIntervalSamples);
		strOut.append (CjocUtils::ConvertFromChar (szLine));
		sprintf(szLine,"LRA loudness (LU) = [");
		strOut.append (CjocUtils::ConvertFromChar (szLine));
		for (unsigned long uln = 0; uln < m_LRALoudnesslkfs.size (); uln++)
		{
			sprintf(szAux,szFormat,m_LRALoudnesslkfs[uln]);
			sprintf(szLine,"%s ",szAux);
			strOut.append (CjocUtils::ConvertFromChar (szLine));
		}
		sprintf(szLine,"]\n\n");
		strOut.append (CjocUtils::ConvertFromChar (szLine));
	}
	
	//Momentary
	sprintf(szLine,"Momentary loudness sample interval (secs) = %f\n",(double)((double)m_lMomentaryUpdateIntervalSamples)/(double)m_ulSampleRate );
	strOut.append (CjocUtils::ConvertFromChar (szLine));
	sprintf(szLine,"Momentary loudness sample interval (audio samples) = %d\n",m_lMomentaryUpdateIntervalSamples);
	strOut.append (CjocUtils::ConvertFromChar (szLine));
	sprintf(szLine,"Momentary loudness (%s) = [",srtUnits.c_str ());
	strOut.append (CjocUtils::ConvertFromChar (szLine));
	for (unsigned long uln = 0; uln < m_MomentaryLoudnesslkfs.size (); uln++)
	{
		sprintf(szAux,szFormat,m_MomentaryLoudnesslkfs[uln]);
		sprintf(szLine,"%s ",szAux);
		strOut.append (CjocUtils::ConvertFromChar (szLine));
	}
	sprintf(szLine,"]\n\n");
	strOut.append (CjocUtils::ConvertFromChar (szLine));

	//Short term
	sprintf(szLine,"Short term loudness sample interval (secs) = %f\n",(double)((double)m_lShortTermUpdateIntervalSamples)/(double)m_ulSampleRate );
	strOut.append (CjocUtils::ConvertFromChar (szLine));
	sprintf(szLine,"Short term loudness sample interval (audio samples) = %d\n",m_lShortTermUpdateIntervalSamples);
	strOut.append (CjocUtils::ConvertFromChar (szLine));
	sprintf(szLine,"Short term loudness (%s) = [",srtUnits.c_str ());
	strOut.append (CjocUtils::ConvertFromChar (szLine));
	for (unsigned long uln = 0; uln < m_ShortTermLoudnesslkfs.size (); uln++)
	{
		sprintf(szAux,szFormat,m_ShortTermLoudnesslkfs[uln]);
		sprintf(szLine,"%s ",szAux);
		strOut.append (CjocUtils::ConvertFromChar (szLine));
	}
	sprintf(szLine,"]\n\n");
	strOut.append (CjocUtils::ConvertFromChar (szLine));

	//TP
	if (m_config.GetTPCalc () == true)
	{
		strOut.append (L"# True Peak values\n\n");

		sprintf(szLine,"Max TP value (dBFS) = [");
		strOut.append (CjocUtils::ConvertFromChar (szLine));
		for (long lC = 0; lC < m_sChannels; lC++)
		{
			double dTPMax = m_TPmeters[lC]->GetMaxTPValue ();
			sprintf(szAux,szFormat,dTPMax);
			sprintf(szLine,"%s ",szAux);
			strOut.append (CjocUtils::ConvertFromChar (szLine));
		}
		sprintf(szLine,"]\n\n");
		strOut.append (CjocUtils::ConvertFromChar (szLine));

		sprintf(szLine,"TP sample interval (secs) = %f\n",(double)((double)m_lTPUpdateIntervalSamples)/(double)m_ulSampleRate );
		strOut.append (CjocUtils::ConvertFromChar (szLine));
		sprintf(szLine,"TP sample interval (audio samples) = %d\n",m_lTPUpdateIntervalSamples );
		strOut.append (CjocUtils::ConvertFromChar (szLine));

		for (long lC = 0; lC < m_sChannels ; lC++)
		{
			sprintf(szLine,"TP values channel %d (dBFS) = [",lC);
			strOut.append (CjocUtils::ConvertFromChar (szLine));
			while (m_TPListChannelsValue[lC]->size ()> 0)
			{
				sprintf(szAux,szFormat,CjocTPLevel::TPFSToDb(m_TPListChannelsValue[lC]->front ()));
				m_TPListChannelsValue[lC]->pop_front();

				sprintf(szLine,"%s ",szAux);
				strOut.append (CjocUtils::ConvertFromChar (szLine));
			}
			sprintf(szLine,"]\n");
			strOut.append (CjocUtils::ConvertFromChar (szLine));
		}
	}

	return strOut;
}

long CjocLoud::GetMomentaryUpdateIntervalSamples()
{
	return m_lMomentaryUpdateIntervalSamples;
}

long CjocLoud::GetShortTermUpdateIntervalSamples()
{
	return m_lShortTermUpdateIntervalSamples;
}

long CjocLoud::GetLRAUpdateIntervalSamples()
{
	return m_lLRAUpdateIntervalSamples;
}
