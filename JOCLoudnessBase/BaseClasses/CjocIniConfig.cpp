#include "CjocIniConfig.h"
#include "CjocUtils.h"
#include "CjocException.h"

#include<iostream>
#include<fstream>

CjocIniConfig::CjocIniConfig(void)
{

}


CjocIniConfig::~CjocIniConfig(void)
{
	
}

void CjocIniConfig::Load (std::wstring &strFile)
{
	if (strFile.empty () == true){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	m_configValues.clear();

	std::ifstream iniFile;

	iniFile.open(strFile,std::ios::in);

	if (iniFile.is_open () == false){throw JOC_Error_arg_(enjocErrors::err_opening_file, strFile.c_str ());}

	while (!iniFile.eof()) 
	{
		std::string strLine;
		getline(iniFile,strLine);
		
		ProcLine (CjocUtils::ConvertFromStr (strLine));
	}

	iniFile.close ();

}

void CjocIniConfig::ProcLine (std::wstring &strLine)
{
	strLine = CjocUtils::Trim(strLine);

	if (strLine.empty () == true){return;}
	if (strLine.substr (0,1) == L"#"){return;}

	size_t pos;

	pos = strLine.find_first_of (L"=");

	if ((pos >=0) && ((pos+1) < strLine.length ()))
	{
		std::wstring strParam = CjocUtils::Trim(strLine.substr (0,pos));
		std::wstring strData = CjocUtils::Trim(strLine.substr (pos+1));

		//ADD
		m_configValues.insert (std::pair <std::wstring, std::wstring>(strParam,strData));
	}
}

bool CjocIniConfig::GetConfigValueS (std::wstring *pstrRet, std::wstring strParam, std::wstring strDef)
{
	bool bRc = false;

	if (pstrRet == NULL){throw JOC_Error_(enjocErrors::err_bad_arguments);}
	if (strParam.empty () == true){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	mapStrStr::iterator iter;

	iter = m_configValues.find (strParam);
	if (iter != m_configValues.end ())
	{
		*pstrRet = iter->second;
		bRc = true;
	}
	else
	{
		*pstrRet = strDef;
	}

	return bRc;
}

bool CjocIniConfig::GetConfigValueSL (std::wstring *pstrRet, std::wstring strParam, unsigned int nIndex)
{
	std::wstring strLine;
	bool bfound = GetConfigValueS (&strLine,strParam,L"");
	if (bfound == false){return false;}

	return CjocUtils::GetParamFromLineWithSeparators(pstrRet,strLine,nIndex,L"|");
}

bool CjocIniConfig::GetConfigValueSFromVector (std::wstring *pstrRet, std::wstring strParam, unsigned int nIndex)
{
	std::wstring strLine;
	bool bfound = GetConfigValueS (&strLine,strParam,L"");
	if (bfound == false){return false;}

	strLine = CjocUtils::FindAndReplace (strLine,L"[",L" ");
	strLine = CjocUtils::FindAndReplace (strLine,L"]",L" ");
	strLine= CjocUtils::Trim (strLine);

	return CjocUtils::GetParamFromLineWithSeparators(pstrRet,strLine,nIndex,L" ");
}

bool CjocIniConfig::GetConfigValueDFromVector (double *pdRet, std::wstring strParam, unsigned int nIndex)
{
	bool bRet = false;
	std::wstring strRet;

	bRet = GetConfigValueSFromVector(&strRet,strParam,nIndex);
	if (bRet == true)
	{
		if (CjocUtils::IsNumeric (strRet) == true)
		{
			if (pdRet != NULL){*pdRet = wcstod(strRet.c_str (),NULL);}		
		}
		else
		{
			bRet = false;
		}
	}

	return bRet;
}

std::wstring CjocIniConfig::GetConfigValueS (std::wstring strParam, std::wstring strDef)
{
	std::wstring strRet;

	GetConfigValueS (&strRet,strParam, strDef);

	return strRet;
}

bool CjocIniConfig::GetConfigValueI (int *pnRet, std::wstring strParam, int nDef)
{
	if (pnRet == NULL){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	std::wstring strDef = CjocUtils::lToStr (nDef);
	std::wstring strRet;

	bool bRc = GetConfigValueS (&strRet,strParam,strDef);


	*pnRet = _wtoi (strRet.c_str ());

	return bRc;
}

int CjocIniConfig::GetConfigValueI (std::wstring strParam, int nDef )
{
	int nRet = 0;

	GetConfigValueI (&nRet,strParam, nDef);

	return nRet;

}

bool CjocIniConfig::GetConfigValueUL (unsigned long *pulRet, std::wstring strParam, unsigned long ulDef)
{
	if (pulRet == NULL){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	std::wstring strDef =  CjocUtils::ulToStr (ulDef);
	std::wstring strRet;

	bool bRc = GetConfigValueS (&strRet,strParam,strDef);
	
	*pulRet = wcstoul(strRet.c_str (),NULL,10);

	return bRc;
}
unsigned long CjocIniConfig::GetConfigValueUL (std::wstring strParam, unsigned long ulDef )
{
	unsigned long ulRet = 0;

	GetConfigValueUL (&ulRet,strParam, ulDef);

	return ulRet;
}

bool CjocIniConfig::GetConfigValueD (double *pdRet, std::wstring strParam, double dDef)
{
	if (pdRet == NULL){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	char szBuf[512];
	sprintf (szBuf,"%f",dDef);
	std::wstring strDef = CjocUtils::ConvertFromChar (szBuf);
	std::wstring strRet;

	bool bRc = GetConfigValueS (&strRet,strParam,strDef);
	
	*pdRet = wcstod(strRet.c_str (),NULL);

	return bRc;
}

double CjocIniConfig::GetConfigValueD (std::wstring strParam, double dDef)
{
	double dRet = 0;

	GetConfigValueD (&dRet,strParam, dDef);

	return dRet;

}

std::wstring CjocIniConfig::DumpAllValues()
{
	std::wstring strRet;

	mapStrStr::iterator iter;

	iter = m_configValues.begin ();
	while (iter != m_configValues.end ())
	{
		strRet = strRet +  L"Param: \"" + iter->first + L"\" - Value: \"" + iter->second + L"\"\n";

		iter++;
	}

	return strRet;
}

//JOC
bool CjocIniConfig::GetConfigValueS (std::wstring *pstrRet, int nValue, std::wstring strDef)
{
	bool bFound = false;

	std::wstring strParam = GetParamStrByVal (nValue,&bFound);

	if (bFound == false){return false;}

	return CjocIniConfig::GetConfigValueS (pstrRet,strParam,strDef);
}

std::wstring CjocIniConfig::GetConfigValueS (int nValue)
{	
	return CjocIniConfig::GetConfigValueS (GetParamStrByVal (nValue));
}

bool CjocIniConfig::GetConfigValueSL (std::wstring *pstrRet, int nValue, unsigned int nIndex)
{
	bool bFound = false;

	std::wstring strParam = GetParamStrByVal (nValue,&bFound);

	if (bFound == false){return false;}

	return CjocIniConfig::GetConfigValueSL (pstrRet,strParam,nIndex);
}

std::wstring CjocIniConfig::GetConfigValueSL (int nValue, unsigned int nIndex)
{
	std::wstring strRet;
	
	CjocIniConfig::GetConfigValueSL (&strRet, GetParamStrByVal (nValue), nIndex);

	return strRet;
}

bool CjocIniConfig::GetConfigValueSFromVector (std::wstring *pstrRet, int nValue, unsigned int nIndex)
{
	bool bFound = false;

	std::wstring strParam = GetParamStrByVal (nValue,&bFound);

	if (bFound == false){return false;}

	return CjocIniConfig::GetConfigValueSFromVector (pstrRet,strParam,nIndex);
}

std::wstring CjocIniConfig::GetConfigValueSFromVector (int nValue, unsigned int nIndex)
{
	std::wstring strRet;
	
	CjocIniConfig::GetConfigValueSFromVector (&strRet, GetParamStrByVal (nValue), nIndex);

	return strRet;
}

bool CjocIniConfig::GetConfigValueDFromVector (double *pdRet, int nValue, unsigned int nIndex)
{
	bool bFound = false;

	std::wstring strParam = GetParamStrByVal (nValue,&bFound);

	if (bFound == false){return false;}

	return CjocIniConfig::GetConfigValueDFromVector (pdRet,strParam,nIndex);
}

double CjocIniConfig::GetConfigValueDFromVector (int nValue, unsigned int nIndex)
{
	double dRet;
	
	CjocIniConfig::GetConfigValueDFromVector (&dRet, GetParamStrByVal (nValue), nIndex);

	return dRet;
}

bool CjocIniConfig::GetConfigValueI (int *pnRet, int nValue, int nDef)
{
	bool bFound = false;

	std::wstring strParam = GetParamStrByVal (nValue,&bFound);

	if (bFound == false){return false;}

	return CjocIniConfig::GetConfigValueI (pnRet,strParam,nDef);
}
int CjocIniConfig::GetConfigValueI (int nValue, int nDef)
{
	return CjocIniConfig::GetConfigValueI (GetParamStrByVal (nValue));
}


bool CjocIniConfig::GetConfigValueUL (unsigned long *pulRet, int nValue, unsigned long ulDef)
{
	bool bFound = false;

	std::wstring strParam = GetParamStrByVal (nValue,&bFound);

	if (bFound == false){return false;}

	return CjocIniConfig::GetConfigValueUL (pulRet,strParam,ulDef);
}
unsigned long CjocIniConfig::GetConfigValueUL (int nValue, unsigned long ulDef )
{
	return CjocIniConfig::GetConfigValueUL (GetParamStrByVal (nValue));
}

bool CjocIniConfig::GetConfigValueD (double *pdRet, int nValue, double dDef)
{
	bool bFound = false;

	std::wstring strParam = GetParamStrByVal (nValue,&bFound);

	if (bFound == false){return false;}

	return CjocIniConfig::GetConfigValueD (pdRet,strParam,dDef);
}
double CjocIniConfig::GetConfigValueD (int nValue, double dDef)
{
	return CjocIniConfig::GetConfigValueD (GetParamStrByVal (nValue));
}

std::wstring CjocIniConfig::GetParamStrByVal (int nValue, bool *pbFound)
{
	std::wstring strParam;
	if (pbFound != NULL) {*pbFound= false;}
	mapIntStr::iterator iter;

	iter = m_configLoadedValues.find (nValue);
	if (iter != m_configLoadedValues.end ())
	{
		strParam = iter->second;
		if (pbFound != NULL) {*pbFound= true;}
	}

	return strParam;
}