#pragma once

#include "jocCommonHeader.h"
#include <map>

class JOC_EXPORTS CjocIniConfig
{
public:
	CjocIniConfig(void);
	~CjocIniConfig(void);

	void Load (std::wstring &strFile);

	bool GetConfigValueS (std::wstring *pstrRet, std::wstring strParam, std::wstring strDef);
	std::wstring GetConfigValueS (std::wstring strParam, std::wstring strDef = L"");

	bool GetConfigValueSL (std::wstring *pstrRet, std::wstring strParam, unsigned int nIndex);
	bool GetConfigValueSFromVector (std::wstring *pstrRet, std::wstring strParam, unsigned int nIndex);

	bool GetConfigValueDFromVector (double *pdRet, std::wstring strParam, unsigned int nIndex);

	bool GetConfigValueI (int *pnRet, std::wstring strParam, int nDef);
	int GetConfigValueI (std::wstring strParam, int nDef = 0);

	bool GetConfigValueUL (unsigned long *pulRet, std::wstring strParam, unsigned long ulDef);
	unsigned long GetConfigValueUL (std::wstring strParam, unsigned long ulDef = 0);

	bool GetConfigValueD (double *pdRet, std::wstring strParam, double dDef);
	double GetConfigValueD (std::wstring strParam, double dDef= 0.0);

	std::wstring DumpAllValues();

	//Retrive functions JOC
	bool GetConfigValueS (std::wstring *pstrRet, int nValue, std::wstring strDef);
	std::wstring GetConfigValueS (int nValue);

	bool GetConfigValueSL (std::wstring *pstrRet, int nValue, unsigned int nIndex);
	std::wstring GetConfigValueSL (int nValue, unsigned int nIndex);

	bool GetConfigValueSFromVector (std::wstring *pstrRet, int nValue, unsigned int nIndex);
	std::wstring GetConfigValueSFromVector (int nValue, unsigned int nIndex);

	bool GetConfigValueDFromVector (double *pdRet, int nValue, unsigned int nIndex);
	double GetConfigValueDFromVector (int nValue, unsigned int nIndex);

	bool GetConfigValueI (int *pnRet, int nValue, int nDef);
	int GetConfigValueI (int nValue, int nDef = 0);

	bool GetConfigValueUL (unsigned long *pulRet, int nValue, unsigned long ulDef);
	unsigned long GetConfigValueUL (int nValue, unsigned long ulDef = 0);

	bool GetConfigValueD (double *pdRet, int nValue, double dDef);
	double GetConfigValueD (int nValue, double dDef= 0.0);

private:
	std::wstring m_strFile;

	//Values
	typedef std::map <std::wstring, std::wstring> mapStrStr;
	mapStrStr m_configValues;

	void ProcLine (std::wstring &strLine);

	//By param
	std::wstring GetParamStrByVal (int nValue, bool *pbFound = NULL);

protected:
	typedef std::map <int, std::wstring> mapIntStr;
	mapIntStr m_configLoadedValues;

};

