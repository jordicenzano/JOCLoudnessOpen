// JOCLoudness.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\JOCLoudnessBase\Loud\JocLoudnessConfig.h"
#include"..\JOCLoudnessBase\JOCLoudnessBase.h"

#include <vector>

//Set new version in each modification (Change it in resources too !!!)
#define APP_VERSION L"1.0.4" 

//If demo = 1 only first 15secs will be analyzed
//1 = DEMO; 0 = RELEASE
#define DEMO 0


//Test 
//"..\..\..\TestFiles\EBU\seq-3341-1-16bit.wav" /tin:1 /tout:5 /outaudiofile:"..\..\..\TestFiles\OUT\out_seq-3341-1-16bit.wav" /config:".\Config\custom.ini" /outlouddata:"..\..\..\TestFiles\OUTLoud\out_seq-3341-1-16bit.txt" /logsfile:"..\..\..\TestFiles\LOGS\JOCLoudness.log"

//Funtions 
//void SetConfigData (CJocLoudnessConfig &Config, CjocLoudConfig *pcfgLoud, enAPP_MODE *pAppMode);
CjocWav::WAVSAVEDATA IniWavOutFile (short sChannels, unsigned long ulFs);
std::wstring GetTimePerformanceValuesStr (CjocLoud &Loud);

