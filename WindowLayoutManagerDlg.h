#pragma once
#include "afxcmn.h"
#include "SystemTray.h"

class CWindowLayoutManagerDlg : public CDialog
{
public:
    CWindowLayoutManagerDlg(CWnd* pParent = NULL); // standard constructor

public:
    void addWindow(HWND hwnd);

private:
    void createTray();
    void minimizeToTray();

    bool setForceForegroundWindow(HWND aHwnd);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

private:
    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_WINDOW_LAYOUT_MANAGER_DIALOG };
#endif

    HICON m_hIcon;
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
    afx_msg void OnBnClickedDelete();
    afx_msg void OnBnClickedRestore();
    afx_msg void OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTaskRestarted(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSingleProcess(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnDestroy();
};
