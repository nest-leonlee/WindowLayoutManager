#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"

class CWindowLayoutManagerApp : public CWinApp
{
public:
    CWindowLayoutManagerApp();

public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

protected:
    DECLARE_MESSAGE_MAP()
};

extern CWindowLayoutManagerApp theApp;