int _tmain(int argc, _TCHAR* argv[])
{
	CjocLog log;
	//CjocCrono crono;
	CJocLoudnessConfig config;

	CJocLoudnessConfig::enAPP_MODE AppMode = CJocLoudnessConfig::enAPP_MODE::APPMODE_LOUD;
		
	if (DEMO > 0){printf ("THIS IS A DEMO VERSION, ONLY 15secs OF THE INPUT FILE WILL BE ANALYSED. To upgrade www.jordicenzano.name\n\n");}
	
	if (argc < 2)
	{
		printf ("******* JOCLoudness by Jordi Cenzano (www.jordicenzano.name) ********\n");
		printf ("You have to use: JOCLoudness.exe infile.wav [/tin:in point in secs] [/tout:out point in secs] [/outaudiofile:outfile.wav] [/config:configfile.ini] [/outlouddata:out data file] [/logsfile:logsloud.log] [/verbose]\n\n");
		printf ("For example:\nJOCLoudness.exe totest.wav /tin:1 /tout:5 /outaudiofile:outpartialfile.wav /config:configcustom.ini /outlouddata:loud.txt /logsfile:logsloud.log\n");
		printf ("Or:\nJOCLoudness.exe \"c:\\audiotest\\totest.wav\" /tin:1 /tout:5 /outaudiofile:\"c:\\out\\outpartialfile.wav\" /config:configcustom.ini /outlouddata:\"c:\\data loud\\loud.txt\" /logsfile:\"c:\\logsloud.log\" /verbose\n");

		printf ("\nMore information in www.jordicenzano.name\n");

		return 0;
	}

	try
	{
		//Set default Log file name (app path)	
		std::wstring strLogsFilePathDefault = CjocUtils::GetFileWithoutExt ((std::wstring)argv[0]) + L".log";

		//Get File In
		std::wstring strFileIN = argv[1];

		//Get Params
		std::vector<std::wstring> strParamArray;	
		for (int n = 1; n < argc; n++)
			strParamArray.push_back (argv[n]);

		bool bVerbose = false;
		bool bLogTimeStamp = true;
		bool bExistOutFile = false;
		double dTin = -1;
		double dTout = -1;
		std::wstring strLogsFilePath = strLogsFilePathDefault;
		std::wstring strOutAudioFile;
		bool bSaveAudioFile = false;
		std::wstring strConfigFile;
		bool bLoadConfigFile = false;
		std::wstring strOutLoudFile;

		bool bFound = CjocUtils::FindParamD(strParamArray,L"/tin:",&dTin);
		bFound = CjocUtils::FindParamD(strParamArray,L"/tout:",&dTout);
		bFound = CjocUtils::FindParamStr (strParamArray,L"/logsfile:",&strLogsFilePath);
		bSaveAudioFile = CjocUtils::FindParamStr(strParamArray,L"/outaudiofile:",&strOutAudioFile);
		bLoadConfigFile = CjocUtils::FindParamStr(strParamArray,L"/config:",&strConfigFile);
		bExistOutFile = CjocUtils::FindParamStr(strParamArray,L"/outlouddata:",&strOutLoudFile);
		bVerbose = CjocUtils::FindParamStr(strParamArray,L"/verbose",NULL);

		//Ini logs
		log.Initialize (strLogsFilePath , MAXLOGSIZE);

		if (DEMO > 0)
		{
			if (dTin >= 0)
			{
				dTout = dTin + 15.0;
			}
			else
			{
				dTout = 15.0;
			}

			log.Append ((std::wstring)L"THIS IS A DEMO VERSION, ONLY 15secs OF THE INPUT FILE WILL BE ANALYSED. To upgrade www.jordicenzano.name",bLogTimeStamp,bVerbose);
		}

		log.Append ((std::wstring)L"Software version: ",(std::wstring)APP_VERSION, bLogTimeStamp);

		log.Append ((std::wstring)L"File to load: ",strFileIN,bLogTimeStamp,bVerbose);
		log.Append ((std::wstring)L"Logs file: ",strLogsFilePath,bLogTimeStamp,bVerbose);
		log.Append ((std::wstring)L"Config file: ",strConfigFile,bLogTimeStamp,bVerbose);
		log.Append ((std::wstring)L"Audio out data file: ",strOutLoudFile,bLogTimeStamp,bVerbose);
		
		CjocLoudConfig cfgLoud;
		//Set default EU config
		cfgLoud.IniR128 ();

		//Load config file 		
		if ((strConfigFile.empty () == false)&&(bLoadConfigFile == true))
		{
			log.Append ((std::wstring)L"Loading config file: ", strConfigFile,bLogTimeStamp,bVerbose);
	
			config.Load (strConfigFile);
			
			log.Append ((std::wstring)L"Loaded config values:\n", config.DumpAllValues (),bLogTimeStamp,bVerbose);

			CJocLoudnessConfig::SetConfigData (config, &cfgLoud, &AppMode);
		}

		std::wstring strAction = L"Loud";
		if (AppMode == CJocLoudnessConfig::enAPP_MODE::APPMODE_FILTER_LPF)
			strAction = L"Filter LPF";
		else if (AppMode == CJocLoudnessConfig::enAPP_MODE::APPMODE_UPSAMPLE)
			strAction = L"Upsample";

		log.Append ((std::wstring)L"Action to do: ",strAction,bLogTimeStamp,bVerbose);
		log.Append ((std::wstring)L"Preset to use: ",cfgLoud.GetPresetModeStr (),bLogTimeStamp,bVerbose);

		CjocWav wavIn;
		wavIn.LoadFile (strFileIN); 
		wavIn.SetInOutPoints (dTin,dTout);

		CjocWav::WAVLOADDATA wavINDta =  wavIn.GetWaveLoadData ();

		//Show file data
		log.Append ((std::wstring)L"Loaded file: ",strFileIN,bLogTimeStamp,bVerbose);		
		log.Append ((std::wstring)L"File info: ",wavIn.GetWaveLoadDataStr(),bLogTimeStamp,bVerbose);

		CjocLoud Loud;

		Loud.CronoActivate (true);

		log.Append ((std::wstring)L"Initializing ...",bLogTimeStamp);		

		std::wstring strCronoValues;

		if (AppMode == CJocLoudnessConfig::enAPP_MODE::APPMODE_LOUD)
		{
			CjocWav *pwavOut = NULL;
			CjocWav::WAVSAVEDATA WavOutData;

			if ((bSaveAudioFile== true)&&(strOutAudioFile.empty ()==false))
			{
				WavOutData = IniWavOutFile (wavINDta.sChannels,wavINDta.ulSampleRate);	
		
				pwavOut = new CjocWav(WavOutData);
				if (pwavOut == NULL){throw JOC_Error_(enjocErrors::err_out_of_memory);}
			
				pwavOut->SaveFile (strOutAudioFile);		
			}

			Loud.SetConfig (cfgLoud);

			Loud.Ini (wavINDta.ulSampleRate ,wavINDta.sChannels); 

			log.Append ((std::wstring)L"Entering in mode loud meter ...",bLogTimeStamp);		

			long lBlockSizeInSamplesPerChannel = cfgLoud.GetReadBlockSizeInSamplesPerChannel();
			double *pdData = (double*)malloc (sizeof (double)*lBlockSizeInSamplesPerChannel*wavINDta.sChannels);
			long lReaded  = 1;
			long lWritten = 1;
			long lOffsetSamples = 0;

			log.Append ((std::wstring)L"Processing file: ", strFileIN, bLogTimeStamp);		
			while (lReaded > 0)
			{
				Loud.CronoStart ();
				lReaded = wavIn.GetAudioDataDoubleAllChannels (pdData,lOffsetSamples,lBlockSizeInSamplesPerChannel );		
				Loud.CronoEnd (CjocCrono::enCronoValues::CRONO_WAV_LOAD);

				if (bSaveAudioFile == true)
				{
					Loud.CronoStart ();
					lWritten = pwavOut->SetAudioDataDoubleAllChannels (pdData, lOffsetSamples, lReaded/WavOutData.sChannels);
					Loud.CronoEnd (CjocCrono::enCronoValues::CRONO_WAV_SAVE);

					if (lWritten != lReaded)
					{//ERROR
						if (pwavOut == NULL){throw JOC_Error_(enjocErrors::err_writingAudioOutFile);}
					}
				}

				Loud.AddSamples (pdData,lReaded);

				lOffsetSamples = lOffsetSamples + lReaded;
			}

			if (pdData != NULL)
			{
				free (pdData);
				pdData = NULL;
			}

			if (bExistOutFile == true)
			{
				log.Append ((std::wstring)L"Saving loud data into: ", (std::wstring)strOutLoudFile,bLogTimeStamp);
				//Save data into out file
				Loud.SaveFile (strOutLoudFile);
			}
			else
			{
				std::wstring strOut = Loud.GetIntegrateValStr ();
				log.Append (strOut,bLogTimeStamp);
				strOut = Loud.GetLRAValStr ();
				log.Append (strOut,bLogTimeStamp);
				strOut.append (L"To get extra information set /outlouddata modifier!!!");
			}

			if (bSaveAudioFile == true)
				log.Append ((std::wstring)L"Number of truncated samples in saved file (overflow): ", (std::wstring)CjocUtils::lToStr (pwavOut->GetNumSamplesTruncated ()),bLogTimeStamp);	

			strCronoValues = GetTimePerformanceValuesStr (Loud);

			if (pwavOut != NULL)
			{
				pwavOut->CloseFile ();
				delete pwavOut;
				pwavOut = NULL;
			}
		}
		else if (AppMode == CJocLoudnessConfig::enAPP_MODE::APPMODE_FILTER_LPF)
		{
			CjocWav *pwavOut = NULL;
			CjocWav::WAVSAVEDATA WavOutData;

			log.Append ((std::wstring)L"Entering in mode filter (TEST!!) ...",bLogTimeStamp);		

			if ((bSaveAudioFile== true)&&(strOutAudioFile.empty ()==false))
			{
				WavOutData = IniWavOutFile (wavINDta.sChannels,wavINDta.ulSampleRate);	
		
				pwavOut = new CjocWav(WavOutData);
				if (pwavOut == NULL){throw JOC_Error_(enjocErrors::err_out_of_memory);}
			
				pwavOut->SaveFile (strOutAudioFile);		
			}

			CjocUpSampleFilter upSFilter;

			long lFcFrec = cfgLoud.GetLPFCutOff ();

			upSFilter.SetFrecCut(wavINDta.ulSampleRate ,lFcFrec);

			long lBlockSizeInSamplesPerChannel = cfgLoud.GetReadBlockSizeInSamplesPerChannel();
			double *pdData = (double*)malloc (sizeof (double)*lBlockSizeInSamplesPerChannel*wavINDta.sChannels);
			long lReaded  = 1;
			long lWritten = 1;
			long lOffsetSamples = 0;

			log.Append ((std::wstring)L"Processing (filter) ...",bLogTimeStamp);		
			while (lReaded > 0)
			{
				Loud.CronoStart ();
				lReaded = wavIn.GetAudioDataDoubleAllChannels (pdData,lOffsetSamples,lBlockSizeInSamplesPerChannel );		
				Loud.CronoEnd (CjocCrono::enCronoValues::CRONO_WAV_LOAD);

				Loud.CronoStart ();
				for (int nCh = 0; nCh < wavINDta.sChannels; nCh++)
					upSFilter.Filter (&pdData[nCh],lBlockSizeInSamplesPerChannel, wavINDta.sChannels);

				Loud.CronoEnd (CjocCrono::enCronoValues::CRONO_TEST_LPF);

				if (bSaveAudioFile == true)
				{
					Loud.CronoStart ();
					lWritten = pwavOut->SetAudioDataDoubleAllChannels (pdData, lOffsetSamples, lReaded/WavOutData.sChannels);
					Loud.CronoEnd (CjocCrono::enCronoValues::CRONO_WAV_SAVE);

					if (lWritten != lReaded)
					{//ERROR
						if (pwavOut == NULL){throw JOC_Error_(enjocErrors::err_writingAudioOutFile);}
					}
				}
			
				lOffsetSamples = lOffsetSamples + lReaded;
			}

			if (bSaveAudioFile == true)
				log.Append ((std::wstring)L"Number of truncated samples in saved file (overflow): ", (std::wstring)CjocUtils::lToStr (pwavOut->GetNumSamplesTruncated ()),bLogTimeStamp);	

			strCronoValues = GetTimePerformanceValuesStr (Loud);

			if (pwavOut != NULL)
			{
				pwavOut->CloseFile ();
				delete pwavOut;
				pwavOut = NULL;
			}
		}
		else if (AppMode == CJocLoudnessConfig::enAPP_MODE::APPMODE_UPSAMPLE)
		{
			long lUpSampleFactor= cfgLoud.GetUpsamplefactor ();
			CjocWav *pwavOut = NULL;
			CjocWav::WAVSAVEDATA WavOutData;

			log.Append ((std::wstring)L"Entering in upsample (TEST!!) ...",bLogTimeStamp);		

			if (wavINDta.sChannels > 1){throw JOC_Error_(enjocErrors::err_wavNumberofChannelsNotSupported);}

			if ((bSaveAudioFile== true)&&(strOutAudioFile.empty ()==false))
			{
				WavOutData = IniWavOutFile (wavINDta.sChannels,wavINDta.ulSampleRate);	
				WavOutData.ulSampleRate = WavOutData.ulSampleRate * lUpSampleFactor;
		
				pwavOut = new CjocWav(WavOutData);
				if (pwavOut == NULL){throw JOC_Error_(enjocErrors::err_out_of_memory);}
			
				pwavOut->SaveFile (strOutAudioFile);		
			}

			CjocUpSampleFilter upSFilter;

			upSFilter.SetFreqSampling(wavINDta.ulSampleRate , lUpSampleFactor);

			long lBlockSizeInSamplesPerChannel = cfgLoud.GetReadBlockSizeInSamplesPerChannel();
			double *pdData = (double*)malloc (sizeof (double)*lBlockSizeInSamplesPerChannel * wavINDta.sChannels);
			double *pdDataUpsampled = (double*)malloc (sizeof (double)*lBlockSizeInSamplesPerChannel * wavINDta.sChannels * lUpSampleFactor);
			long lReaded  = 1;
			long lWritten = 1;
			long lOffsetSamples = 0;

			log.Append ((std::wstring)L"Processing (filter) ...",bLogTimeStamp);		
			while (lReaded > 0)
			{
				
				Loud.CronoStart ();
				lReaded = wavIn.GetAudioDataDoubleAllChannels (pdData,lOffsetSamples,lBlockSizeInSamplesPerChannel);		
				Loud.CronoEnd (CjocCrono::enCronoValues::CRONO_WAV_LOAD);

				Loud.CronoStart ();
				//Upsample
				for (unsigned long ulIndex = 0; ulIndex < (lBlockSizeInSamplesPerChannel * wavINDta.sChannels);ulIndex++)
				{
					for (int nUpsF = 0; nUpsF < lUpSampleFactor ;nUpsF++)
					{
						if (nUpsF == 0)
							pdDataUpsampled[ulIndex * lUpSampleFactor + nUpsF] = pdData[ulIndex];
						else
							pdDataUpsampled[ulIndex * lUpSampleFactor + nUpsF] = 0.0;
					}
				}
				Loud.CronoEnd (CjocCrono::enCronoValues::CRONO_TEST_UPSAMPLE_INSERT);

				//only 1ch allowed
				Loud.CronoStart ();
				upSFilter.Filter (pdDataUpsampled,lBlockSizeInSamplesPerChannel*lUpSampleFactor, wavINDta.sChannels);			
				Loud.CronoEnd (CjocCrono::enCronoValues::CRONO_TEST_UPSAMPLE_LPF);

				if (bSaveAudioFile == true)
				{
					Loud.CronoStart ();
					lWritten = pwavOut->SetAudioDataDoubleAllChannels (pdDataUpsampled, lOffsetSamples*lUpSampleFactor, (lReaded*lUpSampleFactor)/WavOutData.sChannels);
					Loud.CronoEnd (CjocCrono::enCronoValues::CRONO_WAV_SAVE);

					if (lWritten != (lReaded*lUpSampleFactor))
					{//ERROR
						if (pwavOut == NULL){throw JOC_Error_(enjocErrors::err_writingAudioOutFile);}
					}
				}
			
				lOffsetSamples = lOffsetSamples + lReaded;
			}

			if (bSaveAudioFile == true)
				log.Append ((std::wstring)L"Number of truncated samples in saved file (overflow): ", (std::wstring)CjocUtils::lToStr (pwavOut->GetNumSamplesTruncated ()),bLogTimeStamp);	

			strCronoValues = GetTimePerformanceValuesStr (Loud);

			if (pwavOut != NULL)
			{
				pwavOut->CloseFile ();
				delete pwavOut;
				pwavOut = NULL;
			}

			if (pdData != NULL)
			{
				free (pdData);
				pdData = NULL;
			}
			if (pdDataUpsampled != NULL)
			{
				free (pdDataUpsampled);
				pdDataUpsampled = NULL;
			}

		}

		log.Append ((std::wstring)L"Cleaning ...");		

		wavIn.CloseFile ();

		//std::wstring strCronoValues = GetTimePerformanceValuesStr (crono, wavIn, pwavOut, Loud);
		log.Append ((std::wstring)L"Execution times:\n",strCronoValues,bLogTimeStamp,bVerbose);
			
		//TODO: Provar els fitxer de config A85		
		//TODO: ?? Carregar-se el transitori del principi ???????
		//Improvements
		//TODO: Accelerar el LPF True peak		

		//OK: OJO EL TEST DELS 6 canals!!!!!! (cert EBU)!!!
		//OK: implementation of LRA (3342)		
		//OK: True peak opcional
		//OK True peak: OverSample + FILTER!!!		
		//OK Documentar parametres config TP
		//OK Fer un mode no verbose i trure el res per pantalla
		//OK Posar dades del interval de les mostres de momentary si ST en el fitxer de sortida
		//OK Treure el num de samples truncades del savewav
		//OK - Incorporar el nun de versio en els logs, si no no puc localitzar el error (nº de linia)
		//OK save file, temps, repasar err exepctions etc....			
		//OK Incorporar el nun de versio en el file la sortida, si no no puc localitzar el error (nº de linia)
		//OK Fer el mode DEMO dels 15 secs

		log.Append ((std::wstring)L"End\n");	
	}
	catch (CjocException ex)
	{
		printf ("ERROR: %s\n",ex.what ());

		if (log.IsInitialized() == true)
		{
			log.Append (CjocUtils::ConvertFromChar(ex.what ()));
		}
	}
	
	if (DEMO > 0){printf ("THIS IS A DEMO VERSION, ONLY 15secs OF THE INPUT FILE ARE ANALYSED. To upgrade www.jordicenzano.name \n\n");}

	return 0;
}
	
