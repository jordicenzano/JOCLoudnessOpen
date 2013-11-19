#include "CjocWav.h"
#include "..\baseclasses\CjocException.h"
#include "..\BaseClasses\CjocUtils.h"

CjocWav::CjocWav(void)
{
	IniAllData();
} 

CjocWav::CjocWav(WAVSAVEDATA wavData)
{
	IniAllData();
	m_WavSaveData = wavData;
	m_bSave = true;
}

void CjocWav::IniAllData()
{
	m_fp = NULL; 
	memset (&m_WavLoadData,0,sizeof (INTERNALLOADDATA));
	m_WavLoadData.dTin = -1;
	m_WavLoadData.dTout = -1;

	memset (&m_WavSaveData,0,sizeof (WAVSAVEDATA));

	m_lLoadOffsetStartReadAudioData = 0;
	m_lLoadOffsetStopReadAudioData = 0;

	m_lLoadOffsetStartAudioData = 0;
	m_lSaveOffsetStartAudioData = 0;
	m_lLoadFileDataSize = 0;
	m_lSaveFileSoundSize  = 0;
	m_bSave = false;

	m_ulTruncatedSamples = 0;
}

void CjocWav::LoadFile (std::wstring &strFileName ,double dTin, double dTout)
{
	ClearAll ();

	int nRc = 0;
	char cid[4];
	memset (cid,0,sizeof (char)*4);

	m_fp = _wfopen (strFileName.c_str (),L"rb"); 
	if (m_fp == NULL){throw JOC_Error_arg_(enjocErrors::err_opening_file ,strFileName.c_str ());}

	nRc = fread(cid, sizeof(unsigned char), 4, m_fp); //read in first four bytes 
	if (nRc != 4)
	{
		fclose(m_fp);
		throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
	}
	if (strncmp(cid, "RIFF",4) != 0) 
	{
		fclose(m_fp);
		throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
	}

	nRc = fread(&m_WavLoadData.ulChunksSizeTotal, sizeof(unsigned long), 1, m_fp); //4 + chunks data
	if (nRc != 1)
	{
		fclose(m_fp);
		throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
	}

	nRc = fread(cid, sizeof(unsigned char), 4, m_fp); //read in 4 byte string now 
	if (nRc != 4)
	{
		fclose(m_fp);
		throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
	}
	if (strncmp(cid,"WAVE",4) != 0) 
	{
		fclose(m_fp);
		throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
	}

	nRc = fread(cid, sizeof(unsigned char), 4, m_fp); //read in 4 bytes "fmt "; 
	if (nRc != 4)
	{
		fclose(m_fp);
		throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
	}
	if (strncmp(cid,"fmt ",4) != 0) 
	{
		fclose(m_fp);
		throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
	}    

	nRc = fread(&m_WavLoadData.ulChunkSize, sizeof(unsigned long),1,m_fp); 
	if (nRc != 1)
	{
		fclose(m_fp);
		throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
	}
	if (m_WavLoadData.ulChunkSize == 16) 
	{//WAV Format 16
		//FORMAT
		nRc = fread(&m_WavLoadData.sFormatTag, sizeof(short), 1, m_fp); //Only PCM accepted (1) or Float (3)
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}
		if ((m_WavLoadData.sFormatTag != 1) && (m_WavLoadData.sFormatTag != 3))
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_wavunsupportedBitFormat,strFileName.c_str ());
		}

		//Nº of channels
		nRc = fread(&m_WavLoadData.sChannels, sizeof(short),1,m_fp); //1 mono, 2 stereo 
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		//Sample rate
		nRc = fread(&m_WavLoadData.ulSampleRate, sizeof(unsigned long), 1, m_fp); //like 44100, 22050, etc... 
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		nRc = fread(&m_WavLoadData.ulAvgBytesSec,sizeof(unsigned long), 1, m_fp);
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

	    nRc = fread(&m_WavLoadData.sBlockAlign, sizeof(short), 1, m_fp);
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		//Bits  per sample
		nRc = fread(&m_WavLoadData.sBitsPerSample, sizeof(short), 1, m_fp); //8 bit or 16 bit file
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		//Only float 32 accpeted
		if ((m_WavLoadData.sFormatTag == 3)&&(m_WavLoadData.sBitsPerSample != 32))
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_wavunsupportedFloatFormat,strFileName.c_str ());
		}

		//Only 8,16,24 PCM accepted
		if (m_WavLoadData.sFormatTag == 1) 
		{
			if ((m_WavLoadData.sBitsPerSample != 8)&&(m_WavLoadData.sBitsPerSample != 16)&&(m_WavLoadData.sBitsPerSample != 24))
			{
				fclose(m_fp);
				throw JOC_Error_arg_(enjocErrors::err_wavunsupportedPCMFormat,strFileName.c_str ());
			}
		}
	}
	else if (m_WavLoadData.ulChunkSize == 18) 
	{//WAV Format 18
		//FORMAT
		nRc = fread(&m_WavLoadData.sFormatTag, sizeof(short), 1, m_fp); //Only PCM accepted (1) or Float (3)
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}
		if ((m_WavLoadData.sFormatTag != 1) && (m_WavLoadData.sFormatTag != 3))
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_wavunsupportedBitFormat,strFileName.c_str ());
		}

		//Nº of channels
		nRc = fread(&m_WavLoadData.sChannels, sizeof(short),1,m_fp); //1 mono, 2 stereo 
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		//Sample rate
		nRc = fread(&m_WavLoadData.ulSampleRate, sizeof(unsigned long), 1, m_fp); //like 44100, 22050, etc... 
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		nRc = fread(&m_WavLoadData.ulAvgBytesSec,sizeof(unsigned long), 1, m_fp);
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

	    nRc = fread(&m_WavLoadData.sBlockAlign, sizeof(short), 1, m_fp);
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		//Bits  per sample
		nRc = fread(&m_WavLoadData.sBitsPerSample, sizeof(short), 1, m_fp); //8 bit or 16 bit file
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		//Size of extension = 0
		short sTmp = 0;
		nRc = fread(&sTmp, sizeof(short), 1, m_fp);
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}
		if (sTmp != 0)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}		
	}
	else if (m_WavLoadData.ulChunkSize == 40) 
	{//WAVEEX Format 40 
		//FORMAT
		nRc = fread(&m_WavLoadData.sFormatTag, sizeof(short), 1, m_fp); //Only PCM accepted (1) or Float (3)
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}
		if (m_WavLoadData.sFormatTag != -2)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		//Nº of channels
		nRc = fread(&m_WavLoadData.sChannels, sizeof(short),1,m_fp); //1 mono, 2 stereo 
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		//Sample rate
		nRc = fread(&m_WavLoadData.ulSampleRate, sizeof(unsigned long), 1, m_fp); //like 44100, 22050, etc... 
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		nRc = fread(&m_WavLoadData.ulAvgBytesSec,sizeof(unsigned long), 1, m_fp);
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

	    nRc = fread(&m_WavLoadData.sBlockAlign, sizeof(short), 1, m_fp);
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		//Bits  per sample
		nRc = fread(&m_WavLoadData.sBitsPerSample, sizeof(short), 1, m_fp); //8 bit or 16 bit file
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		//Size of extension
		short sTmp;
		nRc = fread(&sTmp, sizeof(short), 1, m_fp); //22 bytes
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}
		if (sTmp != 22)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		//Valid bits per sample
		nRc = fread(&sTmp, sizeof(short), 1, m_fp); 
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}
		if (sTmp != m_WavLoadData.sBitsPerSample)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		//Channel mask (16bytes)
		unsigned long lTmp;
		nRc = fread(&lTmp, sizeof(unsigned long), 1, m_fp); 
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		//Subformat (16bytes) - 2 first 
		unsigned char szTmp[16];
		nRc = fread(szTmp, sizeof(unsigned char), 16, m_fp); 
		if (nRc != 16)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}
		sTmp = 0;
		sTmp = szTmp[1];
		sTmp = sTmp<<8;
		sTmp = sTmp + szTmp[0];
		
		m_WavLoadData.sFormatTag = sTmp;

		//Only float 32 accpeted
		if ((m_WavLoadData.sFormatTag == 3)&&(m_WavLoadData.sBitsPerSample != 32))
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_wavunsupportedFloatFormat,strFileName.c_str ());
		}

		//Only 8,16,24 PCM accepted
		if (m_WavLoadData.sFormatTag == 1) 
		{
			if ((m_WavLoadData.sBitsPerSample != 8)&&(m_WavLoadData.sBitsPerSample != 16)&&(m_WavLoadData.sBitsPerSample != 24))
			{
				fclose(m_fp);
				throw JOC_Error_arg_(enjocErrors::err_wavunsupportedPCMFormat,strFileName.c_str ());
			}
		}

		//Read extra chunk data
		nRc = fread(cid, sizeof(unsigned char), 4, m_fp); //read in 4 bytes "fact"; 
		if (nRc != 4)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}
		if (strncmp(cid,"fact",4) != 0) 
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}    

		//Size = 4
		nRc = fread(&lTmp, sizeof(unsigned long), 1, m_fp); 
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}
		if (lTmp != 4)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}

		//Samplelenght
		nRc = fread(&lTmp, sizeof(unsigned long), 1, m_fp); 
		if (nRc != 1)
		{
			fclose(m_fp);
			throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
		}
	}
	else
	{
		fclose(m_fp);
		throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
	}

	//START CHUNK DATA	
	nRc = fread(cid, sizeof(unsigned char), 4, m_fp); //read in 4 bytes "data"; 
	if (nRc != 4)
	{
		fclose(m_fp);
		throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
	}
	if (strncmp(cid,"data",4) != 0) 
	{
		fclose(m_fp);
		throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
	}    

	nRc = fread(&m_WavLoadData.ulDataSize, sizeof(unsigned long), 1, m_fp); //Data size
	if (nRc != 1)
	{
		fclose(m_fp);
		throw JOC_Error_arg_(enjocErrors::err_loadingwavfile,strFileName.c_str ());
	}

	m_lLoadOffsetStartAudioData = ftell(m_fp);
	m_lLoadFileDataSize = m_lLoadOffsetStartAudioData + m_WavLoadData.ulDataSize;

	m_lLoadOffsetStartReadAudioData = m_lLoadOffsetStartAudioData;
	m_lLoadOffsetStopReadAudioData = m_lLoadFileDataSize;
}

