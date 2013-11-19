#include "CjocUtils.h"
#include <ctime>
#include <algorithm>
#include <stdio.h>

CjocUtils::CjocUtils(void)
{
}


CjocUtils::~CjocUtils(void)
{
	
}

std::string CjocUtils::Trim(std::string& str)
{
	std::string strRet;

	std::string::size_type pos1 = str.find_first_not_of(' ');
	std::string::size_type pos2 = str.find_last_not_of(' ');
  
	if (pos1 != std::string::npos)
		strRet = str.substr(pos1 == std::string::npos ? 0 : pos1, pos2 == std::string::npos ? str.length() - 1 : pos2 - pos1 + 1);

	return strRet;
}

std::wstring CjocUtils::Trim(std::wstring& str)
{
	std::wstring strRet;

	std::wstring::size_type pos1 = str.find_first_not_of(' ');
	std::wstring::size_type pos2 = str.find_last_not_of(' ');
  
	if (pos1 != std::wstring::npos)
		strRet = str.substr(pos1 == std::wstring::npos ? 0 : pos1, pos2 == std::wstring::npos ? str.length() - 1 : pos2 - pos1 + 1);

	return strRet;
}

std::string CjocUtils::ConvertFromwstr (std::wstring wstr)
{
	std::string strRet(wstr.begin (),wstr.end ());

	return strRet;
}

std::wstring CjocUtils::ConvertFromChar (const char *szStr)
{
	std::wstring strRet;

	if (szStr != NULL)
		strRet = std::wstring(szStr,szStr + strlen(szStr));

	return strRet;
}

std::wstring CjocUtils::ToLower (std::wstring& str)
{
	std::wstring strRet= str;

	std::transform( strRet.begin(), strRet.end(), strRet.begin(), tolower);

	return strRet;
}

std::wstring CjocUtils::ToUpper (std::wstring& str)
{
	std::wstring strRet = str;

	std::transform( strRet.begin(), strRet.end(), strRet.begin(), toupper);
	
	return strRet;
}

bool CjocUtils::IsNumeric (std::wstring& str)
{
	bool bRc = true;

	for (int n=0; (n < str.length())&&(bRc == true); n++) 
	{
		//0 - 9,
		if ((str[n] < L'0' )||(str[n] > L'9'))
		{
			//.,+,-
			if ((str[n] != L'.' )&&(str[n] != L'+')&&(str[n] != L'-'))
				bRc = false;
		}
	}

	return bRc;
}

bool CjocUtils::FindParamD (std::vector<std::wstring> &strArray, std::wstring strParam, double *pdVal)
{
	std::wstring strVal;

	bool bFound = FindParamStr (strArray,strParam,&strVal);
	if (bFound == true)
	{
		if (CjocUtils::IsNumeric (strVal)== true)
		{					
			double dVal = _wtof(strVal.c_str ());
			if (pdVal != NULL){*pdVal = dVal;}
		}
		else
		{
			bFound = false;
		}
	}

	return bFound;
}

bool CjocUtils::FindParamStr (std::vector<std::wstring> &strArray, std::wstring strParam, std::wstring *pstrVal)
{
	bool bFound = false;

	for (int n = 0; ((n < strArray.size ())&&(bFound == false));n++)
	{
		std::wstring strArrParam = CjocUtils::ToLower (strArray[n]);

		std::wstring::size_type nPos = strArrParam.find (strParam);
		if (nPos != std::wstring::npos)
		{
			nPos = nPos + strParam.length ();

			if (nPos <= strArrParam.length ())
			{
				std::wstring strVal = strArrParam.substr (nPos);
				bFound = true;

				if (pstrVal != NULL){*pstrVal = strVal;}
			}
		}		
	}

	return bFound;
}

std::string CjocUtils::GetFileWithoutExt (std::string strFile)
{
	std::string strRet = strFile;

	std::string::size_type nPos = strFile.find_last_of (".");
	if ((nPos!= std::string::npos)&&(nPos >= 0))
		strRet = strFile.substr (0,nPos);

	return strRet;
}

