//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "SizeGripWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SizeGripWnd::SizeGripWnd()
{
}

SizeGripWnd::~SizeGripWnd()
{
}

BEGIN_MESSAGE_MAP(SizeGripWnd, CWnd)
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL SizeGripWnd::Create(CWnd *parentWnd, CRect rc, UINT id)
{
    DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
    style |= SBS_SIZEBOX;
    style |= SBS_SIZEGRIP;
    style |= SBS_SIZEBOXBOTTOMRIGHTALIGN;

    return CWnd::Create(_T("SCROLLBAR"), nullptr, style, rc, parentWnd, id, nullptr);
}

BOOL SizeGripWnd::OnSetCursor(CWnd *wnd, UINT hitTest, UINT message)
{
    HCURSOR cursor = AfxGetApp()->LoadStandardCursor((const TCHAR *)IDC_SIZENWSE);
    SetCursor(cursor);
    return TRUE;
}
