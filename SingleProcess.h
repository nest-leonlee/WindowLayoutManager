#ifndef __SINGLE_PROCESS_H__
#define __SINGLE_PROCESS_H__ 1
#pragma once

class SingleProcess
{
public:
    static bool check(void);
    static void lock(void);
    static void unlock(void);
    static unsigned int getMsg(void);
    static void postMsg(void);

protected:
    static HANDLE singleProcessMutex;
};

#endif // __SINGLE_PROCESS_H__
