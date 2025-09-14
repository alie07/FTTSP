// Utils.cpp: implementation of the CDirectory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CDirectory::CDirectory()
//{
//
//}

//CDirectory::~CDirectory()
//{
//
//}


// Try to create Path indicated.
BOOL CDirectory::Create(LPCTSTR path)
{
	TCHAR pOldPath[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH+1, pOldPath);

	CString sPath = path;
	CString sTempDir;

	if (sPath.IsEmpty())
		return TRUE;

	if (sPath[0] != '\\' && (sPath.GetLength() == 1 || sPath[1] != ':'))
	{
		sTempDir = CString(pOldPath);
		sPath = sTempDir + CString("\\") + sPath;
	}

	if (sTempDir.IsEmpty() && sPath.Left(2) == "\\\\")
	{
		int nServerNamePos = sPath.Find('\\', 2);
		if (nServerNamePos == -1)
			return FALSE;
		if (sPath.GetLength() <= nServerNamePos)
			return FALSE;
		int nSharePos = sPath.Find('\\', nServerNamePos + 1);
		if (nSharePos == -1)
			nSharePos = sPath.GetLength();

		sTempDir = sPath.Left(nSharePos);
	}

	sPath.TrimRight('\\');
	sPath+='\\';

	BOOL bSuccess = TRUE;
	int nPos;
	if (sTempDir.IsEmpty())
	{
		//if (sPath.Find(":") != -1)
		//	nPos = sPath.Find('\\') + 1;
		//else
			nPos = sPath.Find('\\');
	}
	else if (sPath.GetLength() > sTempDir.GetLength() + 1)
		nPos = sPath.Find('\\', sTempDir.GetLength() + 2);
	else
		nPos = -1;
	while (nPos != -1)
	{
		sTempDir = sPath.Left(nPos);
		if (! SetCurrentDirectory(sTempDir))
		{
			while (nPos != -1)
			{
				sTempDir = sPath.Left(nPos);
				if (! CreateDirectory(sTempDir, NULL))
				{
					nPos = -1;
					bSuccess = FALSE;
				}
				else
				{
					nPos = sPath.Find('\\', sTempDir.GetLength() +1);
				}
			}
		}
		else
		{
			nPos = sPath.Find('\\', sTempDir.GetLength() +1);
		}
	}

	SetCurrentDirectory(pOldPath);
	return bSuccess;
}

int CStringUtils::ReplaceTag(CString &aString, const CString &aTag, const CString &sReplacement, BOOL bReplaceAll)
{
	CString LowCaseTag = aTag;
	CString LowCaseSrc = aString;
	CString sReplaced;
	int pos;

	LowCaseSrc.MakeLower();
	LowCaseTag.MakeLower();

	pos = LowCaseSrc.Find(LowCaseTag);
	if (pos == -1)
		return 0;

	sReplaced = aString.Left(pos) + sReplacement;

	if (! bReplaceAll)
	{
		// Finish string
		aString = sReplaced + aString.Mid(pos + aTag.GetLength());
		return 1;
	}

	int nCount = 1;
	int nStart = pos + aTag.GetLength();
	while ((pos = LowCaseSrc.Find(LowCaseTag, pos + aTag.GetLength())) != -1)
	{
		sReplaced += aString.Mid(nStart, pos - nStart);
		sReplaced += sReplacement;
		nCount++;
		nStart = pos + aTag.GetLength();
	}
	// Finish string
	aString = sReplaced + aString.Mid(nStart);
	return nCount;
}

BOOL CStringUtils::GetChunk(CString &aString, CString &aChunk, char c)
{
	int index = 0;

	// If input string is empty, this is an error
	if (aString.IsEmpty())
		return FALSE;

	// Look for the separator
	index = aString.Find(c);
	if (index == -1)
	{
		aChunk = aString;
		aString.Empty();
	}
	else
	{
		aChunk = aString.Left(index);
		aString = aString.Mid(index + 1);
	}

	aChunk.TrimLeft();
	aChunk.TrimRight();

	return TRUE;
}

BOOL CStringUtils::GetChunk(CString &aString, DOUBLE &aDouble, char c)
{
	CString strDouble;
	BOOL result = FALSE;

	if (GetChunk(aString, strDouble, c))
	{
		LPSTR lpstr, stop_string;

		lpstr = strDouble.LockBuffer();
		aDouble = strtod(lpstr, &stop_string);
		if (*stop_string == '\0') // All string has been analysed
			result = TRUE;
		strDouble.UnlockBuffer();
	}

	return result;
}

BOOL CStringUtils::GetChunk(CString &aString, CWordArray &aWArray, char c)
{
	CString strWA;
	BOOL result = FALSE;

	if (GetChunk(aString, strWA, c))
	{
		LONG aLong;

		aWArray.RemoveAll();
		while(GetChunk(strWA, aLong, '_'))
		{
			aWArray.Add((WORD) aLong);
		}
		if (aWArray.GetSize() > 0)
			result = TRUE;
	}
	return result;
}

BOOL CStringUtils::GetChunk(CString &aString, LONG &aInt, char c)
{
	CString strDouble;
	BOOL result = FALSE;

	if (GetChunk(aString, strDouble, c))
	{
		LPSTR lpstr, stop_string;

		lpstr = strDouble.LockBuffer();
		aInt = strtol(lpstr, &stop_string, 10);
		if (*stop_string == '\0') // All string has been analysed
			result = TRUE;
		strDouble.UnlockBuffer();
	}

	return result;
}

BOOL CStringUtils::GetChunk (CString &aString, BOOL &aBool  , char c)
{
	CString strBool;

	if (GetChunk(aString, strBool, c))
	{
		if (strBool == "TRUE" ||
			strBool == "1")
			aBool = TRUE;
		else
			aBool = FALSE;
		return TRUE;
	}

	return FALSE;
}

