#pragma once

#include "jocCommonHeader.h"

const unsigned long MAXLOGSIZE = (unsigned long)1048576;

class JOC_EXPORTS CjocLog
{
public:
	CjocLog(void);
	~CjocLog(void);

	void Initialize (std::wstring &strFile, unsigned long lMaxFileSize = 0, bool bAddLogWhenCreateFile = true);
	void Append(std::wstring &strText, bool bTimestamp = true, bool bShowOnScreen = true);
	void Append(std::wstring &strText, std::wstring &strArg, bool bTimestamp = true, bool bShowOnScreen = true);

	bool IsInitialized ();

private:
	std::wstring m_strFile;
	bool m_bTimeStamps;
	
};