std::wstring CjocUtils::GetFileWithoutExt (std::wstring strFile)
{
	std::wstring strRet = strFile;

	std::string::size_type nPos = strFile.find_last_of (L".");
	if ((nPos!= std::string::npos)&&(nPos >= 0))
		strRet = strFile.substr (0,nPos);

	return strRet;
}

std::string CjocUtils::GetFileFromPath (std::string strFilePath)
{
	std::string strRet = strFilePath;

	std::string::size_type nPos = strFilePath.find_last_of ("\\/");
	if ((nPos!= std::string::npos)&&(nPos >= 0))
		strRet = strFilePath.substr (nPos+1);

	return strRet;
}

std::string CjocUtils::AddPathFile (std::string strDir,std::string strFile)
{
	std::string strDirT = Trim (strDir);
	std::string strFileT = Trim (strFile);

	while ((strDirT.length () > 0)&&(strDirT.substr (strDirT.length ()-1) == "\\"))
	{
		strDirT = strDirT.substr (0,strDirT.length ()-1);
	}

	while ((strFileT.length () > 0)&&(strFileT.substr (0,1) == "\\"))
	{
		strFileT = strFileT.substr (1,strFileT.length ()-1);
	}
	
	return strDirT + "\\" + strFileT;
}

std::string CjocUtils::GetPathFromFilePath (std::string strFilePath)
{
	std::string strRet = strFilePath;

	std::string::size_type nPos = strFilePath.find_last_of ("\\");
	if ((nPos!= std::string::npos)&&(nPos >= 0))
		strRet = strFilePath.substr (0,nPos);

	while ((strRet.length () > 0)&&(strRet.substr (strRet.length ()-1) == "\\"))
	{
		strRet = strRet.substr (0,strRet.length ()-1);
	}

	return strRet;
}

bool CjocUtils::GetParamFromLineWithSeparators (std::string *pstrRet, std::string strLine, int nIndex, std::string strSeparator)
{
	bool bRc = false;

	size_t posStart = 0;
	size_t posEnd  = 0;
	size_t lenghtSeparator = 1;//strSeparator.length ();

	unsigned int nData = 0;

	while ((posEnd != std::string::npos)&&(bRc == false))
	{
		posEnd = strLine.find_first_of (strSeparator,posStart+lenghtSeparator);
				
		if (nIndex == nData)
		{
			if (pstrRet != NULL)
			{
				*pstrRet = strLine.substr (posStart,posEnd-posStart);
			}
			bRc = true;
		}

		nData++;
		
		if(posEnd != std::string::npos)
			posStart = posEnd + lenghtSeparator;
		else
			posStart = posEnd;
	}

	return bRc;
}

bool CjocUtils::GetParamFromLineWithSeparators (std::wstring *pstrRet, std::wstring strLine, int nIndex, std::wstring strSeparator)
{
	bool bRc = false;

	size_t posStart = 0;
	size_t posEnd  = 0;
	size_t lenghtSeparator = 1;//strSeparator.length ();

	unsigned int nData = 0;

	while ((posEnd != std::wstring::npos)&&(bRc == false))
	{
		posEnd = strLine.find_first_of (strSeparator,posStart+lenghtSeparator);
				
		if (nIndex == nData)
		{
			if (pstrRet != NULL)
			{
				*pstrRet = strLine.substr (posStart,posEnd-posStart);
			}
			bRc = true;
		}

		nData++;
		
		if(posEnd != std::string::npos)
			posStart = posEnd + lenghtSeparator;
		else
			posStart = posEnd;
	}

	return bRc;
}


