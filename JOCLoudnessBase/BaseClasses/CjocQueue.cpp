#include "CjocQueue.h"
#include "CjocException.h"

CjocQueue::CjocQueue(void)
{
	m_nSize = 0;
	m_nFront = -1;

	m_pData = NULL;
}

void CjocQueue::Ini (int nSize, double dVal)
{
	m_nSize = nSize;
	
	if (m_pData != NULL)
	{
		delete m_pData;
		m_pData = NULL;
	}

	m_pData = new double [m_nSize];
	if (m_pData == NULL){throw JOC_Error_(enjocErrors::err_out_of_memory);}

	for (int n = 0; n < m_nSize; n++)
		m_pData[n] = dVal;

	m_nFront = 0;
}

void CjocQueue::PushFront (double &dVal)
{
	m_pData[m_nFront] = dVal;
	m_nFront++;
	if (m_nFront >= m_nSize)
		m_nFront = 0;
}

double CjocQueue::GetAt(int &nIndex)
{//0 = Last to enter

	int nIndexPos = m_nFront - nIndex - 1;
	if (nIndexPos < 0)
		nIndexPos = m_nSize + nIndexPos;

	return 	m_pData[nIndexPos];
}

CjocQueue::~CjocQueue(void)
{
	if (m_pData != NULL)
	{
		delete m_pData;
		m_pData = NULL;
	}
}