void CjocWav::SetInOutPoints(double dTin, double dTout)
{
	bool bSetIn = true;
	bool bSetOut = true;

	if ((m_WavLoadData.ulSampleRate <= 0)||(m_WavLoadData.sBlockAlign <= 0)){throw JOC_Error_(enjocErrors::err_bad_state);}

	if (((dTout * 1000.0) > GetAudioDurationMS ())||(dTout < 0))
		bSetOut = false;
		
	if (((dTin * 1000.0) > GetAudioDurationMS ())||(dTin < 0))
		bSetIn = false;

	if ((bSetIn == true)&&(bSetOut == true))
	{
		if (dTin >= dTout){throw JOC_Error_(enjocErrors::err_bad_arguments);}
	}

	if (bSetIn == true)
	{
		m_WavLoadData.dTin = dTin;
		unsigned long ulSampleIn = (unsigned long) ((double)m_WavLoadData.ulSampleRate * m_WavLoadData.dTin);
		unsigned long lbytesIn = m_WavLoadData.sBlockAlign * ulSampleIn;

		if (m_lLoadOffsetStartAudioData + lbytesIn < m_lLoadFileDataSize)
			m_lLoadOffsetStartReadAudioData = m_lLoadOffsetStartAudioData + lbytesIn;
	}

	if (bSetOut == true)
	{
		m_WavLoadData.dTout = dTout;
		unsigned long ulSampleOut = (unsigned long) ((double)m_WavLoadData.ulSampleRate * m_WavLoadData.dTout);
		unsigned long lbytesOut = m_WavLoadData.sBlockAlign * ulSampleOut;

		if (m_lLoadOffsetStartAudioData + lbytesOut < m_lLoadFileDataSize)
			m_lLoadOffsetStopReadAudioData = m_lLoadOffsetStartAudioData + lbytesOut;
	}
}

