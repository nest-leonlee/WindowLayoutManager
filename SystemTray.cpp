#include "stdafx.h"
#include "SystemTray.h"
#include "WindowLayoutManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SystemTray::SystemTray()
    : hwnd(NULL)
    , notifyIconData(NULL)
{
}

SystemTray::~SystemTray()
{
    destroyTray();
}

bool SystemTray::createTray(HWND hwnd, unsigned int msg, unsigned int id, const TCHAR *tooltip, HICON icon)
{
    notifyIconData = new NOTIFYICONDATA;

    memset(notifyIconData, 0, sizeof(NOTIFYICONDATA));
    notifyIconData->cbSize           = sizeof(NOTIFYICONDATA);
    notifyIconData->uFlags           = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    notifyIconData->hWnd             = hwnd;
    notifyIconData->hIcon            = icon;
    notifyIconData->uCallbackMessage = msg;
    notifyIconData->uID              = id;
    _tcscpy_s(notifyIconData->szTip, sizeof(notifyIconData->szInfo) / sizeof(notifyIconData->szInfo[0]), tooltip);

    bool sResult = ::Shell_NotifyIcon(NIM_ADD, notifyIconData);
    if (sResult)
    {
        this->hwnd = hwnd;
    }
    else
    {
        destroyTray();
    }

    return sResult;
}

void SystemTray::destroyTray()
{
    if (notifyIconData)
    {
        ::Shell_NotifyIcon(NIM_DELETE, notifyIconData);
        ::DestroyIcon(notifyIconData->hIcon);
        delete notifyIconData;
        notifyIconData = NULL;
        hwnd = NULL;
    }
}

bool SystemTray::recreateTray()
{
    NOTIFYICONDATA sNotifyIconData;
    memcpy(&sNotifyIconData, notifyIconData, sizeof(NOTIFYICONDATA));
    sNotifyIconData.hIcon = ::DuplicateIcon(theApp.m_hInstance, notifyIconData->hIcon);

    destroyTray();

    return createTray(sNotifyIconData.hWnd, sNotifyIconData.uCallbackMessage, sNotifyIconData.uID, sNotifyIconData.szTip, sNotifyIconData.hIcon);
}

void SystemTray::showWindow(bool show, bool animate)
{
    if (show)
    {
        showFromTray(animate);
    }
    else
    {
        hideToTray(animate);
    }
}

void SystemTray::toggleWindow(bool animate)
{
    if (::IsWindowVisible(hwnd) == FALSE)
    {
        showFromTray(animate);
    }
    else
    {
        hideToTray(animate);
    }
}

void SystemTray::showFromTray(bool animate)
{
    ::SetForegroundWindow(hwnd);

    unsigned int uStyle = CWnd::FromHandle(hwnd)->GetStyle();
    if ((uStyle & WS_MINIMIZE) == WS_MINIMIZE)
    {
        ::ShowWindow(hwnd, SW_RESTORE);
    }
    else
    {
        animateMaximizeFromTray();
        ::ShowWindow(hwnd, SW_SHOW);
    }
}

void SystemTray::hideToTray(bool animate)
{
    animateMinimizeToTray();
    ::ShowWindow(hwnd, SW_HIDE);
}

void SystemTray::animateMinimizeToTray()
{
    CRect sFromRect;
    ::GetWindowRect(hwnd, &sFromRect);

    CRect sToRect;
    getTrayWndRect(&sToRect);

    ::DrawAnimatedRects(hwnd, IDANI_CAPTION, &sFromRect, &sToRect);
}

void SystemTray::animateMaximizeFromTray()
{
    CRect sFromRect;
    getTrayWndRect(&sFromRect);

    // I cannot get window rectagle by GetWindowRect API function on minimized window state.
    WINDOWPLACEMENT sWindowPlacement;
    ::GetWindowPlacement(hwnd, &sWindowPlacement);

    ::DrawAnimatedRects(hwnd, IDANI_CAPTION, &sFromRect, &sWindowPlacement.rcNormalPosition);
}

void SystemTray::getTrayWndRect(LPRECT rect)
{
    HWND aTaskBarHwnd = ::FindWindow(_T("Shell_TrayWnd"), NULL);
    if (aTaskBarHwnd)
    {
        HWND sTrayHwnd = ::FindWindowEx(aTaskBarHwnd, NULL, _T("TrayNotifyWnd"), NULL);
        if (sTrayHwnd)
            ::GetWindowRect(sTrayHwnd, rect);
        else
        {
            ::GetWindowRect(aTaskBarHwnd, rect);
            rect->left = rect->right  - 20;
            rect->top  = rect->bottom - 20;
        }
    }
    else
    {
        int sWidth  = GetSystemMetrics(SM_CXSCREEN);
        int sHeight = GetSystemMetrics(SM_CYSCREEN);
        SetRect(rect, sWidth-40, sHeight-20, sWidth, sHeight);
    }
}
