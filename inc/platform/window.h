#ifndef _PLATFORM_WINDOW_INCLUDED_H_
#define _PLATFORM_WINDOW_INCLUDED_H_

#include <windows.h>
#include <string>

#include "platform/eventhandler.h"

/**
 * A window class suitable for GDI or OpenGL drawing
 */
namespace Platform
{
    class Window
    {
    public:
        Window(const std::wstring &name,
               Platform::EventHandler &eventHandler,
               bool doubleBuffered = false);
        virtual ~Window();

        enum Defaults {
            DefaultX = -1,
            DefaultY = -1,
            DefaultWidth = -1,
            DefaultHeight = -1
        };

        /**
        * Open Window inside given window
        */
        bool open(HWND window);

        /**
        * Open Window of the given size
        */
        bool open(int width, int height);

        /**
        * Open Window covering the primary screen
        */
        bool openFullScreen();

        /**
        * Open Window covering the given monitor
        */
        bool openFullScreen(HMONITOR monitor);

        /**
        * Destroy window
        */
        virtual void close();

        int getWidth() { return _width; }
        int getHeight() { return _height; }

        /**
         * Clears the windows background to black
         */
        void clear(HDC hDC = 0);

        /**
         * Call these function to notify Windows that we
         * are drawing to the window.
         */
        HDC beginPaint(PAINTSTRUCT *paintInfo);
        void endPaint(PAINTSTRUCT *paintInfo);

        /**
         * Get a new timerId
         */
        int getNewTimerId();

        /**
         * Adds a new timer.  EventHandler::timer() function will be called
         * in 'milliseconds' milliseconds.  Over and Over.
         *
         * You call this again with same timerId to restart the timeout
         * and/or change the time out.
         */
        void startTimer(int timerId, int milliseconds);

        /**
         * Call this to stop a timer started above
         */
        void stopTimer(int timerId);

        /**
         * Adds a paint event to the event queue
         */
        void sendPaintEvent();

    protected:
        enum WindowType {
            CHILD_WindowType,
            TOPLEVEL_WindowType,
            FULLSCREEN_WindowType
        };

        virtual bool createWindow(HWND parent,
                                  int xPos, int yPos,
                                  int width, int height,
                                  WindowType windowType);

        bool registerWindow(const std::wstring &windowClassName);

        bool dispatchEvent(UINT message, WPARAM wParam, LPARAM lParam);
        static LRESULT APIENTRY WndProc(HWND hWnd,
                                        UINT message,
                                        WPARAM wParam,
                                        LPARAM lParam);

        std::wstring getKeyName(WPARAM virtualKeyCode);

        /**
         * try to create a double buffer
         */
        bool setupDoubleBuffer(HDC hDc);

        /**
         * Remove the double buffer and free up memory
         */
        void removeDoubleBuffer();

        bool _bWindowOpen;
        int _height, _width;

        // Double buffering stuff
        bool _bDoubleBuffered;
        HDC _doubleBufferBackDc;
        HBITMAP _doubleBufferBackBmp;
        HBITMAP _doubleBufferOldBmp;
        RECT _doubleBufferWindowRect;

        std::wstring _name;
        Platform::EventHandler &_eventHandler;

        HWND _hWnd;
        HWND _parent;
        HINSTANCE _hInstance;
        HCURSOR _hcursor;

        bool _bFirstMouseEvent;
        int _mousex, _mousey;
        int _uniqueWindowTimerId;
    };
};

#endif