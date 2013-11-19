#pragma once

#include "jocCommonHeader.h"

class JOC_EXPORTS CjocException :
	public std::exception 
{

public:

	CjocException(void);
	CjocException(int nCode, const std::wstring& strErr, const std::string& strFunc, const std::string& strFile, int nLine);

	~CjocException(void);	

    /*!
     \return the error description and the context as a text string.
    */ 
	virtual const char* what();

private:
    
    std::wstring m_wstrMsg; ///< the formatted error message
	std::string m_strMsg; ///< the formatted error message

    int m_ncode; 
    std::wstring m_strErr; ///< error description
    std::wstring m_strFuncName; ///< function name. Available only when the compiler supports __func__ macro
    std::wstring m_strFileName; ///< source file name where the error has occured
    int m_nLine; ///< line number in the source file where the error has occured 
};