// Return if process name (name.exe) is running
BOOL CProcessUtils::IsProcess32Running(const CString processName)
{
	BOOL exists = FALSE;
	CString sExeFile;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry))
	{
        while (Process32Next(snapshot, &entry))
		{
			sExeFile.Format("%s", entry.szExeFile);
            if (sExeFile == processName)
                exists = TRUE;
		}
	}

    CloseHandle(snapshot);
    return exists;
}

// Blocking wait based on WaitableTimer
BOOL CTimeUtils::WaitSeconds(int seconds, HANDLE hTimer)
{
	ResetEvent(hTimer);
    LARGE_INTEGER liDueTime;

    liDueTime.QuadPart = -1 * (seconds * 10000000);

    // Check waitable handle
    if (NULL == hTimer)
    {
        TRACE("Waitable handle not set: (%d)\n", GetLastError());
		return FALSE;
    }

    // Set a timer to wait for x seconds.
    if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
    {
        TRACE("SetWaitableTimer failed (%d)\n", GetLastError());
		CloseHandle(hTimer);
		return FALSE;
    }

    // Blocking wait for the timer.
    if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
	{
        TRACE("WaitForSingleObject failed (%d)\n", GetLastError());
		CloseHandle(hTimer);
		return FALSE;
	}
    else
	{
		TRACE("Timer was signaled.\n");
	}

	return TRUE;
}

BOOL CTimeUtils::SetTimerSecond(int seconds, HANDLE hTimer)
{
	ResetEvent(hTimer);
    LARGE_INTEGER liDueTime;

    liDueTime.QuadPart = -1 * (seconds * 10000000);

    // Check waitable handle
    if (NULL == hTimer)
    {
        TRACE("Waitable handle not set: (%d)\n", GetLastError());
		return FALSE;
    }

    // Set a timer to wait for x seconds.
    if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
    {
        TRACE("SetWaitableTimer failed (%d)\n", GetLastError());
		CloseHandle(hTimer);
		return FALSE;
    }

	return TRUE;	
}

// Wait until handle signaled
BOOL CTimeUtils::WaitWithMsgPump(int milliSecs, HANDLE hTimer)
{
    LARGE_INTEGER liDueTime;

    liDueTime.QuadPart = -1 * (milliSecs * 10000);

    // Check waitable handle
    if (NULL == hTimer)
    {
        TRACE("Waitable handle not set: (%d)\n", GetLastError());
		return FALSE;
    }

    // Set a timer to wait for x seconds.
    if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
    {
        TRACE("SetWaitableTimer failed (%d)\n", GetLastError());
		CloseHandle(hTimer);
		return FALSE;
    }

	TRACE(_T("Start wait thread for %d millisecs...\n"), milliSecs);

	while (WaitForSingleObject(hTimer, 0) != WAIT_OBJECT_0) // Block until hTimer is not signaled
	{
		AfxGetThread()->PumpMessage();
		Sleep(100);
	}

	TRACE(_T("Handle signaled => wait thread finished!\n"));

	return TRUE;
}

// Wait until flag is not set to TRUE
BOOL CTimeUtils::WaitWithMsgPump(int milliSecs, BOOL volatile &flag)
{
    LARGE_INTEGER liDueTime;

    liDueTime.QuadPart = -1 * (milliSecs * 10000);

	TRACE(_T("Start WaitWithMsgPump with flag = %d, and timeout = %ds\n"), flag, milliSecs/1000);

    // Create waitable handle
    HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (NULL == hTimer)
    {
        TRACE(_T("Waitable handle not set: (%d)\n"), GetLastError());
		return FALSE;
    }

    // Set a timer to wait
    if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
    {
        TRACE(_T("SetWaitableTimer failed (%d)\n"), GetLastError());
		CloseHandle(hTimer);
		return FALSE;
    }

	// Wait loop
	while ((WaitForSingleObject(hTimer, 0) != WAIT_OBJECT_0) && !flag) // Block until flag is not set to TRUE
	{
		AfxGetThread()->PumpMessage();
		TRACE(_T("Message pump, flag = %d\n"), flag);
		Sleep(100);
	}

	TRACE(_T("WaitWithMsgPump ends with flag = %d\n"), flag);

	// Clean
	CancelWaitableTimer(hTimer);
	hTimer = NULL;

	return TRUE;
}

// Blocking wait based on WaitableTimer
BOOL CTimeUtils::WaitFlag(BOOL volatile &flag, int milliSecs)
{
    LARGE_INTEGER liDueTime;

    liDueTime.QuadPart = -1 * (milliSecs * 10000);

	TRACE(_T("Start wait loop with flag = %d, and timeout = %ds\n"), flag, milliSecs/1000);

    // Create waitable handle
    HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (NULL == hTimer)
    {
        TRACE(_T("Waitable handle not set: (%d)\n"), GetLastError());
		return FALSE;
    }

    // Set a timer to wait
    if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
    {
        TRACE(_T("SetWaitableTimer failed (%d)\n"), GetLastError());
		CloseHandle(hTimer);
		return FALSE;
    }

	// Wait loop
	while ((WaitForSingleObject(hTimer, 0) != WAIT_OBJECT_0) && !flag) // Block until flag is not set to TRUE
	{
		TRACE(_T("wait loop flag = %d\n"), flag);
		Sleep(100);
	}

	TRACE(_T("Wait loop ends with flag = %d\n"), flag);

	// Clean
	CancelWaitableTimer(hTimer);
	hTimer = NULL;

	return TRUE;
}
