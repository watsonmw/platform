#include "stdafx.h"

#include "platform/log.h"
#include "platform/eventhandler.h"
#include "platform/parseargs.h"
#include "platform/screensaverapplication.h"

#include <string>
#include <vector>
#include <sstream>
#include <cmath>

using namespace std;

static BOOL CALLBACK EnumDisplayMonitorsCallback
(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    vector<HMONITOR> *monitors = (vector<HMONITOR> *)dwData;
    monitors->push_back(hMonitor);
    return TRUE;
}

namespace Platform
{
    HWND ScreenSaverApplication::convertToWindowHandle(const std::wstring &window)
    {
        __int64 windowHandle = NULL;
        std::wistringstream inputWindowHandleStream(window);
        inputWindowHandleStream >> windowHandle;
        return (HWND)windowHandle;
    }

    ScreenSaverApplication::ScreenSaverApplication(const wstring &name,
            const wstring &cmdLine)
      : Application(name, cmdLine),
        _bFullscreenMode(false)
    {
    }

    bool ScreenSaverApplication::run()
    {
        Log::printf(L"Parsing command line \'%s\'...", _cmdLine.c_str());

        ParseArgs parseArgs(_cmdLine);
        size_t options = parseArgs.getNumberOfOptions();

        if (options) {
            std::wstring option = parseArgs.getOption(0);
            if (option == L"S" ||
                    option == L"s") {
                Log::print(L"Running as fullscreen screensaver");
                _bFullscreenMode = true;
                return startScreenSaver(NULL);
            }
            else if (option == L"a") {
                Log::print(L"Password change mode not supported.");
                return false;
            }
            else if (option == L"c") {
                std::wstring window = parseArgs.getOptionParam(0);
                HWND parentWindow = convertToWindowHandle(window);

                Log::print(L"Running in configure mode");
                return showConfigureDialog(parentWindow);
            }
            else if (option == L"p") {
                Log::print(L"Running in preview mode");
                HWND parentWindow = NULL;

                if (parseArgs.getNumberOfArgs() > 1) {
                    std::wstring window = parseArgs.getArg(1);
                    parentWindow = convertToWindowHandle(window);
                }

                if (parentWindow == NULL) {
                    Log::print(L"Preview mode failed");
                    return false;
                }

                return startScreenSaver(parentWindow);
            }
        }
        else {
            return showConfigureDialog(NULL);
        }
        return false;
    }

    ScreenSaverApplication::~ScreenSaverApplication()
    {
    }

    void ScreenSaverApplication::quitIfInFullscreenMode()
    {
        if (_bFullscreenMode) {
            PostQuitMessage(0);
        }
    }

    void ScreenSaverApplication::moveEvent(Window*, const std::string &event, int absX, int absY, int dx, int dy)
    {
        double distance = sqrt((double)(dx * dx) + (dy * dy));

        if (distance > 1.5) {
            quitIfInFullscreenMode();
        }
    }

    vector<HMONITOR> ScreenSaverApplication::getMonitors()
    {
        vector<HMONITOR> monitors;
        EnumDisplayMonitors(NULL, NULL, EnumDisplayMonitorsCallback, (LPARAM)&monitors);
        return monitors;
    }
};