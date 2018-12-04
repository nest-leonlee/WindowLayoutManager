#include "stdafx.h"
#include "WindowLayoutManager.h"
#include "WindowLayoutManagerDlg.h"
#include "AboutDlg.h"
#include "SystemTray.h"
#include "SingleProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
struct ListItemData
{
    HWND hwnd;
    WINDOWPLACEMENT wp;
    DWORD pid;
};

enum
{
    WM_TRAY_MESSAGE = WM_USER + 1,
};

enum
{
    TIMER_SCAN = 1,
    TIMER_RESTORE,
};

unsigned int WM_TASK_RESTARTED = ::RegisterWindowMessage(_T("TaskbarCreated"));
unsigned int WM_SINGLE_PROCESS = SingleProcess::getMsg();

BOOL MonitorEnumProc(HMONITOR hMonitor, HDC dc, LPRECT rc, LPARAM lParam)
{
    MonitorList* list = reinterpret_cast<MonitorList*>(lParam);

    MONITORINFOEX miex;
    memset(&miex, 0, sizeof(miex));
    miex.cbSize = sizeof(miex);
    if (GetMonitorInfo(hMonitor, &miex))
    {
        list->push_back(miex);
    }

    return TRUE;
}

void getMonitorList(MonitorList& list)
{
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&list);
}
} // namespace anonymous

CWindowLayoutManagerDlg::CWindowLayoutManagerDlg(CWnd* pParent /*=NULL*/)
    : super(IDD_WINDOW_LAYOUT_MANAGER_DIALOG, pParent)
    , saved(false)
    , locked(false)
{
    iconApp = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWindowLayoutManagerDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, listWindow);
}

BEGIN_MESSAGE_MAP(CWindowLayoutManagerDlg, super)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST, OnLvnGetdispinfoList)
    ON_NOTIFY(LVN_DELETEITEM, IDC_LIST, OnLvnDeleteitemList)
    ON_BN_CLICKED(IDOK, &CWindowLayoutManagerDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CWindowLayoutManagerDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_SCAN, &CWindowLayoutManagerDlg::OnBnClickedScan)
    ON_BN_CLICKED(IDC_WHO, &CWindowLayoutManagerDlg::OnBnClickedWho)
    ON_BN_CLICKED(IDC_DELETE, &CWindowLayoutManagerDlg::OnBnClickedDelete)
    ON_BN_CLICKED(IDC_LOCK, &CWindowLayoutManagerDlg::OnBnClickedLock)
    ON_BN_CLICKED(IDC_RESTORE, &CWindowLayoutManagerDlg::OnBnClickedRestore)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST, &CWindowLayoutManagerDlg::OnLvnKeydownList)
    ON_REGISTERED_MESSAGE(WM_TASK_RESTARTED, OnTaskRestarted)
    ON_REGISTERED_MESSAGE(WM_SINGLE_PROCESS, OnSingleProcess)
    ON_MESSAGE(WM_TRAY_MESSAGE, OnTrayNotify)
    ON_WM_DISPLAYCHANGE()
    ON_WM_DESTROY()
    ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CWindowLayoutManagerDlg::OnInitDialog()
{
    super::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu *sysMenu = GetSystemMenu(FALSE);
    if (sysMenu != NULL)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            sysMenu->AppendMenu(MF_SEPARATOR);
            sysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(iconApp, TRUE);  // Set big icon
    SetIcon(iconApp, FALSE); // Set small icon

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    //sizeNone:     Don't resize at all
    //sizeResize:   The control will be stretched in the appropriate direction
    //sizeRepos:    The control will be moved in the appropriate direction
    //sizeRelative: The control will be moved proportionally in the appropriate direction
    AddControl(IDC_LIST,    sizeResize, sizeResize);
    AddControl(IDC_SCAN,    sizeNone,   sizeRepos, FALSE);
    AddControl(IDC_WHO,     sizeNone,   sizeRepos, FALSE);
    AddControl(IDC_DELETE,  sizeNone,   sizeRepos, FALSE);
    AddControl(IDC_LOCK,    sizeNone,   sizeRepos, FALSE);
    AddControl(IDC_RESTORE, sizeRepos,  sizeRepos, FALSE);
    //------------------------------------------------------------

    createTray();

    listWindow.SetExtendedStyle(listWindow.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    listWindow.InsertColumn(0, _T("Window title"), LVCFMT_LEFT, 300, -1);
    listWindow.InsertColumn(1, _T("HWND"),         LVCFMT_LEFT, 100, -1);
    listWindow.InsertColumn(2, _T("PID"),          LVCFMT_LEFT, 100, -1);
    listWindow.InsertColumn(3, _T("Program"),      LVCFMT_LEFT, 150, -1);
    listWindow.InsertColumn(4, _T("Position"),     LVCFMT_LEFT, 100, -1);
    listWindow.InsertColumn(5, _T("Size"),         LVCFMT_LEFT, 100, -1);
    listWindow.InsertColumn(6, _T("Show status"),  LVCFMT_LEFT, 200, -1);

    lockScan(locked);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWindowLayoutManagerDlg::OnDestroy()
{
    super::OnDestroy();

    systemTray.destroyTray();
}

void CWindowLayoutManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlg;
        dlg.DoModal();
    }
    else if (nID == SC_CLOSE)
    {
        minimizeToTray();
    }
    else
    {
        super::OnSysCommand(nID, lParam);
    }
}

