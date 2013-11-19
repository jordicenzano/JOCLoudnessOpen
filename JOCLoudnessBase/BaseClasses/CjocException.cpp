#include "CjocException.h"
#include "CjocUtils.h"

CjocException::CjocException(void)
{
    m_ncode = 0; 
    int m_nline = -1;
}

CjocException::CjocException(int nCode, const std::wstring& strErr, const std::string& strFunc, const std::string& strFile, int nLine)
{
    m_ncode = nCode; 
    m_strErr = strErr;
    m_strFuncName = CjocUtils::ConvertFromStr (strFunc); 
    m_strFileName = CjocUtils::ConvertFromStr (strFile);
    m_nLine = nLine;

	m_wstrMsg = L"Err: ";
	m_wstrMsg.append (CjocUtils::lToStr (nCode));
	m_wstrMsg.append (L" (");
	m_wstrMsg.append (m_strErr);
	m_wstrMsg.append (L")\nFunction: ");
	m_wstrMsg.append (m_strFuncName);
	m_wstrMsg.append (L" \nFile: ");
	m_wstrMsg.append (m_strFileName);
	m_wstrMsg.append (L" \nLine: ");
	m_wstrMsg.append (CjocUtils::lToStr (m_nLine));

	m_strMsg = CjocUtils::ConvertFromwstr (m_wstrMsg);
}

const char* CjocException::what()
{
	return m_strMsg.c_str ();
}

CjocException::~CjocException(void)
{

}

