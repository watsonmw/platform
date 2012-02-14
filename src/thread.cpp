#include "stdafx.h"

#include "platform/thread.h"
#include "platform/log.h"


Platform::Thread::Thread()
  : _handle(NULL)
{
}

Platform::Thread::~Thread()
{
    // This won't stop the thread but it will remove it from
    // the process if the run() method has completed.
    if (_handle != NULL) {
        CloseHandle(_handle);
    }
}

DWORD WINAPI Platform::Thread::runThreadInMemberFunction(LPVOID lpParam)
{
    Platform::Thread *thread = (Platform::Thread *)lpParam;
    thread->run();
    return 0;
}

void Platform::Thread::start()
{
    if (_handle != NULL) {
        Log::print(L"Thread::start() thread already started.");
        return;
    }

    _handle = CreateThread(0, // Default security params
                           0, // Default stack size
                           runThreadInMemberFunction,
                           this,
                           0, // Run now
                           &_threadId);

    if (_handle == NULL) {
        Log::logWin32Error();
    }
}


bool Platform::Thread::isThreadRunning()
{
    if (_handle == NULL) {
        return false;
    }

    // Call the wait function with a time out of zero
    // so it returns immediately.
    DWORD status = WaitForSingleObject(_handle, 0);
    if (status == WAIT_TIMEOUT) {
        return true;
    }

    return false;
}


void Platform::Thread::kill()
{
    if (_handle != NULL) {
        if (isThreadRunning()) {
            TerminateThread(_handle, -1);
        }
        CloseHandle(_handle);
        _handle = NULL;
    }
}