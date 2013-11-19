#include "CjocLog.h"
#include "CjocException.h"
#include <sys/stat.h>
#include "CjocUtils.h"

CjocLog::CjocLog(void)
{
	m_bTimeStamps = true;
}


CjocLog::~CjocLog(void)
{
	
}

void CjocLog::Initialize (std::wstring &strFile, unsigned long lMaxFileSize, bool bAddLogWhenCreateFile)
{
	bool bCreate = false;

	if (strFile.empty () == true){throw JOC_Error_(enjocErrors::err_bad_arguments);}

	FILE *pFile = NULL;
		
	pFile = _wfopen (strFile.c_str (),L"r");
	if (pFile != NULL)
	{
		//Exists
		fclose (pFile);

		//Set file
		m_strFile = strFile;

		struct _stat64i32 buf;
		_wstat(m_strFile.c_str (),&buf);

		if (buf.st_size > lMaxFileSize)
		{
			std::wstring strNewFileName;
			strNewFileName = m_strFile + L".old";

			//Delete .old if exists
			_wremove(strNewFileName.c_str ());

			int nRc = _wrename (m_strFile.c_str (),strNewFileName.c_str ());
			if (nRc != 0){throw JOC_Error_arg_(enjocErrors::err_renaming_file,m_strFile.c_str ());}

			bCreate = true;
		}
	}
	else
	{
		bCreate = true;
	}
	
	if (bCreate == true)
	{
		//Create
		pFile = _wfopen (strFile.c_str (),L"w");
		if (pFile == NULL){throw JOC_Error_arg_(enjocErrors::err_creating_file_for_write,strFile.c_str ());}
		fclose (pFile);

		//Set file
		m_strFile = strFile;

		if (bAddLogWhenCreateFile == true)
			Append((std::wstring)L"Log file created");
	}
}

bool CjocLog::IsInitialized ()
{
	if (m_strFile.empty () == true){return false;}

	return true;
}

void CjocLog::Append(std::wstring &strText, std::wstring &strArg, bool bTimestamp, bool bShowOnScreen)
{
	std::wstring strMsg;
	strMsg = strText;
	strMsg.append (strArg);

	Append(strMsg, bTimestamp, bShowOnScreen);
}

void CjocLog::Append(std::wstring &strText, bool bTimestamp, bool bShowOnScreen)
{
	std::wstring strLog;
	std::wstring strTime;

	if (m_strFile.empty () == true){throw JOC_Error_(enjocErrors::err_bad_state );}

	FILE *pFile = NULL;
		
	pFile = _wfopen (m_strFile.c_str (),L"a+");
	if (pFile == NULL){throw JOC_Error_arg_(enjocErrors::err_opening_file_for_append,m_strFile.c_str ());}

	if (bTimestamp == true)
	{
		strTime = CjocUtils::GetTimeStr (L"%H:%M:%S %d/%m/%Y");
		strTime = strTime + L" - ";		
	}
	
	strLog = strTime;
	strLog.append (strText);
	strLog.append (L"\n");

	int nRc = fputws (strLog.c_str (),pFile);

	fclose(pFile);

	if (nRc < 0){throw JOC_Error_arg_(enjocErrors::err_append_file,m_strFile.c_str ());}

	if (bShowOnScreen == true)
		wprintf (strLog.c_str ());
}