bool CjocUtils::GetTextUntilNoneOf(std::string strInText, std::string strSepChars, std::string *pstrOutText, std::string *pstrCharFound, size_t *pnOffSet)
{
	bool bFound = false;
	bool bEndLine = false;

	size_t posStart = 0;
	if (pnOffSet != NULL){posStart = *pnOffSet;}

	size_t posEnd  = 0;
	size_t lenghtSeparator = 1;

	unsigned int nData = 0;

	while ((posEnd != std::string::npos) && (bFound == false))
	{
		posEnd = strInText.find_first_not_of (strSepChars,posStart);

		if (posEnd != std::string::npos) 
		{
			bFound = true;
			if (pstrOutText != NULL){*pstrOutText = strInText.substr (posStart,posEnd-posStart);}	
			if (pnOffSet != NULL){*pnOffSet = posEnd + lenghtSeparator;}
			if (pstrCharFound != NULL){*pstrCharFound = strInText.substr (posEnd,1);}
		}
	}

	if (posEnd >= std::string::npos)
		bEndLine = true;

	return bEndLine;
}

std::string CjocUtils::GetTimeStr (std::string strFormat)
{
	std::string strRetTime;
	char szTime[128];
	szTime[0] = 0;
	
	time_t t = time(0);
	strftime(szTime, 128, strFormat.c_str (), localtime(&t));

	strRetTime = (std::string)szTime;

	return strRetTime;
}

std::wstring CjocUtils::GetTimeStr (std::wstring strFormat)
{
	std::wstring strRetTime;
	char szTime[128];
	szTime[0] = 0;
	
	time_t t = time(0);
	strftime(szTime, 128, ConvertFromwstr (strFormat).c_str (), localtime(&t));

	strRetTime = ConvertFromChar(szTime);

	return strRetTime;
}

std::string CjocUtils::GetTimeStrFromTime (std::string strFormat, time_t tm)
{
	std::string strRetTime;
	char szTime[128];
	szTime[0] = 0;
	
	strftime(szTime, 128, strFormat.c_str (), localtime(&tm));

	strRetTime = (std::string)szTime;

	return strRetTime;
}

std::wstring CjocUtils::GetTimeStrFromTime (std::wstring strFormat, time_t tm)
{
	std::wstring strRetTime;
	char szTime[128];
	szTime[0] = 0;
	
	strftime(szTime, 128, ConvertFromwstr (strFormat).c_str (), localtime(&tm));

	strRetTime = ConvertFromChar(szTime);

	return strRetTime;
}

std::wstring CjocUtils::lToStr (long lVal)
{
	std::wstring strRet;

	char szBuz[1024];
	
	ltoa(lVal,szBuz,10);

	strRet = ConvertFromChar (szBuz);

	return strRet;
}

std::wstring CjocUtils::ulToStr (unsigned long ulVal)
{
	std::wstring strRet;

	char szBuz[1024];
	
	ultoa(ulVal,szBuz,10);

	strRet = ConvertFromChar (szBuz);

	return strRet;
}

unsigned long CjocUtils::strToul (std::wstring str)
{
	return wcstoul(str.c_str (),NULL,10);
}

std::wstring CjocUtils::dToStr (double dVal, int nPosDecimals)
{
	char szBuz[2048];
	char szFormat[64];

	if (nPosDecimals >= 0)
	{
		sprintf(szFormat, " .%df", nPosDecimals);
		szFormat[0] = '%';
		sprintf(szBuz, szFormat, dVal);
	}
	else
	{
		sprintf(szBuz, "%f", dVal);
	}
		
	std::wstring strRet = CjocUtils::ConvertFromChar (szBuz);

	return strRet;
}

std::wstring CjocUtils::ConvertFromStr (std::string str)
{
	std::wstring strRet;

	strRet.assign (str.begin(),str.end ());

	return strRet ;
}

std::wstring CjocUtils::FindAndReplace (std::wstring strBase, std::wstring strToFind, std::wstring strToReplace)
{
	std::wstring strBaseRet = strBase;

	size_t nPos = strBaseRet .find (strToFind);
	while (nPos != std::wstring::npos)
	{
		strBaseRet .replace (nPos, strToFind.length (), strToReplace );
		nPos = strBaseRet .find(strToFind,nPos+1);
	}

	return strBaseRet;
}

double CjocUtils::modulus(double a, double b)
{
	int result = static_cast<int>( a / b );
	return a - static_cast<double>( result ) * b;
}
