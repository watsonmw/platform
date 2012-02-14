#include "stdafx.h"

#include "platform/log.h"
#include "platform/eventhandler.h"
#include "platform/parseargs.h"
#include "platform/application.h"

#include <string>
#include <vector>
#include <sstream>
#include <cmath>

using namespace std;

namespace Platform
{
    Application::Application(const wstring &name, const wstring &cmdLine)
      : _cmdLine(cmdLine),
        _name(name),
        _registrySettings(name)
    {
    }

    bool Application::run()
    {
        return true;
    }

    Application::~Application()
    {
    }

    bool Application::processPendingEvents()
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message != WM_QUIT) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else {
                return false;
            }
        }

        return true;
    }

    bool Application::processEvents(int sleepTimeMilliseconds)
    {
        if (sleepTimeMilliseconds <= 0) {
            return processPendingEvents();
        }

        MSG msg;
        BOOL bRet;
        UINT_PTR timerId = SetTimer(0, 0, sleepTimeMilliseconds, 0);
        while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
            if (bRet == -1) {
                // Error
                KillTimer(0, timerId);
                return false;
            }
            else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            if (msg.message == WM_TIMER &&
                msg.wParam == timerId) {
                KillTimer(0, timerId);
                processPendingEvents();
                return true;
            }
        }
        KillTimer(0, timerId);
        return false;
    }

    bool Application::processEvents()
    {
        MSG msg;
        BOOL bRet;
        while ((bRet = GetMessage(&msg, NULL, 0, 0 )) != 0) {
            if (bRet == -1) {
                // Error
                return false;
            }
            else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        return true;
    }
};
