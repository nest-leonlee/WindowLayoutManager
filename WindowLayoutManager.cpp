#include "stdafx.h"
#include "WindowLayoutManager.h"
#include "WindowLayoutManagerDlg.h"
#include "SingleProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWindowLayoutManagerApp theApp;

CWindowLayoutManagerApp::CWindowLayoutManagerApp()
{
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

BEGIN_MESSAGE_MAP(CWindowLayoutManagerApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

BOOL CWindowLayoutManagerApp::InitInstance()
{
    INITCOMMONCONTROLSEX initCtrls;
    initCtrls.dwSize = sizeof(initCtrls);
    initCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&initCtrls);

    CWinApp::InitInstance();

    if (!AfxSocketInit())
    {
        AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        return FALSE;
    }

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization
    SetRegistryKey(_T("WindowLayoutManager"));

    if (!SingleProcess::check())
    {
        SingleProcess::postMsg();

        return FALSE;
    }

    SingleProcess::lock();

    CWindowLayoutManagerDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with OK
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with Cancel
    }
    else if (nResponse == -1)
    {
        TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}

int CWindowLayoutManagerApp::ExitInstance()
{
    SingleProcess::unlock();

    return CWinApp::ExitInstance();
}