void CWindowLayoutManagerDlg::minimizeToTray()
{
    systemTray.hideToTray();

    SetForegroundWindow();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWindowLayoutManagerDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, iconApp);
    }
    else
    {
        super::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWindowLayoutManagerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(iconApp);
}

void CWindowLayoutManagerDlg::createTray()
{
    unsigned int iconId = IDR_MAINFRAME;
    HICON icon = (HICON)::LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(iconId), IMAGE_ICON, 0, 0, 0);

    if (!systemTray.createTray(GetSafeHwnd(), WM_TRAY_MESSAGE, IDS_TRAY_NOTIFY, _T("Window Layout Manager"), icon))
    {
        ::DestroyIcon(icon);
    }
}

LRESULT CWindowLayoutManagerDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
    if (wParam == IDS_TRAY_NOTIFY)
    {
        switch (lParam)
        {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            systemTray.showFromTray();
            break;
        }

        case WM_RBUTTONUP:
        {
            SetForegroundWindow();

            CPoint pt;
            ::GetCursorPos(&pt);

            CMenu menu;
            if (menu.LoadMenu(IDR_TRAY))
            {
                CMenu *popupMenu = (CMenu *)menu.GetSubMenu(0);
                if (popupMenu)
                {
                    popupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
                }
            }

            break;
        }
        }
    }

    return 0;
}

LRESULT CWindowLayoutManagerDlg::OnTaskRestarted(WPARAM wParam, LPARAM lParam)
{
    systemTray.recreateTray();

    return 0;
}

BOOL CWindowLayoutManagerDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case ID_TRAY_APP_SHOW:
        systemTray.showFromTray();
        break;

    case ID_TRAY_APP_EXIT:
        OnOK();
        break;
    }

    return super::OnCommand(wParam, lParam);
}

bool CWindowLayoutManagerDlg::setForceForegroundWindow(HWND hwnd)
{
    bool ret;

    HWND foregroundHwnd = ::GetForegroundWindow();

    ::AttachThreadInput(::GetWindowThreadProcessId(foregroundHwnd, NULL), ::GetCurrentThreadId(), TRUE);
    ret = ::SetForegroundWindow(hwnd);
    ::AttachThreadInput(::GetWindowThreadProcessId(foregroundHwnd, NULL), ::GetCurrentThreadId(), FALSE);

    return ret;
}

LRESULT CWindowLayoutManagerDlg::OnSingleProcess(WPARAM wParam, LPARAM lParam)
{
    setForceForegroundWindow(m_hWnd);

    CWnd::ShowWindow(::IsIconic(m_hWnd) ? SW_RESTORE : SW_SHOW);
    UpdateWindow();

    return 0;
}

BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lParam)
{
    CWindowLayoutManagerDlg *dlg = (CWindowLayoutManagerDlg *)lParam;
    dlg->addWindow(hwnd);

    return TRUE;
}

void CWindowLayoutManagerDlg::OnBnClickedScan()
{
    listWindow.SetRedraw(FALSE);

    listWindow.DeleteAllItems();
    EnumWindows(enumWindowsProc, (LPARAM)this);

    savedMonitorInfo.list.clear();
    getMonitorList(savedMonitorInfo.list);
    saved = true;

    listWindow.SetRedraw();
}

