#include "stdafx.h"
#include "SystemTray.h"
#include "WindowLayoutManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SystemTray::SystemTray(void)
    : m_hwnd(NULL)
    , m_notifyIconData(NULL)
{
}

SystemTray::~SystemTray(void)
{
    destroyTray();
}

bool SystemTray::createTray(HWND aWnd, unsigned int aMsg, unsigned int aId, const TCHAR *aToolTip, HICON aIcon)
{
    m_notifyIconData = new NOTIFYICONDATA;

    memset(m_notifyIconData, 0, sizeof(NOTIFYICONDATA));
    m_notifyIconData->cbSize           = sizeof(NOTIFYICONDATA);
    m_notifyIconData->uFlags           = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    m_notifyIconData->hWnd             = aWnd;
    m_notifyIconData->hIcon            = aIcon;
    m_notifyIconData->uCallbackMessage = aMsg;
    m_notifyIconData->uID              = aId;
    _tcscpy_s(m_notifyIconData->szTip, sizeof(m_notifyIconData->szInfo) / sizeof(m_notifyIconData->szInfo[0]), aToolTip);

    bool sResult = ::Shell_NotifyIcon(NIM_ADD, m_notifyIconData);
    if (sResult)
    {
        m_hwnd = aWnd;
    }
    else
    {
        destroyTray();
    }

    return sResult;
}

void SystemTray::destroyTray(void)
{
    if (m_notifyIconData)
    {
        ::Shell_NotifyIcon(NIM_DELETE, m_notifyIconData);
        ::DestroyIcon(m_notifyIconData->hIcon);
        delete m_notifyIconData;
        m_notifyIconData = NULL;
        m_hwnd = NULL;
    }
}

bool SystemTray::recreateTray(void)
{
    NOTIFYICONDATA sNotifyIconData;
    memcpy(&sNotifyIconData, m_notifyIconData, sizeof(NOTIFYICONDATA));
    sNotifyIconData.hIcon = ::DuplicateIcon(theApp.m_hInstance, m_notifyIconData->hIcon);

    destroyTray();

    return createTray(sNotifyIconData.hWnd, sNotifyIconData.uCallbackMessage, sNotifyIconData.uID, sNotifyIconData.szTip, sNotifyIconData.hIcon);
}

void SystemTray::showWindow(bool aShow, bool aAnimate)
{
    if (aShow)
    {
        showFromTray(aAnimate);
    }
    else
    {
        hideToTray(aAnimate);
    }
}

void SystemTray::toggleWindow(bool aAnimate)
{
    if (::IsWindowVisible(m_hwnd) == FALSE)
    {
        showFromTray(aAnimate);
    }
    else
    {
        hideToTray(aAnimate);
    }
}

void SystemTray::showFromTray(bool aAnimate)
{
    ::SetForegroundWindow(m_hwnd);

    unsigned int uStyle = CWnd::FromHandle(m_hwnd)->GetStyle();
    if ((uStyle & WS_MINIMIZE) == WS_MINIMIZE)
    {
        ::ShowWindow(m_hwnd, SW_RESTORE);
    }
    else
    {
        animateMaximizeFromTray();
        ::ShowWindow(m_hwnd, SW_SHOW);
    }
}

void SystemTray::hideToTray(bool aAnimate)
{
    animateMinimizeToTray();
    ::ShowWindow(m_hwnd, SW_HIDE);
}

void SystemTray::animateMinimizeToTray(void)
{
    CRect sFromRect;
    ::GetWindowRect(m_hwnd, &sFromRect);

    CRect sToRect;
    getTrayWndRect(&sToRect);

    ::DrawAnimatedRects(m_hwnd, IDANI_CAPTION, &sFromRect, &sToRect);
}

void SystemTray::animateMaximizeFromTray(void)
{
    CRect sFromRect;
    getTrayWndRect(&sFromRect);

    // I cannot get window rectagle by GetWindowRect API function on minimized window state.
    WINDOWPLACEMENT sWindowPlacement;
    ::GetWindowPlacement(m_hwnd, &sWindowPlacement);

    ::DrawAnimatedRects(m_hwnd, IDANI_CAPTION, &sFromRect, &sWindowPlacement.rcNormalPosition);
}

void SystemTray::getTrayWndRect(LPRECT aRect)
{
    HWND aTaskBarHwnd = ::FindWindow(_T("Shell_TrayWnd"), NULL);
    if (aTaskBarHwnd)
    {
        HWND sTrayHwnd = ::FindWindowEx(aTaskBarHwnd, NULL, _T("TrayNotifyWnd"), NULL);
        if (sTrayHwnd)
            ::GetWindowRect(sTrayHwnd, aRect);
        else
        {
            ::GetWindowRect(aTaskBarHwnd, aRect);
            aRect->left = aRect->right  - 20;
            aRect->top  = aRect->bottom - 20;
        }
    }
    else
    {
        int sWidth  = GetSystemMetrics(SM_CXSCREEN);
        int sHeight = GetSystemMetrics(SM_CYSCREEN);
        SetRect(aRect, sWidth-40, sHeight-20, sWidth, sHeight);
    }
}
