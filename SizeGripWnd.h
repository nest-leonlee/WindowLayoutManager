#ifndef __SIZE_GRIP_WND_H__
#define __SIZE_GRIP_WND_H__
#pragma once

class SizeGripWnd : public CWnd
{
public:
    SizeGripWnd();
    virtual ~SizeGripWnd();

public:
    virtual BOOL Create(CWnd *parentWnd, CRect rc, UINT id);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg BOOL OnSetCursor(CWnd *wnd, UINT hitTest, UINT message);
};

#endif // __SIZE_GRIP_WND_H__