double CjocWav::GetInPoint()
{
	return m_WavLoadData.dTin;
}

double CjocWav::GetOutPoint()
{
	return m_WavLoadData.dTout;
}


/*long CjocWav::GetAudioDataDouble(double *pdAudioData, int nChannel, long lOffsetSamples, long lAudioDataSamples, bool bNormalized)
{
	long lRc = 0;

	if (lAudioDataSamples <= 0){throw JOC_Error_(enjocErrors::err_bad_arguments);}
	
	if (pdAudioData == NULL){throw JOC_Error_(enjocErrors::err_bad_arguments);}
	
	if (nChannel >= m_WavLoadData.sChannels){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if (lOffsetSamples < 0){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if (m_fp == NULL){throw JOC_Error_(enjocErrors::err_bad_state);}

	long lBytesPerSample = m_WavLoadData.sBitsPerSample / 8;

	//m_WavLoadData.sBlockAlign  = nº channels * bytespersample
	long lGotoPosBytes = lOffsetSamples * (m_WavLoadData.sBlockAlign) + (nChannel*m_WavLoadData.sBitsPerSample /8);

	bool bExit = false;
	unsigned char pdata[16];	
	long lSamplesReaded = 0;
	while (bExit == false)
	{		
		lRc = GetAudioData (pdata,lGotoPosBytes,lBytesPerSample);

		if (lRc >= lBytesPerSample)
		{
			pdAudioData[lSamplesReaded] = GetSampleValue(pdata,0,lBytesPerSample, (CjocWav::enWavType)m_WavLoadData.sFormatTag,bNormalized);
			lSamplesReaded++;
			lGotoPosBytes =lGotoPosBytes + m_WavLoadData.sBlockAlign;
			
			if (lSamplesReaded >= lAudioDataSamples)
				bExit = true;
		}
		else
		{
			bExit = true;
		}
	}

	return lSamplesReaded;
}*/

