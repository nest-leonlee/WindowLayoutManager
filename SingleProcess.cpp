#include "stdafx.h"
#include "SingleProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
const TCHAR        singleProcessMutexName[] = _T("WindowLayoutManager - single process");
const unsigned int WM_SINGLE_PROCESS         = ::RegisterWindowMessage(_T("WindowLayoutManager - single process"));
} // anonymous namespace

HANDLE SingleProcess::singleProcessMutex = NULL;

bool SingleProcess::check(void)
{
    HANDLE sSingleProcessMutex;

    sSingleProcessMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, singleProcessMutexName);
    if (sSingleProcessMutex)
    {
        return false;
    }

    ::CloseHandle(singleProcessMutex);
    singleProcessMutex = NULL;

    return true;
}

void SingleProcess::lock(void)
{
    if (!singleProcessMutex)
    {
        singleProcessMutex = ::CreateMutex(NULL, FALSE, singleProcessMutexName);
    }
}

void SingleProcess::unlock(void)
{
    if (singleProcessMutex)
    {
        ::CloseHandle(singleProcessMutex);
        singleProcessMutex = NULL;
    }
}

unsigned int SingleProcess::getMsg(void)
{
    return WM_SINGLE_PROCESS;
}

void SingleProcess::postMsg(void)
{
    ::PostMessage(HWND_BROADCAST, WM_SINGLE_PROCESS, 0, 0);
}