CjocWav::WAVSAVEDATA IniWavOutFile (short sChannels, unsigned long ulFs)
{
	CjocWav::WAVSAVEDATA WavOutData;

	WavOutData.sBitsPerSample = 32;
	WavOutData.sChannels = sChannels;
	WavOutData.sFormatTag = CjocWav::enWavType::WAV_TYPE_PCM_IEEE_FLOAT;
	WavOutData.ulSampleRate = ulFs;

	return WavOutData;
}

std::wstring GetTimePerformanceValuesStr (CjocLoud &Loud)
{
	//Get Crono values 
	CjocCrono::mapStrTime CronoStrTimes;

	//Index 0 is total time
	long lTime = Loud.CronoGetTotalTime ();
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"0 APP total (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_WAV_LOAD);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"1 WAV load (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_WAV_SAVE);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"2 WAV save (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_FILTER_PRE);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"3 Pre filter (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_FILTER_RLB);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"4 RLB filter (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_MEAN_FAST_MOMENTARY);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"5 Mean momentary (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_MEAN_FAST_SHORTTERM);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"6 Mean short term (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_MEAN_FAST_PLAIN);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"7 Mean plain (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_MEAN_FAST_LRA);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"8 Mean LRA (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_LKFS_MOMENTARY);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"9 LKFS calc momentary (sum channels and calc log value) (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_LKFS_SHORTTERM);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"10 LKFS calc short term (sum channels and calc log value) (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_SAVE_LOUD_DATA);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"11 LKFS Data save (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_TP_METTER);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"12 True peak meter (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_LKFS_LRA);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"13 LKFS calc LRA (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_TEST_LPF);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"99 Test LPF (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_TEST_UPSAMPLE_INSERT);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"99 Test upsample insert (ms)",lTime));

	lTime = Loud.CronoGetGapTime (CjocCrono::CRONO_TEST_UPSAMPLE_LPF);
	CronoStrTimes.insert (std::pair <std::wstring ,long>(L"99 Test upsample insert (ms)",lTime));

	return CjocCrono::GetStrTimes (CronoStrTimes);
}