long CjocWav::GetAudioDataDoubleAllChannels(double *pdAudioData, long lOffsetSamples, long lAudioDataSamplesPerChannel, bool bNormalized)
{
	long lRc = 0;

	if (lAudioDataSamplesPerChannel <= 0){throw JOC_Error_(enjocErrors::err_bad_arguments);}
	
	if (pdAudioData == NULL){throw JOC_Error_(enjocErrors::err_bad_arguments);}
			
	if (lOffsetSamples < 0){throw JOC_Error_(enjocErrors::err_bad_arguments);}
		
	if (m_fp == NULL){throw JOC_Error_(enjocErrors::err_bad_state);}

	long lBytesPerSample = m_WavLoadData.sBitsPerSample / 8;
	 
	//Channel 0
	long lGotoPosBytes = lOffsetSamples * lBytesPerSample;//m_WavLoadData.sBlockAlign;

	bool bExit = false;
	unsigned char pdata[2048];	
	long lSamplesReaded = 0;
	long lChannel = 0;
	long lBytesperAllChannels = lBytesPerSample * m_WavLoadData.sChannels;
	if (lBytesperAllChannels > 2048)
		throw JOC_Error_(enjocErrors::err_readingwavdata);

	while (bExit == false)
	{		
		lRc = GetAudioData (pdata,lGotoPosBytes,lBytesperAllChannels);

		if (lRc >= lBytesperAllChannels)
		{
			for (long nC = 0; nC < m_WavLoadData.sChannels;nC++)
				pdAudioData[lSamplesReaded+nC] = GetSampleValue(pdata,nC * lBytesPerSample, lBytesPerSample, (CjocWav::enWavType)m_WavLoadData.sFormatTag, bNormalized);

			lSamplesReaded = lSamplesReaded + m_WavLoadData.sChannels;

			lGotoPosBytes =lGotoPosBytes + lBytesperAllChannels;

			//Check if samples/channel reades reached the limit
			if ((lSamplesReaded/m_WavLoadData.sChannels) >= lAudioDataSamplesPerChannel)
				bExit = true;
		}
		else
		{
			bExit = true;
		}
	}

	return lSamplesReaded;
}

long CjocWav::GetAudioData(unsigned char *pAudioData, long lOffset, long lAudioDataBytes)
{
	if (lOffset < 0)
		return -1;	

	//long lGotoPos = m_lLoadOffsetStartAudioData + lOffset;
	long lGotoPos = m_lLoadOffsetStartReadAudioData + lOffset;
	//if (lGotoPos >= m_lLoadFileDataSize)
	if (lGotoPos >= m_lLoadOffsetStopReadAudioData)
		return -2;	

	long lPos = ftell(m_fp);	
	if (lGotoPos != lPos)
		fseek (m_fp,lGotoPos,SEEK_SET);

	return GetAudioData (pAudioData,lAudioDataBytes);
}

