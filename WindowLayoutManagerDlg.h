#pragma once

#include "afxcmn.h"
#include "SystemTray.h"
#include "ResizingDialog.h"

#include <list>

typedef std::list<MONITORINFOEX> MonitorList;

class CWindowLayoutManagerDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    CWindowLayoutManagerDlg(CWnd* pParent = NULL); // standard constructor

public:
    void addWindow(HWND hwnd);

private:
    void createTray();
    void minimizeToTray();

    bool setForceForegroundWindow(HWND hwnd);

    void timerScan();
    void timerRestore();

    void lockScan(bool lock);

private:
    struct MonitorInfo
    {
        MonitorList list;
    };

    bool saved;
    MonitorInfo savedMonitorInfo;

    bool locked;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_WINDOW_LAYOUT_MANAGER_DIALOG };
#endif

    HICON iconApp;
    CListCtrl listWindow;
    SystemTray systemTray;

protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
    afx_msg void OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedScan();
    afx_msg void OnBnClickedWho();
    afx_msg void OnBnClickedDelete();
    afx_msg void OnBnClickedLock();
    afx_msg void OnBnClickedRestore();
    afx_msg void OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTaskRestarted(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSingleProcess(WPARAM wParam, LPARAM lParam);
    afx_msg void OnDestroy();
    afx_msg void OnDisplayChange(UINT nImageDepth, int cxScreen, int cyScreen);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