void CWindowLayoutManagerDlg::addWindow(HWND hwnd)
{
    if (::GetParent(hwnd))
        return;

    if (!::IsWindowVisible(hwnd))
        return;

    WINDOWPLACEMENT wp = { 0 };
    ::GetWindowPlacement(hwnd, &wp);
    if ((wp.rcNormalPosition.right - wp.rcNormalPosition.left) < 50 ||
       ((wp.rcNormalPosition.bottom - wp.rcNormalPosition.top) < 50))
        return;

    TCHAR title[255] = { 0 };
    ::GetWindowText(hwnd, title, sizeof(title) / sizeof(title[0]));
    if (title[0] == '\0')
        return;

    ListItemData *itemListData = new ListItemData;
    itemListData->hwnd = hwnd;
    itemListData->wp = wp;
    ::GetWindowThreadProcessId(hwnd, &itemListData->pid);

    LVITEM itemList = { 0 };
    itemList.mask       = LVIF_TEXT | LVIF_PARAM;
    itemList.iItem      = listWindow.GetItemCount();
    itemList.iSubItem   = 0;
    itemList.pszText    = LPSTR_TEXTCALLBACK;
    itemList.cchTextMax = 255;
    itemList.lParam     = (LPARAM)itemListData;
    listWindow.InsertItem(&itemList);
}

void CWindowLayoutManagerDlg::OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *sDispInfo = reinterpret_cast<NMLVDISPINFO *>(pNMHDR);
    *pResult = 0;

    LVITEM &itemList = sDispInfo->item;
    ListItemData *itemListData = (ListItemData *)itemList.lParam;

    if ((itemList.mask & LVIF_TEXT) == LVIF_TEXT)
    {
        itemList.pszText[0] = '\0';

        switch (itemList.iSubItem)
        {
        case 0:
            ::GetWindowText(itemListData->hwnd, itemList.pszText, itemList.cchTextMax);
            break;

        case 1:
            _stprintf_s(itemList.pszText, itemList.cchTextMax, _T("%llX"), reinterpret_cast<intptr_t>(itemListData->hwnd));
            break;

        case 2:
        {
            _stprintf_s(itemList.pszText, itemList.cchTextMax, _T("%d"), itemListData->pid);
            break;
        }

        case 3:
        {
            ::GetWindowModuleFileName(itemListData->hwnd, itemList.pszText, itemList.cchTextMax);

            HANDLE process = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, itemListData->pid);
            WCHAR imageName[MAX_PATH] = { 0 };
            DWORD bufsize = MAX_PATH;
            ::QueryFullProcessImageName(process, 0, imageName, &bufsize);
            _tcscpy_s(itemList.pszText, itemList.cchTextMax, imageName);
            ::CloseHandle(process);

            TCHAR *split = _tcsrchr(itemList.pszText, _T('\\'));
            if (split)
            {
                size_t charSize = sizeof(itemList.pszText[0]);
                memmove(itemList.pszText, split + 1, _tcslen(split) * charSize);
            }
            break;
        }

        case 4:
        case 5:
        case 6:
            WINDOWPLACEMENT &wp = itemListData->wp;
            RECT &rc = wp.rcNormalPosition;

            switch (itemList.iSubItem)
            {
            case 4: _stprintf_s(itemList.pszText, itemList.cchTextMax, _T("%d, %d"), rc.left, rc.top); break;
            case 5: _stprintf_s(itemList.pszText, itemList.cchTextMax, _T("%d x %d"), rc.right - rc.left, rc.bottom - rc.top); break;
            case 6:
                LPCTSTR showCmdStr = NULL;
                     if (wp.showCmd == SW_SHOWMINIMIZED) showCmdStr = _T("Minimized");
                else if (wp.showCmd == SW_SHOWMAXIMIZED) showCmdStr = _T("Maximized");
                else if (wp.showCmd == SW_HIDE)          showCmdStr = _T("Hided");

                if (showCmdStr)
                    _stprintf_s(itemList.pszText, itemList.cchTextMax, _T("%d (%s)"), wp.showCmd, showCmdStr);
                else
                    _stprintf_s(itemList.pszText, itemList.cchTextMax, _T("%d"), wp.showCmd);
                break;
            }
        }
    }

    itemList.mask |= LVIF_DI_SETITEM;
}

void CWindowLayoutManagerDlg::OnLvnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_LISTVIEW* nmListView = (NM_LISTVIEW*)pNMHDR;
    *pResult = 0;

    ListItemData *itemListData = (ListItemData *)nmListView->lParam;
    delete itemListData;
}

