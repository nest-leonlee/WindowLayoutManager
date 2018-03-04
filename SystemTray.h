#ifndef __SYSTEM_TRAY_H__
#define __SYSTEM_TRAY_H__
#pragma once

class SystemTray
{
public:
    SystemTray(void);
    virtual ~SystemTray(void);

public:
    bool createTray(HWND aHwnd, unsigned int aMsg, unsigned int aId, const TCHAR *aToolTip, HICON aIcon);
    void destroyTray(void);
    bool recreateTray(void);

    void showWindow(bool aShow, bool aAnimate = true);
    void toggleWindow(bool aAnimate = true);

    void showFromTray(bool aAnimate = true);
    void hideToTray(bool aAnimate = true);

public:
    static void getTrayWndRect(LPRECT aRect);

protected:
    void animateMinimizeToTray(void);
    void animateMaximizeFromTray(void);

protected:
    HWND            m_hwnd;
    NOTIFYICONDATA *m_notifyIconData;
};

#endif // __SYSTEM_TRAY_H__
