#pragma once

#include "jocCommonHeader.h"
#include <vector>

class JOC_EXPORTS CjocUtils
{
public:
	CjocUtils(void);
	~CjocUtils(void);

	//String
	static std::string ConvertFromwstr (std::wstring wstr);
	static std::wstring ConvertFromChar (const char *szStr);
	static std::wstring ConvertFromStr (std::string str);

	static std::wstring FindAndReplace (std::wstring strBase, std::wstring strToFind, std::wstring strToReplace);

	//Conversions
	static std::wstring lToStr (long lVal);
	static std::wstring ulToStr (unsigned long ulVal);
	static std::wstring dToStr (double dVal, int nPosDecimals = 2);

	static unsigned long strToul (std::wstring str);


	static std::string Trim(std::string& str);
	static std::wstring Trim(std::wstring& str);

	static bool FindParamD (std::vector<std::wstring> &strArray, std::wstring strParam, double *pdVal);
	static bool FindParamStr (std::vector<std::wstring> &strArray, std::wstring strParam, std::wstring *pstrVal);

	static std::wstring ToLower (std::wstring& str);
	static std::wstring ToUpper (std::wstring& str);
	static bool IsNumeric (std::wstring& str);
	

	//Time
	static std::string GetTimeStr (std::string strFormat);
	static std::wstring GetTimeStr (std::wstring strFormat);

	static std::string GetTimeStrFromTime (std::string strFormat, time_t tm);
	static std::wstring GetTimeStrFromTime (std::wstring strFormat, time_t tm);

	//File
	static std::string GetFileWithoutExt (std::string strFile);
	static std::wstring GetFileWithoutExt (std::wstring strFile);

	static bool GetParamFromLineWithSeparators (std::string *pstrRet, std::string strLine, int nIndex, std::string strSeparator);
	static bool GetParamFromLineWithSeparators (std::wstring *pstrRet, std::wstring strLine, int nIndex, std::wstring strSeparator);

	static std::string GetFileFromPath (std::string strFilePath);
	static std::string AddPathFile (std::string strDir,std::string strFile);
	static std::string GetPathFromFilePath (std::string strFilePath);
	static bool GetTextUntilNoneOf(std::string strInText, std::string strSepChars, std::string *pstrOutText, std::string *pstrCharFound, size_t *pnOffSet);

	//Math
	static double modulus(double a, double b);
};