#pragma once

class SystemTray
{
public:
    SystemTray();
    virtual ~SystemTray();

public:
    bool createTray(HWND hwnd, unsigned int msg, unsigned int id, const TCHAR *tooltip, HICON icon);
    void destroyTray();
    bool recreateTray();

    void showWindow(bool show, bool animate = true);
    void toggleWindow(bool animate = true);

    void showFromTray(bool animate = true);
    void hideToTray(bool animate = true);

public:
    static void getTrayWndRect(LPRECT rect);

protected:
    void animateMinimizeToTray();
    void animateMaximizeFromTray();

protected:
    HWND            hwnd;
    NOTIFYICONDATA *notifyIconData;
};