long CjocWav::GetAudioData(unsigned char *pAudioData, long lAudioDataBytes)
{
	if (lAudioDataBytes <= 0){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if (pAudioData == NULL){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if (m_fp == NULL){throw JOC_Error_(enjocErrors::err_bad_state );}

	return fread(pAudioData, sizeof(unsigned char), lAudioDataBytes, m_fp);
}

void CjocWav::SaveFile (std::wstring &strFileName)
{
	int nRc = 0;
	unsigned long ulData = 0;

	if ((m_fp != NULL)||(m_bSave == false)){throw JOC_Error_(enjocErrors::err_bad_state);}

	char cid[5];
	memset (cid,0,sizeof (char)*5);

	m_fp = _wfopen(strFileName.c_str (),L"wb"); 
	if (m_fp == NULL){throw JOC_Error_arg_(enjocErrors::err_creating_file_for_write ,strFileName.c_str ());}

	strncpy(cid,"RIFF",5);
	nRc = fwrite (cid, sizeof(unsigned char), 4, m_fp); //Write in first four bytes 
	if (nRc != 4)
	{
		fclose(m_fp);
		JOC_Error_arg_(enjocErrors::err_savingwavfile ,strFileName.c_str ());		
	}

	//ulChunksSizeTotal
	ulData = 0;
	nRc = fwrite (&ulData, sizeof(unsigned long), 1, m_fp); //4 + chunks data
	if (nRc != 1)
	{
		fclose(m_fp);
		JOC_Error_arg_(enjocErrors::err_savingwavfile ,strFileName.c_str ());		
	}

	strncpy(cid,"WAVE",5);
	nRc = fwrite (cid, sizeof(unsigned char), 4, m_fp); 
	if (nRc != 4)
	{
		fclose(m_fp);
		JOC_Error_arg_(enjocErrors::err_savingwavfile ,strFileName.c_str ());		
	}

	strncpy(cid,"fmt ",5);
	nRc = fwrite (cid, sizeof(unsigned char), 4, m_fp); 
	if (nRc != 4)
	{
		fclose(m_fp);
		JOC_Error_arg_(enjocErrors::err_savingwavfile ,strFileName.c_str ());		
	}

	ulData = 16;
	nRc = fwrite(&ulData, sizeof(unsigned long), 1, m_fp); 
	if (nRc != 1)
	{
		fclose(m_fp);
		JOC_Error_arg_(enjocErrors::err_savingwavfile ,strFileName.c_str ());		
	}

	short sFormatTag = m_WavSaveData.sFormatTag;
	nRc = fwrite(&sFormatTag , sizeof(short), 1, m_fp); 
	if (nRc != 1)
	{
		fclose(m_fp);
		JOC_Error_arg_(enjocErrors::err_savingwavfile ,strFileName.c_str ());		
	}

	//Nº of channels
	nRc = fwrite(&m_WavSaveData.sChannels, sizeof(short),1,m_fp); //1 mono, 2 stereo 
	if (nRc != 1)
	{
		fclose(m_fp);
		JOC_Error_arg_(enjocErrors::err_savingwavfile ,strFileName.c_str ());		
	}

	//Sample rate
    nRc = fwrite(&m_WavSaveData.ulSampleRate, sizeof(unsigned long), 1, m_fp); //like 44100, 22050, etc... 
	if (nRc != 1)
	{
		fclose(m_fp);
		JOC_Error_arg_(enjocErrors::err_savingwavfile ,strFileName.c_str ());		
	}

	//ulAvgBytesSec
	unsigned long ulAvgBytesSec = m_WavSaveData.ulSampleRate * m_WavSaveData.sChannels * (m_WavSaveData.sBitsPerSample/8);
	nRc = fwrite(&ulAvgBytesSec,sizeof(unsigned long), 1, m_fp);
	if (nRc != 1)
	{
		fclose(m_fp);
		JOC_Error_arg_(enjocErrors::err_savingwavfile ,strFileName.c_str ());		
	}

	//sBlockAlign
	short sBlockAlign = m_WavSaveData.sChannels * (m_WavSaveData.sBitsPerSample/8);
    nRc = fwrite(&sBlockAlign, sizeof(short), 1, m_fp);
	if (nRc != 1)
	{
		fclose(m_fp);
		JOC_Error_arg_(enjocErrors::err_savingwavfile ,strFileName.c_str ());		
	}

	//Bits  per sample
    nRc = fwrite(&m_WavSaveData.sBitsPerSample, sizeof(short), 1, m_fp); //8,16,24,32 bit/sample 
	if (nRc != 1)
	{
		fclose(m_fp);
		JOC_Error_arg_(enjocErrors::err_savingwavfile ,strFileName.c_str ());		
	}


	//START CHUNK DATA	
	strncpy(cid,"data",5);
	nRc = fwrite(cid, sizeof(unsigned char), 4, m_fp);
	if (nRc != 4)
	{
		fclose(m_fp);
		JOC_Error_arg_(enjocErrors::err_savingwavfile ,strFileName.c_str ());		
	}

	//WRITE AT THE END
	unsigned long ulDataSize = 0;
    nRc = fwrite(&ulDataSize, sizeof(unsigned long), 1, m_fp);
	if (nRc != 1)
	{
		fclose(m_fp);
		JOC_Error_arg_(enjocErrors::err_savingwavfile ,strFileName.c_str ());		
	}

	m_lSaveOffsetStartAudioData = ftell(m_fp);
}

long CjocWav::SetAudioDataDoubleAllChannels (double *pdAudioData, long lOffsetSamples, long lAudioDataSamplesPerChannel)
{
	long lRc = 0;

	if (lAudioDataSamplesPerChannel < 0){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if (pdAudioData == NULL){throw JOC_Error_(enjocErrors::err_bad_arguments);}
	
	if (lOffsetSamples < 0){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if (m_fp == NULL){throw JOC_Error_(enjocErrors::err_bad_state);}

	if (lAudioDataSamplesPerChannel > 0)
	{
		long lBytesPerSample = m_WavSaveData.sBitsPerSample / 8;
		long lBlockAlign  = m_WavSaveData.sChannels * lBytesPerSample;
		long lGotoPosBytes = lOffsetSamples * lBytesPerSample;//lBlockAlign;
		long lAudioDataTotalSamples = lAudioDataSamplesPerChannel * m_WavSaveData.sChannels;
		long lSaveBytes = lBytesPerSample * lAudioDataTotalSamples;
		unsigned char pdata[16];	
		bool bExit = false;
		long lnS = 0;
		while (bExit == false)
		{
			int nRc = SetSampleValue(pdata,0,lBytesPerSample, pdAudioData[lnS],m_WavSaveData.sFormatTag );
			if (nRc != 0)
				m_ulTruncatedSamples++; //Truncated sample VALUE!!!!

			lRc = SetAudioData (pdata,lGotoPosBytes,lBytesPerSample);
			if (lRc >= lBytesPerSample)
			{
				lGotoPosBytes = lGotoPosBytes + lBytesPerSample;
				lnS++;
			
				if (lnS >= lAudioDataTotalSamples)
				{
					lRc = lnS;
					bExit = true;
				}
			}
			else
			{
				lRc = -5;
				bExit = true;
			}
		}
	}

	return lRc;
}

unsigned long CjocWav::GetNumSamplesTruncated (bool bReset)
{
	unsigned long ulTmp = m_ulTruncatedSamples;

	if (bReset == true){m_ulTruncatedSamples = 0;}

	return ulTmp;
}

/*long CjocWav::SetAudioDataDouble(double *pdAudioData, int nChannel, long lOffsetSamples, long lAudioDataSamples)
{
	long lRc = 0;

	if (lAudioDataSamples <= 0){throw JOC_Error_(enjocErrors::err_bad_arguments);}
	
	if (pdAudioData == NULL){throw JOC_Error_(enjocErrors::err_bad_arguments);}
	
	if (nChannel >= m_WavSaveData.sChannels){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if (lOffsetSamples < 0){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if (m_fp == NULL){throw JOC_Error_(enjocErrors::err_bad_state );}

	long lBytesPerSample = m_WavSaveData.sBitsPerSample / 8;
	long lBlockAlign  = m_WavSaveData.sChannels * lBytesPerSample;
	long lGotoPosBytes = (lOffsetSamples * lBlockAlign) + (nChannel*lBytesPerSample);

	bool bExit = false;
	unsigned char pdata[16];	
	long lSamplesWritten = 0;
	while (bExit == false)
	{		
		int nRc = SetSampleValue(pdata,0,lBytesPerSample, pdAudioData[lSamplesWritten],m_WavSaveData.sFormatTag );
		if (nRc != 0)
			m_ulTruncatedSamples++; //Truncated sample VALUE!!!!

		lRc = SetAudioData (pdata,lGotoPosBytes,lBytesPerSample);

		if (lRc >= lBytesPerSample)
		{
			lSamplesWritten++;
			lGotoPosBytes = lGotoPosBytes + lBlockAlign;
			
			if (lSamplesWritten >= lAudioDataSamples)
				bExit = true;
		}
		else
		{
			bExit = true;
		}
	}

	return lSamplesWritten;
}*/

long CjocWav::SetAudioData(unsigned char *pAudioData,long lAudioDataBytes)
{
	if (m_fp == NULL){throw JOC_Error_(enjocErrors::err_bad_state);}

	if (pAudioData == NULL){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	if (lAudioDataBytes <= 0){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	long lRc = fwrite(pAudioData, sizeof(unsigned char), lAudioDataBytes, m_fp);
	m_lSaveFileSoundSize = m_lSaveFileSoundSize  + lRc;

	return lRc;
}

long CjocWav::SetAudioData(unsigned char *pAudioData,long lOffset, long lAudioDataBytes)
{
	if (lOffset < 0){throw JOC_Error_(enjocErrors::err_bad_arguments);}
				
	long lGotoPos = m_lSaveOffsetStartAudioData + lOffset;
	long lPos = ftell(m_fp);	
	if (lGotoPos != lPos)
		fseek (m_fp,lGotoPos,SEEK_SET);

	return SetAudioData (pAudioData,lAudioDataBytes);
}

void CjocWav::CloseFile ()
{
	if (m_fp != NULL)
	{
		if (m_bSave == true)
		{
			int nSavePaddingBytes = 0;
			if (m_lSaveFileSoundSize%2 > 0)
			{//Odd
				nSavePaddingBytes = 1;
				unsigned char cData = 0;
				fwrite (&cData ,sizeof unsigned char (),1,m_fp);
			}
				
			//WRITE AT THE END (4 + chunks data)
			unsigned long ulData = m_lSaveFileSoundSize + 4 + 24 + 4 + 4 + nSavePaddingBytes;
			fseek(m_fp,4,SEEK_SET);
			fwrite (&ulData, sizeof(unsigned long), 1, m_fp);
			
			//WRITE AT THE END (ulDataSize)
			fseek(m_fp,40,SEEK_SET);
			fwrite(&m_lSaveFileSoundSize, sizeof(unsigned long), 1, m_fp);
		}

		fflush(m_fp);

		fclose (m_fp);

		m_fp = NULL;
		m_bSave = false;

		m_lLoadOffsetStartReadAudioData = 0;
		m_lLoadOffsetStopReadAudioData = 0;

		m_lLoadOffsetStartAudioData = 0;
		m_lSaveOffsetStartAudioData = 0;
		m_lLoadFileDataSize = 0;
		m_lSaveFileSoundSize = 0;
	}
}

double CjocWav::GetAudioDurationMS ()
{
	double dRet = 0.0;

	if ((m_WavLoadData.sBlockAlign >= 0)&&(m_WavLoadData.ulSampleRate >= 0))
	{
		unsigned long ulTotalDataSize = m_lLoadOffsetStopReadAudioData-m_lLoadOffsetStartReadAudioData;

		double dSamples = ulTotalDataSize / m_WavLoadData.sBlockAlign;
		dRet = ((dSamples / (double)m_WavLoadData.ulSampleRate)*1000.0);
	}

	return dRet;
}

CjocWav::WAVLOADDATA CjocWav::GetWaveLoadData()
{
	WAVLOADDATA WavRetData;
	memset (&WavRetData,0,sizeof (WAVLOADDATA));

	if (m_bSave == false)
	{
		WavRetData.sBitsPerSample = m_WavLoadData.sBitsPerSample;
		WavRetData.sChannels = m_WavLoadData.sChannels;
		WavRetData.sFormatTag = (CjocWav::enWavType) m_WavLoadData.sFormatTag;
		WavRetData.ulSampleRate = m_WavLoadData.ulSampleRate;
		WavRetData.ulChunkSize = m_WavLoadData.ulChunkSize;
	}

	return WavRetData;
}

std::wstring CjocWav::GetWaveLoadDataStr()
{
	std::wstring strRet;

	strRet = L"Duration: " + CjocUtils::dToStr ((double)GetAudioDurationMS ()/1000.0) + L". Tin: " + CjocUtils::dToStr (m_WavLoadData.dTin) + L". Tout: " + CjocUtils::dToStr (m_WavLoadData.dTout) + L"\n";

	CjocWav::WAVLOADDATA dtaFile = GetWaveLoadData();

	if (dtaFile.ulChunkSize == CjocWav::enWavHeaderFormat::WAV16 )
		strRet = strRet + L"File type: WAVE (Header chunk size = 16)\n";

	if (dtaFile.ulChunkSize == CjocWav::enWavHeaderFormat::WAV18 )
		strRet = strRet + L"File type: WAVE (Header chunk size = 18)\n";

	if (dtaFile.ulChunkSize  == CjocWav::enWavHeaderFormat::WAVEEX40)
		strRet = strRet + L"File type: WAVEEX (Header chunk size = 40)\n";

	strRet = strRet + L"Sampling freq: " + CjocUtils::ulToStr (dtaFile.ulSampleRate)+ L"\n";
	strRet = strRet + L"Bits per sample: " + CjocUtils::ulToStr (dtaFile.sBitsPerSample)+ L"\n";
	strRet = strRet + L"Channels: " + CjocUtils::ulToStr (dtaFile.sChannels)+ L"\n";
	
	if (dtaFile.sFormatTag == CjocWav::enWavType::WAV_TYPE_PCM_INT)
		strRet = strRet + L"Sample type: PCM\n";

	if (dtaFile.sFormatTag == CjocWav::enWavType::WAV_TYPE_PCM_IEEE_FLOAT)
		strRet = strRet + L"Sample type: FLOAT\n";
			
	return strRet;
}

void CjocWav::ClearAll ()
{
	CloseFile ();

	memset (&m_WavLoadData,0,sizeof (INTERNALLOADDATA));
	m_WavLoadData.dTin = -1;
	m_WavLoadData.dTout = -1;

	m_ulTruncatedSamples = 0;

	memset (&m_WavSaveData,0,sizeof (WAVSAVEDATA));
}

CjocWav::~CjocWav(void)
{
	ClearAll ();
}

double CjocWav::GetSampleValue (unsigned char *pSamplesBlock, unsigned long ulOffset, int nSampleSizeBytes,enWavType WavType, bool bNormalized)
{
	double dRet = 0;

	//Included in ulOffset
	int nOffsetChannelBytes  = 0;//int nOffsetChannelBytes = nSampleSizeBytes * nChannelToFilter;

	if (WavType == enWavType::WAV_TYPE_PCM_INT)
	{
		if (nSampleSizeBytes == 1)
		{//8 bits / sample (-128 ... 0 ... +127)
			dRet = pSamplesBlock[ulOffset + nOffsetChannelBytes] - 128;
			if (bNormalized == true)
				dRet = dRet / 128.0;
		}
		else if (nSampleSizeBytes == 2)
		{//16 bits / sample (-32768 ... 0 ... +32767)
			short nSample16 = pSamplesBlock[ulOffset + 1 + nOffsetChannelBytes];
			nSample16 = nSample16<<8;
			nSample16 = nSample16 + pSamplesBlock[ulOffset + nOffsetChannelBytes];
		 
			dRet = (double)nSample16;
			if (bNormalized == true)
				dRet = dRet / 32768.0;
		}
		else if (nSampleSizeBytes == 3)
		{//24 bits / sample
			if ((pSamplesBlock[ulOffset + 2 + nOffsetChannelBytes] & 0x80) == 0)
			{//+
				int nSample32 = pSamplesBlock[ulOffset + 2 + nOffsetChannelBytes];
				nSample32 = nSample32<<8;
				nSample32 = nSample32 + pSamplesBlock[ulOffset + 1 + nOffsetChannelBytes];
				nSample32 = nSample32<<8;
				nSample32 = nSample32 + pSamplesBlock[ulOffset + nOffsetChannelBytes];
				dRet = (double)nSample32;
				if (bNormalized == true)
					dRet = dRet / 8388608.0;
			}
			else
			{//-
				int nSample32 = (pSamplesBlock[ulOffset + 2 + nOffsetChannelBytes] & 0x7F);
				nSample32 = nSample32<<8;
				nSample32 = nSample32 + pSamplesBlock[ulOffset + 1 + nOffsetChannelBytes];
				nSample32 = nSample32<<8;
				nSample32 = nSample32 + pSamplesBlock[ulOffset + nOffsetChannelBytes];
				dRet = (double)-8388608.0 + (double)nSample32;
				if (bNormalized == true)
					dRet = dRet / 8388608.0;
			}
		}
	}
	else
	{
		float *pData = (float*) &pSamplesBlock[ulOffset + nOffsetChannelBytes]; 
		dRet = pData[0];
	}

	return dRet ;
}

int CjocWav::SetSampleValue (unsigned char *pSamplesBlock, unsigned long ulOffset, int nSampleSizeBytes, double Ox, enWavType WavType)
{
	int nRc = 0;

	//Included in ulOffset
	int nOffsetChannelBytes  = 0;//int nOffsetChannelBytes = nSampleSizeBytes * nChannelToFilter;

	if (WavType == enWavType::WAV_TYPE_PCM_INT )
	{
		if (nSampleSizeBytes == 1) //8 bits
		{
			if (Ox > 127)
			{
				Ox = 127;
				nRc = 1; //Up clipping
			}
			if (Ox < -128)
			{
				Ox = -128;
				nRc = 2; //Down clipping
			}
			pSamplesBlock[ulOffset + nOffsetChannelBytes] = (unsigned char) (Ox + 128.0);
		}
		if (nSampleSizeBytes == 2) //16 bits
		{
			if (Ox > 32767.0)
			{
				Ox = 32767.0;
				nRc = 1; //Up clipping
			}
			if (Ox < -32768.0)
			{
				Ox = -32768.0;
				nRc = 2; //Down clipping
			}
			short* pData = (short*) &pSamplesBlock[ulOffset + nOffsetChannelBytes];
			*pData = (short) Ox;
		}
		if (nSampleSizeBytes == 3) //24 bits
		{
			if (Ox > 8388607.0)
			{
				Ox = 8388607.0;
				nRc = 1; //Up clipping
			}
			if (Ox < -8388608.0)
			{
				Ox = -8388608.0;
				nRc = 2; //Down clipping
			}

			int nData = (int) Ox;
			pSamplesBlock[ulOffset + nOffsetChannelBytes] = nData & 0xFF;
			nData = nData >> 8;
			pSamplesBlock[ulOffset + 1 + nOffsetChannelBytes] = nData & 0xFF;
			nData = nData >> 8;
			pSamplesBlock[ulOffset + 2 + nOffsetChannelBytes]= nData & 0xFF;
		}
	}
	else
	{
		if (Ox > 1.0)
		{
			Ox = 1.0;
			nRc = 1; //Up clipping
		}
		if (Ox < -1.0)
		{
			Ox = -1.0;
			nRc = 2; //Up clipping
		}
		float* pData = (float*) &pSamplesBlock[ulOffset + nOffsetChannelBytes];
		*pData = (float) Ox;
	}

	return nRc;
}
