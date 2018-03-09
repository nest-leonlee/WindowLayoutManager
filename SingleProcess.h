#pragma once

class SingleProcess
{
public:
    static bool check();
    static void lock();
    static void unlock();
    static unsigned int getMsg();
    static void postMsg();

protected:
    static HANDLE singleProcessMutex;
};
