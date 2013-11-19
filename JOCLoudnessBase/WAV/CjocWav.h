#pragma once

#include "..\BaseClasses\jocCommonHeader.h"
#include "..\BaseClasses\CjocCrono.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define WAVE_FORMAT_PCM				0x0001		//PCM
#define WAVE_FORMAT_IEEE_FLOAT		0x0003		//IEEE float

#define WAVE_CHUNK16				16
#define WAVE_CHUNK18				18
#define WAVE_CHUNK40				40


class JOC_EXPORTS CjocWav //:	public CjocCrono
{
public:

	enum enWavType 
	{ 
		WAV_TYPE_PCM_INT = WAVE_FORMAT_PCM,
		WAV_TYPE_PCM_IEEE_FLOAT = WAVE_FORMAT_IEEE_FLOAT
	};	

	enum enWavHeaderFormat
	{ 
		WAV16 = WAVE_CHUNK16,
		WAV18 = WAVE_CHUNK18,
		WAVEEX40 = WAVE_CHUNK40
	};	

	typedef struct 
	{
		enWavType sFormatTag; 
		short sChannels;
		short sBitsPerSample;
		unsigned long ulSampleRate;
		unsigned long ulChunkSize;
	}WAVLOADDATA;

	typedef struct 
	{
		enWavType sFormatTag; 
		short sChannels;
		short sBitsPerSample;
		unsigned long ulSampleRate;
	}WAVSAVEDATA;

	CjocWav(void);
	CjocWav(WAVSAVEDATA wavData);

	~CjocWav(void);
	
	//Load *******************
	void LoadFile (std::wstring &strFileName, double dTin = -1, double dTout = -1);
	void SetInOutPoints(double dTin = -1, double dTout = -1);
	double GetInPoint();
	double GetOutPoint();
	
	long GetAudioDataDoubleAllChannels(double *pdAudioData, long lOffsetSamples, long lAudioDataSamplesPerChannel, bool bNormalized = true);

	//Save *******************
	void SaveFile (std::wstring &strFileName);
	long SetAudioDataDoubleAllChannels (double *pdAudioData, long lOffsetSamples, long lAudioDataSamplesPerChannel);

	unsigned long GetNumSamplesTruncated (bool bReset = false);

	//Common
	void CloseFile ();
	WAVLOADDATA GetWaveLoadData();
	std::wstring GetWaveLoadDataStr();
	double GetAudioDurationMS (); //Only load file

	//Static
	static double GetSampleValue (unsigned char *pSamplesBlock, unsigned long ulOffset, int nSampleSizeBytes, enWavType WavType = WAV_TYPE_PCM_INT , bool bNormalized = true);
	static int SetSampleValue (unsigned char *pSamplesBlock, unsigned long ulOffset, int nSampleSizeBytes,double Ox, enWavType WavType = WAV_TYPE_PCM_INT);

private:
	typedef struct
	{
		unsigned long ulChunksSizeTotal; 
		unsigned long ulChunkSize; //These chunks are allowed = 16,18,40(WAVEX)
		short sFormatTag; //1 PCM, 3 IEEE FLOAT
		short sChannels; //2 = st
		short sBlockAlign; //Separation between samples of the same channel
		short sBitsPerSample; //Ex: 16 (2 bytes)
		unsigned long ulSampleRate; //ex 48000
		unsigned long ulAvgBytesSec;
		unsigned long ulDataSize; //Size of sound data
		double dTin;
		double dTout;
	}INTERNALLOADDATA;

	FILE *m_fp; 
	bool m_bSave;

	long m_lLoadOffsetStartAudioData;
	long m_lSaveOffsetStartAudioData;

	long m_lLoadOffsetStartReadAudioData;
	long m_lLoadOffsetStopReadAudioData;


	long m_lLoadFileDataSize;
	long m_lSaveFileSoundSize;

	INTERNALLOADDATA m_WavLoadData;
	WAVSAVEDATA m_WavSaveData;

	unsigned long m_ulTruncatedSamples;
	
	void IniAllData();
	void ClearAll ();

	//LOAD
	long GetAudioData(unsigned char *pAudioData, long lAudioDataBytes);
	long GetAudioData(unsigned char *pAudioData,long lOffset,long lAudioDataBytes);		
	//long GetAudioDataDouble(double *pdAudioData, int nChannel, long lOffsetSamples, long lAudioDataSamples, bool bNormalized = true);

	//SAVE
	long SetAudioData(unsigned char *pAudioData,long lAudioDataBytes);
	long SetAudioData(unsigned char *pAudioData,long lOffset, long lAudioDataBytes);	
	//long SetAudioDataDouble(double *pdAudioData, int nChannel, long lOffsetSamples, long lAudioDataSamples);
};

