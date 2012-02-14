#ifndef _PLATFORM_SCREENSAVERAPPLICATION_H_INCLUDED_
#define _PLATFORM_SCREENSAVERAPPLICATION_H_INCLUDED_

#include <windows.h>
#include <string>
#include <vector>
#include "platform/registrysettings.h"
#include "platform/application.h"

namespace Platform
{
    class ScreenSaverApplication : public Platform::Application
    {
    public:
        ScreenSaverApplication(const std::wstring &name,
                               const std::wstring &cmdLine);
        virtual ~ScreenSaverApplication();

        /**
         * Based on the command line flags either show the configure dialog
         * or start the screen saver.
         */
        bool run();

    protected:
        /**
         * This function should show a configure dialog and return when
         * the setting have been changed.
         */
        virtual bool showConfigureDialog(HWND parent) = 0;

        /**
         * Start the screen saver if parent_window is NULL
         * Then you should start the screen saver in fullscreen mode
         */
        virtual bool startScreenSaver(HWND parentWindow) = 0;

        /**
        * Returns an accessor to our application registry settings
        */
        RegistrySettings& getRegistrySettings() {
            return _registrySettings;
        }

        /**
         * Returns a list of monitors attached to the system.
         * Win98 was the first version of windows to support multiple monitors.
         */
        std::vector<HMONITOR> getMonitors();

    private:
        void quitIfInFullscreenMode();

        void moveEvent(Window *window, const std::string &event, int absX, int absY, int dx, int dy);

        void buttonEvent(Window *window, __int64 virtualKeyCode, bool pressed) {
            quitIfInFullscreenMode();
        }

        void buttonEvent(Window *window, const std::string &event, bool pressed) {
            quitIfInFullscreenMode();
        }

        bool screenSaver() {
            quitIfInFullscreenMode();
            return true;
        }

        bool monitorPower() {
            quitIfInFullscreenMode();
            return true;
        }

        HWND convertToWindowHandle(const std::wstring &window);

        bool _bFullscreenMode;
    };
};

#endif