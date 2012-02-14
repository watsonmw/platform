#include "stdafx.h"

#include "platform/log.h"
#include "platform/openglwindow.h"
#include "platform/eventhandler.h"
#include "platform/settings.h"
#include "platform/parseargs.h"
#include "platform/screensaverapplication.h"

#include "matrixcodesrenderer.h"

#include <string>
#include <sstream>
#include <cmath>

using namespace std;
using namespace Platform;

class MatrixScreenSaver : public Platform::ScreenSaverApplication
{
public:
    MatrixScreenSaver(const wstring &cmdLine)
      : Platform::ScreenSaverApplication(L"Matrix Screensaver", cmdLine) {}

private:
    bool showConfigureDialog(HWND parent) {
        MessageBox(parent, L"Matrix Screensaver\n\n"
           L"Developed and maintained by watsonmw@gmail.com\n\n"
           L"Some Rights Reserved.\n\n"
           L"http://creativecommons.org/licenses/by-nc-sa/3.0/\n\n",
           L"About Matrix Screensaver", 0);
        return false;
    }

    bool startScreenSaver(HWND parentWindow) {
        int launchTime = GetTickCount();
        bool windowOpened = false;

        Settings& settings = Settings::getInstance();
        OpenGLWindow window(L"Matrix Revisted",
                            *this,
                            settings.getInt(L"window.color"));

        if (parentWindow == NULL) {
            windowOpened = window.openFullScreen(settings.getInt(L"window.width"),
                                                 settings.getInt(L"window.height"));
        }
        else {
            windowOpened = window.open(parentWindow);
        }

        if (!windowOpened) {
            Log::print(L"Failed to create main window. Exiting...");
            return false;
        }

        int width = window.getWidth();
        int height = window.getHeight();

        // Bump up the opengl viewport size if in preview mode.
        // We scale this down before rendering to show a scale version
        // of the real thing...
        if (parentWindow != NULL) {
            width *= 4;
            height *= 4;
        }

        MatrixCodesRenderer matrixRenderer(width, height);

        if (!matrixRenderer.loadMatrixGlyphsFromImage(settings.getPath(L"matrixCodes.fontfile"))) {
            Log::print(L"Unable to load font file, exiting...");
            return false;
        }

        Log::printf(L"Total setup time %dms", GetTickCount() - launchTime);

        int sleepTime = 0;
        // Process window, mouse, and keyboard events
        while (processEvents(sleepTime)) {
            int startTime = GetTickCount();

            // If the rendering context has changed we have
            // to reload all the display lists
            if (window.isNewRenderingContext()) {
                Log::print(L"Generating Texture and Display List Caches...");
                matrixRenderer.reloadRenderCaches();
            }

            matrixRenderer.render();

            // Render to back buffer and display
            window.swapBuffers();

            // Calculate the time to wait between frames.
            const int msecondsBetweenFrames = 35;
            int timeTaken = GetTickCount() - startTime;
            sleepTime = msecondsBetweenFrames - timeTaken;
            if (sleepTime < 0) {
                sleepTime = 0;
            }
        }
        return true;
    }
};

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, _TCHAR* cmdLine, int nCmdShow)
{
    Log::print(L"Matrix - Revisited by watsonmw@gmail.com");
    MatrixScreenSaver screenSaver(cmdLine);
    return screenSaver.run() ? 0 : -1;
}