void CWindowLayoutManagerDlg::OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
    *pResult = 0;

    switch (pLVKeyDow->wVKey)
    {
    case VK_F5:
        OnBnClickedScan();
        break;

    case VK_DELETE:
        OnBnClickedDelete();
        break;
    }
}

void CWindowLayoutManagerDlg::OnBnClickedDelete()
{
    int index = -1;
    std::list<int> list;

    auto pos = listWindow.GetFirstSelectedItemPosition();
    while ((index = listWindow.GetNextSelectedItem(pos)) != -1)
    {
        list.push_back(index);
    }

    auto itr = list.rbegin();
    for (; itr != list.rend(); ++itr)
    {
        index = *itr;

        listWindow.DeleteItem(index);
    }

    if (index > listWindow.GetItemCount())
        --index;

    if (index >= 0)
    {
        listWindow.SetFocus();
        listWindow.EnsureVisible(index, TRUE);
        listWindow.SetSelectionMark(index);
        listWindow.SetItemState(index, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
    }
}

void CWindowLayoutManagerDlg::OnBnClickedWho()
{
    int index = listWindow.GetSelectionMark();
    if (index < 0) return;

    ListItemData *itemListData = (ListItemData *)listWindow.GetItemData(index);

    ::SetForegroundWindow(itemListData->hwnd);
    SetForegroundWindow();
    ::FlashWindow(itemListData->hwnd, TRUE);
}

void CWindowLayoutManagerDlg::OnBnClickedOk() {}

void CWindowLayoutManagerDlg::OnBnClickedCancel()
{
    minimizeToTray();
}

void CWindowLayoutManagerDlg::OnBnClickedLock()
{
    lockScan(!locked);
    locked = !locked;
}

void CWindowLayoutManagerDlg::lockScan(bool lock)
{
    if (lock)
    {
        GetDlgItem(IDC_SCAN)->EnableWindow(FALSE);
        GetDlgItem(IDC_LOCK)->SetWindowText(_T("Un&lock"));
    }
    else
    {
        GetDlgItem(IDC_SCAN)->EnableWindow(TRUE);
        GetDlgItem(IDC_LOCK)->SetWindowText(_T("&Lock"));
    }
}

void CWindowLayoutManagerDlg::OnBnClickedRestore()
{
    ListItemData *itemListData;
    int i, count = listWindow.GetItemCount();

    for (i = 0; i < count; ++i)
    {
        itemListData = (ListItemData *)listWindow.GetItemData(i);

        ::SetWindowPlacement(itemListData->hwnd, &itemListData->wp);
    }
}

void CWindowLayoutManagerDlg::OnDisplayChange(UINT nImageDepth, int cxScreen, int cyScreen)
{
    if (saved)
    {
        KillTimer(TIMER_RESTORE);
        SetTimer(TIMER_RESTORE, 1000, NULL);
    }
}

void CWindowLayoutManagerDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TIMER_SCAN)
    {
        timerScan();
        KillTimer(nIDEvent);
        return;
    }
    else if (nIDEvent == TIMER_RESTORE)
    {
        timerRestore();
        KillTimer(nIDEvent);
        return;
    }

    super::OnTimer(nIDEvent);
}

void CWindowLayoutManagerDlg::timerScan()
{
}

bool operator==(const MonitorList& list1, const MonitorList& list2)
{
    auto itr1 = list1.cbegin();
    auto itr2 = list2.cbegin();

    while (itr1 != list1.cend())
    {
        if (_tcsncmp(itr1->szDevice, itr2->szDevice, CCHDEVICENAME) != 0)
        {
            return false;
        }

        if (itr1->rcMonitor.left != itr2->rcMonitor.left ||
            itr1->rcMonitor.top != itr2->rcMonitor.top ||
            itr1->rcMonitor.right != itr2->rcMonitor.right ||
            itr1->rcMonitor.bottom != itr2->rcMonitor.bottom)
        {
            return false;
        }

        ++itr1;
        ++itr2;
    }

    return true;
}

bool operator!=(const MonitorList& list1, const MonitorList& list2)
{
    return !(list1 == list2);
}

void CWindowLayoutManagerDlg::timerRestore()
{
    MonitorList list;
    getMonitorList(list);

    if (list.size() == savedMonitorInfo.list.size())
    {
        bool matched = (list == savedMonitorInfo.list);
        if (matched)
        {
            OnBnClickedRestore();
        }
    }
}

BOOL CWindowLayoutManagerDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F5)
    {
        OnBnClickedScan();
        return TRUE;
    }

    return super::PreTranslateMessage(pMsg);
}
